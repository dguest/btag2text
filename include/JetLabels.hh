#ifndef JET_LABELS_HH
#define JET_LABELS_HH

#include <string>
#include <vector>

#include "ellipsis.hh"

// ________________________________________________________________________
// verbose labels

class JetLabels
{
public:
  const std::string jet_pt = "jet pt";
  const std::string jet_eta = "jet eta";
  const std::string jet_phi = "jet phi";
  const std::string jet_E = "jet energy";
  const std::string jet_m = "jet mass";

  const std::string jet_truthflav = "true jet flavor";

  const std::string jet_ip2d_pb = "ip2d b likelihood";
  const std::string jet_ip2d_pc = "ip2d c likelihood";
  const std::string jet_ip2d_pu = "ip2d light likelihood";
  const std::string jet_ip3d_pb = "ip3d b likelihood";
  const std::string jet_ip3d_pc = "ip3d c likelihood";
  const std::string jet_ip3d_pu = "ip3d light likelihood";

  const std::string jet_sv1_ntrkj = "jet_sv1_ntrkj";
  const std::string jet_sv1_ntrkv = "jet_sv1_ntrkv";
  const std::string jet_sv1_n2t = "jet_sv1_n2t";
  const std::string jet_sv1_m = "jet_sv1_m";
  const std::string jet_sv1_efc = "jet_sv1_efc";
  const std::string jet_sv1_normdist = "jet_sv1_normdist";
  const std::string jet_sv1_Nvtx = "jet_sv1_Nvtx";
  const std::string jet_sv1_sig3d = "jet_sv1_sig3d";

  const std::string jet_jf_m = "jet_jf_m";
  const std::string jet_jf_efc = "jet_jf_efc";
  const std::string jet_jf_deta = "jet_jf_deta";
  const std::string jet_jf_dphi = "jet_jf_dphi";
  const std::string jet_jf_ntrkAtVx = "jet_jf_ntrkAtVx";
  const std::string jet_jf_nvtx = "jet_jf_nvtx";
  const std::string jet_jf_sig3d = "jet_jf_sig3d";
  const std::string jet_jf_nvtx1t = "jet_jf_nvtx1t";
  const std::string jet_jf_n2t = "jet_jf_n2t";
  const std::string jet_jf_VTXsize = "jet_jf_VTXsize";

  const std::string jet_mv2c00 = "jet_mv2c00";
  const std::string jet_mv2c10 = "jet_mv2c10";
  const std::string jet_mv2c20 = "jet_mv2c20";
  const std::string jet_mv2c100 = "jet_mv2c100";

  const std::string track_2_d0_significance = "track_2_d0_significance";
  const std::string track_3_d0_significance = "track_3_d0_significance";
  const std::string track_2_z0_significance = "track_2_z0_significance";
  const std::string track_3_z0_significance = "track_3_z0_significance";
  const std::string n_tracks_over_d0_threshold = "n_tracks_over_d0_threshold";

  const std::vector<JetLabels> vrtrkjets;
  JetLabels() = default;
  JetLabels(int iii): vrtrkjets(iii) {}
};

class TrackLabels
{
public:
  const std::string pt = "track pt";
  const std::string eta = "track eta";
  const std::string theta = "track theta";
  const std::string phi = "track phi";
  const std::string dr = "dR between track and jet";
  const std::string chi2 = "track chi2";
  const std::string ndf = "track ndf";

  const std::string d0 = "d0";
  const std::string z0 = "z0";
  const std::string d0sig = "d0 significance";
  const std::string z0sig = "z0 significance";

  // NOTE: ``innermost'' pixel layer is the one _outside_ the b-layer
  const std::string nInnHits = "number of first pixel layer hits";
  const std::string nNextToInnHits = "number second pixel layer hits";
  const std::string nBLHits = "number of b-layer hits";
  const std::string nsharedBLHits = "number of shared b-layer hits";
  const std::string nsplitBLHits = "number of split b-layer hits";
  const std::string nPixHits = "number of pixel hits";
  const std::string nsharedPixHits = "number of shared pixel hits";
  const std::string nsplitPixHits = "number of split pixel hits";
  const std::string nSCTHits = "number of sct hits";
  const std::string nsharedSCTHits = "number of shared sct hits";
  const std::string expectBLayerHit = "b-layer hit expected";

  const std::string dphi_jet = "dphi between jet and track";

};

class ClusterLabels
{
public:
  const std::string pt = "cluster pt";
  const std::string eta = "cluster eta";
  const std::string phi = "cluster phi";
  const std::string dphi_jet = "cluster dphi with respect to jet";
  const std::string e = "cluster energy";
};


template<>
std::string ellipsis(const TrackLabels&);
template<>
std::string ellipsis(const ClusterLabels&);
template<>
std::string ellipsis(const JetLabels&);

class JfVertexLabels
{
public:
  const std::string chi2 = "chi2";
  const std::string ndf = "ndf";
  const std::string ntrk = "ntrk";
  const std::string l3d = "l3d";
  const std::string sig3d = "sig3d";
};


class TrkUnitLabels
{
public:
  TrackLabels track;
  JfVertexLabels jf;
};

std::vector<TrkUnitLabels> build_tracks(const JetLabels& jet);
std::vector<ClusterLabels> build_clusters(const JetLabels& jet);

// _______________________________________________________________________
// terse labels

std::string quoted(const std::string& in);

#define LABELED(label) const std::string label = quoted(#label)

class TerseJetLabels
{
public:
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

  const std::vector<TerseJetLabels> vrtrkjets;
  TerseJetLabels() = default;
  TerseJetLabels(int iii): vrtrkjets(iii) {}
};

class TerseTrackLabels
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

  // NOTE: ``innermost'' pixel layer is the one _outside_ the b-layer
  LABELED(nInnHits);
  LABELED(nNextToInnHits);
  LABELED(nBLHits);
  LABELED(nsharedBLHits);
  LABELED(nsplitBLHits);
  LABELED(nPixHits);
  LABELED(nsharedPixHits);
  LABELED(nsplitPixHits);
  LABELED(nSCTHits);
  LABELED(nsharedSCTHits);
  LABELED(expectBLayerHit);

  LABELED(dphi_jet);

};

class TerseClusterLabels
{
public:
  const std::string pt = quoted("cluster_pt");
  const std::string eta = quoted("cluster_eta");
  const std::string phi = quoted("cluster_phi");
  const std::string dphi_jet = quoted("cluster_dphi_jet");
  const std::string e = quoted("cluster_energy");
};


class TerseTrkUnitLabels
{
public:
  TerseTrackLabels track;
  JfVertexLabels jf;
};

template<>
std::string ellipsis(const TerseTrackLabels&);
template<>
std::string ellipsis(const TerseClusterLabels&);
template<>
std::string ellipsis(const TerseJetLabels&);

std::vector<TerseTrkUnitLabels> build_tracks(const TerseJetLabels& jet);
std::vector<TerseClusterLabels> build_clusters(const TerseJetLabels& jet);

#undef LABELED


#endif
