#include "FileCLI.hh"
#include "Jets.hh"
#include "SmartChain.hh"
#include "tools.hh"

#include "ndhist/Histogram.hh"

#include "H5Cpp.h"
#include "TROOT.h"

#include <iostream>
// #include <limits>

// const double GeV = 1000;
// const double MAX_PT = 1000*GeV;

const double GeV = 1000;
const double MAX_PT = 1000*GeV;
const int N_BINS = 200;

class JetHists
{
public:
  JetHists();
  void fill(const Jet& jet, double weight = 1);
  void save(H5::CommonFG& out);
  void save(H5::CommonFG& out, std::string subdir);
private:
  Histogram mv2c00;
  Histogram mv2c10;
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

  FlavorPtEtaDistributions pt_eta_reweight("reweight.h5");
  JetHists hists;
  JetHists reweighted_hists;

  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      hists.fill(jet);
    }
  }

  // save histograms
  H5::H5File out_file(cli.out_file(), H5F_ACC_TRUNC);
  hists.save(out_file);

}

// ______________________________________________________________________
// hist methods


JetHists::JetHists():
  mv2c00(N_BINS, 0, 1), mv2c10(N_BINS, 0, 1)
{
}

void JetHists::fill(const Jet& jet, double weight) {
#define BYNAME(name) name.fill(jet.jet_ ## name, weight)
  BYNAME(mv2c00);
  BYNAME(mv2c10);
#undef BYNAME
}

void JetHists::save(H5::CommonFG& out) {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(mv2c00);
  BYNAME(mv2c10);
#undef BYNAME
}
void JetHists::save(H5::CommonFG& out, std::string subdir) {
  H5::Group group(out.createGroup(subdir));
  save(group);
}
