#ifndef HDF5_WRITER
#define HDF5_WRITER

#include "H5Cpp.h"
#include "SubstructureMoments.hh"

#include <vector>
#include <stdexcept>
#include <cassert>

// utility macro to insert H5 types in a struct
#define H5_INSERT(INTO, CLASS, MEMBER)					\
  h5::insert(INTO, #MEMBER, offsetof(CLASS, MEMBER), &CLASS::MEMBER)


namespace h5 {
  // use float to save space, could also use double...
  typedef float outfloat_t;

  // forward declare templates
  template <typename T> H5::DataType get_type();
  template <typename T> T get_empty();

  // for insertion into compound types (H5_INSERT macro above)
  template <typename struct_t, typename member_t>
  void insert(H5::CompType& into, // compound type
              const std::string& nm, // name in file
              size_t offset, // offset in memory
              member_t struct_t::*); // pointer to member in memory

  // The actual writer class
  template <typename T>
  class Writer {
  public:
    Writer(H5::CommonFG& group, const std::string& name,
           hsize_t max_length, hsize_t batch_size);
    Writer(const Writer&) = delete;
    Writer& operator=(Writer&) = delete;
    void add_jet(std::vector<T>);
    void flush();
    void close();
  private:
    hsize_t buffer_size() const;
    H5::DataType _type;
    hsize_t _max_length;
    hsize_t _batch_size;
    hsize_t _offset;
    std::vector<T> _buffer;
    H5::DataSet _ds;
  };

  // 1d writer class
  template <typename T>
  class Writer1d {
  public:
    Writer1d(H5::CommonFG& group, const std::string& name,
             hsize_t batch_size);
    Writer1d(const Writer1d&) = delete;
    Writer1d& operator=(Writer1d&) = delete;
    void add_jet(const T&);
    void flush();
    void close();
  private:
    hsize_t buffer_size() const;
    H5::DataType _type;
    hsize_t _batch_size;
    hsize_t _offset;
    std::vector<T> _buffer;
    H5::DataSet _ds;
  };

  // Utility function to get a ``packed'' version of the datatype.
  // This lets us buffer structures which have stuff we don't want to write.
  H5::DataType packed(H5::DataType);

  // forward declare native primative types
  template<> H5::DataType get_type<float>();
  template<> H5::DataType get_type<double>();
  template<> H5::DataType get_type<int>();
  template<> H5::DataType get_type<bool>();

  // === application specific things ===
  // concrete implementation is in the cxx file

  // jet structure
  struct FatJet {
    outfloat_t pt;
    outfloat_t eta;
    outfloat_t weight;
    outfloat_t mass;

    SubstructureMoments moments;
  };
  template<> H5::DataType get_type<FatJet>();

  // cluster struct and associated template specialization
  struct Cluster {
    outfloat_t pt;
    outfloat_t deta;
    outfloat_t dphi;
    outfloat_t energy;
    bool mask;
  };
  template<> H5::DataType get_type<Cluster>();
  template<> Cluster get_empty<Cluster>();

  // track struct and associated template specialization
  struct Track {
    outfloat_t pt;
    outfloat_t deta;
    outfloat_t dphi;
    outfloat_t charge;

    outfloat_t dr;
    outfloat_t ptfrac;

    int grade;

    outfloat_t d0;
    outfloat_t z0;
    outfloat_t d0sig;
    outfloat_t z0sig;
    outfloat_t d0_ls;
    outfloat_t z0_ls;
    outfloat_t d0sig_ls;
    outfloat_t z0sig_ls;
    outfloat_t chi2;
    outfloat_t ndf;

    int numberOfInnermostPixelLayerHits;
    int numberOfNextToInnermostPixelLayerHits;
    int numberOfBLayerHits;
    int numberOfBLayerSharedHits;
    int numberOfBLayerSplitHits;
    int numberOfPixelHits;
    int numberOfPixelHoles;
    int numberOfPixelSharedHits;
    int numberOfPixelSplitHits;
    int numberOfSCTHits;
    int numberOfSCTHoles;
    int numberOfSCTSharedHits;

    int expectBLayerHit;
    int expectInnermostPixelLayerHit;
    int expectNextToInnermostPixelLayerHit;

    bool mask;
  };
  template<> H5::DataType get_type<Track>();
  template<> Track get_empty<Track>();

  // struct for high level b-tagger variables
  struct HighLevelBTag {
    HighLevelBTag();
    // kinematic
    outfloat_t pt;
    outfloat_t eta;

    int ip3d_ntrk;

    outfloat_t mu_pt; // not used, just to check if sm_mu is there
#define ACTION(var) outfloat_t var
#include "btag_direct_copy_vars.hh"
#undef ACTION

    // Jetfitter
    outfloat_t jf_dr;
    outfloat_t jf_efc;
    outfloat_t jf_m;
    int jf_n2t;
    int jf_ntrkAtVx;
    int jf_nvtx;
    int jf_nvtx1t;
    outfloat_t jf_sig3d;
    // not used in DL1
    outfloat_t jf_deta;
    outfloat_t jf_dphi;
    // int jf_VTXsize;

    // sv1
    outfloat_t sv1_dR;
    outfloat_t sv1_efc;
    outfloat_t sv1_Lxyz;
    outfloat_t sv1_Lxy;
    outfloat_t sv1_m;
    int sv1_n2t;
    int sv1_ntrkv;
    outfloat_t sv1_normdist;

    int sv1_Nvtx;

    // labeling
    int truthflav;
    int LabDr_HadF;
    int hadron_charge;

  };
  template<> H5::DataType get_type<HighLevelBTag>();

  // struct for high level b-tagger variables
  struct HighLevelSubjetBTag {
    HighLevelSubjetBTag();
    HighLevelBTag btag;
    outfloat_t dphi_fatjet;
    outfloat_t deta_fatjet;
    outfloat_t dr_fatjet;
    bool mask;
  };
  template<> H5::DataType get_type<HighLevelSubjetBTag>();

}

// _________________________________________________________________________
// implementation

namespace h5 {

  template <typename struct_t, typename member_t>
  void insert(H5::CompType& into,
              const std::string& nm,
              size_t offset,
              member_t struct_t::*) {
    into.insertMember(nm, offset, get_type<member_t>());
  }

  // 2d writter

  template <typename T>
  Writer<T>::Writer(H5::CommonFG& group, const std::string& name,
                 hsize_t max_length, hsize_t batch_size):
    _type(get_type<T>()),
    _max_length(max_length),
    _batch_size(batch_size),
    _offset(0)
  {
    if (batch_size < 1) {
      throw std::logic_error("batch size must be > 0");
    }
    // create space
    hsize_t initial[2] = {0, max_length};
    hsize_t eventual[2] = {H5S_UNLIMITED, max_length};
    H5::DataSpace space(2, initial, eventual);

    // create params
    H5::DSetCreatPropList params;
    hsize_t chunk_size[2] = {batch_size, max_length};
    params.setChunk(2, chunk_size);
    params.setDeflate(7);

    // create ds
    _ds = group.createDataSet(name, packed(_type), space, params);
  }

  template <typename T>
  void Writer<T>::add_jet(std::vector<T> jet) {
    if (buffer_size() == _batch_size) {
      flush();
    }
    auto dummy = get_empty<T>();
    jet.resize(_max_length, dummy);
    _buffer.insert(_buffer.end(), jet.begin(), jet.end());
  }
  template <typename T>
  void Writer<T>::flush() {
    if (buffer_size() == 0) return;
    // extend the ds
    hsize_t slab_dims[2] = {buffer_size(), _max_length};
    hsize_t total_dims[2] = {buffer_size() + _offset, _max_length};
    _ds.extend(total_dims);

    // setup dataspaces
    H5::DataSpace file_space = _ds.getSpace();
    H5::DataSpace mem_space(2, slab_dims);
    hsize_t offset_dims[2] = {_offset, 0};
    file_space.selectHyperslab(H5S_SELECT_SET, slab_dims, offset_dims);

    // write out
    _ds.write(_buffer.data(), _type, mem_space, file_space);
    _offset += buffer_size();
    _buffer.clear();
  }
  template <typename T>
  void Writer<T>::close() {
    _ds.close();
  }
  template <typename T>
  hsize_t Writer<T>::buffer_size() const {
    assert(_buffer.size() % _max_length == 0);
    return _buffer.size() / _max_length;
  }



  // 1d writter
  template <typename T>
  Writer1d<T>::Writer1d(H5::CommonFG& group, const std::string& name,
                        hsize_t batch_size):
    _type(get_type<T>()),
    _batch_size(batch_size),
    _offset(0)
  {
    if (batch_size < 1) {
      throw std::logic_error("batch size must be > 0");
    }
    // create space
    hsize_t initial[1] = {0};
    hsize_t eventual[1] = {H5S_UNLIMITED};
    H5::DataSpace space(1, initial, eventual);

    // create params
    H5::DSetCreatPropList params;
    hsize_t chunk_size[1] = {batch_size};
    params.setChunk(1, chunk_size);
    params.setDeflate(7);

    // create ds
    _ds = group.createDataSet(name, packed(_type), space, params);
  }

  template <typename T>
  void Writer1d<T>::add_jet(const T& jet) {
    if (buffer_size() == _batch_size) {
      flush();
    }
    _buffer.push_back(jet);
  }
  template <typename T>
  void Writer1d<T>::flush() {
    if (buffer_size() == 0) return;
    // extend the ds
    hsize_t slab_dims[1] = {buffer_size()};
    hsize_t total_dims[1] = {buffer_size() + _offset};
    _ds.extend(total_dims);

    // setup dataspaces
    H5::DataSpace file_space = _ds.getSpace();
    H5::DataSpace mem_space(1, slab_dims);
    hsize_t offset_dims[1] = {_offset};
    file_space.selectHyperslab(H5S_SELECT_SET, slab_dims, offset_dims);

    // write out
    _ds.write(_buffer.data(), _type, mem_space, file_space);
    _offset += buffer_size();
    _buffer.clear();
  }
  template <typename T>
  void Writer1d<T>::close() {
    _ds.close();
  }
  template <typename T>
  hsize_t Writer1d<T>::buffer_size() const {
    return _buffer.size();
  }

}

#endif
