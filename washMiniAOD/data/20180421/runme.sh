#!/bin/sh

ls /eos/uscms/store/user/wsi/standaloneComp/ | grep ctau-1p20e-03_ | grep MINIAOD > sample_0p1cm.txt
ls /eos/uscms/store/user/wsi/standaloneComp/ | grep ctau-0p012_ | grep MINIAOD > sample_1cm.txt
ls /eos/uscms/store/user/wsi/standaloneComp/ | grep ctau-0p12_ | grep MINIAOD > sample_10cm.txt
ls /eos/uscms/store/user/wsi/standaloneComp/ | grep ctau-0p6_ | grep MINIAOD > sample_50cm.txt
ls /eos/uscms/store/user/wsi/standaloneComp/ | grep ctau-1p2_ | grep MINIAOD > sample_100cm.txt
ls /eos/uscms/store/user/wsi/standaloneComp/ | grep ctau-3p6_ | grep MINIAOD > sample_300cm.txt

python modifier.py
