#include "holyc_types.h"
#include <stdio.h>

// Call QSort2a directly with properly typed parameters
U0 QSort2a(U8 **base,I64 num,I64 (*fp_compare)(U8 **_e1,U8 **_e2))
{
  I64 i;
  U8 **left,**right,*pivot;
  left =base;
  right=base+num-1;
  pivot=base[num/2];
  do {
    while ((*fp_compare)(left,&pivot)<0)
      left++;
    while ((*fp_compare)(right,&pivot)>0)
      right--;
    if (left<=right) {
      SwapI64(left,right);
      left++; right--;
    }
  } while (left<=right);
  i=right+1-base;
  if (1<i<num)
    QSort2a(base,i,fp_compare);
  i=base+num-left;
  if (1<i<num)
    QSort2a(left,i,fp_compare);
}

I64 cmp_vals(U8 **_e1, U8 **_e2) {
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

    QSort2a((U8**)ptrs, 5, cmp_vals);

    Print("After:");
    for(I64 i=0; i<5; i++) Print(" %lld", *ptrs[i]);
    Print("\n");

    for(I64 i=1; i<5; i++) if(*ptrs[i-1] > *ptrs[i]) { PrintErr("FAIL\n"); return 1; }
    Print("PASS\n");
    fflush(stdout);
    return 0;
}
