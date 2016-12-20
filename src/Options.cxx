#include "Options.hh"
#include <boost/program_options.hpp>
#include <iostream>

std::ostream& operator<<(std::ostream& in, const Options& opt) {
  in << "output: " << opt.output_file << "\n";
  in << "weight: " << opt.weight;
  return in;
}

Options get_opts(int argc, char* argv[],
                        const std::string& description) {
  namespace po = boost::program_options;
  Options opts;
  std::string usage = "usage: " + std::string(argv[0]) + " <files...>"
    + " -o <output> [-h] [opts...]\n";
  po::options_description opt(usage + "\n" + description);
  opt.add_options()
    ("files", po::value(&opts.input_files)->required(),
     "list of input files")
    ("out-file,o", po::value(&opts.output_file)->required(),
     "output file")
    ("help,h", "Print help messages")
    ("weight,w", po::value(&opts.weight)->default_value(1.0),
     "weights for this file")
    ("verbose,v", po::bool_switch(&opts.verbose), "extra output");

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
    std::cerr << usage << "ERROR: " << err.what() << std::endl;
    exit(1);
  }
  return opts;
}

WriterOptions get_writer_opts(int argc, char* argv[],
                              const std::string& description) {
  namespace po = boost::program_options;
  WriterOptions opts;
  std::string usage = "usage: " + std::string(argv[0]) + " <files...>"
    + " -o <output> [-h] [opts...]\n";
  po::options_description opt(usage + "\n" + description);
  opt.add_options()
    ("files", po::value(&opts.input_files)->required(),
     "list of input files")
    ("out-file,o", po::value(&opts.output_file)->required(),
     "output file")
    ("help,h", "Print help messages")
    ("weight,w", po::value(&opts.weight)->default_value(1.0),
     "weights for this file")
    ("verbose,v", po::bool_switch(&opts.verbose), "extra output")
    ("chunk-size,s", po::value(&opts.chunk_size)->default_value(256),
      "size of chunks to write to HDF5")
    ("track-size,t", po::value(&opts.track_size)->default_value(0),
     "Number of tracks to write out (no container if zero)")
    ("cluster-size,c", po::value(&opts.cluster_size)->default_value(0),
     "Number of clusters to write out (no container if zero)");


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
    std::cerr << usage << "ERROR: " << err.what() << std::endl;
    exit(1);
  }
  return opts;
}


StreamOptions get_stream_opts(int argc, char* argv[],
                              const std::string& description) {
  namespace po = boost::program_options;
  StreamOptions opts;
  po::options_description opt(description);
  opt.add_options()
    ("files", po::value(&opts.input_files)->required(),
     "list of input files")
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
