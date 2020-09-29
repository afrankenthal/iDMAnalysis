# iDMSkimmer
iDM Analysis repository (skimming, trigger/reco efficiency, and analysis)

## Environment setup

```bash
# For all years:
cmsrel CMSSW_10_2_18
cd CMSSW_10_2_18/src
cmsenv

# Need to pull KalmanVertexFit and VertexTools packages to 
# extend the native Kalman Filter range limit:
git cms-addpkg RecoVertex/VertexTools
git cms-addpkg RecoVertex/KalmanVertexFit
# Then Change lines 13 and 14 of RecoVertex/VertexTools/src/SequentialVertexFitter.cc:
# const float TrackerBoundsRadius = 112; ==> const float TrackerBoundsRadius = 740;
# const float TrackerBoundsHalfLength = 273.5; ==> const float TrackerBoundsHalfLength = 960;
# Same for lines 14 and 15 of RecoVertex/KalmanVertexFit/src/SingleTrackVertexConstraint.cc:
# const float TrackerBoundsRadius = 112; ==> const float TrackerBoundsRadius = 740;
# const float TrackerBoundsHalfLength = 273.5; ==> const float TrackerBoundsHalfLength = 960;

# Now clone iDM repo
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
