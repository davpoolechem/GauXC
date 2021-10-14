#ifndef __MY_INTEGRAL_2_1
#define __MY_INTEGRAL_2_1

#include "integral_2_1.h"

void integral_2_1(int npts,
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
