#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string.h>

#include <TApplication.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TCollection.h>
#include <TCut.h>
#include <TDatime.h>
#include <TFile.h>
#include <TH1F.h>
#include <THStack.h>
#include <TLegend.h>
#include <TEfficiency.h>
#include <TGraphAsymmErrors.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include "utils/CMS_lumi.h"
#include "utils/common.h"
using namespace common;
#include "utils/cxxopts.hpp"
#include "utils/json.hpp"
using json = nlohmann::json;
#include "utils/tdrstyle.h"

using std::cout, std::endl, std::map, std::vector;

//TString lumi_13TeV = "59.74 fb^{-1} ";


namespace macro {

    extern "C" void canvasDraw(bool newCanvas,TString type,TString hs_basename, TString hs_suffix,TEfficiency *hs,TCanvas c);
    extern "C" bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg);

}
