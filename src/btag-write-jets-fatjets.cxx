#include "Options.hh"
#include "Jets.hh"
#include "SmartChain.hh"
// #include "FlavorPtEtaDistributions.hh"
#include "HDF5Writer.hh"
#include "constants.hh"
#include "unshittify.hh"
#include "select_jet.hh"
#include "get_tree.hh"

#include "H5Cpp.h"

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
  // required library calls
  H5::Exception::dontPrint();

  // load info
  const auto opts = get_opts(argc, argv, DESCRIPTION);
  SmartChain chain(get_tree(opts.input_files.at(0)));
  for (const auto& in: opts.input_files) {
    chain.add(in);
  }
  Jets jets(chain);
  int n_entries = chain.GetEntries();
  if (opts.verbose) std::cout << "entires: " << n_entries << std::endl;

  H5::H5File out_file(opts.output_file, H5F_ACC_TRUNC);
  h5::Writer<h5::Cluster> cluster_ds(out_file, "clusters", 20, 256);
  h5::Writer<h5::Track> track_ds(out_file, "tracks", 20, 256);

  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      if (! select_fat_jet(jet) ) continue;
      double weight = opts.weight * jet.mc_event_weight;
      std::vector<h5::Cluster> clusters;
      std::vector<h5::Track> tracks;
      for (int n = 0; n < 15; n++) {
        float d = n;
        clusters.push_back({d, d, d, d, false, d});
        tracks.push_back({d, d, d, false, d});
      }
      cluster_ds.add_jet(clusters);
      track_ds.add_jet(tracks);
    }
  }
  cluster_ds.flush();
  cluster_ds.close();
  track_ds.flush();
  track_ds.close();
}
