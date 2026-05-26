#include "holyc_types.h"
#include <stdio.h>

// Simplified QSort2a with debug prints
U0 QSort2a_db(U8 **base, I64 num, I64 (*fp_compare)(U8 **, U8 **), I64 depth)
{
  I64 i;
  U8 **left, **right, *pivot;
  left = base;
  right = base + num - 1;
  pivot = base[num/2];

  I64 safe = 0;
  do {
    while ((*fp_compare)(left, &pivot) < 0) {
        left++;
        if (++safe > 100) { Print("INF left loop\n"); return; }
    }
    while ((*fp_compare)(right, &pivot) > 0) {
        right--;
        if (++safe > 100) { Print("INF right loop\n"); return; }
    }
    if (left <= right) {
      SwapI64(left, right);
      left++; right--;
    }
  } while (left <= right && ++safe < 100);

  i = right + 1 - base;
  if (1 < i && i < num) QSort2a_db(base, i, fp_compare, depth+1);
  i = base + num - left;
  if (1 < i && i < num) QSort2a_db(left, i, fp_compare, depth+1);
}

I64 cmp_ptrs(U8 **_e1, U8 **_e2) {
    I64 va = **(I64**)_e1;
    I64 vb = **(I64**)_e2;
    return va - vb;
}

int main(void) {
    I64 vals[5] = {42, 17, 99, 3, 66};
    I64 *ptrs[5];
    for(I64 i=0; i<5; i++) ptrs[i] = &vals[i];

    Print("Before:");
    for(I64 i=0; i<5; i++) Print(" %lld", *ptrs[i]);
    Print("\n");

    QSort2a_db((U8**)ptrs, 5, cmp_ptrs, 0);

    Print("After: ");
    for(I64 i=0; i<5; i++) Print(" %lld", *ptrs[i]);
    Print("\n");
    fflush(stdout);
    return 0;
}
