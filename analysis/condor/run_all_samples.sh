#!/bin/bash

# always use the latest version of the codebase
./compress_codebase.sh

# choose representative path name according to what is being done
# final EOS path is /store/group/lpcmetx/iDM/analysis/$ntuple_run/$eosdir
eosdir="$USER/ABCD_PFIso" 
macro="configs/macros/RunCutflow.json"
plots="configs/plots/MuonDxyAndPFIso.json"
cutsSR="configs/cuts/SR_dxyNoCuts_NoPFIso.json"
cutsCR="configs/cuts/CRnJets_dxyNoCuts_NoPFIso.json"

rm -f -- condor_args.txt

echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/signal_aEM_all.json $eosdir" >> condor_args.txt
# echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/signal_aEM_main.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/signal_aEM_all.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/signal_0p1_all.json $eosdir" >> condor_args.txt
# echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/signal_aEM_main.json $eosdir" >> condor_args.txt
# echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/signal_aEM_forplots.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/bkgs_QCD_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/bkgs_QCD_4.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/bkgs_QCD_5.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/bkgs_Top_1.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/bkgs_Top_2.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/bkgs_Top_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/bkgs_WJets_1.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/bkgs_WJets_2.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/bkgs_WJets_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2018/bkgs_ZJets.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/bkgs_QCD_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/bkgs_QCD_4.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/bkgs_QCD_5.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/bkgs_Top_1.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/bkgs_Top_2.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/bkgs_Top_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/bkgs_WJets_1.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/bkgs_WJets_2.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/bkgs_WJets_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/bkgs_ZJets.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/data_Run2018A.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/data_Run2018B.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/data_Run2018C.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2018/data_Run2018D.json $eosdir" >> condor_args.txt

echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/signal_aEM_all.json $eosdir" >> condor_args.txt
# echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/signal_aEM_main.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/signal_aEM_all.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/signal_0p1_all.json $eosdir" >> condor_args.txt
# echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/signal_aEM_main.json $eosdir" >> condor_args.txt
# echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/signal_aEM_forplots.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/bkgs_QCD_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/bkgs_QCD_4.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/bkgs_QCD_5.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/bkgs_Top_1.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/bkgs_Top_2.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/bkgs_Top_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/bkgs_WJets_1.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/bkgs_WJets_2.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/bkgs_WJets_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2017/bkgs_ZJets.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/bkgs_QCD_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/bkgs_QCD_4.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/bkgs_QCD_5.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/bkgs_Top_1.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/bkgs_Top_2.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/bkgs_Top_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/bkgs_WJets_1.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/bkgs_WJets_2.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/bkgs_WJets_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/bkgs_ZJets.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/data_Run2017B.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/data_Run2017C.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/data_Run2017D.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/data_Run2017E.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2017/data_Run2017F.json $eosdir" >> condor_args.txt

echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/signal_aEM_all.json $eosdir" >> condor_args.txt
# echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/signal_aEM_main.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/signal_aEM_all.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/signal_0p1_all.json $eosdir" >> condor_args.txt
# echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/signal_aEM_main.json $eosdir" >> condor_args.txt
# echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/signal_aEM_forplots.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/bkgs_QCD_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/bkgs_QCD_4.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/bkgs_QCD_5.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/bkgs_Top_1.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/bkgs_Top_2.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/bkgs_Top_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/bkgs_WJets_1.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/bkgs_WJets_2.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/bkgs_WJets_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsSR configs/samples/fourteenthrun/2016/bkgs_ZJets.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/bkgs_QCD_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/bkgs_QCD_4.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/bkgs_QCD_5.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/bkgs_Top_1.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/bkgs_Top_2.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/bkgs_Top_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/bkgs_WJets_1.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/bkgs_WJets_2.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/bkgs_WJets_3.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/bkgs_ZJets.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/data_Run2016B.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/data_Run2016C.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/data_Run2016D.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/data_Run2016E.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/data_Run2016F.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/data_Run2016G.json $eosdir" >> condor_args.txt
echo "$macro $plots $cutsCR configs/samples/fourteenthrun/2016/data_Run2016H.json $eosdir" >> condor_args.txt

condor_submit condor_job.sub