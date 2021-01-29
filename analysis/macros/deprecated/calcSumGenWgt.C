#include "utils/json.hpp"
using json = nlohmann::json;
#include "utils/common.C"
using namespace common;

using std::cout, std::endl, std::map, std::vector;

void calcSumGenWgt(int which=0) {
    TDatime time_begin;

    map<TString, SampleInfo> samples;

    TString config_name;
    if (which == 0)
        config_name = TString("configs/signal.json");
    else if (which == 1)
        config_name = TString("configs/backgrounds_full.json");
    else if (which == 2)
        config_name = TString("configs/backgrounds_subset.json");
    else if (which == 3)
        config_name = TString("configs/test.json");
    else {
        cout << "Which config option invalid! Returning..." << endl;
        return;
    }

    std::ifstream file(config_name.Data());
    json cfgs;
    file >> cfgs;
    json out_json = cfgs;
    for (auto const & [sample, cfg] : cfgs.items())
        samples[TString(sample)] = SampleInfo{
            listFiles(cfg["dir"].get<std::string>().c_str()), // list of filenames 
            sample, // plot label
            cfg["xsec"].get<float>(), // xsec
            cfg["sum_gen_wgt"].get<float>(), // sum_gen_wgt
            cfg["limit_num_files"].get<int>(), // limit_num_files
            1, // weight
            TString(cfg["group"].get<std::string>()), // plot group
            0, // line color
            1 // line style
        };

    for (auto const & [sample, props] : samples) {
        if (props.sum_gen_wgt > 0.1) continue; // only run over previously empty samples

        cout << "Processing sample " << sample << ", " << props.filenames.size() << " files" << endl;

        TChain * data_gen = new TChain("ntuples_gbm/genT");

        int limit_num_files = -1;

        int count = 0;
        for (auto const & filename : props.filenames) {
            if (count++ >= limit_num_files && limit_num_files != -1) continue;
            data_gen->Add(filename);
        }
        Float_t gen_wgt;
        data_gen->SetBranchAddress("gen_wgt", &gen_wgt);
        data_gen->GetEntries();
        Float_t sum_gen_wgt = 0;
        for (size_t i = 0; i < data_gen->GetEntries(); i++) {
            data_gen->GetEntry(i);
            sum_gen_wgt += gen_wgt;
        }
        cout << "sample: " << sample << ", sum_gen_wgt: " << sum_gen_wgt << endl;
        out_json[sample.Data()]["sum_gen_wgt"] = sum_gen_wgt;
        out_json[sample.Data()]["limit_num_files"] = limit_num_files;
    }
    std::ofstream out("out.json");
    out << std::setw(4) << out_json << endl;

    printTimeElapsed(time_begin);
}
