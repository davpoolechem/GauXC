#pragma once

#include <gauxc/oop_xc_integrator/local_work_driver.hpp>
#include <string>

namespace GauXC {


/// Base type for all types that specify LWD settings (trivial)
struct LocalWorkSettings { virtual ~LocalWorkSettings() noexcept = default; };

/// Factory to generate LocalWorkDriver instances
class LocalWorkDriverFactory {

public:

  using ptr_return_t = std::unique_ptr<LocalWorkDriver>;

  /** Generate a LWD instance
   * 
   *  @param[in] ex        The Execution space for the LWD driver
   *  @param[in] name      The name of the LWD driver to construct (e.g. "Default" or "Reference")
   *  @param[in] settings  Settings to pass to LWD construction
   */
  static ptr_return_t make_local_work_driver(ExecutionSpace ex, 
    std::string name = "Default", 
    LocalWorkSettings settings = LocalWorkSettings());

private:

  static ptr_return_t make_reference_host_driver();

};

}
