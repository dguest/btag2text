#include "Options.hh"
#include "Jets.hh"
#include "SmartChain.hh"
#include "output_templates.hh"
#include "constants.hh"
#include "unshittify.hh"
#include "select_jet.hh"
#include "get_tree.hh"

#include <iostream>
#include <limits>

const std::string DESCRIPTION = (
  "Dump information for jets\n"
  "Should be pretty comprehensive, including "
  "subjets, tracks, and clusters"
  );

// _____________________________________________________________________
// main function

int main(int argc, char* argv[]) {
  unshittify();

  // load info
  const auto opts = get_stream_opts(argc, argv, DESCRIPTION);
  SmartChain chain(get_tree(opts.input_files.at(0)));
  for (const auto& in: opts.input_files) {
    chain.add(in);
  }
  Jets jets(chain);
  int n_entries = chain.GetEntries();

  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      if (! select_fat_jet(jet) ) continue;
      double weight = opts.weight * jet.mc_event_weight;
      std::cout << str_from_fat_jet(jet, weight) << std::endl;
    }
  }
}
