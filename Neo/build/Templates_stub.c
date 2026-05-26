/* Templates_stub.c -- stub for Templates.HC (inline asm data tables)
   Provides the CMP_TEMPLATES lookup tables used by the assembler */

#include "holyc.h"
#include "holyc_compiler_types.h"

/* CMP_TEMPLATES -- 19 template types with instruction entry arrays.
   Each entry is a 3-byte record: [opcode_prefix, opcode, oc_flags]
   In HolyC, these are __extern LABEL at asm labels.
   We define them as minimal valid tables so the assembler doesn't crash. */

/* IC_END */
U8 CMP_TEMPLATES_00[] = { 0x00, 0x00, 0x00 };
/* IC_MOV_REG */
U8 CMP_TEMPLATES_01[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED2 */
U8 CMP_TEMPLATES_02[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED3 */
U8 CMP_TEMPLATES_03[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED4 */
U8 CMP_TEMPLATES_04[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED5 */
U8 CMP_TEMPLATES_05[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED6 */
U8 CMP_TEMPLATES_06[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED7 */
U8 CMP_TEMPLATES_07[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED8 */
U8 CMP_TEMPLATES_08[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED9 */
U8 CMP_TEMPLATES_09[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED10 */
U8 CMP_TEMPLATES_10[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED11 */
U8 CMP_TEMPLATES_11[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED12 */
U8 CMP_TEMPLATES_12[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED13 */
U8 CMP_TEMPLATES_13[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED14 */
U8 CMP_TEMPLATES_14[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED15 */
U8 CMP_TEMPLATES_15[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED16 */
U8 CMP_TEMPLATES_16[] = { 0x00, 0x00, 0x00 };
/* IC_UNUSED17 */
U8 CMP_TEMPLATES_17[] = { 0x00, 0x00, 0x00 };
/* IC_MOV_IMM */
U8 CMP_TEMPLATES_18[] = { 0x00, 0x00, 0x00 };

/* Master template pointer arrays */
U8 *cmp_templates[1] = { &CMP_TEMPLATES_00 };
U8 *cmp_templates_dont_pop[1] = { &CMP_TEMPLATES_00 };
U8 *cmp_templates_dont_push2[1] = { &CMP_TEMPLATES_00 };
U8 *cmp_templates_dont_push[1] = { &CMP_TEMPLATES_00 };
U8 *cmp_templates_dont_push_pop[1] = { &CMP_TEMPLATES_00 };