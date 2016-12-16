#include "JetHists.hh"
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


JetHists::JetHists(bool is_subjet):
  m_is_subjet(is_subjet),
  COUNT(ntrk, 80),
  COUNT(ncluster, 80),

  pt({ {"pt", PT_REWEIGHT_NBINS, 0, PT_REWEIGHT_MAX, BASE_UNITS} }),
  eta({ {"eta", N_BINS, -2.8, 2.8 }}),

  dphi_fatjet({ {"dphi_fatjet", N_BINS, -1.2, 1.2} })
{
}

void JetHists::fill(const Jet& jet, double weight) {
#define BYNAME(name) name.fill(jet.jet_ ## name, weight)
  BYNAME(ntrk);

  BYNAME(pt);
  BYNAME(eta);
  if (m_is_subjet) {
    dphi_fatjet.fill(jet.dphi_fatjet, weight);
  } else {
    BYNAME(ncluster);
  }

#undef BYNAME
}

void JetHists::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(ntrk);

  BYNAME(pt);
  BYNAME(eta);

  if (m_is_subjet) {
    BYNAME(dphi_fatjet);
  } else {
    BYNAME(ncluster);
  }


#undef BYNAME
}
void JetHists::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}
