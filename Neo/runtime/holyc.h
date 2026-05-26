#ifndef _HOLYC_RUNTIME_H
#define _HOLYC_RUNTIME_H

#include "holyc_types.h"
#include "holyc_compiler_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Runtime helper function declarations */
extern void Option(I64 opt, I64 val);
extern void AOTStoreCodeU64(CCmpCtrl *cc, U64 val);

/* HolyC built-ins mapped to C equivalents */
#include <stddef.h>
#define offset(type, member) offsetof(type, member)

/* Additional function stubs */
extern I64 LstMatch(U8 *needle, U8 *haystack, I64 flags);
extern I64 DefineMatch(U8 *needle, U8 *haystack, I64 flags);
extern U8 *AStrNew(U8 *s);
extern U0 Dbg(U8 *fmt, ...);
extern U0 DocWrite(CDoc *doc, ...);
extern Bool IsDbgMode(void);
extern void LexExcept(CCmpCtrl *cc, U8 *str);

static inline CHashTable *HashTableNew(I64 hint) {
    (void)hint;
    CHashTable *ht = (CHashTable*)CAlloc(sizeof(CHashTable) + 15 * sizeof(CHash*));
    ht->mask = 15;
    return ht;
}

/* Initialize the HolyC runtime (Fs, Gs, etc.) */
extern void holyc_runtime_init(void);

extern void  HashAdd(void *entry, CHashTable *table);
extern CHash *HashFind(U8 *key, CHashTable *table, I64 type);

/* File operations */
extern Bool  FileWrite(const U8 *path, const U8 *data, I64 size);
extern Bool  FileExists(const U8 *path);

/* Task management (stubs for Linux) */
extern void Yield(void);
extern void Sleep(I64 ms);
extern void Exit(I64 code);
extern void TaskEnd(void);

/* TempleOS Print wrappers */
extern I64  Print(const U8 *fmt, ...);
extern I64  PrintErr(const U8 *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* _HOLYC_RUNTIME_H */
