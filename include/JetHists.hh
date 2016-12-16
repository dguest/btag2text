#ifndef JET_HISTS_HH
#define JET_HISTS_HH

namespace H5 {
  class CommonFG;
}
struct Jet;

#include "ndhist/Histogram.hh"

#include <string>

class JetHists
{
public:
  JetHists(bool is_subjet = false);
  void fill(const Jet& jet, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  bool m_is_subjet;
  Histogram ntrk;
  Histogram ncluster;

  Histogram pt;
  Histogram eta;

  Histogram dphi_fatjet;
};


#endif // JET_HISTS_HH
