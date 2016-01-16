#ifndef JETS_HH
#define JETS_HH

#include <vector>
#include <ostream>
#include <sstream>

class SmartChain;

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
  // TODO: make these work
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

  // use the IP3D d0/z0 here
  float d0;
  float z0;
  float d0sig;
  float z0sig;
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

template<typename T>
std::string str_from_jet(const T&);

std::vector<TrkUnit> build_tracks(const Jet& jet);

// _______________________________________________________________________
// Implementation

// make up some grammar for the output
#define OUT(NAME) out << j.NAME
#define OUT_COMMA(NAME) out << j.NAME << ", "
#define OPEN out << "{";
#define CLOSE out << "}"
#define OUT_CLOSE(NAME) out << j.NAME << "}"
#define CS out << ", "

template<typename T>
std::string str_from_jet(const T& j) {
  std::stringstream out;

  OUT_COMMA(jet_pt);
  OUT_COMMA(jet_eta);
  OUT_COMMA(jet_truthflav);

  // OUT_COMMA(jet_phi);
  // OUT_COMMA(jet_E);
  // OUT_COMMA(jet_m);

  // MV2
  OPEN {
    OUT_COMMA(jet_mv2c00);
    OUT_COMMA(jet_mv2c10);
    OUT_COMMA(jet_mv2c20);
    OUT(jet_mv2c100);
  } CLOSE;
  CS;

  // high level tracking
  OPEN {
    // OUT_COMMA(jet_ip2d_pb);
    // OUT_COMMA(jet_ip2d_pc);
    // OUT_COMMA(jet_ip2d_pu);

    // ip2d, ip3d
    OPEN {
      OUT_COMMA(jet_ip3d_pb);
      OUT_COMMA(jet_ip3d_pc);
      OUT(jet_ip3d_pu);
    } CLOSE;
    CS;

    // track significance
    OPEN {
      OUT_COMMA(track_2_d0_significance);
      OUT_COMMA(track_3_d0_significance);
      OUT_COMMA(track_2_z0_significance);
      OUT_COMMA(track_3_d0_significance);
      OUT(n_tracks_over_d0_threshold);
    } CLOSE;
  } CLOSE;
  CS;

  // high level vertex
  OPEN {
    // sv1
    OPEN {
      OUT_COMMA(jet_sv1_ntrkj);
      OUT_COMMA(jet_sv1_ntrkv);
      OUT_COMMA(jet_sv1_n2t);
      OUT_COMMA(jet_sv1_m);
      OUT_COMMA(jet_sv1_efc);
      OUT_COMMA(jet_sv1_normdist);
      OUT_COMMA(jet_sv1_Nvtx);
      OUT(jet_sv1_sig3d);
    } CLOSE;
    CS;
    OPEN {
      OUT_COMMA(jet_jf_m);
      OUT_COMMA(jet_jf_efc);
      OUT_COMMA(jet_jf_deta);
      OUT_COMMA(jet_jf_dphi);
      OUT_COMMA(jet_jf_ntrkAtVx);
      OUT_COMMA(jet_jf_nvtx);
      OUT_COMMA(jet_jf_sig3d);
      OUT_COMMA(jet_jf_nvtx1t);
      OUT_COMMA(jet_jf_n2t);
      OUT(jet_jf_VTXsize);
    } CLOSE;
  } CLOSE;

  // // med-level jetfitter
  // OUT_COMMA(jet_jf_vtx_chi2);
  // OUT_COMMA(jet_jf_vtx_ndf);
  // OUT_COMMA(jet_jf_vtx_ntrk);
  // OUT_COMMA(jet_jf_vtx_L3D);
  // OUT_COMMA(jet_jf_vtx_sig3D);

    // med-level sv1
    // OUT_COMMA(jet_sv1_vtx_x);
    // OUT_COMMA(jet_sv1_vtx_y);
    // OUT_COMMA(jet_sv1_vtx_z);
    // jetfitter

  // // track level
  // OUT_COMMA(jet_trk_pt);
  // OUT_COMMA(jet_trk_eta);
  // OUT_COMMA(jet_trk_theta);
  // OUT_COMMA(jet_trk_phi);
  // OUT_COMMA(jet_trk_dr);
  // OUT_COMMA(jet_trk_chi2);
  // OUT_COMMA(jet_trk_ndf);
  // // metrics
  // OUT_COMMA(jet_trk_d0);
  // OUT_COMMA(jet_trk_z0);
  // OUT_COMMA(jet_trk_ip3d_d0);
  // // std::vector<std::vector<float> >* jet_trk_ip3d_d02D;
  // OUT_COMMA(jet_trk_ip3d_z0);
  // OUT_COMMA(jet_trk_ip3d_d0sig);
  // OUT_COMMA(jet_trk_ip3d_z0sig);
  // OUT_COMMA(jet_trk_jf_Vertex);
  return out.str();
}

template<typename T>
std::string str_from_jf_vertex(const T& j) {
  std::stringstream out;
  OUT_COMMA(l3d);
  OUT_COMMA(sig3d);
  OUT_COMMA(ntrk);
  OUT_COMMA(chi2);
  OUT(ndf);
  return out.str();
}

template<typename T>
std::string str_from_track(const T& j) {
  std::stringstream out;
  OUT_COMMA(pt);
  OUT_COMMA(eta);
  OUT_COMMA(theta);
  OUT_COMMA(phi);
  OUT_COMMA(dr);
  OUT_COMMA(chi2);
  OUT_COMMA(ndf);

  OUT_COMMA(d0);
  OUT_COMMA(z0);
  OUT_COMMA(d0sig);
  OUT(z0sig);
  return out.str();
}

template<typename T>
std::string str_from_out_unit(const T& j) {
  std::stringstream out;
  OPEN {
    out << str_from_track(j.track);
  } CLOSE;
  CS;
  OPEN {
    out << str_from_jf_vertex(j.jf);
  } CLOSE;
  return out.str();
}

#undef CS
#undef OUT_COMMA
#undef OPEN
#undef CLOSE
#undef OUT_CLOSE

#endif // JETS_HH
