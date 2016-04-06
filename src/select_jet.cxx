#include "select_jet.hh"
#include "Jets.hh"
#include "constants.hh"

#include <cmath>

namespace {
  bool jvt_check(const Jet& jet) {
    double aeta = std::abs(jet.jet_eta);
    double pt = jet.jet_pt;
    double jvt = jet.jet_JVT;
    if (aeta < 2.4 && pt > 20*GeV && pt < 60*GeV) {
      if (jvt > 0.59) return true;
      else return false;
    } else {
      return true;
    }
  }
}

bool select_jet(const Jet& jet) {
  if (!jvt_check(jet)) return false;
  if ( jet.jet_pt < 20*GeV) return false;
  return true;
}
