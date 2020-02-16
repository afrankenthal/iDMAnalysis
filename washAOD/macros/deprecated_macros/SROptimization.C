#include <string.h>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <TDatime.h>
#include <TCollection.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>
#include <TString.h>
#include <TChain.h>
#include <TApplication.h>
#include <TCut.h>
#include <TH1F.h>

#include "utils/common.C"
using namespace common;
#include "utils/json.hpp"
using json = nlohmann::json;
#include "utils/cxxopts.hpp"

using std::cout, std::endl, std::map, std::vector;

int main(int argc, char ** argv) {
//void cutflowTableFast() { //int argc, char **argv) { //TString which_cutflow="SR", bool test=true) {
    TDatime time_begin;

    cxxopts::Options options("Optimize SR", "Optimize SR by computing varying cuts and computing S/B significance");
    options.add_options()
        ("c,config", "Config file to use", cxxopts::value<std::string>()->default_value("../configs/signal_local.json"))
        ("f,cutflow", "Which cutflow to use", cxxopts::value<std::string>()->default_value("SR"))
        ("p,param", "Which param to set", cxxopts::value<std::string>()->default_value("abs(reco_vtx_dR)"))
        ("v,value", "Which cut value to set param", cxxopts::value<float>()->default_value("0.8"))
        ("o,outfile", "Output file", cxxopts::value<std::string>()->default_value(""))
        ("h,help", "Print help and exit.")
    ;
    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        cout << options.help() << std::endl;
        exit(0);
    }

    // Program options
    TString which_cutflow = TString(result["cutflow"].as<std::string>());
    vector<TString> config_filenames{TString(result["config"].as<std::string>())};
    TString outfilename = TString(result["outfile"].as<std::string>());
    TString custom_cut = TString(result["param"].as<std::string>());
    Float_t dR_cut = result["value"].as<float>(); 
    Int_t dR_cut_mask = 1, nHighPtJets_cut_mask = 1;
    if (custom_cut.Contains("dR"))
        dR_cut_mask = 2;
    else 
        nHighPtJets_cut_mask = 2;
    
    map<TString, SampleInfo> samples;

    for (auto config_filename : config_filenames) { 
        std::ifstream config_file(config_filename.Data());
        json configs;
        config_file >> configs;
        int color = 3;
        for (auto const & [sample, cfg] : configs.items()) {
            vector<TString> filelist{};
            for (auto dir : cfg["dir"].get<std::vector<std::string>>()) {
                vector<TString> newlist = listFiles(dir.c_str());
                filelist.insert(filelist.end(), newlist.begin(), newlist.end());
            }
            samples[TString(sample)] = SampleInfo{
                filelist,
                //listFiles(cfg["dir"].get<std::string>().c_str()), // list of filenames
                sample, // plot label
                cfg["xsec"].get<float>(), // xsec
                cfg["sum_gen_wgt"].get<float>(), // sum_gen_wgt
                cfg["limit_num_files"].get<int>(), // limit_num_files
                1, // weight
                TString(cfg["group"].get<std::string>()), // plot group
                color++, // line color
                1 // line style
            };
        }
    }

    map<TString, vector<Float_t>> cutsIncl, cutsExcl;
    map<TString, vector<Float_t>> cutsGroupIncl, cutsGroupExcl;
    vector<Float_t> empty_cuts_template = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (auto & [sample, props] : samples) {
        cutsIncl.insert(std::make_pair(sample, empty_cuts_template));
        cutsExcl.insert(std::make_pair(sample, empty_cuts_template));
        // map checks for existence when inserting, no concerns about overwriting
        cutsGroupIncl.insert(std::make_pair(props.group, empty_cuts_template));
        cutsGroupExcl.insert(std::make_pair(props.group, empty_cuts_template));
    }

    vector<int> cutOrder { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 21, 22, 23, 19, 20 };
    // Mask -- 1: require cut bit set; 0: require cut bit unset; -1:  cut bit can be anything; 2: custom
    vector<int> cutMask;
    if (which_cutflow == "SR")
        cutMask = { 1, 1, 1, 1, nHighPtJets_cut_mask, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, dR_cut_mask, 1, 1, 1, 1, 1, 1, 1 };
    else if (which_cutflow == "SR_noBTagging")
        cutMask = { 1, 1, 1, 1, 1, -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    else if (which_cutflow == "CR_QCD")
        cutMask = { 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    else if (which_cutflow == "CR_nJets")
        cutMask = { 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    else if (which_cutflow == "CR_dR")
        cutMask = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1 };
    else {
        cout << "ERROR! Didn't understand which cutflow you want. Exiting..." << endl;
        return 0;
    }

    Float_t lumi = 59.97 * 1000; // 1/pb

    for (auto const & [sample, props] : samples) {

        cout << "sample: " << sample << ", sum_gen_wgt: " << props.sum_gen_wgt << endl;
        if (props.sum_gen_wgt < 0.1) continue;

        TChain * data_reco = new TChain("ntuples_gbm/recoT");
        TChain * data_gen = new TChain("ntuples_gbm/genT");
        int count = 0;
        for (auto const & filename : props.filenames) {
            if (count++ >= props.limit_num_files && props.limit_num_files != -1) continue;
            data_gen->Add(filename.Data());
            data_reco->Add(filename.Data());
        }
        //Float_t gen_wgt;
        //data_gen->SetBranchAddress("gen_wgt", &gen_wgt);
        data_gen->GetEntries();
        data_reco->GetEntries();
        data_reco->AddFriend(data_gen, "nutples_gbm/genT");



        uint32_t cutInclPosMask = 0, cutInclNegMask = 0;
        //uint32_t cutExcl;
        bool bCustom = false;
        for (auto j : cutOrder) {
            if (cutMask[j] == 1)
                cutInclPosMask |= (1 << j); 
            else if (cutMask[j] == 0)
                cutInclNegMask |= (1 << j);
            else if (cutMask[j] == 2)
                bCustom = true;
            //cutExclMask = 1 << j;

            // Handle categories cut (GBM-GBM, GBM-DSA, DSA-DSA)
            // Only want one of the three bits on at any given time
            if (j == 21) 
                cutInclPosMask &= ~((1 << 22) | (1 << 23));
            else if (j == 22) 
                cutInclPosMask &= ~((1 << 21) | (1 << 23));
            else if (j == 23) 
                cutInclPosMask &= ~((1 << 21) | (1 << 22));

            if (bCustom) 
                data_reco->Draw("1",
                        TCut(Form("(gen_wgt * %f * %f / %f)", lumi, props.xsec, props.sum_gen_wgt))
                        * (TCut(Form("%d == (cuts&%d)", cutInclPosMask, cutInclPosMask))
                        && TCut(Form("0 == (cuts&%d)", cutInclNegMask)) && TCut(Form("%s < %f", custom_cut.Data(), dR_cut))), 
                        //&& TCut(Form("%d == (((cuts | %d) & (!cuts | !%d)) & %d)", cutInclNegMask, cutInclNegMask, cutInclNegMask, cutInclNegMask))), // XOR
                                    //lumi, props.xsec, props.sum_gen_wgt, cutInclPosMask, cutInclPosMask)), //cutInclNegMask, cutInclNegMask, cutInclNegMask)),
                        //* TCut(Form("gen_wgt * %f * %f / %f", lumi, props.xsec, props.sum_gen_wgt)),
                        //&& TCut(Form("(%d == ((cuts ^ %d) & %d))", cutInclNegMask, cutInclNegMask, cutInclNegMask))),
                        "goff");
            else 
                data_reco->Draw("1",
                        TCut(Form("(gen_wgt * %f * %f / %f)", lumi, props.xsec, props.sum_gen_wgt))
                        * (TCut(Form("%d == (cuts&%d)", cutInclPosMask, cutInclPosMask))
                        && TCut(Form("0 == (cuts&%d)", cutInclNegMask))), 
                        //&& TCut(Form("%d == (((cuts | %d) & (!cuts | !%d)) & %d)", cutInclNegMask, cutInclNegMask, cutInclNegMask, cutInclNegMask))), // XOR
                                    //lumi, props.xsec, props.sum_gen_wgt, cutInclPosMask, cutInclPosMask)), //cutInclNegMask, cutInclNegMask, cutInclNegMask)),
                        //* TCut(Form("gen_wgt * %f * %f / %f", lumi, props.xsec, props.sum_gen_wgt)),
                        //&& TCut(Form("(%d == ((cuts ^ %d) & %d))", cutInclNegMask, cutInclNegMask, cutInclNegMask))),
                        "goff");
            TH1F * htemp = (TH1F*)gDirectory->Get("htemp");
            cutsGroupIncl[props.group][j] += htemp->GetSumOfWeights();
        }

        //uint32_t cuts;
        //data_reco->SetBranchAddress("cuts", &cuts);
        //Int_t nGBMDSAMatch;
        //data_reco->SetBranchAddress("reco_n_gbmdsa_match", &nGBMDSAMatch);

        //int nEntries = data_reco->GetEntries();
        //for (int i = 0; i < nEntries; i++) {
        //    data_reco->GetEntry(i);
        //    bool cumPass = true;
        //
        //    for (auto j : cutOrder) {
        //        // manual cuts (DSA-GBM category)
        //        if (j >= 21) {
        //            if (j == 21) { // only do once for 20,21,22
        //                if (nGBMDSAMatch != -1) {
        //                    if (cumPass)
        //                        cutsGroupIncl[props.group][(int)(21+nGBMDSAMatch)] += gen_wgt * lumi * props.xsec / props.sum_gen_wgt;
        //                    cutsGroupExcl[props.group][(int)(21+nGBMDSAMatch)] += gen_wgt * lumi * props.xsec / props.sum_gen_wgt;
        //                }
        //            }
        //        }
        //        // bitmask cuts
        //        else {
        //            if ((readCutBit(cuts, j) && cutMask[j]==1) || (!readCutBit(cuts,j) && cutMask[j]==0) ||
        //                    cutMask[j]==-1) {
        //                cutsExcl[sample][j] += gen_wgt * lumi * props.xsec / props.sum_gen_wgt;
        //                if (props.group != TString(""))
        //                    cutsGroupExcl[props.group][j] += gen_wgt * lumi * props.xsec / props.sum_gen_wgt;
        //            }
        //            else
        //                cumPass = false;
        //            if (((readCutBit(cuts, j) && cutMask[j]==1) || (!readCutBit(cuts,j) && cutMask[j]==0) || cutMask[j]==-1) && cumPass) {
        //                cutsIncl[sample][j] += gen_wgt * lumi * props.xsec / props.sum_gen_wgt;
        //                if (props.group != TString(""))
        //                    cutsGroupIncl[props.group][j] += gen_wgt * lumi * props.xsec / props.sum_gen_wgt;
        //            }
        //        }
        //    }
        //}
    }

    cout << std::setprecision(1) << std::fixed;

    cout << "Inclusive:" << endl;
    for (auto group : cutsGroupIncl)
        cout << " " << group.first;
    for (auto j : cutOrder) {
        cout << endl << "cut " << j;
        for (auto const & group : cutsGroupIncl) 
            cout << " & " << cutsGroupIncl[group.first][j];
        cout << " \\\\ ";
    }

    if (outfilename != TString("")) {
        std::ofstream outfile;
        outfile.open(outfilename.Data());
        outfile << "Inclusive:" << endl;
        for (auto group : cutsGroupIncl)
            outfile << " " << group.first;
        for (auto j : cutOrder) {
            outfile << endl << "cut " << j;
            for (auto const & group : cutsGroupIncl) 
                outfile << " & " << cutsGroupIncl[group.first][j];
            outfile << " \\\\ ";
        }
        outfile.close();
    }


    //cout << endl << endl << "Exclusive:" << endl;
    //for (auto group : cutsGroupExcl)
    //    cout << " " << group.first;
    //for (auto j : cutOrder) {
    //    cout << endl << "cut " << j;
    //    for (auto group : cutsGroupIncl) 
    //        cout << " & " << cutsGroupExcl[group.first][j];
    //    cout << " \\\\ ";
    //}

    cout << endl;
    printTimeElapsed(time_begin);

    return 0;
}
