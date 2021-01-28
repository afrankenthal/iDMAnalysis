### WARNING this script is just for bookeeping for now!
# Don't run it like this
# 0. Make combine directory inside macro runs
mkdir combine
cp sensitivity_map data_for_combine.json combine
cd combine
# 1. Make datacard from data_for_combine.json file (from SensitivityMap macro)
datacard_maker_from_json_combined_categories_separate_years.py data_for_combine.json
# 2. Convert text datacard into Roofit workspaces
combineTool.py -M T2W -i */datacard.txt -o workspace.root --parallel 16
# 3. Calculate Asymptotic or HybridNew limits
combineTool.py -M AsymptoticLimits -d */workspace.root --there -n .limit --parallel 16
# or
combineTool.py -M HybridNew -d */workspace.root --there -n .limit --parallel 16
for val in 0.5 0.16 0.84 0.975 0.025; do combineTool.py -M HybridNew --expectedFromGrid=$val -d */workspace.root --there -n .limit --parallel 16; done
# 4. Combine limits into json files
combineTool.py -M CollectLimits */*.limit.{Asym,Hyb}* --use-dirs -o limits.json 
# 4.5. Check if any limits from asymptotic method are poor
for lim in limits_sig_*; do if (( $(cat $lim | wc -l) < 6 )); then echo $lim; fi ; done
# 5. Plot limits with python scripts
pickBestLimit2DCombinedCats.py signal_config.json limits_directory

# Obsolete:
#python ../../../scripts/plotLimitsSimple.py limits_reco_METmu_dphi_vs_reco_vtx_significance_cut34_sig_52p5_1.json limits_reco_METmu_dphi_vs_reco_vtx_significance_cut39_sig_52p5_10.json limits_reco_METmu_dphi_vs_reco_vtx_significance_cut39_sig_52p5_100.json limits_reco_METmu_dphi_vs_reco_vtx_significance_cut37_sig_5p25_1000.json
#vi limits_reco_METmu_dphi_vs_reco_vtx_significance_cut34_sig_52p5_1.json # change json label from Higgs mass 'mt120' to ctau parameter
#vi limits_reco_METmu_dphi_vs_reco_vtx_significance_cut39_sig_52p5_10.json # change json label from Higgs mass 'mt120' to ctau parameter
#vi limits_reco_METmu_dphi_vs_reco_vtx_significance_cut39_sig_52p5_100.json # change json label from Higgs mass 'mt120' to ctau parameter
#vi limits_reco_METmu_dphi_vs_reco_vtx_significance_cut36_sig_52p5_10.json # change json label from Higgs mass 'mt120' to ctau parameter
#vi limits_reco_METmu_dphi_vs_reco_vtx_significance_cut36_sig_52p5_100.json # change json label from Higgs mass 'mt120' to ctau parameter
#vi limits_reco_METmu_dphi_vs_reco_vtx_significance_cut37_sig_52p5_1000.json # change json label from Higgs mass 'mt120' to ctau parameter
