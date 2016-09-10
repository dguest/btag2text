#ifndef HDF5_WRITER
#define HDF5_WRITER

#include "H5Cpp.h"

#include <vector>
#include <stdexcept>
#include <cassert>

namespace h5 {
  // use float to save space, could also use double...
  typedef float outfloat_t;

  // forward declare templates
  template <typename T> H5::DataType get_type();
  template <typename T> T get_empty();

  // forward declare native primative types
  template<> H5::DataType get_type<float>();
  template<> H5::DataType get_type<double>();
  template<> H5::DataType get_type<bool>();

  // cluster struct and associated template specialization
  struct Cluster {
    outfloat_t pt;
    outfloat_t deta;
    outfloat_t dphi;
    outfloat_t energy;
    bool mask;
    outfloat_t weight;
  };
  template<> H5::DataType get_type<Cluster>();
  template<> Cluster get_empty<Cluster>();

  // track struct and associated template specialization
  struct Track {
    outfloat_t pt;
    outfloat_t deta;
    outfloat_t dphi;
    bool mask;
    outfloat_t weight;
  };
  template<> H5::DataType get_type<Track>();
  template<> Track get_empty<Track>();

  // Utility function to get a ``packed'' version of the datatype.
  // This lets us buffer structures which have stuff we don't want to write.
  H5::DataType packed(H5::DataType);

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
}

// _________________________________________________________________________
// implementation

namespace h5 {
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
}

#endif
