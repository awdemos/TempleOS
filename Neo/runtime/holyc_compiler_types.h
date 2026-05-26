#ifndef _HOLYC_COMPILER_TYPES_H
#define _HOLYC_COMPILER_TYPES_H

#include "holyc_types.h"

/* Forward declarations for types used before their definition */
typedef struct CHashFun CHashFun;
typedef struct CInst CInst;

/*============================================================================
   RAW TYPES (RT_*) — KernelA.HH
  Base types for the HolyC type system (not same as C types).
  RT_PTR == RT_I64 (signed to allow negative error codes).
============================================================================*/
#define RT_NONE     1
#define RT_I0       2
#define RT_U0       3
#define RT_I8       4
#define RT_U8       5
#define RT_I16      6
#define RT_U16      7
#define RT_I32      8
#define RT_U32      9
#define RT_I64      10
#define RT_PTR      10
#define RT_U64      11
#define RT_F32      12
#define RT_UF32     13
#define RT_F64      14
#define RT_UF64     15
#define RT_RTS_NUM  16

#define I8_MIN (-0x80)
#define I8_MAX 0x7F
#define I16_MIN (-0x8000)
#define I16_MAX 0x7FFF
#define I32_MIN (-0x80000000)
#define I32_MAX 0x7FFFFFFF
#define I64_MAX 0x7FFFFFFFFFFFFFFF
#define U8_MAX  0xFF
#define U8_MIN  0
#define U16_MIN 0

/*============================================================================
   HASH TABLE TYPES (HTT_*) — tells HashAdd/HashFind what to store
============================================================================*/
#define HTT_INVALID         0
#define HTT_EXPORT_SYS_SYM  0x00001 //CHashExport
#define HTT_IMPORT_SYS_SYM  0x00002 //CHashImport
#define HTT_DEFINE_STR      0x00004 //CHashDefineStr
#define HTT_GLBL_VAR        0x00008 //CHashGlblVar
#define HTT_CLASS           0x00010 //CHashClass
#define HTT_INTERNAL_TYPE   0x00020 //CHashClass
#define HTT_FUN             0x00040 //CHashFun
#define HTT_WORD            0x00080 //CHashAC
#define HTT_DICT_WORD       0x00100 //CHashGeneric
#define HTT_KEYWORD         0x00200 //CHashGeneric
#define HTT_ASM_KEYWORD     0x00400 //CHashGeneric
#define HTT_OPCODE          0x00800 //CHashOpcode
#define HTT_REG             0x01000 //CHashReg
#define HTT_FILE            0x02000 //CHashGeneric
#define HTT_MODULE          0x04000 //CHashGeneric
#define HTT_HELP_FILE       0x08000 //CHashSrcSym
#define HTT_FRAME_PTR       0x10000 //CHashGeneric
#define HTG_TYPE_MASK       0x1FFFF

#define HTF_PRIVATE         0x00800000
#define HTF_PUBLIC          0x01000000
#define HTF_EXPORT          0x02000000
#define HTF_IMPORT          0x04000000
#define HTF_IMM             0x08000000
#define HTF_GOTO_LABEL      0x10000000
#define HTF_RESOLVE         0x20000000
#define HTF_UNRESOLVED      0x40000000
#define HTF_LOCAL           0x80000000
#define HTG_FLAGS_MASK      0xFF000000

#define HTG_SRC_SYM (HTT_DEFINE_STR|HTT_GLBL_VAR|HTT_FUN|HTT_CLASS|                     HTT_EXPORT_SYS_SYM|HTT_HELP_FILE)
#define HTG_ALL     -1

/*============================================================================
  COMPILER / CMP CTRL FLAGS — KernelA.HH
============================================================================*/
#define CCF_NULL                0x000000000
#define CCF_CMD_LINE            0x000000001
#define CCF_PMT                 0x000000002
#define CCF_QUESTION_HELP       0x000000004
#define CCF_DONT_FREE_BUF       0x000000008
#define CCF_NO_DEFINES          0x000000010
#define CCF_IN_IF               0x000000020
#define CCF_JUST_LOAD           0x000000040
#define CCF_KEEP_NEW_LINES      0x000000080
#define CCF_KEEP_DOT            0x000000100
#define CCF_KEEP_SIGN_NUM       0x000000200
#define CCF_KEEP_AT_SIGN        0x000000400
#define CCF_NO_CHAR_CONST       0x000000800
#define CCF_NOT_CONST           0x000020000
#define CCF_NO_REG_OPT          0x000040000
#define CCF_IN_QUOTES           0x000080000
#define CCF_EXE_BLK             0x000100000
#define CCF_HAS_MISC_DATA       0x000200000
#define CCF_HAS_RETURN          0x000400000
#define CCF_ASM_EXPRESSIONS     0x000800000
#define CCF_UNRESOLVED          0x001000000
#define CCF_LOCAL               0x002000000
#define CCF_FUN_EXP             0x004000000
#define CCF_POSTINC             0x008000000
#define CCF_POSTDEC             0x010000000
#define CCF_PREINC              0x020000000
#define CCF_PREDEC              0x040000000
#define CCF_ARRAY               0x080000000
#define CCF_RAX                 0x100000000
#define CCF_USE_LAST_U16        0x200000000
#define CCF_LAST_WAS_DOT        0x400000000
#define CCF_AOT_COMPILE         0x800000000
#define CCF_NO_ABSS            0x1000000000
#define CCF_PAREN              0x2000000000
#define CCF_CLASS_DOL_OFFSET   0x4000000000
#define CCF_DONT_MAKE_RES      0x8000000000

#define CMPF_ASM_BLK      1
#define CMPF_ONE_ASM_INS  2
#define CMPF_LEX_FIRST    4
#define CMPF_PRS_SEMICOLON 8

/*============================================================================
  INTERMEDIATE CODE FLAGS (ICF_*) — KernelA.HH
============================================================================*/
#define ICF_RES_TO_F64          0x000000001
#define ICF_RES_TO_INT          0x000000002
#define ICF_ARG1_TO_F64         0x000000004
#define ICF_ARG1_TO_INT         0x000000008
#define ICF_ARG2_TO_F64         0x000000010
#define ICF_ARG2_TO_INT         0x000000020
#define ICF_USE_F64             0x000000040
#define ICF_USE_UNSIGNED        0x000000080
#define ICF_USE_INT             0x000000100
#define ICF_RES_NOT_USED        0x000000200
#define ICF_CODE_FINAL          0x000000400
#define ICF_BY_VAL              0x000000800
#define ICF_SHORT_JMP           0x000001000
#define ICF_PUSH_RES            0x000002000
#define ICF_PASS_TRACE          0x000004000
#define ICF_RES_WAS_STK         0x000008000
#define ICF_ARG1_WAS_STK        0x000010000
#define ICF_ARG2_WAS_STK        0x000020000
#define ICF_PUSH_CMP            0x000040000
#define ICF_POP_CMP             0x000080000
#define ICF_SWAP                0x000100000
#define ICF_ALT_TEMPLATE        0x008000000
#define ICF_LOCK                0x010000000
#define ICF_NO_RIP              0x020000000
#define ICF_DEL_PREV_INS        0x040000000
#define ICF_PREV_DELETED        0x080000000
#define ICF_DONT_RESTORE        0x100000000

#define IC_BODY_SIZE 0x83

/*============================================================================
  CODE MISC TYPES (CMT_*), FLAGS (CMF_*) — KernelA.HH
============================================================================*/
#define CMT_LABEL           0
#define CMT_ASM_LABEL       1
#define CMT_GOTO_LABEL      2
#define CMT_STR_CONST       3
#define CMT_JMP_TABLE       4
#define CMT_FLOAT_CONSTS    5
#define CMT_ARRAY_DIM       6
#define CMT_HASH_ENTRY      7

#define CMF_POP_CMP         0x01
#define CMF_DEFINED         0x02
#define CMF_I8_JMP_TABLE    0x04
#define CMF_U8_JMP_TABLE    0x08
#define CMF_I16_JMP_TABLE   0x10
#define CMF_U16_JMP_TABLE   0x20

/*============================================================================
  FUNCTION FLAGS — KernelA.HH
============================================================================*/
#define Ff_INTERRUPT        8
#define Ff_HASERRCODE       9
#define Ff_ARGPOP           10
#define Ff_NOARGPOP         11
#define Ff_INTERNAL         12
#define Ff__EXTERN          13
#define Ff_DOT_DOT_DOT      14
#define Ff_RET1             15

#define FSF_PUBLIC          0x01
#define FSF_ASM             0x02
#define FSF_STATIC          0x04
#define FSF__               0x08
#define FSF_INTERRUPT       (1<<Ff_INTERRUPT)
#define FSF_HASERRCODE      (1<<Ff_HASERRCODE)
#define FSF_ARGPOP          (1<<Ff_ARGPOP)
#define FSF_NOARGPOP        (1<<Ff_NOARGPOP)

/*============================================================================
  RAW TYPE FLAGS & EXCEPT FLAGS
============================================================================*/
#define RTF_UNSIGNED        1
#define ECF_HAS_PUSH_CMP    0x01
#define Cf_EXTERN           0
#define Cf_INTERNAL_TYPE    1
#define ICf_DONT_PUSH_FLOAT0 21
#define ICf_DONT_POP_FLOAT0  22
#define AAT_ADD_U32         4
#define LFSF_DOC            1
#define LFSF_DEFINE         2
#define RLf_ADAM_SERVER     18
#define CH_SPACE            0x20

/*============================================================================
   REGISTER CONSTANTS
============================================================================*/
#define REG_RAX  0
#define REG_RCX  1
#define REG_RDX  2
#define REG_RBX  3
#define REG_RSP  4
#define REG_RBP  5
#define REG_RSI  6
#define REG_RDI  7
#define REG_R8   8
#define REG_R9   9
#define REG_R10  10
#define REG_R11  11
#define REG_R12  12
#define REG_R13  13
#define REG_R14  14
#define REG_R15  15
#define REG_REGS_NUM 16
#define REG_RIP  16

#define REG_NONE 32
#define REG_ALLOC 33
#define REG_UNDEF I8_MIN

#define REGG_CLOBBERED      0x013F
#define REGG_SAVED          0x0030
#define REGG_STK_TMP        0x0200
#define REGG_LOCAL_VARS     0xCCC0
#define REGG_LOCAL_NON_PTR_VARS 0x3000

#define REGT_NONE 0
#define REGT_R8   1
#define REGT_R16  2
#define REGT_R32  3
#define REGT_R64  4
#define REGT_SEG  5
#define REGT_FSTK 6
#define REGT_MM   7
#define REGT_XMM  8

/*============================================================================
  OPERAND / ARG / MODE CONSTANTS
============================================================================*/
#define OM_SIGNED     0
#define OM_UNSIGNED   1
#define OMS_SIGNED    2
#define OMS_UNSIGNED  3

#define ARGT_NONE	0
#define ARGT_REL8	1
#define ARGT_REL16	2
#define ARGT_REL32	3

#define ARGT_IMM8	4
#define ARGT_IMM16	5
#define ARGT_IMM32	6
#define ARGT_IMM64	7

#define ARGT_UIMM8	8
#define ARGT_UIMM16	9
#define ARGT_UIMM32	10
#define ARGT_UIMM64	11

#define ARGT_R8		12
#define ARGT_R16	13
#define ARGT_R32	14
#define ARGT_R64	15

#define ARGT_RM8	16
#define ARGT_RM16	17
#define ARGT_RM32	18
#define ARGT_RM64	19

#define ARGT_M8		20
#define ARGT_M16	21
#define ARGT_M32	22
#define ARGT_M64	23

#define ARGT_M1632	24
#define ARGT_M16N32	25
#define ARGT_M16N16	26
#define ARGT_M32N32	27

#define ARGT_MOFFS8	28
#define ARGT_MOFFS16	29
#define ARGT_MOFFS32	30
#define ARGT_MOFFS64	31

#define ARGT_AL		32
#define ARGT_AX		33
#define ARGT_EAX	34
#define ARGT_RAX	35

#define ARGT_CL		36
#define ARGT_DX		37
#define ARGT_SREG	39

#define ARGT_SS		40
#define ARGT_DS		41
#define ARGT_ES		42
#define ARGT_FS		43

#define ARGT_GS		44
#define ARGT_CS		45
#define ARGT_ST0	46
#define ARGT_STI	47

#define ARGT_MM		48
#define ARGT_MM32	49
#define ARGT_MM64	50
#define ARGT_XMM	51

#define ARGT_XMM32	52
#define ARGT_XMM64	53
#define ARGT_XMM128	54
#define ARGT_XMM0	55

#define MDf_STK         8
#define MDf_IMM         9
#define MDf_REG         10
#define MDf_DISP        11
#define MDf_SIB         12
#define MDf_RIP_DISP32  13

#define MDF_NULL        0x0000
#define MDF_STK         0x0100
#define MDF_IMM         0x0200
#define MDF_REG         0x0400
#define MDF_DISP        0x0800
#define MDF_SIB         0x1000
#define MDF_RIP_DISP32  0x2000
#define MDG_MASK        0xFF00
#define MDG_DISP_SIB_RIP (MDF_DISP|MDF_SIB|MDF_RIP_DISP32)
#define MDG_REG_DISP_SIB_RIP (MDF_REG|MDG_DISP_SIB_RIP)

/*============================================================================
   TASK FLAGS
============================================================================*/
#define TASKf_TIME_CRITICAL  0x0001
#define TASKf_FORCE_DOC      0x0002
#define TASKf_GRAPHICS       0x0004
#define TASKf_RUN            0x0008
#define TASKf_KILL           0x0010
#define TASKf_SUSPENDED      0x0020
#define TASKf_BIRTH_WAIT     0x0040
#define TASKf_DEATH_WAIT     0x0080
#define TASKf_EXCEPT         0x0100
#define TASKf_NUM_WAIT       0x0200
#define TASKf_WIN_INHIBIT    0x0400
#define TASKf_SIGNAL         0x0800
#define TASKf_SLEEPING       0x1000
#define TASKf_FPU_MMX        0x2000
#define TASKf_LOCK_PENDING   0x4000
#define TASKf_YIELD          0x8000

#define DISPLAYf_TASK_MANAGER_LIST 0x0001

/*============================================================================
  SYS CALL TYPES
============================================================================*/
#define SYS_RET    0
#define SYS_PREEMPT 1
#define SYS_SLEEP  2
#define SYS_EXIT   3
#define SYS_KILL   4

/*============================================================================
  OPT FLAGS
============================================================================*/
#define OPTf_WARN_PAREN            0
#define OPTf_WARN_DUP_TYPES        1
#define OPTf_KEEP_PRIVATE          2
#define OPTf_EXTERNS_TO_IMPORTS    3
#define OPTf_WARN_IMPLICIT_CAST    4
#define OPTf_NO_BUILTIN_CONST      5

#define TYP_DEF 0x800

#define OPTf_USE_IMM64         6
#define OPTf_NO_REG_VAR        7

#define OCF_ALIAS              1

#define I64_MIN (-0x8000000000000000)

#define KERNEL_MODULE_NAME     "Kernel" 

#define MLF_STATIC              0x0010
#define MLF_LASTCLASS           0x0020
#define MLF_DOT_DOT_DOT         0x0040

#define GVF_IMPORT             0x01
#define GVF_FUN                0x02
#define GVF_EXTERN             0x04
#define GVF_DATA_HEAP          0x08
#define GVF_ARRAY              0x10
#define GVF_ALIAS              0x20

#define HTt_CLASS              HTT_CLASS
#define HTt_FUN                HTT_FUN
#define HTt_GLBL_VAR           HTT_GLBL_VAR

/*============================================================================
  IC CODES — CompilerA.HH (0x00-0xB9)
  These are the compiler's INTERMEDIATE CODE opcodes, NOT x86 machine opcodes.
============================================================================*/
#define IC_END              0x00
#define IC_NOP1             0x01
#define IC_END_EXP          0x02
#define IC_NOP2             0x03
#define IC_LABEL            0x04
#define IC_CALL_START       0x05
#define IC_CALL_END         0x06
#define IC_CALL_END2        0x07
#define IC_RETURN_VAL       0x08
#define IC_RETURN_VAL2      0x09
#define IC_IMM_I64          0x0A
#define IC_IMM_F64          0x0B
#define IC_STR_CONST        0x0C
#define IC_ABS_ADDR         0x0D
#define IC_ADDR_IMPORT      0x0E
#define IC_HEAP_GLBL        0x0F
#define IC_SIZEOF           0x10
#define IC_TYPE             0x11
#define IC_GET_LABEL        0x12
#define IC_RBP              0x13
#define IC_REG              0x14
#define IC_FS               0x15
#define IC_MOV_FS           0x16
#define IC_GS               0x17
#define IC_MOV_GS           0x18
#define IC_LEA              0x19
#define IC_MOV              0x1A
#define IC_TO_I64           0x1B
#define IC_TO_F64           0x1C
#define IC_TO_BOOL          0x1D
#define IC_TOUPPER          0x1E
#define IC_HOLYC_TYPECAST   0x1F
#define IC_ADDR             0x20
#define IC_COM              0x21
#define IC_NOT              0x22
#define IC_UNARY_MINUS      0x23
#define IC_DEREF            0x24
#define IC_DEREF_PP         0x25
#define IC_DEREF_MM         0x26
#define IC__PP              0x27
#define IC__MM              0x28
#define IC_PP_              0x29
#define IC_MM_              0x2A
#define IC_SHL              0x2B
#define IC_SHR              0x2C
#define IC_SHL_CONST        0x2D
#define IC_SHR_CONST        0x2E
#define IC_POWER            0x2F
#define IC_MUL              0x30
#define IC_DIV              0x31
#define IC_MOD              0x32
#define IC_AND              0x33
#define IC_OR               0x34
#define IC_XOR              0x35
#define IC_ADD              0x36
#define IC_SUB              0x37
#define IC_ADD_CONST        0x38
#define IC_SUB_CONST        0x39
#define IC_EQU_EQU          0x3A
#define IC_NOT_EQU          0x3B
#define IC_LESS             0x3C
#define IC_GREATER_EQU      0x3D
#define IC_GREATER          0x3E
#define IC_LESS_EQU         0x3F
#define IC_PUSH_CMP         0x40
#define IC_AND_AND          0x41
#define IC_OR_OR            0x42
#define IC_XOR_XOR          0x43
#define IC_ASSIGN           0x44
#define IC_ASSIGN_PP        0x45
#define IC_ASSIGN_MM        0x46
#define IC_SHL_EQU          0x47
#define IC_SHR_EQU          0x48
#define IC_MUL_EQU          0x49
#define IC_DIV_EQU          0x4A
#define IC_MOD_EQU          0x4B
#define IC_AND_EQU          0x4C
#define IC_OR_EQU           0x4D
#define IC_XOR_EQU          0x4E
#define IC_ADD_EQU          0x4F
#define IC_SUB_EQU          0x50
#define IC_JMP              0x51
#define IC_SUB_CALL         0x52
#define IC_SWITCH           0x53
#define IC_NOBOUND_SWITCH   0x54
#define IC_ADD_RSP          0x55
#define IC_ADD_RSP1         0x56
#define IC_ENTER            0x57
#define IC_PUSH_REGS        0x58
#define IC_POP_REGS         0x59
#define IC_LEAVE            0x5A
#define IC_RET              0x5B
#define IC_CALL             0x5C
#define IC_CALL_INDIRECT    0x5D
#define IC_CALL_INDIRECT2   0x5E
#define IC_CALL_IMPORT      0x5F
#define IC_CALL_EXTERN      0x60
#define IC_ASM              0x61
#define IC_PUSH             0x62
#define IC_POP              0x63
#define IC_CLFLUSH          0x64
#define IC_INVLPG           0x65
#define IC_IN_U8            0x66
#define IC_IN_U16           0x67
#define IC_IN_U32           0x68
#define IC_OUT_U8           0x69
#define IC_OUT_U16          0x6A
#define IC_OUT_U32          0x6B
#define IC_GET_RFLAGS       0x6C
#define IC_CARRY            0x6D
#define IC_SET_RFLAGS       0x6E
#define IC_GET_RAX          0x6F
#define IC_SET_RAX          0x70
#define IC_GET_RBP          0x71
#define IC_SET_RBP          0x72
#define IC_GET_RSP          0x73
#define IC_SET_RSP          0x74
#define IC_RIP              0x75
#define IC_RDTSC            0x76
#define IC_BT               0x77
#define IC_BTS              0x78
#define IC_BTR              0x79
#define IC_BTC              0x7A
#define IC_LBTS             0x7B
#define IC_LBTR             0x7C
#define IC_LBTC             0x7D
#define IC_BSF              0x7E
#define IC_BSR              0x7F
#define IC_QUE_INIT         0x80
#define IC_QUE_INS          0x81
#define IC_QUE_INS_REV      0x82
#define IC_QUE_REM          0x83
#define IC_STRLEN           0x84
#define IC_BR_ZERO          0x85
#define IC_BR_NOT_ZERO      0x86
#define IC_BR_CARRY         0x87
#define IC_BR_NOT_CARRY     0x88
#define IC_BR_EQU_EQU       0x89
#define IC_BR_NOT_EQU       0x8A
#define IC_BR_LESS          0x8B
#define IC_BR_GREATER_EQU   0x8C
#define IC_BR_GREATER       0x8D
#define IC_BR_LESS_EQU      0x8E
#define IC_BR_EQU_EQU2      0x8F
#define IC_BR_NOT_EQU2      0x90
#define IC_BR_LESS2         0x91
#define IC_BR_GREATER_EQU2  0x92
#define IC_BR_GREATER2      0x93
#define IC_BR_LESS_EQU2     0x94
#define IC_BR_AND_ZERO      0x95
#define IC_BR_AND_NOT_ZERO  0x96
#define IC_BR_MM_ZERO       0x97
#define IC_BR_MM_NOT_ZERO   0x98
#define IC_BR_AND_AND_ZERO  0x99
#define IC_BR_AND_AND_NOT_ZERO 0x9A
#define IC_BR_OR_OR_ZERO    0x9B
#define IC_BR_OR_OR_NOT_ZERO 0x9C
#define IC_BR_BT            0x9D
#define IC_BR_BTS           0x9E
#define IC_BR_BTR           0x9F
#define IC_BR_BTC           0xA0
#define IC_BR_NOT_BT        0xA1
#define IC_BR_NOT_BTS       0xA2
#define IC_BR_NOT_BTR       0xA3
#define IC_BR_NOT_BTC       0xA4
#define IC_SWAP_U8          0xA5
#define IC_SWAP_U16         0xA6
#define IC_SWAP_U32         0xA7
#define IC_SWAP_I64         0xA8
#define IC_ABS_I64          0xA9
#define IC_SIGN_I64         0xAA
#define IC_MIN_I64          0xAB
#define IC_MIN_U64          0xAC
#define IC_MAX_I64          0xAD
#define IC_MAX_U64          0xAE
#define IC_MOD_U64          0xAF
#define IC_SQR_I64          0xB0
#define IC_SQR_U64          0xB1
#define IC_SQR              0xB2
#define IC_ABS              0xB3
#define IC_SQRT             0xB4
#define IC_SIN              0xB5
#define IC_COS              0xB6
#define IC_TAN              0xB7
#define IC_ATAN             0xB8
#define IC_ICS_NUM          0xB9

#define IC_FIRST            0x00
#define IC_LAST             0xB9
#define IC_NUM              (IC_LAST - IC_FIRST + 1)

/*============================================================================
  PRECEDENCE CONSTANTS (PREC_*) — CompilerA.HH
============================================================================*/
#define PREC_NULL           0x00
#define PREC_TERM           0x04
#define PREC_UNARY_POST     0x08
#define PREC_UNARY_PRE      0x0C
#define PREC_EXP            0x10
#define PREC_MUL            0x14
#define PREC_AND            0x18
#define PREC_XOR            0x1C
#define PREC_OR             0x20
#define PREC_ADD            0x24
#define PREC_CMP            0x28
#define PREC_CMP2           0x2C
#define PREC_AND_AND        0x30
#define PREC_XOR_XOR        0x34
#define PREC_OR_OR          0x38
#define PREC_ASSIGN         0x3C
#define PREC_MAX            0x40

#define ASSOCF_LEFT         1
#define ASSOCF_RIGHT        2

/*============================================================================
  KW_* CONSTANTS — CompilerA.HH
============================================================================*/
#define KW_INCLUDE      0
#define KW_DEFINE       1
#define KW_UNION        2
#define KW_CATCH        3
#define KW_CLASS        4
#define KW_TRY          5
#define KW_IF           6
#define KW_ELSE         7
#define KW_FOR          8
#define KW_WHILE        9
#define KW_EXTERN       10
#define KW__EXTERN      11
#define KW_RETURN       12
#define KW_SIZEOF       13
#define KW__INTERN      14
#define KW_DO           15
#define KW_ASM          16
#define KW_GOTO         17
#define KW_EXE          18
#define KW_BREAK        19
#define KW_SWITCH       20
#define KW_START        21
#define KW_END          22
#define KW_CASE         23
#define KW_DFT          24
#define KW_PUBLIC       25
#define KW_OFFSET       26
#define KW_IMPORT       27
#define KW__IMPORT      28
#define KW_IFDEF        29
#define KW_IFNDEF       30
#define KW_IFAOT        31
#define KW_IFJIT        32
#define KW_ENDIF        33
#define KW_ASSERT       34
#define KW_REG          35
#define KW_NOREG        36
#define KW_LASTCLASS    37
#define KW_NO_WARN      38
#define KW_HELP_INDEX   39
#define KW_HELP_FILE    40
#define KW_STATIC       41
#define KW_LOCK         42
#define KW_DEFINED      43
#define KW_INTERRUPT    44
#define KW_HASERRCODE   45
#define KW_ARGPOP       46
#define KW_NOARGPOP     47
#define AKW_ALIGN       64
#define AKW_ORG         65
#define AKW_I0          66
#define AKW_I8          67
#define AKW_I16         68
#define AKW_I32         69
#define AKW_I64         70
#define AKW_U0          71
#define AKW_U8          72
#define AKW_U16         73
#define AKW_U32         74
#define AKW_U64         75
#define AKW_F64         76
#define AKW_DU8         77
#define AKW_DU16        78
#define AKW_DU32        79
#define AKW_DU64        80
#define AKW_DUP         81
#define AKW_USE16       82
#define AKW_USE32       83
#define AKW_USE64       84
#define AKW_IMPORT      85
#define AKW_LIST        86
#define AKW_NOLIST      87
#define AKW_BINFILE     88
#define KW_KWS_NUM      89

/*============================================================================
   TOKEN CONSTANTS
============================================================================*/
#define TK_EOF          0
#define TK_SUPERSCRIPT  0x001
#define TK_SUBSCRIPT    0x002
#define TK_NORMALSCRIPT 0x003
#define TK_IDENT        0x100
#define TK_STR          0x101
#define TK_I64          0x102
#define TK_CHAR_CONST   0x103
#define TK_F64          0x104
#define TK_PLUS_PLUS    0x105
#define TK_MINUS_MINUS  0x106
#define TK_DEREFERENCE  0x107
#define TK_DBL_COLON    0x108
#define TK_SHL          0x109
#define TK_SHR          0x10A
#define TK_EQU_EQU      0x10B
#define TK_NOT_EQU      0x10C
#define TK_LESS_EQU     0x10D
#define TK_GREATER_EQU  0x10E
#define TK_AND_AND      0x10F
#define TK_OR_OR        0x110
#define TK_XOR_XOR      0x111
#define TK_SHL_EQU      0x112
#define TK_SHR_EQU      0x113
#define TK_MUL_EQU      0x114
#define TK_DIV_EQU      0x115
#define TK_AND_EQU      0x116
#define TK_OR_EQU       0x117
#define TK_XOR_EQU      0x118
#define TK_ADD_EQU      0x119
#define TK_SUB_EQU      0x11A
#define TK_IF           0x11B
#define TK_IFDEF        0x11C
#define TK_IFNDEF       0x11D
#define TK_IFAOT        0x11E
#define TK_IFJIT        0x11F
#define TK_ENDIF        0x120
#define TK_ELSE         0x121
#define TK_MOD_EQU      0x122
#define TK_DOT_DOT      0x123
#define TK_ELLIPSIS     0x124
#define TK_INS_BIN      0x125
#define TK_INS_BIN_SIZE 0x126
#define TK_TKS_NUM      0x127

/*============================================================================
   OPCODE / PREFIX CONSTANTS
============================================================================*/
#define OC_OP_SIZE_PREFIX   0x66
#define OC_ADDR_SIZE_PREFIX 0x67
#define OC_LOCK_PREFIX      0xF0
#define OC_NOP              0x90
#define OC_BPT              0xCC
#define OC_CALL             0xE8
#define OC_JMP_REL8         0xEB
#define OC_NOP2             (OC_NOP<<8+OC_OP_SIZE_PREFIX)

#define PUSH_C_REGS PUSH RAX PUSH RCX PUSH RDX PUSH RBX PUSH R8 PUSH R9
#define POP_C_REGS POP R9 POP R8 POP RBX POP RDX POP RCX POP RAX
#define PUSH_REGS PUSH RAX PUSH RCX PUSH RDX PUSH RBX PUSH RBP PUSH RSI \
PUSH RDI PUSH R8 PUSH R9 PUSH R10 PUSH R11 PUSH R12 PUSH R13 PUSH R14 PUSH R15
#define POP_REGS POP R15 POP R14 POP R13 POP R12 POP R11 POP R10 POP R9 \
POP R8 POP RDI POP RSI POP RBP POP RBX POP RDX POP RCX POP RAX

/*============================================================================
   LENGTH CONSTANTS
============================================================================*/
#define STR_LEN          144
#define TASK_NAME_LEN    32
#define PROGRESS_DESC_LEN 48
#define BP_MAX           8
#define MAX_CMD_LEN      260
#define NUM_CPUS         256
#define DEFAULT_STACK_SIZE 0x100000
#define MIN_STACK_SIZE   0x4000
#define AOT_BIN_BLK_SIZE 65536
#define AOT_BIN_BLK_BITS 16

#define IS_0_ARG        0
#define IS_1_ARG        1
#define IS_2_ARG        2
#define IS_V_ARG        3
#define IST_NULL        0
#define IST_DEREF       1
#define IST_ASSIGN      2
#define IST_CMP         3
#define IEF_OP_SIZE16           0x001
#define IEF_OP_SIZE32           0x002
#define IEF_PLUS_OPCODE         0x004
#define IEF_DONT_SWITCH_MODES   0x008
#define IEF_DFT                 0x010
#define IEF_NOT_IN_64_BIT       0x020
#define IEF_48_REX              0x040
#define IEF_REX_ONLY_R8_R15     0x080
#define IEF_REX_XOR_LIKE        0x100
#define IEF_STI_LIKE            0x200
#define IEF_ENDING_ZERO         0x400
#define IEF_IMM_NOT_REL         1
#define IEF_GOTO_LABEL          1

/*============================================================================
   IMPORT/EXPORT TYPES (IET_*) — AOT relocation types (~404)
============================================================================*/
#define IET_END                 0
#define IET_REL_I0              2
#define IET_IMM_U0              3
#define IET_REL_I8              4
#define IET_IMM_U8              5
#define IET_REL_I16             6
#define IET_IMM_U16             7
#define IET_REL_I32             8
#define IET_IMM_U32             9
#define IET_REL_I64             10
#define IET_IMM_I64             11
#define IET_REL32_EXPORT        16
#define IET_IMM32_EXPORT        17
#define IET_REL64_EXPORT        18
#define IET_IMM64_EXPORT        19
#define IET_ABS_ADDR            20
#define IET_CODE_HEAP           21
#define IET_ZEROED_CODE_HEAP    22
#define IET_DATA_HEAP           23
#define IET_ZEROED_DATA_HEAP    24
#define IET_MAIN                25

/*============================================================================
   CMP TEMPLATES — Backend code snippets used by compiler
============================================================================*/
#define CMP_TEMPLATE_INC        0x00
#define CMP_TEMPLATE_DEC        0x01
#define CMP_TEMPLATE_MOD        0x02
#define CMP_TEMPLATE_LESS       0x03
#define CMP_TEMPLATE_GREATER    0x04
#define CMP_TEMPLATE_LESS_EQU   0x05
#define CMP_TEMPLATE_GREATER_EQU 0x06
#define CMP_TEMPLATE_STRLEN     0x07
#define CMP_TEMPLATE_RDTSC      0x08
#define CMP_TEMPLATE_SIGN_I64   0x09
#define CMP_TEMPLATE_FSTP       0x0A
#define CMP_TEMPLATE_FLD        0x0B
#define CMP_TEMPLATE_SQR        0x0C
#define CMP_TEMPLATE_ABS        0x0D
#define CMP_TEMPLATE_SQRT       0x0E
#define CMP_TEMPLATE_SIN        0x0F
#define CMP_TEMPLATE_COS        0x10
#define CMP_TEMPLATE_TAN        0x11
#define CMP_TEMPLATE_ATAN       0x12

extern U8 *cmp_templates[1];
extern U8 *cmp_templates_dont_pop[1];
extern U8 *cmp_templates_dont_push2[1];
extern U8 *cmp_templates_dont_push[1];
extern U8 *cmp_templates_dont_push_pop[1];

/*============================================================================
  FORWARD DECLARATIONS — opaque types
============================================================================*/
typedef struct CDocEntry     CDocEntry;
typedef struct CArrayDim     CArrayDim;
typedef struct CDC           CDC;
typedef struct CDrv          CDrv;
typedef struct CBlkDev       CBlkDev;
typedef struct CCacheBlk     CCacheBlk;
typedef struct CDirContext   CDirContext;
typedef struct CFile         CFile;
typedef struct CHeapCtrl     CHeapCtrl;
typedef struct CBlkPool      CBlkPool;
typedef struct CCtrl         CCtrl;
typedef struct CMenu         CMenu;
typedef struct CTaskSettings CTaskSettings;
typedef struct CMathODE      CMathODE;
typedef struct CBpt          CBpt;
typedef struct CWinScroll    CWinScroll;
typedef struct CTSS          CTSS;
typedef union CAbsCntsI64   CAbsCntsI64;
typedef struct CStreamBlk    CStreamBlk;
typedef struct CAOT          CAOT;
typedef struct CMetaData     CMetaData;

/*============================================================================
  STRUCT: CBlkDevGlbls — block device globals (KernelA.HH ~line 2730)
============================================================================*/
typedef struct CBlkDevGlbls {
    CBlkDev *blkdevs;
    U8 *dft_iso_filename;
    U8 *dft_iso_c_filename;
    U8 *tmp_filename;
    U8 *home_dir;
    CCacheBlk *cache_base, *cache_ctrl, **cache_hash_table;
    I64 cache_size, read_cnt, write_cnt;
    CDrv *drvs, *let_to_drv[32];
    I64 mount_ide_auto_cnt, ins_base0, ins_base1;
    U8 boot_drv_let, first_hd_drv_let, first_dvd_drv_let;
    Bool dvd_boot_is_good, ins_unit, pad[3];
} CBlkDevGlbls;

/*============================================================================
  STRUCT: CHash — base hash table entry (KernelA.HH ~line 639)
============================================================================*/
typedef struct CHash {
    struct CHash *next, *last;
    U8 *str;
    I64 type;
    I64 use_cnt;
    union {
        U8 *data;
        I64 user_data0;
        I64 cnt;
        I64 val;
    };
} CHash;

/*============================================================================
  STRUCT: CHashTable — hash table header (KernelA.HH ~647)
  body[] is a variable-length array of (mask+1) pointers.
============================================================================*/
typedef struct CHashTable {
    struct CHashTable *next, *last;
    I64 mask;
    I64 locked_flags;
    struct CHash *body[1];
} CHashTable;

/*============================================================================
   STRUCT: CArrayDim — array dimension descriptor (~786)
============================================================================*/
typedef struct CArrayDim {
    struct CArrayDim *next, *last;
    I64 cnt;
    I64 total_cnt;
} CArrayDim;

/*============================================================================
   STRUCT: CHashSrcSym — extends CHash with source info (~728)
============================================================================*/
typedef struct CHashSrcSym {
    struct CHashSrcSym *next, *last;
    U8 *str;
    I64 type;
    I64 use_cnt;
    struct CHashSrcSym *src_link;
    I64 idx;
    I64 dbg_info;
    U8 *import_name;
    struct CHashExport *ie_lst;
    /* Derived-type compatibility members (from CHashGlblVar, CHashExport, etc.) */
    I64 user_data0, user_data1, user_data2;
    I64 size;
    I64 flags;
    struct CHashClass *var_class;
    CHashFun *fun_ptr;
    CArrayDim dim;
    union {
        U8 *data;
        U8 *data_addr;
    };
    union {
        I64 val;
        I64 reg_type;
    };
} CHashSrcSym;

/*============================================================================
  STRUCT: CHashGeneric — extends CHash with user data (~737)
============================================================================*/
typedef struct CHashGeneric {
    struct CHashGeneric *next, *last;
    U8 *str;
    I64 type;
    I64 use_cnt;
    I64 user_data0, user_data1, user_data2;
    I64 val;
    I64 reg_type;
} CHashGeneric;

/*============================================================================
  STRUCT: CHashReg — extends CHash with register info (~752)
============================================================================*/
typedef struct CHashReg {
    struct CHashReg *next, *last;
    U8 *str;
    I64 type;
    I64 use_cnt;
    I64 reg_num;
    I64 reg_type;
} CHashReg;

/*============================================================================
  STRUCT: CHashAC — extends CHash with autocomplete stats (~757)
============================================================================*/
typedef struct CHashAC {
    struct CHashAC *next, *last;
    U8 *str;
    I64 type;
    I64 use_cnt;
    I64 num;
    I64 hits;
} CHashAC;

/*============================================================================
  STRUCT: CHashExport — extends CHashSrcSym with val (~763)
============================================================================*/
typedef struct CHashExport {
    struct CHashExport *next, *last;
    U8 *str;
    I64 type;
    I64 use_cnt;
    struct CHashExport *src_link;
    I64 idx;
    I64 dbg_info;
    U8 *import_name;
    struct CHashExport *ie_lst;
    I64 val;
} CHashExport;

/*============================================================================
  STRUCT: CHashImport — extends CHashSrcSym with module info (~768)
============================================================================*/
typedef struct CHashImport {
    struct CHashImport *next, *last;
    U8 *str;
    I64 type;
    I64 use_cnt;
    struct CHashImport *src_link;
    I64 idx;
    I64 dbg_info;
    U8 *import_name;
    struct CHashImport *ie_lst;
    U8 *module_base;
    struct CHashExport *module_header_entry;
} CHashImport;

/*============================================================================
   STRUCT: CMemberLstMeta — member list metadata (~793)
============================================================================*/
typedef struct CMemberLstMeta {
    struct CMemberLstMeta *next;
    U8 *str;
    I64 flags;
    I64 user_data;
} CMemberLstMeta;

/*============================================================================
  STRUCT: CMemberLst — class member list entry (~801)
============================================================================*/
typedef struct CMemberLst {
    struct CMemberLst *next, *last;
    struct CMemberLst *left, *right;
    struct CMemberLst *left_class_base, *right_class_base;
    U8 *str;
    struct CHashClass *member_class;
    struct CMemberLst *member_class_base;
    struct CMemberLstMeta *meta;
    I64 use_cnt;
    I64 flags;
    I64 reg;
    I64 pad;
    I64 offset;
    I64 size;
    CArrayDim dim;
    U8 *static_data;
    union {
        I64 static_data_rip;
        I64 dft_val;
    };
    CHashFun *fun_ptr;
} CMemberLst;

/*============================================================================
  STRUCT: CHashDefineStr — extends CHashSrcSym with string define info (~828)
============================================================================*/
typedef struct CHashDefineStr {
    struct CHashDefineStr *next, *last;
    U8 *str;
    I64 type;
    I64 use_cnt;
    struct CHashDefineStr *src_link;
    I64 idx;
    I64 dbg_info;
    U8 *import_name;
    struct CHashDefineStr *ie_lst;
    U8 *data;
    I64 sub_idx;
    I64 cnt;
} CHashDefineStr;

/*============================================================================
  STRUCT: CHashClass — extends CHashSrcSym with class metadata (~837)
============================================================================*/
typedef struct CHashClass {
    struct CHashClass *next, *last;
    U8 *str;
    I64 type;
    I64 use_cnt;
    struct CHashClass *src_link;
    I64 idx;
    I64 dbg_info;
    U8 *import_name;
    struct CHashClass *ie_lst;
    I64 size;
    I64 neg_offset;
    I64 member_cnt;
    I64 ptr_stars_cnt;
    I64 raw_type;
    I64 flags;
    struct CMemberLst *member_lst_and_root;
    struct CMemberLst *member_class_base_root;
    struct CMemberLst *last_in_member_lst;
    struct CHashClass *base_class;
    struct CHashClass *fwd_class;
    /* Derived-type compatibility (from CHashFun) */
    I64 arg_cnt;
    struct CHashClass *return_class;
} CHashClass;

/*============================================================================
  STRUCT: CHashFun — extends CHashClass with function metadata (~860)
============================================================================*/
typedef struct CHashFun {
    struct CHashFun *next, *last;
    U8 *str;
    I64 type;
    I64 use_cnt;
    struct CHashFun *src_link;
    I64 idx;
    I64 dbg_info;
    U8 *import_name;
    struct CHashFun *ie_lst;
    I64 size;
    I64 neg_offset;
    I64 member_cnt;
    I64 ptr_stars_cnt;
    I64 raw_type;
    I64 flags;
    struct CMemberLst *member_lst_and_root;
    struct CMemberLst *member_class_base_root;
    struct CMemberLst *last_in_member_lst;
    struct CHashClass *base_class;
    struct CHashClass *fwd_class;
    struct CHashClass *return_class;
    I64 arg_cnt;
    I64 pad;
    I64 used_reg_mask;
    I64 clobbered_reg_mask;
    I64 exe_addr;
    struct CHashFun *ext_lst;
} CHashFun;

/*============================================================================
  STRUCT: CHashGlblVar — extends CHashSrcSym with global variable info (~877)
============================================================================*/
typedef struct CHashGlblVar {
    struct CHashGlblVar *next, *last;
    U8 *str;
    I64 type;
    I64 use_cnt;
    struct CHashGlblVar *src_link;
    I64 idx;
    I64 dbg_info;
    U8 *import_name;
    struct CHashGlblVar *ie_lst;
    I64 size;
    I64 flags;
    struct CHashClass *var_class;
    struct CHashFun *fun_ptr;
    CArrayDim dim;
    U8 *data_addr;
    I64 data_addr_rip;
    struct CAOTHeapGlbl *heap_glbl;
} CHashGlblVar;

/*============================================================================
   STRUCT: CInst — instruction template (~1754)
============================================================================*/
typedef struct CInst {
    U8 ins_entry_num;
    U8 opcode_cnt;
    U8 opcode[4];
    U16 flags;
    U8 slash_val;
    U8 uasm_slash_val;
    U8 opcode_modifier;
    U8 arg1;
    U8 arg2;
    U8 size1;
    U8 size2;
    U8 pad;
} CInst;

/*============================================================================
   STRUCT: CHashOpcode — extends CHash with opcode info (~1960)
============================================================================*/
typedef struct CHashOpcode {
    struct CHashOpcode *next, *last;
    U8 *str;
    I64 type;
    I64 use_cnt;
    U16 inst_entry_cnt;
    U16 oc_flags;
    U16 pad[2];
    CInst ins[1];
} CHashOpcode;

/*============================================================================
  CICType — intermediate code type descriptor (~1638)
  In HolyC this is a U16 with raw_type in bits 0-7 and mode in bits 8-15.
============================================================================*/
typedef U16 CICType;

#define CICType_raw(t)      ((U8)((t) & 0xFF))
#define CICType_raw_type(t) ((U8)((t) & 0xFF))
#define CICType_mode(t)     ((U8)(((t) >> 8) & 0xFF))
#define CICType_pack(r,m)   ((CICType)((U8)(r) | ((U16)(U8)(m) << 8)))
#define CICType_set_raw(t,r) ((t) = CICType_pack((r), CICType_mode(t)))
#define CICType_set_mode(t,m) ((t) = CICType_pack(CICType_raw(t), (m)))

/*============================================================================
  STRUCT: CICArg — intermediate code argument (~1643)
============================================================================*/
typedef struct CICArg {
    CICType type;
    U16 reg;
    I64 disp;
} CICArg;

/*============================================================================
  STRUCT: CICTreeLinks — intermediate code tree links (~1650)
============================================================================*/
typedef struct CICTreeLinks {
    struct CHashClass *arg1_class, *arg2_class;
    struct CIntermediateCode *arg1_tree, *arg2_tree;
    struct CHashClass *class2;
} CICTreeLinks;

/*============================================================================
  STRUCT: CIntermediateCodeBase — base IC node (~1657)
============================================================================*/
typedef struct CIntermediateCodeBase {
    struct CIntermediateCode *next, *last;
    U16 ic_code;
    U16 ic_precedence;
    I16 ic_cnt;
    I16 ic_last_start;
} CIntermediateCodeBase;

/*============================================================================
  STRUCT: CIntermediateCode — full IC node (~1666)
  ic_body[IC_BODY_SIZE] overlays CICTreeLinks t in TempleOS union.
============================================================================*/
typedef struct CIntermediateCode {
    struct CIntermediateCode *next, *last;
    U16 ic_code;
    U16 ic_precedence;
    I16 ic_cnt;
    I16 ic_last_start;
    I64 ic_flags;
    I64 ic_data;
    I64 ic_line;
    struct CHashClass *ic_class;
    struct CHashClass *ic_class2;
    CICArg arg1, arg2, res;
    CICType arg1_type_pointed_to;
    union {
        I64 ic_body[IC_BODY_SIZE];
        CICTreeLinks t;
    };
} CIntermediateCode;

/*============================================================================
   STRUCT: CCodeCtrl — code control block (~2027)
============================================================================*/
typedef struct CCodeCtrl {
    struct CCodeCtrl *coc_next;
    struct CCodeMisc *coc_next_misc, *coc_last_misc;
    CIntermediateCodeBase coc_head;
} CCodeCtrl;

/*============================================================================
  STRUCT: CCodeMisc — code misc info (~1715)
============================================================================*/
typedef struct CCodeMisc {
    struct CCodeMisc *next, *last, *fwd, *dft, *begin;
    U8 *str;
    U32 type;
    U32 flags;
    I64 use_cnt;
    I64 addr;
    union {
        I64 st_len;
        I64 num_consts;
        I64 range;
        I64 rip;
    };
    union {
        struct CCodeMisc **jmp_table;
        F64 *float_consts;
        struct CArrayDim *dim;
        struct CHash *h;
    };
} CCodeMisc;

/*============================================================================
  STRUCT: CSubSwitch — switch sub-block
============================================================================*/
typedef struct CSubSwitch {
    struct CSubSwitch *next, *last;
    CCodeMisc *lb_start, *lb_break;
} CSubSwitch;

/*============================================================================
  STRUCT: CSwitchCase — switch case entry
============================================================================*/
typedef struct CSwitchCase {
    struct CSwitchCase *next;
    CCodeMisc *label;
    I64 val;
    CSubSwitch *ss;
} CSwitchCase;

typedef struct CIntermediateStruct {
    U8 arg_cnt, res_cnt, type;
    U8 fpop, not_const, pad[3];
    U8 *name;
} CIntermediateStruct;

typedef struct COptReg {
    I64 offset;
    struct CMemberLst *m;
} COptReg;

typedef struct CDbgInfo {
    U32 min_line, max_line;
    U32 body[1];
} CDbgInfo;

/*============================================================================
  STRUCT: CPrsStk — parser stack (~1689)
============================================================================*/
typedef struct CPrsStk {
    I64 ptr;
    I64 stk[255];
    I64 ptr2;
    I64 stk2[255];
} CPrsStk;

/*============================================================================
   STRUCT: CAsmUndefHash — unresolved asm reference (~1823)
============================================================================*/
typedef struct CAsmUndefHash {
    struct CAsmUndefHash *next, *last;
    struct CHashExport *hash;
} CAsmUndefHash;

/*============================================================================
   STRUCT: CAbsCntsI64 — absolute address counters (~1816)
============================================================================*/
typedef union CAbsCntsI64 {
    struct {
        U16 abs_addres;
        U16 c_addres;
        U32 externs;
    };
    I64 raw;
} CAbsCntsI64;

/*============================================================================
   STRUCT: CAsmNum — assembler numeric value (~1829)
============================================================================*/
typedef struct CAsmNum {
    I64 i;
    U8 *machine_code;
    struct CAsmUndefHash *local_asm_undef_hash;
    struct CAsmUndefHash *glbl_asm_undef_hash;
    CAbsCntsI64 abs_cnts;
} CAsmNum;

/*============================================================================
  STRUCT: CAsmNum2 — assembler num with relocation flags (~1837)
============================================================================*/
typedef struct CAsmNum2 {
    CAsmNum num;
    I64 U8_cnt;
    I64 rel;
    I64 imm_flag;
} CAsmNum2;

/*============================================================================
  STRUCT: CAsmIns — assembled instruction info (~1844)
============================================================================*/
typedef struct CAsmIns {
    I64 tmpins;
    CAsmNum2 imm;
    CAsmNum2 disp;
    I64 U8_cnt;
    U8 last_opcode_U8;
    U8 REX;
    U8 ModrM;
    U8 SIB;
    U8 has_REX;
    U8 has_ModrM;
    U8 has_SIB;
    U8 has_addr_prefix;
    U8 has_operand_prefix;
    U8 is_dft;
    U8 pad;
} CAsmIns;

/*============================================================================
   STRUCT: CAsmArg — assembler argument (~1856)
============================================================================*/
typedef struct CAsmArg {
    CAsmNum num;
    I64 seg;
    I64 size;
    I64 reg1;
    I64 reg2;
    I64 reg1_type;
    I64 reg2_type;
    I64 scale;
    Bool indirect;
    Bool imm_or_off_present;
    Bool just_seg;
    Bool pad[5];
} CAsmArg;

/*============================================================================
  STRUCT: CAOTBinBlk — AOT binary block (~1810)
============================================================================*/
typedef struct CAOTBinBlk {
    struct CAOTBinBlk *next, *last;
    U8 body[AOT_BIN_BLK_SIZE];
} CAOTBinBlk;

/*============================================================================
   STRUCT: CAOTCtrl — AOT compiler control (~2061)
============================================================================*/
typedef struct CAOTCtrl {
    I64 rip;
    CAsmArg arg1, arg2;
    struct CAOTBinBlk *bin;
    I64 num_bin_U8s;
    I64 max_align_bits;
    I64 org;
    struct CAsmUnresolvedRef *local_unresolved, *glbl_unresolved;
    struct CAOTAbsAddr *abss;
    struct CAOTHeapGlbl *heap_glbls;
    I64 lst_col;
    I64 lst_last_rip;
    U8 *last_label;
    U8 *lst_last_line;
    struct CLexFile *lst_last_lfn;
    I64 seg_size;
    Bool lst;
} CAOTCtrl;

/*============================================================================
  STRUCT: CLexFile — lexer file context (~2047)
============================================================================*/
typedef struct CLexFile {
    struct CLexFile *next, *last;
    U8 *buf;
    U8 **buf_ptr;
    I64 line_num;
    I64 flags;
    U8 *full_name;
    U8 *line_start;
    struct CDoc *doc;
    struct CDocEntry *cur_entry;
    I32 depth;
    I16 last_U16;
    I16 pad;
} CLexFile;

/*============================================================================
  STRUCT: CLexHashTableContext — lexer hash context (~2124)
============================================================================*/
typedef struct CLexHashTableContext {
    struct CLexHashTableContext *next, *last;
    I64 old_flags;
    I64 hash_mask;
    struct CHashGeneric *local_var_lst;
    struct CHashFun *fun;
    struct CHashTable *hash_table_lst;
    struct CHashTable *define_hash_table;
    struct CHashTable *local_hash_table;
    struct CHashTable *glbl_hash_table;
} CLexHashTableContext;

/*============================================================================
  STRUCT: CCmpCtrl — compiler control block (~2179)
============================================================================*/
typedef struct CCmpCtrl {
    struct CCmpCtrl *next, *last;
    I64 token;
    I64 flags;
    I64 cur_i64;
    F64 cur_f64;
    U8 *cur_str;
    I64 cur_str_len;
    I64 class_dol_offset;
    U8 *dollar_buf;
    I64 dollar_cnt;
    I64 cur_help_idx;
    I16 last_U16;
    I16 pad1;
    I64 min_line, max_line, last_line_num;
    I64 lock_cnt;
    U32 *char_bmp_alpha_numeric;
    CLexHashTableContext htc;
    struct CHashGeneric *hash_entry;
    CAbsCntsI64 abs_cnts;
    struct CAsmUndefHash *asm_undef_hash;
    struct CHashGeneric *local_var_entry;
    struct CIntermediateCode *lb_leave;
    U8 *cur_buf_ptr;
    struct CLexFile *lex_include_stk;
    struct CLexFile *lex_prs_stk;
    struct CLexFile *fun_lex_file;
    struct CStreamBlk *next_stream_blk;
    struct CStreamBlk *last_stream_blk;
    struct CAOT *aot;
    I64 pass;
    I64 opts;
    I64 pass_trace;
    I64 saved_pass_trace;
    I64 error_cnt;
    I64 warning_cnt;
    I64 cur_ic_float_op_num;
    I64 last_ic_float_op_num;
    struct CIntermediateCode *last_float_op_ic;
    I64 last_dont_pushable;
    I64 last_dont_popable;
    I64 last_popable;
    I64 last_float_op_pos;
    I64 dont_push_float;
    struct CCodeCtrl coc;
    struct CPrsStk *ps;
    struct CAOTCtrl *aotc;
    I64 aot_depth;
    I64 pmt_line;
} CCmpCtrl;

/*============================================================================
  STRUCT: CCmpGlbls — compiler globals (~2226)
============================================================================*/
typedef struct CCmpGlbls {
    struct CHashTable *asm_hash;
    struct CHashClass *internal_types[RT_RTS_NUM];
    struct CIntermediateCode ic_nop;
    U32 *dual_U16_tokens1;
    U32 *dual_U16_tokens2;
    U32 *dual_U16_tokens3;
    U32 *binary_ops;
    I64 num_reg_vars;
    I64 num_non_ptr_vars;
    I64 stk_tmps_mask;
    I64 reg_vars_mask;
    I64 non_ptr_vars_mask;
    U8 *to_reg_vars_map;
    U8 *non_ptr_vars_map;
    I64 size_arg_mask[9];
    I64 compiled_lines;
} CCmpGlbls;

/*============================================================================
   STRUCT: CUAsmGlbls — assembler globals (KernelA.HH ~line 2400)
============================================================================*/
typedef struct CUAsmGlbls {
    CInst **table_16_32;
    CInst **table_64;
    I64 table_16_32_entries;
    I64 table_64_entries;
    I64 ins64_arg_mask;
    I64 signed_arg_mask;
    I64 mem_arg_mask;
} CUAsmGlbls;

/*============================================================================
  STRUCT: CExcept — exception handler context (~3116)
============================================================================*/
typedef struct CExcept {
    struct CExcept *next, *last;
    U8 *hndlr_catch;
    U8 *hndlr_untry;
    I64 rsp, rbp, rflags, rsi, rdi;
    I64 r10, r11, r12, r13, r14, r15;
} CExcept;

/*============================================================================
  STRUCT: CFPU — FPU/MMX save area (~3123)
============================================================================*/
typedef struct CFPU {
    U8 body[512];
} CFPU;

/*============================================================================
  STRUCT: CJob — job/servicer descriptor (~3147)
============================================================================*/
typedef struct CJob {
    struct CJob *next, *last;
    struct CJobCtrl *ctrl;
    I64 job_code;
    I64 flags;
    I64 msg_code;
    I64 (*addr)(void);
    I64 fun_arg;
    U8 *aux_str;
    I64 aux1, aux2;
    I64 res;
    struct CTask *spawned_task;
    struct CTask *master_task;
} CJob;

/*============================================================================
  STRUCT: CJobCtrl — job control block (~3165)
============================================================================*/
typedef struct CJobCtrl {
    struct CJob *next_waiting, *last_waiting;
    struct CJob *next_done, *last_done;
    I64 flags;
} CJobCtrl;

/*============================================================================
  STRUCT: CTaskStk — task stack descriptor (~3257)
============================================================================*/
typedef struct CTaskStk {
    struct CTaskStk *next_stk, *last_stk;
    I64 stk_size;
    U8 *stk_ptr;
    U8 *stk_base;
} CTaskStk;

/*============================================================================
  STRUCT: CTaskDying — dying task descriptor (~3265)
============================================================================*/
typedef struct CTaskDying {
    struct CTaskDying *next, *last;
    I64 wake_jiffy;
} CTaskDying;

typedef struct CWinScroll {
    I64 xmin, xmax, pos;
} CWinScroll;

/*============================================================================
  STRUCT: CTask — the full task control block (~3271)
============================================================================*/
typedef struct CTask {
    struct CTask *next, *last;
    U32 task_signature;
    U32 win_inhibit;
    I64 wake_jiffy;
    U32 task_flags;
    U32 display_flags;
    struct CHeapCtrl *code_heap;
    struct CHeapCtrl *data_heap;
    struct CDoc *put_doc;
    struct CDoc *display_doc;
    struct CDoc *border_doc;
    I64 win_left, win_right, win_top, win_bottom;
    struct CDrv *cur_dv;
    U8 *cur_dir;
    struct CTask *parent_task;
    struct CTask *next_task, *last_task;
    struct CTask *next_input_filter_task;
    I64 win_width, win_height;
    I64 pix_left, pix_right, pix_width;
    I64 pix_top, pix_bottom, pix_height;
    I64 scroll_x, scroll_y, scroll_z;
    I64 rip, rflags, rsp, rsi;
    I64 rax, rcx, rdx, rbx, rbp, rdi;
    I64 r8, r9, r10, r11, r12, r13, r14, r15;
    struct CCPU *gs;
    struct CFPU *fpu_mmx;
    I64 swap_cnter;
    I64 (*draw_it)(void);
    U8 task_title[STR_LEN];
    U8 task_name[TASK_NAME_LEN];
    I64 wallpaper_data[STR_LEN];
    U8 title_src, border_src;
    U8 text_attr, border_attr;
    U16 win_z_num;
    I64 pad3;
    struct CTaskStk *stk;
    struct CExcept *next_except, *last_except;
    I64 except_rbp;
    I64 except_ch;
    I64 except_callers[BP_MAX];
    Bool catch_except;
    Bool new_answer;
    I64 answer_type;
    U8 pad4[5];
    I64 answer;
    F64 answer_time;
    struct CBpt *bpt_lst;
    struct CCtrl *next_ctrl, *last_ctrl;
    struct CMenu *cur_menu;
    struct CTaskSettings *next_settings;
    struct CMathODE *next_ode, *last_ode;
    I64 last_ode_time;
    struct CHashTable *hash_table;
    CJobCtrl srv_ctrl;
    struct CCmpCtrl *next_cc, *last_cc;
    struct CHashFun *last_fun;
    I64 (*task_end_cb)(void);
    struct CTask *song_task;
    struct CTask *animate_task;
    I64 rand_seed;
    I64 task_num;
    I64 fault_num, fault_err_code;
    struct CTask *popup_task, *dbg_task;
    struct CWinScroll horz_scroll, vert_scroll;
    I64 user_data;
} CTask;

/*============================================================================
  STRUCT: CCPU — per-CPU state (~3391)
============================================================================*/
typedef struct CCPU {
    struct CCPU *next, *last;
    I64 addr;
    I64 num;
    I64 cpu_flags;
    I64 startup_rip;
    I64 idle_pt_hits;
    F64 idle_factor;
    I64 total_jiffies;
    struct CTask *seth_task;
    struct CTask *idle_task;
    I64 tr;
    I64 swap_cnter;
    I64 (*profiler_timer_irq)(void);
    struct CTaskDying *next_dying, *last_dying;
    I64 kill_jiffy;
    struct CTSS *tss;
    I64 start_stk[16];
} CCPU;

/*============================================================================
   PLACEHOLDER STRUCTS — opaque types that the compiler references
============================================================================*/
struct CDC           { I64 dummy; };
struct CDrv          { I64 dummy; };
struct CDirContext   { I64 dummy; };
struct CFile         { I64 dummy; };
struct CHeapCtrl     { I64 dummy; };
struct CBlkPool      { I64 dummy; };
struct CCtrl         { I64 dummy; };
struct CMenu         { I64 dummy; };
struct CTaskSettings { I64 dummy; };
struct CMathODE      { I64 dummy; };
struct CBpt          { I64 dummy; };
struct CTSS          { I64 dummy; };
struct CStreamBlk    { U8 *body; };
struct CMetaData     { I64 dummy; };

/*============================================================================
   STRUCT: CDocBin — document binary data block (~1118)
============================================================================*/
typedef struct CDocBin {
    struct CDocBin *next, *last;
    I32 tmp_use_cnt, renum_num;
    U8 *tag;
    U32 num, flags, size, use_cnt;
    U8 *data;
} CDocBin;

/*============================================================================
   STRUCT: CDocSettings — document rendering settings (~1157)
============================================================================*/
typedef struct CDocSettings {
    U32 final_u32_attr;
    I16 left_margin, right_margin, indent;
    U16 page_len, header, footer;
    I8 shifted_x, shifted_y;
    U8 state, comment_depth, paren_depth, brace_depth;
    U8 cur_text_attr, dft_text_attr;
} CDocSettings;

/*============================================================================
   STRUCT: CDocEntryBase — base doc entry (~1169)
============================================================================*/
typedef struct CDocEntryBase {
    struct CDocEntryBase *next, *last;
    U8 *tag;
    union {
        U8 type_u8;
        U32 type;
    };
    I32 page_line_num;
    I64 de_flags;
    I32 x, y;
    U32 min_col, max_col;
    CDocSettings settings;
    I64 user_data;
} CDocEntryBase;

/*============================================================================
   STRUCT: CDoc — document (minimal definition for compiler)
============================================================================*/
typedef struct CEdFileName {
    U8 name[256];
} CEdFileName;

typedef struct CDoc {
    struct CDoc *next, *last;
    CDocEntryBase head;
    I64 flags;
    struct CDocEntry *head_ptr;
    CDocBin bin_head;
    I64 cur_bin_num;
    CEdFileName filename;
} CDoc;

/*============================================================================
   STRUCT: CDocEntry — full doc entry (~1191)
============================================================================*/
typedef struct CDocEntry {
    struct CDocEntryBase *next, *last;
    U8 *tag;
    union {
        U8 type_u8;
        U32 type;
    };
    I32 page_line_num;
    I64 de_flags;
    I32 x, y;
    U32 min_col, max_col;
    CDocSettings settings;
    I64 user_data;

    union {
        I64 attr;
        I64 cursor_x_offset;
        I64 (*left_cb)(CDoc *doc, struct CDocEntry *doc_e);
        I64 left_exp;
    };
    U8 *left_macro;
    union {
        I64 cursor_y_offset;
        I64 (*right_cb)(CDoc *doc, struct CDocEntry *doc_e);
        I64 right_exp;
    };
    U8 *right_macro;
    U8 *(*tag_cb)(CDoc *doc, struct CDocEntry *doc_e, struct CTask *mem_task);
    U8 *define_str;
    U8 *aux_str;
    U8 *bin_ptr_link;
    U8 *html_link;
    U8 *my_fmt_data;
    I64 hex_ed_width;
    I32 scroll_len;
    I32 len;
    I32 bin_num;
    U8 raw_type;
    U8 pad[3];
    CDocBin *bin_data;
    U8 *data;
} CDocEntry;

/*============================================================================
   STRUCT: CAsmUnresolvedRef — unresolved asm reference (~1866)
============================================================================*/
typedef struct CAsmUnresolvedRef {
    struct CAsmUnresolvedRef *next;
    I64 type, line_num;
    U8 *machine_code;
    I64 rip, rel_rip;
    struct CAOT *aot;
    U8 *str;
    struct CAsmUndefHash *asm_undef_hash;
    Bool U8_avail;
    Bool imm_flag;
} CAsmUnresolvedRef;

/*============================================================================
   STRUCT: CAOTImportExport — AOT import/export entry (~1969)
============================================================================*/
typedef struct CAOTImportExport {
    struct CAOTImportExport *next, *last;
    I64 rip, flags;
    struct CAOT *aot;
    U8 *str;
    U8 *src_link;
    U8 type;
    U8 pad[7];
} CAOTImportExport;

/*============================================================================
   STRUCT: CAOTAbsAddr — AOT absolute address patch (~1987)
============================================================================*/
typedef struct CAOTAbsAddr {
    struct CAOTAbsAddr *next;
    I64 rip;
    U8 type;
    U8 pad[7];
} CAOTAbsAddr;

/*============================================================================
   STRUCT: CAOTHeapGlblRef — AOT heap global reference (~1994)
============================================================================*/
typedef struct CAOTHeapGlblRef {
    struct CAOTHeapGlblRef *next;
    I64 rip;
} CAOTHeapGlblRef;

/*============================================================================
   STRUCT: CAOTHeapGlbl — AOT heap global (~2000)
============================================================================*/
typedef struct CAOTHeapGlbl {
    struct CAOTHeapGlbl *next;
    U8 *str;
    I64 size;
    CAOTHeapGlblRef *references;
} CAOTHeapGlbl;

/*============================================================================
   STRUCT: CExternUsage — external symbol usage for AOT fixups
============================================================================*/
typedef struct CExternUsage {
    struct CExternUsage *next;
    I64 rip;
} CExternUsage;

/*============================================================================
   STRUCT: CBinFile — binary file header for AOT output
============================================================================*/
typedef struct CBinFile {
    U16 jmp;
    U16 reserved;
    U32 bin_signature;
    I64 org;
    I64 module_align_bits;
    I64 patch_table_offset;
    I64 file_size;
} CBinFile;

/*============================================================================
   STRUCT: CAOT — Ahead-of-Time compiled unit (~2008)
============================================================================*/
typedef struct CAOT {
    struct CAOT *next, *last;
    U8 *buf;
    I64 rip, rip2;
    I64 aot_U8s;
    I64 max_align_bits, org;
    struct CAOT *parent_aot;
    CAOTImportExport *next_ie, *last_ie;
    CAOTAbsAddr *abss;
    CAOTHeapGlbl *heap_glbls;
} CAOT;

/*============================================================================
  GLOBALS — task and CPU pointers (Fs()/Gs() macros)
============================================================================*/
extern struct CCmpGlbls cmp;
extern struct CUAsmGlbls uasm;
extern struct CTask *__holyc_fs;
extern struct CCPU  *__holyc_gs;

#define Fs() (__holyc_fs)
#define Gs() (__holyc_gs)

extern CHashDefineStr *DefineLstLoad(U8 *dname, U8 *lst);
extern void HashDel(CHash *h);
extern I64 HashTypeNum(CHash *h);
extern void HashSrcFileSet(void *cc, void *hash, I64 line);
extern CHash *HashSingleTableFind(U8 *key, CHashTable *ht, I64 type, I64 flags);
extern void HashAddAfter(CHash *after, CHash *entry);
extern void HashTableDel(CHashTable *ht);

extern void LinkedLstDel(void *lst);
extern I64 LinkedLstSize(void *lst);
extern void QueDel(void *q);
extern void QueVectU8Put(U8 **v, I64 idx, U8 val);
extern U8 QueVectU8Get(U8 **v, I64 idx);
extern void QueVectU8Del(U8 **v);

/*============================================================================
   STUB CONSTANTS — Missing values from Adam layer and other headers
   These are placeholder values to allow compilation. Correct values
   should be extracted from TempleOS source and verified.
============================================================================*/

/* Member list flags (MLF_*) */
#define MLF_DFT_AVAILABLE       0x0001
#define MLF_STR_DFT_AVAILABLE   0x0002
#define MLF_FUN                 0x0004
#define MLF_NO_UNUSED_WARN      0x0008

/* Member list meta flags (MLMF_*) */
#define MLMF_IS_STR             0x0001

/* Parser flags (PRS0_*, PRS1_*, PRS1B_*) */
#define PRS0_NULL               0
#define PRS0_EXTERN             1
#define PRS0__EXTERN            2
#define PRS0_IMPORT             3
#define PRS0__IMPORT            4
#define PRS0__INTERN            5
#define PRS0_TYPECAST           0x000006
#define PRS1_NULL               0
#define PRS1B_NULL              0
#define PRS1_LOCAL_VAR          0x000100
#define PRS1B_LOCAL_VAR         1
#define PRS1_FUN_ARG            0x000200
#define PRS1B_FUN_ARG           2
#define PRS1_CLASS              0x000300
#define PRS1B_CLASS             PRS1_CLASS
#define PRS1_STATIC_LOCAL_VAR   0x000400
#define PRS1B_STATIC_LOCAL_VAR  PRS1_STATIC_LOCAL_VAR
#define PRS1_NOT_REALLY__EXTERN 0x000500
#define PRSF_UNION              0x0001

/* Function flags masks */
#define FSG_FUN_FLAGS1 (FSF_INTERRUPT|FSF_HASERRCODE|FSF_ARGPOP|FSF_NOARGPOP)
#define FSG_FUN_FLAGS2 (FSG_FUN_FLAGS1|FSF_PUBLIC)

/* Precedence associativity mask */
#define ASSOC_MASK              3

/* Operand mode constants */
#define OM_CB 1
#define OM_CW 2
#define OM_CD 3
#define OM_IB 5
#define OM_IW 6
#define OM_ID 7

/* Binary file signature */
#define BIN_SIGNATURE_VAL       'TOSB'

/* Log constants */
#define log2_10 3.321928094887362
#define log2_e  1.442695040888963
#define log10_2 0.301029995663981
#define loge_2  0.693147180559945

/* Alias constants */
#define HTt_EXPORT_SYS_SYM  HTT_EXPORT_SYS_SYM
#define HTt_KEYWORD         HTT_KEYWORD
#define CCf_PAREN           CCF_PAREN
#define CCf_FUN_EXP         CCF_FUN_EXP
#define ICf_LOCK            ICF_LOCK
#define HTf_PUBLIC          HTF_PUBLIC
#define HTf_UNRESOLVED      HTF_UNRESOLVED

/* Optimization flags (OPTF_* and OPTf_* aliases) */
#define OPTF_WARN_UNUSED_VAR        0
#define OPTF_WARN_HEADER_MISMATCH   1
#define OPTF_ECHO                   2
#define OPTF_TRACE                  3
#define OPTF_GLBLS_ON_DATA_HEAP     4
#define OPTf_WARN_UNUSED_VAR        OPTF_WARN_UNUSED_VAR
#define OPTf_WARN_HEADER_MISMATCH   OPTF_WARN_HEADER_MISMATCH
#define OPTf_ECHO                   OPTF_ECHO
#define OPTf_TRACE                  OPTF_TRACE
#define OPTf_GLBLS_ON_DATA_HEAP     OPTF_GLBLS_ON_DATA_HEAP

/* AAT (AOT Arithmetic Type) constants */
#define AAT_ADD_U8              0
#define AAT_ADD_U16             1
#define AAT_ADD_U32             2
#define AAT_ADD_U64             3
#define AAT_SUB_U8              4
#define AAT_SUB_U16             5
#define AAT_SUB_U32             6
#define AAT_SUB_U64             7

/* IC generation flags (ICG_*) */
#define ICG_NO_CVT_MASK         0xFFFF

/* Register displacement modes (MDG_*) */
#define MDG_REG_DISP_SIB        0x0001

/* SV (Stack Variable) flags */
#define SV_NONE                 0
#define SV_R_REG                1
#define SV_I_REG                2
#define SV_STI_LIKE             3

/* RTG (Raw Type Group) mask */
#define RTG_MASK                0x00FF

/* Pointer stars count */
#define PTR_STARS_NUM           8

/* U16/U32 max values */
#define U16_MAX                 0xFFFF
#define U32_MAX                 0xFFFFFFFF

/* CCf (Cmp Ctrl flags) - note: CCF_ are different */
/* CCf_USE_LAST_U16 must match bit index of CCF_USE_LAST_U16 (0x200000000 = bit 33) */
#define CCf_USE_LAST_U16        33

/* DOCT (Document Entry Types) */
#define DOCT_TEXT               0
#define DOCT_NEW_LINE           1
#define DOCT_TAB                2
#define DOCT_INS_BIN            3
#define DOCT_INS_BIN_SIZE       4
#define DOCT_SHIFTED_Y          5
#define DOCT_MARKER             6
#define DOCT_CURSOR             7
#define DOCT_TYPES_NUM          8

#define DOCF_DBL_DOLLARS        0x01

/* DOCEF (Document Entry Flags) */
#define DOCEF_TAG               0x0001
#define DOCEF_DEFINE            0x0002
#define DOCEF_TAG_CB            0x0004
#define DOCEF_WORD_WRAP         0x0008
#define DOCEF_HIGHLIGHT         0x0010
#define DOCEF_SKIP              0x0020
#define DOCEF_FILTER_SKIP       0x0040

/* DOCG (Document Group) flags */
#define DOCG_BL_IV_UL           0x0001

/* CH (Character constants) */
#define CH_CURSOR               0x7F
#define CH_SHIFT_SPACE          0x80

/* Missing types */
typedef struct CQueVectU8 CQueVectU8;

/* Missing globals */
extern U8 *char_bmp_alpha_numeric;
extern U8 *char_bmp_alpha_numeric_no_at;
extern U8 *char_bmp_printable;
extern U8 *char_bmp_non_eol;
extern U8 char_bmp_displayable[16];
extern CBlkDevGlbls blkdev;
extern I64 sys_var_init_val;
extern I64 sys_var_init_flag;

/* CHashClass member - arg_cnt is in CHashFun, not CHashClass */
/* We add it here as a compatibility hack since some code expects it */
#ifndef CHashClass_arg_cnt_hack
#define CHashClass_arg_cnt_hack
#endif

/* CHashSrcSym - user_data0 is in CHashGeneric */
#ifndef CHashSrcSym_user_data0_hack
#define CHashSrcSym_user_data0_hack
#endif

/* Missing lb_break member - likely in a label/switch structure */
#ifndef lb_break_hack
#define lb_break_hack
#endif

/* CStreamBlk body member */
#ifndef CStreamBlk_body_hack
#define CStreamBlk_body_hack
#endif

extern U8 *char_bmp_hex_numeric;
extern U8 *char_bmp_dec_numeric;
extern U8 *char_bmp_non_eol_white_space;
extern U8 *mem_boot_base;

#define HTt_DEFINE_STR      HTT_DEFINE_STR
#define HTt_INTERNAL_TYPE   HTT_INTERNAL_TYPE
#define HTt_FILE            HTT_FILE
#define HTt_MODULE          HTT_MODULE
#define HTt_WORD            HTT_WORD
#define RLf_DOC             1
#define DOCF_NO_CURSOR      0x02
#define CM_CONSTS_NUM       256

extern void *QueVectU8New(void);
extern void *adam_task;

#define CHash_data(h)       ((CHashSrcSym*)(h))->data
#define CHash_user_data0(h) ((CHashGeneric*)(h))->user_data0
#define CHash_cnt(h)        ((CHashDefineStr*)(h))->cnt

#endif /* _HOLYC_COMPILER_TYPES_H */
