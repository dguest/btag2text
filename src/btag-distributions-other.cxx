#include "FileCLI.hh"
#include "Jets.hh"
#include "SmartChain.hh"
#include "FlavorPtEtaDistributions.hh"
#include "constants.hh"
#include "hist_tools.hh"
#include "select_jet.hh"

#include "ndhist/Histogram.hh"
#include "covol/Covariance.hh"

#include "H5Cpp.h"
#include "TROOT.h"

#include <iostream>

// various plotting constants
const double MAX_VX_MASS = 10*GeV;
const double JET_WIDTH = 0.5;
const double MAX_SIGNIFICANCE = 50;
const double MV2_HIGH = 1.00001;
const double MV2_LOW = -MV2_HIGH;

class JetHists
{
public:
  JetHists();
  void fill(const Jet& jet, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  Histogram mv2c00;
  Histogram mv2c10;
  Histogram mv2c20;
  Histogram mv2c100;

  Histogram ip3d_pu;
  Histogram ip3d_pc;
  Histogram ip3d_pb;

  Histogram ip3d_ntrk;
  Histogram ntrk;
  Histogram JVT;

  // Histogram sv1_ntrkj; // never seems to exist...
  Histogram sv1_ntrkv;
  Histogram sv1_n2t;
  Histogram sv1_m;
  Histogram sv1_efc;
  Histogram sv1_normdist;
  Histogram sv1_Nvtx;
  Histogram sv1_sig3d;
  Histogram jf_m;
  Histogram jf_efc;
  Histogram jf_deta;
  Histogram jf_dphi;
  Histogram jf_ntrkAtVx;
  Histogram jf_nvtx;
  Histogram jf_sig3d;
  Histogram jf_nvtx1t;
  Histogram jf_n2t;
  Histogram jf_VTXsize;

  Histogram pt;
  Histogram eta;
};

class FlavoredHists
{
public:
  void fill(const Jet& jet, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  std::map<int, JetHists> m_hists;
};

// _____________________________________________________________________
// main function

int main(int argc, char* argv[]) {
  gROOT->ProcessLine("#include <vector>");
  FileCLI cli(argc, argv);

  SmartChain chain(JET_COLLECTION);
  for (const auto& in: cli.in_files()) {
    chain.add(in);
  }
  Jets jets(chain);
  int n_entries = chain.GetEntries();
  std::cout << n_entries << " entries in chain" << std::endl;

  require(REWEIGHT_FILE);
  FlavorPtEtaDistributions pt_eta_reweight(
    H5::H5File(REWEIGHT_FILE, H5F_ACC_RDONLY));
  FlavoredHists hists;
  FlavoredHists reweighted_hists;

  Covariance jetcov( get_jet_variables() );

  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      if (! select_jet(jet) ) continue;
      hists.fill(jet);
      auto jet_vars = get_map(jet);
      double weight = pt_eta_reweight.get(jet_vars, jet.jet_truthflav);
      reweighted_hists.fill(jet, weight);
      jetcov.fill(jet_vars, weight);
    }
  }

  // save histograms
  using namespace grp;
  H5::H5File out_file(cli.out_file(), H5F_ACC_TRUNC);
  // hists
  auto hist_group = out_file.createGroup(HIST);
  hists.save(hist_group, RAW);
  reweighted_hists.save(hist_group, REWEIGHTED);
  // covariance
  auto cov_group = out_file.createGroup(COV);
  jetcov.write_to(cov_group, REWEIGHTED);

}

// ______________________________________________________________________
// hist methods

JetHists::JetHists():
  // MV2
  RANGE(mv2c00, MV2_LOW, MV2_HIGH),
  RANGE(mv2c10, MV2_LOW, MV2_HIGH),
  RANGE(mv2c20, MV2_LOW, MV2_HIGH),
  RANGE(mv2c100,MV2_LOW, MV2_HIGH),

  // IP3D
  ZERO_ONE(ip3d_pu),
  ZERO_ONE(ip3d_pc),
  ZERO_ONE(ip3d_pb),

  COUNT(ip3d_ntrk, 30),
  COUNT(ntrk, 30),
  ZERO_ONE(JVT),

  // SV1
  COUNT(sv1_ntrkv, 20),
  COUNT(sv1_n2t, 20),
  ENERGY(sv1_m, MAX_VX_MASS),
  ZERO_ONE(sv1_efc),
  RANGE(sv1_normdist, 0, MAX_SIGNIFICANCE),
  COUNT(sv1_Nvtx, 2),
  RANGE(sv1_sig3d, 0, MAX_SIGNIFICANCE),

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
  eta({ {"eta", N_BINS, -2.8, 2.8 }})
{
}

void JetHists::fill(const Jet& jet, double weight) {
#define BYNAME(name) name.fill(jet.jet_ ## name, weight)
  BYNAME(mv2c00);
  BYNAME(mv2c10);
  BYNAME(mv2c20);
  BYNAME(mv2c100);

  BYNAME(pt);
  BYNAME(eta);

  BYNAME(ip3d_pu);
  BYNAME(ip3d_pc);
  BYNAME(ip3d_pb);

  BYNAME(ip3d_ntrk);
  BYNAME(ntrk);

  BYNAME(JVT);

  // BYNAME(sv1_ntrkj);
  BYNAME(sv1_ntrkv);
  BYNAME(sv1_n2t);
  BYNAME(sv1_m);
  BYNAME(sv1_efc);
  BYNAME(sv1_normdist);
  BYNAME(sv1_Nvtx);
  BYNAME(sv1_sig3d);
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

#undef BYNAME
}

void JetHists::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(mv2c00);
  BYNAME(mv2c10);
  BYNAME(mv2c20);
  BYNAME(mv2c100);

  BYNAME(pt);
  BYNAME(eta);

  BYNAME(ip3d_pu);
  BYNAME(ip3d_pc);
  BYNAME(ip3d_pb);

  BYNAME(ip3d_ntrk);
  BYNAME(ntrk);

  BYNAME(JVT);

// BYNAME(sv1_ntrkj);
  BYNAME(sv1_ntrkv);
  BYNAME(sv1_n2t);
  BYNAME(sv1_m);
  BYNAME(sv1_efc);
  BYNAME(sv1_normdist);
  BYNAME(sv1_Nvtx);
  BYNAME(sv1_sig3d);
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

#undef BYNAME
}
void JetHists::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}

// ________________________________________________________________________
// flavored hists

void FlavoredHists::fill(const Jet& jet, double weight) {
  int ftl = jet.jet_truthflav;
  m_hists[ftl].fill(jet, weight);
}

void FlavoredHists::save(H5::CommonFG& out) const {
  for (const auto& flav_and_hist: m_hists) {
    std::string fg_name = std::to_string(flav_and_hist.first);
    flav_and_hist.second.save(out, fg_name);
  }
}
void FlavoredHists::save(H5::CommonFG& out, std::string name) const {
  H5::Group group(out.createGroup(name));
  save(group);
}
