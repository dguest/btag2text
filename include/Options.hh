#ifndef OPTIONS_HH
#define OPTIONS_HH

#include <vector>
#include <string>
#include <ostream>

struct Options
{
  std::vector<std::string> input_files;
  std::string output_file;
  double weight;
  bool verbose;
};

struct WriterOptions
{
  std::vector<std::string> input_files;
  std::string output_file;
  double weight;
  bool verbose;
  size_t chunk_size;
  size_t track_size;
  size_t cluster_size;
};
std::ostream& operator<<(std::ostream&, const Options&);
struct StreamOptions
{
  std::vector<std::string> input_files;
  double weight;
};

Options get_opts(int argc, char* argv[], const std::string& description);
WriterOptions get_writer_opts(int argc, char* argv[],
                              const std::string& description);
StreamOptions get_stream_opts(int argc, char* argv[],
                              const std::string& description);

#endif
