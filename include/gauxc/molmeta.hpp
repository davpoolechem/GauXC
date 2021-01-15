#pragma once

#include <gauxc/molecule.hpp>

namespace GauXC {

class MolMeta {

  size_t              natoms_;
  std::vector<double> rab_;
  std::vector<double> dist_nearest_; 

  void compute_rab(const Molecule&);
  void compute_dist_nearest();

public:

  MolMeta() = delete;
  MolMeta( const Molecule& );

  MolMeta( const MolMeta & );
  MolMeta( MolMeta&& ) noexcept;

  ~MolMeta() noexcept;

  size_t natoms() const { return natoms_; }

  const auto& rab()          const { return rab_; }
        auto& rab()                { return rab_; }

  const auto& dist_nearest() const { return dist_nearest_; }
        auto& dist_nearest()       { return dist_nearest_; }

  template <typename Archive>
  void serialize( Archive& ar ) {
    ar( natoms_, rab_, dist_nearest_ );
  }

};

}
