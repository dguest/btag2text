#include "Options.hh"
#include <boost/program_options.hpp>
#include <iostream>

Options get_opts(int argc, char* argv[], const std::string& description) {
  namespace po = boost::program_options;
  Options opts;
  po::options_description opt(description);
  opt.add_options()
    ("files", po::value(&opts.input_files)->required(),
     "list of input files")
    ("out_file,o", po::value(&opts.output_file)->required(),
     "output file")
    ("help,h", "Print help messages")
    ("weight,w", po::value(&opts.weight)->default_value(1.0),
     "weights for this file");
  po::positional_options_description pos_opts;
  pos_opts.add("files", -1);

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv).options(opt)
            .positional(pos_opts).run(), vm);
    if ( vm.count("help") ) {
      std::cout << opt << std::endl;
      exit(1);
    }
    po::notify(vm);
  } catch (po::error& err) {
    std::cerr << opt << "\nERROR: " << err.what() << std::endl;
    exit(1);
  }
  return opts;
}
