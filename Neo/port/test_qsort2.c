#include "holyc_types.h"
#include "QSort.c"
#include <stdio.h>

I64 cmp_int(I64 a, I64 b) { return a - b; }

int main(void) {
    Print("=== Test QSortI64 10 elements ===\n");
    I64 arr[] = {9,3,7,1,5,8,2,6,4,0};
    I64 num = 10;
    QSortI64(arr, num, cmp_int);
    for(I64 i=0; i<num; i++) Print("%lld ", arr[i]);
    Print("\n");

    Print("=== Test QSortI64 5 elements ===\n");
    I64 arr2[] = {42,17,99,3,66};
    QSortI64(arr2, 5, cmp_int);
    for(I64 i=0; i<5; i++) Print("%lld ", arr2[i]);
    Print("\n");

    Print("DONE\n");
    fflush(stdout);
    return 0;
}
