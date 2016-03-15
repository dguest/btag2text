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

class ClusterHists
{
public:
  ClusterHists();
  void fill(const Cluster& track, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  Histogram pt;
  Histogram eta;
  Histogram phi;
  Histogram e;
};

class FlavoredHists
{
public:
  void fill(const Cluster& track, int flavor, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  std::map<int, ClusterHists> m_hists;
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

  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      auto jet_vars = get_map(jet);
      double weight = pt_eta_reweight.get(jet_vars, jet.jet_truthflav);
      for (const auto& cluster: build_clusters(jet)) {
        hists.fill(cluster, jet.jet_truthflav, weight);
      }
    }
  }

  // save histograms
  using namespace grp;
  H5::H5File out_file(cli.out_file(), H5F_ACC_TRUNC);
  auto hist_group = out_file.createGroup(HIST);
  hists.save(hist_group, REWEIGHTED);
}

// ______________________________________________________________________
// hist methods

ClusterHists::ClusterHists():
  ENERGY(pt, 40*GeV),
  RANGE(eta, -2.8, 2.8),
  RANGE(phi, -3.142, 3.142),
  ENERGY(e, 40*GeV)
{
}

void ClusterHists::fill(const Cluster& track, double weight) {
#define BYNAME(name) name.fill(track.name, weight)
  BYNAME(pt);
  BYNAME(eta);
  BYNAME(phi);
  BYNAME(e);
  // quality info
#undef BYNAME
}

void ClusterHists::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(pt);
  BYNAME(eta);
  BYNAME(phi);
  BYNAME(e);
#undef BYNAME
}
void ClusterHists::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}

// ________________________________________________________________________
// flavored hists

void FlavoredHists::fill(const Cluster& track, int ftl, double weight) {
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
