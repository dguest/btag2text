#include "Options.hh"
#include "Jets.hh"
#include "hdf5_object_builders.hh"
#include "SmartChain.hh"
#include "FlavorPtEtaDistributions.hh"
#include "HDF5Writer.hh"
#include "constants.hh"
#include "unshittify.hh"
#include "select_jet.hh"
#include "math.hh"
#include "get_tree.hh"
#include "sort_functions.hh"

#include "LwtnnWrapper.hh"

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
  typedef h5::HighLevelSubjetBTag btag_t;
  // required library calls
  H5::Exception::dontPrint();
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  // load configuration
  const auto opts = get_opts(argc, argv, DESCRIPTION,
                             opt::reweight_file | opt::writer |
                             opt::max_weight | opt::lwtnn);

  // load input files
  SmartChain chain(get_tree(opts.input_files.at(0)));
  for (const auto& in: opts.input_files) {
    chain.add(in);
  }
  Jets jets(chain);
  int n_entries = chain.GetEntries();
  if (opts.verbose) std::cout << "entires: " << n_entries << std::endl;

  // (maybe) get reweighting histogram
  Distribution* pt_rw = nullptr;
  if (opts.rw_file.size() > 0) {
    H5::H5File rw_file(opts.rw_file, H5F_ACC_RDONLY);
    pt_rw = new Distribution(rw_file.openDataSet("signal_weights"));
  }

  LwtnnWrapper network(opts.network_file);

  // setup outputs
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
  h5::Writer1d<btag_t> subjet1(out_file, "subjet1", n_chunk);
  h5::Writer1d<btag_t> subjet2(out_file, "subjet2", n_chunk);
  h5::Writer1d<btag_t> subjet3(out_file, "subjet3", n_chunk);

  for (int iii = 0; iii < n_entries; iii++) {
    if (g_kill_signal == SIGINT || g_kill_signal == SIGTERM) break;
    chain.GetEntry(iii);

    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);

      // jet selection
      double nn_score = network ? network.compute(jet) : 0;
      if (! select_fat_jet(jet) ) continue;
      double weight = opts.weight * jet.mc_event_weight;
      if (pt_rw) weight *= pt_rw->get({{"pt", jet.jet_pt}});
      if (weight > opts.max_weight) continue;

      // build vectors and fill output arrays
      std::vector<h5::Cluster> clusters = get_clusters(jet);
      std::vector<h5::Track> tracks = get_tracks(jet, TrackSelection::ALL);
      std::sort(tracks.begin(), tracks.end(), sort::by_decreasing_abs_d0sig);
      std::sort(clusters.begin(), clusters.end(), sort::by_decreasing_pt);
      if (cluster_ds) cluster_ds->add_jet(clusters);
      if (track_ds) track_ds->add_jet(tracks);
      h5::FatJet hjet = get_fat_jet(jet, weight);
      hjet.discriminant = nn_score;
      jet_ds.add_jet(hjet);
      const auto& subs = jet.vrtrkjets;
      subjet1.add_jet(subs.size() > 0 ?
                      get_subjet_btagging(subs.at(0)): btag_t());
      subjet2.add_jet(subs.size() > 1 ?
                      get_subjet_btagging(subs.at(1)): btag_t());
      subjet3.add_jet(subs.size() > 2 ?
                      get_subjet_btagging(subs.at(2)): btag_t());
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
  subjet3.flush();
  subjet3.close();
}
