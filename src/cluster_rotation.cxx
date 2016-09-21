#include "cluster_rotation.hh"

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

#include <vector>

namespace {
  using namespace Eigen;
  typedef Matrix<double, 2, Dynamic> Points;

  Points points_from_vector(const std::vector<Point>& pts) {
    using namespace Eigen;
    const int n_pts = pts.size();
    Points raw(2, n_pts);
    for (int nnn = 0; nnn < n_pts; nnn++) {
      raw(0, nnn) = pts.at(nnn).x;
      raw(1, nnn) = pts.at(nnn).y;
    }
    return raw;
  }

  VectorXd weights_from_vector(const std::vector<Point>& pts) {
    using namespace Eigen;
    const int n_pts = pts.size();
    VectorXd weights(n_pts);
    for (int nnn = 0; nnn < n_pts; nnn++) {
      weights(nnn) = pts.at(nnn).w;
    }
    return weights;
  }


  Eigen::Vector2d get_principal_axis(Points x, VectorXd weights) {
    using namespace Eigen;
    // 1. get the mean, shifted values, and (unnormalized) covariance
    MatrixXd W = weights.asDiagonal(); // weights
    Vector2d mu = (x * W).rowwise().sum() / weights.sum(); // mean
    Points s = x.colwise() - mu;                     // shifted vals
    Matrix2d cov = s * W * s.adjoint(); // cov (not normalized)

    // 2. find the eigenvalues
    EigenSolver<Matrix2d> solver(cov);
    Matrix2d eigenvectors = solver.eigenvectors().real();
    Vector2d eigenvalues = solver.eigenvalues().real();
    Vector2d::Index leading;
    eigenvalues.maxCoeff(&leading);
    Vector2d principal = eigenvectors.col(leading);

    // 3. make sure the principal axis points toward the largest weight
    VectorXd projection = principal.adjoint() * s;
    VectorXd::Index largest;
    weights.maxCoeff(&largest);
    if (projection(largest) < 0) {
      return -principal;
    } else {
      return principal;
    }
  }

  Eigen::Matrix2d get_rotation_matrix(const Eigen::Vector2d& principal) {
    // rotates everything to point along y
    using namespace Eigen;
    Matrix2d out;
    auto x = principal(0);
    auto y = principal(1);
    out <<
      y, -x,
      x, y;
    return out;
  }

}

Point get_principal(const std::vector<Point>& in) {
  using namespace Eigen;
  Points raw = points_from_vector(in);
  VectorXd weights = weights_from_vector(in);
  Vector2d principal = get_principal_axis(raw, weights);
  return {principal(0), principal(1)};
}


std::vector<Point> get_points_along_principal(
  const Point& ppoint,
  const std::vector<Point>& in) {
  using namespace Eigen;
  Vector2d principal;
  principal << ppoint.x, ppoint.y;
  Matrix2d rotation = get_rotation_matrix(principal);

  Points raw = points_from_vector(in);
  VectorXd weights = weights_from_vector(in);
  Points rotated = rotation * raw;
  int n_points = in.size();
  std::vector<Point> out;
  for (int nnn = 0; nnn < n_points; nnn++) {
    out.push_back({rotated(0,nnn), rotated(1,nnn), in.at(nnn).w});
  }
  return out;
}

