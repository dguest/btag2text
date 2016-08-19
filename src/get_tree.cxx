#include "get_tree.hh"

#include "TFile.h"
#include <stdexcept>
#include <set>
#include <fstream>

#include "TKey.h"

namespace {
  bool exists(const std::string& file_name) {
    std::ifstream file(file_name.c_str(), std::ios::binary);
    if (!file) {
      file.close();
      return false;
    }
    file.close();
    return true;
  }
}

std::string get_tree(const std::string& file_name) {
  if (!exists(file_name)) {
    throw std::logic_error(file_name + " doesn't exist");
  }
  TFile file(file_name.c_str());
  std::set<std::string> keys;
  int n_keys = file.GetListOfKeys()->GetSize();
  for (int keyn = 0; keyn < n_keys; keyn++) {
    keys.insert(file.GetListOfKeys()->At(0)->GetName());
  }
  size_t n_unique = keys.size();
  if (n_unique > 1) {
    std::string prob = "can't make up tree with " +
      std::to_string(n_unique) + " keys [";
    size_t uniq_n = 0;
    for (const auto& key: keys) {
      prob.append(key);
      uniq_n++;
      if (uniq_n < n_unique) prob.append(", ");
    }
    prob.append("]");
    throw std::logic_error(prob);
  }
  auto* key = dynamic_cast<TKey*>(file.GetListOfKeys()->At(0));
  return key->GetName();
}

