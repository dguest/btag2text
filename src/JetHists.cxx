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
  // MV2
  RANGE(mv2c10, MV2_LOW, MV2_HIGH),
  RANGE(mv2c20, MV2_LOW, MV2_HIGH),

  // IP3D
  ZERO_ONE(ip3d_pu),
  ZERO_ONE(ip3d_pc),
  ZERO_ONE(ip3d_pb),
  COUNT(ip3d_ntrk, 30),
  COUNT(ntrk, 30),

  // SV1
  COUNT(sv1_ntrkv, 20),
  COUNT(sv1_n2t, 20),
  ENERGY(sv1_m, MAX_VX_MASS),
  ZERO_ONE(sv1_efc),
  RANGE(sv1_normdist, 0, MAX_SIGNIFICANCE),
  COUNT(sv1_Nvtx, 2),

  // jetfitter
  ENERGY(jf_m, MAX_VX_MASS),
  ZERO_ONE(jf_efc),
  RANGE(jf_deta, 0, JET_WIDTH),
  RANGE(jf_dphi, 0, JET_WIDTH),
  COUNT(jf_ntrkAtVx, 10),
  COUNT(jf_nvtx, 4),
  RANGE(jf_sig3d, 0, MAX_SIGNIFICANCE),
  COUNT(jf_nvtx1t, 10),
  COUNT(jf_n2t, 15),
  COUNT(jf_VTXsize, 10),

  pt({ {"pt", PT_REWEIGHT_NBINS, 0, PT_REWEIGHT_MAX, BASE_UNITS} }),
  eta({ {"eta", N_BINS, -2.8, 2.8 }}),

  dphi_fatjet({ {"dphi_fatjet", N_BINS, -1.2, 1.2} })
{
}

void JetHists::fill(const Jet& jet, double weight) {
#define BYNAME(name) name.fill(jet.jet_ ## name, weight)
  BYNAME(mv2c10);
  BYNAME(mv2c20);

  BYNAME(ip3d_pu);
  BYNAME(ip3d_pc);
  BYNAME(ip3d_pb);
  BYNAME(ip3d_ntrk);
  BYNAME(ntrk);

  BYNAME(sv1_ntrkv);
  BYNAME(sv1_n2t);
  BYNAME(sv1_m);
  BYNAME(sv1_efc);
  BYNAME(sv1_normdist);
  BYNAME(sv1_Nvtx);

  BYNAME(jf_m);
  BYNAME(jf_efc);
  BYNAME(jf_deta);
  BYNAME(jf_dphi);
  BYNAME(jf_ntrkAtVx);
  BYNAME(jf_nvtx);
  BYNAME(jf_sig3d);
  BYNAME(jf_nvtx1t);
  BYNAME(jf_n2t);
  BYNAME(jf_VTXsize);

  BYNAME(pt);
  BYNAME(eta);
  if (m_is_subjet) {
    dphi_fatjet.fill(jet.dphi_fatjet, weight);
  }

#undef BYNAME
}

void JetHists::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(mv2c10);
  BYNAME(mv2c20);

  BYNAME(ip3d_pu);
  BYNAME(ip3d_pc);
  BYNAME(ip3d_pb);
  BYNAME(ip3d_ntrk);
  BYNAME(ntrk);

  BYNAME(sv1_ntrkv);
  BYNAME(sv1_n2t);
  BYNAME(sv1_m);
  BYNAME(sv1_efc);
  BYNAME(sv1_normdist);
  BYNAME(sv1_Nvtx);

  BYNAME(jf_m);
  BYNAME(jf_efc);
  BYNAME(jf_deta);
  BYNAME(jf_dphi);
  BYNAME(jf_ntrkAtVx);
  BYNAME(jf_nvtx);
  BYNAME(jf_sig3d);
  BYNAME(jf_nvtx1t);
  BYNAME(jf_n2t);
  BYNAME(jf_VTXsize);

  BYNAME(pt);
  BYNAME(eta);

  if (m_is_subjet) {
    BYNAME(dphi_fatjet);
  }

#undef BYNAME
}
void JetHists::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}
