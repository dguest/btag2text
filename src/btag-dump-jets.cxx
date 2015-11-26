#include "FileCLI.hh"

#include "TChain.h"

#include <ostream>
#include <iostream>

const double GeV = 1000;
const double MAX_PT = 1000*GeV;

// ______________________________________________________________________
// define classes
class Jets
{
public:
  Jets(TChain& chain);
  void select(int);
  int number() const;
private:
  friend std::ostream& operator<<(std::ostream&, Jets&);
};

std::ostream& operator<<(std::ostream&, Jets&);

// _____________________________________________________________________
// class implementation

Jets::Jets(TChain& chain) {

}

// _____________________________________________________________________
// main function

int main(int argc, char* argv[]) {
  FileCLI cli(argc, argv);

  TChain chain("bTag_AntiKt4EMTopoJets");
  for (const auto& in: cli.in_files()) {
    chain.Add(in.c_str());
  }
  Jets jets(chain);
}
