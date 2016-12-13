#ifndef HDF5_OBJECT_BUILDERS
#define HDF5_OBJECT_BUILDERS

#include "HDF5Writer.hh"

#include <vector>

struct Jet;

h5::HighLevelBTag get_btagging(const Jet& jet);

std::vector<h5::Cluster> get_clusters(const Jet& jet);
std::vector<h5::Track> get_tracks(const Jet& jet);

#endif
