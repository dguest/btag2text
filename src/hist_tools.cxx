#include "hist_tools.hh"

std::vector<Axis> range(std::string name,
                        double low, double high,
                        std::string units) {
  return { {name, N_BINS, low, high, units} };
}
std::vector<Axis> zero_to_one(std::string name) {
  return range(name, 0, 1.00001);
}
std::vector<Axis> count(std::string name, int max) {
  return { {name, max + 1, -0.5, max + 0.5} };
}

