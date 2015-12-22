#include "FileCLI.hh"
#include "Jets.hh"
#include "SmartChain.hh"
#include "tools.hh"
#include "constants.hh"

#include "ndhist/Histogram.hh"

#include "H5Cpp.h"
#include "TROOT.h"

#include <iostream>
// #include <limits>

// const double GeV = 1000;
// const double MAX_PT = 1000*GeV;

// const double GeV = 1000;
// const double MAX_PT = 1000*GeV;
const int N_BINS = 200;
const std::string reweight_file = "reweight.h5";

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
  Histogram sv1_ntrkj;
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

  SmartChain chain("bTag_AntiKt4EMTopoJets");
  for (const auto& in: cli.in_files()) {
    chain.add(in);
  }
  Jets jets(chain);
  int n_entries = chain.GetEntries();
  std::cout << n_entries << " entries in chain" << std::endl;
  if (!exists(reweight_file)) {
    throw std::logic_error(reweight_file + " not found");
  }
  FlavorPtEtaDistributions pt_eta_reweight(
    H5::H5File(reweight_file, H5F_ACC_RDONLY));
  FlavoredHists hists;
  FlavoredHists reweighted_hists;

  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      hists.fill(jet);
      std::map<std::string, double> pt_eta{
        {"pt", jet.jet_pt},
        {"eta", std::abs(jet.jet_eta)}};
      double weight = pt_eta_reweight.get(pt_eta, jet.jet_truthflav);
      reweighted_hists.fill(jet, weight);
    }
  }

  // save histograms
  H5::H5File out_file(cli.out_file(), H5F_ACC_TRUNC);
  hists.save(out_file, "raw");
  reweighted_hists.save(out_file, "reweighted");

}

// ______________________________________________________________________
// hist methods

namespace {
  std::vector<Axis> range(std::string name,
                          double low, double high,
                          std::string units = "") {
    return { {name, N_BINS, low, high, units} };
  }
  std::vector<Axis> zero_to_one(std::string name) {
    return range(name, 0, 1.00001);
  }
  std::vector<Axis> count(std::string name, int max) {
    return { {name, max + 1, -0.5, max + 0.5} };
  }
}

const double MAX_VX_MASS = 10;

JetHists::JetHists():
#define ZERO_ONE(name) name( zero_to_one( #name) )
#define COUNT(name, max) name( count( #name, max) )
#define RANGE(name, low, high) name( range( #name, low, high) )
#define ENERGY(name, high) name( range( #name, 0, high, "MeV") )
  ZERO_ONE(mv2c00),
  ZERO_ONE(mv2c10),
  ZERO_ONE(mv2c20),
  ZERO_ONE(mv2c100),

  ZERO_ONE(ip3d_pu),
  ZERO_ONE(ip3d_pc),
  ZERO_ONE(ip3d_pb),

  COUNT(sv1_ntrkj, 30),
  COUNT(sv1_ntrkv, 20),
  COUNT(sv1_n2t, 20),
  ENERGY(sv1_m, MAX_VX_MASS),
  ZERO_ONE(sv1_efc),
  RANGE(sv1_normdist, 0, 10),
  COUNT(sv1_Nvtx, 10),
  RANGE(sv1_sig3d, 0, 10),

  ENERGY(jf_m, MAX_VX_MASS),
  ZERO_ONE(jf_efc),
  RANGE(jf_deta, 0, 4),
  RANGE(jf_dphi, 0, 3.2),
  COUNT(jf_ntrkAtVx, 10),
  COUNT(jf_nvtx, 4),
  RANGE(jf_sig3d, 0, 10),
  COUNT(jf_nvtx1t, 10),
  COUNT(jf_n2t, 10),
  COUNT(jf_VTXsize, 10),
#undef ZERO_ONE
#undef COUNT
#undef RANGE
#undef ENERGY

  pt({ {"pt", PT_REWEIGHT_NBINS, 0, PT_REWEIGHT_MAX, "MeV"} }),
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
  BYNAME(sv1_ntrkj);
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
  BYNAME(sv1_ntrkj);
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
