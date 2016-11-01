Example Text File Dumper for H->bb
==================================

This is a minimal set of code to dump b-tagging ntuples which are
stored in ROOT into a text file. I've taken the liberty of removing a
lot of extra programs (which may be important later) so that this is
as simple as possible.

Installing
----------

This code depends on:

 - ROOT6
 - Boost (for ProgramOptions)

To install, after you've cloned this repository, you should be be able
to simply type `make`. The executable binaries should be built in
`bin`.

Running
-------

You'll need some ntuples that describe either signal or background to
get started. Once you have them, you can run

```sh
./bin/btag-dump-jets-fatjets {path-to-data-file} | head -n 10
```

where `{path-to-data-file}` points to a data file. This will print out
the first 10 jets from the file as text. Each line should be valid
json, so it can be parsed one line at a time with a standard json
parser.

In practice it sometimes makes more sense to store this in a gzipped
text file:

```sh
./bin/btag-dump-jets-fatjets {path-to-data-file} | gzip - > output_file.txt.gz
```

Note that most executable scripts also have help, try running them
with the `-h` option.

To get a key for the variables, run

```sh
./bin/btag-dump-jet-labels-fatjets-terse
```

For more explanation as to what these variables are, see
`docs/FATJETS.md`.

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
