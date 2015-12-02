#include "FileCLI.hh"

#include "SmartChain.hh"
#include "TROOT.h"
#include "Tinterpreter.h"
#include <ostream>
#include <iostream>
#include <vector>

// const double GeV = 1000;
// const double MAX_PT = 1000*GeV;

// ______________________________________________________________________
// define classes
class Jets
{
public:
  Jets(SmartChain& chain);
  std::string asString(int) const;
  int number() const;
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

// _____________________________________________________________________
// class implementation

Jets::Jets(SmartChain& chain):
  m_chain(&chain)
{
  // kinematics
  m_chain->SetBranch("jet_pt", &jet_pt);
  m_chain->SetBranch("jet_eta", &jet_eta);
  m_chain->SetBranch("jet_phi", &jet_phi);
  m_chain->SetBranch("jet_E", &jet_E);
  m_chain->SetBranch("jet_m", &jet_m);

  // flavor label
  m_chain->SetBranch("jet_truthflav", &jet_truthflav);

  m_chain->SetBranch("jet_ip2d_pb", &jet_ip2d_pb);
  m_chain->SetBranch("jet_ip2d_pc", &jet_ip2d_pc);
  m_chain->SetBranch("jet_ip2d_pu", &jet_ip2d_pu);
  m_chain->SetBranch("jet_ip3d_pb", &jet_ip3d_pb);
  m_chain->SetBranch("jet_ip3d_pc", &jet_ip3d_pc);
  m_chain->SetBranch("jet_ip3d_pu", &jet_ip3d_pu);

  m_chain->SetBranch("jet_sv1_ntrkj", &jet_sv1_ntrkj);
  m_chain->SetBranch("jet_sv1_ntrkv", &jet_sv1_ntrkv);
  m_chain->SetBranch("jet_sv1_n2t", &jet_sv1_n2t);
  m_chain->SetBranch("jet_sv1_m", &jet_sv1_m);
  m_chain->SetBranch("jet_sv1_efc", &jet_sv1_efc);
  m_chain->SetBranch("jet_sv1_normdist", &jet_sv1_normdist);
  m_chain->SetBranch("jet_sv1_Nvtx", &jet_sv1_Nvtx);
  m_chain->SetBranch("jet_sv1_sig3d", &jet_sv1_sig3d);

  m_chain->SetBranch("jet_sv1_vtx_x", &jet_sv1_vtx_x);
  m_chain->SetBranch("jet_sv1_vtx_y", &jet_sv1_vtx_y);
  m_chain->SetBranch("jet_sv1_vtx_z", &jet_sv1_vtx_z);

  m_chain->SetBranch("jet_jf_m", &jet_jf_m);
  m_chain->SetBranch("jet_jf_efc", &jet_jf_efc);
  m_chain->SetBranch("jet_jf_deta", &jet_jf_deta);
  m_chain->SetBranch("jet_jf_dphi", &jet_jf_dphi);
  m_chain->SetBranch("jet_jf_ntrkAtVx", &jet_jf_ntrkAtVx);
  m_chain->SetBranch("jet_jf_nvtx", &jet_jf_nvtx);
  m_chain->SetBranch("jet_jf_sig3d", &jet_jf_sig3d);
  m_chain->SetBranch("jet_jf_nvtx1t", &jet_jf_nvtx1t);
  m_chain->SetBranch("jet_jf_n2t", &jet_jf_n2t);
  m_chain->SetBranch("jet_jf_VTXsize", &jet_jf_VTXsize);

  m_chain->SetBranch("jet_jf_vtx_chi2", &jet_jf_vtx_chi2);
  m_chain->SetBranch("jet_jf_vtx_ndf", &jet_jf_vtx_ndf);
  m_chain->SetBranch("jet_jf_vtx_ntrk", &jet_jf_vtx_ntrk);
  m_chain->SetBranch("jet_jf_vtx_L3D", &jet_jf_vtx_L3D);
  m_chain->SetBranch("jet_jf_vtx_sig3D", &jet_jf_vtx_sig3D);

  m_chain->SetBranch("jet_mv2c00", &jet_mv2c00);
  m_chain->SetBranch("jet_mv2c10", &jet_mv2c10);
  m_chain->SetBranch("jet_mv2c20", &jet_mv2c20);
  m_chain->SetBranch("jet_mv2c100", &jet_mv2c100);


  m_chain->SetBranch("jet_trk_pt", &jet_trk_pt);
  m_chain->SetBranch("jet_trk_eta", &jet_trk_eta);
  m_chain->SetBranch("jet_trk_theta", &jet_trk_theta);
  m_chain->SetBranch("jet_trk_phi", &jet_trk_phi);
  m_chain->SetBranch("jet_trk_dr", &jet_trk_dr);
  m_chain->SetBranch("jet_trk_chi2", &jet_trk_chi2);
  m_chain->SetBranch("jet_trk_ndf", &jet_trk_ndf);

  m_chain->SetBranch("jet_trk_d0", &jet_trk_d0);
  m_chain->SetBranch("jet_trk_z0", &jet_trk_z0);
  m_chain->SetBranch("jet_trk_ip3d_d0", &jet_trk_ip3d_d0);
  // m_chain->SetBranch("jet_trk_ip3d_d02D", &jet_trk_ip3d_d02D);
  m_chain->SetBranch("jet_trk_ip3d_z0", &jet_trk_ip3d_z0);
  m_chain->SetBranch("jet_trk_ip3d_d0sig", &jet_trk_ip3d_d0sig);
  m_chain->SetBranch("jet_trk_ip3d_z0sig", &jet_trk_ip3d_z0sig);
  m_chain->SetBranch("jet_trk_jf_Vertex", &jet_trk_jf_Vertex);


}
int Jets::number() const {
  return jet_pt->size();
}
std::string Jets::asString(int number) const {
  return std::to_string(jet_pt->at(number));
}

// _____________________________________________________________________
// main function

int main(int argc, char* argv[]) {
  gROOT->ProcessLine("#include <vector>");
  // printf("%s", argv[0]);
  // gROOT->GetInterpreter()->GenerateDictionary("vector<vector<float> >");
  // gROOT->GetInterpreter()->GenerateDictionary("vector<vector<int> >");
  FileCLI cli(argc, argv);

  SmartChain chain("bTag_AntiKt4EMTopoJets");
  for (const auto& in: cli.in_files()) {
    chain.add(in);
  }
  Jets jets(chain);
  int n_entries = chain.GetEntries();
  std::cout << n_entries << " entries in chain" << std::endl;
  for (int iii = 0; iii < 10; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.number();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      std::cout << jets.asString(jjj) << std::endl;
    }
  }
}
