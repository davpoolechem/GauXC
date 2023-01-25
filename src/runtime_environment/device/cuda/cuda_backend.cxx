/**
 * GauXC Copyright (c) 2020-2023, The Regents of the University of California,
 * through Lawrence Berkeley National Laboratory (subject to receipt of
 * any required approvals from the U.S. Dept. of Energy). All rights reserved.
 *
 * See LICENSE.txt for retails
 */
#include "cuda_backend.hpp"

namespace GauXC {

CUDABackend::CUDABackend() {

  // Create CUDA Stream and CUBLAS Handles and make them talk to eachother
  master_stream = std::make_shared< util::cuda_stream >();
  master_handle = std::make_shared< util::cublas_handle >();

  cublasSetStream( *master_handle, *master_stream );

#ifdef GAUXC_ENABLE_MAGMA
  // Setup MAGMA queue with CUDA stream / cuBLAS handle
  master_magma_queue_ = std::make_shared< util::magma_queue >(0, *master_stream, *master_handle);
#endif

}

CUDABackend::~CUDABackend() noexcept = default;

CUDABackend::device_buffer_t CUDABackend::allocate_device_buffer(int64_t sz) {
  void* ptr;
  auto stat = cudaMalloc(&ptr, sz);
  GAUXC_CUDA_ERROR( "CUDA Malloc Failed", stat );
  return device_buffer_t{ptr,sz};
}

size_t CUDABackend::get_available_mem() {
  size_t cuda_avail, cuda_total;
  auto stat = cudaMemGetInfo( &cuda_avail, &cuda_total );
  GAUXC_CUDA_ERROR( "MemInfo Failed", stat );
  return cuda_avail;
}

void CUDABackend::free_device_buffer( void* ptr ) {
  cudaFree(ptr);
}

void CUDABackend::master_queue_synchronize() {
  cudaStreamSynchronize( *master_stream );
}


device_queue CUDABackend::queue() {
  return device_queue(master_stream);
}



void CUDABackend::create_blas_queue_pool(int32_t ns) {
  blas_streams.resize(ns);
  blas_handles.resize(ns);
  for( auto i = 0; i < ns; ++i ) {
    blas_streams[i] = std::make_shared<util::cuda_stream>();
    blas_handles[i] = std::make_shared<util::cublas_handle>();
    cublasSetStream( *blas_handles[i], *blas_streams[i] );
  }
}

void CUDABackend::sync_master_with_blas_pool() {
  const auto n_streams = blas_streams.size();
  std::vector<util::cuda_event> blas_events( n_streams );
  for( size_t iS = 0; iS < n_streams; ++iS )
    blas_events[iS].record( *blas_streams[iS] );

  for( auto& event : blas_events ) master_stream->wait(event);
}

void CUDABackend::sync_blas_pool_with_master() {
  util::cuda_event master_event;
  master_event.record( *master_stream );
  for( auto& stream : blas_streams ) stream->wait( master_event );
}

size_t CUDABackend::blas_pool_size(){ return blas_streams.size(); }

device_queue CUDABackend::blas_pool_queue(int32_t i) {
  return device_queue( blas_streams.at(i) );
}

device_blas_handle CUDABackend::blas_pool_handle(int32_t i) {
  return device_blas_handle( blas_handles.at(i) );
}
device_blas_handle CUDABackend::master_blas_handle() {
  return device_blas_handle( master_handle );
}










void CUDABackend::copy_async_( size_t sz, const void* src, void* dest,
  std::string msg ) {
  auto stat = cudaMemcpyAsync( dest, src, sz, cudaMemcpyDefault, *master_stream );
  GAUXC_CUDA_ERROR( "CUDA Memcpy Async Failed ["+msg+"]", stat );
}

void CUDABackend::set_zero_(size_t sz, void* data, std::string msg ) {
  auto stat = cudaMemset( data, 0, sz );
  GAUXC_CUDA_ERROR( "CUDA Memset Failed ["+msg+"]", stat );
}

void CUDABackend::set_zero_async_master_queue_(size_t sz, void* data, std::string msg ) {
  auto stat = cudaMemsetAsync( data, 0, sz, *master_stream );
  GAUXC_CUDA_ERROR( "CUDA Memset Failed ["+msg+"]", stat );
}

void CUDABackend::copy_async_2d_( size_t M, size_t N, const void* A, size_t LDA,
  void* B, size_t LDB, std::string msg ) {
  auto stat = cudaMemcpy2DAsync( B, LDB, A, LDA, M, N, cudaMemcpyDefault,
    *master_stream );
  GAUXC_CUDA_ERROR( "CUDA 2D Memcpy Async Failed ["+msg+"]", stat );
}



std::unique_ptr<DeviceBackend> make_device_backend() {
  return std::make_unique<CUDABackend>();
}
}
