#include "Options.hh"
#include "Jets.hh"
#include "SmartChain.hh"
#include "constants.hh"
#include "ClusterImages.hh"
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

// various plotting constants
const double MAX_VX_MASS = 10*GeV;
const double MV2_HIGH = 1.00001;
const double MV2_LOW = -MV2_HIGH;
const double PT_MAX = 2*TeV;
const double ETA_MAX = 3.0;

class JetHists
{
public:
  JetHists();
  void fill(const Jet& jet, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  Histogram pt;
  Histogram eta;
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

  JetHists hists;
  ClusterImages images(125*GeV);
  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      if (! select_jet(jet) ) continue;
      hists.fill(jet, opts.weight);
      auto clusters = build_clusters(jet);
      images.fill(clusters, jet, opts.weight);
    }
  }

  // save histograms
  using namespace grp;
  H5::H5File out_file(opts.output_file, H5F_ACC_TRUNC);
  // hists
  hists.save(out_file, HIST);
  images.save(out_file, IMAGE);
  write_attr(out_file, "n_entries", n_entries);
}

// ______________________________________________________________________
// hist methods

JetHists::JetHists():
  // MV2
  ENERGY(pt, PT_MAX),
  RANGE(eta, -ETA_MAX, ETA_MAX)
{
}

void JetHists::fill(const Jet& jet, double weight) {
#define BYNAME(name) name.fill(jet.jet_ ## name, weight)
  BYNAME(pt);
  BYNAME(eta);
#undef BYNAME
}

void JetHists::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(pt);
  BYNAME(eta);
#undef BYNAME
}
void JetHists::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}

