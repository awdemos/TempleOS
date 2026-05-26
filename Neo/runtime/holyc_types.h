#ifndef _HOLYC_TYPES_H
#define _HOLYC_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <sys/mman.h>
#include <assert.h>
#include <stdarg.h>

//============================================================================
// HOLYC TYPE MAPPING — direct #define for near-C compatibility
// Use: -include holyc_types.h when compiling .HC files
//============================================================================

typedef uint64_t    U64;
typedef int64_t     I64;
typedef uint32_t    U32;
typedef int32_t     I32;
typedef uint16_t    U16;
typedef int16_t     I16;
typedef uint8_t     U8;
typedef int8_t      I8;
typedef int8_t      I8i;
typedef double      F64;
typedef float       F32;
typedef void        U0;
typedef int         Bool;

#define INVALID_PTR  ((void*)(intptr_t)0x7FFFFFFFFFFFFFFFUL)
#ifndef NULL
#define NULL         ((void*)0)
#endif
#define TRUE         1
#define FALSE        0
#define ON           1
#define OFF          0

// HolyC special values
#define pi           3.14159265358979323846
#define exp_1        2.71828182845904523536

// HolyC integer union access macros
#define UNION_U8(x, idx)   (((U8*)&(x))[idx])
#define UNION_I8(x, idx)   (((I8*)&(x))[idx])
#define UNION_U16(x, idx)  (((U16*)&(x))[idx])
#define UNION_U32(x, idx)  (((U32*)&(x))[idx])
#define UNION_I64(x)       (*(I64*)&(x))
#define UNION_U64(x)       (*(U64*)&(x))
#define UNION_F64(x)       (*(F64*)&(x))

// HolyC variadic compatibility (TODO: proper va_list conversion)
// HolyC uses implicit argc/argv in variadic functions.
// This stub makes them compile but needs proper implementation.
#define __HOLYC_ARGC  0
#define __HOLYC_ARGV  ((I64*)NULL)

//============================================================================
// QUEUE (doubly-linked list) primitives
//============================================================================
typedef struct QueNode {
    struct QueNode *next, *last;
} QueNode;

static inline void QueInit(void *head) {
    QueNode *h = (QueNode *)head;
    h->next = h->last = h;
}

static inline void QueIns(void *ins, void *after) {
    QueNode *i = (QueNode *)ins;
    QueNode *a = (QueNode *)after;
    i->next = a->next;
    i->last = a;
    a->next->last = i;
    a->next = i;
}

static inline void QueInsRev(void *ins, void *before) {
    QueNode *i = (QueNode *)ins;
    QueNode *b = (QueNode *)before;
    i->last = b->last;
    i->next = b;
    b->last->next = i;
    b->last = i;
}

static inline void QueRem(void *node) {
    QueNode *n = (QueNode *)node;
    n->last->next = n->next;
    n->next->last = n->last;
}

static inline Bool QueIsEmpty(void *head) {
    return ((QueNode *)head)->next == (QueNode *)head;
}

#define QUEUE_FOR(head, type, var) \
    for (type *var = (type*)((QueNode*)(head))->next; \
         var != (type*)(head); \
         var = (type*)(((QueNode*)var)->next))

//============================================================================
// ALLOCATORS
// HolyC allows MAlloc(size) and MAlloc(size, heap) (overloading by arg count).
// C uses varargs so both call forms work.
//============================================================================
static inline void *MAlloc(I64 size, ...) {
    (void)size;
    return size > 0 ? malloc((size_t)size) : NULL;
}
static inline void *CAlloc(I64 size, ...) {
    (void)size;
    return size > 0 ? calloc(1, (size_t)size) : NULL;
}
static inline void Free(void *p) { if (p) free(p); }

//============================================================================
// MEMORY OPS
//============================================================================
static inline void MemCpy(void *d, const void *s, I64 n) { memcpy(d, s, (size_t)n); }
static inline void MemSet(void *d, U8 v, I64 n) { memset(d, v, (size_t)n); }
static inline I64 MemCmp(const void *a, const void *b, I64 n) { return memcmp(a, b, (size_t)n); }

//============================================================================
// STRING OPS
//============================================================================
static inline I64 StrLen(const U8 *s) { return s ? (I64)strlen((const char*)s) : 0; }
static inline I64 StrCpy(U8 *d, const U8 *s) { if(!d||!s)return 0; I64 n=0; while((*d++=*s++))n++; return n; }
static inline I64 StrCmp(const U8 *a, const U8 *b) { return (I64)strcmp((const char*)a,(const char*)b); }
static inline U8 *StrChr(const U8 *s, U8 c) { return s ? (U8*)strchr((const char*)s,c) : NULL; }
static inline U8 *StrStr(const U8 *h, const U8 *n) { return h&&n ? (U8*)strstr((const char*)h,(const char*)n) : NULL; }
static inline I64 StrOcc(const U8 *s, U8 c) { if(!s)return 0; I64 n=0; while(*s)if(*s++==c)n++; return n; }

static inline U8 *MStrPrint(const U8 *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    I64 len = (I64)vsnprintf(NULL, 0, (const char*)fmt, ap); va_end(ap);
    U8 *buf = (U8*)MAlloc(len + 1);
    va_start(ap, fmt); vsnprintf((char*)buf, (size_t)(len+1), (const char*)fmt, ap); va_end(ap);
    return buf;
}

static inline U8 *ExtDft(const U8 *name, const U8 *ext) {
    return StrChr(name, '.') ? MStrPrint("%s", name) : MStrPrint("%s.%s", name, ext);
}

//============================================================================
// HASH TABLE — std hash for test files
//============================================================================
#define HASH_BUCKETS 256

typedef struct StdHashEntry {
    struct StdHashEntry *next;
    U8 *key;
    I64 hash;
    void *val;
} StdHashEntry;

typedef struct StdHashTable {
    struct StdHashTable *next;
    struct StdHashEntry *buckets[HASH_BUCKETS];
    I64 num;
} StdHashTable;

static inline I64 HashStr(const U8 *s) {
    if (!s) return 0; I64 h = 0; while (*s) h = (h * 33) ^ (I64)(*s++); return h & 0x7FFFFFFF;
}



static inline void HashTableAdd(StdHashTable *ht, const U8 *key, void *val) {
    if(!ht||!key)return; I64 h=HashStr(key); I64 i=(I64)((U64)h%HASH_BUCKETS);
    StdHashEntry *e=(StdHashEntry*)MAlloc(sizeof(StdHashEntry));
    e->key=MStrPrint("%s",key); e->hash=h; e->val=val; e->next=ht->buckets[i]; ht->buckets[i]=e; ht->num++;
}

static inline void *HashTableFind(StdHashTable *ht, const U8 *key) {
    if(!ht||!key)return NULL; I64 h=HashStr(key);
    for(StdHashEntry *e=ht->buckets[(U64)h%HASH_BUCKETS];e;e=e->next)
        if(e->hash==h&&!StrCmp(e->key,key)) return e->val;
    return NULL;
}

static inline void HashTablePurge(StdHashTable *ht) {
    if(!ht)return; for(I64 i=0;i<HASH_BUCKETS;i++){StdHashEntry*e=ht->buckets[i];while(e){StdHashEntry*n=e->next;Free(e->key);Free(e);e=n;}ht->buckets[i]=NULL;}ht->num=0;
}

//============================================================================
// I/O
//============================================================================
static inline U8 *FileRead(const U8 *path, I64 *size) {
    FILE *f = fopen((const char*)path, "rb");
    if(!f) {
        if(size) *size=0;
        U8 *buf = (U8*)MAlloc(1);
        buf[0] = 0;
        return buf;
    }
    fseek(f,0,SEEK_END); I64 sz=(I64)ftell(f); fseek(f,0,SEEK_SET);
    U8 *buf=(U8*)MAlloc(sz+1); I64 n=fread(buf,1,(size_t)sz,f); buf[n]=0; fclose(f);
    if(size)*size=n; return buf;
}

static inline I64 Print(const U8 *fmt, ...) {
    va_list ap; va_start(ap,fmt); I64 r=vprintf((const char*)fmt,ap); va_end(ap); return r;
}
static inline I64 PrintErr(const U8 *fmt, ...) {
    va_list ap; va_start(ap,fmt); I64 r=vfprintf(stderr,(const char*)fmt,ap); va_end(ap); return r;
}

//============================================================================
// MATH
//============================================================================
#define Sin(x)    sin(x)
#define Cos(x)    cos(x)
#define Sqrt(x)   sqrt(x)
#define Abs(x)    (((x)<0)?-(x):(x))
#define MinI(a,b) (((a)<(b))?(a):(b))
#define MaxI(a,b) (((a)>(b))?(a):(b))

//============================================================================
// BIT OPS
//============================================================================
#define LBts(v,bit)  (*(v) |=  (1ULL<<(bit)))
#define LBtr(v,bit)  (*(v) &= ~(1ULL<<(bit)))

// HolyC SwapI64 is a compiler intrinsic accepting any pointer type (8-byte swap).
// Macros replicate this for C — no type safety on pointer, but matches HolyC semantics.
#define SwapI64(a,b) do { I64 *_ai_=(I64*)(a), *_bi_=(I64*)(b), _sti_=*_ai_; *_ai_=*_bi_; *_bi_=_sti_; } while(0)
#define SwapU64(a,b) do { U64 *_au_=(U64*)(a), *_bu_=(U64*)(b), _stu_=*_au_; *_au_=*_bu_; *_bu_=_stu_; } while(0)
#define SwapU8(a,b)  do { U8  *_a8_=(U8*)(a),  *_b8_=(U8*)(b),  _st8_=*_a8_; *_a8_=*_b8_; *_b8_=_st8_; } while(0)

//============================================================================
// EXCEPTIONS (setjmp/longjmp based)
//============================================================================
extern jmp_buf __holyc_exception_buf;
extern I64    __holyc_exception_code;

#define try     { jmp_buf __buf; memcpy(__buf,__holyc_exception_buf,sizeof(jmp_buf)); \
                  I64 __ex=setjmp(__holyc_exception_buf); if(__ex==0) {
#define catch   __holyc_exception_code=__ex; } else { \
                  memcpy(__holyc_exception_buf,__buf,sizeof(jmp_buf));
#define endtry  } memcpy(__holyc_exception_buf,__buf,sizeof(jmp_buf)); }

//============================================================================
// COMPILER FLAGS
//============================================================================
#define OPTf_WARN_PAREN            0
#define OPTf_WARN_DUP_TYPES        1
#define OPTf_KEEP_PRIVATE          2
#define OPTf_EXTERNS_TO_IMPORTS    3
#define OPTf_WARN_IMPLICIT_CAST    4
#define OPTf_NO_BUILTIN_CONST      5

#define CCf_AOT_COMPILE           0x0001LL
#define CCf_PASS_TRACE_PRESENT    0x0002LL

#define CMPF_ASM_BLK              1
#define CMPF_ONE_ASM_INS          2
#define CMPF_LEX_FIRST            4
#define CMPF_PRS_SEMICOLON        8

extern U8 char_bmp_displayable[16];
extern U32 sys_run_level;

extern Bool Bt(I64 *bmp, I64 bit);
extern Bool Bts(I64 *bmp, I64 bit);
extern Bool Btr(I64 *bmp, I64 bit);
extern I64 Bsf(I64 v);
extern I64 Bsr(I64 v);
extern I64 CeilU64(I64 v, I64 alignment);
extern I64 Call(I64 (*f)(void));
extern U8 *StrNew(U8 *s);
extern U8 *FileNameAbs(U8 *f);
extern I64 MSize(U8 *p);
extern I64 MSize2(U8 *p);
extern U8 *MAllocIdent(U8 *s);
extern I64 ClampI64(I64 v, I64 min, I64 max);
extern I64 Pow10I64(I64 e);
extern I64 ToUpper(I64 ch);
extern I64 MaxI64(I64 a, I64 b);
extern I64 MinI64(I64 a, I64 b);
extern Bool ToBool(I64 v);
extern I64 ToI64(F64 v);
extern F64 ToF64(I64 v);
extern Bool BEqu(U8 *bmp, I64 bit, Bool val);
extern I64 StrIMatch(U8 *a, U8 *b);

extern void AdamErr(U8 *fmt, ...);
extern Bool IsRaw(void);
extern Bool Raw(Bool v);
extern void FixSet(U8 *fn, I64 line);
extern void PrintWarn(U8 *fmt, ...);
extern U8 *Caller(I64 n);
extern void _throw(I64 ch, Bool no_log);
#define throw(ch) _throw(ch,FALSE)
extern void PutFileLink(U8 *fn, I64 line, I64 col);

extern void FlushMsgs(void);

extern U8 *SysSymImportsResolve(U8 *name);
extern U8 *ExtChg(U8 *name, U8 *ext);
extern U8 *StrPrintJoin(U8 *buf, U8 *fmt, ...);
extern I64 LastFun(I64 argc, U8 **argv);
extern void CatPrint(U8 *buf, U8 *fmt, ...);
extern void StrPrint(U8 *buf, U8 *fmt, ...);
extern Bool PutSrcLink(U8 *src, I64 line, U8 *tag);
extern Bool DefineSub(U8 *s, I64 idx);
extern void QSortI64(I64 *base, I64 num, I64 (*cmp)(I64 *, I64 *));
extern void MemSetI64(I64 *dst, I64 val, I64 cnt);
extern U8 *MHeapCtrl(U8 *p);
extern I64 GetOption(I64 opt);
extern void QSort(void *base, I64 num, I64 width, I64 (*cmp)(U8 *, U8 *));
struct CHash;
struct CHashTable;
extern void HashSrcFileSet(void *cc, void *hash, I64 line);
extern struct CHash *HashSingleTableFind(U8 *key, struct CHashTable *ht, I64 type, I64 flags);

#endif
