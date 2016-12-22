#ifndef HDF5_OBJECT_BUILDERS
#define HDF5_OBJECT_BUILDERS

#include "HDF5Writer.hh"

#include <vector>

struct Jet;

h5::FatJet get_fat_jet(const Jet& jet, double weight);
h5::HighLevelSubjetBTag get_btagging(const Jet& jet);

std::vector<h5::Cluster> get_clusters(const Jet& jet);
std::vector<h5::Track> get_tracks(const Jet& jet);

#endif
