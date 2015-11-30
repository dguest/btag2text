#include "SmartChain.hh"
#include "TChain.h"
#include "TFile.h"
#include "TError.h"

#include <cassert>

SmartChain::SmartChain(std::string tree_name):
  TChain(tree_name.c_str()),
  m_tree_name(tree_name),
  m_last_tree(-1)
{
}

int SmartChain::add(std::string file_name, long long nentries) {
  m_files.push_back(file_name);

  TFile file(file_name.c_str());

  if (!file.IsOpen() || file.IsZombie()) {
    throw std::runtime_error("bad file: " + file_name);
  }
  TTree* tree = dynamic_cast<TTree*>(file.Get(m_tree_name.c_str()));
  if (!tree || (tree->GetEntries() == 0)) {
    return 0;
  }
  return TChain::Add(file_name.c_str(), nentries);
}
int SmartChain::GetEntry(long long int entry_n, int getall) {
  // have to turn off errors even though I explicetly told root I don't
  // want them. These fucking designers are retarded.
  int old_error_level = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kFatal;
  int return_val = TChain::GetEntry(entry_n, getall);
  gErrorIgnoreLevel = old_error_level;
  int this_tree_n = GetTreeNumber();
  if (this_tree_n != m_last_tree) {
    for (auto br_itr: m_set_branches) {
      if (m_fake_branches.count(br_itr)) continue;
      if (!GetBranch(br_itr.c_str())) {
	throw_bad_branch(br_itr);
      }
    }
    m_last_tree = this_tree_n;
  }
  return return_val;
}

std::vector<std::string> SmartChain::get_all_branch_names() const {
  return m_set_branches;
}
std::string SmartChain::get_current_file() const {
  assert(m_last_tree < int(m_files.size()));
  return m_files.at(m_last_tree);
}

void SmartChain::SetBranchAddressPrivate(std::string name, void* branch,
					 chain::MissingBranchAction action) {
  check_for_dup(name);
  m_set_branch_set.insert(name);
  m_set_branches.push_back(name);

  unsigned branches_found = 0;
  SetBranchStatus(name.c_str(), 1, &branches_found);
  if (branches_found != 1) {
    switch (action) {
    case chain::NULL_POINTER: {
      SetBranchStatus(name.c_str(), 0, &branches_found);
      m_fake_branches.insert(name);
      return;
    }
    case chain::NULL_NO_RECORD: {
      SetBranchStatus(name.c_str(), 0, &branches_found);
      m_set_branches.pop_back();
      m_set_branch_set.erase(name);
      return;
    }
    case chain::THROW: {
      // mark this branch as "fake" (don't set in the future)
      m_fake_branches.insert(name);
      throw_bad_branch(name);
    }
    default: throw std::logic_error("unknown action in " __FILE__);
    }
  }

  // other checks (not sure if they are needed, who knows what root does)
  int return_code = TChain::SetBranchAddress(name.c_str(), branch);
  if (return_code != 0 && return_code != 5 ){
    std::string issue = "can not set " + name + " , return code " +
      std::to_string(return_code);
    throw std::runtime_error(issue);
  }
  if (!GetBranch(name.c_str())) {
    throw_bad_branch(name);
  }

}

void SmartChain::fake_set(const std::string& name){
  check_for_dup(name);
  m_set_branch_set.insert(name);
  m_set_branches.push_back(name);
  m_fake_branches.insert(name);
}

// ================ private functions ====================

void SmartChain::throw_bad_branch(std::string name) const {
  std::string issue = "can't find branch " + name;
  std::string file = GetFile()->GetName();
  issue.append(" bad file: " + file);
  throw MissingBranchError(issue);

}

std::string SmartChain::get_files_string() const {
  std::string issue = " files: ";
  for (Strings::const_iterator itr = m_files.begin();
       itr != m_files.end(); itr++) {
    issue.append(*itr);
    if (*itr != *m_files.rbegin()) issue.append(", ");
  }
  return issue;
}

void SmartChain::check_for_dup(const std::string& name) const {
  if (m_set_branch_set.count(name)) {
    std::string err = "setting branch address: " + name + " is already"
      " set in " + std::string(GetName());
    throw std::runtime_error(err);
  }
}


MissingBranchError::MissingBranchError(const std::string& what_arg):
  std::runtime_error(what_arg)
{
}
