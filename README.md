# iDMSkimmer
iDM Analysis repository (skimming, trigger/reco efficiency, and analysis)

## Environment setup

```bash
# For 2018
cmsrel CMSSW_10_2_14
cd CMSSW_10_2_14/src
cmsenv

# For both years (needs testing to validate):
cmsrel CMSSW_10_2_18
cd CMSSW_10_2_18/src
cmsenv

git clone https://git@github.com:afrankenthal/iDMSkimmer.git
cd iDMSkimmer/washAOD
scram b -j8
```

## Running ntuplizer to make flat trees from AODs

```bash
# Might need to renew your proxy:
voms-proxy-init -voms cms -valid 192:00

# Test run
cmsRun python/iDMAnalyzer_cfg.py test=1

# Call multicrab to run over all samples
cd python
./multicrab_iDMAnalyzer -c submit -w WORKAREA -s X # where X = {data,MC,custom}
```
