#include "holyc.h"
#include "holyc_compiler_types.h"
#include <sched.h>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>

jmp_buf __holyc_exception_buf;
I64    __holyc_exception_code;
U8    *__holyc_exception_msg;

/* Globals defined in compiler.c (Compiler.PRJ): */
extern struct CTask *__holyc_fs;
extern struct CCPU  *__holyc_gs;
extern struct CCmpGlbls cmp;
extern struct CUAsmGlbls uasm;

/* Declared in stubs — must be extern or defined here */
extern I64 CMP_TEMPLATES;
extern I64 _LAST_FUN(I64 argc, I64 *argv);

I64 sys_var_init_flag = 0;
I64 sys_var_init_val = 0;
extern U8 *cmp_templates[1];
extern U8 *cmp_templates_dont_push2[1];
extern U8 *cmp_templates_dont_push_pop[1];
extern U8 *cmp_templates_dont_push[1];
extern U8 *cmp_templates_dont_pop[1];

CBlkDevGlbls blkdev;
U8 *char_bmp_non_eol = NULL;

static U32 char_bmp_non_eol_data[16] = {
    0xFFFFDBFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
    0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
    0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
    0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF
};

static void init_char_bitmaps(void) {
    char_bmp_non_eol = (U8*)char_bmp_non_eol_data;
}

static void __attribute__((noreturn)) __holyc_fatal_exception(I64 ch) {
    void *bt[32];
    int n = backtrace(bt, 32);
    Print("Fatal exception: %c (0x%lX) outside any try block\n", (int)(ch & 0xFF), ch);
    backtrace_symbols_fd(bt, n, 2);
    _exit(1);
}

void holyc_runtime_init(void) {
    static CTask dummy_task;
    static CCPU  dummy_cpu;
    static CTaskStk dummy_stk;
    static CHashTable *task_hash_table = NULL;
    if (!task_hash_table) task_hash_table = HashTableNew(16);

    init_char_bitmaps();

    if (setjmp(__holyc_exception_buf) != 0) {
        __holyc_fatal_exception(__holyc_exception_code);
    }

    MemSet(&dummy_task, 0, sizeof(CTask));
    MemSet(&dummy_cpu, 0, sizeof(CCPU));
    QueInit(&dummy_task);
    QueInit(&dummy_cpu);
    dummy_stk.stk_size  = DEFAULT_STACK_SIZE;
    dummy_task.stk      = &dummy_stk;
    dummy_task.gs       = &dummy_cpu;

    dummy_task.hash_table = task_hash_table;

    QueInit(&dummy_task.next_cc);
    dummy_task.last_cc = &dummy_task.next_cc;

    __holyc_fs = &dummy_task;
    __holyc_gs = &dummy_cpu;
    adam_task = &dummy_task;

    MemSet(&cmp, 0, sizeof(CCmpGlbls));
    MemSet(&uasm, 0, sizeof(CUAsmGlbls));
}

void Option(I64 opt, I64 val) {
    (void)opt; (void)val;
}

void Yield(void) {
    sched_yield();
}

void Sleep(I64 ms) {
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

void Exit(I64 code) {
    exit((int)code);
}

void TaskEnd(void) {
    exit(0);
}

/* TempleOS HashAdd(entry, hash_table) — adds existing entry to table.
   The entry already has its key set. */
void HashAdd(void *_entry, CHashTable *ht) {
    CHash *entry = (CHash*)_entry;
    if (!entry || !ht) return;
    I64 mask = ht->mask;
    I64 h    = HashStr(entry->str) & mask;
    CHash **pp = &ht->body[h];
    entry->next = *pp;
    *pp = entry;
}

CHash *HashFind(U8 *key, CHashTable *ht, I64 type) {
    if (!ht || !key) return NULL;
    while (ht) {
        I64 mask = ht->mask;
        I64 h    = HashStr(key) & mask;
        CHash **pp = &ht->body[h];
        for (CHash *e = *pp; e; e = e->next) {
            if (e->str && StrCmp(e->str, key) == 0 && e->type == type)
                return e;
        }
        ht = ht->next;
    }
    return NULL;
}

U32 sys_run_level = 0;

Bool Bt(I64 *bmp, I64 bit) { U8 *b = (U8*)bmp; return (b[bit >> 3] >> (bit & 7)) & 1; }
Bool Bts(I64 *bmp, I64 bit) { U8 *b = (U8*)bmp; Bool r = Bt(bmp, bit); b[bit >> 3] |= 1 << (bit & 7); return r; }
Bool Btr(I64 *bmp, I64 bit) { U8 *b = (U8*)bmp; Bool r = Bt(bmp, bit); b[bit >> 3] &= ~(1 << (bit & 7)); return r; }
I64 Bsr(I64 v) { return v ? (63 - __builtin_clzll(v)) : -1; }

U8 *StrNew(U8 *s) { U8 *r = MAlloc(StrLen(s) + 1); StrCpy(r, s); return r; }
U8 *FileNameAbs(U8 *f) { return StrNew(f); }
I64 MSize(U8 *p) { return p ? 16 : 0; }
I64 MSize2(U8 *p) { return MSize(p); }
U8 *MAllocIdent(U8 *s) { return StrNew(s); }
I64 ClampI64(I64 v, I64 min, I64 max) { return v < min ? min : (v > max ? max : v); }
I64 Pow10I64(I64 e) { I64 r = 1; while (e-- > 0) r *= 10; return r; }
I64 ToUpper(I64 ch) { return (ch >= 'a' && ch <= 'z') ? ch - 32 : ch; }
I64 MaxI64(I64 a, I64 b) { return a > b ? a : b; }
I64 MinI64(I64 a, I64 b) { return a < b ? a : b; }
Bool ToBool(I64 v) { return v != 0; }
I64 ToI64(F64 v) { return (I64)v; }
F64 ToF64(I64 v) { return (F64)v; }
Bool BEqu(U8 *bmp, I64 bit, Bool val) { if (val) return Bts(bmp, bit); else return Btr(bmp, bit); }
I64 StrIMatch(U8 *a, U8 *b) { return StrCmp(a, b); }

CHashDefineStr *DefineLstLoad(U8 *dname, U8 *lst) {
    I64 cnt = 0;
    U8 *ptr;
    CHashDefineStr *tmph = (CHashDefineStr *)CAlloc(sizeof(CHashDefineStr));
    tmph->type = HTT_DEFINE_STR;
    tmph->str = StrNew(dname);
    ptr = lst;
    while (*ptr) {
        if (*ptr != '@') cnt++;
        while (*ptr++) ;
    }
    I64 total = (ptr - lst) + 1;
    tmph->data = (U8 *)MAlloc(total);
    MemCpy(tmph->data, lst, total);
    tmph->cnt = cnt;
    HashAdd(tmph, Fs()->hash_table);
    return tmph;
}
void HashDel(CHash *h) { }
I64 HashTypeNum(CHash *h) { return h ? h->type : 0; }
void HashSrcFileSet(void *cc, void *hash, I64 line) { }
CHash *HashSingleTableFind(U8 *key, CHashTable *ht, I64 type, I64 flags) { return HashFind(key, ht, type); }
void HashAddAfter(CHash *after, CHash *entry) { }
void HashTableDel(CHashTable *ht) { }

void LinkedLstDel(void *lst) { }
I64 LinkedLstSize(void *lst) { return 0; }
void QueDel(void *q) { }
void QueVectU8Put(U8 **v, I64 idx, U8 val) { }
U8 QueVectU8Get(U8 **v, I64 idx) { return 0; }
void QueVectU8Del(U8 **v) { }

void AdamErr(U8 *fmt, ...) { }
Bool IsRaw(void) { return FALSE; }
Bool Raw(Bool v) { return v; }
void Dbg(U8 *fmt, ...) { }
void FixSet(U8 *fn, I64 line) { }
void PrintWarn(U8 *fmt, ...) { }
U8 *Caller(I64 n) { return NULL; }
void _throw(I64 ch, Bool no_log) {
    longjmp(__holyc_exception_buf, ch);
}
void PutFileLink(U8 *fn, I64 line, I64 col) { }
void *DocPutLine(void *doc, void *doc_e) { return doc_e; }
void FlushMsgs(void) { }

U8 *SysSymImportsResolve(U8 *name) { return NULL; }
U8 *ExtChg(U8 *name, U8 *ext) { return name; }
U8 *StrPrintJoin(U8 *buf, U8 *fmt, ...) { return buf; }
I64 LastFun(I64 argc, U8 **argv) { return 0; }
void CatPrint(U8 *buf, U8 *fmt, ...) { }
void StrPrint(U8 *buf, U8 *fmt, ...) { }
Bool PutSrcLink(U8 *src, I64 line, U8 *tag) { return FALSE; }
Bool DefineSub(U8 *s, I64 idx) { return FALSE; }
void QSortI64(I64 *base, I64 num, I64 (*cmp)(I64 *, I64 *)) { }
void MemSetI64(I64 *dst, I64 val, I64 cnt) { while (cnt-- > 0) *dst++ = val; }
U8 *MHeapCtrl(U8 *p) { return NULL; }
I64 GetOption(I64 opt) { return 0; }
void QSort(void *base, I64 num, I64 width, I64 (*cmp)(U8 *, U8 *)) { }

void *DocNew(U8 *filename, void *task) { return NULL; }
U0 DocWrite(CDoc *doc, ...) { }
void *DocLoad(void *doc, U8 *src, I64 size) { return NULL; }
void *DocPrint(void *doc, U8 *fmt, ...) { return NULL; }
void DocBinsValidate(void *doc) { }
void DocDel(void *doc) { }
I64 DocSize(CDoc *doc) { return 0; }
U8 *Doc2PlainText(CDoc *doc, CDocEntry *doc_e) { return NULL; }
Bool FileWrite(const U8 *path, const U8 *data, I64 size) { return FALSE; }
U0 CmdLinePmt(void) { }

U32 char_bmp_hex_numeric_data[16] =
    {0x00000000,0x03FF0000,0x0000007E,0x0000007E,0,0,0,0,0,0,0,0,0,0,0,0};
U8 *char_bmp_hex_numeric = (U8*)char_bmp_hex_numeric_data;

U32 char_bmp_dec_numeric_data[16] =
    {0x00000000,0x03FF0000,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
U8 *char_bmp_dec_numeric = (U8*)char_bmp_dec_numeric_data;

U32 char_bmp_non_eol_white_space_data[16] =
    {0x80000200,0x00000001,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
U8 *char_bmp_non_eol_white_space = (U8*)char_bmp_non_eol_white_space_data;

U32 char_bmp_printable_data[16] = {
    0x80002600,0xFFFFFFFF,0xFFFFFFFF,0x7FFFFFFF,
    0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
    0,0,0,0,0,0,0,0
};
U8 *char_bmp_printable = (U8*)char_bmp_printable_data;

U32 char_bmp_alpha_numeric_data[16] =
    {0x00000000,0x03FF0000,0x87FFFFFF,0x07FFFFFE,
    0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
    0,0,0,0,0,0,0,0};
U8 *char_bmp_alpha_numeric = (U8*)char_bmp_alpha_numeric_data;

U32 char_bmp_alpha_numeric_no_at_data[16] =
    {0x00000000,0x03FF0000,0x87FFFFFE,0x07FFFFFE,
    0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
    0,0,0,0,0,0,0,0};
U8 *char_bmp_alpha_numeric_no_at = (U8*)char_bmp_alpha_numeric_no_at_data;

U8 char_bmp_displayable[16] = {
    0x00,0x00,0x00,0x80,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0x7F
};

U8 *mem_boot_base = NULL;

I64 Bsf(I64 v) { return v ? __builtin_ctzll(v) : -1; }
I64 CeilU64(I64 v, I64 alignment) { return (v + alignment - 1) & ~(alignment - 1); }
void *QueVectU8New(void) { return NULL; }
I64 Call(I64 (*f)(void)) {
    Print("  Call(%p)\n", f);
    I64 res = f();
    Print("  Call returned %d\n", res);
    return res;
}
Bool IsDbgMode(void) { return FALSE; }
I64 LstMatch(U8 *s, U8 *lst, I64 flags) {
    if (!s || !lst) return -1;
    I64 idx = 0;
    (void)flags;
    while (*lst) {
        if (StrCmp(s, lst) == 0) return idx;
        while (*lst) lst++;
        lst++;
        idx++;
    }
    return -1;
}
I64 DefineMatch(U8 *needle, U8 *haystack_lst_dname, I64 flags) {
    CHashDefineStr *tmph = (CHashDefineStr *)HashFind(haystack_lst_dname, Fs()->hash_table, HTT_DEFINE_STR);
    if (!tmph || !tmph->data) return -1;
    return LstMatch(needle, tmph->data, flags);
}
U8 *AStrNew(U8 *s) { return StrNew(s); }
void *adam_task = NULL;

static void segv_handler(int sig) {
    void *bt[32];
    int n = backtrace(bt, 32);
    const char msg[] = "SEGV caught\n";
    write(2, msg, sizeof(msg)-1);
    backtrace_symbols_fd(bt, n, 2);
    _exit(1);
}

void _debug_char(char c) {
    write(2, &c, 1);
}

extern U0 CInit(void);
extern I64 ExeFile(U8 *name, I64 ccf_flags);
extern I64 ExePrint(U8 *fmt, ...);
extern CCmpCtrl *CmpCtrlNew(U8 *buf, I64 flags, U8 *filename);
extern I64 Lex(CCmpCtrl *cc);
extern U0 CmpCtrlDel(CCmpCtrl *cc);
extern U8 *LexStmt2Bin(CCmpCtrl *cc, I64 *_type, I64 cmp_flags);

I64 ExeCmdLine(struct CCmpCtrl *cc) {
    I64 res=0,type;
    U8 *machine_code;
    I64 iter=0;
    PrintErr("ExeCmdLine: token=%d (0x%X)\n", cc->token, cc->token);
    while (cc->token && (cc->token!='}' || !(cc->flags & 0x2000))) {
        if (++iter > 100) {
            PrintErr("ExeCmdLine: max iterations reached, stopping\n");
            break;
        }
        PrintErr("ExeCmdLine: iteration %d, token=%d\n", iter, cc->token);
        cc->flags &= ~0x4000000;
        machine_code=LexStmt2Bin(cc,&type,0);
        PrintErr("ExeCmdLine: LexStmt2Bin returned %p\n", machine_code);
        if (machine_code!=(U8*)0x7FFFFFFFFFFFFFFFUL) {
            PrintErr("  [JIT] Executing machine code at %p\n", machine_code);
            res=Call(machine_code);
            PrintErr("  [JIT] Result: %d\n", res);
            Free(machine_code);
        } else {
            PrintErr("  [JIT] LexStmt2Bin returned INVALID_PTR\n");
            break;
        }
    }
    PrintErr("ExeCmdLine: done, res=%d\n", res);
    return res;
}

int main(int argc, char **argv) {
    signal(SIGSEGV, segv_handler);
    holyc_runtime_init();
    Print("Before CInit...\n");
    fflush(stdout);
    CInit();
    Print("After CInit...\n");
    fflush(stdout);
    Print("NeoTempleOS Compiler initialized\n");
    
    if (argc > 1) {
        Print("Compiling: %s\n", argv[1]);
        I64 res = ExeFile(argv[1], 0);
        Print("Result: %d\n", res);
    } else {
        Print("Direct test:\n");
        U8 *buf = "5;";
        CCmpCtrl *cc = CmpCtrlNew(buf, 0x40, "test");
        Print("  Created CmpCtrl\n");
        I64 tok = Lex(cc);
        Print("  First token: %d\n", tok);
        if (tok) {
            Print("  Calling ExeCmdLine...\n");
            I64 res = ExeCmdLine(cc);
            Print("  ExeCmdLine returned: %d\n", res);
        }
        CmpCtrlDel(cc);
        Print("Done\n");
    }
    
    return 0;
}
