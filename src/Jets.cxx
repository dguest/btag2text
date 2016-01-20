#include "Jets.hh"
#include "constants.hh"
#include "math.hh"

#include "SmartChain.hh"

#include <vector>
#include <algorithm> // sort
#include <cassert>
#include <iostream>

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

    // fill track-level stuff
    int n_tracks = jet.jet_trk_ip3d_z0.size();
    for (int nnn = 0; nnn < n_tracks; nnn++) {
      int vx_idx = jet.jet_trk_jf_Vertex.at(nnn);

#define PB(EXT, DEF) jet.jet_jf_trk_vtx_ ## EXT.push_back(  \
        vx_idx == -1 ? DEF : jet.jet_jf_vtx_ ## EXT.at(vx_idx) )
      PB(chi2, 0);
      PB(ndf, 0);
      PB(ntrk, 0);
      PB(L3D, 0);
      PB(sig3D, 0);
#undef PB
    }
  }

  bool pass_checks(Jet& jet) {
    size_t n_trk = jet.jet_trk_pt.size();
    if (n_trk != jet.jet_trk_ip3d_d0sig.size()) return false;
    if (n_trk != jet.jet_trk_jf_Vertex.size()) return false;
    return true;
  }

  template<typename T>
  std::vector<T> multiply(const std::vector<T>& vec, T mult) {
    std::vector<T> out;
    for (const auto entry: vec) {
      out.push_back(entry*mult);
    }
    return out;
  }

  void setDerived(Track& track, const Jet& jet) {
    float dphi = phi_mpi_pi(track.phi, jet.jet_phi);
    float d0 = d0_from_signed_d0_dphi(track.d0, dphi);

    track.dphi_jet = dphi;
    track.d0raw = d0;
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

  m_chain->SetBranch("jet_ip3d_ntrk", &jet_ip3d_ntrk);

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

  m_chain->SetBranch("jet_trk_algo", &jet_trk_algo);
  m_chain->SetBranch("jet_trk_orig", &jet_trk_orig);

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
Jet Jets::getJet(int pos) const {
  Jet o;
  // kinematics                   // kinematics
  o.jet_pt = jet_pt->at(pos)*MeV;
  o.jet_eta = jet_eta->at(pos);
  o.jet_phi = jet_phi->at(pos);
  o.jet_E = jet_E->at(pos)*MeV;
  o.jet_m = jet_m->at(pos)*MeV;

  // flavor label                 // flavor label
  o.jet_truthflav = jet_truthflav->at(pos);
  // track counts
  o.jet_ntrk = jet_trk_pt->at(pos).size();
  o.jet_ip3d_ntrk = jet_ip3d_ntrk->at(pos);

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
  o.jet_sv1_m = jet_sv1_m->at(pos)*MeV;
  o.jet_sv1_efc = jet_sv1_efc->at(pos);
  o.jet_sv1_normdist = jet_sv1_normdist->at(pos);
  o.jet_sv1_Nvtx = jet_sv1_Nvtx->at(pos);
  o.jet_sv1_sig3d = jet_sv1_sig3d->at(pos);
  // med-level sv1                // med-level sv1
  o.jet_sv1_vtx_x = jet_sv1_vtx_x->at(pos); // Units?
  o.jet_sv1_vtx_y = jet_sv1_vtx_y->at(pos); // Units?
  o.jet_sv1_vtx_z = jet_sv1_vtx_z->at(pos); // Units?
  // jetfitter                    // jetfitter
  o.jet_jf_m = jet_jf_m->at(pos)*MeV;
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

  // track level
  // multiply is used here to give the vector the right units
  o.jet_trk_pt = multiply<float>(jet_trk_pt->at(pos),MeV);
  o.jet_trk_eta = jet_trk_eta->at(pos);
  o.jet_trk_theta = jet_trk_theta->at(pos);
  o.jet_trk_phi = jet_trk_phi->at(pos);
  o.jet_trk_dr = jet_trk_dr->at(pos);
  o.jet_trk_chi2 = jet_trk_chi2->at(pos);
  o.jet_trk_ndf = jet_trk_ndf->at(pos);

  o.jet_trk_algo = jet_trk_algo->at(pos);
  o.jet_trk_orig = jet_trk_orig->at(pos);

  // metrics                      // metrics
  o.jet_trk_d0 = jet_trk_d0->at(pos); // Units?
  o.jet_trk_z0 = jet_trk_z0->at(pos); // Units?
  o.jet_trk_ip3d_d0 = jet_trk_ip3d_d0->at(pos); // Units?
  o.jet_trk_ip3d_z0 = jet_trk_ip3d_z0->at(pos); // Units?
  o.jet_trk_ip3d_d0sig = jet_trk_ip3d_d0sig->at(pos);
  o.jet_trk_ip3d_z0sig = jet_trk_ip3d_z0sig->at(pos);
  o.jet_trk_jf_Vertex = jet_trk_jf_Vertex->at(pos);

  // derived stuff
  fill_derived(o);

  assert(pass_checks(o));

  return o;
};

template<typename T>
T checked(const std::vector<T>& vec, int num, const std::string& err) {
  try {
    return vec.at(num);
  } catch (std::out_of_range& e) {
    std::string error = err + " index: " + std::to_string(num) +
      " vector length: " + std::to_string(vec.size());
    throw std::out_of_range(error);
  }
}

#define CHECK_AT(var, number) checked(var, number, #var)

// _____________________________________________________________________
// arrange track vectors into track units
std::vector<TrkUnit> build_tracks(const Jet& jet){

  // counter to check tracks
  int n_ip3d = 0;

  std::vector<TrkUnit> out;
  const size_t n_trks = jet.jet_trk_pt.size();
  for (size_t trkn = 0; trkn < n_trks; trkn++) {

    // first copy over the track parameters
    Track track;
#define COPY(par) track.par = CHECK_AT(jet.jet_trk_ ## par, trkn)
    COPY(pt);
    COPY(eta);
    COPY(theta);
    COPY(phi);
    COPY(dr);
    COPY(chi2);
    COPY(ndf);
    COPY(algo);
    COPY(orig);
#undef COPY
    // special copy for ip3d vars
#define COPY(par) track.par = CHECK_AT(jet.jet_trk_ip3d_ ## par, trkn)
    COPY(d0);
    COPY(z0);
    COPY(d0sig);
    COPY(z0sig);
#undef COPY
    setDerived(track, jet);

    // now copy the jetfitter vertices
    JfVertex vx;
#define COPY(par) vx.par = CHECK_AT(jet.jet_jf_trk_vtx_ ## par, trkn)
    COPY(chi2);
    COPY(ndf);
    COPY(ntrk);
#undef COPY
    vx.l3d = CHECK_AT(jet.jet_jf_trk_vtx_L3D, trkn);
    vx.sig3d = CHECK_AT(jet.jet_jf_trk_vtx_sig3D, trkn);

    out.push_back({track, vx});

    // safety check
    if (track.usedFor(IP3D)) n_ip3d++;
  }
  // safety check
  assert(n_ip3d == jet.jet_ip3d_ntrk);
  return out;
}


// ______________________________________________________________________
// track origin checks
bool Track::usedFor(TAGGERALGO tagger_enum) const {
  return algo & 0x1 << tagger_enum;
}
bool Track::hasOrigin(TRKORIGIN orign_enum) const {
  return orig & 0x1 << orign_enum;
}

// ______________________________________________________________________
// jet labeling and printing to text

