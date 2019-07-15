#include "utils/common.C"
using namespace common;
#include "utils/json.hpp"
using json = nlohmann::json;
using std::cout, std::endl, std::map, std::vector;
#include <iomanip>

void cutflowTable(TString which_cutflow="SR", bool test=true) {
    TDatime time_begin;

    map<TString, SampleInfo> samples;

    TString config_file;
    if (test)
        config_file = TString("configs/backgrounds_subset.json");
    else
        config_file = TString("configs/backgrounds_full.json");

    std::ifstream bkgs_file(config_file.Data());
    json bkg_cfgs;
    bkgs_file >> bkg_cfgs;
    int color = 3;
    for (auto const & [bkg, cfg] : bkg_cfgs.items()) 
        samples[TString(bkg)] = SampleInfo{
            listFiles(cfg["dir"].get<std::string>().c_str()), // list of filenames
            bkg, // plot label
            cfg["xsec"].get<float>(), // xsec
            cfg["sum_gen_wgt"].get<float>(), // sum_gen_wgt
            cfg["limit_num_files"].get<int>(), // limit_num_files
            1, // weight
            TString(cfg["group"].get<std::string>()), // plot group
            color++, // line color
            1 // line style
        };

    //map<TString, SignalInfo> mchis{
    //    {"5p25", SignalInfo{"5", "0p5", "0.1", kYellow}},
    //    {"52p5", SignalInfo{"50", "5p0", "0.1", kBlue}},
    //    {"6p0", SignalInfo{"5", "2p0", "0.4", kGreen}},
    //    {"60p0", SignalInfo{"50", "20p0", "0.4", kRed}}
    //};
    //vector<TString> ctaus{"1", "10", "100", "1000"};
    //for (auto ctau : ctaus) 
    //    for (auto const & [mchi, cfg] : mchis) 
    //        samples[Form("%s_%s", mchi.Data(), ctau.Data())] = SampleInfo{
    //            vector<TString>{Form("../Mchi-%s_dMchi-%s_ctau-%s.root", mchi.Data(), cfg.dmchi.Data(), ctau.Data())}, // list of filenames
    //            Form("m1 = %s GeV, #Delta = %s, c#tau = %s mm", cfg.m1.Data(), cfg.delta.Data(), ctau.Data()), // plot label
    //            1, // xsec
    //            -1, // sum_gen_wgt
    //            -1, // limit_num_files
    //            1, // weight
    //            "", // plot group
    //            cfg.color + 1*(int)(distance(ctaus.begin(), find(ctaus.begin(), ctaus.end(), ctau))), // line color
    //            2 // line style
    //        };

    //map<TString, TString> files { 
    //    {"5p25", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-5p25_dMchi-0p5_ctau-100.root"},
    //    {"52p5", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-52p5_dMchi-5p0_ctau-100.root"},
    //    {"6p0", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-6p0_dMchi-2p0_ctau-100.root"}, 
    //    {"60p0", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-60p0_dMchi-20p0_ctau-100.root"}
    //};
    //map<TString, vector<UInt_t> > cutsIncl {
    //    {"5p25", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    //    {"52p5", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    //    {"6p0", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    //    {"60p0", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
    //};
    //map<TString, vector<UInt_t>> cutsExcl {
    //    {"5p25", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    //    {"52p5", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    //    {"6p0", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
    //    {"60p0", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
    //};
    map<TString, vector<Float_t>> cutsIncl, cutsExcl;
    map<TString, vector<Float_t>> cutsGroupIncl, cutsGroupExcl;
    vector<Float_t> empty_cuts_template = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    cutsGroupIncl.insert(std::make_pair(TString("Diboson"), empty_cuts_template));
    cutsGroupIncl.insert(std::make_pair(TString("DY"), empty_cuts_template));
    cutsGroupIncl.insert(std::make_pair(TString("QCD"), empty_cuts_template));
    cutsGroupIncl.insert(std::make_pair(TString("TTbar"), empty_cuts_template));
    cutsGroupIncl.insert(std::make_pair(TString("ZJets"), empty_cuts_template));
    cutsGroupIncl.insert(std::make_pair(TString("WJets"), empty_cuts_template));
    cutsGroupExcl.insert(std::make_pair(TString("Diboson"), empty_cuts_template));
    cutsGroupExcl.insert(std::make_pair(TString("DY"), empty_cuts_template));
    cutsGroupExcl.insert(std::make_pair(TString("QCD"), empty_cuts_template));
    cutsGroupExcl.insert(std::make_pair(TString("TTbar"), empty_cuts_template));
    cutsGroupExcl.insert(std::make_pair(TString("ZJets"), empty_cuts_template));
    cutsGroupExcl.insert(std::make_pair(TString("WJets"), empty_cuts_template));

    vector<int> cutOrder { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 19, 20, 21, 17, 18 };
    vector<int> cutMask;
    if (which_cutflow == "SR")
        cutMask = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    else if (which_cutflow == "CR_nJets")
        cutMask = { 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    else if (which_cutflow == "CR_dR")
        cutMask = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1 };
    else {
        cout << "ERROR! Didn't understand which cutflow you want. Exiting..." << endl;
        return;
    }


    Float_t lumi = 59.97 * 1000; // 1/pb

    for (auto const & [sample, props] : samples) {

        TChain * data_reco = new TChain("ntuples_gbm/recoT");
        TChain * data_gen = new TChain("ntuples_gbm/genT");
        int count = 0;
        for (auto const & filename : props.filenames) {
            if (count++ >= props.limit_num_files && props.limit_num_files != -1) continue;
            data_gen->Add(filename.Data());
            data_reco->Add(filename.Data());
        }
        Float_t gen_wgt;
        data_gen->SetBranchAddress("gen_wgt", &gen_wgt);
        data_gen->GetEntries();
        data_reco->GetEntries();
        data_reco->AddFriend(data_gen, "nutples_gbm/genT");

        cout << "sample: " << sample << ", sum_gen_wgt: " << props.sum_gen_wgt << endl;

        uint32_t cuts;
        data_reco->SetBranchAddress("cuts", &cuts);
        Int_t nGBMDSAMatch;
        data_reco->SetBranchAddress("reco_n_gbmdsa_match", &nGBMDSAMatch);
        
        cutsIncl.insert(std::make_pair(sample, empty_cuts_template));
        cutsExcl.insert(std::make_pair(sample, empty_cuts_template));
        int nEntries = data_reco->GetEntries();
        for (int i = 0; i < nEntries; i++) {
            data_reco->GetEntry(i);
            bool cumPass = true;
            for (auto j : cutOrder) {
                // manual cuts (DSA-GBM category)
                if (j >= 19) {
                    if (j == 19) { // only do once for 19,20,21
                        if (nGBMDSAMatch != -1) {
                            if (cumPass)
                                cutsGroupIncl[props.group][(int)(19+nGBMDSAMatch)] += gen_wgt * lumi * props.xsec / props.sum_gen_wgt;
                            cutsGroupExcl[props.group][(int)(19+nGBMDSAMatch)] += gen_wgt * lumi * props.xsec / props.sum_gen_wgt;
                        }
                    }
                }
                // bitmask cuts
                else {
                    if ((readCutBit(cuts, j) && cutMask[j]) || (!readCutBit(cuts,j) && !cutMask[j])) {
                        cutsExcl[sample][j] += gen_wgt * lumi * props.xsec / props.sum_gen_wgt;
                        if (props.group != TString(""))
                            cutsGroupExcl[props.group][j] += gen_wgt * lumi * props.xsec / props.sum_gen_wgt;
                    }
                    else
                        cumPass = false;
                    if (((readCutBit(cuts, j) && cutMask[j]) || (!readCutBit(cuts,j) && !cutMask[j])) && cumPass) {
                        cutsIncl[sample][j] += gen_wgt * lumi * props.xsec / props.sum_gen_wgt;
                        if (props.group != TString(""))
                            cutsGroupIncl[props.group][j] += gen_wgt * lumi * props.xsec / props.sum_gen_wgt;
                    }
                }
            }
        }
    }
    cout << std::setprecision(1) << std::fixed;

    cout << "Inclusive:" << endl;
    for (auto group : cutsGroupIncl)
        cout << group.first << " ";
    cout << endl;
    for (auto j : cutOrder) {
        cout << "cut " << j << " & ";
        for (auto const & group : cutsGroupIncl) {
            cout << cutsGroupIncl[group.first][j] << " & ";
        }
        cout << " \\\\ " << endl;
    }
    cout << "Exclusive:" << endl;
    for (auto group : cutsGroupExcl)
        cout << group.first << " ";
    cout << endl;
    for (auto j : cutOrder) {
        cout << "cut " << j << " & ";
        for (auto group : cutsGroupIncl) {
            cout << cutsGroupExcl[group.first][j] << " & ";
        }
        cout << " \\\\ " << endl;
    }

    printTimeElapsed(time_begin);
}
