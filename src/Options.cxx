#include "Options.hh"
#include <boost/program_options.hpp>
#include <iostream>

std::ostream& operator<<(std::ostream& in, const Options& opt) {
  in << "output: " << opt.output_file << "\n";
  in << "weight: " << opt.weight;
  return in;
}

Options::Options():
  chunk_size(0), track_size(0), cluster_size(0),
  max_weight(-INFINITY)
{}

Options get_opts(int argc, char* argv[],
                 const std::string& description,
                 unsigned opt_flags) {
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

  if (opt_flags & opt::reweight_file) {
    opt.add_options()("reweight-file,r", po::value(&opts.rw_file),
                      "file to reweight this distribution by pt");
  }
  if (opt_flags & opt::max_weight) {
    opt.add_options()(
      "max-weight,m", po::value(&opts.max_weight)->default_value(INFINITY),
      "remove events with higher weights");
  }
  if (opt_flags & opt::writer) {
    opt.add_options()
      ("chunk-size,s", po::value(&opts.chunk_size)->default_value(256),
       "size of chunks to write to HDF5")
      ("track-size,t", po::value(&opts.track_size)->default_value(0),
       "Number of tracks to write out (no container if zero)")
      ("cluster-size,c", po::value(&opts.cluster_size)->default_value(0),
       "Number of clusters to write out (no container if zero)");
  }
  if (opt_flags & opt::lwtnn) {
    opt.add_options()
      ("network-file,n", po::value(&opts.network_file),
        "lwtnn file to read in");
  }

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

Options get_writer_opts(int argc, char* argv[],
                        const std::string& description,
                        unsigned opt_flags) {
  return get_opts(argc, argv, description, opt_flags | opt::writer);
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
