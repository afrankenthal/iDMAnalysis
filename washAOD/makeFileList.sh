#! /bin/bash

mchi=52p5
dmchi=5p0
ctau=1
basedir="2018/GenFilter"
basename="Mchi-${mchi}_dMchi-${dmchi}_ctau-${ctau}"

eos root://cmseos.fnal.gov ls /store/user/as2872/iDM/AOD_Samples/$basedir/$basename > data/iDM/$basedir/${basename}.list

sed -i '/MINIAOD/d' data/iDM/$basedir/${basename}.list

sed -i "s|^|root://cmsxrootd-site.fnal.gov//store/user/asterenb/iDM/AOD_Samples/$basedir/$basename/|" data/iDM/$basedir/${basename}.list
