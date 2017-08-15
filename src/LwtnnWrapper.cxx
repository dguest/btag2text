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
  m_n_inputs = config.inputs.size();
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
  auto tracks = get_tracks(jet, TrackSelection::ALL);
  tracks.resize(m_n_inputs, h5::get_empty_track());
  for (const auto& track: tracks) {
    auto track_ref = [&inputs, &track_n](const std::string& name) -> double& {
      return inputs["track_" + std::to_string(track_n) + "_" + name];
    };
#define SETREF(NAME) track_ref(#NAME) = track.NAME
    SETREF(pt);
    SETREF(deta);
    SETREF(dphi);
    SETREF(dr);
    SETREF(ptfrac);
    SETREF(d0);
    SETREF(z0);
    SETREF(d0sig);
    SETREF(z0sig);
    SETREF(d0_ls);
    SETREF(ndf);
    SETREF(numberOfInnermostPixelLayerHits);
    SETREF(numberOfNextToInnermostPixelLayerHits);
    SETREF(numberOfBLayerHits);
    SETREF(numberOfBLayerSharedHits);
    SETREF(numberOfBLayerSplitHits);
    SETREF(numberOfPixelHits);
    SETREF(numberOfPixelHoles);
    SETREF(numberOfPixelSharedHits);
    SETREF(numberOfPixelSplitHits);
    SETREF(numberOfSCTHits);
    SETREF(numberOfSCTHoles);
    SETREF(numberOfSCTSharedHits);
    SETREF(expectBLayerHit);
    SETREF(mask);
#undef SETREF
    track_n++;
    if (inputs.size() > m_n_inputs) break;
  }
  return m_lwtnn->compute(inputs).at(m_output_name);
};

