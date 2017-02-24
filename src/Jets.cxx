#include "Jets.hh"
#include "constants.hh"
#include "math.hh"

#include "SmartChain.hh"

#include "TVector3.h"

#include <vector>
#include <algorithm> // sort
#include <cassert>
#include <iostream>

// ________________________________________________________________________
// forward declare some utility functions
#define CHECK_AT(var, number) checked(var, number, #var)

namespace {
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

  std::vector<float> sort_and_pad(std::vector<float> in, int number) {
    typedef decltype(in)::value_type vtype;
    std::sort(in.begin(), in.end(), std::greater<vtype>());
    int needed = number - in.size();
    if (needed > 0) in.insert(in.end(), needed,
                              std::numeric_limits<vtype>::infinity());
    return in;
  }

  template <typename T, typename U>
  void vec_copysign(T& target, const U& reference) {
    for (size_t iii = 0; iii < reference.size(); iii++) {
      const auto& ref = reference.at(iii);
      auto& targ = target.at(iii);
      targ = std::copysign(targ, ref);
    }
  }


  void fill_sv1_vertex(Jet& j) {
    size_t nvtx = j.jet_sv1_vtx_x.size();
    if (nvtx == 0) {
      j.jet_sv1_dR = NAN;
      j.jet_sv1_Lxy = NAN;
      j.jet_sv1_Lxyz = NAN;
      return;
    }
    double dx = j.jet_sv1_vtx_x.at(0) - j.PVx;
    double dy = j.jet_sv1_vtx_y.at(0) - j.PVy;
    double dz = j.jet_sv1_vtx_z.at(0) - j.PVz;
    TVector3 vx(dx, dy, dz);
    TVector3 jet_ax;
    jet_ax.SetPtEtaPhi(j.jet_pt, j.jet_eta, j.jet_phi);
    j.jet_sv1_dR = vx.DeltaR(jet_ax);
    j.jet_sv1_Lxy = vx.Perp();
    j.jet_sv1_Lxyz = vx.Mag();
  }

  void fill_derived(Jet& jet) {
    auto sorted_d0 = sort_and_pad(jet.jet_trk_ip_d0sig, 3);
    jet.track_2_d0_significance = sorted_d0.at(1);
    jet.track_3_d0_significance = sorted_d0.at(2);
    auto sorted_z0 = sort_and_pad(jet.jet_trk_ip_z0sig, 3);
    jet.track_2_z0_significance = sorted_z0.at(1);
    jet.track_3_z0_significance = sorted_z0.at(2);

    int n_over = 0;
    for (const auto& d0: jet.jet_trk_ip_z0sig) {
      if (d0 > D0_THRESHOLD) n_over++;
    }
    jet.n_tracks_over_d0_threshold = n_over;
    // TODO: add jet width

    fill_sv1_vertex(jet);

    // fill track-level stuff
    int n_tracks = jet.jet_trk_ip_z0.size();
    for (int nnn = 0; nnn < n_tracks; nnn++) {
      // int vx_idx = jet.jet_trk_jf_Vertex.at(nnn);

// #define PB(EXT, DEF) jet.jet_jf_trk_vtx_ ## EXT.push_back(vx_idx == -1 ? DEF : jet.jet_jf_vtx_ ## EXT.at(vx_idx) )
//       PB(chi2, 0);
//       PB(ndf, 0);
//       PB(ntrk, 0);
//       PB(L3D, 0);
//       PB(sig3D, 0);
// #undef PB
    }
  }

  bool pass_checks(Jet& jet) {
    size_t n_trk = jet.jet_trk_pt.size();
    if (n_trk != jet.jet_trk_ip_d0sig.size()) return false;
    // if (n_trk != jet.jet_trk_jf_Vertex.size()) return false;
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
    float d0 = track.d0;

    track.dphi_jet = dphi;
    track.d0 = d0;
  }

  void setDerived(Cluster& cluster, const Jet& jet) {
    float dphi = phi_mpi_pi(cluster.phi, jet.jet_phi);

    cluster.dphi_jet = dphi;
  }

  template<typename T>
  T getValue(const std::vector<std::vector<T> >& vec,
             size_t p1, size_t p2, const std::string& name) {
    if (vec.size() <= p1) {
      throw std::logic_error(
        "can't access jet " + std::to_string(p1) + " in vector " + name);
    } else if (vec.at(p1).size() <= p2) {
      throw std::logic_error(
        "can't access subjet " + std::to_string(p2) + " in jet "
        + std::to_string(p1) + " in vector " + name);
    }
    return vec.at(p1).at(p2);
  }
}

Subjets::Subjets(SmartChain& chain, const std::string& name):
  m_valid(true)
{
  try {
    init_branches(chain, name);
  } catch (MissingBranchError& err) {
    m_valid = false;
  }
}
void Subjets::init_branches(SmartChain& chain, const std::string& name) {
  auto nm = [&name](const std::string& var) {
    return "jet_" + name + "_" + var;
  };
#define SET_BRANCH(var) chain.SetBranch(nm(#var), &var)
  SET_BRANCH(pt);
  SET_BRANCH(eta);
  SET_BRANCH(phi);
  // SET_BRANCH(E);
  SET_BRANCH(m);

  SET_BRANCH(ntrk);
  SET_BRANCH(ip3d_ntrk);

  // SET_BRANCH(ip2d_pb);
  // SET_BRANCH(ip2d_pc);
  // SET_BRANCH(ip2d_pu);
  SET_BRANCH(ip3d_pb);
  SET_BRANCH(ip3d_pc);
  SET_BRANCH(ip3d_pu);

  // SET_BRANCH(sv1_ntrkj);
  SET_BRANCH(sv1_ntrkv);
  SET_BRANCH(sv1_n2t);
  SET_BRANCH(sv1_m);
  SET_BRANCH(sv1_efc);
  SET_BRANCH(sv1_normdist);
  SET_BRANCH(sv1_Nvtx);
  // SET_BRANCH(sv1_sig3d);

  // SET_BRANCH(sv1_vtx_x);
  // SET_BRANCH(sv1_vtx_y);
  // SET_BRANCH(sv1_vtx_z);

  SET_BRANCH(jf_m);
  SET_BRANCH(jf_efc);
  SET_BRANCH(jf_deta);
  SET_BRANCH(jf_dphi);
  SET_BRANCH(jf_ntrkAtVx);
  SET_BRANCH(jf_nvtx);
  SET_BRANCH(jf_sig3d);
  SET_BRANCH(jf_nvtx1t);
  SET_BRANCH(jf_n2t);
  SET_BRANCH(jf_VTXsize);

  // SET_BRANCH(jf_vtx_chi2);
  // SET_BRANCH(jf_vtx_ndf);
  // SET_BRANCH(jf_vtx_ntrk);
  // SET_BRANCH(jf_vtx_L3D);
  // SET_BRANCH(jf_vtx_sig3D);

  // SET_BRANCH(mv2c00);
  SET_BRANCH(mv2c10);
  // SET_BRANCH(mv2c100);

#undef SET_BRANCH
}

Jet Subjets::getJet(size_t jet, size_t subjet) const {
  if (!m_valid) throw DisabledBranchError("no subjets to access");
#define COPY(var) o.jet_ ## var = getValue(*var, jet, subjet, #var)
  Jet o;

  // kinematics                   // kinematics
  o.jet_pt = pt->at(jet).at(subjet)*MeV;
  COPY(eta);
  COPY(phi);
  o.jet_m = m->at(jet).at(subjet)*MeV;

  // track counts
  COPY(ntrk);
  COPY(ip3d_ntrk);

  // track variables
  COPY(ip3d_pb);
  COPY(ip3d_pc);
  COPY(ip3d_pu);

  COPY(sv1_ntrkv);
  COPY(sv1_n2t);
  o.jet_sv1_m = getValue(*sv1_m, jet, subjet, "sv1_m")*MeV;
  COPY(sv1_efc);
  COPY(sv1_normdist);
  COPY(sv1_Nvtx);

  o.jet_jf_m = getValue(*jf_m, jet, subjet, "jf_m")*MeV;
  COPY(jf_efc);
  COPY(jf_deta);
  COPY(jf_dphi);
  COPY(jf_ntrkAtVx);
  COPY(jf_nvtx);
  COPY(jf_sig3d);
  COPY(jf_nvtx1t);
  COPY(jf_n2t);
  COPY(jf_VTXsize);

  COPY(mv2c10);

#undef COPY

  fill_derived(o);

  return o;
}
size_t Subjets::size(size_t jet) const {
  if (!m_valid) throw DisabledBranchError("no subjets");
  return pt->at(jet).size();
}
bool Subjets::valid() const {
  return m_valid;
}


SubstructureMomentArray::SubstructureMomentArray(SmartChain& chain):
  m_valid(true)
{
  try {
    init_branches(chain);
  } catch (MissingBranchError& err) {
    m_valid = false;
  }
}
void SubstructureMomentArray::init_branches(SmartChain& chain) {
  auto nm = [](const std::string& var) {
    return "jet_substructure_moment_" + var;
  };
#define SET_BRANCH(var) chain.SetBranch(nm(#var), &m_ ## var)
  SET_BRANCH(tau21);
  SET_BRANCH(c1);
  SET_BRANCH(c2);
  SET_BRANCH(c1_beta2);
  SET_BRANCH(c2_beta2);
  SET_BRANCH(d2);
  SET_BRANCH(d2_beta2);
#undef SET_BRANCH
}

SubstructureMoments SubstructureMomentArray::getMoments(int number) const {
  if (!m_valid) throw DisabledBranchError("no moments to access");
#define COPY(var) o.var = CHECK_AT(*m_ ## var, number)
  SubstructureMoments o;
  COPY(tau21);
  COPY(c1);
  COPY(c2);
  COPY(c1_beta2);
  COPY(c2_beta2);
  COPY(d2);
  COPY(d2_beta2);
  return o;
#undef COPY
}
size_t SubstructureMomentArray::size() const {
  if (!m_valid) throw DisabledBranchError("no moments to access");
  return m_tau21->size();
}
bool SubstructureMomentArray::valid() const {
  return m_valid;
}


Jets::Jets(SmartChain& chain, const std::string& track_prefix):
  m_chain(&chain),
  m_trkjet(chain, "trkjet"),
  m_vrtrkjet(chain, "vrtrkjet"),
  m_moments(chain),
  m_clusters_valid(true),
  m_ipmp_valid(true),
  m_tracks_valid(true),
  m_lifetime_signed_ip(false)
{
#define SET_BRANCH(variable) m_chain->SetBranch(#variable, &variable)
  // event
  SET_BRANCH(avgmu);
  m_chain->SetBranch("mcwg", &mc_event_weight);

  SET_BRANCH(PVx);
  SET_BRANCH(PVy);
  SET_BRANCH(PVz);

  // kinematics
  SET_BRANCH(jet_pt);
  SET_BRANCH(jet_eta);
  SET_BRANCH(jet_phi);
  SET_BRANCH(jet_E);
  SET_BRANCH(jet_m);

  SET_BRANCH(jet_pt_orig);
  SET_BRANCH(jet_eta_orig);
  SET_BRANCH(jet_phi_orig);

  SET_BRANCH(jet_JVT);

  // flavor label
  SET_BRANCH(jet_truthflav);
  SET_BRANCH(jet_LabDr_HadF);

  // cluster and calo
  try {
  SET_BRANCH(jet_cluster_pt                 );
  SET_BRANCH(jet_cluster_eta                );
  SET_BRANCH(jet_cluster_phi                );
  SET_BRANCH(jet_cluster_e                  );
  SET_BRANCH(jet_cluster_clustersize        );
  SET_BRANCH(jet_cluster_isolation          );
  SET_BRANCH(jet_cluster_lateral            );
  SET_BRANCH(jet_cluster_longitudinal       );
  SET_BRANCH(jet_cluster_second_lambda      );
  SET_BRANCH(jet_cluster_second_r           );
  SET_BRANCH(jet_cluster_center_lambda      );
  SET_BRANCH(jet_cluster_center_mag         );
  SET_BRANCH(jet_cluster_eng_pos            );
  SET_BRANCH(jet_cluster_em_probability     );
  SET_BRANCH(jet_cluster_eng_frac_max       );
  SET_BRANCH(jet_cluster_first_eng_dens     );
  } catch (MissingBranchError& err) {
    m_clusters_valid = false;
  }

#define ACTION(name) m_chain->SetBranch("jet_" #name, &jet_ ## name)
  ACTION(mu_pt);
  ACTION(ip3d_ntrk);
#include "btag_direct_copy_vars.hh"
#undef ACTION

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

  // try {
  // jet_ntrk is defined from the size of a vector later
#define SET_TRACK_BRANCH(variable) \
    m_chain->SetBranch(track_prefix + #variable, &jet_trk_ ## variable)
  // SET_BRANCH(jet_ip3d_ntrk);

  SET_TRACK_BRANCH(pt);
  SET_TRACK_BRANCH(eta);
  SET_TRACK_BRANCH(theta);
  SET_TRACK_BRANCH(phi);
  // SET_TRACK_BRANCH(dr);
  SET_TRACK_BRANCH(chi2);
  SET_TRACK_BRANCH(ndf);

  // SET_TRACK_BRANCH(algo);
  // SET_TRACK_BRANCH(orig);

  SET_TRACK_BRANCH(d0);
  SET_TRACK_BRANCH(z0);
  SET_TRACK_BRANCH(ip_d0);
  // SET_BRANCH(jet_trk_ip3d_d02D);
  SET_TRACK_BRANCH(ip_z0);
  try {
    SET_TRACK_BRANCH(ip_d0sig);
    SET_TRACK_BRANCH(ip_z0sig);
  } catch (MissingBranchError& err) {
    m_chain->SetBranch(track_prefix + "ip3d_d0sig", &jet_trk_ip_d0sig);
    m_chain->SetBranch(track_prefix + "ip3d_z0sig", &jet_trk_ip_z0sig);
    m_lifetime_signed_ip = true;
  }
  SET_TRACK_BRANCH(ip3d_grade);

  SET_TRACK_BRANCH(nInnHits);
  SET_TRACK_BRANCH(nNextToInnHits);
  SET_TRACK_BRANCH(nBLHits);
  SET_TRACK_BRANCH(nsharedBLHits);
  SET_TRACK_BRANCH(nsplitBLHits);
  SET_TRACK_BRANCH(nPixHits);
  SET_TRACK_BRANCH(nPixHoles);
  SET_TRACK_BRANCH(nsharedPixHits);
  SET_TRACK_BRANCH(nsplitPixHits);
  SET_TRACK_BRANCH(nSCTHits);
  SET_TRACK_BRANCH(nSCTHoles);
  SET_TRACK_BRANCH(nsharedSCTHits);
  SET_TRACK_BRANCH(expectBLayerHit);
  SET_TRACK_BRANCH(expectInnermostPixelLayerHit);
  SET_TRACK_BRANCH(expectNextToInnermostPixelLayerHit);
  // SET_TRACK_BRANCH(jf_Vertex);
#undef SET_TRACK_BRANCH

  // } catch (MissingBranchError& err) {
  //   m_tracks_valid = false;
  // }

#undef SET_BRANCH
}
size_t Jets::size() const {
  return jet_pt->size();
}
Jet Jets::getJet(size_t pos) const {
#define COPY(var) o.var = CHECK_AT(*var,pos)
#define COPY_MULT(var, mult) o.var = CHECK_AT(*var,pos)*mult
#define COPY_MULTV(var, mult) o.var = multiply<float>(CHECK_AT(*var,pos),mult)
  Jet o;
  // event
  o.avgmu = avgmu;
  o.mc_event_weight = mc_event_weight;

  o.PVx = PVx*mm;
  o.PVy = PVy*mm;
  o.PVz = PVz*mm;

  // kinematics                   // kinematics
  o.jet_pt_calibrated = jet_pt->at(pos)*MeV;
  o.jet_eta_calibrated = jet_eta->at(pos);
  o.jet_phi_calibrated = jet_phi->at(pos);
  o.jet_E = jet_E->at(pos)*MeV;
  o.jet_m = jet_m->at(pos)*MeV;

  // uncalibrated kinematics
  o.jet_pt = jet_pt_orig->at(pos)*MeV;
  o.jet_eta = jet_eta_orig->at(pos);
  o.jet_phi = jet_phi_orig->at(pos);
  o.jet_E = jet_E->at(pos)*MeV;
  o.jet_m = jet_m->at(pos)*MeV;

  COPY(jet_JVT);

  // flavor label                 // flavor label
  COPY(jet_truthflav);
  COPY(jet_LabDr_HadF);

  // cluster
  if (m_clusters_valid) {
    o.jet_ncluster = jet_cluster_pt->at(pos).size();
    o.jet_cluster_pt = multiply<float>(jet_cluster_pt->at(pos),MeV);
    COPY(jet_cluster_eta);
    COPY(jet_cluster_phi);
    o.jet_cluster_e = multiply<float>(jet_cluster_e->at(pos), MeV);
    COPY(jet_cluster_clustersize        );
    COPY(jet_cluster_isolation          );
    COPY(jet_cluster_lateral            );
    COPY(jet_cluster_longitudinal       );
    COPY(jet_cluster_second_lambda      );
    COPY(jet_cluster_second_r           );
    COPY(jet_cluster_center_lambda      );
    COPY(jet_cluster_center_mag         );
    COPY(jet_cluster_eng_pos            );
    COPY(jet_cluster_em_probability     );
    COPY(jet_cluster_eng_frac_max       );
    COPY(jet_cluster_first_eng_dens     );
  }


  // high level                   // high level
#define ACTION(var) o.jet_ ## var = CHECK_AT(*jet_ ## var, pos)
  ACTION(mu_pt);
  ACTION(ip3d_ntrk);
#include "btag_direct_copy_vars.hh"
#undef ACTION

  // sv1                          // sv1
  COPY(jet_sv1_ntrkv);
  COPY(jet_sv1_n2t);
  o.jet_sv1_m = jet_sv1_m->at(pos)*MeV;
  COPY(jet_sv1_efc);
  COPY(jet_sv1_normdist);
  COPY(jet_sv1_Nvtx);
  COPY(jet_sv1_sig3d);
  // med-level sv1                // med-level sv1
  COPY_MULTV(jet_sv1_vtx_x, mm); // Units?
  COPY_MULTV(jet_sv1_vtx_y, mm); // Units?
  COPY_MULTV(jet_sv1_vtx_z, mm); // Units?
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
  COPY_MULTV(jet_jf_vtx_L3D, mm); // TODO: check this
  COPY(jet_jf_vtx_sig3D);

  if (m_tracks_valid) {
    // track counts
    o.jet_ntrk = m_tracks_valid ? jet_trk_pt->at(pos).size() : -1;
    // track level
    // multiply is used here to give the vector the right units
    o.jet_trk_pt = multiply<float>(jet_trk_pt->at(pos),MeV);
    COPY(jet_trk_eta);
    COPY(jet_trk_theta);
    COPY(jet_trk_phi);
    // COPY(jet_trk_dr);
    COPY(jet_trk_chi2);
    COPY(jet_trk_ndf);

    // COPY(jet_trk_algo);
    // COPY(jet_trk_orig);

    // TODO: check these units
    COPY_MULTV(jet_trk_d0, mm); // Units?
    COPY_MULTV(jet_trk_z0, mm); // Units?
    o.jet_trk_ip_d0 = multiply<float>(jet_trk_ip_d0->at(pos), mm);
    COPY_MULTV(jet_trk_ip_z0, mm); // Units?
    COPY(jet_trk_ip_d0sig);
    COPY(jet_trk_ip_z0sig);
    if (m_lifetime_signed_ip) {
      vec_copysign(o.jet_trk_ip_d0sig, o.jet_trk_ip_d0);
      vec_copysign(o.jet_trk_ip_z0sig, o.jet_trk_ip_z0);
    }
    COPY(jet_trk_ip3d_grade);
    // COPY(jet_trk_jf_Vertex);

    COPY(jet_trk_nInnHits);
    COPY(jet_trk_nNextToInnHits);
    COPY(jet_trk_nBLHits);
    COPY(jet_trk_nsharedBLHits);
    COPY(jet_trk_nsplitBLHits);
    COPY(jet_trk_nPixHits);
    COPY(jet_trk_nPixHoles);
    COPY(jet_trk_nsharedPixHits);
    COPY(jet_trk_nsplitPixHits);
    COPY(jet_trk_nSCTHits);
    COPY(jet_trk_nSCTHoles);
    COPY(jet_trk_nsharedSCTHits);
    COPY(jet_trk_expectBLayerHit);
    COPY(jet_trk_expectInnermostPixelLayerHit);
    COPY(jet_trk_expectNextToInnermostPixelLayerHit);
  }

#undef COPY
#undef COPY_MULT
#undef COPY_MULTV

  // derived stuff
  fill_derived(o);

  assert(pass_checks(o));

  if (m_trkjet.valid()) {
    for (size_t sub_pos = 0; sub_pos < m_trkjet.size(pos); sub_pos++) {
      auto jet = m_trkjet.getJet(pos, sub_pos);
      jet.dphi_fatjet = phi_mpi_pi(jet.jet_phi, o.jet_phi);
      jet.deta_fatjet = jet.jet_eta - o.jet_eta;
      o.trkjets.push_back(jet);
    }
    std::sort(o.trkjets.begin(), o.trkjets.end(),
              [](const Jet& f, const Jet& s){return f.jet_pt > s.jet_pt; });

  }
  if (m_vrtrkjet.valid()) {
    for (size_t sub_pos = 0; sub_pos < m_vrtrkjet.size(pos); sub_pos++) {
      auto jet = m_vrtrkjet.getJet(pos, sub_pos);
      // this was throwing "dphi: -2.497447 - nan = nan", i.e. some
      // jets had nan for phi, weird
      jet.dphi_fatjet = phi_mpi_pi(jet.jet_phi, o.jet_phi);
      jet.deta_fatjet = jet.jet_eta - o.jet_eta;
      o.vrtrkjets.push_back(jet);
    }
    std::sort(o.vrtrkjets.begin(), o.vrtrkjets.end(),
                [](const Jet& f, const Jet& s){return f.jet_pt > s.jet_pt; });

  }
  if (m_moments.valid()) {
    o.moments = m_moments.getMoments(pos);
  }
  return o;
};
double Jets::eventWeight() const {
  return mc_event_weight;
}



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
    track.avgmu = jet.avgmu;

#define COPY(par) track.par = CHECK_AT(jet.jet_trk_ ## par, trkn)
    COPY(pt);
    COPY(eta);
    COPY(theta);
    COPY(phi);
    track.dr = std::hypot(jet.jet_eta - track.eta,
                          phi_mpi_pi(jet.jet_phi, track.phi));
    // COPY(algo);
    // COPY(orig);
    COPY(ip3d_grade);
    COPY(chi2);
    COPY(ndf);
    COPY(nInnHits);
    COPY(nNextToInnHits);
    COPY(nBLHits);
    COPY(nsharedBLHits);
    COPY(nsplitBLHits);
    COPY(nPixHits);
    COPY(nPixHoles);
    COPY(nsharedPixHits);
    COPY(nsplitPixHits);
    COPY(nSCTHits);
    COPY(nSCTHoles);
    COPY(nsharedSCTHits);
    COPY(expectBLayerHit);
    COPY(expectInnermostPixelLayerHit);
    COPY(expectNextToInnermostPixelLayerHit);

#undef COPY
    // special copy for ip3d vars
#define COPY(par) track.par = CHECK_AT(jet.jet_trk_ip_ ## par, trkn)
    COPY(d0);
    COPY(z0);
    COPY(d0sig);
    COPY(z0sig);
#undef COPY
    track.ip3d_grade = CHECK_AT(jet.jet_trk_ip3d_grade, trkn);
    setDerived(track, jet);

    // now copy the jetfitter vertices
    JfVertex vx;
    out.push_back({track, vx});

    // safety check
    // if (track.usedFor(IP3D)) n_ip3d++;
  }
  // safety check
  // assert(n_ip3d == 0 || n_ip3d == jet.jet_ip3d_ntrk);
  return out;
}
std::vector<Cluster> build_clusters(const Jet& jet) {
  std::vector<Cluster> clusters;
  size_t n_clusters = jet.jet_cluster_pt.size();
  for (size_t cluster_n = 0; cluster_n < n_clusters; cluster_n++) {
    Cluster cluster;
#define COPY(par) cluster.par = CHECK_AT(jet.jet_cluster_ ## par, cluster_n)
    COPY(pt);
    COPY(eta);
    COPY(phi);
    COPY(e);
    COPY(clustersize        );
    COPY(isolation          );
    COPY(lateral            );
    COPY(longitudinal       );
    COPY(second_lambda      );
    COPY(second_r           );
    COPY(center_lambda      );
    COPY(center_mag         );
    COPY(eng_pos            );
    COPY(em_probability     );
    COPY(eng_frac_max       );
    COPY(first_eng_dens     );
#undef COPY
    setDerived(cluster, jet);
    clusters.push_back(cluster);
  }
  return clusters;
}


// ______________________________________________________________________
// track origin checks
bool Track::usedFor(TAGGERALGO tagger_enum) const {
  assert(false);
  // return algo & 0x1 << tagger_enum;
}
bool Track::hasOrigin(TRKORIGIN orign_enum) const {
  assert(false);
  // return orig & 0x1 << orign_enum;
}

// ______________________________________________________________________
// jet labeling and printing to text

std::map<std::string, double> get_pt_eta_map(const Jet& jet) {
  std::map<std::string, double> map;
#define ADD(val) map.emplace(#val, jet.jet_ ## val)
  ADD(pt);
  ADD(eta);
  map.emplace("abs_eta", std::abs(jet.jet_eta));
  return map;
}

std::map<std::string, double> get_map(const Jet& jet) {
  std::map<std::string, double> map = get_pt_eta_map(jet);
  ADD(phi);
  ADD(m);
  ADD(ntrk);
  ADD(ip3d_ntrk);
  ADD(ip3d_pb);
  ADD(ip3d_pc);
  ADD(ip3d_pu);
#undef ADD
  return map;
}


std::ostream& operator<<(std::ostream& out, const Jet& jet) {
#define PRINT(val) out << "jet " #val " " << jet.jet_ ## val << "\n"
  PRINT(pt);
  PRINT(eta);
  PRINT(phi);
  PRINT(m);
  PRINT(JVT);
#undef PRINT
  return out;
}


std::ostream& operator<<(std::ostream& out, const SubstructureMoments& mom) {
  assert(false);
  return out;
}

// ________________________________________________________________________
// exceptions
DisabledBranchError::DisabledBranchError(const std::string& what):
  std::logic_error(what)
{
}
