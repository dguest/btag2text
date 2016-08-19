#include <boost/program_options.hpp>
#include "Jets.hh"
#include "SmartChain.hh"
// #include "FlavorPtEtaDistributions.hh"
#include "constants.hh"
#include "hist_tools.hh"
#include "select_jet.hh"
#include "unshittify.hh"
#include "get_tree.hh"

#include "ndhist/Histogram.hh"

#include "H5Cpp.h"
#include "TROOT.h"

#include <iostream>

const std::string description = "build distributions for fat jets";

// various plotting constants
const double MAX_VX_MASS = 10*GeV;
const double MV2_HIGH = 1.00001;
const double MV2_LOW = -MV2_HIGH;
const double PT_MAX = 2*TeV;
const double ETA_MAX = 3.0;

class JetHists
{
public:
  JetHists();
  void fill(const Jet& jet, double weight = 1);
  void save(H5::CommonFG& out) const;
  void save(H5::CommonFG& out, std::string subdir) const;
private:
  Histogram pt;
  Histogram eta;
};

// _____________________________________________________________________
// main function

int main(int argc, char* argv[]) {
  unshittify();
  // command parsing
  namespace po = boost::program_options;
  std::string out_name = "";
  po::options_description opt(description);
  opt.add_options()
    ("help,h", "Print help messages")
    ("out_file,o", po::value<std::string>(&out_name)->required(),
     "output file")
    ("weight,w", po::value<double>()->default_value(1.0),
     "weights for this file");
  po::positional_options_description pos_opts;
  pos_opts.add("files", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(opt)
            .positional(pos_opts).run(), vm);
  if ( vm.count("help") ) {
    std::cout << opt << std::endl;
    return 0;
  }
  try {
    po::notify(vm);
  } catch (po::error& err) {
    std::cerr << opt << "\nERROR: " << err.what() << std::endl;
    return 1;
  }
  auto files = vm["files"].as<std::vector<std::string> >();
  if (files.size() == 0) {
    std::cerr << "ERROR: need files" << std::endl;
    return 1;
  }

  // running
  SmartChain chain(get_tree(files.at(0)));
  for (const auto& in: files) {
    chain.add(in);
  }
  Jets jets(chain);
  int n_entries = chain.GetEntries();
  std::cout << n_entries << " entries in chain" << std::endl;

  JetHists hists;
  double weight = vm["weight"].as<double>();
  for (int iii = 0; iii < n_entries; iii++) {
    chain.GetEntry(iii);
    int n_jets = jets.size();
    for (int jjj = 0; jjj < n_jets; jjj++) {
      auto jet = jets.getJet(jjj);
      if (! select_jet(jet) ) continue;
      hists.fill(jet, weight);
    }
  }

  // save histograms
  using namespace grp;
  H5::H5File out_file(out_name, H5F_ACC_TRUNC);
  // hists
  hists.save(out_file, HIST);
}

// ______________________________________________________________________
// hist methods

JetHists::JetHists():
  // MV2
  ENERGY(pt, PT_MAX),
  RANGE(eta, -ETA_MAX, ETA_MAX)
{
}

void JetHists::fill(const Jet& jet, double weight) {
#define BYNAME(name) name.fill(jet.jet_ ## name, weight)
  BYNAME(pt);
  BYNAME(eta);
#undef BYNAME
}

void JetHists::save(H5::CommonFG& out) const {
#define BYNAME(name) name.write_to(out, #name)
  BYNAME(pt);
  BYNAME(eta);
#undef BYNAME
}
void JetHists::save(H5::CommonFG& out, std::string subdir) const {
  H5::Group group(out.createGroup(subdir));
  save(group);
}

