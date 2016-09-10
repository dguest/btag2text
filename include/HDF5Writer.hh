#ifndef HDF5_WRITER
#define HDF5_WRITER

#include "H5Cpp.h"

#include <vector>

namespace h5 {

  typedef float outfloat_t;

  struct Cluster {
    outfloat_t pt;
    outfloat_t deta;
    outfloat_t dphi;
    outfloat_t energy;
    bool mask;
    outfloat_t weight;
  };

  class Writer {
  public:
    Writer(H5::CommonFG& group, const std::string& name,
           hsize_t max_length, hsize_t batch_size);
    Writer(const Writer&) = delete;
    Writer& operator=(Writer&) = delete;
    void add_jet(std::vector<Cluster>);
    void flush();
    void close();
  private:
    H5::DataType _type;
    hsize_t _max_length;
    hsize_t _batch_size;
    hsize_t _offset;
    std::vector<Cluster> _buffer;
    H5::DataSet _ds;
  };
}
#endif
