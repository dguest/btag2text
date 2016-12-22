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

  H5::CompType get_high_level_subjet_btag_type();
  H5::CompType get_high_level_btag_type();
  void add_high_level_subjet_info(H5::CompType&);
  void add_high_level_btag_info(H5::CompType&, size_t offset);
}

namespace h5 {

  HighLevelBTag::HighLevelBTag():
    pt(0), eta(0),
    ip3d_pu(0), ip3d_pc(0), ip3d_pb(0),
    ipmp_pu(0), ipmp_pc(0), ipmp_pb(0), ipmp_ptau(0),
    sv1_Nvtx(0), sv1_ntrkv(0), sv1_n2t(0), sv1_m(0), sv1_efc(0),
    sv1_normdist(0), sv1_dR(0), sv1_Lxy(0), sv1_Lxyz(0),
    jf_m(0), jf_efc(0), jf_deta(0), jf_dphi(0), jf_dr(0),
    jf_sig3d(0), jf_nvtx(0), jf_ntrkAtVx(0), jf_nvtx1t(0),
    jf_n2t(0), jf_VTXsize(0),
    mv2c10(0),
    truthflav(0), LabDr_HadF(0)
  {
  }
  HighLevelSubjetBTag::HighLevelSubjetBTag():
    btag(),
    dphi_fatjet(0), deta_fatjet(0), dr_fatjet(0),
    mask(true)
  {
  }

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
  H5::DataType get_type<FatJet>() {
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
  template<>
  H5::DataType get_type<HighLevelSubjetBTag>() {
    return get_high_level_subjet_btag_type();
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
    typedef h5::FatJet jet_t;
    H5::CompType type(sizeof(jet_t));
    H5_INSERT(type, jet_t, weight);
    H5_INSERT(type, jet_t, pt);
    H5_INSERT(type, jet_t, eta);
    H5_INSERT(type, jet_t, mass);
#define H5_INSERT_SUB(MEMBER)                                         \
    h5::insert(type, #MEMBER,                                         \
               offsetof(jet_t, moments) +                             \
               offsetof(SubstructureMoments, MEMBER),                 \
               &SubstructureMoments::MEMBER)
    H5_INSERT_SUB(tau21);
    H5_INSERT_SUB(c1);
    H5_INSERT_SUB(c2);
    H5_INSERT_SUB(c1_beta2);
    H5_INSERT_SUB(c2_beta2);
    H5_INSERT_SUB(d2);
    H5_INSERT_SUB(d2_beta2);
#undef H5_INSERT_SUB

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

    INSERT(d0);
    INSERT(z0);
    INSERT(d0sig);
    INSERT(z0sig);
    INSERT(chi2);
    INSERT(ndf);

    INSERT(numberOfInnermostPixelLayerHits);
    INSERT(numberOfNextToInnermostPixelLayerHits);
    INSERT(numberOfBLayerHits);
    INSERT(numberOfBLayerSharedHits);
    INSERT(numberOfBLayerSplitHits);
    INSERT(numberOfPixelHits);
    INSERT(numberOfPixelHoles);
    INSERT(numberOfPixelSharedHits);
    INSERT(numberOfPixelSplitHits);
    INSERT(numberOfSCTHits);
    INSERT(numberOfSCTHoles);
    INSERT(numberOfSCTSharedHits);
    INSERT(expectBLayerHit);

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
    add_high_level_btag_info(type, 0);
    return type;
  }
  H5::CompType get_high_level_subjet_btag_type() {
    H5::CompType type(sizeof(h5::HighLevelSubjetBTag));
    add_high_level_btag_info(type, offsetof(h5::HighLevelSubjetBTag, btag));
    add_high_level_subjet_info(type);
    return type;
  }
  void add_high_level_subjet_info(H5::CompType& type) {
#define INSERT(name) H5_INSERT(type, h5::HighLevelSubjetBTag, name)
    INSERT(dphi_fatjet);
    INSERT(deta_fatjet);
    INSERT(dr_fatjet);
    INSERT(mask);
#undef INSERT
  }
  void add_high_level_btag_info(H5::CompType& type, size_t offset) {
#define H5_INSERT_SUB(MEMBER)                                         \
    h5::insert(type, #MEMBER,                                         \
               offset + offsetof(h5::HighLevelBTag, MEMBER),          \
               &h5::HighLevelBTag::MEMBER)

    H5_INSERT_SUB(pt);
    H5_INSERT_SUB(eta);

    // IP3D
    H5_INSERT_SUB(ip3d_pu);
    H5_INSERT_SUB(ip3d_pc);
    H5_INSERT_SUB(ip3d_pb);

    // H5_INSERT_SUB(ipmp_pu);
    // H5_INSERT_SUB(ipmp_pc);
    // H5_INSERT_SUB(ipmp_pb);
    // H5_INSERT_SUB(ipmp_ptau);

    // sv1
    H5_INSERT_SUB(sv1_Nvtx);
    H5_INSERT_SUB(sv1_ntrkv);
    H5_INSERT_SUB(sv1_n2t);
    H5_INSERT_SUB(sv1_m);
    H5_INSERT_SUB(sv1_efc);
    H5_INSERT_SUB(sv1_normdist);
    H5_INSERT_SUB(sv1_dR);
    H5_INSERT_SUB(sv1_Lxy);
    H5_INSERT_SUB(sv1_Lxyz);

    // Jetfitter
    H5_INSERT_SUB(jf_m);
    H5_INSERT_SUB(jf_efc);
    H5_INSERT_SUB(jf_deta);
    H5_INSERT_SUB(jf_dphi);
    H5_INSERT_SUB(jf_dr);
    H5_INSERT_SUB(jf_sig3d);
    H5_INSERT_SUB(jf_nvtx);
    H5_INSERT_SUB(jf_ntrkAtVx);
    H5_INSERT_SUB(jf_nvtx1t);
    H5_INSERT_SUB(jf_n2t);
    H5_INSERT_SUB(jf_VTXsize);

    // MV2
    H5_INSERT_SUB(mv2c10);

    // labeling
    H5_INSERT_SUB(truthflav);
    H5_INSERT_SUB(LabDr_HadF);

#undef H5_INSERT_SUB
  }

}
