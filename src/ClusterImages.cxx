#include "ClusterImages.hh"
#include "cluster_rotation.hh"
#include "Jets.hh"
#include "math.hh"

#include "ndhist/Histogram.hh"
#include "H5Cpp.h"

#include <string>
#include <cassert>

// ________________________________________________________________________
// cluster images
const double JET_R = 1.0;
const std::vector<Axis> XY_AX{
  {"x", 100, -JET_R, JET_R},
  {"y", 100, -JET_R, JET_R}};

ClusterImages::ClusterImages(double mass):
  m_image(new Histogram(XY_AX)),
  m_tracks(new Histogram(XY_AX)),
  m_em_energy(new Histogram(XY_AX)),
  m_had_energy(new Histogram(XY_AX)),
  m_mass(mass)
{}
void ClusterImages::fill(const std::vector<Cluster>& clusters,
                         const Jet& jet,
                         double weight) {
  std::vector<Point> cluster_points;
  for (const auto& cluster: clusters) {
    double dphi = phi_mpi_pi(cluster.phi, jet.jet_phi);
    double deta = cluster.eta - jet.jet_eta;
    double jet_r = 2 * m_mass / jet.jet_pt;
    cluster_points.push_back({deta / jet_r, dphi / jet_r, cluster.e});
  }

  Point principal = get_principal(cluster_points);

  const auto rotated_clusters = get_points_along_principal(
    principal,
    cluster_points);
  for (size_t iii = 0; iii < rotated_clusters.size(); iii++) {
    const auto& point = rotated_clusters.at(iii);
    const auto& cluster = clusters.at(iii);
    std::map<std::string, double> eta_phi {
      {"x", point.x}, {"y", point.y} };
    m_image->fill(eta_phi, cluster.e * weight);

    double emfc = cluster.em_probability;

    assert(0 <= emfc && emfc <= 1);
    m_em_energy->fill(eta_phi, emfc * cluster.e * weight);
    m_had_energy->fill(eta_phi, (1-emfc) * cluster.e * weight);
  }

  const auto tracks = build_tracks(jet);
  std::vector<Point> track_points;
  for (const auto& track_grp: tracks) {
    const auto& track = track_grp.track;
    double dphi = phi_mpi_pi(track.phi, jet.jet_phi);
    double deta = track.eta - jet.jet_eta;
    double jet_r = 2 * m_mass / jet.jet_pt;
    track_points.push_back({deta / jet_r, dphi / jet_r, track.pt});
  }
  const auto rotated_tracks = get_points_along_principal(
    principal,
    track_points);
  for (size_t iii = 0; iii < rotated_tracks.size(); iii++) {
    const auto& point = rotated_tracks.at(iii);
    const auto& track = tracks.at(iii).track;
    std::map<std::string, double> eta_phi {
      {"x", point.x}, {"y", point.y} };
    m_tracks->fill(eta_phi, track.pt);
  }
}
void ClusterImages::save(H5::CommonFG& out) const {
#define BYNAME(name) m_ ## name->write_to(out, #name)
  BYNAME(image);
  BYNAME(tracks);
  BYNAME(em_energy);
  BYNAME(had_energy);
#undef BYNAME
}
void ClusterImages::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}
