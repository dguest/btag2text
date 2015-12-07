#include "Jets.hh"
#include "constants.hh"

#include "SmartChain.hh"

#include <vector>
#include <algorithm> // sort


// ________________________________________________________________________
// forward declare some utility functions

namespace {
  std::vector<float> sort_and_pad(std::vector<float> in, int number) {
    typedef decltype(in)::value_type vtype;
    std::sort(in.begin(), in.end(), std::greater<vtype>());
    int needed = number - in.size();
    if (needed > 0) in.insert(in.end(), needed,
                              std::numeric_limits<vtype>::infinity());
    return in;
  }
  void fill_derived(Jet& jet) {
    auto sorted_d0 = sort_and_pad(jet.jet_trk_ip3d_d0sig, 3);
    jet.track_2_d0_significance = sorted_d0.at(1);
    jet.track_3_d0_significance = sorted_d0.at(2);
    auto sorted_z0 = sort_and_pad(jet.jet_trk_ip3d_z0sig, 3);
    jet.track_2_z0_significance = sorted_z0.at(1);
    jet.track_3_z0_significance = sorted_z0.at(2);

    int n_over = 0;
    for (const auto& d0: jet.jet_trk_ip3d_z0sig) {
      if (d0 > D0_THRESHOLD) n_over++;
    }
    jet.n_tracks_over_d0_threshold = n_over;
    // TODO: add jet width
  }
}


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

  // derived stuff
  fill_derived(o);

  return o;
};
