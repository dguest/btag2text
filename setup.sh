# setup script for HDF5 dumper
echo -n "setting up ATLAS..."
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
. ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh -q
echo " done"

# -- get boost --
echo -n "setting up boost..."
localSetupBoost boost-1.60.0-python2.7-x86_64-slc6-gcc49 -q
# required by makefile
export BOOST_PATH=$ALRB_BOOST_ROOT
echo " done"

# -- get root --
echo -n "setting up root..."
localSetupROOT -q
echo " done"

# -- get HDF5 --
echo -n "setting up HDF5..."
lsetup hdf5 -q
export HDF_PATH=$ALRB_HDF5_PATH
echo " done"

# -- get Eigen3
echo -n "setting up Eigen3..."
localSetupSFT -q releases/LCG_84/eigen/3.2.7
EIGEN_INCLUDE_PATH=$SFT_HOME_eigen/include/eigen3
echo " done"
