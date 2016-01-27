#include "FileCLI.hh"
#include "Jets.hh"
#include "SmartChain.hh"
#include "FlavorPtEtaDistributions.hh"
#include "constants.hh"
#include "hist_tools.hh"

#include "ndhist/Histogram.hh"

#include "H5Cpp.h"
#include "TROOT.h"

#include <iostream>

// various plotting constants
// const double MAX_VX_MASS = 10*GeV;
// const double JET_WIDTH = 0.5;
// const double MAX_SIGNIFICANCE = 50;

class TrackHists
{
public:
  TrackHists();
  void fill(const Track& track, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  Histogram pt;
  Histogram eta;
};

class FlavoredHists
{
public:
  void fill(const Track& track, int flavor, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  std::map<int, TrackHists> m_hists;
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

  require(REWEIGHT_FILE);
  FlavorPtEtaDistributions pt_eta_reweight(
    H5::H5File(REWEIGHT_FILE, H5F_ACC_RDONLY));
  FlavoredHists hists;

  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      std::map<std::string, double> pt_eta{
        {"pt", jet.jet_pt},
        {"eta", std::abs(jet.jet_eta)}};
      double weight = pt_eta_reweight.get(pt_eta, jet.jet_truthflav);
      for (const auto& track_unit: build_tracks(jet)) {
        hists.fill(track_unit.track, jet.jet_truthflav, weight);
      }
    }
  }

  // save histograms
  H5::H5File out_file(cli.out_file(), H5F_ACC_TRUNC);
  hists.save(out_file, "reweighted");
}

// ______________________________________________________________________
// hist methods

TrackHists::TrackHists():
  ENERGY(pt, 40*GeV),
  eta({ {"eta", N_BINS, -2.8, 2.8 }})
{
}

void TrackHists::fill(const Track& track, double weight) {
#define BYNAME(name) name.fill(track.name, weight)
  BYNAME(pt);
  BYNAME(eta);
#undef BYNAME
}

void TrackHists::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(pt);
  BYNAME(eta);
#undef BYNAME
}
void TrackHists::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}

// ________________________________________________________________________
// flavored hists

void FlavoredHists::fill(const Track& track, int ftl, double weight) {
  m_hists[ftl].fill(track, weight);
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
