#include "CleanStream.hh"

#include <string>
#include <iostream>

using namespace cleanstream;

std::string CleanStream::str() const {
  return _stream.str();
}

CleanStream& operator<<(CleanStream& out, const char* str) {
  out._stream << str;
  return out;
}

CleanStream& operator<<(CleanStream& out, const std::string& str) {
  out._stream << str;
  return out;
}

