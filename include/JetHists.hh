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
  Histogram mv2c10;
  Histogram mv2c20;

  Histogram ip3d_pu;
  Histogram ip3d_pc;
  Histogram ip3d_pb;
  Histogram ip3d_ntrk;
  Histogram ntrk;

  Histogram ga_ntrk;

  Histogram sv1_ntrkv;
  Histogram sv1_n2t;
  Histogram sv1_m;
  Histogram sv1_efc;
  Histogram sv1_normdist;
  Histogram sv1_Nvtx;

  Histogram jf_m;
  Histogram jf_efc;
  Histogram jf_deta;
  Histogram jf_dphi;
  Histogram jf_ntrkAtVx;
  Histogram jf_nvtx;
  Histogram jf_sig3d;
  Histogram jf_nvtx1t;
  Histogram jf_n2t;
  Histogram jf_VTXsize;

  Histogram pt;
  Histogram eta;

  Histogram dphi_fatjet;
};


#endif // JET_HISTS_HH
