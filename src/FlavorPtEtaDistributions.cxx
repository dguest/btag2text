#include "FlavorPtEtaDistributions.hh"

#include "H5Cpp.h"

#include <stdexcept>

// _______________________________________________________________________
// rewieghting

FlavorPtEtaDistributions::FlavorPtEtaDistributions(const H5::CommonFG& grp) {
  for (int flavor: {0, 4, 5, 15}) {
    std::string name = std::to_string(flavor);
    try {
      m_dists.emplace(flavor, grp.openDataSet(name));
    } catch (H5::FileIException& exc) {
      throw std::logic_error("missing flavor: " + std::to_string(flavor));
    }
  }
}
double FlavorPtEtaDistributions::get(const InMap& kin, int flavor) const {
  return m_dists.at(flavor).get(kin);
}
