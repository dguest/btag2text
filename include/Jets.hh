#ifndef JETS_HH
#define JETS_HH

#include <vector>
#include <ostream>

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
  // kinematics
  float jet_pt;
  float jet_eta;
  float jet_phi;
  float jet_E;
  float jet_m;

  // flavor label
  int jet_truthflav;

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
  std::vector<float> jet_trk_ip3d_d0;
  // std::vector<std::vector<float> >* jet_trk_ip3d_d02D;
  std::vector<float> jet_trk_ip3d_z0;
  std::vector<float> jet_trk_ip3d_d0sig;
  std::vector<float> jet_trk_ip3d_z0sig;
  std::vector<int> jet_trk_jf_Vertex;

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
  float pt;
  float eta;
  float theta;
  float phi;
  float dr;
  float chi2;
  float ndf;
  int algo;
  int orig;

  // use the IP3D d0/z0 here
  float d0;
  float z0;
  float d0sig;
  float z0sig;

  // IP3D-like proxy for DL
  float dphi_jet;
  float d0raw;

  // bitmask checks
  bool usedFor(TAGGERALGO algo) const;
  bool hasOrigin(TRKORIGIN origin) const;
};

struct TrkUnit
{
  Track track;
  JfVertex jf;
};

class Jets
{
public:
  Jets(SmartChain& chain);
  Jet getJet(int) const;
  int size() const;
private:
  SmartChain* m_chain;

  // kinematics
  std::vector<float>* jet_pt;
  std::vector<float>* jet_eta;
  std::vector<float>* jet_phi;
  std::vector<float>* jet_E;
  std::vector<float>* jet_m;

  // flavor label
  std::vector<int>* jet_truthflav;

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
  std::vector<std::vector<int> >* jet_trk_jf_Vertex;

};

std::ostream& operator<<(std::ostream&, Jets&);


#endif // JETS_HH
