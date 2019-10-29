#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using std::cout, std::endl, std::map, std::vector;

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
#include "utils/json.hpp"
using json = nlohmann::json;

namespace macro {

    bool mCutflowTables(map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg);

}
