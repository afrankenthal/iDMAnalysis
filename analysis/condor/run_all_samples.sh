#!/bin/bash

# always use the latest version of the codebase
./compress_codebase.sh

# choose representative folder name according to what is being done
eosdir="Baseline"

./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2018/signal_aEM_all.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2018/signal_aEM_main.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2018/bkgs_DY.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2018/bkgs_QCD_1.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2018/bkgs_QCD_2.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2018/bkgs_Top_1.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2018/bkgs_Top_2.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2018/bkgs_WJets.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2018/bkgs_ZJets.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2018/bkgs_DY.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2018/bkgs_QCD_1.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2018/bkgs_QCD_2.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2018/bkgs_Top_1.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2018/bkgs_Top_2.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2018/bkgs_WJets.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2018/bkgs_ZJets.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2018/data_Run2018A.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2018/data_Run2018B.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2018/data_Run2018C.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2018/data_Run2018D.json $eosdir

./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2017/signal_aEM_all.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2017/signal_aEM_main.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2017/bkgs_DY.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2017/bkgs_QCD_1.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2017/bkgs_QCD_2.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2017/bkgs_Top_1.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2017/bkgs_Top_2.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2017/bkgs_WJets.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2017/bkgs_ZJets.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2017/bkgs_DY.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2017/bkgs_QCD_1.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2017/bkgs_QCD_2.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2017/bkgs_Top_1.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2017/bkgs_Top_2.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2017/bkgs_WJets.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2017/bkgs_ZJets.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2017/data_Run2017B.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2017/data_Run2017C.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2017/data_Run2017D.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2017/data_Run2017E.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2017/data_Run2017F.json $eosdir

./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2016/signal_aEM_all.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2016/signal_aEM_main.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2016/bkgs_DY.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2016/bkgs_QCD_1.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2016/bkgs_QCD_2.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2016/bkgs_Top_1.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2016/bkgs_Top_2.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2016/bkgs_WJets.json $eosdir
./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/SR.json configs/samples/fourteenthrun/2016/bkgs_ZJets.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/bkgs_DY.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/bkgs_QCD_1.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/bkgs_QCD_2.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/bkgs_Top_1.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/bkgs_Top_2.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/bkgs_WJets.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/bkgs_ZJets.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/data_Run2016B.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/data_Run2016C.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/data_Run2016D.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/data_Run2016E.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/data_Run2016F.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/data_Run2016G.json $eosdir
# ./submit_condor.sh configs/macros/Run2DCutflowVertexError.json configs/cuts/CRnJets.json configs/samples/fourteenthrun/2016/data_Run2016H.json $eosdir