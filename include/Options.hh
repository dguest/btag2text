#ifndef OPTIONS_HH
#define OPTIONS_HH

#include <vector>
#include <string>
#include <ostream>

namespace opt {
  const unsigned reweight_file = 0x1 << 0;
  const unsigned max_weight = 0x1 << 1;
  const unsigned writer = 0x1 << 2;
}

struct Options
{
  std::vector<std::string> input_files;
  std::string output_file;
  double weight;
  bool verbose;
  size_t chunk_size;
  size_t track_size;
  size_t cluster_size;
  std::string rw_file;
  double max_weight;
  Options();
};

std::ostream& operator<<(std::ostream&, const Options&);
struct StreamOptions
{
  std::vector<std::string> input_files;
  double weight;
};

Options get_opts(int argc, char* argv[],
                 const std::string& description,
                 unsigned flags = 0);
Options get_writer_opts(int argc, char* argv[],
                        const std::string& description,
                        unsigned flags = 0);
StreamOptions get_stream_opts(int argc, char* argv[],
                              const std::string& description);

#endif
