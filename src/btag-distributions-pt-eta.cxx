#include "FileCLI.hh"
#include "constants.hh"
#include "get_tree.hh"

#include "ndhist/Histogram.hh"

#include "TChain.h"

#include "H5Cpp.h"
#include <iostream>

class FlavorPtEtaHists
{
public:
  FlavorPtEtaHists(double pt_max, double eta_max = 2.5);
  void fill(int ftl, double pt, double eta, double weight = 1);
  void save(H5::CommonFG& out);
  void save(H5::CommonFG& out, std::string subdir);
private:
  double m_pt_max;
  double m_eta_max;
  std::map<int, Histogram> m_flavors;
};

FlavorPtEtaHists::FlavorPtEtaHists(double pt_max, double eta_max):
  m_pt_max(pt_max), m_eta_max(eta_max)
{
}

void FlavorPtEtaHists::fill(int ftl, double pt, double eta, double weight) {
  if (!m_flavors.count(ftl)) {
    std::vector<Axis> axes{
      {"pt", PT_REWEIGHT_NBINS, 0, m_pt_max, BASE_UNITS},
      {"abs_eta", 1, 0, m_eta_max} };
    m_flavors.emplace(ftl, axes);
  }
  m_flavors.at(ftl).fill({pt, std::abs(eta)}, weight);
}

void FlavorPtEtaHists::save(H5::CommonFG& out) {
  for (const auto& flav_and_hist: m_flavors) {
    std::string histname = std::to_string(flav_and_hist.first);
    flav_and_hist.second.write_to(out, histname);
  }
}
void FlavorPtEtaHists::save(H5::CommonFG& out, std::string subdir) {
  H5::Group group(out.createGroup(subdir));
  save(group);
}

struct Jet {
  Jet();
  std::vector<float>* pt;
  std::vector<float>* eta;
  std::vector<int>* ftl;
};
Jet::Jet(): pt(0), eta(0), ftl(0) {}

int main(int argc, char* argv[]) {
  FileCLI cli(argc, argv);

  // setup chain
  TChain chain(get_tree(cli.in_files().at(0)).c_str());
  for (const auto& in: cli.in_files()) {
    chain.Add(in.c_str());
  }
  Jet kin;
  chain.SetBranchAddress("jet_pt", &kin.pt);
  chain.SetBranchAddress("jet_eta", &kin.eta);
  chain.SetBranchAddress("jet_truthflav", &kin.ftl);

  // setup histograms
  FlavorPtEtaHists hists(PT_REWEIGHT_MAX);

  int entries = chain.GetEntries();
  for (int iii = 0; iii < entries; iii++) {
    chain.GetEntry(iii);
    int jets = kin.pt->size();
    for (int jjj = 0; jjj < jets; jjj++) {
      hists.fill(kin.ftl->at(jjj), kin.pt->at(jjj)*MeV, kin.eta->at(jjj));
    }
  }

  // save histograms
  H5::H5File out_file(cli.out_file(), H5F_ACC_TRUNC);
  hists.save(out_file);
}
