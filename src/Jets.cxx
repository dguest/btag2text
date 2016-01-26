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
    float d0 = d0_from_signed_d0_dphi(track.signed_d0, dphi);

    track.dphi_jet = dphi;
    track.d0 = d0;
  }
}


Jets::Jets(SmartChain& chain):
  m_chain(&chain)
{
#define SET_BRANCH(variable) m_chain->SetBranch(#variable, &variable)
  // kinematics
  SET_BRANCH(jet_pt);
  SET_BRANCH(jet_eta);
  SET_BRANCH(jet_phi);
  SET_BRANCH(jet_E);
  SET_BRANCH(jet_m);

  // flavor label
  SET_BRANCH(jet_truthflav);

  SET_BRANCH(jet_ip3d_ntrk);

  SET_BRANCH(jet_ip2d_pb);
  SET_BRANCH(jet_ip2d_pc);
  SET_BRANCH(jet_ip2d_pu);
  SET_BRANCH(jet_ip3d_pb);
  SET_BRANCH(jet_ip3d_pc);
  SET_BRANCH(jet_ip3d_pu);

  SET_BRANCH(jet_sv1_ntrkj);
  SET_BRANCH(jet_sv1_ntrkv);
  SET_BRANCH(jet_sv1_n2t);
  SET_BRANCH(jet_sv1_m);
  SET_BRANCH(jet_sv1_efc);
  SET_BRANCH(jet_sv1_normdist);
  SET_BRANCH(jet_sv1_Nvtx);
  SET_BRANCH(jet_sv1_sig3d);

  SET_BRANCH(jet_sv1_vtx_x);
  SET_BRANCH(jet_sv1_vtx_y);
  SET_BRANCH(jet_sv1_vtx_z);

  SET_BRANCH(jet_jf_m);
  SET_BRANCH(jet_jf_efc);
  SET_BRANCH(jet_jf_deta);
  SET_BRANCH(jet_jf_dphi);
  SET_BRANCH(jet_jf_ntrkAtVx);
  SET_BRANCH(jet_jf_nvtx);
  SET_BRANCH(jet_jf_sig3d);
  SET_BRANCH(jet_jf_nvtx1t);
  SET_BRANCH(jet_jf_n2t);
  SET_BRANCH(jet_jf_VTXsize);

  SET_BRANCH(jet_jf_vtx_chi2);
  SET_BRANCH(jet_jf_vtx_ndf);
  SET_BRANCH(jet_jf_vtx_ntrk);
  SET_BRANCH(jet_jf_vtx_L3D);
  SET_BRANCH(jet_jf_vtx_sig3D);

  SET_BRANCH(jet_mv2c00);
  SET_BRANCH(jet_mv2c10);
  SET_BRANCH(jet_mv2c20);
  SET_BRANCH(jet_mv2c100);


  SET_BRANCH(jet_trk_pt);
  SET_BRANCH(jet_trk_eta);
  SET_BRANCH(jet_trk_theta);
  SET_BRANCH(jet_trk_phi);
  SET_BRANCH(jet_trk_dr);
  SET_BRANCH(jet_trk_chi2);
  SET_BRANCH(jet_trk_ndf);

  SET_BRANCH(jet_trk_algo);
  SET_BRANCH(jet_trk_orig);

  SET_BRANCH(jet_trk_d0);
  SET_BRANCH(jet_trk_z0);
  SET_BRANCH(jet_trk_ip3d_d0);
  // SET_BRANCH(jet_trk_ip3d_d02D);
  SET_BRANCH(jet_trk_ip3d_z0);
  SET_BRANCH(jet_trk_ip3d_d0sig);
  SET_BRANCH(jet_trk_ip3d_z0sig);
  SET_BRANCH(jet_trk_ip3d_grade);
  SET_BRANCH(jet_trk_jf_Vertex);

  SET_BRANCH(jet_trk_nInnHits);
  SET_BRANCH(jet_trk_nNextToInnHits);
  SET_BRANCH(jet_trk_nBLHits);
  SET_BRANCH(jet_trk_nsharedBLHits);
  SET_BRANCH(jet_trk_nsplitBLHits);
  SET_BRANCH(jet_trk_nPixHits);
  SET_BRANCH(jet_trk_nsharedPixHits);
  SET_BRANCH(jet_trk_nsplitPixHits);
  SET_BRANCH(jet_trk_nSCTHits);
  SET_BRANCH(jet_trk_nsharedSCTHits);
  SET_BRANCH(jet_trk_expectBLayerHit);

#undef SET_BRANCH
}
int Jets::size() const {
  return jet_pt->size();
}
Jet Jets::getJet(int pos) const {
#define COPY(var) o.var = var->at(pos);
  Jet o;
  // kinematics                   // kinematics
  o.jet_pt = jet_pt->at(pos)*MeV;
  COPY(jet_eta);
  COPY(jet_phi);
  o.jet_E = jet_E->at(pos)*MeV;
  o.jet_m = jet_m->at(pos)*MeV;

  // flavor label                 // flavor label
  COPY(jet_truthflav);
  // track counts
  o.jet_ntrk = jet_trk_pt->at(pos).size();
  COPY(jet_ip3d_ntrk);

  // high level                   // high level
  // ip2d, ip3d                   // ip2d, ip3d
  COPY(jet_ip2d_pb);
  COPY(jet_ip2d_pc);
  COPY(jet_ip2d_pu);
  COPY(jet_ip3d_pb);
  COPY(jet_ip3d_pc);
  COPY(jet_ip3d_pu);
  // sv1                          // sv1
  COPY(jet_sv1_ntrkj);
  COPY(jet_sv1_ntrkv);
  COPY(jet_sv1_n2t);
  o.jet_sv1_m = jet_sv1_m->at(pos)*MeV;
  COPY(jet_sv1_efc);
  COPY(jet_sv1_normdist);
  COPY(jet_sv1_Nvtx);
  COPY(jet_sv1_sig3d);
  // med-level sv1                // med-level sv1
  COPY(jet_sv1_vtx_x); // Units?
  COPY(jet_sv1_vtx_y); // Units?
  COPY(jet_sv1_vtx_z); // Units?
  // jetfitter                    // jetfitter
  o.jet_jf_m = jet_jf_m->at(pos)*MeV;
  COPY(jet_jf_efc);
  COPY(jet_jf_deta);
  COPY(jet_jf_dphi);
  COPY(jet_jf_ntrkAtVx);
  COPY(jet_jf_nvtx);
  COPY(jet_jf_sig3d);
  COPY(jet_jf_nvtx1t);
  COPY(jet_jf_n2t);
  COPY(jet_jf_VTXsize);
  // med-level jetfitter          // med-level jetfitter
  COPY(jet_jf_vtx_chi2);
  COPY(jet_jf_vtx_ndf);
  COPY(jet_jf_vtx_ntrk);
  COPY(jet_jf_vtx_L3D);
  COPY(jet_jf_vtx_sig3D);

  // MV2                          // MV2
  COPY(jet_mv2c00);
  COPY(jet_mv2c10);
  COPY(jet_mv2c20);
  COPY(jet_mv2c100);

  // track level
  // multiply is used here to give the vector the right units
  o.jet_trk_pt = multiply<float>(jet_trk_pt->at(pos),MeV);
  COPY(jet_trk_eta);
  COPY(jet_trk_theta);
  COPY(jet_trk_phi);
  COPY(jet_trk_dr);
  COPY(jet_trk_chi2);
  COPY(jet_trk_ndf);

  COPY(jet_trk_algo);
  COPY(jet_trk_orig);

  // metrics                      // metrics
  COPY(jet_trk_d0); // Units?
  COPY(jet_trk_z0); // Units?
  o.jet_trk_ip3d_signed_d0 = jet_trk_ip3d_d0->at(pos); // Units?
  COPY(jet_trk_ip3d_z0); // Units?
  COPY(jet_trk_ip3d_d0sig);
  COPY(jet_trk_ip3d_z0sig);
  COPY(jet_trk_ip3d_grade);
  COPY(jet_trk_jf_Vertex);

  COPY(jet_trk_nInnHits);
  COPY(jet_trk_nNextToInnHits);
  COPY(jet_trk_nBLHits);
  COPY(jet_trk_nsharedBLHits);
  COPY(jet_trk_nsplitBLHits);
  COPY(jet_trk_nPixHits);
  COPY(jet_trk_nsharedPixHits);
  COPY(jet_trk_nsplitPixHits);
  COPY(jet_trk_nSCTHits);
  COPY(jet_trk_nsharedSCTHits);
  COPY(jet_trk_expectBLayerHit);

#undef COPY

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
    COPY(algo);
    COPY(orig);

    COPY(chi2);
    COPY(ndf);
    COPY(nInnHits);
    COPY(nNextToInnHits);
    COPY(nBLHits);
    COPY(nsharedBLHits);
    COPY(nsplitBLHits);
    COPY(nPixHits);
    COPY(nsharedPixHits);
    COPY(nsplitPixHits);
    COPY(nSCTHits);
    COPY(nsharedSCTHits);
    COPY(expectBLayerHit);

#undef COPY
    // special copy for ip3d vars
#define COPY(par) track.par = CHECK_AT(jet.jet_trk_ip3d_ ## par, trkn)
    COPY(signed_d0);
    COPY(z0);
    COPY(d0sig);
    COPY(z0sig);
#undef COPY
    track.ip3d_grade = CHECK_AT(jet.jet_trk_ip3d_grade, trkn);
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

