#include "Jets.hh"
#include "JetLabels.hh"

#include <iostream>

int main(int /*argc*/, char** /*argv*/) {
  OutJet<JetLabels> labels;
  std::cout << labels << std::endl;
  return 0;
}
