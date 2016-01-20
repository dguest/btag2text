#ifndef FLAVOR_PT_ETA_DISTRIBUTIONS_HH
#define FLAVOR_PT_ETA_DISTRIBUTIONS_HH

#include "ndhist/Distribution.hh"

namespace H5 {
  class CommonFG;
}

// _______________________________________________________________________
// rewieghting

class FlavorPtEtaDistributions
{
public:
  typedef std::map<std::string, double> InMap;
  FlavorPtEtaDistributions(const H5::CommonFG& input);
  double get(const InMap& kinematics, int flavor) const;
private:
  std::map<int, Distribution> m_dists;
};


#endif  // FLAVOR_PT_ETA_DISTRIBUTIONS_HH
