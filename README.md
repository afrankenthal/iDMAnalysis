# Instruction

## Setup

```bash
export SCRAM_ARCH=slc6_amd64_gcc630
cmsrel CMSSW_9_4_5
cd CMSSW_9_4_5/src
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
cmsRun python/ConfFile_cfg.py       test=1
cmsRun python/skim_cfg.py           test=1
cmsRun python/simplevertexer_cfg.py test=1

# run over samples
cmsRun python/simplevertexer_cfg.py inputFiles_load=data/20180421/sample_1cm.txt outputFile=simplevertex_1cm.root
# Or
cd test
sh simplevtxrun.sh
```
