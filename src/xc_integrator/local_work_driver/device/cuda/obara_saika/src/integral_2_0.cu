/**
 * GauXC Copyright (c) 2020-2023, The Regents of the University of California,
 * through Lawrence Berkeley National Laboratory (subject to receipt of
 * any required approvals from the U.S. Dept. of Energy). All rights reserved.
 *
 * See LICENSE.txt for retails
 */
#include <math.h>
#include "../include/gpu/chebyshev_boys_computation.hpp"
#include "config_obara_saika.hpp"
#include "integral_2_0.hu"

namespace XGPU {
  __inline__ __device__ void dev_integral_2_0_driver(size_t npts,
				   double *_points_x,
				   double *_points_y,
				   double *_points_z,
           shell_pair* sp,
				   double *Xi,
				   double *Xj,
				   int ldX,
				   double *Gi,
				   double *Gj,
				   int ldG, 
				   double *weights, 
				   double *boys_table) {
    //__shared__ double temp[128 * 6];
    double temp_0, temp_1, temp_2, temp_3, temp_4, temp_5;
    const auto nprim_pairs = sp->nprim_pairs();
    const auto prim_pairs  = sp->prim_pairs();
    
    const int npts_int = (int) npts;

    for(int p_outer = blockIdx.x * blockDim.x; p_outer < npts_int; p_outer += gridDim.x * blockDim.x) {
      double *_point_outer_x = (_points_x + p_outer);
      double *_point_outer_y = (_points_y + p_outer);
      double *_point_outer_z = (_points_z + p_outer);

      int p_inner = threadIdx.x;
      if (threadIdx.x < npts_int - p_outer) {

      //for(int i = 0; i < 6; ++i) SCALAR_STORE((temp + i * blockDim.x + threadIdx.x), SCALAR_ZERO());
      temp_0 = SCALAR_ZERO();
      temp_1 = SCALAR_ZERO();
      temp_2 = SCALAR_ZERO();
      temp_3 = SCALAR_ZERO();
      temp_4 = SCALAR_ZERO();
      temp_5 = SCALAR_ZERO();

      for(int ij = 0; ij < nprim_pairs; ++ij) {
	double RHO = prim_pairs[ij].gamma;
	double RHO_INV = prim_pairs[ij].gamma_inv;
	double X_PA = prim_pairs[ij].PA.x;
	double Y_PA = prim_pairs[ij].PA.y;
	double Z_PA = prim_pairs[ij].PA.z;

	double xP = prim_pairs[ij].P.x;
	double yP = prim_pairs[ij].P.y;
	double zP = prim_pairs[ij].P.z;

	double eval = prim_pairs[ij].K_coeff_prod;

	// Evaluate T Values
	SCALAR_TYPE xC = SCALAR_LOAD((_point_outer_x + p_inner));
	SCALAR_TYPE yC = SCALAR_LOAD((_point_outer_y + p_inner));
	SCALAR_TYPE zC = SCALAR_LOAD((_point_outer_z + p_inner));

	SCALAR_TYPE X_PC = SCALAR_SUB(xP, xC);
	SCALAR_TYPE Y_PC = SCALAR_SUB(yP, yC);
	SCALAR_TYPE Z_PC = SCALAR_SUB(zP, zC);

	SCALAR_TYPE TVAL = SCALAR_MUL(X_PC, X_PC);
	TVAL = SCALAR_FMA(Y_PC, Y_PC, TVAL);
	TVAL = SCALAR_FMA(Z_PC, Z_PC, TVAL);
	TVAL = SCALAR_MUL(RHO, TVAL);

	SCALAR_TYPE t00, t01, t02, TVAL_inv_e;

	// Evaluate Boys function
	boys_element<2>(&TVAL, &TVAL_inv_e, &t02, boys_table);

	// Evaluate VRR Buffer
	SCALAR_TYPE t10, t11, t20, tx, ty;

	t01 = SCALAR_MUL(SCALAR_ADD(SCALAR_MUL(TVAL, t02), TVAL_inv_e), SCALAR_SET1(0.66666666666666662966));
	t00 = SCALAR_MUL(SCALAR_ADD(SCALAR_MUL(TVAL, t01), TVAL_inv_e), SCALAR_SET1(2.00000000000000000000));

	t00 = SCALAR_MUL(eval, t00);
	t01 = SCALAR_MUL(eval, t01);
	t02 = SCALAR_MUL(eval, t02);
	t10 = SCALAR_MUL(X_PA, t00);
	t10 = SCALAR_FNMA(X_PC, t01, t10);
	t11 = SCALAR_MUL(X_PA, t01);
	t11 = SCALAR_FNMA(X_PC, t02, t11);
	t20 = SCALAR_MUL(X_PA, t10);
	t20 = SCALAR_FNMA(X_PC, t11, t20);
	tx = SCALAR_SUB(t00, t01);
	ty = SCALAR_SET1(0.5 * 1);
	ty = SCALAR_MUL(ty, RHO_INV);
	t20 = SCALAR_FMA(tx, ty, t20);
	//tx = SCALAR_LOAD((temp + 0 * blockDim.x + threadIdx.x));
  tx = temp_0;
	tx = SCALAR_ADD(tx, t20);
	//SCALAR_STORE((temp + 0 * blockDim.x + threadIdx.x), tx);
  temp_0 = tx;
	t20 = SCALAR_MUL(Y_PA, t10);
	t20 = SCALAR_FNMA(Y_PC, t11, t20);
	//tx = SCALAR_LOAD((temp + 1 * blockDim.x + threadIdx.x));
  tx = temp_1;
	tx = SCALAR_ADD(tx, t20);
	//SCALAR_STORE((temp + 1 * blockDim.x + threadIdx.x), tx);
  temp_1 = tx;
	t20 = SCALAR_MUL(Z_PA, t10);
	t20 = SCALAR_FNMA(Z_PC, t11, t20);
	//tx = SCALAR_LOAD((temp + 2 * blockDim.x + threadIdx.x));
  tx = temp_2;
	tx = SCALAR_ADD(tx, t20);
	//SCALAR_STORE((temp + 2 * blockDim.x + threadIdx.x), tx);
  temp_2 = tx;
	t10 = SCALAR_MUL(Y_PA, t00);
	t10 = SCALAR_FNMA(Y_PC, t01, t10);
	t11 = SCALAR_MUL(Y_PA, t01);
	t11 = SCALAR_FNMA(Y_PC, t02, t11);
	t20 = SCALAR_MUL(Y_PA, t10);
	t20 = SCALAR_FNMA(Y_PC, t11, t20);
	tx = SCALAR_SUB(t00, t01);
	ty = SCALAR_SET1(0.5 * 1);
	ty = SCALAR_MUL(ty, RHO_INV);
	t20 = SCALAR_FMA(tx, ty, t20);
	//tx = SCALAR_LOAD((temp + 3 * blockDim.x + threadIdx.x));
  tx = temp_3;
	tx = SCALAR_ADD(tx, t20);
	//SCALAR_STORE((temp + 3 * blockDim.x + threadIdx.x), tx);
  temp_3 = tx;
	t20 = SCALAR_MUL(Z_PA, t10);
	t20 = SCALAR_FNMA(Z_PC, t11, t20);
	//tx = SCALAR_LOAD((temp + 4 * blockDim.x + threadIdx.x));
  tx = temp_4;
	tx = SCALAR_ADD(tx, t20);
	//SCALAR_STORE((temp + 4 * blockDim.x + threadIdx.x), tx);
  temp_4 = tx;
	t10 = SCALAR_MUL(Z_PA, t00);
	t10 = SCALAR_FNMA(Z_PC, t01, t10);
	t11 = SCALAR_MUL(Z_PA, t01);
	t11 = SCALAR_FNMA(Z_PC, t02, t11);
	t20 = SCALAR_MUL(Z_PA, t10);
	t20 = SCALAR_FNMA(Z_PC, t11, t20);
	tx = SCALAR_SUB(t00, t01);
	ty = SCALAR_SET1(0.5 * 1);
	ty = SCALAR_MUL(ty, RHO_INV);
	t20 = SCALAR_FMA(tx, ty, t20);
	//tx = SCALAR_LOAD((temp + 5 * blockDim.x + threadIdx.x));
  tx = temp_5;
	tx = SCALAR_ADD(tx, t20);
	//SCALAR_STORE((temp + 5 * blockDim.x + threadIdx.x), tx);
  temp_5 = tx;
      }

    if (
      abs(temp_0) > 1e-12 || abs(temp_1) > 1e-12 || abs(temp_2) > 1e-12 ||
      abs(temp_3) > 1e-12 || abs(temp_4) > 1e-12 || abs(temp_5) > 1e-12
    ) {
	double *Xik = (Xi + p_outer + p_inner);
	double *Xjk = (Xj + p_outer + p_inner);
	double *Gik = (Gi + p_outer + p_inner);
	double *Gjk = (Gj + p_outer + p_inner);

	SCALAR_TYPE const_value_v = SCALAR_LOAD((weights + p_outer + p_inner));

	double const_value, X_ABp, Y_ABp, Z_ABp, comb_m_i, comb_n_j, comb_p_k;
	SCALAR_TYPE const_value_w;
	SCALAR_TYPE tx, ty, tz, tw, t0, t1, t2, t3, t4, t5;

	X_ABp = 1.0; comb_m_i = 1.0;
	Y_ABp = 1.0; comb_n_j = 1.0;
	Z_ABp = 1.0; comb_p_k = 1.0;
	const_value = comb_m_i * comb_n_j * comb_p_k * X_ABp * Y_ABp * Z_ABp;
	const_value_w = SCALAR_MUL(const_value_v, const_value);
  #if 0
	tx = SCALAR_LOAD((Xik + 0 * ldX));
	ty = SCALAR_LOAD((Xjk + 0 * ldX));
	tz = SCALAR_LOAD((Gik + 0 * ldG));
	tw = SCALAR_LOAD((Gjk + 0 * ldG));
	//t0 = SCALAR_LOAD((temp + 0 * blockDim.x + threadIdx.x));
	t0 = temp_0;
	t0 = SCALAR_MUL(t0, const_value_w);
	tz = SCALAR_FMA(ty, t0, tz);
	tw = SCALAR_FMA(tx, t0, tw);
	SCALAR_STORE((Gik + 0 * ldG), tz);
	SCALAR_STORE((Gjk + 0 * ldG), tw);
	tx = SCALAR_LOAD((Xik + 1 * ldX));
	ty = SCALAR_LOAD((Xjk + 0 * ldX));
	tz = SCALAR_LOAD((Gik + 1 * ldG));
	tw = SCALAR_LOAD((Gjk + 0 * ldG));
	//t1 = SCALAR_LOAD((temp + 1 * blockDim.x + threadIdx.x));
	t1 = temp_1;
	t1 = SCALAR_MUL(t1, const_value_w);
	tz = SCALAR_FMA(ty, t1, tz);
	tw = SCALAR_FMA(tx, t1, tw);
	SCALAR_STORE((Gik + 1 * ldG), tz);
	SCALAR_STORE((Gjk + 0 * ldG), tw);
	tx = SCALAR_LOAD((Xik + 2 * ldX));
	ty = SCALAR_LOAD((Xjk + 0 * ldX));
	tz = SCALAR_LOAD((Gik + 2 * ldG));
	tw = SCALAR_LOAD((Gjk + 0 * ldG));
	//t2 = SCALAR_LOAD((temp + 2 * blockDim.x + threadIdx.x));
	t2 = temp_2;
	t2 = SCALAR_MUL(t2, const_value_w);
	tz = SCALAR_FMA(ty, t2, tz);
	tw = SCALAR_FMA(tx, t2, tw);
	SCALAR_STORE((Gik + 2 * ldG), tz);
	SCALAR_STORE((Gjk + 0 * ldG), tw);
	tx = SCALAR_LOAD((Xik + 3 * ldX));
	ty = SCALAR_LOAD((Xjk + 0 * ldX));
	tz = SCALAR_LOAD((Gik + 3 * ldG));
	tw = SCALAR_LOAD((Gjk + 0 * ldG));
	//t3 = SCALAR_LOAD((temp + 3 * blockDim.x + threadIdx.x));
	t3 = temp_3;
	t3 = SCALAR_MUL(t3, const_value_w);
	tz = SCALAR_FMA(ty, t3, tz);
	tw = SCALAR_FMA(tx, t3, tw);
	SCALAR_STORE((Gik + 3 * ldG), tz);
	SCALAR_STORE((Gjk + 0 * ldG), tw);
	tx = SCALAR_LOAD((Xik + 4 * ldX));
	ty = SCALAR_LOAD((Xjk + 0 * ldX));
	tz = SCALAR_LOAD((Gik + 4 * ldG));
	tw = SCALAR_LOAD((Gjk + 0 * ldG));
	//t4 = SCALAR_LOAD((temp + 4 * blockDim.x + threadIdx.x));
	t4 = temp_4;
	t4 = SCALAR_MUL(t4, const_value_w);
	tz = SCALAR_FMA(ty, t4, tz);
	tw = SCALAR_FMA(tx, t4, tw);
	SCALAR_STORE((Gik + 4 * ldG), tz);
	SCALAR_STORE((Gjk + 0 * ldG), tw);
	tx = SCALAR_LOAD((Xik + 5 * ldX));
	ty = SCALAR_LOAD((Xjk + 0 * ldX));
	tz = SCALAR_LOAD((Gik + 5 * ldG));
	tw = SCALAR_LOAD((Gjk + 0 * ldG));
	//t5 = SCALAR_LOAD((temp + 5 * blockDim.x + threadIdx.x));
	t5 = temp_5;
	t5 = SCALAR_MUL(t5, const_value_w);
	tz = SCALAR_FMA(ty, t5, tz);
	tw = SCALAR_FMA(tx, t5, tw);
	SCALAR_STORE((Gik + 5 * ldG), tz);
	SCALAR_STORE((Gjk + 0 * ldG), tw);
  #else
	tx = SCALAR_LOAD((Xik + 0 * ldX));
	ty = SCALAR_LOAD((Xjk + 0 * ldX));
	t0 = SCALAR_MUL(temp_0, const_value_w);
	tz = SCALAR_MUL(ty, t0);
	tw = SCALAR_MUL(tx, t0);
	atomicAdd((Gik + 0 * ldG), tz);

	tx = SCALAR_LOAD((Xik + 1 * ldX));
	t1 = SCALAR_MUL(temp_1, const_value_w);
	tz = SCALAR_MUL(ty, t1);
	tw = SCALAR_FMA(tx, t1, tw);
	atomicAdd((Gik + 1 * ldG), tz);

	tx = SCALAR_LOAD((Xik + 2 * ldX));
	t2 = SCALAR_MUL(temp_2, const_value_w);
	tz = SCALAR_MUL(ty, t2);
	tw = SCALAR_FMA(tx, t2, tw);
	atomicAdd((Gik + 2 * ldG), tz);

	tx = SCALAR_LOAD((Xik + 3 * ldX));
	t3 = SCALAR_MUL(temp_3, const_value_w);
	tz = SCALAR_MUL(ty, t3);
	tw = SCALAR_FMA(tx, t3, tw);
	atomicAdd((Gik + 3 * ldG), tz);

	tx = SCALAR_LOAD((Xik + 4 * ldX));
	t4 = SCALAR_MUL(temp_4, const_value_w);
	tz = SCALAR_MUL(ty, t4);
	tw = SCALAR_FMA(tx, t4, tw);
	atomicAdd((Gik + 4 * ldG), tz);

	tx = SCALAR_LOAD((Xik + 5 * ldX));
	t5 = SCALAR_MUL(temp_5, const_value_w);
	tz = SCALAR_MUL(ty, t5);
	tw = SCALAR_FMA(tx, t5, tw);
	atomicAdd((Gik + 5 * ldG), tz);

	atomicAdd((Gjk + 0 * ldG), tw);
  #endif
      }
      }
    }
  }

  __global__ void dev_integral_2_0(size_t npts,
				   double *points_x,
				   double *points_y,
				   double *points_z,
           shell_pair* sp,
				   double *Xi,
				   double *Xj,
				   int ldX,
				   double *Gi,
				   double *Gj,
				   int ldG, 
				   double *weights, 
				   double *boys_table) {
    dev_integral_2_0_driver( npts, points_x, points_y, points_z, sp, Xi, Xj, ldX,
      Gi, Gj, ldG, weights, boys_table );
  }

  void integral_2_0(size_t npts,
		    double *points_x,
		    double *points_y,
		    double *points_z,
        shell_pair* sp,
		    double *Xi,
		    double *Xj,
		    int ldX,
		    double *Gi,
		    double *Gj,
		    int ldG, 
		    double *weights, 
		  double *boys_table,
      cudaStream_t stream) {
    dev_integral_2_0<<<320, 128, 0, stream>>>(npts,
				   points_x,
				   points_y,
				   points_z,
           sp,
				   Xi,
				   Xj,
				   ldX,
				   Gi,
				   Gj,
				   ldG, 
				   weights,
				   boys_table);
  }

  template <bool swap>
  __inline__ __device__ void dev_integral_2_0_batched_driver(
           const GauXC::ShellPairToTaskDevice* sp2task,
           GauXC::XCDeviceTask*                device_tasks,
				   double *boys_table) {

    //if (sp2task->shell_pair_device->nprim_pairs() == 0) return;
    const int ntask = sp2task->ntask;
    for( int i_task = blockIdx.y; i_task < ntask; i_task += gridDim.y ) {
    
      const auto iT = sp2task->task_idx_device[i_task];
      const auto* task  = device_tasks + iT;
      const auto  npts  = task->npts;

      int i_off, j_off;
      if constexpr ( swap ) {
        j_off = sp2task->task_shell_off_row_device[i_task]*npts;
        i_off = sp2task->task_shell_off_col_device[i_task]*npts;
      } else {
        i_off = sp2task->task_shell_off_row_device[i_task]*npts;
        j_off = sp2task->task_shell_off_col_device[i_task]*npts;
      }


      dev_integral_2_0_driver( 
        npts,
        task->points_x,
        task->points_y,
        task->points_z,
        sp2task->shell_pair_device,
        task->fmat + i_off,
        task->fmat + j_off,
        npts,
        task->gmat + i_off,
        task->gmat + j_off,
        npts,
        task->weights, boys_table );
    }

  }

  template <bool swap>
  __global__ void dev_integral_2_0_batched(
           const GauXC::ShellPairToTaskDevice* sp2task,
           GauXC::XCDeviceTask*                device_tasks,
				   double *boys_table) {
    dev_integral_2_0_batched_driver<swap>(sp2task,device_tasks,boys_table);
  }



  void integral_2_0_batched(bool swap, size_t ntask_sp,
        const GauXC::ShellPairToTaskDevice* sp2task,
        GauXC::XCDeviceTask*                device_tasks,
		    double *boys_table,
        cudaStream_t stream) {

    int nthreads = 128;
    int nblocks_x = 160;
    int nblocks_y = ntask_sp;
    dim3 nblocks(nblocks_x, nblocks_y);

    if(swap)
      dev_integral_2_0_batched<true><<<nblocks,nthreads,0,stream>>>(
        sp2task, device_tasks, boys_table );
    else
      dev_integral_2_0_batched<false><<<nblocks,nthreads,0,stream>>>(
        sp2task, device_tasks, boys_table );

  }



  template <bool swap>
  __global__ void dev_integral_2_0_shell_batched(
           int nsp,
           const GauXC::ShellPairToTaskDevice* sp2task,
           GauXC::XCDeviceTask*                device_tasks,
				   double *boys_table) {
    for(int i = blockIdx.z; i < nsp; i+= gridDim.z ) {
      dev_integral_2_0_batched_driver<swap>(sp2task+i,device_tasks,boys_table);
    }
  }

  void integral_2_0_shell_batched(
        bool swap,
        size_t nsp,
        size_t max_ntask,
        const GauXC::ShellPairToTaskDevice* sp2task,
        GauXC::XCDeviceTask*                device_tasks,
		    double *boys_table,
        cudaStream_t stream) {

    int nthreads = 128;
    int nblocks_x = 1;
    int nblocks_y = max_ntask;
    int nblocks_z = nsp;
    dim3 nblocks(nblocks_x, nblocks_y, nblocks_z);
    if(swap)
      dev_integral_2_0_shell_batched<true><<<nblocks,nthreads,0,stream>>>(
        nsp, sp2task, device_tasks, boys_table );
    else
      dev_integral_2_0_shell_batched<false><<<nblocks,nthreads,0,stream>>>(
        nsp, sp2task, device_tasks, boys_table );
  }
}
