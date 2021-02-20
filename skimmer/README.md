# Skimmer

## Pull and compile packages

Pull KalmanVertexFit and VertexTools packages locally to extend the native Kalman Filter range limit:

```bash
git cms-addpkg RecoVertex/VertexTools
git cms-addpkg RecoVertex/KalmanVertexFit
```
and apply the following git patch:

```bash
curl https://home.fnal.gov/~as2872/extendTrackerDimensions.patch | git apply
```

Now compile everything:

```bash
scram b -j 8
```

Then head to the skimmer folder:

```bash
cd iDMAnalysis/skimmer
```

You might need to renew your proxy first:

```bash
voms-proxy-init -voms cms -valid 192:00
```

For a local test run:

```bash
cmsRun test/run_ntuplizer_cfg.py test=1
```

Call multicrab to run over more samples in EOS:

```bash
multicrab_iDMAnalysis.py -c submit -w WORKAREA -s X # where X = {data,MC,custom}
```