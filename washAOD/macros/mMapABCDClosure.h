#ifndef mMAPABCDCLOSURE_HH
#define mMAPABCDCLOSURE_HH

#include <algorithm>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
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
#include <TH2D.h>
#include <THStack.h>
#include <TImage.h>
#include <TROOT.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include "utils/common.h"
using namespace common;
#include "utils/cxxopts.hpp"
#include "utils/json.hpp"
using json = nlohmann::json;

using std::cout, std::endl, std::map, std::vector;

namespace macro {

    extern "C" bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg);

}

#endif // mMAPABCDCLOSURE_HH
