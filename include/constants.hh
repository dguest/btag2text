#ifndef CONSTANTS_HH
#define CONSTANTS_HH

// use base units of GeV to make stuff more consistent with Delphes /
// easier to read
static const char* BASE_UNITS = "GeV";
const double MeV = 0.001;
const double GeV = 1000*MeV;
const double TeV = 1000*GeV;

static const char* BASE_LENGTH = "mm";
const double mm = 1.0;
const double cm = 10*mm;

// various other constants
const double D0_THRESHOLD = 1.8; // track counting significance
const double PT_REWEIGHT_MAX = 3000*GeV;
const int PT_REWEIGHT_NBINS = 300;

// used by the macros that define plots
const int N_BINS = 200;
const int N_BINS_FINE = 2000;

// constant IO stuff
static const char* REWEIGHT_FILE = "reweight.h5";
// static const char* JET_COLLECTION = "bTag_AntiKt4EMTopoJets";
static const char* JET_COLLECTION = "bTag_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets";
namespace grp {
  static const char* HIST = "hists";
  static const char* COV = "cov";
  static const char* REWEIGHTED = "reweighted";
  static const char* RAW = "raw";
  static const char* IMAGE = "image";
}
#endif
