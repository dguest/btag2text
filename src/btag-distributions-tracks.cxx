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
  // Histogram theta;
  // Histogram phi;
  Histogram dr;
  Histogram d0;
  Histogram z0;
  Histogram signed_d0;
  Histogram d0sig;
  Histogram z0sig;
  Histogram dphi_jet;

  // quality info
  Histogram chi2;
  Histogram ndf;
  Histogram ip3d_grade;
  Histogram nInnHits;
  Histogram nNextToInnHits;
  Histogram nBLHits;
  Histogram nsharedBLHits;
  Histogram nsplitBLHits;
  Histogram nPixHits;
  Histogram nsharedPixHits;
  Histogram nsplitPixHits;
  Histogram nSCTHits;
  Histogram nsharedSCTHits;
  Histogram expectBLayerHit;

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

const double D0_MAX = 5*mm;
const double Z0_MAX = 30*mm;
TrackHists::TrackHists():
  ENERGY(pt, 40*GeV),
  RANGE(eta, -2.8, 2.8),
  RANGE(dr, 0, 0.6),
  LENGTH(d0, -D0_MAX, D0_MAX),
  LENGTH(z0, -Z0_MAX, Z0_MAX),
  LENGTH(signed_d0, -D0_MAX, D0_MAX),
  RANGE(d0sig, 0, 100),
  RANGE(z0sig, 0, 100),
  RANGE(dphi_jet, -0.5, 0.5),
  RANGE(chi2, 0, 100),
  COUNT(ndf, 100),
  COUNT(ip3d_grade, 20),
  COUNT(nInnHits, 5),
  COUNT(nNextToInnHits, 5),
  COUNT(nBLHits, 4),
  COUNT(nsharedBLHits, 4),
  COUNT(nsplitBLHits, 4),
  COUNT(nPixHits, 10),
  COUNT(nsharedPixHits, 10),
  COUNT(nsplitPixHits, 10),
  COUNT(nSCTHits, 20),
  COUNT(nsharedSCTHits, 20),
  COUNT(expectBLayerHit, 4)

{
}

void TrackHists::fill(const Track& track, double weight) {
#define BYNAME(name) name.fill(track.name, weight)
  BYNAME(pt);
  BYNAME(eta);

  BYNAME(dr);
  BYNAME(d0);
  BYNAME(z0);
  BYNAME(signed_d0);
  BYNAME(d0sig);
  BYNAME(z0sig);
  BYNAME(dphi_jet);

  // quality info
  BYNAME(chi2);
  BYNAME(ndf);
  BYNAME(ip3d_grade);
  BYNAME(nInnHits);
  BYNAME(nNextToInnHits);
  BYNAME(nBLHits);
  BYNAME(nsharedBLHits);
  BYNAME(nsplitBLHits);
  BYNAME(nPixHits);
  BYNAME(nsharedPixHits);
  BYNAME(nsplitPixHits);
  BYNAME(nSCTHits);
  BYNAME(nsharedSCTHits);
  BYNAME(expectBLayerHit);
#undef BYNAME
}

void TrackHists::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(pt);
  BYNAME(eta);

  BYNAME(dr);
  BYNAME(d0);
  BYNAME(z0);
  BYNAME(signed_d0);
  BYNAME(d0sig);
  BYNAME(z0sig);
  BYNAME(dphi_jet);

  // quality info
  BYNAME(chi2);
  BYNAME(ndf);
  BYNAME(ip3d_grade);
  BYNAME(nInnHits);
  BYNAME(nNextToInnHits);
  BYNAME(nBLHits);
  BYNAME(nsharedBLHits);
  BYNAME(nsplitBLHits);
  BYNAME(nPixHits);
  BYNAME(nsharedPixHits);
  BYNAME(nsplitPixHits);
  BYNAME(nSCTHits);
  BYNAME(nsharedSCTHits);
  BYNAME(expectBLayerHit);
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
