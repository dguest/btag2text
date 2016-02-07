#ifndef CLEAN_STREAM_HH
#define CLEAN_STREAM_HH

#include <sstream>
#include <cmath>


namespace cleanstream {
  const std::string TEXT_INF = "\"Inf\"";
  const std::string TEXT_NINF = "\"-Inf\"";
  const std::string TEXT_NAN = "\"NaN\"";
}

class CleanStream {
public:
  std::string str() const;
private:
  std::stringstream _stream;
  friend CleanStream& operator<<(CleanStream&, int);
  friend CleanStream& operator<<(CleanStream&, const std::string&);
  friend CleanStream& operator<<(CleanStream&, const char*);

  template<typename T>
  friend CleanStream& operator<<(CleanStream&, T);
};

template<typename T>
CleanStream& operator<<(CleanStream& out, T num) {
  using namespace cleanstream;
  if (std::isnan(num)) {
    out._stream << TEXT_NAN;
  } else if (std::isinf(num)) {
    out._stream << (std::signbit(num) ? TEXT_NINF : TEXT_INF);
  } else if (num == -99) {
    out._stream << TEXT_NINF;
  } else {
    out._stream << num;
  }
  return out;
}

CleanStream& operator<<(CleanStream&, int);


#endif
