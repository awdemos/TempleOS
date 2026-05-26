#include "holyc_types.h"
#include <stdio.h>

// Inline QSort2a for debugging
U0 QSort2a_debug(U8 **base, I64 num, I64 (*fp_compare)(U8 *, U8 *))
{
  I64 i;
  U8 **left, **right, *pivot;
  if (num <= 1) { Print("  base case num=%lld\n", num); return; }
  left = base;
  right = base + num - 1;
  pivot = base[num/2];
  Print("  pivot=%p base[0]=%p base[%lld]=%p\n", pivot, base[0], num/2, base[num/2]);
  fflush(stdout);

  I64 iter = 0;
  do {
    while (iter < 10 && (*fp_compare)((U8*)(left), &pivot) < 0) {
        Print("  left++ from %p\n", left); fflush(stdout);
        left++;
    }
    while (iter < 10 && (*fp_compare)((U8*)(right), &pivot) > 0) {
        Print("  right-- from %p\n", right); fflush(stdout);
        right--;
    }
    Print("  left=%p right=%p left<=right=%d\n", left, right, left <= right); fflush(stdout);
    if (left <= right) {
      SwapI64(left, right);
      left++; right--;
    }
    iter++;
    if (iter > 20) { Print("  INFINITE LOOP DETECTED\n"); break; }
  } while (left <= right);

  Print("  post-loop: left=%p right=%p base=%p num=%lld\n", left, right, base, num);
  i = right + 1 - base;
  Print("  left-recurse i=%lld\n", i);
  if (1 < i && i < num) QSort2a_debug(base, i, fp_compare);
  i = base + num - left;
  Print("  right-recurse i=%lld\n", i);
  if (1 < i && i < num) QSort2a_debug(left, i, fp_compare);
}

U0 QSort_debug(U8 *base, I64 num, I64 width, I64 (*fp_compare)(U8 *, U8 *))
{
  U8 *tmp;
  if (width && num > 1) {
    if (width == sizeof(U8 *))
      QSort2a_debug((U8 **)base, num, fp_compare);
    else {
      tmp = MAlloc(width * 2);
      // QSort2b would go here
      Free(tmp);
    }
  }
}

typedef struct { I64 x; I64 y; } Pair;

I64 cmp_pair_x(U8 *a, U8 *b) {
    I64 va = ((Pair*)a)->x;
    I64 vb = ((Pair*)b)->x;
    return va - vb;
}

int main(void) {
    // Test 1: pairs via QSort2b path (width != 8)
    Print("=== Test pairs via QSort2b ===\n");
    Pair pairs[4] = {{3,9},{1,7},{4,6},{2,8}};
    QSort_debug((U8*)pairs, 4, sizeof(Pair), cmp_pair_x);
    for(I64 i=1; i<4; i++) if(pairs[i-1].x > pairs[i].x) { PrintErr("FAIL pairs\n"); return 1; }
    Print("PASS pairs\n");

    // Test 2: pointers via QSort2a (width == 8)
    Print("=== Test ptrs via QSort2a ===\n");
    I64 vals[5] = {42, 17, 99, 3, 66};
    I64 *ptrs[5];
    for(I64 i=0; i<5; i++) ptrs[i] = &vals[i];

    // For QSort2a, compare function receives U8** data
    // where elements are U8* pointers to the actual data
    I64 cmp_ptrs(U8 *a, U8 *b) {
        // a and b are actually U8** pointing to array slots
        // *a is U8* (8 bytes) = the pointer stored in the slot
        // We need **(I64**)a to get the pointed-to I64
        I64 va = **(I64**)a;
        I64 vb = **(I64**)b;
        Print("    cmp a=%p b=%p va=%lld vb=%lld\n", a, b, va, vb);
        fflush(stdout);
        return va - vb;
    }

    QSort2a_debug((U8**)ptrs, 5, cmp_ptrs);
    for(I64 i=1; i<5; i++) if(*ptrs[i-1] > *ptrs[i]) { PrintErr("FAIL ptrs\n"); return 1; }
    Print("PASS ptrs\n");

    Print("DONE\n");
    fflush(stdout);
    return 0;
}
