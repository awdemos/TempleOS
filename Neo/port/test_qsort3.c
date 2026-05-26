#include "holyc_types.h"
#include "QSort.c"
#include <stdio.h>

typedef struct { I64 x; I64 y; } Pair;

I64 cmp_pair_x(U8 *a, U8 *b) {
    return ((Pair*)a)->x - ((Pair*)b)->x;
}

int main(void) {
    Print("=== Test QSort pairs (width=16) ===\n");
    Pair pairs[4] = {{3,9}, {1,7}, {4,6}, {2,8}};
    QSort((U8*)pairs, 4, sizeof(Pair), cmp_pair_x);
    for(I64 i=0; i<4; i++) Print("{%lld,%lld} ", pairs[i].x, pairs[i].y);
    Print("\n");
    for(I64 i=1; i<4; i++) {
        if (pairs[i-1].x > pairs[i].x) {
            PrintErr("FAIL at %lld\n", i);
            return 1;
        }
    }
    Print("PASS pairs\n");

    Print("DONE\n");
    fflush(stdout);
    return 0;
}
