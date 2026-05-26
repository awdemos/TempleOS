#include "holyc_types.h"
#include "QSort.c"
#include <stdio.h>

I64 cmp_asc(I64 a, I64 b) { return a - b; }
I64 cmp_desc(I64 a, I64 b) { return b - a; }

typedef struct { I64 x; I64 y; } Pair;

// QSort2a passes U8** to compare (array elements are pointers).
// Compare by pointed-to I64 values.
I64 cmp_ptr(U8 *a, U8 *b) {
    I64 va = **(I64**)a;
    I64 vb = **(I64**)b;
    return va - vb;
}
I64 cmp_pair_x(U8 *a, U8 *b) { return ((Pair*)a)->x - ((Pair*)b)->x; }

int main(void) {
    // Test QSortI64
    I64 arr[] = {9, 3, 7, 1, 5, 8, 2, 6, 4, 0};
    I64 num = sizeof(arr)/sizeof(arr[0]);
    QSortI64(arr, num, cmp_asc);
    for(I64 i = 0; i < num; i++) {
        if (arr[i] != i) {
            PrintErr("FAIL QSortI64 asc: arr[%lld]=%lld, expected %lld\n", i, arr[i], i);
            return 1;
        }
    }
    Print("PASS QSortI64 asc\n");

    // Test QSortI64 descending
    QSortI64(arr, num, cmp_desc);
    for(I64 i = 0; i < num; i++) {
        if (arr[i] != num-1-i) {
            PrintErr("FAIL QSortI64 desc: arr[%lld]=%lld, expected %lld\n", i, arr[i], num-1-i);
            return 1;
        }
    }
    Print("PASS QSortI64 desc\n");

    // Test QSort with U8* array of pointers (width == sizeof(U8*))
    I64 *ptrs[5];
    I64 vals[5] = {42, 17, 99, 3, 66};
    for(I64 i = 0; i < 5; i++) ptrs[i] = &vals[i];
    QSort((U8*)ptrs, 5, sizeof(U8*), cmp_ptr);
    for(I64 i = 1; i < 5; i++) {
        if (*(I64*)ptrs[i-1] > *(I64*)ptrs[i]) {
            PrintErr("FAIL QSort ptrs: %lld > %lld at %lld\n",
                *(I64*)ptrs[i-1], *(I64*)ptrs[i], i);
            return 1;
        }
    }
    Print("PASS QSort ptrs\n");

    // Test QSort with struct (width != sizeof(U8*))
    Pair pairs[4] = {{3,9}, {1,7}, {4,6}, {2,8}};
    QSort((U8*)pairs, 4, sizeof(Pair), cmp_pair_x);
    for(I64 i = 1; i < 4; i++) {
        if (pairs[i-1].x > pairs[i].x) {
            PrintErr("FAIL QSort pairs: %lld > %lld at %lld\n",
                pairs[i-1].x, pairs[i].x, i);
            return 1;
        }
    }
    Print("PASS QSort pairs\n");

    Print("All tests passed!\n");
    return 0;
}
