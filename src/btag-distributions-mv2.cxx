#include "FileCLI.hh"
#include "Jets.hh"
#include "SmartChain.hh"
#include "FlavorPtEtaDistributions.hh"
#include "constants.hh"
#include "hist_tools.hh"
#include "select_jet.hh"

#include "ndhist/Histogram.hh"
// #include "covol/Covariance.hh"

#include "H5Cpp.h"
#include "TROOT.h"

#include <iostream>

// various plotting constants
const double MAX_VX_MASS = 10*GeV;
const double JET_WIDTH = 0.5;
const double MAX_SIGNIFICANCE = 50;
const double MV2_HIGH = 1.00001;
const double MV2_LOW = -MV2_HIGH;
const int MV2_2D_BINS = 10;

class JetHists
{
public:
  JetHists();
  void fill(const Jet& jet, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  Histogram mv2c00;
  Histogram mv2c100;
  Histogram mv2;
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
    }
  }

  // save histograms
  using namespace grp;
  H5::H5File out_file(cli.out_file(), H5F_ACC_TRUNC);
  // hists
  auto hist_group = out_file.createGroup(HIST);
  hists.save(hist_group, RAW);
  reweighted_hists.save(hist_group, REWEIGHTED);
}

// ______________________________________________________________________
// hist methods

JetHists::JetHists():
  // MV2
  RANGE(mv2c00, MV2_LOW, MV2_HIGH),
  RANGE(mv2c100, MV2_LOW, MV2_HIGH),
  mv2({ {"mv2c100", MV2_2D_BINS, MV2_LOW, MV2_HIGH},
      {"mv2c00", MV2_2D_BINS, MV2_LOW, MV2_HIGH} })
{
}

void JetHists::fill(const Jet& jet, double weight) {
#define BYNAME(name) name.fill(jet.jet_ ## name, weight)
  BYNAME(mv2c00);
  BYNAME(mv2c100);
#undef BYNAME
  mv2.fill(
    { {"mv2c00", jet.jet_mv2c00},
      {"mv2c100", jet.jet_mv2c100} }, weight);
}

void JetHists::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(mv2);
  BYNAME(mv2c00);
  BYNAME(mv2c100);
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
