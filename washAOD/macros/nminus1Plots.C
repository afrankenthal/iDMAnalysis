#include "tdrstyle.C"
#include "json.hpp"
using json = nlohmann::json;

using std::cout, std::endl, std::map, std::vector;

typedef struct sample_info {
    vector<TString> filenames;
    TString label;
    Float_t xsec;
    Float_t weight;
    TString group;
    int color;
    int style;
} sample_info;

bool readCutBit(uint32_t cuts, int bit) { return cuts & (1 << bit); }

void list_files(sample_info & sample, const char *dirname="", const char *ext=".root") {
    TSystemDirectory dir(dirname, dirname); 
    TList *files = dir.GetListOfFiles(); 
    if (files) { 
        TSystemFile *file; 
        TString fname; 
        TIter next(files); 
        while ((file=(TSystemFile*)next())) { 
            fname = file->GetName(); 
            if (!file->IsDirectory() && fname.EndsWith(ext)) 
                sample.filenames.push_back(Form("%s/%s", dirname, fname.Data()));
        }
    } 
}

void nminus1Plots(bool fSave=false) {
    TDatime time_begin;
    map<TString, sample_info> samples;
    std::ifstream bkgs_file("backgrounds.json");
    json bkgs_cfg;
    bkgs_file >> bkgs_cfg;
    int color = 3;
    for (auto const & [bkg, cfg] : bkgs_cfg.items()) {
        samples.insert(std::make_pair(TString(bkg),
                    sample_info{
                    {}, // list of filenames
                    bkg, // plot label
                    cfg["xsec"].get<float>(), // xsec
                    1, // weight
                    TString(cfg["group"].get<std::string>()), // plot group
                    color++, // line color
                    1 // line style
                    }));
        list_files(samples[bkg], cfg["dir"].get<std::string>().c_str()); 
    }

    //gROOT->LoadMacro("tdrstyle.C");
    setTDRStyle();

    typedef struct signal_info {
        TString m1;
        TString dmchi;
        TString delta;
        int color;
    } signal_info;
    map<TString, signal_info> mchis{
        {"5p25", signal_info{"5", "0p5", "0.1", kYellow}},
        {"52p5", signal_info{"50", "5p0", "0.1", kBlue}},
        {"6p0", signal_info{"5", "2p0", "0.4", kGreen}},
        {"60p0", signal_info{"50", "20p0", "0.4", kRed}}
    };
    vector<TString> ctaus{"1", "10", "100", "1000"};
    for (auto ctau : ctaus) 
        for (auto const & [mchi, cfg] : mchis) 
            samples.insert(std::make_pair(Form("%s_%s", mchi.Data(), ctau.Data()),
                        sample_info{
                        vector<TString>{Form("../Mchi-%s_dMchi-%s_ctau-%s.root", mchi.Data(), cfg.dmchi.Data(), ctau.Data())}, // list of filenames
                        Form("m1 = %s GeV, #Delta = %s, c#tau = %s mm", cfg.m1.Data(), cfg.delta.Data(), ctau.Data()), // plot label
                        1, // xsec
                        1, // weight
                        "", // plot group
                        cfg.color + 1*(int)(distance(ctaus.begin(), find(ctaus.begin(), ctaus.end(), ctau))), // line color
                        2 // line style
                        }));

    for (auto const & [sample, props] : samples) {
        cout << sample << endl;
        if (!props.filenames.empty())
            cout << props.filenames.at(0) << endl;
    }

    map<TString, map<TString, TH1F *>> hists;
    int numCuts_ = 19;

    typedef struct cut_info {
        TCut tcut;
        TString plot_var;
        TString plot_description;
        Int_t nbins;
        Float_t lowX;
        Float_t highX;
        TString label;
        vector<TString> sample_list;
    } cut_info;

    vector<TCut> bits;
    for (int bit = 0; bit < numCuts_; bit++)
        bits.push_back(TCut(Form("(cuts & (1 << %d))", bit)));

    TCut noSelection = "";
    TCut muon1 = bits[5] && bits[6] && bits[7] && bits[8] && bits[9];
    TCut muon2 = bits[10] && bits[11] && bits[12] && bits[13] && bits[14];
    TCut initialSelection = bits[1] && bits[2] && bits[3] && muon1;
    TCut nMinusDr = initialSelection && bits[4] && muon2 && bits[16];
    TCut nMinusMmumu = initialSelection && bits[4] && muon2 && bits[15];
    TCut nMinusNJets = initialSelection && muon2 && bits[15] && bits[16];
    TCut nMinusGoodMu = bits[1] && bits[2] && bits[3] && bits[4];

    //TCut signalRegion = initialSelection && "cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && cutsVec[14]";
    //TCut controlRegion_QCD = initialSelection && "!cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && cutsVec[14]";
    //TCut controlRegion_EW = initialSelection && "cutsVec[3] && !cutsVec[9] && !cutsVec[10] && !cutsVec[11] && !cutsVec[12] && !cutsVec[14]";
    //TCut controlRegion_TTbar = initialSelection && "cutsVec[3] && cutsVec[9] && cutsVec[10] && cutsVec[11] && cutsVec[12] && !cutsVec[14]";

    map<TString, cut_info> cuts {
        //{"no_selection", cut_info{noSelection, "No Selection"}},
        //{"initial_selection", cut_info{initialSelection, "Initial Selection"}},
        {"nMinusDr", cut_info{nMinusDr, "reco_vtx_dR", "Di-muon dR", 10, 0, 6, "N-1 (dR)",
                                 vector<TString>{"5p25_100", "60p0_1", "Di-boson", "QCD", "DY", "TTbar", "V+Jets"}}},
            {"nMinusMmumu", cut_info{nMinusMmumu, "reco_Mmumu", "M_{#mu#mu} [GeV]", 20, 0, 50, "N-1 (M_{#mu#mu})",
                                 vector<TString>{"5p25_100", "60p0_1", "Di-boson", "QCD", "DY", "TTbar", "V+Jets"}}},
            {"nMinusNJets", cut_info{nMinusNJets, "reco_PF_n_highPt_jets", "Number of high pT (> 30 GeV) jets", 9, 1, 10, "N-1 (nJets)",
                                 vector<TString>{"5p25_100", "60p0_1", "Di-boson", "QCD", "DY", "TTbar", "V+Jets"}}},
            {"nMinusGoodMu", cut_info{nMinusGoodMu, "reco_n_good_dsa", "Number of quality dSA muons", 9, 1, 10, "N-1 (nGoodMuons)",
                                 vector<TString>{"5p25_100", "60p0_1", "Di-boson", "QCD", "DY", "TTbar", "V+Jets"}}},
        //{"signal_region", cut_info{signalRegion, "Signal Region"}},
        //{"control_region_QCD", cut_info{controlRegion_QCD, "QCD Control Region"}},
        //{"control_region_EW", cut_info{controlRegion_EW, "EW Control Region"}},
        //{"control_region_TTbar", cut_info{controlRegion_TTbar, "TTbar Control Region"}}
    };

    map<TString, TCanvas *> canvases;

    for (auto const & [sample, props] : samples) {

        //int count = 0;
        TChain * data_gen = new TChain("ntuples_gbm/genT");
        TChain * data_reco = new TChain("ntuples_gbm/recoT");
        for (auto const & filename : props.filenames) {
            //if (count++ >= 2) continue;
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

        Float_t sum_gen_wgt = 0;
        for (size_t i = 0; i < data_gen->GetEntries(); i++) {
            data_gen->GetEntry(i);
            sum_gen_wgt += gen_wgt;
        }
        cout << "sample: " << sample << ", sum_gen_wgt: " << sum_gen_wgt << endl;

        for (auto const & [cut_name, cut] : cuts) {
            if (std::find(cut.sample_list.begin(), cut.sample_list.end(), sample) != cut.sample_list.end()) {
                data_reco->Draw(Form("%s>>h%s_%s(%d,%f,%f)", cut.plot_var.Data(), cut_name.Data(), sample.Data(), cut.nbins, cut.lowX, cut.highX), cut.tcut, "goff");
                hists[sample][cut_name] = (TH1F*)gDirectory->Get(Form("h%s_%s", cut_name.Data(), sample.Data()));
            }
            else if (std::find(cut.sample_list.begin(), cut.sample_list.end(), props.group) != cut.sample_list.end()) {
                data_reco->Draw(Form("%s>>h%s_%s(%d,%f,%f)", cut.plot_var.Data(), cut_name.Data(), props.group.Data(), cut.nbins, cut.lowX, cut.highX), cut.tcut * Form("gen_wgt * %f / %f", props.xsec, sum_gen_wgt), "goff");
                hists[props.group][cut_name] = (TH1F*)gDirectory->Get(Form("h%s_%s", cut_name.Data(), props.group.Data()));
            }
        }
    }

    for (auto const & [cut_name, cut] : cuts) {
        THStack * hs = new THStack(Form("%s", cut_name.Data()), Form("%s", cut.label.Data()));
        auto legend = new TLegend(0.45,0.7,0.85,0.9);
        legend->SetFillStyle(0);
        //auto legend = new TLegend(0.25,0.2);
        color = 3;
        for (auto & sample : cut.sample_list) {
            hists[sample][cut_name]->Scale(1/hists[sample][cut_name]->Integral(), "width");
            if (samples.find(sample) != samples.end()) {
                hists[sample][cut_name]->SetLineColor(samples[sample].color);
                hists[sample][cut_name]->SetLineStyle(samples[sample].style);
            }
            else {
                if (color == 5) // yellow
                    hists[sample][cut_name]->SetLineColor(kYellow+1);
                else
                    hists[sample][cut_name]->SetLineColor(color);
                color++;
            }
            hists[sample][cut_name]->SetLineWidth(2);
            hs->Add(hists[sample][cut_name]);
            if (samples.find(sample) != samples.end())
                legend->AddEntry(hists[sample][cut_name], samples[sample].label.Data());
            else
                legend->AddEntry(hists[sample][cut_name], sample);
        }
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
        }
    }
    TDatime time_end;
    cout << "Time elapsed: " << time_end.Convert() - time_begin.Convert() << " seconds or " 
         << (time_end.Convert() - time_begin.Convert())/3600.0 << " hours" << endl;
}
