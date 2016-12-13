#include "HDF5Writer.hh"

#include "H5Cpp.h"


namespace {
  // concrete functions used in template specialization
  H5::CompType get_jet_type();

  H5::CompType get_cluster_type();
  h5::Cluster get_empty_cluster();

  H5::CompType get_track_type();
  h5::Track get_empty_track();

  H5::EnumType get_bool_type();

  H5::CompType get_high_level_btag_type();
}

namespace h5 {

  // actual template specializations
  template<>
  H5::DataType get_type<float>() {
    return H5::PredType::NATIVE_FLOAT;
  }
  template<>
  H5::DataType get_type<double>() {
    return H5::PredType::NATIVE_DOUBLE;
  }
  template<>
  H5::DataType get_type<int>() {
    return H5::PredType::NATIVE_INT;
  }
  template<>
  H5::DataType get_type<bool>() {
    return get_bool_type();
  }

  template<>
  H5::DataType get_type<Jet>() {
    return get_jet_type();
  }
  template<>
  H5::DataType get_type<Cluster>() {
    return get_cluster_type();
  }
  template<>
  Cluster get_empty<Cluster>() {
    return get_empty_cluster();
  }

  template<>
  H5::DataType get_type<Track>() {
    return get_track_type();
  }
  template<>
  Track get_empty<Track>() {
    return get_empty_track();
  }

  template<>
  H5::DataType get_type<HighLevelBTag>() {
    return get_high_level_btag_type();
  }

  // packing utility
  H5::DataType packed(H5::DataType in) {
    // TODO: Figure out why a normal copy constructor doesn't work here.
    //       The normal one seems to create shallow copies.
    auto out = H5::CompType(H5Tcopy(in.getId()));
    out.pack();
    return out;
  }

}

namespace {
  H5::CompType get_jet_type() {
    auto ftype = h5::get_type<h5::outfloat_t>();
    H5::CompType type(sizeof(h5::Jet));
    H5_INSERT(type, h5::Jet, weight);
    H5_INSERT(type, h5::Jet, pt);
    H5_INSERT(type, h5::Jet, eta);
    return type;
  }

  H5::CompType get_cluster_type() {
    auto ftype = h5::get_type<h5::outfloat_t>();
    auto btype = h5::get_type<bool>();
    H5::CompType type(sizeof(h5::Cluster));
#define INSERT(name) H5_INSERT(type, h5::Cluster, name)
    INSERT(pt);
    INSERT(deta);
    INSERT(dphi);
    INSERT(energy);
    INSERT(mask);
#undef INSERT
    return type;
  }
  h5::Cluster get_empty_cluster() {
    h5::Cluster cl;
    cl.pt = 0;
    cl.deta = 0;
    cl.dphi = 0;
    cl.energy = 0;
    cl.mask = true;
    return cl;
  }


  H5::CompType get_track_type() {
    auto ftype = h5::get_type<h5::outfloat_t>();
    auto btype = h5::get_type<bool>();
    H5::CompType type(sizeof(h5::Track));
#define INSERT(name) H5_INSERT(type, h5::Track, name)
    INSERT(pt);
    INSERT(deta);
    INSERT(dphi);
    INSERT(mask);
#undef INSERT
    return type;
  }
  h5::Track get_empty_track() {
    h5::Track cl;
    cl.pt = 0;
    cl.deta = 0;
    cl.dphi = 0;
    cl.mask = true;
    return cl;
  }

  H5::EnumType get_bool_type() {
    bool TRUE = true;
    bool FALSE = false;
    H5::EnumType btype(sizeof(bool));
    btype.insert("TRUE", &TRUE);
    btype.insert("FALSE", &FALSE);
    return btype;
  }

  H5::CompType get_high_level_btag_type() {
    H5::CompType type(sizeof(h5::HighLevelBTag));
#define INSERT(name) H5_INSERT(type, h5::HighLevelBTag, name)
    INSERT(pt);
    INSERT(eta);

    // IP3D
    INSERT(ip3d_pu);
    INSERT(ip3d_pc);
    INSERT(ip3d_pb);

    INSERT(ipmp_pu);
    INSERT(ipmp_pc);
    INSERT(ipmp_pb);
    INSERT(ipmp_ptau);

    // sv1
    INSERT(sv1_Nvtx);
    INSERT(sv1_ntrkv);
    INSERT(sv1_n2t);
    INSERT(sv1_m);
    INSERT(sv1_efc);
    INSERT(sv1_normdist);
    INSERT(sv1_dR);
    INSERT(sv1_Lxy);
    INSERT(sv1_Lxyz);

    // Jetfitter
    INSERT(jf_m);
    INSERT(jf_efc);
    INSERT(jf_deta);
    INSERT(jf_dphi);
    INSERT(jf_dr);
    INSERT(jf_sig3d);
    INSERT(jf_nvtx);
    INSERT(jf_ntrkAtVx);
    INSERT(jf_nvtx1t);
    INSERT(jf_n2t);
    INSERT(jf_VTXsize);

    // labeling
    INSERT(truthflav);
    INSERT(LabDr_HadF);

#undef INSERT
    return type;
  }

}
