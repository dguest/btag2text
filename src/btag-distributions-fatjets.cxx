#include "Options.hh"
#include "Jets.hh"
#include "SmartChain.hh"
#include "constants.hh"
#include "ClusterImages.hh"
#include "JetHists.hh"
#include "SubstructureHists.hh"
#include "hist_tools.hh"
#include "select_jet.hh"
#include "unshittify.hh"
#include "get_tree.hh"
#include "h5_tools.hh"

#include "ndhist/Histogram.hh"

#include "H5Cpp.h"
#include "TROOT.h"

#include <iostream>

const std::string DESCRIPTION = "build distributions for fat jets";

class FatJetHists
{
public:
  FatJetHists(int n_subjets);
  void fill(const Jet& jet, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  std::vector<JetHists> m_subjets;
  JetHists m_fatjet;
  SubstructureHists m_substruct;
};

// _____________________________________________________________________
// main function

int main(int argc, char* argv[]) {
  unshittify();
  // command parsing
  const auto opts = get_opts(argc, argv, DESCRIPTION);
  if (opts.verbose) std::cout << opts << std::endl;
  // running
  SmartChain chain(get_tree(opts.input_files.at(0)));
  for (const auto& in: opts.input_files) {
    chain.add(in);
  }
  Jets jets(chain);
  int n_entries = chain.GetEntries();
  double sum_event_weights = 0;

  FatJetHists hists(2);
  ClusterImages images(125*GeV);
  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    sum_event_weights += jets.eventWeight();
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      if (! select_fat_jet(jet) ) continue;
      double weight = opts.weight * jet.mc_event_weight;
      hists.fill(jet, weight);
      auto clusters = build_clusters(jet);
      images.fill(clusters, jet, weight);
    }
  }

  // save histograms
  using namespace grp;
  H5::H5File out_file(opts.output_file, H5F_ACC_TRUNC);
  // hists
  hists.save(out_file, HIST);
  images.save(out_file, IMAGE);
  write_attr(out_file, "n_entries", n_entries);
  write_attr(out_file, "sum_event_weights", sum_event_weights);
}

FatJetHists::FatJetHists(int n_subjets):
  m_subjets(n_subjets),
  m_fatjet()
{
}
void FatJetHists::fill(const Jet& jet, double weight) {
  const auto& subjets = jet.vrtrkjets;
  const size_t max_idx = std::min(subjets.size(), m_subjets.size());
  for (size_t jet_idx = 0; jet_idx < max_idx; jet_idx++) {
    m_subjets.at(jet_idx).fill(subjets.at(jet_idx), weight);
  }
  m_fatjet.fill(jet, weight);
  m_substruct.fill(jet.moments, weight);
}

void FatJetHists::save(H5::CommonFG& out) const {
  H5::Group fatjet(out.createGroup("fatjet"));
  m_fatjet.save(fatjet);
  m_substruct.save(fatjet);
  for (size_t jet_idx = 0; jet_idx < m_subjets.size(); jet_idx++) {
    m_subjets.at(jet_idx).save(out, "subjet_" + std::to_string(jet_idx));
  }
}
void FatJetHists::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}
