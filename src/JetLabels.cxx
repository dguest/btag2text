#include "JetLabels.hh"

std::vector<TrkUnitLabels> build_tracks(const JetLabels& jet) {
  TrkUnitLabels lables;
  return {lables};
}

template<>
std::string ellipsis(const TrackLabels&) {
  return ", ... ";
}
