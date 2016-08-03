#ifndef JETS_HH
#define JETS_HH

#include <vector>
#include <map>
#include <ostream>

#include "covol/CovVar.hh"

class SmartChain;

// enum info copied from https://goo.gl/WLPwL4
enum TAGGERALGO { IP2D=0,
                  IP3D,
                  SV0,
                  SV1,
                  JF };

enum TRKORIGIN { PUFAKE=-1,
                 FROMB,
                 FROMC,
                 FRAG,
                 GEANT };

// ______________________________________________________________________
// classes

struct Jet
{
  // event
  double avgmu;

  // kinematics
  float jet_pt;
  float jet_eta;
  float jet_phi;
  float jet_E;
  float jet_m;

  // cleaning
  float jet_JVT;

  // flavor label
  int jet_truthflav;

  // cluster and calo
  std::vector<float> jet_cluster_pt                  ;
  std::vector<float> jet_cluster_eta                 ;
  std::vector<float> jet_cluster_phi                 ;
  std::vector<float> jet_cluster_e                   ;
  std::vector<unsigned int> jet_cluster_clustersize  ;
  std::vector<float> jet_cluster_isolation           ;
  std::vector<float> jet_cluster_lateral             ;
  std::vector<float> jet_cluster_longitudinal        ;
  std::vector<float> jet_cluster_second_lambda       ;
  std::vector<float> jet_cluster_second_r            ;
  std::vector<float> jet_cluster_center_lambda       ;
  std::vector<float> jet_cluster_center_mag          ;
  std::vector<float> jet_cluster_eng_pos             ;
  std::vector<float> jet_cluster_em_probability      ;
  std::vector<float> jet_cluster_eng_frac_max        ;
  std::vector<float> jet_cluster_first_eng_dens      ;

  // track counts
  int jet_ntrk;
  int jet_ip3d_ntrk;

  // high level
  // ip2d, ip3d
  float jet_ip2d_pb;
  float jet_ip2d_pc;
  float jet_ip2d_pu;
  float jet_ip3d_pb;
  float jet_ip3d_pc;
  float jet_ip3d_pu;
  // sv1
  int jet_sv1_ntrkj;
  int jet_sv1_ntrkv;
  int jet_sv1_n2t;
  float jet_sv1_m;
  float jet_sv1_efc;
  float jet_sv1_normdist;
  int jet_sv1_Nvtx;
  float jet_sv1_sig3d;
  // med-level sv1
  std::vector<float> jet_sv1_vtx_x;
  std::vector<float> jet_sv1_vtx_y;
  std::vector<float> jet_sv1_vtx_z;
  // jetfitter
  float jet_jf_m;
  float jet_jf_efc;
  float jet_jf_deta;
  float jet_jf_dphi;
  float jet_jf_ntrkAtVx;
  int jet_jf_nvtx;
  float jet_jf_sig3d;
  int jet_jf_nvtx1t;
  int jet_jf_n2t;
  int jet_jf_VTXsize;
  // med-level jetfitter
  std::vector<float> jet_jf_vtx_chi2;
  std::vector<float> jet_jf_vtx_ndf;
  std::vector<int> jet_jf_vtx_ntrk;
  std::vector<float> jet_jf_vtx_L3D;
  std::vector<float> jet_jf_vtx_sig3D;

  // MV2
  double jet_mv2c00;
  double jet_mv2c10;
  double jet_mv2c20;
  double jet_mv2c100;

  // track level
  std::vector<float> jet_trk_pt;
  std::vector<float> jet_trk_eta;
  std::vector<float> jet_trk_theta;
  std::vector<float> jet_trk_phi;
  std::vector<float> jet_trk_dr;
  std::vector<float> jet_trk_chi2;
  std::vector<float> jet_trk_ndf;

  std::vector<int> jet_trk_algo;
  std::vector<int> jet_trk_orig;

  // metrics
  std::vector<float> jet_trk_d0;
  std::vector<float> jet_trk_z0;
  std::vector<float> jet_trk_ip3d_signed_d0;
  // std::vector<std::vector<float> >* jet_trk_ip3d_d02D;
  std::vector<float> jet_trk_ip3d_z0;
  std::vector<float> jet_trk_ip3d_d0sig;
  std::vector<float> jet_trk_ip3d_z0sig;
  std::vector<int> jet_trk_ip3d_grade;
  std::vector<int> jet_trk_jf_Vertex;

  std::vector<int> jet_trk_nInnHits;
  std::vector<int> jet_trk_nNextToInnHits;
  std::vector<int> jet_trk_nBLHits;
  std::vector<int> jet_trk_nsharedBLHits;
  std::vector<int> jet_trk_nsplitBLHits;
  std::vector<int> jet_trk_nPixHits;
  std::vector<int> jet_trk_nsharedPixHits;
  std::vector<int> jet_trk_nsplitPixHits;
  std::vector<int> jet_trk_nSCTHits;
  std::vector<int> jet_trk_nsharedSCTHits;
  std::vector<int> jet_trk_expectBLayerHit;

  // --- derived quantities ---
  // track counting
  float track_2_d0_significance;
  float track_3_d0_significance;
  float track_2_z0_significance;
  float track_3_z0_significance;
  int   n_tracks_over_d0_threshold;
  // jetfitter vertices
  std::vector<float> jet_jf_trk_vtx_chi2;
  std::vector<float> jet_jf_trk_vtx_ndf;
  std::vector<int>   jet_jf_trk_vtx_ntrk;
  std::vector<float> jet_jf_trk_vtx_L3D;
  std::vector<float> jet_jf_trk_vtx_sig3D;

  std::vector<Jet> trkjets;
  std::vector<Jet> vrtrkjets;

};

struct JfVertex
{
  float chi2;
  float ndf;
  int ntrk;
  float l3d;
  float sig3d;
};

struct Cluster
{
  float pt                  ;
  float eta                 ;
  float phi                 ;
  float e                   ;
  unsigned int clustersize  ;
  float isolation           ;
  float lateral             ;
  float longitudinal        ;
  float second_lambda       ;
  float second_r            ;
  float center_lambda       ;
  float center_mag          ;
  float eng_pos             ;
  float em_probability      ;
  float eng_frac_max        ;
  float first_eng_dens      ;
};

struct Track
{
  // event
  double avgmu;

  float pt;
  float eta;
  float theta;
  float phi;
  float dr;

  // bitmasks
  int algo;
  int orig;

  // use the IP3D d0/z0 here
  float signed_d0;
  float z0;
  float d0sig;
  float z0sig;

  // quality info
  float chi2;
  float ndf;
  int ip3d_grade;
  int nInnHits; // number of innermost pixel layer hits
  int nNextToInnHits; // number of next-to-innermost pixel layer hits
  int nBLHits; // number of b-layer hits
  int nsharedBLHits; // number of shared b-layer hits
  int nsplitBLHits; // number of split b-layer hits
  int nPixHits; // number of pixel hits
  int nsharedPixHits; // number of shared pixel hits
  int nsplitPixHits; // number of split pixel hits
  int nSCTHits; // number of sct hits
  int nsharedSCTHits; // number of shared sct hits
  int expectBLayerHit; // is a b-layer hit expected?

  // IP3D-like proxy for DL
  float dphi_jet;
  float d0;
  // TODO: figure out how to add the original z0
  // float z0;

  // bitmask checks
  bool usedFor(TAGGERALGO algo) const;
  bool hasOrigin(TRKORIGIN origin) const;
};

struct TrkUnit
{
  Track track;
  JfVertex jf;
};

class Subjets
{
public:
  Subjets(SmartChain& chain, const std::string& name);
  Jet getJet(int jet, int subjet) const;
  int size(int jet) const;
private:
  // subjet-wise branches
  // kinematics
  std::vector<std::vector<float> >* pt;
  std::vector<std::vector<float> >* eta;
  std::vector<std::vector<float> >* phi;
  // std::vector<std::vector<float> >* E;
  std::vector<std::vector<float> >* m;

  // track counts
  std::vector<std::vector<int> >* ip3d_ntrk;

  // ip2d, ip3d
  std::vector<std::vector<float> >* ip3d_pb;
  std::vector<std::vector<float> >* ip3d_pc;
  std::vector<std::vector<float> >* ip3d_pu;
  // sv1
  // std::vector<std::vector<int> >* sv1_ntrkj;
  std::vector<std::vector<int> >* sv1_ntrkv;
  std::vector<std::vector<int> >* sv1_n2t;
  std::vector<std::vector<float> >* sv1_m;
  std::vector<std::vector<float> >* sv1_efc;
  std::vector<std::vector<float> >* sv1_normdist;
  std::vector<std::vector<int> >* sv1_Nvtx;

  // jetfitter
  std::vector<std::vector<float> >* jf_m;
  std::vector<std::vector<float> >* jf_efc;
  std::vector<std::vector<float> >* jf_deta;
  std::vector<std::vector<float> >* jf_dphi;
  std::vector<std::vector<float> >* jf_ntrkAtVx;
  std::vector<std::vector<int> >* jf_nvtx;
  std::vector<std::vector<float> >* jf_sig3d;
  std::vector<std::vector<int> >* jf_nvtx1t;
  std::vector<std::vector<int> >* jf_n2t;
  std::vector<std::vector<int> >* jf_VTXsize;
  // med-level jetfitter

  // MV2
  // std::vector<std::vector<double> >* mv2c00;
  std::vector<std::vector<float> >* mv2c10;
  std::vector<std::vector<float> >* mv2c20;
  // std::vector<std::vector<double> >* mv2c100;

};

class Jets
{
public:
  Jets(SmartChain& chain);
  Jet getJet(int) const;
  int size() const;
private:
  SmartChain* m_chain;
  Subjets m_trkjet;
  Subjets m_vrtrkjet;

  // event
  double avgmu;

  // kinematics
  std::vector<float>* jet_pt;
  std::vector<float>* jet_eta;
  std::vector<float>* jet_phi;
  std::vector<float>* jet_E;
  std::vector<float>* jet_m;

  // cleaning
  std::vector<float>* jet_JVT;

  // flavor label
  std::vector<int>* jet_truthflav;

  // cluster and calo
  std::vector<std::vector<float> >* jet_cluster_pt                  ;
  std::vector<std::vector<float> >* jet_cluster_eta                 ;
  std::vector<std::vector<float> >* jet_cluster_phi                 ;
  std::vector<std::vector<float> >* jet_cluster_e                   ;
  std::vector<std::vector<unsigned int> >* jet_cluster_clustersize  ;
  std::vector<std::vector<float> >* jet_cluster_isolation           ;
  std::vector<std::vector<float> >* jet_cluster_lateral             ;
  std::vector<std::vector<float> >* jet_cluster_longitudinal        ;
  std::vector<std::vector<float> >* jet_cluster_second_lambda       ;
  std::vector<std::vector<float> >* jet_cluster_second_r            ;
  std::vector<std::vector<float> >* jet_cluster_center_lambda       ;
  std::vector<std::vector<float> >* jet_cluster_center_mag          ;
  std::vector<std::vector<float> >* jet_cluster_eng_pos             ;
  std::vector<std::vector<float> >* jet_cluster_em_probability      ;
  std::vector<std::vector<float> >* jet_cluster_eng_frac_max        ;
  std::vector<std::vector<float> >* jet_cluster_first_eng_dens      ;

  // track counts
  std::vector<int>* jet_ip3d_ntrk;

  // high level
  // ip2d, ip3d
  std::vector<float>* jet_ip2d_pb;
  std::vector<float>* jet_ip2d_pc;
  std::vector<float>* jet_ip2d_pu;
  std::vector<float>* jet_ip3d_pb;
  std::vector<float>* jet_ip3d_pc;
  std::vector<float>* jet_ip3d_pu;
  // sv1
  std::vector<int>* jet_sv1_ntrkj;
  std::vector<int>* jet_sv1_ntrkv;
  std::vector<int>* jet_sv1_n2t;
  std::vector<float>* jet_sv1_m;
  std::vector<float>* jet_sv1_efc;
  std::vector<float>* jet_sv1_normdist;
  std::vector<int>* jet_sv1_Nvtx;
  std::vector<float>* jet_sv1_sig3d;
  // med-level sv1
  std::vector<std::vector<float> >* jet_sv1_vtx_x;
  std::vector<std::vector<float> >* jet_sv1_vtx_y;
  std::vector<std::vector<float> >* jet_sv1_vtx_z;
  // jetfitter
  std::vector<float>* jet_jf_m;
  std::vector<float>* jet_jf_efc;
  std::vector<float>* jet_jf_deta;
  std::vector<float>* jet_jf_dphi;
  std::vector<float>* jet_jf_ntrkAtVx;
  std::vector<int>* jet_jf_nvtx;
  std::vector<float>* jet_jf_sig3d;
  std::vector<int>* jet_jf_nvtx1t;
  std::vector<int>* jet_jf_n2t;
  std::vector<int>* jet_jf_VTXsize;
  // med-level jetfitter
  std::vector<std::vector<float> >* jet_jf_vtx_chi2;
  std::vector<std::vector<float> >* jet_jf_vtx_ndf;
  std::vector<std::vector<int> >* jet_jf_vtx_ntrk;
  std::vector<std::vector<float> >* jet_jf_vtx_L3D;
  std::vector<std::vector<float> >* jet_jf_vtx_sig3D;

  // MV2
  std::vector<double>* jet_mv2c00;
  std::vector<double>* jet_mv2c10;
  std::vector<double>* jet_mv2c20;
  std::vector<double>* jet_mv2c100;

  // track level
  std::vector<std::vector<float> >* jet_trk_pt;
  std::vector<std::vector<float> >* jet_trk_eta;
  std::vector<std::vector<float> >* jet_trk_theta;
  std::vector<std::vector<float> >* jet_trk_phi;
  std::vector<std::vector<float> >* jet_trk_dr;
  std::vector<std::vector<float> >* jet_trk_chi2;
  std::vector<std::vector<float> >* jet_trk_ndf;

  std::vector<std::vector<int> >* jet_trk_algo;
  std::vector<std::vector<int> >* jet_trk_orig;

  // metrics
  std::vector<std::vector<float> >* jet_trk_d0;
  std::vector<std::vector<float> >* jet_trk_z0;
  std::vector<std::vector<float> >* jet_trk_ip3d_d0;
  // std::vector<std::vector<float> >* jet_trk_ip3d_d02D;
  std::vector<std::vector<float> >* jet_trk_ip3d_z0;
  std::vector<std::vector<float> >* jet_trk_ip3d_d0sig;
  std::vector<std::vector<float> >* jet_trk_ip3d_z0sig;
  std::vector<std::vector<int> >* jet_trk_ip3d_grade;

  std::vector<std::vector<int> >* jet_trk_nInnHits;
  std::vector<std::vector<int> >* jet_trk_nNextToInnHits;
  std::vector<std::vector<int> >* jet_trk_nBLHits;
  std::vector<std::vector<int> >* jet_trk_nsharedBLHits;
  std::vector<std::vector<int> >* jet_trk_nsplitBLHits;
  std::vector<std::vector<int> >* jet_trk_nPixHits;
  std::vector<std::vector<int> >* jet_trk_nsharedPixHits;
  std::vector<std::vector<int> >* jet_trk_nsplitPixHits;
  std::vector<std::vector<int> >* jet_trk_nSCTHits;
  std::vector<std::vector<int> >* jet_trk_nsharedSCTHits;
  std::vector<std::vector<int> >* jet_trk_expectBLayerHit;

  std::vector<std::vector<int> >* jet_trk_jf_Vertex;


};


// ______________________________________________________________________
// misc functions

std::vector<TrkUnit> build_tracks(const Jet& jet);
std::ostream& operator<<(std::ostream&, Jets&);

std::vector<Cluster> build_clusters(const Jet& jet);

// get named maps of variables (last one supplies all)
std::map<std::string, double> get_pt_eta_map(const Jet& jet);
std::map<std::string, double> get_map(const Jet& jet);

std::vector<CovVar> get_jet_variables();

#endif // JETS_HH
