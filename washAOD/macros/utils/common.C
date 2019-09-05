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
        {"60p0_1", GroupPlotInfo{"(50,70) GeV, 100 mm", kBlack, DASHED, SIGNAL}},
        {"5p25_100", GroupPlotInfo{"(5,5.5) GeV, 1 mm", kBlue, DASHED, SIGNAL}},
        {"52p5_1000", GroupPlotInfo{"(50,55) GeV, 1000 mm", kGreen, DASHED, SIGNAL}},
        {"data", GroupPlotInfo{"Data", kBlack, MARKER, DATA}},
    };

    typedef struct TH1Info {
        TString quantity;
        std::vector<std::string> groups;
        std::vector<int> cuts;
        TString name;
        TString title;
        Int_t nbins;
        Double_t lowX;
        Double_t highX;
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

    void printTimeElapsed(TDatime begin) {
        TDatime end;
        std::cout << "Time elapsed: " << (end.Convert() - begin.Convert()) << " seconds"
            << " or " << (end.Convert() - begin.Convert())/3600.0 << " hours" << std::endl;
    }

    //vector<TString> listFiles(SampleInfo & sample, const char *dirname="", const char *ext=".root") {
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
                    //sample.filenames.push_back(Form("%s/%s", dirname, fname.Data()));
            }
        } 
        return filenames;
    }

    bool readCutBit(uint32_t cuts, int bit) { return cuts & (1 << bit); }

}

#endif // COMMON_H
