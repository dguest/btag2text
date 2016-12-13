#include "Options.hh"
#include "Jets.hh"
#include "hdf5_object_builders.hh"
#include "SmartChain.hh"
// #include "FlavorPtEtaDistributions.hh"
#include "HDF5Writer.hh"
#include "constants.hh"
#include "unshittify.hh"
#include "select_jet.hh"
#include "math.hh"
#include "get_tree.hh"

#include "H5Cpp.h"

#include <iostream>
#include <limits>

const std::string DESCRIPTION = (
  "Dump information for jets to HDF5"
  );

// _____________________________________________________________________
// main function

int main(int argc, char* argv[]) {
  unshittify();
  // required library calls
  H5::Exception::dontPrint();

  // load info
  const auto opts = get_writer_opts(argc, argv, DESCRIPTION);
  SmartChain chain(get_tree(opts.input_files.at(0)));
  for (const auto& in: opts.input_files) {
    chain.add(in);
  }
  Jets jets(chain);
  int n_entries = chain.GetEntries();
  if (opts.verbose) std::cout << "entires: " << n_entries << std::endl;

  size_t n_chunk = opts.chunk_size;
  H5::H5File out_file(opts.output_file, H5F_ACC_TRUNC);
  h5::Writer<h5::Cluster>* cluster_ds(nullptr);
  if (opts.cluster_size > 0) {
    cluster_ds = new h5::Writer<h5::Cluster>(
      out_file, "clusters", opts.cluster_size, n_chunk);
  }
  h5::Writer<h5::Track>* track_ds(nullptr);
  if (opts.track_size > 0) {
    track_ds = new h5::Writer<h5::Track>(
      out_file, "tracks", opts.track_size, n_chunk);
  }
  h5::Writer1d<h5::FatJet> jet_ds(out_file, "jets", n_chunk);
  h5::Writer1d<h5::HighLevelBTag> subjet1(out_file, "subjet1", n_chunk);
  h5::Writer1d<h5::HighLevelBTag> subjet2(out_file, "subjet2", n_chunk);

  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      if (! select_fat_jet(jet) ) continue;
      double weight = opts.weight * jet.mc_event_weight;
      std::vector<h5::Cluster> clusters = get_clusters(jet);
      std::vector<h5::Track> tracks = get_tracks(jet);
      if (cluster_ds) cluster_ds->add_jet(clusters);
      if (track_ds) track_ds->add_jet(tracks);
      h5::FatJet hjet = get_fat_jet(jet, weight);
      jet_ds.add_jet(hjet);
      const auto& subs = jet.vrtrkjets;
      subjet1.add_jet(
        subs.size() > 0 ? get_btagging(subs.at(0)): h5::HighLevelBTag());
      subjet1.add_jet(
        subs.size() > 1 ? get_btagging(subs.at(1)): h5::HighLevelBTag());
    }
  }
  if (cluster_ds) {
    cluster_ds->flush();
    cluster_ds->close();
  }
  if (track_ds) {
    track_ds->flush();
    track_ds->close();
  }
  jet_ds.flush();
  jet_ds.close();

  subjet1.flush();
  subjet1.close();
  subjet2.flush();
  subjet2.close();
}
