#include "get_tree.hh"

#include "TFile.h"
#include <stdexcept>
#include "TKey.h"

std::string get_tree(const std::string& file_name) {
  TFile file(file_name.c_str());
  int n_keys = file.GetListOfKeys()->GetSize();
  if (n_keys != 1) {
    throw std::logic_error("can't make up tree with " +
                           std::to_string(n_keys) + " keys");
  }
  auto* key = dynamic_cast<TKey*>(file.GetListOfKeys()->At(0));
  return key->GetName();
}

