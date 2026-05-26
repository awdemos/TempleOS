#include "holyc_types.h"
#include "QSort.c"
#include <stdio.h>

typedef struct { I64 x; I64 y; } Pair;

I64 cmp_asc(I64 a, I64 b) { return a - b; }
I64 cmp_desc(I64 a, I64 b) { return b - a; }
I64 cmp_pair_x(U8 *a, U8 *b) { return ((Pair*)a)->x - ((Pair*)b)->x; }

int main(void) {
    // Test QSortI64 ascending
    I64 arr[] = {9,3,7,1,5,8,2,6,4,0};
    I64 num = 10;
    QSortI64(arr, num, cmp_asc);
    for(I64 i=0; i<num; i++) if(arr[i] != i) { PrintErr("FAIL QSortI64 asc\n"); return 1; }
    Print("PASS QSortI64 asc\n");

    // Test QSortI64 descending
    QSortI64(arr, num, cmp_desc);
    for(I64 i=0; i<num; i++) if(arr[i] != num-1-i) { PrintErr("FAIL QSortI64 desc\n"); return 1; }
    Print("PASS QSortI64 desc\n");

    // Test QSort pairs (QSort2b path, width=16)
    Pair pairs[4] = {{3,9},{1,7},{4,6},{2,8}};
    QSort((U8*)pairs, 4, sizeof(Pair), cmp_pair_x);
    for(I64 i=1; i<4; i++) if(pairs[i-1].x > pairs[i].x) { PrintErr("FAIL pairs\n"); return 1; }
    Print("PASS QSort pairs\n");

    // QSort2a fast path tested in test_qsort8.c (direct QSort2a call)

    Print("ALL PASS\n");
    fflush(stdout);
    return 0;
}
