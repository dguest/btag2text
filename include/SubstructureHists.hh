#ifndef SUBSTRUCTUREMOMENT_HISTS_HH
#define SUBSTRUCTUREMOMENT_HISTS_HH

namespace H5 {
  class CommonFG;
}
struct SubstructureMoments;

#include "ndhist/Histogram.hh"

#include <string>

class SubstructureHists
{
public:
  SubstructureHists();
  void fill(const SubstructureMoments& moments, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  Histogram tau21;
  Histogram c1;
  Histogram c2;
  Histogram c1_beta2;
  Histogram c2_beta2;
  Histogram d2;
  Histogram d2_beta2;

};


#endif
