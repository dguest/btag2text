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
// silly templates to make out stream thing work right
template<typename T>
class OutJet: public T
{};

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
#undef LABELED
};

template<typename T>
std::ostream& operator<<(std::ostream& out, OutJet<T>& j) {
#define OUT_COMMA(NAME) out << j.NAME << ", "
  OUT_COMMA(jet_pt);
  OUT_COMMA(jet_eta);
  OUT_COMMA(jet_phi);
  OUT_COMMA(jet_E);
  OUT_COMMA(jet_m);

  // flavor label
  OUT_COMMA(jet_truthflav);

  // high level
  // ip2d, ip3d
  out << j.jet_ip2d_pb;
  OUT_COMMA(jet_ip2d_pc);
  OUT_COMMA(jet_ip2d_pu);
  OUT_COMMA(jet_ip3d_pb);
  OUT_COMMA(jet_ip3d_pc);
  OUT_COMMA(jet_ip3d_pu);
  // sv1
  OUT_COMMA(jet_sv1_ntrkj);
  OUT_COMMA(jet_sv1_ntrkv);
  OUT_COMMA(jet_sv1_n2t);
  OUT_COMMA(jet_sv1_m);
  OUT_COMMA(jet_sv1_efc);
  OUT_COMMA(jet_sv1_normdist);
  OUT_COMMA(jet_sv1_Nvtx);
  OUT_COMMA(jet_sv1_sig3d);
  // med-level sv1
  // OUT_COMMA(jet_sv1_vtx_x);
  // OUT_COMMA(jet_sv1_vtx_y);
  // OUT_COMMA(jet_sv1_vtx_z);
  // jetfitter
  // OUT_COMMA(jet_jf_m);
  // OUT_COMMA(jet_jf_efc);
  // OUT_COMMA(jet_jf_deta);
  // OUT_COMMA(jet_jf_dphi);
  // OUT_COMMA(jet_jf_ntrkAtVx);
  // OUT_COMMA(jet_jf_nvtx);
  // OUT_COMMA(jet_jf_sig3d);
  // OUT_COMMA(jet_jf_nvtx1t);
  // OUT_COMMA(jet_jf_n2t);
  // OUT_COMMA(jet_jf_VTXsize);
  // // med-level jetfitter
  // OUT_COMMA(jet_jf_vtx_chi2);
  // OUT_COMMA(jet_jf_vtx_ndf);
  // OUT_COMMA(jet_jf_vtx_ntrk);
  // OUT_COMMA(jet_jf_vtx_L3D);
  // OUT_COMMA(jet_jf_vtx_sig3D);

  // // MV2
  // OUT_COMMA(jet_mv2c00);
  // OUT_COMMA(jet_mv2c10);
  // OUT_COMMA(jet_mv2c20);
  // OUT_COMMA(jet_mv2c100);

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
#undef OUT_COMMA
  return out;
}


// ______________________________________________________________________
// define classes

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
};

class Jets
{
public:
  Jets(SmartChain& chain);
  OutJet<Jet> getJet(int) const;
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
int Jets::size() const {
  return jet_pt->size();
}
OutJet<Jet> Jets::getJet(int pos) const {
  OutJet<Jet> o;
  // kinematics                   // kinematics
  o.jet_pt = jet_pt->at(pos);
  o.jet_eta = jet_eta->at(pos);
  o.jet_phi = jet_phi->at(pos);
  o.jet_E = jet_E->at(pos);
  o.jet_m = jet_m->at(pos);

  // flavor label                 // flavor label
  o.jet_truthflav = jet_truthflav->at(pos);

  // high level                   // high level
  // ip2d, ip3d                   // ip2d, ip3d
  o.jet_ip2d_pb = jet_ip2d_pb->at(pos);
  o.jet_ip2d_pc = jet_ip2d_pc->at(pos);
  o.jet_ip2d_pu = jet_ip2d_pu->at(pos);
  o.jet_ip3d_pb = jet_ip3d_pb->at(pos);
  o.jet_ip3d_pc = jet_ip3d_pc->at(pos);
  o.jet_ip3d_pu = jet_ip3d_pu->at(pos);
  // sv1                          // sv1
  o.jet_sv1_ntrkj = jet_sv1_ntrkj->at(pos);
  o.jet_sv1_ntrkv = jet_sv1_ntrkv->at(pos);
  o.jet_sv1_n2t = jet_sv1_n2t->at(pos);
  o.jet_sv1_m = jet_sv1_m->at(pos);
  o.jet_sv1_efc = jet_sv1_efc->at(pos);
  o.jet_sv1_normdist = jet_sv1_normdist->at(pos);
  o.jet_sv1_Nvtx = jet_sv1_Nvtx->at(pos);
  o.jet_sv1_sig3d = jet_sv1_sig3d->at(pos);
  // med-level sv1                // med-level sv1
  o.jet_sv1_vtx_x = jet_sv1_vtx_x->at(pos);
  o.jet_sv1_vtx_y = jet_sv1_vtx_y->at(pos);
  o.jet_sv1_vtx_z = jet_sv1_vtx_z->at(pos);
  // jetfitter                    // jetfitter
  o.jet_jf_m = jet_jf_m->at(pos);
  o.jet_jf_efc = jet_jf_efc->at(pos);
  o.jet_jf_deta = jet_jf_deta->at(pos);
  o.jet_jf_dphi = jet_jf_dphi->at(pos);
  o.jet_jf_ntrkAtVx = jet_jf_ntrkAtVx->at(pos);
  o.jet_jf_nvtx = jet_jf_nvtx->at(pos);
  o.jet_jf_sig3d = jet_jf_sig3d->at(pos);
  o.jet_jf_nvtx1t = jet_jf_nvtx1t->at(pos);
  o.jet_jf_n2t = jet_jf_n2t->at(pos);
  o.jet_jf_VTXsize = jet_jf_VTXsize->at(pos);
  // med-level jetfitter          // med-level jetfitter
  o.jet_jf_vtx_chi2 = jet_jf_vtx_chi2->at(pos);
  o.jet_jf_vtx_ndf = jet_jf_vtx_ndf->at(pos);
  o.jet_jf_vtx_ntrk = jet_jf_vtx_ntrk->at(pos);
  o.jet_jf_vtx_L3D = jet_jf_vtx_L3D->at(pos);
  o.jet_jf_vtx_sig3D = jet_jf_vtx_sig3D->at(pos);

  // MV2                          // MV2
  o.jet_mv2c00 = jet_mv2c00->at(pos);
  o.jet_mv2c10 = jet_mv2c10->at(pos);
  o.jet_mv2c20 = jet_mv2c20->at(pos);
  o.jet_mv2c100 = jet_mv2c100->at(pos);

  // track level                  // track level
  o.jet_trk_pt = jet_trk_pt->at(pos);
  o.jet_trk_eta = jet_trk_eta->at(pos);
  o.jet_trk_theta = jet_trk_theta->at(pos);
  o.jet_trk_phi = jet_trk_phi->at(pos);
  o.jet_trk_dr = jet_trk_dr->at(pos);
  o.jet_trk_chi2 = jet_trk_chi2->at(pos);
  o.jet_trk_ndf = jet_trk_ndf->at(pos);
  // metrics                      // metrics
  o.jet_trk_d0 = jet_trk_d0->at(pos);
  o.jet_trk_z0 = jet_trk_z0->at(pos);
  o.jet_trk_ip3d_d0 = jet_trk_ip3d_d0->at(pos);
  o.jet_trk_ip3d_z0 = jet_trk_ip3d_z0->at(pos);
  o.jet_trk_ip3d_d0sig = jet_trk_ip3d_d0sig->at(pos);
  o.jet_trk_ip3d_z0sig = jet_trk_ip3d_z0sig->at(pos);
  o.jet_trk_jf_Vertex = jet_trk_jf_Vertex->at(pos);
  return o;
};

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
