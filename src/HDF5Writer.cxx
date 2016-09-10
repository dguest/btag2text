#include "HDF5Writer.hh"

#include "H5Cpp.h"

#include <stdexcept>
#include <cassert>

namespace {
  H5::CompType get_cluster_type();
  h5::Cluster get_empty_cluster();
}

namespace h5 {
  Writer::Writer(H5::CommonFG& group, const std::string& name,
                 hsize_t max_length, hsize_t batch_size):
    _type(get_cluster_type()),
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
    _ds = group.createDataSet(name, _type, space, params);
  }

  void Writer::add_jet(std::vector<Cluster> jet) {
    if (buffer_size() == _batch_size) {
      flush();
    }
    auto dummy = get_empty_cluster();
    jet.resize(_max_length, dummy);
    _buffer.insert(_buffer.begin(), jet.begin(), jet.end());
  }
  void Writer::flush() {
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
  void Writer::close() {
    _ds.close();
  }
  hsize_t Writer::buffer_size() const {
    assert(_buffer.size() % _max_length == 0);
    return _buffer.size() / _max_length;
  }

}

namespace {
  H5::CompType get_cluster_type() {
    auto ftype = H5::PredType::NATIVE_FLOAT;
    bool TRUE = true;
    bool FALSE = false;
    H5::EnumType btype(sizeof(bool));
    btype.insert("TRUE", &TRUE);
    btype.insert("FALSE", &FALSE);
    H5::CompType type(sizeof(h5::Cluster));
#define INSERT(name, intype)                                      \
    type.insertMember(#name, offsetof(h5::Cluster, name), intype)
    INSERT(pt, ftype);
    INSERT(deta, ftype);
    INSERT(dphi, ftype);
    INSERT(energy, ftype);
    INSERT(mask, btype);
    INSERT(weight, ftype);
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
    cl.weight = 0;
    return cl;
  }

}
