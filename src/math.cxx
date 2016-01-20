#include "math.hh"

#include <cmath>
#include <cassert>

namespace {
  const double pi = std::atan2(0, -1);
}

double phi_mpi_pi(double phi1, double phi2) {
  double diff = phi1 - phi2;
  if (std::abs(diff) > pi) {
    diff -= std::copysign(2*pi, diff);
  }
  assert(std::abs(diff) <= pi);
  return diff;
}

double d0_from_signed_d0_dphi(double signed_d0, double dphi) {
  assert(std::abs(dphi) <= pi);
  bool neg = std::signbit(signed_d0);
  double unsigned_d0 = std::abs(signed_d0);
  if (dphi >= 0 &&  neg) return unsigned_d0;
  if (dphi <  0 && !neg) return unsigned_d0;
  if (dphi >= 0 && !neg) return -unsigned_d0;
  if (dphi <  0 &&  neg) return -unsigned_d0;
  assert(false);
}
