#ifndef mMAINANALYSIS_HH
#define mMAINANALYSIS_HH

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <string.h>

#include <TApplication.h>
#include <TChain.h>
#include <TCollection.h>
#include <TCut.h>
#include <TDatime.h>
#include <TH1F.h>
#include <TH2F.h>
#include <THStack.h>
#include <TSelector.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include "utils/common.h"
using namespace common;
#include "utils/cxxopts.hpp"
#include "utils/json.hpp"
using json = nlohmann::json;
#include "utils/tdrstyle.h"

#include "RDataFrameSelectors/RDFAnalysis.h"
#include "TSelectors/mainAnalysisSelector.h"

using std::cout, std::endl, std::map, std::vector;

namespace macro {

    bool mMainAnalysis(map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg); 
}

#endif // mMAINANALYSIS_HH
