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

std::vector<h5::Track> get_tracks(const Jet& jet) {
  std::vector<h5::Track> tracks;
  for (const auto& track_vx: build_tracks(jet)) {
    const auto& track = track_vx.track;
    h5::Track tk;
    tk.pt = track.pt;
    tk.deta = track.eta - jet.jet_eta;
    tk.dphi = phi_mpi_pi(track.phi, jet.jet_phi);
    tk.mask = false;
    tracks.push_back(tk);
  }
  return tracks;
}

h5::HighLevelBTag get_btagging(const Jet& jet) {
  h5::HighLevelBTag btag;
#define COPY(var) btag.var = jet.jet_ ## var
  COPY(pt);
  COPY(eta);
  btag.dphi_fatjet = jet.dphi_fatjet;
  btag.deta_fatjet = jet.deta_fatjet;
  btag.dr_fatjet = std::hypot(jet.dphi_fatjet, jet.deta_fatjet);

  COPY(ip3d_pb);
  COPY(ip3d_pc);
  COPY(ip3d_pu);

  COPY(ipmp_pb);
  COPY(ipmp_pc);
  COPY(ipmp_pu);
  COPY(ipmp_ptau);

  COPY(sv1_Nvtx);
  COPY(sv1_ntrkv);
  COPY(sv1_n2t);
  COPY(sv1_m);
  COPY(sv1_efc);
  COPY(sv1_normdist);
  COPY(sv1_dR);
  COPY(sv1_Lxy);
  COPY(sv1_Lxyz);

  // Jetfitter
  COPY(jf_m);
  COPY(jf_efc);
  COPY(jf_deta);
  COPY(jf_dphi);
  btag.jf_dr = std::hypot(jet.jet_jf_deta, jet.jet_jf_dphi);
  COPY(jf_sig3d);
  COPY(jf_nvtx);
  COPY(jf_ntrkAtVx);
  COPY(jf_nvtx1t);
  COPY(jf_n2t);
  COPY(jf_VTXsize);

  COPY(mv2c10);

  // labeling
  COPY(truthflav);
  COPY(LabDr_HadF);

  btag.mask = false;

#undef COPY

  return btag;
}

