#include "ClusterImages.hh"
#include "cluster_rotation.hh"
#include "Jets.hh"
#include "math.hh"

#include "ndhist/Histogram.hh"
#include "H5Cpp.h"

#include <string>

// ________________________________________________________________________
// cluster images
const double JET_R = 1.0;
ClusterImages::ClusterImages(double mass):
  m_image(new Histogram{
      {"x", 100, -JET_R, JET_R},
      {"y", 100, -JET_R, JET_R}}),
  m_mass(mass)
{}
void ClusterImages::fill(const std::vector<Cluster>& clusters,
                         const Jet& jet,
                         double weight) {
  std::vector<Point> points;
  for (const auto& cluster: clusters) {
    double dphi = phi_mpi_pi(cluster.phi, jet.jet_phi);
    double deta = cluster.eta - jet.jet_eta;
    double jet_r = 2 * m_mass / jet.jet_pt;
    points.push_back({deta / jet_r, dphi / jet_r, cluster.e});
  }
  const auto rotated_points = get_points_along_principal(points);
  for (const auto& point: rotated_points) {
    std::map<std::string, double> eta_phi {
      {"x", point.x}, {"y", point.y} };
    m_image->fill(eta_phi, point.w * weight);
  }
}
void ClusterImages::save(H5::CommonFG& out) const {
#define BYNAME(name) m_ ## name->write_to(out, #name)
  BYNAME(image);
#undef BYNAME
}
void ClusterImages::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}
