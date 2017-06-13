#include "select_jet.hh"
#include "Jets.hh"
#include "constants.hh"

#include <cmath>

namespace {
  bool jvt_check(const Jet& jet) {
    double aeta = std::abs(jet.jet_eta_calibrated);
    double pt = jet.jet_pt_calibrated;
    double jvt = jet.jet_JVT;
    if (aeta < 2.4 && pt > 20*GeV && pt < 60*GeV) {
      if (jvt > 0.59) return true;
      return false;
    } else {
      return true;
    }
  }
}

bool select_jet(const Jet& jet) {
  if (std::isnan(jet.jet_pt)) return false;
  if (!jvt_check(jet)) return false;
  if ( jet.jet_pt_calibrated < 20*GeV) return false;
  if ( std::abs(jet.jet_eta_calibrated) > 2.5) return false;
  return true;
}

bool select_fat_jet(const Jet& jet) {
  if (std::isnan(jet.jet_pt)) return false;
  if (jet.jet_pt < 200*GeV) return false;
  if (std::abs(jet.jet_eta) > 2.5) return false;
  bool in_window = (76*GeV < jet.jet_m) && (jet.jet_m < 146*GeV);
  if (!in_window) return false;
  return true;
}
