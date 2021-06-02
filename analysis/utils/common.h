#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vector>
#include <map>

#include <TCollection.h>
#include <TColor.h>
#include <TDatime.h>
#include <TList.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

namespace common {

    enum MODE { BKG, DATA, SIGNAL };
    extern "C" {
        static std::map<TString, MODE> mapMODE{ {"BKG",BKG}, {"DATA",DATA}, {"SIGNAL",SIGNAL} };
    }
    enum STYLE { SOLID=1, DASHED, MARKER=20 };

    typedef struct GroupPlotInfo {
        TString legend;
        Int_t color;
        STYLE style; // mainly solid (1) or dashed (2)
        MODE mode; // 0 for fill (background), 1 for marker (data), 2 for line (signal)
    } GroupPlotInfo;

    //extern "C" { 
    //    static std::map<TString, GroupPlotInfo> group_plot_info{
    //    {"MCTotal", GroupPlotInfo{"MC", kRed, SOLID, BKG}},
    //    {"DY", GroupPlotInfo{"Z(ll)+Jets", kOrange, SOLID, BKG}},
    //    {"QCD", GroupPlotInfo{"QCD", kRed, SOLID, BKG}},
    //    {"Multiboson", GroupPlotInfo{"Di-/Tri-boson", kGreen, SOLID, BKG}},
    //    {"ZJets", GroupPlotInfo{"Z(#nu#nu)+Jets", 4, SOLID, BKG}},
    //    {"WJets", GroupPlotInfo{"W(l#nu)+Jets", kYellow, SOLID, BKG}},
    //    {"GJets", GroupPlotInfo{"#gamma+Jets", kCyan, SOLID, BKG}},
    //    {"Top", GroupPlotInfo{"Top", 6, SOLID, BKG}},
    //    {"sig_60p0_1", GroupPlotInfo{"(50,70) GeV, 1 mm", kBlack, SOLID, SIGNAL}},
    //    {"sig_5p25_1000", GroupPlotInfo{"(5,5.5) GeV, 1000 mm", kBlue, SOLID, SIGNAL}},
    //    {"sig_52p5_100", GroupPlotInfo{"(50,55) GeV, 100 mm", kOrange, SOLID, SIGNAL}},
    //    {"sig_6p0_10", GroupPlotInfo{"(5,7) GeV, 10 mm", kMagenta, SOLID, SIGNAL}},
    //    {"data", GroupPlotInfo{"Data", kBlack, MARKER, DATA}}
    //    }; 
    //}
    extern "C" { 
        static std::map<TString, GroupPlotInfo> group_plot_info{
        {"MCTotal", GroupPlotInfo{"MC", kRed, SOLID, BKG}},
        {"DY", GroupPlotInfo{"Z(ll)+Jets", kOrange-3, SOLID, BKG}},
        {"QCD", GroupPlotInfo{"QCD", kRed+1, SOLID, BKG}},
        {"Multiboson", GroupPlotInfo{"Di-/Tri-boson", kTeal+4, SOLID, BKG}},
        {"ZJets", GroupPlotInfo{"Z(#nu#nu)+Jets", kAzure+7, SOLID, BKG}},
        {"WJets", GroupPlotInfo{"W(l#nu)+Jets", kGray+1, SOLID, BKG}},
        {"GJets", GroupPlotInfo{"#gamma+Jets", kCyan, SOLID, BKG}},
        {"Top", GroupPlotInfo{"Top", kViolet-2, SOLID, BKG}},
        {"sig_60p0_1", GroupPlotInfo{"(50,70) GeV, 1 mm", kBlack, SOLID, SIGNAL}},
        {"sig_60p0_10", GroupPlotInfo{"(50,70) GeV, 10 mm", kBlack, SOLID, SIGNAL}},
        {"sig_60p0_100", GroupPlotInfo{"(50,70) GeV, 100 mm", kBlack, SOLID, SIGNAL}},
        {"sig_60p0_1000", GroupPlotInfo{"(50,70) GeV, 1000 mm", kBlack, SOLID, SIGNAL}},
        {"sig_5p25_1", GroupPlotInfo{"(5,5.5) GeV, 1 mm", kCyan, SOLID, SIGNAL}},
        {"sig_5p25_10", GroupPlotInfo{"(5,5.5) GeV, 10 mm", kCyan, SOLID, SIGNAL}},
        {"sig_5p25_100", GroupPlotInfo{"(5,5.5) GeV, 100 mm", kCyan, SOLID, SIGNAL}},
        {"sig_5p25_1000", GroupPlotInfo{"(5,5.5) GeV, 1000 mm", kCyan, SOLID, SIGNAL}},
        {"sig_52p5_1", GroupPlotInfo{"(50,55) GeV, 1 mm", kGreen-4, SOLID, SIGNAL}},
        {"sig_52p5_10", GroupPlotInfo{"(50,55) GeV, 10 mm", kGreen-4, SOLID, SIGNAL}},
        {"sig_52p5_100", GroupPlotInfo{"(50,55) GeV, 100 mm", kGreen-4, SOLID, SIGNAL}},
        {"sig_52p5_1000", GroupPlotInfo{"(50,55) GeV, 1000 mm", kGreen-4, SOLID, SIGNAL}},
        {"sig_6p0_1", GroupPlotInfo{"(5,7) GeV, 1 mm", kYellow-3, SOLID, SIGNAL}},
        {"sig_6p0_10", GroupPlotInfo{"(5,7) GeV, 10 mm", kYellow-3, SOLID, SIGNAL}},
        {"sig_6p0_100", GroupPlotInfo{"(5,7) GeV, 100 mm", kYellow-3, SOLID, SIGNAL}},
        {"sig_6p0_1000", GroupPlotInfo{"(5,7) GeV, 1000 mm", kYellow-3, SOLID, SIGNAL}},
        {"data", GroupPlotInfo{"Data", kBlack, MARKER, DATA}}
        }; 
    }


    typedef struct THInfo {
        TString quantity;
        std::vector<std::string> groups_to_exclude;
        TString type;
        TString name;
        TString title;
        Int_t nbinsX;
        Double_t lowX;
        Double_t highX;
        std::vector<double> binEdgesX;
        TString nMinus1CutDescription;
        // 2D plot params
        TString quantity2;
        Int_t nbinsY;
        Double_t lowY;
        Double_t highY;
    } TH1Info;

    typedef struct SignalInfo {
        TString m1;
        TString dmchi;
        TString delta;
        int color;
    } SignalInfo;

    typedef struct CutInfo {
        TString cut;
        TString description;
        TString inclusive;
        TString efficiency;
        TString special;
        TString book_plot;
        TString print_events;
    } CutInfo;

    typedef struct SampleInfo {
        std::vector<TString> filenames;
        TString label;
        Float_t xsec;
        Float_t sum_gen_wgt;
        Int_t limit_num_files;
        Float_t weight;
        int year;
        Float_t lumi;
        TString group;
        MODE mode;
        int color;
        int style;
    } SampleInfo;

    //extern "C" {
    //    static std::map<int, std::string> cut_descriptions {
    //        {0, "No cuts"},
    //        {1, "MET filter bits pass"},
    //        {2, "HEM vetoed"},
    //        {3, "120 GeV MET trigger fired"},
    //        {4, "MET > 200 GeV"},
    //        {5, "nJets > 0 (pT > 30 GeV)"},
    //        {6, "jets |#eta| < 2.5"},
    //        {7, "Leading jet pT > 200 GeV"},
    //        {8, "Veto electrons"},
    //        {9, "Veto photons"},
    //        {10, "good dSA muons >= 0"},
    //        {11, "|#Delta#Phi(jet, MET)| > 1"},
    //        {12, "nJets < 3 (pT > 30 GeV)"},
    //        {13, "0 b-tagged jets"},
    //        {14, "good dSA muons >= 1"},
    //        {15, "good dSA muons >= 2"},
    //        {16, "muon 1: >= 2 muon planes"},
    //        {17, "muon 1: >= 12 muon hits"},
    //        {18, "muon 1: #chi2 < 10"},
    //        {19, "muon 1: pT > 5 GeV"},
    //        {20, "muon 1: |eta| < 2.4"},
    //        {21, "muon 2: >= 2 muon planes"},
    //        {22, "muon 2: >= 12 muon hits"},
    //        {23, "muon 2: #chi2 < 10"},
    //        {24, "muon 2: pT > 5 GeV"},
    //        {25, "muon 2: |eta| < 2.4"},
    //        {26, "muon dR < 0.9"},
    //        {27, "muons: OSSF"},
    //        {28, "0 GM-dSA matched"},
    //        {29, "1 GM-dSA matched"},
    //        {30, "2 GM-dSA matched"}
    //    };
    //}

    inline void printTimeElapsed(TDatime begin) {
        TDatime end;
        std::cout << std::endl << ">>>>>>>>> Time elapsed: " << (end.Convert() - begin.Convert()) << " seconds"
            << " or " << (end.Convert() - begin.Convert())/3600.0 << " hours" << std::endl;
    }

    inline std::vector<TString> listFiles(const char *dirname="", const char *ext=".root") {
        TSystemDirectory dir(dirname, dirname); 
        // First check if dirname is actually just a single .root file
        if (TString(dirname).EndsWith(TString(ext)))
            return std::vector<TString>{TString(dirname)};
        std::vector<TString> filenames;
        TList *files = dir.GetListOfFiles(); 
        if (files) { 
            TSystemFile *file; 
            TString fname; 
            TIter next(files); 
            while ((file=(TSystemFile*)next())) { 
                fname = file->GetName(); 
                if (!file->IsDirectory() && fname.EndsWith(ext)) 
                    filenames.push_back(Form("%s/%s", dirname, fname.Data()));
            }
        } 
        return filenames;
    }

    inline bool readCutBit(uint32_t cuts, int bit) { return cuts & (1 << bit); }

} // namespace common

#endif // COMMON_H
