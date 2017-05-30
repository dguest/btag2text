#include "Options.hh"
#include "Jets.hh"
#include "SmartChain.hh"
#include "constants.hh"
#include "ClusterImages.hh"
#include "JetHists.hh"
#include "BTagHists.hh"
#include "SubstructureHists.hh"
#include "hist_tools.hh"
#include "select_jet.hh"
#include "unshittify.hh"
#include "get_tree.hh"
#include "h5_tools.hh"

#include "ndhist/Histogram.hh"
#include "ndhist/Exceptions.hh"

#include "H5Cpp.h"
#include "TROOT.h"

#include <iostream>
#include <sstream>

const double MV2_HIGH = 1.00001;
const double MV2_LOW = -MV2_HIGH;

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
  std::vector<BTagHists> m_subjets_btag;
  JetHists m_fatjet;
  SubstructureHists m_substruct;
  Histogram m_mv2_min;
  int m_n_one_cluster;
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
  if (opts.verbose) std::cout << "entires: " << n_entries << std::endl;
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
  m_subjets(n_subjets, true),
  m_subjets_btag(n_subjets),
  m_fatjet(),
  m_mv2_min({{"mv2_min", 2000, MV2_LOW, MV2_HIGH}}),
  m_n_one_cluster(0)
{
}
void FatJetHists::fill(const Jet& jet, double weight) {
  const auto& subjets = jet.vrtrkjets;
  const size_t max_idx = std::min(subjets.size(), m_subjets.size());
  for (size_t jet_idx = 0; jet_idx < max_idx; jet_idx++) {
    try {
      m_subjets.at(jet_idx).fill(subjets.at(jet_idx), weight);
      m_subjets_btag.at(jet_idx).fill(subjets.at(jet_idx), weight);
    } catch (HistogramNanError& err) {
      std::stringstream erstr;
      erstr << err.what() << ", fatjet:\n" << jet;
      throw HistogramNanError(erstr.str());
    }
  }
  m_fatjet.fill(jet, weight);
  if (jet.jet_cluster_pt.size() > 1) {
    m_substruct.fill(jet.moments, weight);
  } else {
    m_n_one_cluster++;
  }
  if (subjets.size() >= 2) {
    double mv2_1 = subjets.at(0).jet_mv2c10;
    double mv2_2 = subjets.at(1).jet_mv2c10;
    m_mv2_min.fill(std::min(mv2_1, mv2_2), weight);
  }
}

void FatJetHists::save(H5::CommonFG& out) const {
  H5::Group fatjet(out.createGroup("fatjet"));
  m_fatjet.save(fatjet);
  m_substruct.save(fatjet);
  write_attr(fatjet, "n_one_cluster", m_n_one_cluster);
  for (size_t jet_idx = 0; jet_idx < m_subjets.size(); jet_idx++) {
    H5::Group subjet(out.createGroup("subjet_" + std::to_string(jet_idx)));
    m_subjets.at(jet_idx).save(subjet);
    m_subjets_btag.at(jet_idx).save(subjet);
  }
  m_mv2_min.write_to(fatjet, "mv2c10_min");
}
void FatJetHists::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}
