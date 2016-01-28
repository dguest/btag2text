#ifndef HIST_TOOLS_HH
#define HIST_TOOLS_HH

#include "constants.hh"

#include "ndhist/Axis.hh"

#include <vector>

std::vector<Axis> range(std::string name,
                        double low, double high,
                        std::string units = "");
std::vector<Axis> zero_to_one(std::string name);
std::vector<Axis> count(std::string name, int max);

#define ZERO_ONE(name) name( zero_to_one( #name) )
#define COUNT(name, max) name( count( #name, max) )
#define RANGE(name, low, high) name( range( #name, low, high) )
#define LENGTH(name, low, high) name( range( #name, low, high, BASE_LENGTH) )
#define ENERGY(name, high) name( range( #name, 0, high, BASE_UNITS) )

#endif
