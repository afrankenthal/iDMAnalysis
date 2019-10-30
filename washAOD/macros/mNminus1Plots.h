#ifndef mNMINUS1PLOTS_HH
#define mNMINUS1PLOTS_HH

#include <TCanvas.h>
#include <TCut.h>
#include <TChain.h>
#include <TH1F.h>
#include <THStack.h>
#include <TLegend.h>
#include <TPaveText.h>

#include "utils/common.h"
using namespace common;
#include "utils/json.hpp"
using json = nlohmann::json;
#include "utils/tdrstyle.h"

using std::cout, std::endl, std::map, std::vector;

namespace macro {

    extern "C" bool process(map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg);

} // namespace macro

#endif // nNMINUS1PLOTS_HH
