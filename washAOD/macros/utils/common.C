#ifndef COMMON_H
#define COMMON_H

#include <TColor.h>

namespace common {

    enum MODE { BKG, DATA, SIGNAL };
    std::map<TString, MODE> mapMODE{ {"BKG",BKG}, {"DATA",DATA}, {"SIGNAL",SIGNAL} };
    enum STYLE { SOLID=1, DASHED, MARKER=20 };

    typedef struct GroupPlotInfo {
        TString legend;
        Int_t color;
        STYLE style; // mainly solid (1) or dashed (2)
        MODE mode; // 0 for fill (background), 1 for marker (data), 2 for line (signal)
    } GroupPlotInfo;

    std::map<TString, GroupPlotInfo> group_plot_info{
        {"DY", GroupPlotInfo{"Z(ll)+Jets", kOrange, SOLID, BKG}},
        {"QCD", GroupPlotInfo{"QCD", kRed, SOLID, BKG}},
        {"Multiboson", GroupPlotInfo{"Di-/Tri-boson", kGreen, SOLID, BKG}},
        {"ZJets", GroupPlotInfo{"Z(#nu#nu)+Jets", 4, SOLID, BKG}},
        {"WJets", GroupPlotInfo{"W(l#nu)+Jets", kYellow, SOLID, BKG}},
        {"Top", GroupPlotInfo{"Top", 6, SOLID, BKG}},
        {"60p0_1", GroupPlotInfo{"(50,70) GeV, 100 mm", kBlack, SOLID, SIGNAL}},
        {"5p25_100", GroupPlotInfo{"(5,5.5) GeV, 1 mm", kBlue, SOLID, SIGNAL}},
        {"52p5_1000", GroupPlotInfo{"(50,55) GeV, 1000 mm", kOrange, SOLID, SIGNAL}},
        {"data", GroupPlotInfo{"Data", kBlack, MARKER, DATA}}
    };

    typedef struct THInfo {
        TString quantity;
        std::vector<std::string> groups;
        std::vector<int> cuts;
        TString name;
        TString title;
        Int_t nbinsX;
        Double_t lowX;
        Double_t highX;
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

    typedef struct SampleInfo {
        std::vector<TString> filenames;
        TString label;
        Float_t xsec;
        Float_t sum_gen_wgt;
        Int_t limit_num_files;
        Float_t weight;
        TString group;
        MODE mode;
        int color;
        int style;
    } SampleInfo;

    std::map<int, std::string> cut_descriptions {
        {0, "No cuts"},
        {1, "MET filter bits pass"},
        {2, "120 GeV MET trigger fired"},
        {3, "MET > 200 GeV"},
        {4, "Leading jet pT > 120 GeV"},
        {5, "nJets < 3 (pT > 30 GeV)"},
        {6, "0 b-tagged jets"},
        {7, "2 dSA muons"},
        {8, "muon 1: >= 2 muon planes"},
        {9, "muon 1: >= 12 muon hits"},
        {10, "muon 1: #chi2 < 10"},
        {11, "muon 1: pT > 5 GeV"},
        {12, "muon 1: eta < 2.4"},
        {13, "muon 2: >= 2 muon planes"},
        {14, "muon 2: >= 12 muon hits"},
        {15, "muon 2: #chi2 < 10"},
        {16, "muon 2: pT > 5 GeV"},
        {17, "muon 2: eta < 2.4"},
        {18, "muon dR < 0.9"},
        {19, "muons: OSSF"},
        {20, "0 GM-dSA matched"},
        {21, "1 GM-dSA matched"},
        {22, "2 GM-dSA matched"}
    };

    void printTimeElapsed(TDatime begin) {
        TDatime end;
        std::cout << "Time elapsed: " << (end.Convert() - begin.Convert()) << " seconds"
            << " or " << (end.Convert() - begin.Convert())/3600.0 << " hours" << std::endl;
    }

    std::vector<TString> listFiles(const char *dirname="", const char *ext=".root") {
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

    bool readCutBit(uint32_t cuts, int bit) { return cuts & (1 << bit); }

} // namespace common

#endif // COMMON_H
