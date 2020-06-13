### WARNING this script is just for bookeeping for now!
# Don't run it like this
mkdir combineRunMASSXX
cp sensitivity_map data_for_combine.json combineRunMASSXX
cd combineRUNMASSXX
DatacardMakerFromJson data_for_combine.json
combineTool.py -M T2W -i */datacard.txt -o workspace.root --parallel 16
combineTool.py -M HybridNew -d */workspace.root --there -n .limit --parallel 16
combineTool.py -M HybridNew --expectedFromGrid=0.5 -d */workspace.root --there -n .limit --parallel 16
combineTool.py -M HybridNew --expectedFromGrid=0.16 -d */workspace.root --there -n .limit --parallel 16
combineTool.py -M HybridNew --expectedFromGrid=0.84 -d */workspace.root --there -n .limit --parallel 16
combineTool.py -M HybridNew --expectedFromGrid=0.975 -d */workspace.root --there -n .limit --parallel 16
combineTool.py -M HybridNew --expectedFromGrid=0.025 -d */workspace.root --there -n .limit --parallel 16
combineTool.py -M CollectLimits */*.limit.Hyb* --use-dirs -o limits.json 
vi limits_reco_METmu_dphi_vs_reco_vtx_significance_cut34_sig_52p5_1.json # change json label from Higgs mass 'mt120' to ctau parameter
vi limits_reco_METmu_dphi_vs_reco_vtx_significance_cut39_sig_52p5_10.json # change json label from Higgs mass 'mt120' to ctau parameter
vi limits_reco_METmu_dphi_vs_reco_vtx_significance_cut39_sig_52p5_100.json # change json label from Higgs mass 'mt120' to ctau parameter
vi limits_reco_METmu_dphi_vs_reco_vtx_significance_cut36_sig_52p5_10.json # change json label from Higgs mass 'mt120' to ctau parameter
vi limits_reco_METmu_dphi_vs_reco_vtx_significance_cut36_sig_52p5_100.json # change json label from Higgs mass 'mt120' to ctau parameter
vi limits_reco_METmu_dphi_vs_reco_vtx_significance_cut37_sig_52p5_1000.json # change json label from Higgs mass 'mt120' to ctau parameter
python ../../../scripts/plotLimitsSimple.py limits_reco_METmu_dphi_vs_reco_vtx_significance_cut34_sig_52p5_1.json limits_reco_METmu_dphi_vs_reco_vtx_significance_cut39_sig_52p5_10.json limits_reco_METmu_dphi_vs_reco_vtx_significance_cut39_sig_52p5_100.json limits_reco_METmu_dphi_vs_reco_vtx_significance_cut37_sig_5p25_1000.json
