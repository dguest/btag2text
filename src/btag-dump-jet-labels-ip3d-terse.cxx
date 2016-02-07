#include "Jets.hh"
#include "JetLabels.hh"
#include "output_templates.hh"

#include <iostream>


int main(int /*argc*/, char** /*argv*/) {
  TerseJetLabels jet;
  std::cout << str_from_ip3d_jet(jet, "\"weight\"") << std::endl;
  return 0;
}
