#include <string.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include <TDatime.h>
#include <TCollection.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>
#include <TString.h>
#include <TChain.h>
#include <TApplication.h>
#include <TCut.h>
#include <TH1F.h>
#include <TSelector.h>

#include "utils/common.C"
using namespace common;
#include "utils/json.hpp"
using json = nlohmann::json;
#include "utils/cxxopts.hpp"
#include "TSelectors/mainAnalysisSelectorData.h"
#include "TSelectors/mainAnalysisSelectorMC.h"

using std::cout, std::endl, std::map, std::vector;

namespace macro {

    bool mMainAnalysis(map<TString, SampleInfo> samples, json cfg) {
        setTDRStyle();

        map<TString, vector<Double_t>> cutsInclusive;
        map<TString, vector<Double_t>> cutsGroupInclusive;

        map<TString, TH1Info*> histos_info;

        // macro options

        json plots_cfg = cfg["plots"];
        for (auto & plot : plots_cfg) 
            histos_info[plot["name"].get<std::string>()] = new TH1Info{
                    plot["quantity"].get<std::string>(),
                    plot["groups"].get<std::vector<std::string>>(),
                    plot["cuts"].get<std::vector<int>>(),
                    plot["name"].get<std::string>(), 
                    plot["title"].get<std::string>(),
                    plot["nbins"].get<int>(),
                    plot["lowX"].get<double>(),
                    plot["highX"].get<double>()
                    };

        TString out_filename = TString(cfg["outfilename"].get<std::string>());
        TString region = TString(cfg["region"].get<std::string>());
        //TString custom_cut = TString(cfg["param"].get<std::string>());
        //Float_t dR_cut = cfg["value"].get<float>(); 
        //Int_t dR_cut_mask = 1, nHighPtJets_cut_mask = 1;
        //if (custom_cut.Contains("dR"))
        //    dR_cut_mask = 2;
        //else 
        //    nHighPtJets_cut_mask = 2;

        map<TString, map<common::MODE, map<int, THStack*>>> all_hstacks; // THStack objects, indices: name of hist, mode (bkg/data/sig), cut number
        for (auto & [name, info] : histos_info) {
            for (auto cut : info->cuts) {
                all_hstacks[name][common::BKG][cut] = new THStack(Form("%s_cut%d-BKG", name.Data(), cut), info->title);
                all_hstacks[name][common::DATA][cut] = new THStack(Form("%s_cut%d-DATA", name.Data(), cut), info->title);
                all_hstacks[name][common::SIGNAL][cut] = new THStack(Form("%s_cut%d-SIG", name.Data(), cut), info->title);
            }
        }

        Float_t lumi = 59.97 * 1000; // 1/pb

        // TODO re-run latest ntuplizer with MC so can have single selector class
        mainAnalysisSelectorMC * MCSelector = (mainAnalysisSelectorMC*)TSelector::GetSelector("TSelectors/mainAnalysisSelectorMC.C+");
        mainAnalysisSelectorData * dataSelector = (mainAnalysisSelectorData*)TSelector::GetSelector("TSelectors/mainAnalysisSelectorData.C+");

        MCSelector->SetHistos(histos_info);

        for (auto const & [sample, props] : samples) {
            bool isData = (props.sum_gen_wgt < 0);
            bool doSubsetOnly = (props.limit_num_files != -1);

            cout << "sample: " << sample << endl; //", sum_gen_wgt: " << props.sum_gen_wgt << endl;
            if (props.sum_gen_wgt < 0.1 && !isData) continue;

            TChain * data_reco = new TChain("ntuples_gbm/recoT");
            TChain * data_gen = new TChain("ntuples_gbm/genT");
            int count = 0;
            for (auto const & filename : props.filenames) {
                if (count++ >= props.limit_num_files && doSubsetOnly) continue;
                data_reco->Add(filename.Data());
                data_gen->Add(filename.Data());
            }

            if (!isData)
                data_reco->AddFriend(data_gen);

            vector<double> cutflow;

            if (!isData) { // MC

                MCSelector->SetParams(props, lumi, region);
                data_reco->Process(MCSelector);

                cutflow = MCSelector->GetCutflow();
                cutsInclusive.insert(std::make_pair(sample, cutflow));
                if (cutsGroupInclusive.find(props.group) == cutsGroupInclusive.end())
                    cutsGroupInclusive.insert(std::make_pair(props.group, cutflow));
                else
                    std::transform(cutsGroupInclusive[props.group].begin(), cutsGroupInclusive[props.group].end(), cutflow.begin(), cutsGroupInclusive[props.group].begin(), std::plus<double>( )); // sum std::vectors element-wise

                map<TString, map<int, TH1F*>> sample_histos = MCSelector->GetHistograms();
                for (auto & [hist_name, hists] : sample_histos) {
                    for (auto & [cut, hist] : hists) {
                        TList * hist_list = all_hstacks[hist_name][props.mode][cut]->GetHists();
                        bool kFound = false;
                        TIter next(hist_list);
                        TH1F * h; 
                        while ((h=(TH1F*)next())) {
                            if (TString(h->GetName()) == TString(hist->GetName())) { // hist already exists in stack
                                h->Add(hist);
                                kFound = true;
                            }
                        }
                        if (!kFound) // first time hist is filled, so add to stack
                            all_hstacks[hist_name][props.mode][cut]->Add(hist);
                    }
                }
            }
            else { // data
                
                data_reco->Process(dataSelector);//"mainAnalysisSelectorData.C+");

                //cutflow = DataSelector->GetCutflow();
            }
        }

        cout << "Inclusive cutflow:" << endl;
        cout << std::setprecision(1) << std::fixed;
        for (auto group : cutsGroupInclusive)
            cout << " " << group.first;
        auto first = cutsGroupInclusive.begin();
        size_t vec_size = first->second.size();
        for (size_t cut = 0; cut < vec_size; cut++) {
            cout << endl << "cut " << cut;
            for (auto const & group : cutsGroupInclusive) 
                cout << " & " << cutsGroupInclusive[group.first][cut];
            cout << " \\\\ ";
        }

        TFile * outfile = new TFile(out_filename, "RECREATE");
        for (auto & [plot_name, modes] : all_hstacks)
            for (auto & [mode, cuts] : modes)
                for (auto & [cut, stack] : cuts) 
                    if (stack->GetNhists() > 0)
                        stack->Write();
        outfile->Close();

        return 0;
    }
}
