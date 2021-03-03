# Analysis

This is a self-contained macro-based framework similar in spirit to cmsRun. One can write a new macro and dynamically load it into the framework at runtime. The point of entry is `macroRun.cc`. The MC and data samples to use, and the cuts and histograms to make can all be specified at runtime as well via a collection of json config files (see 'Running' below).

The framework takes care of loading the samples and parsing them into a clean structure that can be easily used by any macro, to avoid code repetition and promote a uniform syntax across the codebase.

## Setting up the environment

The framework is meant to be run on the LPC, though it probably works on lxplus without major changes. It uses RDataFrame which is a recent ROOT feature, so after logging into the LPC, first source the `setup.sh` script which sets up path locations and an LCG release with a recent version of ROOT (LCG_99):

```bash
$ source setup.sh
```
The ROOT version set up by this LCG is 6.22.06, and the compiler is GCC 8.3.0.

**NOTE**: this setup should not be used simultaneously with a CMSSW release, since cmsenv sets up conflicting software versions. Use a different session between running the skimmer and this code.

**NOTE**: currently `CMakeLists.txt` is hard-coded for a particular major version of ROOT (i.e. 6.22), so there isn't actually flexibility on the LCG version.

## Compiling

The framework uses CMake to easily build the executable and each macro as a shared library. Just make a build directory, call cmake from inside and then run make install to build:

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make -j8
$ make install
```

This will install the shared libraries under `lib/` and the macroRun executable under `bin/`, as well as any needed ROOT dictionaries.

## Running

To run, invoke macroRun with the desired set of configs (assuming you are in the base `analysis` folder):

```bash
$ cd ..
$ macroRun \
-m configs/macros/[macro-config].json \
-p configs/plots/[plot-config].json \
-c configs/cuts/[cuts-config].json \
-s configs/samples/[ntuple-version]/[bkg-config].json \
-s configs/samples/[ntuple-version]/[data-config].json \
-s configs/samples/[ntuple-version]/[signal-config].json \
-o [out-file].root
```

There are four sets of relevant configs depending on the workflow:

- a `cuts` config to define the desired set of cuts
- a `macro` config to define the macros to run and associated settings
- a `plots` config to define the set of plots to produce
- a set of `sample` configs to specify datasets to run on (data, background, or signal). You can run on only background or signal or data by omitting the other configs.

Type `macroRun -h` to see the full allowed syntax.

## Adding new macros

As mentioned, the framework is able to dynamically load new macros. To make a new macro, just create a new file `mYourMacro.C` under `macros/` following the structure of existing macros. Note that all macros must have the same call signature, with the same main function `process`. When you run `make` from the build directory, it should automatically find and compile the new macro and move it to `lib/` for use by the framework. Then all that's left is to create a `.json` config file under `configs/macros/` to use the new macro.

## Current macros and "official" analysis workflow

The main analysis for iDM is done with a set of macros which are to be run in the appropriate order:

1) `mMainAnalysis.C`: this macro runs on top of the flat ntuples and generates the desired histograms with the specified cuts, all via the json config files. In the latest version it uses ROOT's RDataFrame for columnar analysis as well as automatic multi-threading support (by invoking the `Selectors/RDataFrame/RDFAnalysis.C` code). With minor modifications it's also possible to select the older TSelector analyzer (in `Selectors/TSelectors/mainAnalysisSelector.C`), though that is much slower than the RDF version. The macro creates up to 3 THStacks for each desired histogram and for each cut: data, background, and signal THStacks (if their respective sample configs are provided). All THStacks are saved to the specified output ROOT file. The macro configs controlling the plots to be aggregrated include (e.g.) `RunCutflow.json`, and the set of plots to produce `Nominal2D.json` and `Nominal1D.json`. Example invocation:

    ```bash
    $ macroRun -m configs/macros/RunCutflow.json -p configs/plots/Nominal2D.json -c configs/cuts/CRnJets.json -s configs/samples/fourteenthrun/2018/backgrounds_full.json -s configs/samples/fourteenthrun/2018/data_full.json -o plots_CRnJets_fourteenthrun_2018.root
    $ macroRun -m configs/macros/RunCutflow.json -p configs/plots/Nominal2D.json -c configs/cuts/CRnJets.json -s configs/samples/fourteenthrun/2017/backgrounds_full.json -s configs/samples/fourteenthrun/2017/data_full.json -o plots_CRnJets_fourteenthrun_2017.root
    $ macroRun -m configs/macros/RunCutflow.json -p configs/plots/Nominal2D.json -c configs/cuts/CRnJets.json -s configs/samples/fourteenthrun/2016/backgrounds_full.json -s configs/samples/fourteenthrun/2016/data_full.json -o plots_CRnJets_fourteenthrun_2016.root
    ```
    This runs the macro for each year, including signal, background, and data samples. It produces plots for the multijet control region (there is a similar cut config for the SR). Here `fourteenthrun` refers to the 14th version of the ntuples in EOS.

2) `mMergeAndCollectStacks.C`: merges stacks from previous runs with separate years or combine when run in parallel condor jobs (see below). Example command following the above:

    ```bash
    $ macroRun -m configs/macros/MergeAndCollectStacks.json
    -i plots_CRnJets_fourteenthrun_2016.root
    -i plots_CRnJets_fourteenthrun_2017.root
    -i plots_CRnJets_fourteenthrun_2018.root
    -o plots_CRnJets_fourteenthrun_161718.root
    ```
    (A list of input files can also be provided with the `-l` option.)

3) `mMakeXXCanvasFromStacks.C`: This set of macros takes the previously produced merged-stack ROOT file and creates TCanvas objects out of all THStacks. By default background THStacks are actually stacked, signal THStacks are overlaid, and data THStacks contain only one entry (naturally). The macros will save all of the TCanvas objects together with the stacks in the same ROOT file by default. The macro config json is `MakeCanvases.json` which calls both 1D and 2D macros. Example:

    ```bash
    $ macroRun -m configs/macros/MakeCanvases.json -c configs/cuts/CRnJets.json -y 161718 -i plots_CRnJets_fourteenthrun_161718.root
    ```
    (Note that the combined years of the merged stacks must be provided for the appropriate CMS lumi labels on the TCanvas.)

4) `mSaveCanvases.C`: This macro takes again the ROOT file and saves only the TCanvas objects to a specified directory. Three versions of each plot are saved: `.pdf`, `.png`, and `.root`. The macro saves all plots to a local temporary folder `plot_temp`, and also tries to copy them back to EOS if that's where they were originally located. But one can also just manually copy all the plots from the temporary folder to e.g. a CERN or FNAL website to visualize them all at once. Example:

    ```bash
    $ macroRun -m configs/macros/SaveCanvases.json -i plots_CRnJets_fourteenthrun_161718.root
    ```

    For producing plots of observables and cutflows only, steps 1-4 are all that's needed. But to derive yields and limits, the next steps are also required.

5) `mMakeTemplates.C`: This macro just takes 2D plots from a VR (or CR) ROOT file and creates 1D profile templates from them, in both X and Y. The VR used for this part of the analysis is the multijet (nJets), because it contains very similar physics to the SR. These profile templates, when derived from both data and background MC, can be used to estimate the background in the SR without looking at data at all there. See the AN for more details, but essentially:

    ```
    bkg in SR from data prediction = (bkg in VR from data / bkg in VR from MC) * bkg in SR from MC
    ```

    Example command:

    ```bash
    $ macroRun -m configs/macros/MakeTemplates.json -i plots_CRnJets_fourteenthrun_161718.root
    ```

6) `mMapABCDSensitivityYearly.C`: This macro computes the ABCD yields for all input TCanvases, and for all years and match categories, from both backgrounds and signal. It also computes the sensitivity based on the signal and background expected. The ABCD boundaries can be given in the config, `MapABCDSensitivityYearly.json`, or can be left to be optimized by the macro. If optimization is requested, it first computes the sensitivity for the combined years 16+17+18 to find the optimal bins, and then computes separately by year the yields for each plot using those bins. Remember, to compute the "background yields" in SR, the template is needed, since we can't look at data in the SR before unblinding. So the necessary inputs for this macro are (hardcoded in the config, notice this one does not take command line arguments): 

    * 2D ABCD plots in the multijet VR from data
    * 2D ABCD plots in the multijet VR from background MC
    * 2D ABCD plots in the SR from background MC
    * 2D ABCD plots in teh SR from signal MC
    * the template file.

    Except for the template file, there are 3 sets of these 4 files for each year. The output of this macro is a huge json file called `data_for_combine.json` which contains the bin boundaries, yields, and sensitivity for each match category and for each year. This file can be directly fed to the limit code to automatically prepare datacards and compute limits. Check out the `limits` folder for those instructions.

## Condor

It's also possible to run step 1) of the workflow on condor to speed things up, and then use the `mMergeAndCollectStacks.C` to combine all the different samples and years together. To use condor, go to the `condor` folder, modify the `run_all_samples.sh` script to suit your needs, and run it. This script first compresses the codebase to send to condor, then run the `submit_condor.sh` script for each sample, each having a possibly different macro, cut, or sample config. Notice that some of the background and data samples are divided up into several parts to roughly balance the condor runtime. These can all be combined later on with the merge macro. The `$eosdir` variable is required to separate different workflows from different users on EOS. Make sure to change it before running the script so as not to overwrite any existing files.