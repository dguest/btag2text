#ifndef OPTIONS_HH
#define OPTIONS_HH

#include <vector>
#include <string>

struct Options
{
  std::vector<std::string> input_files;
  std::string output_file;
  double weight;
};

Options get_opts(int argc, char* argv[], const std::string& description);

#endif
