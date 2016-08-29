Data Files
----------

This directory contains two `.txt.gz` files. Each file is arranged
with one jet per line, with each line being valid JSON. The files are:

 - signal.txt.gz: the signal, in this case H -> bb

 - background.txt.gz: the background, QCD dijet events

### Jet Description ###

Each jet is arranged into jet kinematics, raw track information, raw
cluster information, high-level cluster information, and high-level
track information.

#### Jet Kinematics ####

Several Variables describe each jet:

 - `jet_pt`: jet momentum transverse to the beam line
 - `jet_eta`: "[pseudorapidity][eta]": something like a momentum along
   the beam line
 - `weight`: the weight the jet should be given in training. Two jets
   with weight 0.5 should be given the same priority as one with
   weight 1.0.

#### Raw Track Information ####

The raw track information is similar to the information describing the
jet, but with some low-level tracking information mixed in. There are
a variable number of tracks in each jet, and the information for each
is organized in terms of kinematics, impact parameter, and track
quality.

##### Kinematics #####

Tracks have `pt` and `eta` like jets. They also have `dphi_jet`, which
indicates the azimuthal displacement with respect to the jet axis
(perpendicular to the `eta` unit vector).

##### Impact Parameter #####

Blah

Description Files
-----------------

There are also several files to describe the outputs. The file that
ends in `.txt` is a (mostly) plain-English description of the
variables we're using, in the same order as the data files.

The `.json` files are valid JSON, and give the shorter names we use
for these variables internally. Ideally any NN trained on this data
will include a list specifying which inputs were used, in which order,
and using the same names as these `.json` files.

### Special Variables ###

There are three 'special' variables included in the description:

 - "..." is used in the description files to indicate that the
   structure it's inside is a variable-length array.

 - "jet_truthflav" is the target 'flavor'. It should take the values 0
   (light-jet), 4 (charm-jet), 5 (b-jet), and 15 (tau-jet).

 - "weight" is the jet weighting. This should be used to remove any
   flavor bias the pt and eta spectra may have. Two jets with weight
   0.5 should be given equal priority as one jet with weight 1.0.

[eta]: https://en.wikipedia.org/wiki/Pseudorapidity
