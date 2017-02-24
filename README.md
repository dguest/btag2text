Preprocessor For Flavor Tagging Data
====================================

This is a collection of code to process flavor tagging ROOT ntuples into inputs for machine learning studies.

Installing
----------

This code depends on:

 - ROOT6
 - Boost (for ProgramOptions)
 - HDF5

To install, after you've cloned this repository, you should be be able
to simply type `make`. The executable binaries should be built in
`bin`.

Specific uses
-------------

Some specific use cases are documented on github wikis. For example
see [running for Hbb tagging][1] and [running for single btagging][2].

[1]: https://github.com/dguest/btag2text/wiki/Running-for-H--bb-Tagging
[2]: https://github.com/dguest/btag2text/wiki/Running-for-single-btagging

Code Organization
-----------------

Source code for all the programs is in `src/`, while the corresponding
header files are stored in `include/`. The top level files all have
the prefix `btag-` and the same name (minus the `.cxx`) as the
executable they produce. Some important files include:

 - `Jets.{cxx,hh}`: Code that builds jet objects from root data
 - `include/output_templates.hh`: functions used to dump jet
   information. These are templated so that they can be used in two
   ways. When called on jets, they will dump the jet information. When
   called on "label" objects they will dump the labels (in the same
   format as the jet information).
 - `JetLabels.hh`: Structures with the same fields as the objects in
   `Jets`, used by the info dumping functions to print labels.
