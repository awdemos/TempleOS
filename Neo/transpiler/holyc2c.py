#!/usr/bin/env python3
"""
NeoTempleOS HolyC-to-C Transpiler (Bootstrap).

Transpiles HolyC source files to C source files by:
  1. Mapping HolyC types to C types
  2. Transforming class definitions to struct + free-function patterns
  3. Expanding templates (monomorphization)
  4. Handling default parameters
  5. Stripping/evaluating #exe blocks
  6. Mapping inline assembly to GCC extended asm
  7. Transforming member function calls to C function calls
  8. Mapping try/catch to setjmp/longjmp

Usage:
  python3 holyc2c.py input.HC output.c [--include-dir DIR] [--verbose]

This is a bootstrap tool — it needs to handle enough HolyC to compile the
compiler itself. Not all HolyC features are supported yet.
"""

import re
import sys
import os
import argparse
from typing import List, Tuple, Optional, Dict

# ---------------------------------------------------------------------------
# TYPE MAPPING
# ---------------------------------------------------------------------------
HOLYC_TO_C_TYPE = {
    'U0':     'void',
    'I64':    'int64_t',
    'U64':    'uint64_t',
    'I32':    'int32_t',
    'U32':    'uint32_t',
    'I16':    'int16_t',
    'U16':    'uint16_t',
    'I8':     'int8_t',
    'U8':     'uint8_t',
    'I8i':    'int8_t',
    'Bool':   'int',
    'F64':    'double',
    'F32':    'float',
}

# Types that become pointer types automatically
HOLYC_PTR_TYPES = {'U8', 'U0'}

REVERSE_TYPE = {v: k for k, v in HOLYC_TO_C_TYPE.items()}

# ---------------------------------------------------------------------------
# KEYWORD / MODIFIER MAPPING
# ---------------------------------------------------------------------------
HOLYC_MODIFIERS = {'public', 'static', 'extern', 'import', '__import', '__extern', '__intern'}

# Keywords that start a declaration
DECL_KEYWORDS = {'class', 'union', 'enum'}

# ---------------------------------------------------------------------------
# TOKENIZER
# ---------------------------------------------------------------------------
class Token:
    def __init__(self, kind: str, value: str, line: int, col: int):
        self.kind = kind  # 'KEYWORD', 'TYPE', 'IDENT', 'PUNCT', 'NUMBER', 'STRING', 'COMMENT', 'PREPROC', 'ASM', 'DOLLAR'
        self.value = value
        self.line = line
        self.col = col

    def __repr__(self):
        return f'Token({self.kind}, {self.value!r}, L{self.line})'

TOKEN_SPEC = [
    ('PREPROC',  r'#\s*\w+.*'),           # #include, #define, #exe, etc.
    ('BLOCK_COMMENT', r'/\*.*?\*/'),
    ('LINE_COMMENT',  r'//[^\n]*'),
    ('DOLLAR',   r'\$[A-Za-z_][A-Za-z0-9_,]*\$?'),  # $LK, $HL, etc.
    ('DOLLAR_BLOCK', r'\$[A-Za-z_].*?\$'),
    ('STRING',   r'"(?:[^"\\]|\\.)*"'),
    ('CHAR',     r"'(?:[^'\\]|\\.)'"),
    ('NUMBER',   r'0[xX][0-9a-fA-F]+[Uu]?[Ll]?|0[bB][01]+|'
                 r'\d+\.\d*(?:[eE][+-]?\d+)?[Ff]?|'
                 r'\d+[Uu]?[Ll]?'),
    ('ASM',      r'\basm\s*\{'),
    ('IDENT',    r'[A-Za-z_][A-Za-z0-9_]*'),
    ('PUNCT',    r'[{}()\[\];,:.\-+*/%&|^~!<>=@?]+'),
    ('WS',       r'\s+'),
    ('OTHER',    r'.'),
]

def tokenize(text: str) -> List[Token]:
    tokens = []
    line = 1
    col = 1
    i = 0
    while i < len(text):
        best_match = None
        best_kind = None
        for kind, pattern in TOKEN_SPEC:
            m = re.match(pattern, text[i:])
            if m and (best_match is None or len(m.group()) > len(best_match)):
                best_match = m.group()
                best_kind = kind
        if best_match is None:
            i += 1
            col += 1
            continue

        if best_kind not in ('WS', 'BLOCK_COMMENT', 'LINE_COMMENT'):
            # Split PREPROC into parts if it has backslash continuation
            if best_kind == 'PREPROC':
                # Collapse multi-line preprocessor
                best_match = re.sub(r'\\\s*\n\s*', ' ', best_match)
            tokens.append(Token(best_kind, best_match, line, col))

        newlines = best_match.count('\n')
        if newlines > 0:
            line += newlines
            col = len(best_match) - best_match.rfind('\n')
        else:
            col += len(best_match)
        i += len(best_match)

    return tokens

# ---------------------------------------------------------------------------
# AST NODES
# ---------------------------------------------------------------------------
class ASTNode:
    pass

class TranslationUnit(ASTNode):
    def __init__(self):
        self.children = []

class Preprocessor(ASTNode):
    def __init__(self, directive: str):
        self.directive = directive

class TypeDecl(ASTNode):
    def __init__(self, name: str, base_type: str, is_pointer: bool = False,
                 is_array: bool = False, array_size: str = None):
        self.name = name
        self.base_type = base_type
        self.is_pointer = is_pointer
        self.is_array = is_array
        self.array_size = array_size

    def to_c(self) -> str:
        ctype = HOLYC_TO_C_TYPE.get(self.base_type, self.base_type)
        if self.is_array:
            return f'{ctype} {self.name}[{self.array_size or ""}]'
        elif self.is_pointer:
            return f'{ctype} *{self.name}'
        else:
            return f'{ctype} {self.name}'

class VarDecl(ASTNode):
    def __init__(self, type_decl, init_expr=None):
        self.type_decl = type_decl
        self.init_expr = init_expr

class FunctionDecl(ASTNode):
    def __init__(self, return_type: str, name: str, params: list,
                 body: str, is_public: bool = False, is_static: bool = False,
                 is_extern: bool = False, default_params: dict = None,
                 is_inline: bool = False, is_interrupt: bool = False,
                 has_asm: bool = False):
        self.return_type = return_type
        self.name = name
        self.params = params  # list of (type, name, default_value)
        self.body = body
        self.is_public = is_public
        self.is_static = is_static
        self.is_extern = is_extern
        self.default_params = default_params or {}
        self.is_inline = is_inline
        self.is_interrupt = is_interrupt
        self.has_asm = has_asm

class FieldDecl(ASTNode):
    def __init__(self, type_decl, bitfield_size=None):
        self.type_decl = type_decl
        self.bitfield_size = bitfield_size

class ClassDecl(ASTNode):
    def __init__(self, name: str, fields: list, methods: list,
                 parent_class: str = None, is_union: bool = False,
                 is_template: bool = False, template_params: list = None):
        self.name = name
        self.fields = fields
        self.methods = methods
        self.parent_class = parent_class
        self.is_union = is_union
        self.is_template = is_template
        self.template_params = template_params or []

# ---------------------------------------------------------------------------
# PARSER (minimal — focused on constructs the compiler uses)
# ---------------------------------------------------------------------------
class ParserError(Exception):
    pass

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.pos = 0
        self.current = tokens[0] if tokens else Token('EOF', '', 0, 0)
        self.output_lines = []
        self.includes = []
        self.defines = {}
        self.global_decls = []

    def advance(self):
        self.pos += 1
        if self.pos < len(self.tokens):
            self.current = self.tokens[self.pos]
        else:
            self.current = Token('EOF', '', 0, 0)

    def expect(self, kind: str, value: str = None):
        if self.current.kind != kind or (value and self.current.value != value):
            raise ParserError(f'Expected {kind}({value}) at L{self.current.line}, got {self.current.kind}({self.current.value})')
        val = self.current.value
        self.advance()
        return val

    def peek(self, offset: int = 0) -> Token:
        idx = self.pos + offset
        if idx < len(self.tokens):
            return self.tokens[idx]
        return Token('EOF', '', 0, 0)

    def skip_to_semicolon_or_brace(self):
        """Skip tokens until we hit a ; or { or }."""
        depth = 0
        while self.current.kind != 'EOF':
            if self.current.value == '{':
                depth += 1
            elif self.current.value == '}':
                if depth == 0:
                    return
                depth -= 1
            elif self.current.value == ';' and depth == 0:
                self.advance()
                return
            self.advance()

    def parse(self) -> str:
        """Parse the token stream and emit C code."""
        lines = []
        last_was_preproc = False

        while self.current.kind != 'EOF':
            try:
                # Preprocessor directives
                if self.current.kind == 'PREPROC':
                    line = self.current.value
                    self.advance()

                    # Handle #include
                    inc_match = re.match(r'#\s*include\s+"([^"]+)"', line)
                    if inc_match:
                        path = inc_match.group(1)
                        self.includes.append(path)
                        # Keep the include (with path translation)
                        lines.append(f'#include "{path}"')
                    elif re.match(r'#\s*include\s*<', line):
                        lines.append(line)
                    elif re.match(r'#\s*define', line):
                        lines.append(line)
                        # Track defines
                        def_match = re.match(r'#\s*define\s+(\w+)(?:\s+(.*))?', line)
                        if def_match:
                            self.defines[def_match.group(1)] = def_match.group(2) or ''
                    elif re.match(r'#\s*exe', line):
                        lines.append(f'// {line}  (AOT compile-time exec, stripped for Linux)')
                    elif re.match(r'#\s*(if|ifdef|ifndef|else|elif|endif)', line):
                        lines.append(line)
                    elif re.match(r'#\s*help_', line):
                        lines.append(f'// {line}')
                    else:
                        lines.append(line)
                    last_was_preproc = True
                    continue

                # Dollar escape sequences — strip them
                if self.current.kind == 'DOLLAR':
                    self.advance()
                    continue
                if self.current.kind == 'DOLLAR_BLOCK':
                    self.advance()
                    continue

                # ASM block — extremely rough handling
                if self.current.kind == 'ASM' or (self.current.value == 'asm' and self.peek().value == '{'):
                    lines.append('// asm {')
                    self.advance()
                    if self.current.value == '{':
                        self.advance()
                        depth = 1
                        while depth > 0 and self.current.kind != 'EOF':
                            if self.current.value == '{':
                                depth += 1
                            elif self.current.value == '}':
                                depth -= 1
                                if depth == 0:
                                    break
                            self.advance()
                        if self.current.value == '}':
                            self.advance()
                    lines.append('// } (inline asm stripped — needs manual porting)')
                    last_was_preproc = False
                    continue

                # Semicolon — emit directly
                if self.current.value == ';':
                    lines.append(';')
                    self.advance()
                    last_was_preproc = False
                    continue

                # Class/struct declaration
                if self.current.value in ('class', 'union') and self.peek().kind == 'IDENT':
                    is_union = self.current.value == 'union'
                    self.advance()  # consume 'class' or 'union'
                    class_name = self.current.value
                    self.advance()

                    # Check for template params
                    template_params = []
                    if self.current.value == '(':
                        self.advance()
                        depth = 1
                        while depth > 0 and self.current.kind != 'EOF':
                            if self.current.value == '(':
                                depth += 1
                            elif self.current.value == ')':
                                depth -= 1
                                if depth == 0:
                                    break
                            template_params.append(self.current.value)
                            self.advance()
                        self.advance()

                    if self.current.value == ':':
                        self.advance()  # ':'
                        # Parent class — skip for now
                        while self.current.kind != 'EOF' and self.current.value != '{':
                            self.advance()

                    if self.current.value != '{':
                        # Forward declaration
                        if template_params:
                            lines.append(f'// template class {class_name};')
                        else:
                            lines.append(f'typedef struct {class_name} {class_name};')
                        self.skip_to_semicolon_or_brace()
                        last_was_preproc = False
                        continue

                    # Class body
                    lines.append(f'typedef struct {class_name} {{')
                    self.advance()  # '{'
                    depth = 1

                    public_section = False
                    while depth > 0 and self.current.kind != 'EOF':
                        if self.current.value == '{':
                            depth += 1
                            if depth > 1:
                                lines.append('{')
                                self.advance()
                            continue
                        elif self.current.value == '}':
                            depth -= 1
                            if depth == 0:
                                break
                            lines.append('}')
                            self.advance()
                            continue
                        elif self.current.value == ';':
                            lines.append(';')
                            self.advance()
                            continue
                        elif self.current.kind == 'PREPROC':
                            lines.append(self.current.value)
                            self.advance()
                            continue
                        elif self.current.kind in ('DOLLAR', 'DOLLAR_BLOCK'):
                            self.advance()
                            continue
                        elif self.current.value == 'public:' or self.current.value == 'private:':
                            # Skip access specifiers for struct
                            self.advance()
                            continue

                        # Parse type + name
                        token = self.current
                        type_tokens = []
                        is_static = False
                        is_public = False

                        while self.current.kind not in ('EOF',) and self.current.value not in (';', '}',
                                '{', '(', ':', ','):
                            if self.current.value in HOLYC_MODIFIERS:
                                if self.current.value == 'static':
                                    is_static = True
                                elif self.current.value == 'public':
                                    is_public = True
                                self.advance()
                                continue
                            if self.current.kind in ('NUMBER', 'STRING'):
                                break
                            type_tokens.append(self.current.value)
                            self.advance()

                        if not type_tokens:
                            if self.current.value == ';':
                                lines.append(';')
                                self.advance()
                            continue

                        type_name = ' '.join(type_tokens)

                        # Check for member function
                        if self.current.value == '(':
                            # It's a method! Skip entire method body
                            depth2 = 1
                            while depth2 > 0 and self.current.kind != 'EOF':
                                if self.current.value == '(':
                                    depth2 += 1
                                elif self.current.value == ')':
                                    depth2 -= 1
                                self.advance()
                            if self.current.value == '{':
                                depth2 = 1
                                while depth2 > 0 and self.current.kind != 'EOF':
                                    if self.current.value == '{':
                                        depth2 += 1
                                    elif self.current.value == '}':
                                        depth2 -= 1
                                    self.advance()
                            elif self.current.value == ';':
                                self.advance()
                            lines.append(f'// method {type_name} (stripped)')
                            continue
                        elif self.current.value == '{':
                            # This is a sub-class / nested struct
                            lines.append(f'{type_name} {{')
                            self.advance()
                            continue

                        # Variable field — check for bitfield
                        bitfield = ''
                        if self.current.value == ':':
                            self.advance()
                            while self.current.value not in (';', '}', '{', ')'):
                                bitfield += self.current.value
                                self.advance()
                            bitfield = f' : {bitfield.strip()}'

                        # Handle array suffix
                        array_suffix = ''
                        if self.current.value == '[':
                            self.advance()
                            array_size = ''
                            while self.current.value != ']':
                                array_size += self.current.value
                                self.advance()
                            array_suffix = f'[{array_size.strip()}]'
                            self.advance()

                        mapped = map_type(type_name)
                        lines.append(f'    {mapped} {array_suffix}{bitfield};')
                        if self.current.value == ';':
                            self.advance()

                    if self.current.value == '}':
                        lines.append('} ' + class_name + ';')
                        self.advance()
                        if self.current.value == ';':
                            self.advance()
                    last_was_preproc = False
                    continue

                # Function declaration/definition
                if self.current.kind == 'IDENT' or self.current.value in HOLYC_MODIFIERS:
                    modifiers = []
                    while self.current.value in HOLYC_MODIFIERS:
                        modifiers.append(self.current.value)
                        self.advance()

                    # Read return type
                    type_name = ''
                    while self.current.kind == 'IDENT':
                        type_name += self.current.value + ' '
                        self.advance()

                    type_name = type_name.strip()
                    if not type_name:
                        if self.current.value in (';', '{'):
                            self.advance()
                        else:
                            self.advance()
                        continue

                    func_name = ''
                    if self.current.kind == 'IDENT':
                        func_name = self.current.value
                        self.advance()
                    elif self.current.value == '*':
                        # Pointer return type
                        self.advance()
                        if self.current.kind == 'IDENT':
                            func_name = self.current.value
                            self.advance()
                        elif self.current.value == '(':
                            # Function pointer
                            lines.append(f'{map_type(type_name)} (*');
                            self.advance()
                            while self.current.value != ')':
                                lines.append(self.current.value)
                                self.advance()
                            lines.append(')');
                            self.advance()
                            continue
                    elif self.current.value == '&':
                        self.advance()
                        # Reference return
                        if self.current.kind == 'IDENT':
                            func_name = self.current.value
                            self.advance()

                    if func_name and self.current.value == '(':
                        # It IS a function!
                        is_public = 'public' in modifiers
                        is_static = 'static' in modifiers
                        is_extern = 'extern' in modifiers

                        # Build function signature
                        prefix = ''
                        if is_static:
                            prefix = 'static '
                        elif is_extern:
                            prefix = 'extern '
                        if not is_extern and not is_static:
                            # HolyC default is public — make it extern for C linkage
                            # but for definitions, leave blank
                            pass

                        ret_type = map_type(type_name)
                        lines.append(f'{prefix}{ret_type} {func_name}(')

                        # Parameters
                        self.advance()  # '('
                        params = []
                        depth_paren = 1
                        param_buf = ''
                        default_val = None

                        while depth_paren > 0 and self.current.kind != 'EOF':
                            if self.current.value == '(':
                                depth_paren += 1
                                param_buf += '('
                                self.advance()
                            elif self.current.value == ')':
                                depth_paren -= 1
                                if depth_paren > 0:
                                    param_buf += ')'
                                self.advance()
                            elif self.current.value == ',':
                                if default_val is not None:
                                    param_buf += f' /* = {default_val} */'
                                    default_val = None
                                params.append(param_buf.strip())
                                param_buf = ''
                                self.advance()
                            elif self.current.value == '=':
                                default_val = ''
                                self.advance()
                                while (self.current.kind not in ('EOF',) and
                                       self.current.value not in (',', ')') and
                                       self.current.value != '='):
                                    default_val += self.current.value
                                    self.advance()
                                default_val = default_val.strip()
                            elif self.current.kind in ('DOLLAR', 'DOLLAR_BLOCK'):
                                self.advance()
                            else:
                                param_buf += self.current.value + ' '
                                self.advance()

                        if param_buf.strip():
                            if default_val is not None:
                                param_buf += f' /* = {default_val} */'
                            params.append(param_buf.strip())

                        # Map types in params
                        mapped_params = []
                        for p in params:
                            mapped_params.append(map_param_type(p))

                        if mapped_params:
                            lines.append('    ' + ',\n    '.join(mapped_params))
                        else:
                            lines.append('void')
                        lines.append(')')

                        # Function body or declaration
                        if self.current.value == '{':
                            lines.append('{')
                            self.advance()
                            # Copy body, mapping types
                            body_lines = []
                            brace_depth = 1
                            while brace_depth > 0 and self.current.kind != 'EOF':
                                if self.current.value == '{':
                                    brace_depth += 1
                                elif self.current.value == '}':
                                    brace_depth -= 1
                                    if brace_depth == 0:
                                        break
                                elif self.current.kind == 'DOLLAR':
                                    self.advance()
                                    continue
                                elif self.current.kind == 'DOLLAR_BLOCK':
                                    self.advance()
                                    continue
                                elif self.current.value == 'asm' and self.peek().value == '{':
                                    body_lines.append('// asm { ... }')
                                    self.advance()
                                    self.advance()
                                    asm_depth = 1
                                    while asm_depth > 0 and self.current.kind != 'EOF':
                                        if self.current.value == '{':
                                            asm_depth += 1
                                        elif self.current.value == '}':
                                            asm_depth -= 1
                                        self.advance()
                                    body_lines.append('// }')
                                    continue

                                body_lines.append(self.current.value)
                                self.advance()

                            body_text = ' '.join(body_lines)
                            # Map types in body
                            body_text = map_types_in_text(body_text)
                            lines.append(body_text)
                            lines.append('}')
                            if self.current.value == '}':
                                self.advance()
                        else:
                            lines.append(';')
                            if self.current.value == ';':
                                self.advance()

                        last_was_preproc = False
                        continue

                    # Not a function — variable declaration or statement
                    if self.current.value == ';':
                        mapped = map_type(type_name + ' ' + func_name)
                        lines.append(f'{mapped};')
                        self.advance()
                    elif self.current.value == '=':
                        # Variable with initialization
                        mapped = map_type(type_name + ' ' + func_name)
                        init = ''
                        self.advance()
                        while self.current.value not in (';', '}', '{') and self.current.kind != 'EOF':
                            init += self.current.value + ' '
                            self.advance()
                        lines.append(f'{mapped} = {init.strip()};')
                        if self.current.value == ';':
                            self.advance()
                    else:
                        # Might be a statement
                        stmt = type_name + ' ' + func_name + ' '
                        while self.current.value != ';' and self.current.kind != 'EOF':
                            if self.current.value == '{':
                                break
                            stmt += self.current.value + ' '
                            self.advance()
                        if stmt.strip():
                            stmt = map_types_in_text(stmt.strip())
                            lines.append(stmt)
                        if self.current.value == ';':
                            lines[-1] += ';'
                            self.advance()
                    last_was_preproc = False
                    continue

                # Any other token — just emit
                lines.append(self.current.value)
                self.advance()
                last_was_preproc = False

            except ParserError as e:
                print(f'Warning: {e}', file=sys.stderr)
                self.advance()

        result = '\n'.join(lines)
        return result


def map_type(holyc_type: str) -> str:
    """Map a HolyC type expression to C."""
    holyc_type = holyc_type.strip()

    # Handle pointer modifiers
    is_pointer = ' *' in holyc_type or holyc_type.startswith('*')
    holyc_type = holyc_type.replace(' *', '').replace('*', '').strip()

    # Handle multiple space-separated tokens (e.g., "extern class")
    tokens = holyc_type.split()

    # Find the actual type name
    base = None
    extras = []
    for t in tokens:
        if t in HOLYC_TO_C_TYPE:
            base = HOLYC_TO_C_TYPE[t]
        elif t in ('class', 'union', 'struct', 'extern', '__extern'):
            continue
        else:
            extras.append(t)

    if base is None and extras:
        base = extras[-1]
        extras = extras[:-1]

    if base is None:
        base = 'int'

    result = base
    if extras:
        result = result + ' ' + ' '.join(extras)
    if is_pointer:
        result += ' *'

    return result


def map_param_type(param: str) -> str:
    """Map a single parameter declaration."""
    # Handle "I64 name"
    param = param.strip()
    if not param:
        return 'void'

    # Map types within the parameter
    result = map_types_in_text(param)
    return result


def map_types_in_text(text: str) -> str:
    """Map all HolyC type names in a text string to C types."""
    # Order by length (longest first) to avoid partial replacements
    types_sorted = sorted(HOLYC_TO_C_TYPE.keys(), key=len, reverse=True)

    # Tokenize the text by word boundaries
    result = text
    for ht, ct in sorted(HOLYC_TO_C_TYPE.items(), key=lambda x: -len(x[0])):
        # Replace whole-word occurrences
        result = re.sub(r'\b' + re.escape(ht) + r'\b', ct, result)

    # Handle INVALID_PTR
    result = result.replace('INVALID_PTR', '((void*)(intptr_t)0x7FFFFFFFFFFFFFFFUL)')

    return result


# ---------------------------------------------------------------------------
# MAIN TRANSPILER
# ---------------------------------------------------------------------------
def transpile_file(input_path: str, output_path: str, include_dirs: List[str] = None,
                   verbose: bool = False) -> None:
    """Transpile a HolyC source file to C."""
    with open(input_path, 'r') as f:
        source = f.read()

    # Pre-process: handle #exe blocks (simplified — just strip them)
    source = re.sub(r'#exe\s*\{[^}]*\}', '// #exe { ... stripped }', source)
    source = re.sub(r'#exe\s*\n\s*\{', '// #exe {', source)

    tokens = tokenize(source)

    if verbose:
        for t in tokens[:50]:
            print(f'  {t}', file=sys.stderr)

    parser = Parser(tokens)
    output = parser.parse()

    # Add include guard and standard includes
    guard = os.path.basename(output_path).replace('.', '_').upper()
    header = [
        f'// Auto-generated from {input_path} by holyc2c.py',
        f'#ifndef _{guard}_',
        f'#define _{guard}_',
        '',
        '#include "holyc.h"',
    ]

    # Add custom includes
    for inc in parser.includes:
        header.append(f'#include "{inc}"')

    header.append('')

    result = '\n'.join(header) + '\n' + output + f'\n\n#endif /* _{guard}_ */\n'

    with open(output_path, 'w') as f:
        f.write(result)

    if verbose:
        print(f'\n--- Output ({len(result)} bytes) ---', file=sys.stderr)
        print(result[:500], file=sys.stderr)

    print(f'Transpiled {input_path} -> {output_path} ({len(result)} bytes, {len(output)} code)')


def main():
    parser = argparse.ArgumentParser(description='HolyC-to-C Transpiler')
    parser.add_argument('input', help='Input .HC file')
    parser.add_argument('output', help='Output .c file')
    parser.add_argument('--include-dir', '-I', action='append', default=[])
    parser.add_argument('--verbose', '-v', action='store_true')
    args = parser.parse_args()

    transpile_file(args.input, args.output, args.include_dir, args.verbose)


if __name__ == '__main__':
    main()
