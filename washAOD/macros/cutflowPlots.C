#include "tdrstyle.C"

void cutflowPlots(std::string variable, std::string description, int nbins, float lowx, float highx, bool fSave=false) {
    //gROOT->LoadMacro("tdrstyle.C");
    //setTDRStyle();

    std::map<std::string, int> colors { {"5p25", kRed+1}, {"52p5", kBlue+1}, {"6p0", kGreen+1}, {"60p0", kBlack} };
    std::map<std::string, std::string> labels { 
        {"5p25", "m1 = 5 GeV, #Delta = 0.1"},
        {"52p5", "m1 = 50 GeV, #Delta = 0.1"}, 
        {"6p0", "m1 = 5 GeV, #Delta = 0.4"}, 
        {"60p0", "m1 = 50 GeV, #Delta = 0.4"} 
    };

    std::map<std::string, std::string> files { 
        {"5p25", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-5p25_dMchi-0p5_ctau-100.root"},
        {"52p5", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-52p5_dMchi-5p0_ctau-100.root"},
        {"6p0", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-6p0_dMchi-2p0_ctau-100.root"}, 
        {"60p0", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-60p0_dMchi-20p0_ctau-100.root"}
    };

    std::map<std::string, std::map<std::string, TH1F *> > hists;
    int numCuts_ = 16;

    TCut noSelection = "";
    TCut initialSelection = "cutsVec[1] && cutsVec[2] && cutsVec[4] && cutsVec[5] && cutsVec[6] && cutsVec[7]";
    TCut signalRegion = initialSelection && "cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && cutsVec[14]";

    std::map<std::string, TCut> cuts { 
        {"no_selection", noSelection},
        {"initial_selection", initialSelection},
        {"signal_region", signalRegion}
    };
    std::map<std::string, std::string> cut_labels {
        {"no_selection", "No Selection"},
        {"initial_selection", "Initial Selection"},
        {"signal_region", "Initial Selection + Signal Region"}
    };

    std::map<std::string, TCanvas *> canvases;

    for (auto & file : files) {

        TFile * f = TFile::Open(file.second.c_str());
        TTree * recoT = (TTree*)f->Get("SREffi_dsa/reco");
        recoT->UseCurrentStyle();

        for (auto cut : cuts) {
            recoT->Draw(Form("%s>>h%s_%s(%d,%f,%f)", variable.c_str(), cut.first.c_str(), file.first.c_str(), nbins, lowx, highx), cut.second, "goff");
            hists[file.first][cut.first] = (TH1F*)gDirectory->Get(Form("h%s_%s", cut.first.c_str(), file.first.c_str()));
        }
    }
    for (auto cut : cuts) {
        THStack * hs = new THStack(Form("%s", cut.first.c_str()), Form("%s", cut_labels[cut.first].c_str()));
        auto legend = new TLegend(0.6,0.7,0.9,0.9);
        for (auto & file : files) {
            hists[file.first][cut.first]->Scale(1/hists[file.first][cut.first]->Integral(), "width");
            hists[file.first][cut.first]->SetLineColor(colors[file.first]);
            hists[file.first][cut.first]->SetLineWidth(2);
            hs->Add(hists[file.first][cut.first]);
            legend->AddEntry(hists[file.first][cut.first], labels[file.first].c_str());
        }
        canvases[cut.first] = new TCanvas();
        canvases[cut.first]->cd();
        hs->Draw("E HIST nostack");
        hs->GetXaxis()->SetTitle(Form("%s", description.c_str()));
        hs->GetYaxis()->SetTitle("A. U.");
        gPad->Modified();
        legend->Draw();
        if (fSave) 
            canvases[cut.first]->SaveAs(Form("temp_plots/%s_%s.pdf", variable.c_str(), cut.first.c_str()));
    }
}
