# Macros

This is a self-contained macro framework similar in spirit to cmsRun. It allows one to write a new macro, and dynamically load it into the framework at runtime. The point of entry is the file macroRun.C. The MC and data samples to use, as well as the cuts and histograms to make can all be specified at runtime as well via a collection of json config files (see 'Running' below).

The framework takes care of loading the samples and parsing them into a nice structure that can be easily used by any macro, to avoid code repetition and promote a uniform syntax across code with different purposes.

## Setting up environment

The framework is meant to be run on the LPC, though it probably works on lxplus without major changes needed. It uses RDataFrame which is a recent ROOT feature, so after logging into the LPC first set up an LCG release with a new version of ROOT, such as LCG_96python3:

```bash
$ source /cvmfs/sft.cern.ch/lcg/views/LCG_96python3/x86_64-slc6-gcc8-opt/setup.sh
```
(Only tested on SL6 for now, but hopefully CentOS 7 works too.) The ROOT version setup by this LCG is 6.18.00, and the compiler is GCC 8.2.0.

## Compiling

The framework has a CMakeLists.txt file to make it easy to build the executable and the macros as shared libraries. Just make a build directory, call cmake and then run make install to build:

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make install -j8
```

This will install the libraries and the macroRun executable under macros/bin/, as well as needed ROOT dictionaries.

## Running

To run, invoke macroRun with the desired set of configs (assuming you're at the build dir from the previous step):

```bash
$ cd ../bin
$ ./macroRun -c ../configs/cuts/CUTFILENAME.json -m ../configs/macros/MACROFILENAME.json \
-b ../configs/samples/CRABITERATION/BACKGROUNDCONFIG.json -d ../configs/samples/CRABITERATION/DATACONFIG.json \
-s ../configs/samples/SIGNALCONFIG.json -o OUTFILE.root
```

(note the need to switch to the bin directory for now, in order to avoid having to specify LD_LIBRARY_PATH to find the macro libraries.)

Capitalized names are placeholders for the actual config files and dirs. There's a config to define the desired set of cuts, another one to define the macros to run and each of theirconfigs, and a set of configs to specify the ntuples to run for each group of samples (data, background, and signal).

Run '.macroRun -h' to see the expected syntax.

## Adding new macros

As mentioned above, the framework is able to dynamically load new macros. To create a new macro, add two new files 'mYourMacro.C' and 'mYourMacro.h' following the structure of an existing macro. Note that all macros must have the same call signature, with the same main function 'process'. Then add this new macro name (mYourMacro) to the list of libraries to be build in 'CMakeLists.txt' (near the top of the file). When you run 'make' from the build directory, it should compile the new macro and place it under bin/ to be used by the framework. Then all that's left is to create a .json config file uncer configs/macros/ to use the new macro.

## Current macros and workflow

The main analysis for iDM is done with a set of macros which are to be run in the appropriate order:

* mMainAnalysis.C: this macro runs on top of the flat ntuples and generates the desired histograms with the specified cuts, all via the json config files. In its latest version it uses ROOT's RDataFrame for columnar analysis as well as automatic multi-threading support (by invoking the RDataFrameSelectors/RDFAnalysis.C code). With minor modifications it's also possible to select the older TSelector analyzer (in TSelectors/mainAnalysisSelector.C), though that is much slower than the RDF version. This macro creates 3 THStacks for each desired histogram, and for each subsequent cut: a data, background, and signal THStack. All the THStacks are saved to the specified output ROOT file.
* mMakePlotsFromFile.C: This macro takes the previous output ROOT file, and makes TCanvas objects out of all the THStacks. By default the background THStack is stacked, while the signal one is overlaid (and the data THStack contains only one entry, the data sample). This macro will save all of the Canvases together with the stacks in the same ROOT file.
* mSaveCanvases.C: This macro takes again the ROOT file and saves only the TCanvas objects to the specified directory, with a sub-directory for each histogram. Three versions of each plot are saved: .pdf, .png, and .root. All the histograms with different cuts are saved to the appropriate sub-directory.

One example of a workflow would be:

```bash
$ ./macroRun -c ../configs/cuts/CR_nJets.json -m ../configs/macros/cutflow_CR_nJets.json -b ../configs/samples/sixthrun/backgrounds_full.json -d ../configs/samples/sixthrun/data_full.json -o plots_CR_nJets_sixthrun.root
$ ./macroRun -c ../configs/cuts/CR_nJets.json -m ../configs/macros/MakePlots.json -i plots_CR_nJets_sixthrun.root
$ ./macroRun -c ../configs/cuts/CR_nJets.json -m ../configs/macros/SaveCanvases.json -i plots_CR_nJets_sixthrun.root
```
