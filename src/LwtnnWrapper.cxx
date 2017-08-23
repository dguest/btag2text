#include "LwtnnWrapper.hh"
#include "Jets.hh"

#include "lwtnn/LightweightNeuralNetwork.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/NanReplacer.hh"

#include "hdf5_object_builders.hh"

#include <fstream>
#include <map>

LwtnnWrapper::LwtnnWrapper(const std::string& nn_file):
  m_lwtnn(nullptr),
  m_replacer(nullptr)
{
  if (nn_file.size() == 0) return;
  std::ifstream istream(nn_file);
  lwt::JSONConfig config = lwt::parse_json(istream);
  m_lwtnn = new lwt::LightweightNeuralNetwork(
    config.inputs, config.layers, config.outputs);
  if (config.outputs.size() != 1) {
    throw std::logic_error("network must have one output");
  }
  m_replacer = new lwt::NanReplacer(config.defaults, lwt::rep::all);
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
  inputs["jet_pt"] = jet.jet_pt;
  inputs["jet_eta"] = jet.jet_eta;

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

  const auto& subs = jet.vrtrkjets;
  for (size_t sub_n = 0; sub_n < 3; sub_n++) {
    auto sub = subs.size() > sub_n ?
      get_subjet_btagging(subs.at(sub_n)): h5::HighLevelSubjetBTag();
    auto sub_ref = [&inputs, &sub_n](const std::string& name) -> double& {
      return inputs["subjet_" + std::to_string(sub_n + 1) + "_" + name];
    };
    if (subs.size() > sub_n) {
      sub_ref("pt") = subs.at(sub_n).jet_pt;
      sub_ref("eta") = subs.at(sub_n).jet_eta;
    } else {
      sub_ref("pt") = 0;
      sub_ref("eta") = 0;
    }
#define ACTION(NAME) sub_ref(#NAME) = sub.btag.NAME
    ACTION(ip3d_ntrk);
#include "btag_direct_copy_vars.hh"
    ACTION(jf_dr);
    ACTION(jf_efc);
    ACTION(jf_m);
    ACTION(jf_n2t);
    ACTION(jf_ntrkAtVx);
    ACTION(jf_nvtx);
    ACTION(jf_nvtx1t);
    ACTION(jf_sig3d);
    // not used in DL1
    ACTION(jf_deta);
    ACTION(jf_dphi);
    // int jf_VTXsize;

    // sv1
    ACTION(sv1_dR);
    ACTION(sv1_efc);
    ACTION(sv1_Lxyz);
    ACTION(sv1_Lxy);
    ACTION(sv1_m);
    ACTION(sv1_n2t);
    ACTION(sv1_ntrkv);
    ACTION(sv1_normdist);

    ACTION(sv1_Nvtx);
#undef ACTION
#define ACTION(NAME) sub_ref(#NAME) = sub.NAME
    ACTION(dphi_fatjet);
    ACTION(deta_fatjet);
    ACTION(dr_fatjet);
    ACTION(mask);
#undef ACTION
  }
  return m_lwtnn->compute(m_replacer->replace(inputs)).at(m_output_name);
};

