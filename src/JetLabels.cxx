#include "JetLabels.hh"

std::vector<TrkUnitLabels> build_tracks(const JetLabels& jet) {
  TrkUnitLabels lables;
  return {lables};
}

std::vector<TerseTrkUnitLabels> build_tracks(const TerseJetLabels& jet) {
  TerseTrkUnitLabels lables;
  return {lables};
}

template<>
std::string ellipsis(const TrackLabels&) {
  return ", ... ";
}

template<>
std::string ellipsis(const TerseTrackLabels&) {
  return ", \"...\" ";
}


std::string quoted(const std::string& in) {
  return "\"" + in + "\"";
}
