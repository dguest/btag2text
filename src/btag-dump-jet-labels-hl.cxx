#include "Jets.hh"
#include "JetLabels.hh"
#include "output_templates.hh"

#include <iostream>


int main(int /*argc*/, char** /*argv*/) {
  JetLabels jet_labels;
  std::cout << str_from_hl_jet(jet_labels) << std::endl;
  return 0;
}
