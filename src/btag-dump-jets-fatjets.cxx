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
  "Dump information for jets including "
  "subjets, tracks, and clusters\n\n"
  "Options"
  );

// _____________________________________________________________________
// main function

int main(int argc, char* argv[]) {
  // undo some weird root stuff
  unshittify();

  // load options
  const std::string usage = "usage: " + std::string(argv[0])
    + " [options] <root-file>...\n\n";
  const auto opts = get_stream_opts(argc, argv, usage + DESCRIPTION);

  // build chain and add files
  SmartChain chain(get_tree(opts.input_files.at(0)));
  for (const auto& in: opts.input_files) {
    chain.add(in);
  }

  // the Jets buffer, which reads the data out of the chain and
  // builds jet objects
  Jets jets(chain);

  // main program loop
  int n_entries = chain.GetEntries();
  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);

      // jet selection function is defined in select_jet
      if (! select_fat_jet(jet) ) continue;

      double weight = opts.weight * jet.mc_event_weight;

      // dump a line of text corresponding to this jet
      std::cout << str_from_fat_jet(jet, weight) << std::endl;
    }
  }
}
