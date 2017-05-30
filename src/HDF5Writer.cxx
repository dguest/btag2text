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
    jf_dr(0),
    jf_efc(0),
    jf_m(0),
    jf_n2t(0),
    jf_ntrkAtVx(0),
    jf_nvtx(0),
    jf_nvtx1t(0),
    jf_sig3d(0),
    jf_deta(0),
    jf_dphi(0),
    sv1_dR(0),
    sv1_efc(0),
    sv1_Lxyz(0),
    sv1_Lxy(0),
    sv1_m(0),
    sv1_n2t(0),
    sv1_ntrkv(0),
    sv1_normdist(0),
    truthflav(0),
    LabDr_HadF(0)
  {
#define ACTION(var) var = 0
#include "btag_direct_copy_vars.hh"
#undef ACTION
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
    // INSERT(charge);

    INSERT(dr);
    INSERT(ptfrac);

    // INSERT(grade);

    INSERT(d0);
    INSERT(z0);
    INSERT(d0sig);
    INSERT(z0sig);
    INSERT(d0_ls);
    INSERT(z0_ls);
    INSERT(d0sig_ls);
    INSERT(z0sig_ls);
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
    // INSERT(expectInnermostPixelLayerHit);
    // INSERT(expectNextToInnermostPixelLayerHit);

    INSERT(mask);
#undef INSERT
    return type;
  }
  h5::Track get_empty_track() {
    h5::Track cl;
    cl.pt = 0;
    cl.deta = 0;
    cl.dphi = 0;
    // cl.charge = 0;

    cl.dr = 0;
    cl.ptfrac = 0;

    cl.d0 = 0;
    cl.z0 = 0;
    cl.d0sig = 0;
    cl.z0sig = 0;
    cl.d0_ls = 0;
    cl.z0_ls = 0;
    cl.d0sig_ls = 0;
    cl.z0sig_ls = 0;
    cl.chi2 = 0;
    cl.ndf = 0;

    cl.numberOfInnermostPixelLayerHits = 0;
    cl.numberOfNextToInnermostPixelLayerHits = 0;
    cl.numberOfBLayerHits = 0;
    cl.numberOfBLayerSharedHits = 0;
    cl.numberOfBLayerSplitHits = 0;
    cl.numberOfPixelHits = 0;
    cl.numberOfPixelHoles = 0;
    cl.numberOfPixelSharedHits = 0;
    cl.numberOfPixelSplitHits = 0;
    cl.numberOfSCTHits = 0;
    cl.numberOfSCTHoles = 0;
    cl.numberOfSCTSharedHits = 0;

    cl.expectBLayerHit = 0;
    // cl.expectInnermostPixelLayerHit = 0;
    // cl.expectNextToInnermostPixelLayerHit = 0;

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
#define ACTION(MEMBER)                                         \
    h5::insert(type, #MEMBER,                                         \
               offset + offsetof(h5::HighLevelBTag, MEMBER),          \
               &h5::HighLevelBTag::MEMBER)

    ACTION(pt);
    ACTION(eta);

    ACTION(ip3d_ntrk);
#include "btag_direct_copy_vars.hh"

    // sv1
    // ACTION(sv1_Nvtx);
    ACTION(sv1_ntrkv);
    ACTION(sv1_n2t);
    ACTION(sv1_m);
    ACTION(sv1_efc);
    ACTION(sv1_normdist);
    ACTION(sv1_dR);
    ACTION(sv1_Lxy);
    ACTION(sv1_Lxyz);

    // Jetfitter
    ACTION(jf_m);
    ACTION(jf_efc);
    ACTION(jf_deta);
    ACTION(jf_dphi);
    ACTION(jf_dr);
    ACTION(jf_sig3d);
    ACTION(jf_nvtx);
    ACTION(jf_ntrkAtVx);
    ACTION(jf_nvtx1t);
    ACTION(jf_n2t);
    // ACTION(jf_VTXsize);


    // labeling
    ACTION(truthflav);
    ACTION(LabDr_HadF);
    // TODO: get this working
    // ACTION(hadron_charge);

#undef ACTION
  }

}
