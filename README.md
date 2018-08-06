# Instruction

## Setup

```bash
export SCRAM_ARCH=slc6_amd64_gcc700
cmsrel CMSSW_10_2_0
cd CMSSW_10_2_0/src
cmsenv

git clone https://wsi@gitlab.cern.ch/wsi/Firefighter.git
cd Firefighter
scram b -j12
```

## Run
```bash
#voms-proxy-init -voms cms -valid 192:00
```

### Processing AOD

```bash
cd washAOD

# run for test
cmsRun python/tuplizer_cfg.py test=1

# run over samples
cd test
cat cmds.txt
```
