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

SHELL        := bash

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
# minimalist version
WRITE_PREFIX := btag-write-THIS_TO_DISABLE
ALL_WRITERS  := $(filter $(OUTPUT)/$(WRITE_PREFIX)%,$(ALL_EXE_PATHS))
WRITE_OBJ    := Options Jets SmartChain
WRITE_OBJ    += select_jet math get_tree unshittify
WRITE_OBJ    += HDF5Writer hdf5_object_builders
WRITE_OBJ_PATHS := $(filter $(WRITE_OBJ:%=$(BUILD)/%.o),$(GEN_OBJ_PATHS))
WRITE_OBJ_PATHS += $(TDICT_PATHS)

# _______________________________________________________________
# Add Libraries

# --- load in root config
ROOTCFLAGS    := $(shell root-config --cflags)
USED_ROOT     := Core RIO Tree Physics
ROOTLIBS      := -L$(shell root-config --libdir) $(USED_ROOT:%=-l%)
ROOTLIBS      += -Wl,-rpath,$(shell root-config --libdir)
GCC_PATH      := $(shell which $(CXX))
GCC_LIB_PATH  := $(dir $(GCC_PATH))../lib64
ROOTLIBS      += -Wl,-rpath,$(GCC_LIB_PATH)

ROOTLDFLAGS   := $(shell root-config --ldflags)

CXXFLAGS     += $(ROOTCFLAGS)
LDFLAGS      += $(ROOTLDFLAGS)
LIBS         += $(ROOTLIBS)

# --- add HDF5
HDF_INFO := $(shell h5c++ -showconfig | grep 'Installation point:')
HDF_PATH ?= $(strip $(shell echo $(HDF_INFO) | cut -d ':' -f 2 ))
ifndef HDF_PATH
$(error "couldn't find HDF, quitting")
endif

CXXFLAGS     += -I$(HDF_PATH)/include
LIBS         += -L$(HDF_PATH)/lib -Wl,-rpath,$(HDF_PATH)/lib
LIBS         += -lhdf5_cpp -lhdf5

# --- add boost
ifdef BOOST_PATH
CXXFLAGS     += -I$(BOOST_PATH)/include
LIBS         += -L$(BOOST_PATH)/lib -Wl,-rpath,$(BOOST_PATH)/lib
endif
LIBS         += -lboost_program_options

# --- writer-only LIBS version with fewer dependencies
WRITER_LIBS  := $(LIBS)

# --- add ndhist
NDHIST       := ndhist/lib/libndhist.so
LIBS         += $(shell $(CURDIR)/ndhist/bin/ndhist-config --libs)
CXXFLAGS     += $(shell $(CURDIR)/ndhist/bin/ndhist-config --cflags)

# --- add eigen
ifdef EIGEN_INCLUDE_PATH
CXXFLAGS     += -I$(EIGEN_INCLUDE_PATH)
else
CXXFLAGS     += $(shell pkg-config eigen3 --cflags)
endif

# --- first call here
cpp: $(ALL_TOP_LEVEL)
all: $(ALL_TOP_LEVEL) plotting
writer: $(ALL_WRITERS)

all-top-level: $(ALL_TOP_LEVEL)

$(NDHIST):
	@echo " -- building ndhist -- "
	@$(MAKE) -C ndhist
	@echo " -- done building ndhist -- "

plotting:
	@echo "installing plotting scripts"
	@./ndhist-python/ndhist-install-python install

.PHONY: plotting all cpp writer

# _______________________________________________________________
# Add Build Rules

# build writer executables (fewer things to include)
WRITE_OBJ_PFX := $(BUILD)/$(WRITE_PREFIX)
$(OUTPUT)/$(WRITE_PREFIX)%: $(WRITE_OBJ_PATHS) $(WRITE_OBJ_PFX)%.o
	@mkdir -p $(OUTPUT)
	@echo "linking writer $^ --> $@"
	@$(CXX) -o $@ $^ $(WRITER_LIBS) $(LDFLAGS)
	@cp -f $(DICT)/*.pcm $(OUTPUT)

# build general executables
EXEC_OBJ_PFX := $(BUILD)/$(EXE_PREFIX)
$(OUTPUT)/$(EXE_PREFIX)%: $(GEN_OBJ_PATHS) $(EXEC_OBJ_PFX)%.o $(NDHIST)
	@mkdir -p $(OUTPUT)
	@echo "linking $^ --> $@"
	@$(CXX) -o $@ $(filter-out $(NDHIST),$^) $(LIBS) $(LDFLAGS)
	@cp -f $(DICT)/*.pcm $(OUTPUT)


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
.PHONY : clean rmdep
CLEANLIST     = *~ *.o *.o~ *.d core
clean:
	rm -fr $(CLEANLIST) $(CLEANLIST:%=$(BUILD)/%) $(CLEANLIST:%=$(DEP)/%)
	rm -fr $(BUILD) $(DICT) $(OUTPUT)
	make -C ndhist clean
	./ndhist-python/ndhist-install-python remove

rmdep:
	rm -f $(DEP)/*.d
