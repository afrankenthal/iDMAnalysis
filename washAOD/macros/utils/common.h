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
    extern std::map<TString, MODE> mapMODE;
    enum STYLE { SOLID=1, DASHED, MARKER=20 };

    typedef struct GroupPlotInfo {
        TString legend;
        Int_t color;
        STYLE style; // mainly solid (1) or dashed (2)
        MODE mode; // 0 for fill (background), 1 for marker (data), 2 for line (signal)
    } GroupPlotInfo;

    extern std::map<TString, GroupPlotInfo> group_plot_info;

    typedef struct THInfo {
        TString quantity;
        std::vector<std::string> groups;
        std::vector<int> cuts;
        TString type;
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

    typedef struct CutInfo {
        TString cut;
        TString description;
        TString inclusive;
    } CutInfo;

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

    extern std::map<int, std::string> cut_descriptions;

    void printTimeElapsed(TDatime begin);

    std::vector<TString> listFiles(const char *dirname="", const char *ext=".root");

    bool readCutBit(uint32_t cuts, int bit);

} // namespace common

#endif // COMMON_H
