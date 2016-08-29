#include "Jets.hh"
#include "JetLabels.hh"
#include "output_templates.hh"

#include <iostream>


int main(int /*argc*/, char** /*argv*/) {
  TerseJetLabels jet(1);
  std::cout << str_from_fat_jet(jet, "\"weight\"") << std::endl;
  return 0;
}
