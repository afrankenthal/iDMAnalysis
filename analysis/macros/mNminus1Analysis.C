#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string.h>
#include <vector>
using std::cout, std::endl, std::map, std::vector;

#include <TApplication.h>
#include <TChain.h>
#include <TCollection.h>
#include <TCut.h>
#include <TDatime.h>
#include <TH1F.h>
#include <TH2F.h>
#include <THStack.h>
#include <TSelector.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include "../utils/common.h"
using namespace common;
#include "../utils/cxxopts.hpp"
#include "../utils/json.hpp"
using json = nlohmann::json;
#include "../utils/tdrstyle.h"
#include "../Selectors/RDataFrame/nMinus1Selector.h"

namespace macro {

    extern "C" bool process(map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {

        // for main analysis macro cuts vector needs to be non-empty
        if (cuts_info.size() == 0) {
            cout << "Error! Main analysis requires specification of cuts. None found. Returning..." << endl;
            return 0;
        }

        setTDRStyle();

        map<TString, THInfo*> histos_info;

        // macro options

        json plots_cfg = cfg["plots"];
        for (auto & plot : plots_cfg) 
            histos_info[plot["name"].get<std::string>()] = new THInfo{
                    plot["quantity"].get<std::string>(),
                    plot["groups"].get<std::vector<std::string>>(),
                    plot["type"].get<std::string>(),
                    plot["name"].get<std::string>(), 
                    plot["title"].get<std::string>(),
                    plot["nbinsX"].get<int>(),
                    plot["lowX"].get<double>(),
                    plot["highX"].get<double>(),
                    // optional params --> 2D plot
                    ((plot.find("binEdgesX") != plot.end()) ? plot["binEdgesX"].get<std::vector<double>>() : std::vector<double>(1,-1)),
                    plot.value("nMinus1CutDescription", ""),
                    plot.value("quantity2", ""),
                    plot.value("nbinsY", -1), 
                    plot.value("lowY", -1.0),
                    plot.value("highY", -1.0)
                    };

        TString out_filename = TString(cfg["outfilename"].get<std::string>());

        map<TString, map<common::MODE, vector<TH1*>>> all_hstacks; // THStack objects, indices: name of hist, mode (bkg/data/sig)
        for (auto & [name, info] : histos_info) {
            all_hstacks[name][common::BKG] = vector<TH1*>();
            all_hstacks[name][common::DATA] = vector<TH1*>();
            all_hstacks[name][common::SIGNAL] = vector<TH1*>();
        }

        //Float_t lumi = 14.7 * 1000; // 1/pb
        Float_t lumi = 59.74 * 1000; // 1/pb
        //Float_t lumi = 29.41 * 1000; // 1/pb


        // Process each sample at a time (e.g. QCD_HT100-200)
        for (auto const & [sample, props] : samples) { 
            //bool isData = (props.sum_gen_wgt < 0);
            bool isData = (props.mode == common::DATA);
            bool doSubsetOnly = (props.limit_num_files != -1);

            cout << "Sample: " << sample << endl;
            if (props.sum_gen_wgt < 0.1 && !isData) continue; // don't have data yet for these samples, continue

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

            std::unique_ptr<nMinus1Selector> dfAnalysis = std::make_unique<nMinus1Selector>();//new nMinus1Selector();
            dfAnalysis->SetHistos(histos_info);
            dfAnalysis->SetCuts(cuts_info);
            dfAnalysis->SetParams(props, lumi);

            dfAnalysis->Process(data_reco);

            map<TString, ROOT::RDF::RResultPtr<TH1D>> all_sample_histos_1D = dfAnalysis->GetHistograms1D();
            map<TString, ROOT::RDF::RResultPtr<TH2D>> all_sample_histos_2D = dfAnalysis->GetHistograms2D();
            for (auto & [histo_name, histo] : all_sample_histos_2D) {
                bool kFound = false;
                for (TH1 * existing_histo : all_hstacks[histo_name][props.mode]) {
                    if (TString(existing_histo->GetName()) == TString(histo->GetName())) { // hist already exists in stack
                        histo->Sumw2();
                        existing_histo->Add(histo.GetPtr());
                        kFound = true;
                    }
                }
                if (!kFound) { // first time hist is referenced, so add to stack
                    TH1 * h_pointer = (TH1*)(histo.GetPtr())->Clone();
                    h_pointer->Sumw2();
                    h_pointer->SetDirectory(0);
                    all_hstacks[histo_name][props.mode].push_back(h_pointer);
                }
            }
            for (auto & [histo_name, histo] : all_sample_histos_1D) {
                bool kFound = false;
                for (TH1 * existing_histo : all_hstacks[histo_name][props.mode]) {
                    if (TString(existing_histo->GetName()) == TString(histo->GetName())) { // hist already exists in stack
                        histo->Sumw2();
                        existing_histo->Add(histo.GetPtr());
                        kFound = true;
                    }
                }
                if (!kFound) { // first time hist is referenced, so add to stack
                    TH1 * h_pointer = (TH1*)(histo.GetPtr())->Clone();
                    h_pointer->Sumw2();
                    h_pointer->SetDirectory(0);
                    std::cout << "hist nentries: " << h_pointer->GetEntries() << std::endl;
                    if (props.mode == common::BKG) {
                        h_pointer->SetFillColor(common::group_plot_info[props.group].color);
                    }
                    else if (props.mode == common::DATA) {
                        h_pointer->SetMarkerColor(common::group_plot_info[props.group].color);
                        h_pointer->SetMarkerStyle(common::group_plot_info[props.group].style);
                        h_pointer->SetMarkerSize(0.9);
                    }
                    else if (props.mode == common::SIGNAL) {
                        h_pointer->SetLineColor(common::group_plot_info[props.group].color);
                        h_pointer->SetLineStyle(common::group_plot_info[props.group].style);
                        h_pointer->SetLineWidth(2);
                        h_pointer->SetMarkerSize(0);
                    }
                    all_hstacks[histo_name][props.mode].push_back(h_pointer);
                }
            }
        }

        // Save histograms into THStack objects

        TFile * outfile = new TFile(out_filename, "RECREATE");
        for (auto & [plot_name, modes] : all_hstacks) {
            for (auto & [mode, hist_vec] : modes) {
                THStack * hstack;
                if (mode == common::BKG)
                    hstack = new THStack(Form("%s_-BKG", plot_name.Data()), histos_info[plot_name]->title);
                else if (mode == common::DATA)
                    hstack = new THStack(Form("%s_-DATA", plot_name.Data()), histos_info[plot_name]->title);
                else if (mode == common::SIGNAL)
                    hstack = new THStack(Form("%s_-SIGNAL", plot_name.Data()), histos_info[plot_name]->title);
                //sort by "smallest integral first"
                std::sort(hist_vec.begin(), hist_vec.end(),
                        [](TH1 *a, TH1 *b) { return a->Integral() < b->Integral(); });
                for (auto hist : hist_vec)
                    hstack->Add(hist);
                if (hstack->GetNhists() > 0)
                    hstack->Write();
            }
        }
        outfile->Close();

        return 0;
    }
}
