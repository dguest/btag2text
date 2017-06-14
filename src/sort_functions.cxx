#include "sort_functions.hh"
#include "HDF5Writer.hh"

#include <cmath>

namespace sort {
  // sort tracks
  bool by_decreasing_abs_d0sig(h5::Track& t1, h5::Track& t2){
    return std::abs(t1.d0sig) > std::abs(t2.d0sig);
  }
  bool by_decreasing_pt(h5::Cluster& c1, h5::Cluster& c2) {
    return c1.pt > c2.pt;
  }
}

