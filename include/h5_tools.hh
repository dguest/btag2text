#ifndef H5_TOOLS_HH
#define H5_TOOLS_HH

#include <string>

#include "H5Cpp.h"

H5::PredType get_type(int val);
H5::PredType get_type(double val);

template<typename T>
H5::PredType get_type(T) = delete; //to prevent corrosion

template<typename M>
void write_attr(H5::H5Location& loc, const std::string& name, M value) {
  auto type = get_type(value);
  loc.createAttribute(name, type, H5S_SCALAR).write(type, &value);
}

#endif
