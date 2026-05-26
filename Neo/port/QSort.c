#include "holyc_types.h"
U0 QSortI64(I64 *base,I64 num, I64 (*fp_compare)(I64 e1,I64 e2))
{
  I64 i,*left,*right,pivot;
  if (num>1) {
    left =base;
    right=base+num-1;
    pivot=base[num/2];
    do {
      while ((*fp_compare)(*left,pivot)<0)
	left++;
      while ((*fp_compare)(*right,pivot)>0)
	right--;
      if (left<=right)
	SwapI64(left++,right--);
    } while (left<=right);
    i=right+1-base;
    if (((1) < (i)) && ((i) < (num)))
      QSortI64(base,i,fp_compare);
    i=base+num-left;
    if (((1) < (i)) && ((i) < (num)))
      QSortI64(left,i,fp_compare);
  }
}

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
    if (left<=right)
      SwapI64(left++,right--);
  } while (left<=right);
  i=right+1-base;
  if (((1) < (i)) && ((i) < (num)))
    QSort2a(base,i,fp_compare);
  i=base+num-left;
  if (((1) < (i)) && ((i) < (num)))
    QSort2a(left,i,fp_compare);
}
U0 QSort2b(U8 *base,I64 num, I64 width,
	I64 (*fp_compare)(U8 *e1,U8 *e2),U8 *tmp)
{
  I64 i;
  U8 *left,*right,*pivot=tmp+width;
  left =base;
  right=base+(num-1)*width;
  MemCpy(pivot,base+num/2*width,width);
  do {
    while ((*fp_compare)(left,pivot)<0)
      left+=width;
    while ((*fp_compare)(right,pivot)>0)
      right-=width;
    if (left<=right) {
      if (left!=right) {
	MemCpy(tmp,right,width);
	MemCpy(right,left,width);
	MemCpy(left,tmp,width);
      }
      left+=width;
      right-=width;
    }
  } while (left<=right);
  i=1+(right-base)/width;
  if (((1) < (i)) && ((i) < (num)))
    QSort2b(base,i,width,fp_compare,tmp);
  i=num+(base-left)/width;
  if (((1) < (i)) && ((i) < (num)))
    QSort2b(left,i,width,fp_compare,tmp);
}
U0 QSort(U8 *base,I64 num, I64 width, I64 (*fp_compare)(U8 *e1,U8 *e2))
{
  U8 *tmp;
  if (width && num>1) {
    if (width==sizeof(U8 *))	
      QSort2a(base,num,fp_compare);
    else {
      tmp=MAlloc(width*2);
      QSort2b(base,num,width,fp_compare,tmp);
      Free(tmp);
    }
  }
}
