#ifndef OUTPUT_TEMPLATES_HH
#define OUTPUT_TEMPLATES_HH

#include <vector>
#include <ostream>

#include "ellipsis.hh"
#include "CleanStream.hh"

template<typename T>
std::string str_from_jet(const T&);

// _______________________________________________________________________
// Implementation

// make up some grammar for the output
#define OUT(NAME) out << j.NAME
#define OUT_COMMA(NAME) out << j.NAME << ", "
#define OPEN out << "[";
#define CLOSE out << "]"
#define OPEN_LIST out << "[";
#define CLOSE_LIST out << "]"
#define OUT_CLOSE(NAME) out << j.NAME << "]"
#define CS out << ", "

// basic building blocks
template<typename T, typename U = std::string>
std::string str_from_basic_jet_pars(const T& j, const U& weight = "weight") {
  CleanStream out;
  OUT_COMMA(jet_pt);
  OUT_COMMA(jet_eta);
  OUT_COMMA(jet_truthflav);
  out << weight;
  return out.str();
}
template<typename T>
std::string str_from_ip3d(const T& j) {
  CleanStream out;
  OUT_COMMA(jet_ip3d_pb);
  OUT_COMMA(jet_ip3d_pc);
  OUT(jet_ip3d_pu);
  return out.str();
}
template<typename T>
std::string str_from_ip2d(const T& j) {
  CleanStream out;
  OUT_COMMA(jet_ip2d_pb);
  OUT_COMMA(jet_ip2d_pc);
  OUT(jet_ip2d_pu);
  return out.str();
}

template<typename T>
std::string str_from_hl_vars(const T& j) {
  CleanStream out;
  // MV2
  OPEN {
    OUT_COMMA(jet_mv2c00);
    OUT_COMMA(jet_mv2c10);
    OUT_COMMA(jet_mv2c20);
    OUT(jet_mv2c100);
  } CLOSE;
  CS;

  // high level tracking
  OPEN {
    // ip2d, ip3d
    OPEN {
      out << str_from_ip2d(j);
    } CLOSE;
    CS;
    OPEN {
      out << str_from_ip3d(j);
    } CLOSE;
    CS;

    // track significance
    OPEN {
      OUT_COMMA(track_2_d0_significance);
      OUT_COMMA(track_3_d0_significance);
      OUT_COMMA(track_2_z0_significance);
      OUT_COMMA(track_3_d0_significance);
      OUT(n_tracks_over_d0_threshold);
    } CLOSE;
  } CLOSE;
  CS;

  // high level vertex
  OPEN {
    // sv1
    OPEN {
      OUT_COMMA(jet_sv1_ntrkj);
      OUT_COMMA(jet_sv1_ntrkv);
      OUT_COMMA(jet_sv1_n2t);
      OUT_COMMA(jet_sv1_m);
      OUT_COMMA(jet_sv1_efc);
      OUT_COMMA(jet_sv1_normdist);
      OUT_COMMA(jet_sv1_Nvtx);
      OUT(jet_sv1_sig3d);
    } CLOSE;
    CS;
    OPEN {
      OUT_COMMA(jet_jf_m);
      OUT_COMMA(jet_jf_efc);
      OUT_COMMA(jet_jf_deta);
      OUT_COMMA(jet_jf_dphi);
      OUT_COMMA(jet_jf_ntrkAtVx);
      OUT_COMMA(jet_jf_nvtx);
      OUT_COMMA(jet_jf_sig3d);
      OUT_COMMA(jet_jf_nvtx1t);
      OUT_COMMA(jet_jf_n2t);
      OUT(jet_jf_VTXsize);
    } CLOSE;
  } CLOSE;
  return out.str();
}

// whole jet object
template<typename T>
std::string str_from_jet(const T& j) {
  CleanStream out;
  out << str_from_basic_jet_pars(j) << ", ";
  out << str_from_hl_vars(j);
  return out.str();
}

// medium level stuff
template<typename T>
std::string str_from_jf_vertex(const T& j) {
  CleanStream out;
  OUT_COMMA(l3d);
  OUT_COMMA(sig3d);
  OUT_COMMA(ntrk);
  OUT_COMMA(chi2);
  OUT(ndf);
  return out.str();
}

template<typename T>
std::string str_from_track(const T& j) {
  CleanStream out;
  OUT_COMMA(pt);
  OUT_COMMA(eta);
  OUT_COMMA(theta);
  OUT_COMMA(phi);
  OUT_COMMA(dr);
  OUT_COMMA(chi2);
  OUT_COMMA(ndf);

  OUT_COMMA(d0);
  OUT_COMMA(z0);
  OUT_COMMA(d0sig);
  OUT(z0sig);
  return out.str();
}

template<typename T>
std::string str_from_out_unit(const T& j) {
  CleanStream out;
  OPEN {
    out << str_from_track(j.track);
  } CLOSE;
  CS;
  OPEN {
    out << str_from_jf_vertex(j.jf);
  } CLOSE;
  return out.str();
}

template<typename T>
std::string str_from_ip(const T& j) {
  CleanStream out;
  OUT_COMMA(d0);
  OUT_COMMA(d0sig);
  OUT_COMMA(z0);
  OUT(z0sig);
  return out.str();
}

template<typename T>
std::string str_from_track_kinematics(const T& j) {
  CleanStream out;
  OUT_COMMA(pt);
  OUT_COMMA(eta);
  OUT(dphi_jet);
  return out.str();
}

template<typename T>
std::string str_from_track_quality(const T& j) {
  CleanStream out;
  OUT_COMMA(chi2);
  OUT_COMMA(ndf);
  OUT_COMMA(nBLHits);
  OUT_COMMA(expectBLayerHit);
  OUT_COMMA(nInnHits);
  OUT_COMMA(nNextToInnHits);
  // OUT_COMMA(nsharedBLHits);
  // OUT_COMMA(nsplitBLHits);
  OUT_COMMA(nPixHits);
  // OUT_COMMA(nSharedPixHits);
  // OUT_COMMA(nsplitPixHits);
  OUT(nSCTHits);
  // OUT_COMMA(nsharedSCTHits);
  return out.str();
}

template<typename T>
std::string str_from_all_track_ip(const T& j) {
  CleanStream out;

  auto tracks = build_tracks(j);
  int n_tracks = tracks.size();
  for (int trackn = 0; trackn < n_tracks; trackn++) {
    const auto& track = tracks.at(trackn).track;
    OPEN {
      OPEN { out << str_from_track_kinematics(track); } CLOSE; CS;
      OPEN { out << str_from_ip(track); } CLOSE; CS;
      OPEN { out << str_from_track_quality(track); } CLOSE;
    } CLOSE;
    // by default this next line does nothing, it can be overloaded
    // for classes that spit out names
    out << ellipsis(track);
    if (trackn < n_tracks - 1) CS;
  }
  return out.str();
}

template<typename T>
std::string str_from_cluster_kinematics(const T& j) {
  CleanStream out;
  OUT_COMMA(pt);
  OUT_COMMA(eta);
  OUT_COMMA(phi);
  OUT(e);
  return out.str();
}
template<typename T>
std::string str_from_all_clusters(const T& j) {
  CleanStream out;

  auto clusters = build_clusters(j);
  int n_clusters = clusters.size();
  for (int nnn = 0; nnn < n_clusters; nnn++) {
    const auto& cluster = clusters.at(nnn);
    // OPEN {
      OPEN {
        out << str_from_cluster_kinematics(cluster);
      } CLOSE;
    // } CLOSE;
    // by default this next line does nothing, it can be overloaded
    // for classes that spit out names
    out << ellipsis(cluster);
    if (nnn < n_clusters - 1) CS;
  }
  return out.str();
}

// _________________________________________________________________________
// top level output functions

// more complicated output functions
template<typename T, typename U = std::string>
std::string str_from_ip3d_jet(const T& j, const U& weight = "weight") {
  CleanStream out;
  OPEN {
    out << str_from_basic_jet_pars(j, weight) << ", ";
    OPEN_LIST {
      out << str_from_all_track_ip(j);
    } CLOSE_LIST;
    CS;
    OPEN {
      out << str_from_ip3d(j);
    } CLOSE;
  } CLOSE;
  return out.str();
}

template<typename T, typename U = std::string>
std::string str_from_hl_jet(const T& j, const U& weight = "weight") {
  CleanStream out;
  OPEN {
    out << str_from_basic_jet_pars(j, weight) << ", ";
    OPEN {
      out << str_from_hl_vars(j);
    } CLOSE;
  } CLOSE;
  return out.str();
}


template<typename T, typename U = std::string>
std::string str_from_all_jet(const T& j, const U& weight = "weight") {
  CleanStream out;
  OPEN {
    out << str_from_basic_jet_pars(j, weight) << ", ";
    OPEN_LIST {
      out << str_from_all_track_ip(j);
    } CLOSE_LIST;
    CS;
    OPEN_LIST {
      out << str_from_all_clusters(j);
    } CLOSE_LIST;
    CS;
    OPEN {
      out << str_from_ip3d(j);
    } CLOSE; CS;
    OPEN {
      out << str_from_hl_vars(j);
    } CLOSE;
  } CLOSE;
  return out.str();
}

// _______________________________________________________________________
// subjet stuff

template<typename T>
std::string str_from_subjets(const T& jets) {
  CleanStream out;
  int n_jets = jets.size();
  for (int jetn = 0; jetn < n_jets; jetn++) {
    const auto& jet = jets.at(jetn);
    OPEN {
      OPEN {
        out << str_from_ip3d(jet);
      } CLOSE; CS;
      OPEN {
        out << str_from_hl_vars(jet);
      } CLOSE;
    } CLOSE;
    // by default this next line does nothing, it can be overloaded
    // for classes that spit out names
    out << ellipsis(jet);
    if (jetn < n_jets - 1) CS;
  }
  return out.str();
}


template<typename T, typename U = std::string>
std::string str_from_fat_jet(const T& j, const U& weight = "weight") {
  CleanStream out;
  OPEN {
    out << str_from_basic_jet_pars(j, weight) << ", ";
    OPEN_LIST {
      out << str_from_all_track_ip(j);
    } CLOSE_LIST;
    CS;
    OPEN_LIST {
      out << str_from_all_clusters(j);
    } CLOSE_LIST;
    CS;
    OPEN {
      out << str_from_ip3d(j);
    } CLOSE; CS;
    OPEN_LIST {
      out << str_from_subjets(j.vrtrkjets);
    } CLOSE_LIST;
  } CLOSE;
  return out.str();
}


#undef CS
#undef OUT_COMMA
#undef OPEN
#undef CLOSE
#undef OUT_CLOSE

#endif // OUTPUT_TEMPLATES_HH
