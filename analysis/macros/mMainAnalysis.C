#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string.h>
#include <vector>
using std::cout, std::endl, std::map, std::vector;

#include <TH1F.h>
#include <TH2F.h>
#include <THStack.h>
#include <TString.h>

#include "../Selectors/RDataFrame/RDFAnalysis.h"
#include "../Selectors/RDataFrame/CosmicMuonAnalysis.h"
#include "../utils/cxxopts.hpp"
#include "../utils/common.h"
using namespace common;
#include "../utils/json.hpp"
using json = nlohmann::json;
#include "../utils/tdrstyle.h"

namespace macro {

    extern "C" bool process(map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {

        // for main analysis macro cuts vector needs to be non-empty
        if (cuts_info.size() == 0) {
            cout << "Error! Main analysis requires specification of cuts. None found. Returning..." << endl;
            return 0;
        }

        setTDRStyle();

        long long nEventsTotal = 0;

        map<TString, vector<Double_t>> cutsInclusive;
        map<TString, vector<Double_t>> cutsGroupInclusive;

        map<TString, THInfo*> histos_info;

        // macro options

        json plots_cfg = cfg["plots"];
        for (auto & plot : plots_cfg) 
            histos_info[plot["name"].get<std::string>()] = new THInfo{
                plot["quantity"].get<std::string>(),
                plot["groups_to_exclude"].get<std::vector<std::string>>(),
                plot["type"].get<std::string>(),
                plot["name"].get<std::string>(), 
                plot["title"].get<std::string>(),
                plot["nbinsX"].get<int>(),
                plot["lowX"].get<double>(),
                plot["highX"].get<double>(),
                // optional params
                ((plot.find("binEdgesX") != plot.end()) ? plot["binEdgesX"].get<std::vector<double>>() : std::vector<double>(1,-1)),
                plot.value("nMinus1CutDescription", ""),
                plot.value("quantity2", ""),
                plot.value("nbinsY", -1), 
                plot.value("lowY", -1.0),
                plot.value("highY", -1.0)
            };

        TString out_filename = TString(cfg["outfilename"].get<std::string>());

        int systematic_ = cfg["systematic"].get<int>();

        // THStack objects, indices: name of hist, mode (bkg/data/sig), cut number
        map<TString, map<common::MODE, map<int, vector<RDFResultPtr1D>>>> all_hstacks_1D; 
        map<TString, map<common::MODE, map<int, vector<RDFResultPtr2D>>>> all_hstacks_2D;
        
        //////********************* BEGIN OBSOLETE ***********************//////
        // THStack objects, indices: name of hist, mode (bkg/data/sig), cut number
        //map<TString, map<common::MODE, map<int, vector<ROOT::RDF::RResultPtr<TH1D>>>>> all_hstacks; 
        //for (auto & [name, info] : histos_info) {
        //    for (auto cut : info->cuts) {
        //        all_hstacks[name][common::BKG][cut] = vector<TH1*>();
        //        all_hstacks[name][common::DATA][cut] = vector<TH1*>();
        //        all_hstacks[name][common::SIGNAL][cut] = vector<TH1*>();
        //    }
        //}
        // This way just uses new CMake build system which already compiles the selector (see CMakeLists.txt)
        //mainAnalysisSelector * MCSelector = new mainAnalysisSelector();
        //mainAnalysisSelector * dataSelector = new mainAnalysisSelector();
        //MCSelector->SetMode(common::SIGNAL);
        //dataSelector->SetMode(common::DATA);
        //MCSelector->SetHistos(histos_info);
        //dataSelector->SetHistos(histos_info);
        //////********************* END OBSOLETE ***********************//////


        // Process each sample at a time (e.g. QCD_HT100-200)
        // TODO would be nice to make this parallel, but unclear how on top of RDF
        for (auto const & [sample, props] : samples) { 
            
            //bool isData = (props.sum_gen_wgt < 0);
            bool isData = (props.mode == common::DATA);
            bool doSubsetOnly = (props.limit_num_files != -1);

            cout << endl << "Sample: " << sample << endl;
            if (props.sum_gen_wgt < 1e-10 && !isData) continue; // don't have data yet for these samples, continue

            TChain * data_reco = new TChain("ntuples_gbm/recoT");
            TChain * data_gen = new TChain("ntuples_gbm/genT");
            int count = 0;
            for (auto const & filename : props.filenames) {
                if (count++ >= props.limit_num_files && doSubsetOnly) break;
                data_reco->Add(filename.Data());
                data_gen->Add(filename.Data());
            }

            //////********************* BEGIN OBSOLETE ***********************//////
            //mainAnalysisSelector * currentSelector;
            //if (!isData)
            //    currentSelector = MCSelector;
            //else
            //    currentSelector = dataSelector;
            //currentSelector->SetHistos(histos_info);
            //currentSelector->SetParams(props,/* lumi,*/ region); // obsolete
            //data_reco->Process(currentSelector); --> obsolete, use RDataFrame instead
            //vector<double> cutflow = currentSelector->GetCutflow();
            //map<TString, map<int, TH1*>> sample_histos = currentSelector->GetHistograms();
            //////********************* END OBSOLETE ***********************//////

            if (!isData)
                data_reco->AddFriend(data_gen);

            TString analysis_type = "main";
            if (cfg.find("analysis") != cfg.end())
                analysis_type = cfg["analysis"];

            RDFAnalysis * dfAnalysis = new RDFAnalysis();
            CosmicMuonAnalysis * dfAnalysis_cosmics = new CosmicMuonAnalysis();

            vector<RDFResultPtrSumType_d> cutflow_ptr;
            map<TString, map<int, RDFResultPtr1D>> all_sample_histos_1D;
            map<TString, map<int, RDFResultPtr2D>> all_sample_histos_2D;

            if (analysis_type == TString("main")) {
                    if (!dfAnalysis->Begin(systematic_))
                        return false;
                    dfAnalysis->SetMacroConfig(cfg);
                    dfAnalysis->SetHistoConfig(histos_info);
                    dfAnalysis->SetCutConfig(cuts_info);
                    dfAnalysis->SetSampleConfig(props);
                    dfAnalysis->Process(data_reco);
                    nEventsTotal += dfAnalysis->GetNEvents();
                    cutflow_ptr = dfAnalysis->GetCutflow();
                    all_sample_histos_1D = dfAnalysis->GetHistograms1D();
                    all_sample_histos_2D = dfAnalysis->GetHistograms2D();
            }
            else if (analysis_type == TString("cosmics")) {
                    dfAnalysis_cosmics->Begin();
                    dfAnalysis_cosmics->SetMacroConfig(cfg);
                    dfAnalysis_cosmics->SetHistoConfig(histos_info);
                    dfAnalysis_cosmics->SetCutConfig(cuts_info);
                    dfAnalysis_cosmics->SetSampleConfig(props);
                    dfAnalysis_cosmics->Process(data_reco);
                    cutflow_ptr = dfAnalysis_cosmics->GetCutflow();
                    all_sample_histos_1D = dfAnalysis_cosmics->GetHistograms1D();
                    all_sample_histos_2D = dfAnalysis_cosmics->GetHistograms2D();
            }
            else {
                cout << "ERROR! Unknown analysis type requested. Exiting..." << endl;
                return 0;
            }

            vector<double> cutflow;
            for (auto cut : cutflow_ptr)
                cutflow.push_back(cut.GetValue());

            // if this is first sample of group, create entry
            if (cutsGroupInclusive.find(props.group) == cutsGroupInclusive.end())
                cutsGroupInclusive.insert(std::make_pair(props.group, cutflow));
            else // otherwise, sum std::vectors element-wise
                std::transform(cutsGroupInclusive[props.group].begin(), cutsGroupInclusive[props.group].end(), 
                        cutflow.begin(), cutsGroupInclusive[props.group].begin(), std::plus<double>());

            for (auto & [histo_name, histos_by_cut] : all_sample_histos_2D) {
                for (auto & [cut, histo] : histos_by_cut) {
                    bool kFound = false;
                    for (auto existing_histo : all_hstacks_2D[histo_name][props.mode][cut]) {
                        if (TString(existing_histo->GetName()) == TString(histo->GetName())) { // hist already exists in stack
                            histo->Sumw2();
                            existing_histo->Add(histo.GetPtr());
                            kFound = true;
                        }
                    }
                    if (!kFound) { // first time hist is referenced, so add to stack
                        histo->Sumw2();
                        histo->SetDirectory(0);
                        all_hstacks_2D[histo_name][props.mode][cut].push_back(histo);
                    }
                }
            }
            for (auto & [histo_name, histos_by_cut] : all_sample_histos_1D) {
                for (auto & [cut, histo] : histos_by_cut) {
                    bool kFound = false;
                    for (auto existing_histo : all_hstacks_1D[histo_name][props.mode][cut]) {
                        if (TString(existing_histo->GetName()) == TString(histo->GetName())) { // hist already exists in stack
                            histo->Sumw2();
                            existing_histo->Add(histo.GetPtr());
                            kFound = true;
                        }
                    }
                    if (!kFound) { // first time hist is referenced, so add to stack
                        histo->Sumw2();
                        histo->SetDirectory(0);
                        if (props.mode == common::BKG) {
                            if (common::group_plot_info.find(props.group) != common::group_plot_info.end())
                                histo->SetFillColor(common::group_plot_info[props.group].color);
                            else
                                histo->SetFillColorAlpha(kBlue, 0.35);
                        }
                        else if (props.mode == common::DATA) {
                            if (common::group_plot_info.find(props.group) != common::group_plot_info.end()) {
                                histo->SetMarkerColor(common::group_plot_info[props.group].color);
                                histo->SetMarkerStyle(common::group_plot_info[props.group].style);
                            }
                            else {
                                histo->SetMarkerColor(kBlack);
                                histo->SetMarkerStyle(kSolid);
                            }
                            histo->SetMarkerSize(0.9);
                        }
                        else if (props.mode == common::SIGNAL) {
                            if (common::group_plot_info.find(props.group) != common::group_plot_info.end()) {
                                histo->SetLineColor(common::group_plot_info[props.group].color);
                                histo->SetLineStyle(common::group_plot_info[props.group].style);
                            }
                            else {
                                histo->SetLineColor(kBlack);
                                histo->SetLineStyle(kDashed);
                            }
                            histo->SetLineWidth(2);
                            histo->SetMarkerSize(0);
                        }
                        all_hstacks_1D[histo_name][props.mode][cut].push_back(histo);
                    }
                }
            }

            if (analysis_type == TString("main"))
                dfAnalysis->Terminate();
            else
                dfAnalysis_cosmics->Terminate();
            delete dfAnalysis;
            delete dfAnalysis_cosmics;
            delete data_gen;
            delete data_reco;
        }

        // Save histograms into THStack objects

        TFile * outfile = new TFile(out_filename, "RECREATE");
        
        //sort by "smallest integral first"
        auto sortHists = [](auto a_hist_ptr, auto b_hist_ptr) { return a_hist_ptr->Integral() < b_hist_ptr->Integral(); };

        for (auto & [plot_name, modes] : all_hstacks_1D) {
            for (auto & [mode, cuts] : modes) {
                for (auto & [cut, hist_vec] : cuts) {
                    THStack * hstack;
                    if (mode == common::BKG)
                        hstack = new THStack(Form("%s_cut%02d-BKG", plot_name.Data(), cut), histos_info[plot_name]->title);
                    else if (mode == common::DATA)
                        hstack = new THStack(Form("%s_cut%02d-DATA", plot_name.Data(), cut), histos_info[plot_name]->title);
                    else if (mode == common::SIGNAL)
                        hstack = new THStack(Form("%s_cut%02d-SIGNAL", plot_name.Data(), cut), histos_info[plot_name]->title);
                    
                    std::sort(hist_vec.begin(), hist_vec.end(), sortHists);

                    for (auto hist : hist_vec)
                        hstack->Add(hist.GetPtr());

                    if (hstack->GetNhists() == 0)
                        continue;

                    hstack->Write();

                    if (!cuts_info[cut].efficiency.Contains("none")) {
                        THStack * hstack2 = new THStack(*hstack);
                        if (mode == common::BKG)
                            hstack2->SetName(Form("%s_%scut%02d-BKG", plot_name.Data(), cuts_info[cut].efficiency.Data(), cut));
                        else if (mode == common::DATA)
                            hstack2->SetName(Form("%s_%scut%02d-DATA", plot_name.Data(), cuts_info[cut].efficiency.Data(), cut));
                        else if (mode == common::SIGNAL)
                            hstack2->SetName(Form("%s_%scut%02d-SIGNAL", plot_name.Data(), cuts_info[cut].efficiency.Data(), cut));
                        hstack2->Write();
                    }
                }
            }
        }
        for (auto & [plot_name, modes] : all_hstacks_2D) {
            for (auto & [mode, cuts] : modes) {
                for (auto & [cut, hist_vec] : cuts) {
                    THStack * hstack;
                    if (mode == common::BKG)
                        hstack = new THStack(Form("%s_cut%02d-BKG", plot_name.Data(), cut), histos_info[plot_name]->title);
                    else if (mode == common::DATA)
                        hstack = new THStack(Form("%s_cut%02d-DATA", plot_name.Data(), cut), histos_info[plot_name]->title);
                    else if (mode == common::SIGNAL)
                        hstack = new THStack(Form("%s_cut%02d-SIGNAL", plot_name.Data(), cut), histos_info[plot_name]->title);
                    
                    std::sort(hist_vec.begin(), hist_vec.end(), sortHists);

                    for (auto hist : hist_vec)
                        hstack->Add(hist.GetPtr());

                    if (hstack->GetNhists() == 0)
                        continue;
                    
                    hstack->Write();

                    if (!cuts_info[cut].efficiency.Contains("none")) {
                        THStack * hstack2 = new THStack(*hstack);
                        if (mode == common::BKG)
                            hstack2->SetName(Form("%s_%scut%02d-BKG", plot_name.Data(), cuts_info[cut].efficiency.Data(), cut));
                        else if (mode == common::DATA)
                            hstack2->SetName(Form("%s_%scut%02d-DATA", plot_name.Data(), cuts_info[cut].efficiency.Data(), cut));
                        else if (mode == common::SIGNAL)
                            hstack2->SetName(Form("%s_%scut%02d-SIGNAL", plot_name.Data(), cuts_info[cut].efficiency.Data(), cut));
                        hstack2->Write();
                    }
                }
            }
        }

        outfile->Close();

        // Print Latex cutflow table to file

        out_filename.ReplaceAll(".root", "_cutflow.txt");
        std::ofstream cutflow_file(out_filename.Data());

        cutflow_file << "Cut# & Description & Data & Bkg";
        for (auto const & [group, all_cut_numbers] : cutsGroupInclusive)
            if (group != TString("data"))
                cutflow_file << " & " << group;

        auto first_vec = cutsGroupInclusive.begin();
        size_t vec_size = (cutsGroupInclusive.size() > 0) ? first_vec->second.size() : 0;

        for (size_t cut = 0; cut < vec_size; cut++) {
            cutflow_file << endl << "cut" << cut << " & " << cuts_info[cut].description;

            Double_t total_background = 0.0;
            for (auto const & [group, all_cut_numbers] : cutsGroupInclusive)
                if (group != TString("data") && (!group.Contains("sig_"))) // only sum bkgs
                    total_background += all_cut_numbers[cut];

            Double_t total_data = (cutsGroupInclusive.find("data") != cutsGroupInclusive.end()) ? 
                cutsGroupInclusive["data"][cut] : 0.0;
            cutflow_file << " & " << total_data;
            cutflow_file << " & " << total_background;

            for (auto const & [group, all_cut_numbers] : cutsGroupInclusive)
                if (group != TString("data"))
                    cutflow_file << " & " << all_cut_numbers[cut];

            cutflow_file << " \\\\ ";
        }

        cutflow_file.close();
        cutflow_file.clear();

        // Print csv cutflow table to file and cout

        out_filename.ReplaceAll(".txt", ".csv");
        cutflow_file.open(out_filename.Data());

        cout << endl << "Inclusive cutflow:" << endl;
        cout << "Cut# Description Data Bkg";
        cutflow_file << "Cut# Description Data Bkg";
        for (auto const & [group, all_cut_numbers] : cutsGroupInclusive) {
            if (group == TString("data")) continue;
            cout << " " << group;
            cutflow_file << ", " << group;
        }

        first_vec = cutsGroupInclusive.begin();
        vec_size = (cutsGroupInclusive.size() > 0) ? first_vec->second.size() : 0;

        cout << std::setprecision(1) << std::fixed;
        for (size_t cut = 0; cut < vec_size; cut++) {
            cout << endl << "cut" << cut << " " << "\"" << cuts_info[cut].description << "\"";
            cutflow_file << endl << "cut" << cut << " " << "\"" << cuts_info[cut].description << "\"";

            Double_t total_background = 0.0;
            for (auto const & [group, all_cut_numbers] : cutsGroupInclusive)
                if (group != TString("data") && (!group.Contains("sig_"))) // only sum bkgs
                    total_background += all_cut_numbers[cut];

            Double_t total_data = (cutsGroupInclusive.find("data") != cutsGroupInclusive.end()) ? 
                cutsGroupInclusive["data"][cut] : 0.0;
            cutflow_file << ", " << total_data;
            cutflow_file << ", " << total_background;
            cout << " " << total_data;
            cout << " " << total_background;

            for (auto const & [group, all_cut_numbers] : cutsGroupInclusive) {
                if (group == TString("data")) continue;
                cout << " " << all_cut_numbers[cut];
                cutflow_file << ", " << all_cut_numbers[cut];
            }
        }

        cout << endl << endl << "Total number of events processed by mMainAnalysis: " << nEventsTotal << endl;
        return 0;
    }
}