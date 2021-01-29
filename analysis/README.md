# Analysis

This is a self-contained macro-based framework similar in spirit to cmsRun. One can write a new macro and dynamically load it into the framework at runtime. The point of entry is macroRun.cc. The MC and data samples to use, and the cuts and histograms to make can all be specified at runtime as well via a collection of json config files (see 'Running' below).

The framework takes care of loading the samples and parsing them into a clean structure that can be easily used by any macro, to avoid code repetition and promote a uniform syntax across the codebase.

## Setting up environment

The framework is meant to be run on the LPC, though it probably works on lxplus without major changes. It uses RDataFrame which is a recent ROOT feature, so after logging into the LPC, first set up an LCG release with a new version of ROOT, such as LCG_96python3:

```bash
$ source /cvmfs/sft.cern.ch/lcg/views/LCG_96python3/x86_64-centos7-gcc8-opt/setup.sh
```
The ROOT version set up by this LCG is 6.18.00, and the compiler is GCC 8.2.0.

## Compiling

The framework has a CMakeLists.txt file to easily build the executable and each macro as a shared library. Just make a build directory, call cmake and then run make install to build:

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make install -j8
```

This will install the libraries and the macroRun executable under bin/, as well as needed ROOT dictionaries.

## Running

To run, invoke macroRun with the desired set of configs from the bin/ dir:

```bash
$ cd ../bin
$ ./macroRun -c ../configs/cuts/CUTFILENAME.json -m ../configs/macros/MACROFILENAME.json \
-b ../configs/samples/CRABITERATION/BACKGROUNDCONFIG.json -d ../configs/samples/CRABITERATION/DATACONFIG.json \
-s ../configs/samples/SIGNALCONFIG.json -o OUTFILE.root
```

(note the need to switch to the bin directory for now, in order to avoid having to specify LD_LIBRARY_PATH to find the macro libraries.)

Capitalized names are placeholders for the actual config files and dirs. There's a config to define the desired set of cuts, another one to define the macros to run and each of theirconfigs, and a set of configs to specify the ntuples to run for each group of samples (data, background, and signal). You can run on only background or signal or data by omitting their respective configs.

Run './macroRun -h' to see the expected syntax.

## Adding new macros

As mentioned above, the framework is able to dynamically load new macros. To create a new macro, just create a new file 'mYourMacro.C' under macros/following the structure of existing macros. Note that all macros must have the same call signature, with the same main function 'process'. When you run 'make' from the build directory, it should automatically find and compile the new macro and move it to bin/ for use by the framework. Then all that's left is to create a .json config file uncer configs/macros/ to use the new macro.

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
