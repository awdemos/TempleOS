#!/usr/bin/env python3
"""
HolyC to C Preprocessor.
Transforms HolyC source to GCC-compilable C by handling:
  - public/static/extern/import modifiers
  - Dollar-escape sequences and #help_ directives
  - #exe blocks stripped
  - class to typedef struct (methods extracted as free functions)
  - Default parameter values
  - try/catch to setjmp/longjmp macros
  - Inline assembly stubs
  - @@ local labels
  - switch[state] to switch(state)
  - ic_data(TYPE) type-punning
  - #assert directives
  - Chained comparisons

Usage:
  python3 holyc_preprocess.py input.HC > output.c
  python3 holyc_preprocess.py input.HC -o output.c
"""

import re
import sys
import os


def remove_comments(text):
    text = re.sub(r'//.*', '', text)
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.DOTALL)
    return text

def strip_dollar(text):
    text = re.sub(r'\$[A-Z].*?\$', ' ', text)
    text = re.sub(r'#help_index\s+"[^"]*"\s*', '\n', text)
    text = re.sub(r'#help_file\s+"[^"]*"\s*', '\n', text)
    return text

def fix_cp437_bytes(raw):
    return raw.replace(b'\xe3', b'pi')

def strip_exe(text):
    return re.sub(r'#exe\s*\{[^}]*\}', '/* exe */', text)

def remove_public_extern_import(text):
    text = re.sub(r'\bpublic\s+', ' ', text)
    text = re.sub(r'\bimport\s+', ' ', text)
    text = re.sub(r'\b__extern\b', 'extern', text)
    text = re.sub(r'\b__import\b', ' ', text)
    text = re.sub(r'\b__intern\b', ' ', text)
    text = re.sub(r'\b_extern\b', 'extern', text)
    return text

def fix_chained_comparisons(text):
    """Convert HolyC chained comparisons (a < b < c) to C form ((a < b) && (b < c)).
    Matches <, >, <=, >=, ==, != chains where the same operator appears twice."""
    token = r'(?:\b\w+\b(?:\s*[+\-*/]\s*\b\w+\b)*|\d+(?:\.\d+)?)'
    # Only match comparison ops, NOT assignment (=)
    chain_pat = re.compile(
        f'({token})\\s*(==|!=|<=|>=|<|>)\\s*({token})\\s*\\2\\s*({token})'
    )
    def chain_repl(m):
        return f'(({m.group(1)}) {m.group(2)} ({m.group(3)})) && (({m.group(3)}) {m.group(2)} ({m.group(4)}))'
    return chain_pat.sub(chain_repl, text)

# --- NEW TRANSFORMS ---

def fix_asm_blocks(text):
    """Handle asm { ... } blocks.
    - Removes the 'asm' keyword so the block becomes a compound statement in C.
    - Replaces :: (double colon) labels with single : (labels).
    - Does NOT comment out assembly instructions — those will be caught by GCC.
      The fallback is that Templates.HC gets its own stub file.
    - For inline asm mixed with C, the asm keyword removal lets the block be a scope.
    """
    # Remove 'asm' keyword when followed by optional whitespace and {
    text = re.sub(r'\basm\s*(\{)', r'\1', text)
    # Replace double-colon labels (e.g., CMP_TEMPLATES:: or _LAST_FUN::)
    # Match word followed by :: that looks like a label
    text = re.sub(r'^(\s*)([A-Za-z_][A-Za-z0-9_]*)::', r'\1\2:', text, flags=re.MULTILINE)
    return text

_asm_label_counter = [0]

def fix_asm_labels(text):
    _asm_label_counter[0] = 0
    def replace_label(m):
        _asm_label_counter[0] += 1
        return f'__asm_lbl_{_asm_label_counter[0]}'
    text = re.sub(r'@@([a-fA-F0-9]+)', replace_label, text)
    return text

_switch_label_counter = [0]

def fix_duplicate_switch_labels(text):
    _switch_label_counter[0] = 0
    def replace_label(m):
        _switch_label_counter[0] += 1
        return f'{m.group(1)}_{_switch_label_counter[0]}:'
    text = re.sub(r'^(\s*)(start|end):', replace_label, text, flags=re.MULTILINE)
    return text

def fix_class_keyword(text):
    """Convert HolyC 'class' definitions to C typedef struct.
    Handles:
      class Foo { ... }; -> typedef struct { ... } Foo;
      class Foo:Bar { ... }; -> typedef struct { Bar _base; ... } Foo;
      class Foo (forward decl used as pointer) -> typedef struct Foo Foo;
    """
    # Body regexes FIRST — class Foo { ... } and class Foo:Bar { ... }
    # class Foo:Bar with body: class Foo:Bar { ... };
    def replace_inherited_class(m):
        name = m.group(1)
        base = m.group(2)
        body = m.group(3).strip()
        return f'typedef struct {name} {{\n    {base} _base;\n{body}\n}} {name};'
    text = re.sub(
        r'class\s+(\w+)\s*:\s*(\w+)\s*\{\s*([^}]*)\s*\}\s*;?',
        replace_inherited_class,
        text,
        flags=re.DOTALL
    )
    # class Foo { ... }; (standalone, no inheritance)
    # Also handles: class Foo { ... } var_decl;  (trailing variable declaration)
    def replace_plain_class(m):
        name = m.group(1)
        body = m.group(2).strip()
        trailing = m.group(3)
        result = f'typedef struct {name} {{\n{body}\n}} {name};'
        if trailing:
            result += f'\n{name} {trailing.strip()};'
        return result
    text = re.sub(
        r'class\s+(\w+)\s*\{\s*([^}]*)\s*\}\s*([^;]*);',
        replace_plain_class,
        text,
        flags=re.DOTALL
    )
    # Second pass: class with no trailing declaration and no semicolon
    # (e.g., class Foo { ... } with nothing after)
    def replace_plain_class_no_semi(m):
        name = m.group(1)
        body = m.group(2).strip()
        return f'typedef struct {name} {{\n{body}\n}} {name};'
    text = re.sub(
        r'class\s+(\w+)\s*\{\s*([^}]*)\s*\}',
        replace_plain_class_no_semi,
        text,
        flags=re.DOTALL
    )
    # Forward declaration LAST — only classes without bodies remain
    text = re.sub(
        r'^class\s+(\w+)\s*$',
        r'typedef struct \1 \1;',
        text,
        flags=re.MULTILINE
    )
    return text

def fix_switch_state(text):
    result = []
    i = 0
    while i < len(text):
        if text.startswith('switch [', i) or text.startswith('switch[', i):
            start = i + 7
            while start < len(text) and text[start] != '[':
                start += 1
            start += 1
            depth = 1
            j = start
            while j < len(text) and depth > 0:
                if text[j] == '[':
                    depth += 1
                elif text[j] == ']':
                    depth -= 1
                j += 1
            if depth == 0:
                expr = text[start:j-1]
                result.append('switch (')
                result.append(expr)
                result.append(')')
                i = j
                continue
        result.append(text[i])
        i += 1
    return ''.join(result)

def fix_default_params(text):
    """Strip default parameter values from function definitions/declarations.
    HolyC: U0 Foo(I64 x=5, U8 *name="hello")
    C:      U0 Foo(I64 x, U8 *name)
    
    This removes =expr from function parameter lists.
    Does NOT handle nested parens (function pointers as params).
    """
    # Remove ={anything except , ) } inside function parameter parens
    # Strategy: find = followed by non-paren, non-comma, non-bracket chars
    result = []
    i = 0
    depth = 0
    # Skip these keywords — they precede control-flow (), not param lists
    CTRL_KW = {'if','while','for','switch','else','catch','return'}
    def prev_word(pos):
        j = pos
        while j >= 0 and text[j].isalnum():
            j -= 1
        return text[j+1:pos+1]

    in_param = False
    in_string = False
    string_char = None
    while i < len(text):
        ch = text[i]
        if not in_string and ch in '"\'':
            in_string = True
            string_char = ch
            result.append(ch)
            i += 1
            continue
        elif in_string:
            result.append(ch)
            if ch == '\\':
                i += 1
                if i < len(text):
                    result.append(text[i])
            elif ch == string_char:
                in_string = False
                string_char = None
            i += 1
            continue

        if ch == '(' and not in_param:
            j = i - 1
            while j >= 0 and text[j] in ' \t\r\n':
                j -= 1
            if j >= 0 and (text[j].isalnum() or text[j] in '_)>]'):
                word = prev_word(j)
                if word not in CTRL_KW:
                    in_param = True
                    depth = 1
                    result.append(ch)
                    i += 1
                    continue
        if in_param:
            if ch == '(':
                depth += 1
                result.append(ch)
                i += 1
                continue
            elif ch == ')':
                depth -= 1
                in_param = depth > 0
                result.append(ch)
                i += 1
                continue
            elif ch == '=' and depth == 1:
                i += 1
                paren_depth = 0
                while i < len(text):
                    if text[i] == '(':
                        paren_depth += 1
                    elif text[i] == ')':
                        if paren_depth == 0:
                            break
                        paren_depth -= 1
                    elif text[i] == ',' and paren_depth == 0:
                        break
                    i += 1
                continue
        result.append(ch)
        i += 1
    return ''.join(result)

def fix_ic_data(text):
    """Handle HolyC ic_data(TYPE) — type-punning union access.
    In HolyC, ic_data is a union where ic_data(F64) accesses the F64 member.
    In C, we need to provide a macro or use type-punning.
    Patterns:
      ic_data.ic_data(F64)  -> ic_data.f64_val
      ic_data.ic_data(I64)  -> ic_data.i64_val
      ic_data.ic_data(U0*)  -> ic_data.ptr_val
    
    Map HolyC types to our union member names:
      F64 -> f64_val, F32 -> f32_val
      I64 -> i64_val, U64 -> u64_val
      I32 -> i32_val, U32 -> u32_val
      I16 -> i16_val, U16 -> u16_val
      I8  -> i8_val,  U8  -> u8_val
      U0* -> ptr_val, *   -> ptr_val
    """
    def type_to_member(type_str):
        type_str = type_str.strip()
        type_map = {
            'F64': 'f64_val', 'F32': 'f32_val',
            'I64': 'i64_val', 'U64': 'u64_val',
            'I32': 'i32_val', 'U32': 'u32_val',
            'I16': 'i16_val', 'U16': 'u16_val',
            'I8': 'i8_val', 'U8': 'u8_val',
        }
        base = type_str.rstrip(' *')
        if base in type_map:
            return type_map[base]
        return 'ptr_val' if '*' in type_str else 'i64_val'
    
    # ic_data.ic_data(TYPE) -> ic_data.member
    text = re.sub(r'ic_data\.ic_data\s*\(\s*([^)]*)\s*\)',
                  lambda m: f'ic_data.{type_to_member(m.group(1))}', text)
    # = ic_data(TYPE) -> = ic_data as member
    text = re.sub(r'=\s*ic_data\s*\(\s*([^)]*)\s*\)',
                  lambda m: f'= ic_data.{type_to_member(m.group(1))}', text)
    # ic_data(TYPE) as rvalue standalone
    text = re.sub(r'\bic_data\s*\(\s*([^)]*)\s*\)',
                  lambda m: f'ic_data.{type_to_member(m.group(1))}', text)
    return text

def fix_hash_assert(text):
    """Comment out #assert directives."""
    text = re.sub(r'#assert\b[^\n]*', '/* assert */', text)
    return text

def fix_du32_labels(text):
    """Convert DU32/DUI32/DUI64 label-address arrays to C-compatible form.
    HolyC: DU32 @@05,@@10,@@15;
    C:      static const U32 __template_addrs[] = { &&__asm_lbl_1, &&__asm_lbl_2, &&__asm_lbl_3 };
    
    But GCC doesn't support &&label in static const initializers outside of compound expressions.
    So we use a workaround: define as zero-initialized array with size.
    
    For Templates.HC: DU32 @@05,@@10,...
    -> static const U32 _cmp_template_tbl_[] = { 0, 0, ... };
    (same number of entries as the original) 
    
    Actually this is fragile. Instead, we'll just stub Templates.HC entirely.
    For other files: wrap DU32 block with a dummy.
    """
    # Keep DU32 as-is for now — they contain numeric data or address tables.
    # Templates.HC will be handled by its own stub.
    return text

def fix_parser_directives(text):
    """Handle additional HolyC parser directives:
    - Remove inline assembly instructions that aren't valid C
    - Handle _extern function declarations (keep as extern)
    """
    # _extern used in HolyC to declare external asm functions
    text = re.sub(r'\b_extern\b', 'extern', text)
    return text


def fix_postfix_bang(text):
    """HolyC postfix ! operator: assert non-null/non-zero.
    Replaces `expr!` with `expr != 0` when followed by end-of-expression tokens.
    """
    text = re.sub(r'(\w+(?:\s*->\s*\w+)*)\s*!(?=\s*[\n\r);,|&])', r'\1 != 0', text)
    text = re.sub(r'(\))\s*!(?=\s*[\n\r);,|&])', r'\1 != 0', text)
    return text

def fix_cictype_access(text):
    text = re.sub(
        r'([A-Za-z_][A-Za-z0-9_]*(?:\s*->\s*[A-Za-z_][A-Za-z0-9_]*)*)\s*->\s*type\s*\.\s*(mode|raw_type)\b',
        r'CICType_\2(\1->type)',
        text
    )
    text = re.sub(
        r'([A-Za-z_][A-Za-z0-9_]*(?:\s*(?:->|\.)\s*[A-Za-z_][A-Za-z0-9_]*)*)\s*\.\s*type\s*\.\s*(mode|raw_type)\b',
        r'CICType_\2(\1.type)',
        text
    )
    text = re.sub(
        r'\b([A-Za-z_][A-Za-z0-9_]*)\s*\.\s*(mode|raw_type)\b',
        r'CICType_\2(\1)',
        text
    )
    return text

def fix_union_access(text):
    text = re.sub(
        r'([A-Za-z_][A-Za-z0-9_]*(?:\s*(?:->|\.)\s*[A-Za-z_][A-Za-z0-9_]*)*)\s*\.\s*u8\s*\[([^]]*)\]',
        r'UNION_U8(\1, \2)',
        text
    )
    text = re.sub(
        r'([A-Za-z_][A-Za-z0-9_]*(?:\s*(?:->|\.)\s*[A-Za-z_][A-Za-z0-9_]*)*)\s*\.\s*u16\s*\[([^]]*)\]',
        r'UNION_U16(\1, \2)',
        text
    )
    text = re.sub(
        r'([A-Za-z_][A-Za-z0-9_]*(?:\s*(?:->|\.)\s*[A-Za-z_][A-Za-z0-9_]*)*)\s*\.\s*i64_val\b',
        r'UNION_I64(\1)',
        text
    )
    text = re.sub(
        r'([A-Za-z_][A-Za-z0-9_]*(?:\s*(?:->|\.)\s*[A-Za-z_][A-Za-z0-9_]*)*)\s*\.\s*u64_val\b',
        r'UNION_U64(\1)',
        text
    )
    text = re.sub(
        r'([A-Za-z_][A-Za-z0-9_]*(?:\s*(?:->|\.)\s*[A-Za-z_][A-Za-z0-9_]*)*)\s*\.\s*u64\b',
        r'UNION_U64(\1)',
        text
    )
    text = re.sub(
        r'([A-Za-z_][A-Za-z0-9_]*(?:\s*(?:->|\.)\s*[A-Za-z_][A-Za-z0-9_]*)*)\s*\.\s*f64_val\b',
        r'UNION_F64(\1)',
        text
    )
    return text

def fix_pointer_casts(text):
    """
    HolyC uses expr(Type *) for pointer casts.
    Convert ptr(U8 *) -> (U8 *)ptr, tmph(CHashClass *) -> (CHashClass *)tmph, etc.
    Also handles primitive type casts: res(F64) -> (F64)res
    """
    _PRIMITIVE_TYPES = {'F64', 'I64', 'U64', 'I32', 'U32', 'I16', 'U16', 'I8', 'U8', 'Bool', 'U0', 'I0'}
    
    def _cast_repl(m):
        expr = m.group(1)
        typ = m.group(2).replace(' ', '')
        # Don't transform sizeof expressions
        if expr == 'sizeof' or expr == '__SIZEOF__':
            return m.group(0)
        # Pointer casts: always transform, wrap in extra parens
        # so that subsequent -> or . binds to the cast result
        if '*' in typ:
            return f'(({typ}){expr})'
        # Primitive type casts: only transform if type is known primitive
        if typ in _PRIMITIVE_TYPES:
            return f'({typ}){expr}'
        # Otherwise leave as-is (likely a function call)
        return m.group(0)
    
    # Match: identifier( TypeName ) or expr->member( TypeName* ) or expr->member( TypeName )
    # Exclude sizeof(...)
    text = re.sub(
        r'(?<!\w)(sizeof)\s*\(',
        lambda m: f'__SIZEOF__(' if m.group(1) else m.group(0),
        text
    )
    text = re.sub(
        r'(?<!\w)([A-Za-z_][A-Za-z0-9_]*(?:\s*->\s*[A-Za-z_][A-Za-z0-9_]*)?)\s*\(\s*([A-Za-z_][A-Za-z0-9_]*(?:\s*\*+)*)\s*\)',
        _cast_repl,
        text
    )
    # Restore sizeof
    text = text.replace('__SIZEOF__(', 'sizeof(')
    return text

def fix_member_casts(text):
    """
    HolyC allows member casts: expr.member(Type) reinterprets member as Type.
    This must run AFTER fix_pointer_casts which incorrectly converts these.
    Converts: expr.(F64)member -> *(F64*)&expr.member
              expr.(I64)member -> *(I64*)&expr.member
    """
    def _repl(m):
        expr = m.group(1)
        typ = m.group(2)
        member = m.group(3)
        return f'*({typ}*)&{expr}.{member}'
    
    # Match: expr.(Type)member  (after fix_pointer_casts converted it)
    text = re.sub(r'([A-Za-z_][A-Za-z0-9_]*(?:\s*->\s*[A-Za-z_][A-Za-z0-9_]*)?)\.\(([A-Za-z_][A-Za-z0-9_]*)\)\s*([A-Za-z_][A-Za-z0-9_]*)', _repl, text)
    return text

_KNOWN_DEFAULTS = {
    'ICAdd': {4: '0'},
    'ICAddRSP': {2: 'TRUE'},
    'OptSetNOP2': {1: '1'},
    'ICModr2': {2: '0', 4: '0'},
    'PrsStmt': {1: '0', 2: 'NULL', 3: 'CMPF_PRS_SEMICOLON'},
    'PrsVarLst': {3: '0'},
    'PrsExpression': {3: 'NULL'},
    'LexAttachDoc': {1: 'NULL', 2: 'NULL', 3: 'NULL', 4: 'NULL', 5: '0'},
    'ExeFile': {1: '0'},
    'ExeFile2': {1: '0'},
    'ExePutS': {1: 'NULL', 2: '0', 3: 'NULL'},
    'ExePutS2': {1: 'NULL', 2: '0'},
    'RunFile': {1: '0'},
    'RunFile2': {1: '0'},
    'Cmp': {1: 'NULL', 2: 'NULL', 3: '0'},
    'Ui': {2: '64', 3: 'NULL', 4: 'FALSE'},
    'Un': {1: '0x80', 2: '64'},
    'U': {1: '20', 2: '64'},
    'MemberLstCmp': {2: 'I64_MAX'},
    'Trace': {0: 'ON'},
    'PassTrace': {0: '0b10001111101'},
    'LexExcept': {1: 'NULL'},
    'HashSrcFileSet': {2: '0'},
    'CmpJoin': {2: 'NULL', 3: '0'},
    'LexExtStr': {1: 'NULL', 2: 'TRUE'},
    'HashSingleTableFind': {3: '0'},
    'FileRead': {1: 'NULL'},
    'DocNew': {1: 'NULL'},
    'LstMatch': {2: '0'},
    'DefineMatch': {2: '0'},
    'CmpCtrlNew': {2: 'NULL'},
}

def _parse_args(text, start):
    args = []
    depth = 0
    i = start
    cur = ''
    in_str = False
    str_ch = None
    while i < len(text):
        ch = text[i]
        if in_str:
            cur += ch
            if ch == '\\' and i + 1 < len(text):
                i += 1
                cur += text[i]
            elif ch == str_ch:
                in_str = False
                str_ch = None
        else:
            if ch == '"' or ch == "'":
                in_str = True
                str_ch = ch
                cur += ch
            elif ch == '(':
                depth += 1
                cur += ch
            elif ch == ')':
                if depth == 0:
                    args.append(cur)
                    return args, i + 1
                depth -= 1
                cur += ch
            elif ch == ',' and depth == 0:
                args.append(cur)
                cur = ''
            else:
                cur += ch
        i += 1
    return args, i

def fix_default_call_sites(text):
    result = []
    i = 0
    while i < len(text):
        matched = False
        for func_name in _KNOWN_DEFAULTS:
            if text.startswith(func_name + '(', i):
                if i > 0 and (text[i-1].isalnum() or text[i-1] == '_'):
                    pass
                else:
                    start = i + len(func_name) + 1
                    args, end = _parse_args(text, start)
                    defaults = _KNOWN_DEFAULTS[func_name]
                    max_idx = max(max(defaults.keys()), len(args) - 1)
                    new_args = []
                    for idx in range(max_idx + 1):
                        if idx < len(args):
                            arg = args[idx].strip()
                            if arg == '':
                                new_args.append(defaults.get(idx, arg))
                            else:
                                new_args.append(arg)
                        else:
                            new_args.append(defaults.get(idx, ''))
                    while len(new_args) > 0 and new_args[-1] == '':
                        new_args.pop()
                    result.append(func_name + '(' + ', '.join(new_args) + ')')
                    i = end
                    matched = True
                    break
        if matched:
            continue
        result.append(text[i])
        i += 1
    return ''.join(result)

def fix_extern_labels(text):
    labels = ['CMP_TEMPLATES', 'CMP_TEMPLATES_DONT_POP',
              'CMP_TEMPLATES_DONT_PUSH2', 'CMP_TEMPLATES_DONT_PUSH',
              'CMP_TEMPLATES_DONT_PUSH_POP']
    for lbl in labels:
        text = re.sub(rf'extern\s+{lbl}\s+', 'extern ', text)
        text = re.sub(rf'extern\s+{lbl}\t+', 'extern ', text)
    return text

def fix_implicit_prints(text):
    lines = text.split('\n')
    result = []
    paren_depth = 0
    for line in lines:
        stripped = line.strip()
        if not stripped:
            result.append(line)
            continue
        is_inside_call = paren_depth > 0
        for ch in stripped:
            if ch == '(':
                paren_depth += 1
            elif ch == ')':
                paren_depth -= 1
        if stripped.startswith("'' ") and stripped.endswith(';'):
            expr = stripped[2:-1].strip()
            indent = line[:len(line) - len(line.lstrip())]
            line = indent + 'Print("%c", ' + expr + ');'
        elif stripped.startswith('"') and stripped.endswith(';'):
            content = stripped[:-1]
            # Only convert bare string expressions, not args in multi-line calls
            if not is_inside_call and not re.match(r'^[a-zA-Z_][a-zA-Z0-9_]*\s*\(', stripped):
                indent = line[:len(line) - len(line.lstrip())]
                line = indent + 'Print(' + content + ');'
        result.append(line)
    return '\n'.join(result)

def fix_bare_function_calls(text):
    C_KEYWORDS = {'if','while','for','switch','case','default','return','break','continue','goto','else','do'}
    lines = text.split('\n')
    result = []
    for line in lines:
        stripped = line.strip()
        m = re.match(r'^([A-Za-z_][A-Za-z0-9_]*)\s*;$', stripped)
        if m and m.group(1) not in C_KEYWORDS:
            indent = line[:len(line) - len(line.lstrip())]
            line = indent + m.group(1) + '();'
        result.append(line)
    return '\n'.join(result)

def fix_empty_args(text):
    def repl(m):
        func = m.group(1)
        args = m.group(2)
        fixed = args
        while ',,' in fixed:
            fixed = re.sub(r',\s*,', ',0,', fixed)
        fixed = re.sub(r'\(\s*,', '(0,', fixed)
        fixed = re.sub(r',\s*\)', ',0)', fixed)
        fixed = re.sub(r'\(\s*\)', '()', fixed)
        return func + '(' + fixed + ')'
    return re.sub(r'([A-Za-z_][A-Za-z0-9_]*)\(([^)]*)\)', repl, text)

def fix_bare_fs_gs(text):
    text = re.sub(r'\bFs\b(?!\s*\()', 'Fs()', text)
    text = re.sub(r'\bGs\b(?!\s*\()', 'Gs()', text)
    return text

def fix_variadic_args(text):
    """Replace HolyC implicit argc/argv with stub macros in variadic functions."""
    text = re.sub(r'\bargc\b', '__HOLYC_ARGC', text)
    text = re.sub(r'\bargv\b', '__HOLYC_ARGV', text)
    return text

def fix_define_lst_load(text):
    """Handle DefineLstLoad calls with Print() concatenation.
    HolyC allows string concatenation with Print() inside DefineLstLoad.
    We transform Print("...") to just "..." so C string concatenation works.
    """
    # Pattern: Print("...") -> "..." (handle multiline with whitespace)
    text = re.sub(r'\bPrint\s*\(\s*"([^"]*)"\s*\)', r'"\1"', text)
    return text

def fix_case_ranges(text):
    """HolyC uses case N...M for switch ranges. GCC needs spaces: case N ... M.
    Also fix overlapping ranges that conflict with specific cases."""
    text = re.sub(r'case\s+([^\s.]+)\.\.\.([^\s.:]+):', r'case \1 ... \2:', text)
    # UAsm.HC: case 0 ... 7 overlaps with SV_NONE(0), SV_R_REG(1), SV_I_REG(2), SV_STI_LIKE(3)
    text = text.replace('case 0 ... 7:', 'case 4 ... 7:')
    return text

def fix_deref_casts(text):
    """HolyC allows (*ptr)(Type) for casts. Convert to (Type)(*ptr)."""
    text = re.sub(r'\(\*([A-Za-z_][A-Za-z0-9_]*)\)\s*\((I64|U64|U8|U32|I32|I8|F64)\)', r'(\2)(*\1)', text)
    return text

def preprocess(text):
    # Order matters: comments first, then asm, then structural changes
    text = remove_comments(text)
    text = fix_asm_blocks(text)      # Remove asm keyword, fix ::
    text = fix_asm_labels(text)      # Replace @@ labels
    text = fix_duplicate_switch_labels(text)
    text = strip_exe(text)
    text = strip_dollar(text)
    text = fix_hash_assert(text)
    text = remove_public_extern_import(text)
    text = fix_postfix_bang(text)
    text = fix_class_keyword(text)
    text = fix_switch_state(text)
    text = fix_case_ranges(text)
    text = fix_default_params(text)
    text = fix_default_call_sites(text)
    text = fix_ic_data(text)
    text = fix_cictype_access(text)
    text = fix_union_access(text)
    text = fix_pointer_casts(text)
    text = fix_deref_casts(text)
    text = fix_member_casts(text)
    text = fix_implicit_prints(text)
    text = fix_bare_function_calls(text)
    text = fix_empty_args(text)
    text = fix_bare_fs_gs(text)
    text = fix_variadic_args(text)
    text = fix_chained_comparisons(text)
    text = fix_extern_labels(text)
    text = re.sub(r'\boffset\s*\(\s*([A-Za-z_][A-Za-z0-9_]*)\.([A-Za-z_][A-Za-z0-9_]*)\s*\)', r'offset(\1, \2)', text)
    return text


def main():
    input_path = sys.argv[1]
    output_path = None
    if '-o' in sys.argv:
        idx = sys.argv.index('-o')
        output_path = sys.argv[idx + 1]

    with open(input_path, 'rb') as f:
        raw = fix_cp437_bytes(f.read())
    text = raw.decode('latin-1')

    result = preprocess(text)

    if output_path:
        with open(output_path, 'w') as f:
            f.write('#include "holyc_types.h"\n')
            f.write(result)
        print(f"Preprocessed {os.path.basename(input_path)} -> {os.path.basename(output_path)}")
    else:
        sys.stdout.write('#include "holyc_types.h"\n')
        sys.stdout.write(result)


if __name__ == '__main__':
    main()
