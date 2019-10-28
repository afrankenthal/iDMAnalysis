#ifndef mSROPTIMIZATION_HH
#define mSROPTIMIZATION_HH

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string.h>

#include <TApplication.h>
#include <TChain.h>
#include <TCollection.h>
#include <TCut.h>
#include <TDatime.h>
#include <TH1F.h>
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

    bool mSROptimization(map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg);

}

#endif // mSROPTIMIZATION_HH
