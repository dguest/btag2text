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
std::ostream& operator<<(std::ostream&, const Options&);
struct StreamOptions
{
  std::vector<std::string> input_files;
  double weight;
};

Options get_opts(int argc, char* argv[], const std::string& description);
StreamOptions get_stream_opts(int argc, char* argv[],
                              const std::string& description);

#endif
