#include "JetLabels.hh"

std::vector<TrkUnitLabels> build_tracks(const JetLabels& jet) {
  TrkUnitLabels lables;
  return {lables};
}
std::vector<ClusterLabels> build_clusters(const JetLabels& jet) {
  ClusterLabels lables;
  return {lables};
}


std::vector<TerseTrkUnitLabels> build_tracks(const TerseJetLabels& jet) {
  TerseTrkUnitLabels lables;
  return {lables};
}
std::vector<TerseClusterLabels> build_clusters(const TerseJetLabels& jet) {
  TerseClusterLabels lables;
  return {lables};
}

template<>
std::string ellipsis(const TrackLabels&) {
  return ", ... ";
}
template<>
std::string ellipsis(const ClusterLabels&) {
  return ", ... ";
}
template<>
std::string ellipsis(const JetLabels&) {
  return ", ... ";
}

template<>
std::string ellipsis(const TerseTrackLabels&) {
  return ", \"...\" ";
}
template<>
std::string ellipsis(const TerseClusterLabels&) {
  return ", \"...\" ";
}
template<>
std::string ellipsis(const TerseJetLabels&) {
  return ", \"...\" ";
}




std::string quoted(const std::string& in) {
  return "\"" + in + "\"";
}
