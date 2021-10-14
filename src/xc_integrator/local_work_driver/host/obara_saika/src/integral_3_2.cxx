#include <math.h>
#include "boys_computation.h"
#include "integral_data_types.h"

#define PI 3.14159265358979323846

void integral_3_2(int npts,
                  shells shellA,
                  shells shellB,
                  point *_points,
                  double *Xi,
                  double *Xj,
                  int ldX,
                  double *Gi,
                  double *Gj,
                  int ldG, 
                  double *weights) {
   double temp[46];

   for(int point_idx = 0; point_idx < npts; ++point_idx) {
      point C = *(_points + point_idx);

      double xA = shellA.origin.x;
      double yA = shellA.origin.y;
      double zA = shellA.origin.z;

      double xB = shellB.origin.x;
      double yB = shellB.origin.y;
      double zB = shellB.origin.z;

      double X_AB = (xA - xB);
      double Y_AB = (yA - yB);
      double Z_AB = (zA - zB);

      double beta_in = 0.0;
      for(int i = 0; i < shellA.m; ++i) {
         for(int j = 0; j < shellB.m; ++j) {
            double aA = shellA.coeff[i].alpha;
            double cA = shellA.coeff[i].coeff;

            double aB = shellB.coeff[j].alpha;
            double cB = shellB.coeff[j].coeff;

            double RHO = aA + aB;
            double RHO_INV = 1.0 / RHO;

            double xC = C.x;
            double yC = C.y;
            double zC = C.z;

            double xP = (aA * xA + aB * xB) * RHO_INV;
            double yP = (aA * yA + aB * yB) * RHO_INV;
            double zP = (aA * zA + aB * zB) * RHO_INV;

            double X_PB = (xP - xB);
            double Y_PB = (yP - yB);
            double Z_PB = (zP - zB);

            double X_PC = (xP - xC);
            double Y_PC = (yP - yC);
            double Z_PC = (zP - zC);

            double t00, t01, t02, t03, t04, t05, t10, t11, t12, t13, t14, t20, t21, t22, t23, t30, t31, t32, t40, t41, t50;

            double eval = cA * cB * 2 * PI * RHO_INV * exp(-1.0 * (X_AB * X_AB + Y_AB * Y_AB + Z_AB * Z_AB) * aA * aB * RHO_INV);
            double tval = RHO * (X_PC * X_PC + Y_PC * Y_PC + Z_PC * Z_PC);

#ifdef BOYS_REFERENCE
            t00 = eval * boys_reference(0, tval);
            t01 = eval * boys_reference(1, tval);
            t02 = eval * boys_reference(2, tval);
            t03 = eval * boys_reference(3, tval);
            t04 = eval * boys_reference(4, tval);
            t05 = eval * boys_reference(5, tval);
#elif BOYS_ASYMP
            t00 = eval * boys_asymp(0, tval);
            t01 = eval * boys_asymp(1, tval);
            t02 = eval * boys_asymp(2, tval);
            t03 = eval * boys_asymp(3, tval);
            t04 = eval * boys_asymp(4, tval);
            t05 = eval * boys_asymp(5, tval);
#else
            #error "TYPE NOT DEFINED!"
#endif

            t10 = X_PB * t00 - X_PC * t01;
            t11 = X_PB * t01 - X_PC * t02;
            t12 = X_PB * t02 - X_PC * t03;
            t13 = X_PB * t03 - X_PC * t04;
            t14 = X_PB * t04 - X_PC * t05;
            t20 = X_PB * t10 - X_PC * t11 + 0.5 * RHO_INV * 1 * (t00 - t01);
            t21 = X_PB * t11 - X_PC * t12 + 0.5 * RHO_INV * 1 * (t01 - t02);
            t22 = X_PB * t12 - X_PC * t13 + 0.5 * RHO_INV * 1 * (t02 - t03);
            t23 = X_PB * t13 - X_PC * t14 + 0.5 * RHO_INV * 1 * (t03 - t04);
            t30 = X_PB * t20 - X_PC * t21 + 0.5 * RHO_INV * 2 * (t10 - t11);
            t31 = X_PB * t21 - X_PC * t22 + 0.5 * RHO_INV * 2 * (t11 - t12);
            t32 = X_PB * t22 - X_PC * t23 + 0.5 * RHO_INV * 2 * (t12 - t13);
            *(temp + 0) = beta_in * (*(temp + 0)) + t30;

            t40 = X_PB * t30 - X_PC * t31 + 0.5 * RHO_INV * 3 * (t20 - t21);
            t41 = X_PB * t31 - X_PC * t32 + 0.5 * RHO_INV * 3 * (t21 - t22);
            *(temp + 10) = beta_in * (*(temp + 10)) + t40;

            t50 = X_PB * t40 - X_PC * t41 + 0.5 * RHO_INV * 4 * (t30 - t31);
            *(temp + 25) = beta_in * (*(temp + 25)) + t50;

            t50 = Y_PB * t40 - Y_PC * t41;
            *(temp + 26) = beta_in * (*(temp + 26)) + t50;

            t50 = Z_PB * t40 - Z_PC * t41;
            *(temp + 27) = beta_in * (*(temp + 27)) + t50;

            t40 = Y_PB * t30 - Y_PC * t31;
            t41 = Y_PB * t31 - Y_PC * t32;
            *(temp + 11) = beta_in * (*(temp + 11)) + t40;

            t50 = Y_PB * t40 - Y_PC * t41 + 0.5 * RHO_INV * 1 * (t30 - t31);
            *(temp + 28) = beta_in * (*(temp + 28)) + t50;

            t50 = Z_PB * t40 - Z_PC * t41;
            *(temp + 29) = beta_in * (*(temp + 29)) + t50;

            t40 = Z_PB * t30 - Z_PC * t31;
            t41 = Z_PB * t31 - Z_PC * t32;
            *(temp + 12) = beta_in * (*(temp + 12)) + t40;

            t50 = Z_PB * t40 - Z_PC * t41 + 0.5 * RHO_INV * 1 * (t30 - t31);
            *(temp + 30) = beta_in * (*(temp + 30)) + t50;

            t30 = Y_PB * t20 - Y_PC * t21;
            t31 = Y_PB * t21 - Y_PC * t22;
            t32 = Y_PB * t22 - Y_PC * t23;
            *(temp + 1) = beta_in * (*(temp + 1)) + t30;

            t40 = Y_PB * t30 - Y_PC * t31 + 0.5 * RHO_INV * 1 * (t20 - t21);
            t41 = Y_PB * t31 - Y_PC * t32 + 0.5 * RHO_INV * 1 * (t21 - t22);
            *(temp + 13) = beta_in * (*(temp + 13)) + t40;

            t50 = Y_PB * t40 - Y_PC * t41 + 0.5 * RHO_INV * 2 * (t30 - t31);
            *(temp + 31) = beta_in * (*(temp + 31)) + t50;

            t50 = Z_PB * t40 - Z_PC * t41;
            *(temp + 32) = beta_in * (*(temp + 32)) + t50;

            t40 = Z_PB * t30 - Z_PC * t31;
            t41 = Z_PB * t31 - Z_PC * t32;
            *(temp + 14) = beta_in * (*(temp + 14)) + t40;

            t50 = Z_PB * t40 - Z_PC * t41 + 0.5 * RHO_INV * 1 * (t30 - t31);
            *(temp + 33) = beta_in * (*(temp + 33)) + t50;

            t30 = Z_PB * t20 - Z_PC * t21;
            t31 = Z_PB * t21 - Z_PC * t22;
            t32 = Z_PB * t22 - Z_PC * t23;
            *(temp + 2) = beta_in * (*(temp + 2)) + t30;

            t40 = Z_PB * t30 - Z_PC * t31 + 0.5 * RHO_INV * 1 * (t20 - t21);
            t41 = Z_PB * t31 - Z_PC * t32 + 0.5 * RHO_INV * 1 * (t21 - t22);
            *(temp + 15) = beta_in * (*(temp + 15)) + t40;

            t50 = Z_PB * t40 - Z_PC * t41 + 0.5 * RHO_INV * 2 * (t30 - t31);
            *(temp + 34) = beta_in * (*(temp + 34)) + t50;

            t20 = Y_PB * t10 - Y_PC * t11;
            t21 = Y_PB * t11 - Y_PC * t12;
            t22 = Y_PB * t12 - Y_PC * t13;
            t23 = Y_PB * t13 - Y_PC * t14;
            t30 = Y_PB * t20 - Y_PC * t21 + 0.5 * RHO_INV * 1 * (t10 - t11);
            t31 = Y_PB * t21 - Y_PC * t22 + 0.5 * RHO_INV * 1 * (t11 - t12);
            t32 = Y_PB * t22 - Y_PC * t23 + 0.5 * RHO_INV * 1 * (t12 - t13);
            *(temp + 3) = beta_in * (*(temp + 3)) + t30;

            t40 = Y_PB * t30 - Y_PC * t31 + 0.5 * RHO_INV * 2 * (t20 - t21);
            t41 = Y_PB * t31 - Y_PC * t32 + 0.5 * RHO_INV * 2 * (t21 - t22);
            *(temp + 16) = beta_in * (*(temp + 16)) + t40;

            t50 = Y_PB * t40 - Y_PC * t41 + 0.5 * RHO_INV * 3 * (t30 - t31);
            *(temp + 35) = beta_in * (*(temp + 35)) + t50;

            t50 = Z_PB * t40 - Z_PC * t41;
            *(temp + 36) = beta_in * (*(temp + 36)) + t50;

            t40 = Z_PB * t30 - Z_PC * t31;
            t41 = Z_PB * t31 - Z_PC * t32;
            *(temp + 17) = beta_in * (*(temp + 17)) + t40;

            t50 = Z_PB * t40 - Z_PC * t41 + 0.5 * RHO_INV * 1 * (t30 - t31);
            *(temp + 37) = beta_in * (*(temp + 37)) + t50;

            t30 = Z_PB * t20 - Z_PC * t21;
            t31 = Z_PB * t21 - Z_PC * t22;
            t32 = Z_PB * t22 - Z_PC * t23;
            *(temp + 4) = beta_in * (*(temp + 4)) + t30;

            t40 = Z_PB * t30 - Z_PC * t31 + 0.5 * RHO_INV * 1 * (t20 - t21);
            t41 = Z_PB * t31 - Z_PC * t32 + 0.5 * RHO_INV * 1 * (t21 - t22);
            *(temp + 18) = beta_in * (*(temp + 18)) + t40;

            t50 = Z_PB * t40 - Z_PC * t41 + 0.5 * RHO_INV * 2 * (t30 - t31);
            *(temp + 38) = beta_in * (*(temp + 38)) + t50;

            t20 = Z_PB * t10 - Z_PC * t11;
            t21 = Z_PB * t11 - Z_PC * t12;
            t22 = Z_PB * t12 - Z_PC * t13;
            t23 = Z_PB * t13 - Z_PC * t14;
            t30 = Z_PB * t20 - Z_PC * t21 + 0.5 * RHO_INV * 1 * (t10 - t11);
            t31 = Z_PB * t21 - Z_PC * t22 + 0.5 * RHO_INV * 1 * (t11 - t12);
            t32 = Z_PB * t22 - Z_PC * t23 + 0.5 * RHO_INV * 1 * (t12 - t13);
            *(temp + 5) = beta_in * (*(temp + 5)) + t30;

            t40 = Z_PB * t30 - Z_PC * t31 + 0.5 * RHO_INV * 2 * (t20 - t21);
            t41 = Z_PB * t31 - Z_PC * t32 + 0.5 * RHO_INV * 2 * (t21 - t22);
            *(temp + 19) = beta_in * (*(temp + 19)) + t40;

            t50 = Z_PB * t40 - Z_PC * t41 + 0.5 * RHO_INV * 3 * (t30 - t31);
            *(temp + 39) = beta_in * (*(temp + 39)) + t50;

            t10 = Y_PB * t00 - Y_PC * t01;
            t11 = Y_PB * t01 - Y_PC * t02;
            t12 = Y_PB * t02 - Y_PC * t03;
            t13 = Y_PB * t03 - Y_PC * t04;
            t14 = Y_PB * t04 - Y_PC * t05;
            t20 = Y_PB * t10 - Y_PC * t11 + 0.5 * RHO_INV * 1 * (t00 - t01);
            t21 = Y_PB * t11 - Y_PC * t12 + 0.5 * RHO_INV * 1 * (t01 - t02);
            t22 = Y_PB * t12 - Y_PC * t13 + 0.5 * RHO_INV * 1 * (t02 - t03);
            t23 = Y_PB * t13 - Y_PC * t14 + 0.5 * RHO_INV * 1 * (t03 - t04);
            t30 = Y_PB * t20 - Y_PC * t21 + 0.5 * RHO_INV * 2 * (t10 - t11);
            t31 = Y_PB * t21 - Y_PC * t22 + 0.5 * RHO_INV * 2 * (t11 - t12);
            t32 = Y_PB * t22 - Y_PC * t23 + 0.5 * RHO_INV * 2 * (t12 - t13);
            *(temp + 6) = beta_in * (*(temp + 6)) + t30;

            t40 = Y_PB * t30 - Y_PC * t31 + 0.5 * RHO_INV * 3 * (t20 - t21);
            t41 = Y_PB * t31 - Y_PC * t32 + 0.5 * RHO_INV * 3 * (t21 - t22);
            *(temp + 20) = beta_in * (*(temp + 20)) + t40;

            t50 = Y_PB * t40 - Y_PC * t41 + 0.5 * RHO_INV * 4 * (t30 - t31);
            *(temp + 40) = beta_in * (*(temp + 40)) + t50;

            t50 = Z_PB * t40 - Z_PC * t41;
            *(temp + 41) = beta_in * (*(temp + 41)) + t50;

            t40 = Z_PB * t30 - Z_PC * t31;
            t41 = Z_PB * t31 - Z_PC * t32;
            *(temp + 21) = beta_in * (*(temp + 21)) + t40;

            t50 = Z_PB * t40 - Z_PC * t41 + 0.5 * RHO_INV * 1 * (t30 - t31);
            *(temp + 42) = beta_in * (*(temp + 42)) + t50;

            t30 = Z_PB * t20 - Z_PC * t21;
            t31 = Z_PB * t21 - Z_PC * t22;
            t32 = Z_PB * t22 - Z_PC * t23;
            *(temp + 7) = beta_in * (*(temp + 7)) + t30;

            t40 = Z_PB * t30 - Z_PC * t31 + 0.5 * RHO_INV * 1 * (t20 - t21);
            t41 = Z_PB * t31 - Z_PC * t32 + 0.5 * RHO_INV * 1 * (t21 - t22);
            *(temp + 22) = beta_in * (*(temp + 22)) + t40;

            t50 = Z_PB * t40 - Z_PC * t41 + 0.5 * RHO_INV * 2 * (t30 - t31);
            *(temp + 43) = beta_in * (*(temp + 43)) + t50;

            t20 = Z_PB * t10 - Z_PC * t11;
            t21 = Z_PB * t11 - Z_PC * t12;
            t22 = Z_PB * t12 - Z_PC * t13;
            t23 = Z_PB * t13 - Z_PC * t14;
            t30 = Z_PB * t20 - Z_PC * t21 + 0.5 * RHO_INV * 1 * (t10 - t11);
            t31 = Z_PB * t21 - Z_PC * t22 + 0.5 * RHO_INV * 1 * (t11 - t12);
            t32 = Z_PB * t22 - Z_PC * t23 + 0.5 * RHO_INV * 1 * (t12 - t13);
            *(temp + 8) = beta_in * (*(temp + 8)) + t30;

            t40 = Z_PB * t30 - Z_PC * t31 + 0.5 * RHO_INV * 2 * (t20 - t21);
            t41 = Z_PB * t31 - Z_PC * t32 + 0.5 * RHO_INV * 2 * (t21 - t22);
            *(temp + 23) = beta_in * (*(temp + 23)) + t40;

            t50 = Z_PB * t40 - Z_PC * t41 + 0.5 * RHO_INV * 3 * (t30 - t31);
            *(temp + 44) = beta_in * (*(temp + 44)) + t50;

            t10 = Z_PB * t00 - Z_PC * t01;
            t11 = Z_PB * t01 - Z_PC * t02;
            t12 = Z_PB * t02 - Z_PC * t03;
            t13 = Z_PB * t03 - Z_PC * t04;
            t14 = Z_PB * t04 - Z_PC * t05;
            t20 = Z_PB * t10 - Z_PC * t11 + 0.5 * RHO_INV * 1 * (t00 - t01);
            t21 = Z_PB * t11 - Z_PC * t12 + 0.5 * RHO_INV * 1 * (t01 - t02);
            t22 = Z_PB * t12 - Z_PC * t13 + 0.5 * RHO_INV * 1 * (t02 - t03);
            t23 = Z_PB * t13 - Z_PC * t14 + 0.5 * RHO_INV * 1 * (t03 - t04);
            t30 = Z_PB * t20 - Z_PC * t21 + 0.5 * RHO_INV * 2 * (t10 - t11);
            t31 = Z_PB * t21 - Z_PC * t22 + 0.5 * RHO_INV * 2 * (t11 - t12);
            t32 = Z_PB * t22 - Z_PC * t23 + 0.5 * RHO_INV * 2 * (t12 - t13);
            *(temp + 9) = beta_in * (*(temp + 9)) + t30;

            t40 = Z_PB * t30 - Z_PC * t31 + 0.5 * RHO_INV * 3 * (t20 - t21);
            t41 = Z_PB * t31 - Z_PC * t32 + 0.5 * RHO_INV * 3 * (t21 - t22);
            *(temp + 24) = beta_in * (*(temp + 24)) + t40;

            t50 = Z_PB * t40 - Z_PC * t41 + 0.5 * RHO_INV * 4 * (t30 - t31);
            *(temp + 45) = beta_in * (*(temp + 45)) + t50;

            beta_in = 1.0;
         }
      }

      double *Xik = (Xi + point_idx * ldX);
      double *Xjk = (Xj + point_idx * ldX);
      double *Gik = (Gi + point_idx * ldG);
      double *Gjk = (Gj + point_idx * ldG);

      for(int c0 = 0; c0 <= 2; ++c0) {
         for(int c1 = 0; c1 <= c0; ++c1) {
            int m = 2 - c0;
            int n = c0 - c1;
            int p = c1;

            int idxB = (((2 - m) * (2 - m + 1)) >> 1) + p;

            double X_ABp = 1.0, comb_m_i = 1.0;
            for(int i = 0; i <= m; ++i) {
               double rcp_i;

               double Y_ABp = 1.0, comb_n_j = 1.0;
               for(int j = 0; j <= n; ++j) {
                  double rcp_j;

                  double Z_ABp = 1.0, comb_p_k = 1.0;
                  for(int k = 0; k <= p; ++k) {
                     double rcp_k;
                     int mv, pv, Lv = 5 - i - j - k;

                     int offset = (Lv * (Lv + 1) * (Lv + 2) - 60) / 6;
                     double const_value = comb_m_i * comb_n_j * comb_p_k * X_ABp * Y_ABp * Z_ABp;

                     mv = 3 + m - i; pv = 0 + p - k;
                     double t0 = *(temp + offset + (((Lv - mv) * (Lv - mv + 1)) >> 1) + pv) * const_value;
                     *(Gjk + 0) += *(Xik + idxB) * t0;
                     *(Gik + idxB) += *(Xjk + 0) * t0;

                     mv = 2 + m - i; pv = 0 + p - k;
                     double t1 = *(temp + offset + (((Lv - mv) * (Lv - mv + 1)) >> 1) + pv) * const_value;
                     *(Gjk + 1) += *(Xik + idxB) * t1;
                     *(Gik + idxB) += *(Xjk + 1) * t1;

                     mv = 2 + m - i; pv = 1 + p - k;
                     double t2 = *(temp + offset + (((Lv - mv) * (Lv - mv + 1)) >> 1) + pv) * const_value;
                     *(Gjk + 2) += *(Xik + idxB) * t2;
                     *(Gik + idxB) += *(Xjk + 2) * t2;

                     mv = 1 + m - i; pv = 0 + p - k;
                     double t3 = *(temp + offset + (((Lv - mv) * (Lv - mv + 1)) >> 1) + pv) * const_value;
                     *(Gjk + 3) += *(Xik + idxB) * t3;
                     *(Gik + idxB) += *(Xjk + 3) * t3;

                     mv = 1 + m - i; pv = 1 + p - k;
                     double t4 = *(temp + offset + (((Lv - mv) * (Lv - mv + 1)) >> 1) + pv) * const_value;
                     *(Gjk + 4) += *(Xik + idxB) * t4;
                     *(Gik + idxB) += *(Xjk + 4) * t4;

                     mv = 1 + m - i; pv = 2 + p - k;
                     double t5 = *(temp + offset + (((Lv - mv) * (Lv - mv + 1)) >> 1) + pv) * const_value;
                     *(Gjk + 5) += *(Xik + idxB) * t5;
                     *(Gik + idxB) += *(Xjk + 5) * t5;

                     mv = 0 + m - i; pv = 0 + p - k;
                     double t6 = *(temp + offset + (((Lv - mv) * (Lv - mv + 1)) >> 1) + pv) * const_value;
                     *(Gjk + 6) += *(Xik + idxB) * t6;
                     *(Gik + idxB) += *(Xjk + 6) * t6;

                     mv = 0 + m - i; pv = 1 + p - k;
                     double t7 = *(temp + offset + (((Lv - mv) * (Lv - mv + 1)) >> 1) + pv) * const_value;
                     *(Gjk + 7) += *(Xik + idxB) * t7;
                     *(Gik + idxB) += *(Xjk + 7) * t7;

                     mv = 0 + m - i; pv = 2 + p - k;
                     double t8 = *(temp + offset + (((Lv - mv) * (Lv - mv + 1)) >> 1) + pv) * const_value;
                     *(Gjk + 8) += *(Xik + idxB) * t8;
                     *(Gik + idxB) += *(Xjk + 8) * t8;

                     mv = 0 + m - i; pv = 3 + p - k;
                     double t9 = *(temp + offset + (((Lv - mv) * (Lv - mv + 1)) >> 1) + pv) * const_value;
                     *(Gjk + 9) += *(Xik + idxB) * t9;
                     *(Gik + idxB) += *(Xjk + 9) * t9;

                     Z_ABp *= (-1.0) * Z_AB; rcp_k = 1.0 / (1.0 * (k + 1)); comb_p_k = (comb_p_k * (p - k)) * rcp_k;
                  }

                  Y_ABp *= (-1.0) * Y_AB; rcp_j = 1.0 / (1.0 * (j + 1)); comb_n_j = (comb_n_j * (n - j)) * rcp_j;
               }

               X_ABp *= (-1.0) * X_AB; rcp_i = 1.0 / (1.0 * (i + 1)); comb_m_i = (comb_m_i * (m - i)) * rcp_i;
            }
         }
      }

      *(Gjk + 0) *= *(weights + point_idx);
      *(Gjk + 1) *= *(weights + point_idx);
      *(Gjk + 2) *= *(weights + point_idx);
      *(Gjk + 3) *= *(weights + point_idx);
      *(Gjk + 4) *= *(weights + point_idx);
      *(Gjk + 5) *= *(weights + point_idx);
      *(Gjk + 6) *= *(weights + point_idx);
      *(Gjk + 7) *= *(weights + point_idx);
      *(Gjk + 8) *= *(weights + point_idx);
      *(Gjk + 9) *= *(weights + point_idx);

      *(Gik + 0) *= *(weights + point_idx);
      *(Gik + 1) *= *(weights + point_idx);
      *(Gik + 2) *= *(weights + point_idx);
      *(Gik + 3) *= *(weights + point_idx);
      *(Gik + 4) *= *(weights + point_idx);
      *(Gik + 5) *= *(weights + point_idx);
   }
}
