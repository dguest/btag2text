Data Files
----------

This directory contains two `.txt.gz` files. Each file is arranged
with one jet per line, with each line being valid JSON. The files are:

 - `signal.txt.gz`: the signal, in this case H -> bb

 - `background.txt.gz`: the background, QCD dijet events

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
   weight 1.0. These weights are an artifact of the Monte Carlo
   techniques we use to simulate the physical processes we're
   studying.

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

The impact parameter is a measure of how much a track "misses" the
interaction point by. We're interested in this quantity because tracks
that miss the interaction point are more likely to come from a
displaced vertex.

 - `d0`: the amount by which the track "misses" the interaction
   point, in the transverse to beam axis direction.
 - `z0`: like `d0` but along the beam axis direction.
 - `d0sig`: significance (displacement over uncertainty) of `d0`
 - `z0sig`: same, for `z0`

##### Track Quality #####

Several parameters describe the track "quality" i.e. how well it was
reconstructed from raw hits in the tracker. The parameters `chi2` and
`ndf` refer to the hit chi squared and the number of degrees of
freedom for the track fit, respectively. The remaining quantities
describe the number of sensors that detected the particle.


#### Raw Cluster Information ####

After particles leave the tracker they form "clusters" in the
calorimeter. There can be any number of these in a given jet, and the
distribution can tell us something about the jet's origin. The
clusters are summarized as follows:

 - `cluster_pt`: energy of the cluster projected along the direction
   transverse to the beam axis, assuming that the energy came from the
   interaction point.
 - `cluster_eta`: cluster [pseudorapidity][eta], relative to the
   interaction point.
 - `cluster_dphi_jet`: azimuthal displacement with respect to the jet
   axis (perpendicular to the `eta` unit vector).
 - `cluster_energy`: energy in the cluster, not projected against
   anything.

#### High-Level Cluster Information ####

The raw cluster information can be combined into various
discriminants: `tau21`, `c1`, `c2`, `c1_beta2`, `c2_beta2`, `d2`, and
`d2_beta2`.

**TODO:** add more descriptions here.

#### High-Level Track Information ####

Flavor-tagging in H->bb "fat" jets is still an open question, and as
such there is no standard track-based discriminant to compare
to. Instead, we form "subjets" (each of which should contain a single
b-quark) and run the standard flavor tagging over each of them.

The first group in each subjet is the subjet kinematics: `jet_pt`,
`jet_eta` and `dphi_fatjet`, which are similar to their track
counterparts, except applied to aggregate of all subjet constituents.

ATLAS uses three standard flavor-taggers, which form the next three
groups for each subjet. These are JetFitter (`jet_jf_*`), SV1
(`jet_sv1_*`) and IP3D (`jet_sv1_*`). These three discriminant are
combined into a BDT, which forms the last group in each subject. Since
the BDT outputs (`jet_mv2c*`) are composed of the other three groups,
they should't be necessary as inputs to a NN, and building a
discriminant on `mv2*` variables will add some undesirable dependencies.

Description Files
-----------------

There are also several `labels` files to describe the outputs. The
file that ends in `.txt` is a (mostly) plain-English description of
the variables we're using, in the same order as the data files.

The `.json` files are valid JSON, and give the shorter names we use
for these variables internally. Ideally any NN trained on this data
will include a list specifying which inputs were used, in which order,
and using the same names as these `.json` files.

### Special Variables ###

There is one 'special' variables included in the description:

 - `...` is used in the description files to indicate that the
   structure it's inside is a variable-length array.

[eta]: https://en.wikipedia.org/wiki/Pseudorapidity
