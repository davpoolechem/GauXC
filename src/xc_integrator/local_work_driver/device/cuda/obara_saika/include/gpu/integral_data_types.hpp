#pragma once
#include <cmath>
#include <gauxc/shell_pair.hpp>

namespace XGPU {

  typedef struct {
    double x, y, z;
  } point;

  typedef struct {
    double alpha, coeff;
  } coefficients;

  typedef struct {
    point origin;
    coefficients *coeff;
    int m, L;
  } shells;

#if 0
  typedef struct {
    point P;
    point PA;
    point PB;

    double K_coeff_prod;
    double gamma;
    double gamma_inv;
  } prim_pair;
#else
  using prim_pair = GauXC::PrimitivePair<double>;
  using shell_pair = GauXC::ShellPair<double>;
#endif

}

