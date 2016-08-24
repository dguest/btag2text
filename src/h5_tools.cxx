#include "h5_tools.hh"

#include "H5Cpp.h"

H5::PredType get_type(int) {
  return H5::PredType::NATIVE_INT;
}
H5::PredType get_type(double) {
  return H5::PredType::NATIVE_DOUBLE;
}
