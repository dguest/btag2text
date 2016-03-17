#include "FileCLI.hh"
#include "Jets.hh"
#include "SmartChain.hh"
#include "constants.hh"
#include "hist_tools.hh"
#include "get_tree.hh"
#include "math.hh"
#include "cluster_rotation.hh"

#include "ndhist/Histogram.hh"

#include "H5Cpp.h"
#include "TROOT.h"
#include "TFile.h"

#include <iostream>

class ClusterHists
{
public:
  ClusterHists();
  void fill(const Cluster& track, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  Histogram pt;
  Histogram eta;
  Histogram phi;
  Histogram e;
};

class ClusterImages
{
public:
  ClusterImages(double mass);
  void fill(const std::vector<Cluster>& points, const Jet& jet);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  Histogram image;
  double m_mass;
};

class FlavoredHists
{
public:
  void fill(const Cluster& track, int flavor, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  std::map<int, ClusterHists> m_hists;
};

// _____________________________________________________________________
// main function

int main(int argc, char* argv[]) {
  gROOT->ProcessLine("#include <vector>");
  FileCLI cli(argc, argv);
  std::string tree_name = get_tree(cli.in_files().front());

  SmartChain chain(tree_name);
  for (const auto& in: cli.in_files()) {
    chain.add(in);
  }
  Jets jets(chain);
  int n_entries = chain.GetEntries();
  // int n_entries = 1;
  std::cout << n_entries << " entries in chain" << std::endl;

  FlavoredHists hists;
  ClusterImages images(125*GeV);

  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      auto clusters = build_clusters(jet);
      for (const auto& cluster: clusters) {
        hists.fill(cluster, jet.jet_truthflav);
      }
      images.fill(clusters, jet);
    }
  }

  // save histograms
  using namespace grp;
  H5::H5File out_file(cli.out_file(), H5F_ACC_TRUNC);
  auto hist_group = out_file.createGroup(HIST);
  hists.save(hist_group, RAW);
  auto imgroup = out_file.createGroup("images");
  images.save(imgroup, "raw");
}

// ______________________________________________________________________
// hist methods

ClusterHists::ClusterHists():
  ENERGY(pt, 40*GeV),
  RANGE(eta, -2.8, 2.8),
  RANGE(phi, -3.142, 3.142),
  ENERGY(e, 40*GeV)
{
}

void ClusterHists::fill(const Cluster& track, double weight) {
#define BYNAME(name) name.fill(track.name, weight)
  BYNAME(pt);
  BYNAME(eta);
  BYNAME(phi);
  BYNAME(e);
  // quality info
#undef BYNAME
}

void ClusterHists::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(pt);
  BYNAME(eta);
  BYNAME(phi);
  BYNAME(e);
#undef BYNAME
}
void ClusterHists::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}

// ________________________________________________________________________
// cluster images
const double JET_R = 1.0;
ClusterImages::ClusterImages(double mass):
  image({{"x", 100, -JET_R, JET_R}, {"y", 100, -JET_R, JET_R}}),
  m_mass(mass)
{}
void ClusterImages::fill(const std::vector<Cluster>& clusters, const Jet& jet) {
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
    image.fill(eta_phi, point.w);
  }
}
void ClusterImages::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(image);
#undef BYNAME
}
void ClusterImages::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}
// ________________________________________________________________________
// flavored hists

void FlavoredHists::fill(const Cluster& track, int ftl, double weight) {
  m_hists[ftl].fill(track, weight);
}

void FlavoredHists::save(H5::CommonFG& out) const {
  for (const auto& flav_and_hist: m_hists) {
    std::string fg_name = std::to_string(flav_and_hist.first);
    flav_and_hist.second.save(out, fg_name);
  }
}
void FlavoredHists::save(H5::CommonFG& out, std::string name) const {
  H5::Group group(out.createGroup(name));
  save(group);
}

