#ifndef SORT_FUNCTIONS_HH
#define SORT_FUNCTIONS_HH

namespace h5 {
  struct Track;
  struct Cluster;
}

namespace sort {
  bool by_decreasing_abs_d0sig(h5::Track& t1, h5::Track& t2);
  bool by_decreasing_pt(h5::Cluster& c1, h5::Cluster& c2);
}
#endif
