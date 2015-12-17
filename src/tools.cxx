#include "tools.hh"

#include "H5Cpp.h"

// _______________________________________________________________________
// rewieghting

FlavorPtEtaDistributions::FlavorPtEtaDistributions(const H5::CommonFG& grp) {
  for (int flavor: {0, 4, 5, 15}) {
    std::string name = std::to_string(flavor);
    m_dists.emplace(flavor, grp.openDataSet(name));
  }
}
double FlavorPtEtaDistributions::get(const InMap& kin, int flavor) const {
  return m_dists.at(flavor).get(kin);
}
