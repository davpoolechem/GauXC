#ifndef __MY_INTEGRAL_3_1
#define __MY_INTEGRAL_3_1

#include "integral_3_1.h"

void integral_3_1(int npts,
                  shells shellA,
                  shells shellB,
                  point *points,
                  double *Xi,
                  double *Xj,
                  int ldX,
                  double *Gi,
                  double *Gj,
                  int ldG, 
                  double *weights);

#endif
