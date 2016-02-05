#include "FileCLI.hh"
#include "Jets.hh"
#include "SmartChain.hh"
#include "FlavorPtEtaDistributions.hh"
#include "output_templates.hh"
#include "constants.hh"

#include "H5Cpp.h"
#include "TROOT.h"

#include <iostream>
#include <limits>

// const double GeV = 1000;
// const double MAX_PT = 1000*GeV;

std::string help = "Dump IP3D information for jets";

// _____________________________________________________________________
// main function

int main(int argc, char* argv[]) {
  // required library calls
  H5::Exception::dontPrint();
  gROOT->ProcessLine("#include <vector>");

  // load info
  FileCLI cli(argc, argv, Output::NO, help);
  SmartChain chain("bTag_AntiKt4EMTopoJets");
  for (const auto& in: cli.in_files()) {
    chain.add(in);
  }
  Jets jets(chain);
  int n_entries = chain.GetEntries();

  // load distributions
  require(REWEIGHT_FILE);
  FlavorPtEtaDistributions pt_eta_reweight(
    H5::H5File(REWEIGHT_FILE, H5F_ACC_RDONLY));

  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      auto pt_eta = get_pt_eta_map(jet);
      double weight = pt_eta_reweight.get(pt_eta, jet.jet_truthflav);
      std::cout << str_from_ip3d_jet(jet, weight) << std::endl;
    }
  }
}
