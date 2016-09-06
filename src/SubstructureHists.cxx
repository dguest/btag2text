#include "SubstructureHists.hh"
#include "Jets.hh"
#include "constants.hh"
#include "hist_tools.hh"

#include "H5Cpp.h"

#include <cmath> // isnan

// various plotting constants
const double MAX_VX_MASS = 10*GeV;
const double JET_WIDTH = 0.5;
const double MAX_SIGNIFICANCE = 50;
const double MV2_HIGH = 1.00001;
const double MV2_LOW = -MV2_HIGH;

SubstructureHists::SubstructureHists():
  // IP3D
  RANGE(tau21, 0, 1.5),
  RANGE(c1, 0, 0.5),
  RANGE(c2, 0, 0.7),
  RANGE(c1_beta2, 0, 0.8),
  RANGE(c2_beta2, 0, 0.8),
  RANGE(d2, 0, 200),
  RANGE(d2_beta2, 0, 200)
{
}

void SubstructureHists::fill(const SubstructureMoments& mom, double weight) {
#define BYNAME(name) name.fill(mom.name, weight)
  BYNAME(tau21);
  BYNAME(c1);
  BYNAME(c2);
  BYNAME(c1_beta2);
  BYNAME(c2_beta2);
  BYNAME(d2);
  BYNAME(d2_beta2);

#undef BYNAME
}

void SubstructureHists::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(tau21);
  BYNAME(c1);
  BYNAME(c2);
  BYNAME(c1_beta2);
  BYNAME(c2_beta2);
  BYNAME(d2);
  BYNAME(d2_beta2);

#undef BYNAME
}
void SubstructureHists::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}
