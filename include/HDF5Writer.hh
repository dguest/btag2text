#ifndef HDF5_WRITER
#define HDF5_WRITER

#include "H5Cpp.h"

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
  struct Jet {
    outfloat_t pt;
    outfloat_t eta;
    outfloat_t weight;
  };
  template<> H5::DataType get_type<Jet>();

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
    bool mask;
  };
  template<> H5::DataType get_type<Track>();
  template<> Track get_empty<Track>();

  // struct for high level b-tagger variables
  struct HighLevelBTag {

    // kinematic
    outfloat_t pt;
    outfloat_t eta;

    // ip3d
    outfloat_t ip3d_pu;
    outfloat_t ip3d_pc;
    outfloat_t ip3d_pb;

    // ipmp
    outfloat_t ipmp_pu;
    outfloat_t ipmp_pc;
    outfloat_t ipmp_pb;
    outfloat_t ipmp_ptau;

    // sv1
    int sv1_Nvtx;
    int sv1_ntrkv;
    int sv1_n2t;
    outfloat_t sv1_m;
    outfloat_t sv1_efc;
    outfloat_t sv1_normdist;
    outfloat_t sv1_dR;
    outfloat_t sv1_Lxy;
    outfloat_t sv1_Lxyz;

    // Jetfitter
    outfloat_t jf_m;
    outfloat_t jf_efc;
    outfloat_t jf_deta;
    outfloat_t jf_dphi;
    outfloat_t jf_dr;
    outfloat_t jf_sig3d;
    int jf_nvtx;
    int jf_ntrkAtVx;
    int jf_nvtx1t;
    int jf_n2t;
    int jf_VTXsize;

    // labeling
    int truthflav;
    int LabDr_HadF;

  };
  template<> H5::DataType get_type<HighLevelBTag>();

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
    _buffer.insert(_buffer.begin(), jet.begin(), jet.end());
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
