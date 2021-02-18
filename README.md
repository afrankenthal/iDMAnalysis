# iDMAnalysis

iDM Analysis repository (includes skimmer, analysis, and limits computation)

## Environment setup

```bash
# For all years:
$ cmsrel CMSSW_10_2_18
$ cd CMSSW_10_2_18/src
$ cmsenv
$ git cms-init
```

## Cloning the repository

To contribute development, fork this repository on GitHub and then clone from your own fork:

```bash
git clone git@github.com:[you]/iDMAnalysis.git
```

Otherwise, just directly clone this repository:

```bash
git clone git@github.com:afrankenthal/iDMAnalysis.git
```

or via HTTPS if you don't have SSH keys set up:

```bash
git clone https://github.com/afrankenthal/iDMAnalysis.git
```

Follow the README files below depending on what you want to do.

## Running ntuplizer to make flat trees from AODs

Check out the instructions in the [README](skimmer/) file inside skimmer/.

## Running the analysis on top of flat ntuples

Check out the instructions in the [README](analysis/) file inside analysis/.

## Running limits computations

Check out the instructions in the [README](limits/) file inside limits/.

## Contributing

To contribute code, create a Pull Request from your fork (either the master/main branch or a feature branch) to this repository. Don't forget to merge in or rebase (preferred) any upstream changes before making the PR.