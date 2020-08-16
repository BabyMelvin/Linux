/* More subroutines needed by GCC output code on some machines.  */

#include "gcclib.h"

#define umul_ppmm(xh, xl, a, b) \
{register USItype __t0, __t1, __t2;                                     \
  __asm__ ("%@ Inlined umul_ppmm					\n\
        mov     %2, %5, lsr #16						\n\
        mov     %0, %6, lsr #16						\n\
        bic     %3, %5, %2, lsl #16					\n\
        bic     %4, %6, %0, lsl #16					\n\
        mul     %1, %3, %4						\n\
        mul     %4, %2, %4						\n\
        mul     %3, %0, %3						\n\
        mul     %0, %2, %0						\n\
        adds    %3, %4, %3						\n\
        addcs   %0, %0, #65536						\n\
        adds    %1, %1, %3, lsl #16					\n\
        adc     %0, %0, %3, lsr #16"                                    \
           : "=&r" ((USItype) (xh)),                                    \
             "=r" ((USItype) (xl)),                                     \
             "=&r" (__t0), "=&r" (__t1), "=r" (__t2)                    \
           : "r" ((USItype) (a)),                                       \
             "r" ((USItype) (b)));}


#define __umulsidi3(u, v) \
  ({DIunion __w;                                                        \
    umul_ppmm (__w.s.high, __w.s.low, u, v);                            \
    __w.ll; })


DItype
__muldi3 (DItype u, DItype v)
{
  DIunion w;
  DIunion uu, vv;

  uu.ll = u,
  vv.ll = v;

  w.ll = __umulsidi3 (uu.s.low, vv.s.low);
  w.s.high += ((USItype) uu.s.low * (USItype) vv.s.high
               + (USItype) uu.s.high * (USItype) vv.s.low);

  return w.ll;
}
