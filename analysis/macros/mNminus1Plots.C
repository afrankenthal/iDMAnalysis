#include <TCanvas.h>
#include <TCut.h>
#include <TChain.h>
#include <TH1F.h>
#include <THStack.h>
#include <TLegend.h>
#include <TPaveText.h>

#include "../utils/common.h"
using namespace common;
#include "../utils/json.hpp"
using json = nlohmann::json;
#include "../utils/tdrstyle.h"

using std::cout, std::endl, std::map, std::vector;

namespace macro {

    extern "C" bool process(map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) { 
        bool fSave = cfg["fSave"].get<bool>();

        vector<int> colors{kBlack, kRed, kMagenta, kBlue,
            kCyan, kGreen, kYellow+1};

        typedef struct GroupPlotProps {
            TString legend;
            Int_t color;
            Int_t style;
        } GroupPlotProps;

        map<TString, GroupPlotProps> group_plot_props {
            {"DY", GroupPlotProps{"Z(ll)+Jets", 2, 1}},
            {"QCD", GroupPlotProps{"QCD", 3, 1}},
            {"Multiboson", GroupPlotProps{"Di-/Tri-boson", 6, 1}},
            {"ZJets", GroupPlotProps{"Z(#nu#nu)+Jets", 800, 1}},
            {"WJets", GroupPlotProps{"W(l#nu)+Jets", 41, 1}},
            {"Top", GroupPlotProps{"t#bar{t}, tW", 4, 1}},
            {"5p25_100", GroupPlotProps{"5,5.5 GeV, c#tau=100mm", 1, 2}},
            {"60p0_1", GroupPlotProps{"50,70 GeV, c#tau=1mm", 2, 2}}
        };

        //gROOT->LoadMacro("tdrstyle.C");
        setTDRStyle();

        for (auto const & [sample, props] : samples) {
            cout << sample << endl;
            if (!props.filenames.empty())
                cout << props.filenames.at(0) << endl;
        }

        map<TString, map<TString, TH1F *>> hists;
        int numCuts_ = 19;

        typedef struct CutInfo {
            TCut tcut;
            TString plot_var;
            TString plot_description;
            Int_t nbins;
            Float_t lowX;
            Float_t highX;
            TString label;
            vector<TString> sample_list;
        } CutInfo;

        vector<TCut> bits;
        for (int bit = 0; bit < numCuts_; bit++)
            bits.push_back(TCut(Form("(cuts & (1 << %d))", bit)));

        TCut noSelection = "";
        TCut muon1 = bits[6] && bits[7] && bits[8] && bits[9] && bits[10];
        TCut muon2 = bits[11] && bits[12] && bits[13] && bits[14] && bits[15];
        TCut initialSelection = bits[1] && bits[2] && bits[3] && muon1;
        TCut nMinusDr = initialSelection && bits[4] && bits[5] && muon2 && bits[17] && bits[18];
        TCut nMinusMmumu = initialSelection && bits[4] && bits[5] && muon2 && bits[16] && bits[18];
        TCut nMinusNJets = initialSelection && muon2 && bits[16] && bits[17] && bits[18];
        TCut nMinusGoodMu = bits[1] && bits[2] && bits[3] && bits[4] && bits[5];
        TCut nMinusDPhiJetMet = bits[1] && bits[2] && bits[3]; // noSelection; // initialSelection; // && bits[4] && muon2 && bits[15] && bits[16];

        //TCut signalRegion = initialSelection && "cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && cutsVec[14]";
        //TCut controlRegion_QCD = initialSelection && "!cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && cutsVec[14]";
        //TCut controlRegion_EW = initialSelection && "cutsVec[3] && !cutsVec[9] && !cutsVec[10] && !cutsVec[11] && !cutsVec[12] && !cutsVec[14]";
        //TCut controlRegion_TTbar = initialSelection && "cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && !cutsVec[14]";

        vector<TString> groups_to_plot{"5p25_100", "60p0_1", "Multiboson", "QCD", "DY", "Top", "ZJets", "WJets"};

        map<TString, CutInfo> cuts{};
        cuts["nMinusDr"] = CutInfo{nMinusDr, "reco_vtx_dR", "Di-muon dR", 10, 0, 6, "N-1 (dR)", groups_to_plot};
        cuts["nMinusMmumu"] = CutInfo{nMinusMmumu, "reco_Mmumu", "M_{#mu#mu} [GeV]", 60, 0, 120, "N-1 (M_{#mu#mu})", groups_to_plot};
        cuts["nMinusNJets"] = CutInfo{nMinusNJets, "reco_PF_n_highPt_jets", "Number of high pT (> 30 GeV) jets", 9, 1, 10, "N-1 (nJets)", groups_to_plot};
        cuts["nMinusGoodMu"] = CutInfo{nMinusGoodMu, "reco_n_good_dsa", "Number of quality dSA muons", 9, 1, 10, "N-1 (nGoodMuons)", groups_to_plot};
        cuts["nMinusDPhiJetMet"] = CutInfo{nMinusDPhiJetMet, "reco_PF_METjet_dphi", "#Delta#Phi(leading jet, MET)", 20, -3.2, 3.2, "N-1 (dPhiJetMet)", groups_to_plot};

        map<TString, TCanvas *> canvases;

        for (auto const & [sample, props] : samples) {
            int count = 0;
            TChain * data_gen = new TChain("ntuples_gbm/genT");
            TChain * data_reco = new TChain("ntuples_gbm/recoT");
            for (auto const & filename : props.filenames) {
                if (count++ >= props.limit_num_files && props.limit_num_files != -1) continue;
                data_gen->Add(filename);
                data_reco->Add(filename);
            }
            Float_t gen_wgt;
            data_gen->SetBranchAddress("gen_wgt", &gen_wgt);
            data_gen->GetEntries();
            data_reco->GetEntries();
            data_gen->UseCurrentStyle();
            data_reco->UseCurrentStyle();
            data_reco->AddFriend(data_gen, "ntuples_gbm/genT");

            cout << "sample: " << sample << ", sum_gen_wgt: " << props.sum_gen_wgt << endl;

            float lumi = 59.97 * 1000; // 1/pb

            for (auto const & [cut_name, cut] : cuts) {
                if (std::find(cut.sample_list.begin(), cut.sample_list.end(), sample) != cut.sample_list.end()) {
                    if (hists[sample].find(cut_name) == hists[sample].end())
                        hists[sample][cut_name] = new TH1F(Form("h%s_%s", cut_name.Data(), sample.Data()), "", cut.nbins, cut.lowX, cut.highX);
                    data_reco->Draw(Form("%s>>+h%s_%s", cut.plot_var.Data(), cut_name.Data(), sample.Data()), cut.tcut, "goff");
                    hists[sample][cut_name] = (TH1F*)gDirectory->Get(Form("h%s_%s", cut_name.Data(), sample.Data()));
                }
                else if (std::find(cut.sample_list.begin(), cut.sample_list.end(), props.group) != cut.sample_list.end()) {
                    if (hists[props.group].find(cut_name) == hists[props.group].end())
                        hists[props.group][cut_name] = new TH1F(Form("h%s_%s", cut_name.Data(), props.group.Data()), "", cut.nbins, cut.lowX, cut.highX);
                    data_reco->Draw(Form("%s>>+h%s_%s", cut.plot_var.Data(), cut_name.Data(), props.group.Data()), cut.tcut * Form("gen_wgt * %f * %f / %f", lumi, props.xsec, props.sum_gen_wgt), "goff");
                }
            }
        }

        for (auto const & [cut_name, cut] : cuts) {
            THStack * hs = new THStack(Form("%s", cut_name.Data()), Form("%s", cut.label.Data()));
            auto legend = new TLegend(0.45,0.7,0.85,0.9);
            legend->SetFillStyle(0);
            //auto legend = new TLegend(0.25,0.2);
            int color = 0;
            for (auto & sample : cut.sample_list) {
                if (hists[sample].find(cut_name) == hists[sample].end()) continue;
                if (hists[sample][cut_name]->Integral() < 0.00001) continue;
                hists[sample][cut_name]->Scale(1/hists[sample][cut_name]->Integral(), "width");
                //if (samples.find(sample) != samples.end()) {
                //    hists[sample][cut_name]->SetLineColor(samples[sample].color);
                //    hists[sample][cut_name]->SetLineStyle(samples[sample].style);
                //}
                //else { // group instead
                    hists[sample][cut_name]->SetLineColor(group_plot_props[sample].color);
                    hists[sample][cut_name]->SetLineStyle(group_plot_props[sample].style);
                    //hists[sample][cut_name]->SetLineColor(colors[color++]);
                //}
                hists[sample][cut_name]->SetLineWidth(2);
                hs->Add(hists[sample][cut_name]);
                //if (samples.find(sample) != samples.end())
                //    legend->AddEntry(hists[sample][cut_name], samples[sample].label.Data());
                //else // group instead
                    legend->AddEntry(hists[sample][cut_name], group_plot_props[sample].legend);
            }

            if (hs->GetNhists() == 0) continue;

            canvases[cut_name] = new TCanvas();
            canvases[cut_name]->cd();
            hs->Draw("E HIST nostack");
            TPaveText *text = new TPaveText(0.25,0.8,0.4,1.0, "NDC");
            text->AddText(Form("%s", cut.label.Data()));
            text->SetBorderSize(0);
            text->SetFillColor(0);
            text->SetTextFont(53); 
            text->SetTextSize(20);
            text->SetFillStyle(0);
            hs->GetXaxis()->SetTitle(Form("%s", cut.plot_description.Data()));
            hs->GetYaxis()->SetTitle("A. U.");
            hs->GetYaxis()->SetTitleOffset(1.4);
            gPad->Modified();
            legend->Draw();
            text->Draw();
            if (fSave) {
                canvases[cut_name]->SaveAs(Form("temp_plots/%s_%s.pdf", cut.plot_var.Data(), cut_name.Data()));
                canvases[cut_name]->SaveAs(Form("temp_plots/%s_%s.png", cut.plot_var.Data(), cut_name.Data()));
                canvases[cut_name]->SaveAs(Form("temp_plots/%s_%s.root", cut.plot_var.Data(), cut_name.Data()));
                canvases[cut_name]->SaveAs(Form("temp_plots/%s_%s.C", cut.plot_var.Data(), cut_name.Data()));
            }
        }
        return 0;
    }

} // namespace macro
