#ifndef JETS_HH
#define JETS_HH

#include <vector>
#include <map>
#include <ostream>

#include "Cluster.hh"
#include "SubstructureMoments.hh"

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
  double mc_event_weight;

  double PVx;
  double PVy;
  double PVz;

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
  int jet_LabDr_HadF;

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

  // cluster counts
  int jet_ncluster;

  // high level
  // ip2d, ip3d
  float jet_ip2d_pb;
  float jet_ip2d_pc;
  float jet_ip2d_pu;
  float jet_ip3d_pb;
  float jet_ip3d_pc;
  float jet_ip3d_pu;

  double jet_ipmp_pb;
  double jet_ipmp_pc;
  double jet_ipmp_pu;
  double jet_ipmp_ptau;

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
  int jet_jf_ntrkAtVx;
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
  int jet_ip3d_ntrk;
  std::vector<float> jet_trk_pt;
  std::vector<float> jet_trk_eta;
  std::vector<float> jet_trk_theta;
  std::vector<float> jet_trk_phi;
  // std::vector<float> jet_trk_dr;
  std::vector<float> jet_trk_chi2;
  std::vector<float> jet_trk_ndf;

  // std::vector<int> jet_trk_algo;
  std::vector<int> jet_trk_orig;

  // metrics
  std::vector<float> jet_trk_d0;
  std::vector<float> jet_trk_z0;
  std::vector<float> jet_trk_ip_d0;
  // std::vector<std::vector<float> >* jet_trk_ip3d_d02D;
  std::vector<float> jet_trk_ip_z0;
  std::vector<float> jet_trk_ip_d0sig;
  std::vector<float> jet_trk_ip_z0sig;
  std::vector<int> jet_trk_ip3d_grade;
  // std::vector<int> jet_trk_jf_Vertex;

  std::vector<int> jet_trk_nInnHits;
  std::vector<int> jet_trk_nNextToInnHits;
  std::vector<int> jet_trk_nBLHits;
  std::vector<int> jet_trk_nsharedBLHits;
  std::vector<int> jet_trk_nsplitBLHits;
  std::vector<int> jet_trk_nPixHits;
  std::vector<int> jet_trk_nPixHoles;
  std::vector<int> jet_trk_nsharedPixHits;
  std::vector<int> jet_trk_nsplitPixHits;
  std::vector<int> jet_trk_nSCTHits;
  std::vector<int> jet_trk_nSCTHoles;
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
  // sv1 vertices
  float jet_sv1_dR;
  float jet_sv1_Lxy;
  float jet_sv1_Lxyz;
  // --- end derived ---

  // subjets
  std::vector<Jet> trkjets;
  std::vector<Jet> vrtrkjets;

  // jet moments
  SubstructureMoments moments;

  // only defined for subjets
  float dphi_fatjet;
  float deta_fatjet;
};

struct JfVertex
{
  float chi2;
  float ndf;
  int ntrk;
  float l3d;
  float sig3d;
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
  // int algo;
  int orig;

  // NOTE: these are no longer lifetime-signed!
  float d0;
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
  int nPixHoles; // number of pixel hits
  int nsharedPixHits; // number of shared pixel hits
  int nsplitPixHits; // number of split pixel hits
  int nSCTHits; // number of sct hits
  int nSCTHoles; // number of sct hits
  int nsharedSCTHits; // number of shared sct hits
  int expectBLayerHit; // is a b-layer hit expected?

  // IP3D-like proxy for DL
  float dphi_jet;
  // float d0;
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
  Jet getJet(size_t jet, size_t subjet) const;
  size_t size(size_t jet) const;
  bool valid() const;
private:
  void init_branches(SmartChain& chain, const std::string& name);
  // subjet-wise branches
  // kinematics
  std::vector<std::vector<float> >* pt;
  std::vector<std::vector<float> >* eta;
  std::vector<std::vector<float> >* phi;
  // std::vector<std::vector<float> >* E;
  std::vector<std::vector<float> >* m;

  // track counts
  std::vector<std::vector<int> >* ntrk;
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
  std::vector<std::vector<int> >* jf_ntrkAtVx;
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

  // disable if any of the branches are missing
  bool m_valid;
};

class SubstructureMomentArray
{
public:
  SubstructureMomentArray(SmartChain& chain);
  SubstructureMoments getMoments(int) const;
  size_t size() const;
  bool valid() const;
private:
  void init_branches(SmartChain& chain);
  std::vector<float>* m_tau21;
  std::vector<float>* m_c1;
  std::vector<float>* m_c2;
  std::vector<float>* m_c1_beta2;
  std::vector<float>* m_c2_beta2;
  std::vector<float>* m_d2;
  std::vector<float>* m_d2_beta2;

  // disable if any of the branches are missing
  bool m_valid;
};

class Jets
{
public:
  Jets(SmartChain& chain, const std::string& track_prefix = "jet_ga_trk_");
  Jet getJet(size_t) const;
  size_t size() const;
  double eventWeight() const;
private:
  SmartChain* m_chain;
  Subjets m_trkjet;
  Subjets m_vrtrkjet;
  SubstructureMomentArray m_moments;

  // event
  double avgmu;
  double mc_event_weight;

  double PVx;
  double PVy;
  double PVz;

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
  std::vector<int>* jet_LabDr_HadF;

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

  std::vector<double>* jet_ipmp_pb;
  std::vector<double>* jet_ipmp_pc;
  std::vector<double>* jet_ipmp_pu;
  std::vector<double>* jet_ipmp_ptau;
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
  // std::vector<std::vector<float> >* jet_trk_dr;
  std::vector<std::vector<float> >* jet_trk_chi2;
  std::vector<std::vector<float> >* jet_trk_ndf;

  // std::vector<std::vector<int> >* jet_trk_algo;
  // std::vector<std::vector<int> >* jet_trk_orig;

  // metrics
  std::vector<std::vector<float> >* jet_trk_d0;
  std::vector<std::vector<float> >* jet_trk_z0;
  std::vector<std::vector<float> >* jet_trk_ip_d0;
  // std::vector<std::vector<float> >* jet_trk_ip3d_d02D;
  std::vector<std::vector<float> >* jet_trk_ip_z0;
  std::vector<std::vector<float> >* jet_trk_ip_d0sig;
  std::vector<std::vector<float> >* jet_trk_ip_z0sig;
  // std::vector<std::vector<int> >* jet_trk_ip3d_grade;

  std::vector<std::vector<int> >* jet_trk_nInnHits;
  std::vector<std::vector<int> >* jet_trk_nNextToInnHits;
  std::vector<std::vector<int> >* jet_trk_nBLHits;
  std::vector<std::vector<int> >* jet_trk_nsharedBLHits;
  std::vector<std::vector<int> >* jet_trk_nsplitBLHits;
  std::vector<std::vector<int> >* jet_trk_nPixHits;
  std::vector<std::vector<int> >* jet_trk_nPixHoles;
  std::vector<std::vector<int> >* jet_trk_nsharedPixHits;
  std::vector<std::vector<int> >* jet_trk_nsplitPixHits;
  std::vector<std::vector<int> >* jet_trk_nSCTHits;
  std::vector<std::vector<int> >* jet_trk_nSCTHoles;
  std::vector<std::vector<int> >* jet_trk_nsharedSCTHits;
  std::vector<std::vector<int> >* jet_trk_expectBLayerHit;

  // std::vector<std::vector<int> >* jet_trk_jf_Vertex;

  // std::vector<std::vector<float> >* jet_ga_trk_pt;

  // turn some branches off if they aren't valid
  bool m_clusters_valid;
  bool m_ipmp_valid;
  bool m_tracks_valid;
  bool m_lifetime_signed_ip;
};

// ______________________________________________________________________
// exceptions

class DisabledBranchError: public std::logic_error
{
public:
  DisabledBranchError(const std::string& what_arg);
};


// ______________________________________________________________________
// misc functions

std::vector<TrkUnit> build_tracks(const Jet& jet);
std::ostream& operator<<(std::ostream&, Jets&);
std::ostream& operator<<(std::ostream&, const SubstructureMoments&);

std::vector<Cluster> build_clusters(const Jet& jet);

// get named maps of variables (last one supplies all)
std::map<std::string, double> get_pt_eta_map(const Jet& jet);
std::map<std::string, double> get_map(const Jet& jet);

#endif // JETS_HH
