#include "Jets.hh"
#include "JetLabels.hh"
#include "output_templates.hh"

#include <iostream>


int main(int /*argc*/, char** /*argv*/) {
  JetLabels jet;
  std::cout << str_from_all_jet(jet) << std::endl;
  return 0;
}
