#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using std::cout, std::endl, std::map, std::vector;

#include <TApplication.h>
#include <TChain.h>
#include <TCollection.h>
#include <TCut.h>
#include <TDatime.h>
#include <TH1F.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include "../utils/common.h"
using namespace common;
#include "../utils/json.hpp"
using json = nlohmann::json;

namespace macro {

    extern "C" bool process(map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {

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
        // Mask -- 1: require cut bit set; 0: require cut bit unset; -1:  cut bit can be anything
        vector<int> cutMask;
        std::string which_cutflow = "SR";
        if (which_cutflow == "SR")
            cutMask = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
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
            for (auto j : cutOrder) {
                if (cutMask[j] == 1)
                    cutInclPosMask |= (1 << j); 
                else if (cutMask[j] == 0)
                    cutInclNegMask |= (1 << j);
                //cutExclMask = 1 << j;

                // Handle categories cut (GBM-GBM, GBM-DSA, DSA-DSA)
                // Only want one of the three bits on at any given time
                if (j == 21) 
                    cutInclPosMask &= ~((1 << 22) | (1 << 23));
                else if (j == 22) 
                    cutInclPosMask &= ~((1 << 21) | (1 << 23));
                else if (j == 23) 
                    cutInclPosMask &= ~((1 << 21) | (1 << 22));

                data_reco->Draw("1",
                        TCut(Form("(gen_wgt * %f * %f / %f)", lumi, props.xsec, props.sum_gen_wgt))
                        * (TCut(Form("%d == (cuts&%d)", cutInclPosMask, cutInclPosMask))
                            && TCut(Form("0 == (cuts&%d)", cutInclNegMask))), // XOR
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

        if (cfg.find("outfilename") != cfg.end()) {
            std::ofstream outfile;
            outfile.open(cfg["outfilename"].get<std::string>());
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

        return 0;
    }

}
