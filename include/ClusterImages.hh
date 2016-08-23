#ifndef CLUSTER_IMAGES_HH
#define CLUSTER_IMAGES_HH

class Histogram;
struct Jet;
namespace H5 {
  class CommonFG;
}

#include "Cluster.hh"

#include <vector>
#include <string>

class ClusterImages
{
public:
  ClusterImages(double mass);
  ClusterImages(ClusterImages&) = delete;
  ClusterImages& operator=(ClusterImages&) = delete;
  void fill(const std::vector<Cluster>& points, const Jet& jet,
            double weight = 1.0);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  Histogram* m_image;
  double m_mass;
};


#endif
