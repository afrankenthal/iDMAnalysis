#!/bin/bash

cmsRun python/tuplizer_cfg.py year=2017 inputFiles_load=data/iDM/Mchi-6p0_dMchi-2p0_ctau-1000_v2.list outputFile=Mchi-6p0_dMchi-2p0_ctau-1000.root 2>&1

cmsRun python/tuplizer_cfg.py year=2017 inputFiles_load=data/iDM/Mchi-6p0_dMchi-2p0_ctau-100_v2.list outputFile=Mchi-6p0_dMchi-2p0_ctau-100.root 2>&1

cmsRun python/tuplizer_cfg.py year=2017 inputFiles_load=data/iDM/Mchi-6p0_dMchi-2p0_ctau-10_v2.list outputFile=Mchi-6p0_dMchi-2p0_ctau-10.root 2>&1

cmsRun python/tuplizer_cfg.py year=2017 inputFiles_load=data/iDM/Mchi-6p0_dMchi-2p0_ctau-1_v2.list outputFile=Mchi-6p0_dMchi-2p0_ctau-1.root 2>&1
