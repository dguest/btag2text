#include "Jets.hh"
#include "JetLabels.hh"

#include <iostream>


int main(int /*argc*/, char** /*argv*/) {
  JetLabels jet;
  std::cout << str_from_ip3d_jet(jet) << std::endl;
  return 0;
}
