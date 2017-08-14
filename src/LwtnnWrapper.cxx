#include "LwtnnWrapper.hh"
#include "Jets.hh"

#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/parse_json.hh"

#include "hdf5_object_builders.hh"

#include <fstream>
#include <map>

LwtnnWrapper::LwtnnWrapper(const std::string& nn_file):
  m_lwtnn(nullptr)
{
  if (nn_file.size() == 0) return;
  std::ifstream istream(nn_file);
  lwt::JSONConfig config = lwt::parse_json(istream);
  m_lwtnn = new lwt::LightweightNeuralNetwork(
    config.inputs, config.layers, config.outputs);
  if (config.outputs.size() != 1) {
    throw std::logic_error("network must have one output");
  }
  m_output_name = config.outputs.at(0);
}

LwtnnWrapper::~LwtnnWrapper() {
  delete m_lwtnn;
}

LwtnnWrapper::operator bool() const {
  return m_lwtnn;
}


double LwtnnWrapper::compute(const Jet& jet) const {
  if (!m_lwtnn) return {};
  std::map<std::string, double> inputs;
  inputs["pt"] = jet.jet_pt;
  inputs["eta"] = jet.jet_eta;

  size_t track_n = 0;
  for (const auto& track: get_tracks(jet, TrackSelection::ALL)) {
    inputs["track_" + std::to_string(track_n) + "_pt"] = track.pt;
    track_n++;
  }
  return m_lwtnn->compute(inputs).at(m_output_name);
};

