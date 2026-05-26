#include "holyc_types.h"
#include "QSort.c"
#include <stdio.h>

// QSort2a passes U8** to compare (array slots contain U8* pointers).
// Must double-deref to compare pointed-to values.
I64 cmp_ptr(U8 *a, U8 *b) {
    I64 va = **(I64**)a;
    I64 vb = **(I64**)b;
    return va - vb;
}

int main(void) {
    I64 vals[5] = {42, 17, 99, 3, 66};
    I64 *ptrs[5];
    for(I64 i=0; i<5; i++) ptrs[i] = &vals[i];

    Print("=== Test QSort ptrs (width=8, QSort2a path) ===\n");
    Print("Before: ");
    for(I64 i=0; i<5; i++) Print("%lld ", *ptrs[i]);
    Print("\n");

    QSort((U8*)ptrs, 5, sizeof(U8*), cmp_ptr);

    Print("After:  ");
    for(I64 i=0; i<5; i++) Print("%lld ", *ptrs[i]);
    Print("\n");

    for(I64 i=1; i<5; i++) {
        if (*(I64*)ptrs[i-1] > *(I64*)ptrs[i]) {
            PrintErr("FAIL ptrs: %lld > %lld at %lld\n",
                *(I64*)ptrs[i-1], *(I64*)ptrs[i], i);
            return 1;
        }
    }
    Print("PASS ptrs\n");
    Print("DONE\n");
    fflush(stdout);
    return 0;
}
