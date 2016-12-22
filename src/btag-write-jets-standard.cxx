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

#include <csignal>

namespace
{
  volatile std::sig_atomic_t g_kill_signal;
}

void signal_handler(int signal)
{
  std::cerr << "caught signal, dying slowly..." << std::endl;
  g_kill_signal = signal;
}

const std::string DESCRIPTION = (
  "Dump information for jets to HDF5"
  );

// _____________________________________________________________________
// main function

int main(int argc, char* argv[]) {
  unshittify();
  typedef h5::HighLevelBTag btag_t;
  // required library calls
  H5::Exception::dontPrint();
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

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
  h5::Writer1d<btag_t> jet_ds(out_file, "jets", n_chunk);

  for (int iii = 0; iii < n_entries; iii++) {
    if (g_kill_signal == SIGINT || g_kill_signal == SIGTERM) break;
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      if (! select_jet(jet) ) continue;
      double weight = opts.weight * jet.mc_event_weight;
      std::vector<h5::Cluster> clusters = get_clusters(jet);
      std::vector<h5::Track> tracks = get_tracks(jet);
      if (cluster_ds) cluster_ds->add_jet(clusters);
      if (track_ds) track_ds->add_jet(tracks);
      jet_ds.add_jet(get_btagging(jet));
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
}
