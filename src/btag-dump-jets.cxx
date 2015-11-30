#include "FileCLI.hh"

#include "SmartChain.hh"
#include "TROOT.h"

#include <ostream>
#include <iostream>
#include <vector>

const double GeV = 1000;
const double MAX_PT = 1000*GeV;

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

  // high-level
  // ip2d, ip3d
  std::vector<float>* jet_ip2d_pb;
  std::vector<float>* jet_ip2d_pc;
  std::vector<float>* jet_ip2d_pu;
  std::vector<float>* jet_ip3d_pb;
  std::vector<float>* jet_ip3d_pc;
  std::vector<float>* jet_ip3d_pu;


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
  m_chain->SetBranchAddress("jet_truthflav", &jet_truthflav);

  // high level
  m_chain->SetBranchAddress("jet_ip2d_pb", &jet_ip2d_pb);
  m_chain->SetBranchAddress("jet_ip2d_pc", &jet_ip2d_pc);
  m_chain->SetBranchAddress("jet_ip2d_pu", &jet_ip2d_pu);
  m_chain->SetBranchAddress("jet_ip3d_pb", &jet_ip3d_pb);
  m_chain->SetBranchAddress("jet_ip3d_pc", &jet_ip3d_pc);
  m_chain->SetBranchAddress("jet_ip3d_pu", &jet_ip3d_pu);

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
