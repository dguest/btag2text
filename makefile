# makefile for generic c++ project
# generated with `makeproject` on Mon Nov 23 11:33:16 CET 2015
# Author: Dan Guest <dguest@cern.ch>

# _______________________________________________________________
# Basic Setup

# --- set dirs
BUILD        := build
SRC          := src
INC          := include
DICT         := dict
OUTPUT       := bin
LIB          := lib

#  set search path
vpath %.cxx  $(SRC)
vpath %.hh   $(INC)
vpath %.h    $(INC)
vpath %Dict.h $(DICT)
vpath %Dict.cxx $(DICT)

# --- set compiler and flags (roll c options and include paths together)
CXX          ?= g++
CXXFLAGS     := -O2 -Wall -fPIC -I$(INC) -g -std=c++11
CXXFLAGS     += -Wno-unused-variable
LIBS         := # blank, more will be added below
LDFLAGS      := # blank, more will be added below

# ---- define objects from files in the SRC directory
GEN_OBJ_SRC   := $(wildcard $(SRC)/*.cxx)
GEN_OBJ       := $(notdir $(GEN_OBJ_SRC:%.cxx=%.o))

# this list may be manipulated in other segments further down
GEN_OBJ_PATHS := $(GEN_OBJ:%=$(BUILD)/%)

# --- all top level (added further down)
ALL_TOP_LEVEL := # blank, more will be added below

# _______________________________________________________________
# Add Top Level Objects

# --- stuff used for the c++ executable
# everything with this prefix will be built as an executable
EXE_PREFIX   := btag-

ALL_EXE_SRC   := $(wildcard $(SRC)/$(EXE_PREFIX)*.cxx)
ALL_EXE       := $(notdir $(ALL_EXE_SRC:%.cxx=%))
ALL_EXE_PATHS := $(ALL_EXE:%=$(OUTPUT)/%)

# filter out the general objects
GEN_OBJ_PATHS := $(filter-out $(BUILD)/$(EXE_PREFIX)%.o,$(GEN_OBJ_PATHS))

# add to all top level
ALL_TOP_LEVEL += $(ALL_EXE_PATHS)

# _______________________________________________________________
# ROOT dicts
DICT_FILES     := $(INC)/Stl.h
TDICTS         := $(notdir $(DICT_FILES:.h=Dict.o))
TDICT_PATHS    := $(TDICTS:%=$(BUILD)/%)
# ALL_TOP_LEVEL  += $(TDICT_PATHS)
GEN_OBJ_PATHS  += $(TDICT_PATHS)
.SECONDARY: $(TDICT_PATHS)

# _______________________________________________________________
# Add Libraries

# --- load in root config
ROOTCFLAGS    := $(shell root-config --cflags)
ROOTLIBS      := $(shell root-config --libs)
# ROOTLIBS      += -lCore -lTree -lRIO
ROOTLDFLAGS   := $(shell root-config --ldflags)

CXXFLAGS     += $(ROOTCFLAGS)
LDFLAGS      += $(ROOTLDFLAGS)
LIBS         += $(ROOTLIBS)

# --- add HDF5
HDF_INFO := $(shell h5c++ -showconfig | grep 'Installation point:')
HDF_PATH := $(strip $(shell echo $(HDF_INFO) | cut -d ':' -f 2 ))
ifndef HDF_PATH
$(error "couldn't find HDF, quitting")
endif

CXXFLAGS     += -I$(HDF_PATH)/include
LIBS         += -L$(HDF_PATH)/lib -Wl,-rpath,$(HDF_PATH)/lib
LIBS         += -lhdf5_cpp -lhdf5

LIBS         += $(shell ndhist-config --libs)
CXXFLAGS     += $(shell ndhist-config --cflags)

LIBS         += $(shell covol-config --libs)
CXXFLAGS     += $(shell covol-config --cflags)
CXXFLAGS     += $(shell pkg-config eigen3 --cflags)

# boost
ifdef BOOST_PATH
LIBS         += -L$(BOOST_PATH)/lib -Wl,-rpath,$(BOOST_PATH)/lib
endif
LIBS         += -lboost_program_options

# --- first call here
all: $(ALL_TOP_LEVEL)

# _______________________________________________________________
# Add Build Rules

# build exe
$(OUTPUT)/$(EXE_PREFIX)%: $(GEN_OBJ_PATHS) $(BUILD)/$(EXE_PREFIX)%.o
	@mkdir -p $(OUTPUT)
	@echo "linking $^ --> $@"
	@$(CXX) -o $@ $^ $(LIBS) $(LDFLAGS)
	@cp $(DICT)/*.pcm $(OUTPUT)


# root dictionary generation
LINKDEF := $(INC)/LinkDef.h
$(DICT)/%Dict.cxx: %.h $(LINKDEF)
	@echo making dict $@
	@mkdir -p $(DICT)
	@rootcint -f $@ -c -I$(INC) $(INC)/$*.h $(LINKDEF)

$(BUILD)/%Dict.o: $(DICT)/%Dict.cxx
	@mkdir -p $(BUILD)
	@echo compiling dict $@
	@$(CXX) -I. $(CXXFLAGS) $(ROOTCFLAGS) -c $< -o $@

# compile rule
$(BUILD)/%.o: %.cxx
	@echo compiling $<
	@mkdir -p $(BUILD)
	@$(CXX) -c $(CXXFLAGS) $< -o $@


# use auto dependency generation
ALLOBJ       := $(GEN_OBJ)
DEP          := $(BUILD)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),rmdep)
include  $(ALLOBJ:%.o=$(DEP)/%.d)
endif
endif

DEPTARGSTR = -MT $(BUILD)/$*.o -MT $(DEP)/$*.d
$(DEP)/%.d: %.cxx
	@echo making dependencies for $<
	@mkdir -p $(DEP)
	@$(CXX) -MM -MP $(DEPTARGSTR) $(CXXFLAGS) $(PY_FLAGS) $< -o $@

# clean
.PHONY : clean rmdep all
CLEANLIST     = *~ *.o *.o~ *.d core
clean:
	rm -fr $(CLEANLIST) $(CLEANLIST:%=$(BUILD)/%) $(CLEANLIST:%=$(DEP)/%)
	rm -fr $(BUILD) $(DICT) $(OUTPUT)

rmdep:
	rm -f $(DEP)/*.d
