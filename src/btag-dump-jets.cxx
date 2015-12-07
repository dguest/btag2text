#include "FileCLI.hh"
#include "Jets.hh"
#include "SmartChain.hh"

#include "TROOT.h"

#include <ostream>
#include <iostream>
#include <limits>

// const double GeV = 1000;
// const double MAX_PT = 1000*GeV;



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

#undef LABELED
};

template<typename T>
std::ostream& operator<<(std::ostream& out, OutJet<T>& j) {

  // make up some grammar for the output
#define OUT(NAME) out << j.NAME
#define OUT_COMMA(NAME) out << j.NAME << ", "
#define OPEN out << "{";
#define CLOSE out << "}"
#define OUT_CLOSE(NAME) out << j.NAME << "}"
#define CS out << ", "

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
#undef CS
#undef OUT_COMMA
#undef OPEN
#undef CLOSE
#undef OUT_CLOSE
  return out;
}



// _____________________________________________________________________
// class implementation


// _____________________________________________________________________
// main function

int main(int argc, char* argv[]) {
  gROOT->ProcessLine("#include <vector>");
  FileCLI cli(argc, argv);

  SmartChain chain("bTag_AntiKt4EMTopoJets");
  for (const auto& in: cli.in_files()) {
    chain.add(in);
  }
  Jets jets(chain);
  int n_entries = chain.GetEntries();
  std::cout << n_entries << " entries in chain" << std::endl;

  OutJet<JetLabels> labels;
  std::cout << labels << std::endl;

  for (int iii = 0; iii < 10; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      std::cout << jet << std::endl;
    }
  }
}
