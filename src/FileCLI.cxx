#include "FileCLI.hh"

#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>


bool exists(const std::string& file_name) {
  std::ifstream file(file_name.c_str(), std::ios::binary);
  if (!file) {
    file.close();
    return false;
  }
  file.close();
  return true;
}

std::string red(const std::string& st) {
  return "\033[31;1m" + st + "\033[m";
}



FileCLI::FileCLI(int nargs, char* argc[], Output output, std::string help):
  m_nargs(nargs),
  m_name(argc[0]),
  m_overwrite(false),
  m_output_mode(output),
  m_help(help)
{
  for (int nn = 0; nn < nargs; nn++) {
    m_args.push_back(argc[nn]);
  }

  int argn = 1;
  while (argn < nargs) {
    argn += check_opts(argn);
  }

  // error checking
  if (!m_overwrite && exists(m_output)){
    throw std::runtime_error("can't overwrite " + m_output);
  }
  if (m_output.size() == 0 && output == Output::YES) {
    throw std::runtime_error("no output given");
  }
  if (m_files.size() == 0) throw std::runtime_error("no inputs given");
}

void FileCLI::usage() const
{
  std::string output_string;
  switch (m_output_mode) {
  case Output::YES: output_string = " -o <output>"; break;
  case Output::NO: output_string = ""; break;
  case Output::OPTIONAL: output_string = " [-o <output>]"; break;
  }
  std::cerr << "usage: " << m_name << " [options] <inputs>..."
            << output_string << "\n";
  if (m_help.size() > 0) std::cerr << "\n" << m_help << "\n";

  // help options
  std::cerr << "\n"
            << "options:\n"
            << " -h, --help: print this help\n"
            << " -f: overwrite output\n"
    ;
}

int FileCLI::check_opts(int argn) {
  std::string arg = m_args.at(argn);
  bool compound = arg.size() > 1 && (arg.at(0) == '-') && (arg.at(1) != '-');
  bool is_file = true;
  if ( (arg == "--help") || (compound && strchr(arg.c_str(), 'h') ) ) {
    usage();
    exit(1);
  }
  // parse options
  if (compound) {
    if (strchr(arg.c_str(), 'f')) {
      m_overwrite = true;
      is_file = false;
    }
  }
  // parse outputs
  if (compound && strchr(arg.c_str(), 'o')) {
    if (m_output_mode == Output::NO) {
      throw std::runtime_error("-o isn't used");
    }
    if (! (argn + 1 < m_nargs)) {
      throw std::runtime_error("no argument for -o given");
    }
    m_output = m_args.at(argn + 1);
    return 2;
  }
  // fill input files...
  if (!is_file) return 1;
  std::string file = m_args.at(argn);
  if (!exists(file)) throw std::runtime_error(file + " doesn't exist");
  m_files.push_back(file);
  return 1;
}

std::vector<std::string> FileCLI::in_files() const {
  return m_files;
}
std::string FileCLI::out_file() const {
  return m_output;
}
