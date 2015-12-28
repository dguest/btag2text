#include "Jets.hh"
#include "JetLabels.hh"

#include <iostream>

class TrkUnitLabels
{
public:
  OutTrack<TrackLabels> track;
  OutJfVertex<JfVertexLabels> jf;
};


int main(int /*argc*/, char** /*argv*/) {
  OutJet<JetLabels> labels;
  std::cout << labels << std::endl;

  OutUnit<TrkUnitLabels> trk_labels;
  std::cout << trk_labels << std::endl;
  return 0;
}
