#pragma once
#include <gauxc/oop_xc_integrator/replicated/replicated_xc_host_integrator.hpp>
#include <gauxc/oop_xc_integrator/replicated/impl.hpp>

namespace GauXC {

/// Factory to generate ReplicatedXCIntegrator instances
template <typename MatrixType>
struct ReplicatedXCIntegratorFactory {

  using integrator_type = detail::ReplicatedXCIntegrator<MatrixType>;
  using value_type      = typename integrator_type::value_type;
  using ptr_return_t    = std::unique_ptr<integrator_type>;

  
  /** Generate a ReplicatedXCIntegrator instance
   *
   *  @param[in]  ex                 Execution space for integrator instance
   *  @param[in]  integration_kernel Name of integration scaffold to load ("Default", "Reference", etc)
   *  @param[in]  func               XC functional to integrate
   *  @param[in]  lb                 Pregenerated LoadBalancer instance
   *  @param[in]  lwd                Local Work Driver
   */
  static ptr_return_t make_integrator_impl( 
    ExecutionSpace ex,
    std::string integrator_kernel,
    std::shared_ptr<functional_type>   func,
    std::shared_ptr<LoadBalancer>      lb,
    std::unique_ptr<LocalWorkDriver>&& lwd ) {

    using host_factory = 
      detail::ReplicatedXCHostIntegratorFactory<value_type>;

    switch(ex) {

      case ExecutionSpace::Host:
        return std::make_unique<integrator_type>( 
          host_factory::make_integrator_impl(
            integrator_kernel, func, lb, std::move(lwd) 
          )
        );

      default:
        throw std::runtime_error("ReplicatedXCIntegrator type not Recognized");
    }

    return nullptr;

  }

 
};


}
