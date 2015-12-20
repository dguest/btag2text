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


JetHists::JetHists():
  mv2c00(N_BINS, 0, 1), mv2c10(N_BINS, 0, 1),
  pt({ {"pt", PT_REWEIGHT_NBINS, 0, PT_REWEIGHT_MAX, "MeV"} }),
  eta({ {"eta", N_BINS, -2.8, 2.8 }})
{
}

void JetHists::fill(const Jet& jet, double weight) {
#define BYNAME(name) name.fill(jet.jet_ ## name, weight)
  BYNAME(mv2c00);
  BYNAME(mv2c10);
  BYNAME(pt);
  BYNAME(eta);
#undef BYNAME
}

void JetHists::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(mv2c00);
  BYNAME(mv2c10);
  BYNAME(pt);
  BYNAME(eta);
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
