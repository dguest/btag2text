#ifndef LWTNN_WRAPPER_HH
#define LWTNN_WRAPPER_HH

#include <string>

struct Jet;

namespace lwt {
  class LightweightNeuralNetwork;
  class NanReplacer;
}

class LwtnnWrapper
{
public:
  LwtnnWrapper(const std::string& nn_file);
  LwtnnWrapper(LwtnnWrapper&) = delete;
  LwtnnWrapper& operator=(LwtnnWrapper&) = delete;

  ~LwtnnWrapper();

  explicit operator bool() const;
  double compute(const Jet&) const;
private:
  lwt::LightweightNeuralNetwork* m_lwtnn;
  lwt::NanReplacer* m_replacer;
  std::string m_output_name;
  size_t m_n_inputs;
};

#endif
