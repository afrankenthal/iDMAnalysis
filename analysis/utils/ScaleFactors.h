#include <cstdlib>

#include <TFile.h>
#include <TFormula.h>
#include <TH1.h>
#include <TH2F.h>

#include "rapidcsv.h"

using std::unique_ptr, std::cout, std::endl, std::vector, std::map;

class ScaleFactors {

public:

    ScaleFactors() {}
    ~ScaleFactors() {}

    enum SYST {
        NOMINAL, // == 0 
        BTAG_DOWN, // == 1, and so on
        BTAG_UP,
        GM_ID_UP,
        GM_ID_DOWN,
        DSA_ID_UP,
        DSA_ID_DOWN,
        DSA_RECO_UP,
        DSA_RECO_DOWN
    };

    bool buildScaleFactors(SYST syst = NOMINAL);

    typedef struct bTagSF_t {
        float lower_pt_edge, upper_pt_edge;
        std::unique_ptr<TFormula> formula;
    } bTagSF_t;

    typedef struct gmLowPtSF_t {
        float lower_eta_edge, upper_eta_edge;
        float central, down, up;
    } gmLowPtSF_t;

    static bool compareBTagLowerEdge(const bTagSF_t &a, const float &b) { return (a.lower_pt_edge < b); }
    static bool compareBTagUpperEdge(const bTagSF_t &a, const float &b) { return (a.upper_pt_edge < b); }
    static bool compareGMLowerEdge(const gmLowPtSF_t &a, const float &b) { return (a.lower_eta_edge < b); }
    static bool compareGMUpperEdge(const gmLowPtSF_t &a, const float &b) { return (a.upper_eta_edge < b); }

    map<int, vector<gmLowPtSF_t>> gmlowpt_sfs;
    map<int, vector<bTagSF_t>> btag_sfs;
    map<std::string, map<int, unique_ptr<TH1>>> all_sfs;

private:

    vector<bTagSF_t> buildBTagSF(rapidcsv::Document btag_file, SYST syst);
    std::unique_ptr<TH1> buildSF(TString filename, TString histoname);

};