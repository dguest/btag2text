#include "hdf5_object_builders.hh"

#include "Jets.hh"
#include "HDF5Writer.hh"
#include "math.hh"

#include <cmath>

h5::FatJet get_fat_jet(const Jet& jet, double weight) {
  h5::FatJet hjet{jet.jet_pt, jet.jet_eta};
  hjet.weight = weight;
  hjet.moments = jet.moments;
  hjet.mass = jet.jet_m;
  return hjet;

}

std::vector<h5::Cluster> get_clusters(const Jet& jet) {
  std::vector<h5::Cluster> clusters;
  for (const auto& cluster: build_clusters(jet)) {
    h5::Cluster cl;
    cl.pt = cluster.pt;
    cl.deta = cluster.eta - jet.jet_eta;
    cl.dphi = cluster.dphi_jet;
    cl.energy = cluster.e;
    cl.mask = false;
    clusters.push_back(cl);
  }
  return clusters;
}

std::vector<h5::Track> get_tracks(const Jet& jet, TrackSelection sel) {
  std::vector<h5::Track> tracks;
  for (const auto& track_vx: build_tracks(jet)) {
    const auto& track = track_vx.track;
    if (sel == TrackSelection::IP3D_ONLY) {
      if (track.ip3d_grade < 0) continue;
    }
    double deta = track.eta - jet.jet_eta;
    double dphi = phi_mpi_pi(track.phi, jet.jet_phi);

    h5::Track tk;
    tk.pt = track.pt;
    tk.deta = deta;
    tk.dphi = dphi;
    // tk.charge = track.charge;

    tk.dr = std::hypot(deta, dphi);
    tk.ptfrac = track.pt / jet.jet_pt;

    tk.grade = track.ip3d_grade;

#define COPY(var) tk.var = track.var
    COPY(d0);
    COPY(z0);
    COPY(d0sig);
    COPY(z0sig);
    COPY(d0_ls);
    COPY(z0_ls);
    COPY(d0sig_ls);
    COPY(z0sig_ls);
    COPY(chi2);
    COPY(ndf);
#undef COPY

#define COPY(from, too) tk.too = track.from
    COPY(nInnHits,        numberOfInnermostPixelLayerHits);
    COPY(nNextToInnHits,  numberOfNextToInnermostPixelLayerHits);
    COPY(nBLHits,         numberOfBLayerHits);
    COPY(nsharedBLHits,   numberOfBLayerSharedHits);
    COPY(nsplitBLHits,    numberOfBLayerSplitHits);
    COPY(nPixHits,        numberOfPixelHits);
    COPY(nPixHoles,       numberOfPixelHoles);
    COPY(nsharedPixHits,  numberOfPixelSharedHits);
    COPY(nsplitPixHits,   numberOfPixelSplitHits);
    COPY(nSCTHits,        numberOfSCTHits);
    COPY(nSCTHoles,       numberOfSCTHoles);
    COPY(nsharedSCTHits,  numberOfSCTSharedHits);
#undef COPY
#define COPY(var) tk.var = track.var
    COPY(expectBLayerHit);
    COPY(expectInnermostPixelLayerHit);
    COPY(expectNextToInnermostPixelLayerHit);
#undef COPY

    tk.mask = false;
    tracks.push_back(tk);
  }
  return tracks;
}

h5::HighLevelSubjetBTag get_subjet_btagging(const Jet& jet) {
  h5::HighLevelSubjetBTag btag;
  btag.btag = get_btagging(jet);

  btag.dphi_fatjet = jet.dphi_fatjet;
  btag.deta_fatjet = jet.deta_fatjet;
  btag.dr_fatjet = std::hypot(jet.dphi_fatjet, jet.deta_fatjet);

  btag.mask = false;

  return btag;
}

h5::HighLevelBTag get_btagging(const Jet& jet) {
  h5::HighLevelBTag btag;
#define ACTION(var) btag.var = jet.jet_ ## var
  ACTION(pt);
  ACTION(eta);

  ACTION(ip3d_ntrk);
  ACTION(mu_pt);
#include "btag_direct_copy_vars.hh"

  ACTION(sv1_Nvtx);
  ACTION(sv1_ntrkv);
  ACTION(sv1_n2t);
  ACTION(sv1_m);
  ACTION(sv1_efc);
  ACTION(sv1_normdist);
  ACTION(sv1_dR);
  ACTION(sv1_Lxy);
  ACTION(sv1_Lxyz);

  // Jetfitter
  ACTION(jf_m);
  ACTION(jf_efc);
  ACTION(jf_deta);
  ACTION(jf_dphi);
  btag.jf_dr = std::hypot(jet.jet_jf_deta, jet.jet_jf_dphi);
  ACTION(jf_sig3d);
  ACTION(jf_nvtx);
  ACTION(jf_ntrkAtVx);
  ACTION(jf_nvtx1t);
  ACTION(jf_n2t);

  // labeling
  ACTION(truthflav);
  ACTION(LabDr_HadF);
  btag.hadron_charge = jet.hadron_charge;
#undef ACTION
  set_defaults_to_nan(btag);
  return btag;
}


void set_defaults_to_nan(h5::HighLevelBTag& btag) {
  if (btag.sv1_Nvtx == 0) {
#define NAN_VAL(name) btag.sv1_ ## name = NAN
    NAN_VAL(m);
    NAN_VAL(efc);
    NAN_VAL(normdist);
    NAN_VAL(dR);
    NAN_VAL(Lxy);
    NAN_VAL(Lxyz);
#undef NAN_VAL
  }
  if (btag.jf_nvtx + btag.jf_nvtx1t <= 0) {
#define NAN_VAL(name) btag.jf_ ## name = NAN
    NAN_VAL(m);
    NAN_VAL(efc);
    NAN_VAL(deta);
    NAN_VAL(dr);
    NAN_VAL(dphi);
    NAN_VAL(sig3d);
#undef NAN_VAL
  }
  if (btag.ip3d_ntrk == 0) {
#define NAN_VAL(name) btag.ip3d_ ## name = NAN
    NAN_VAL(pu);
    NAN_VAL(pc);
    NAN_VAL(pb);
#undef NAN_VAL
// #define NAN_VAL(name) btag.rnnip_ ## name = NAN
//     NAN_VAL(pu);
//     NAN_VAL(pc);
//     NAN_VAL(pb);
//     NAN_VAL(ptau);
// #undef NAN_VAL
  }
  if (btag.ip2d_pu < 0) {
#define NAN_VAL(name) btag.ip2d_ ## name = NAN
    NAN_VAL(pu);
    NAN_VAL(pc);
    NAN_VAL(pb);
#undef NAN_VAL
  }
  if (btag.mu_pt <= 0) {
#define NAN_VAL(name) btag.name = NAN
    NAN_VAL(mu_dR);
    NAN_VAL(mu_mombalsignif);
    NAN_VAL(mu_d0);
    NAN_VAL(mu_pTrel);
    NAN_VAL(mu_qOverPratio);
    NAN_VAL(mu_scatneighsignif);
#undef NAN_VAL
  }

}
