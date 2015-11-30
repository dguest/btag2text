#ifndef SMART_CHAIN_H
#define SMART_CHAIN_H

#include "TChain.h"
#include <string>
#include <vector>
#include <set>
#include <stdexcept>

namespace chain {
  enum MissingBranchAction {THROW, NULL_POINTER, NULL_NO_RECORD};
}

class SmartChain: public TChain {
public:
  using TChain::Add;
  SmartChain(std::string tree_name);
  virtual int add(std::string file_name, long long nentries = -1);
  virtual int GetEntry(long long int entry_n, int getall = 0);
  template<typename T, typename Z>
  void SetBranch(T name, Z branch,
		 chain::MissingBranchAction = chain::THROW);
  void fake_set(const std::string&);
  std::vector<std::string> get_all_branch_names() const;
  std::string get_current_file() const;
private:
  typedef std::vector<std::string> Strings;
  void SetBranchAddressPrivate(std::string name, void* branch,
			       chain::MissingBranchAction action);
  void throw_bad_branch(std::string name) const;
  std::string get_files_string() const;
  void check_for_dup(const std::string& branch_name) const;
  std::string m_tree_name;
  Strings m_set_branches;
  std::set<std::string> m_set_branch_set;
  std::set<std::string> m_fake_branches;
  Strings m_files;
  int m_last_tree; 		// to trigger branch check
};

class MissingBranchError: public std::runtime_error
{
public:
  MissingBranchError(const std::string& what_arg);
};

// -------- implementation -----


template<typename T, typename Z>
void SmartChain::SetBranch(T name, Z branch,
			   chain::MissingBranchAction action) {
  *branch = 0;
  SetBranchAddressPrivate(name, branch, action);
}


#endif //SMART_CHAIN_H
