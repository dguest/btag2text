#ifndef JET_LABELS_HH
#define JET_LABELS_HH

#include <string>

class JetLabels
{
public:
#define LABELED(label) const std::string label = #label
  LABELED(jet_pt);
  LABELED(jet_eta);
  LABELED(jet_phi);
  LABELED(jet_E);
  LABELED(jet_m);

  LABELED(jet_truthflav);

  LABELED(jet_ip2d_pb);
  LABELED(jet_ip2d_pc);
  LABELED(jet_ip2d_pu);
  LABELED(jet_ip3d_pb);
  LABELED(jet_ip3d_pc);
  LABELED(jet_ip3d_pu);

  LABELED(jet_sv1_ntrkj);
  LABELED(jet_sv1_ntrkv);
  LABELED(jet_sv1_n2t);
  LABELED(jet_sv1_m);
  LABELED(jet_sv1_efc);
  LABELED(jet_sv1_normdist);
  LABELED(jet_sv1_Nvtx);
  LABELED(jet_sv1_sig3d);

  LABELED(jet_jf_m);
  LABELED(jet_jf_efc);
  LABELED(jet_jf_deta);
  LABELED(jet_jf_dphi);
  LABELED(jet_jf_ntrkAtVx);
  LABELED(jet_jf_nvtx);
  LABELED(jet_jf_sig3d);
  LABELED(jet_jf_nvtx1t);
  LABELED(jet_jf_n2t);
  LABELED(jet_jf_VTXsize);

  LABELED(jet_mv2c00);
  LABELED(jet_mv2c10);
  LABELED(jet_mv2c20);
  LABELED(jet_mv2c100);

  LABELED(track_2_d0_significance);
  LABELED(track_3_d0_significance);
  LABELED(track_2_z0_significance);
  LABELED(track_3_z0_significance);
  LABELED(n_tracks_over_d0_threshold);

};

class TrackLabels
{
public:
  LABELED(pt);
  LABELED(eta);
  LABELED(theta);
  LABELED(phi);
  LABELED(dr);
  LABELED(chi2);
  LABELED(ndf);

  LABELED(d0);
  LABELED(z0);
  LABELED(d0sig);
  LABELED(z0sig);
};

class JfVertexLabels
{
public:
  LABELED(chi2);
  LABELED(ndf);
  LABELED(ntrk);
  LABELED(l3d);
  LABELED(sig3d);
};

#undef LABELED


#endif
