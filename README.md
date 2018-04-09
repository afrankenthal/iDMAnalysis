# Instruction

## Setup

```bash
export SCRAM_ARCH=slc6_amd64_gcc630
cmsrel CMSSW_9_4_5
cd CMSSW_9_4_5/src
cmsenv

git clone https://wsi@gitlab.cern.ch/wsi/Firefighter.git
cd Firefighter
scram b -j8
```

## Run
```bash
#voms-proxy-init -voms cms -valid 192:00

cmsRun DecayLengthAnalyzer/python/ConfFile_cfg.py \
inputFiles=root://cms-xrd-global.cern.ch//store/user/escalant/HTo2LongLivedTo4mu_MH-125_MFF-20_CTau-1300mm_TuneCUETP8M1_13TeV_pythia8/crab_HTo2LongLivedTo4mu_MH-125_MFF-20_CTau-1300mm_TuneCUETP8M1_13TeV_pythia8_AODSIM-ReHLT_V37-v1/180128_100557/0000/displacedMuons_RAW2DIGI_L1Reco_RECO_1.root \
outputFile=histo_ctau-1300mm.root
```
