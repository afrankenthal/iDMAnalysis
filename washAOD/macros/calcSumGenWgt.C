#include "utils/json.hpp"
using json = nlohmann::json;
#include "utils/common.C"
using namespace common;

using std::cout, std::endl, std::map, std::vector;

void calcSumGenWgt() {
    TDatime time_begin;

    map<TString, SampleInfo> samples;

    std::ifstream bkgs_file("configs/backgrounds_full.json");
    json bkgs_cfg;
    bkgs_file >> bkgs_cfg;
    json out_json = bkgs_cfg;
    for (auto const & [bkg, cfg] : bkgs_cfg.items())
        samples[TString(bkg)] = SampleInfo{
            listFiles(cfg["dir"].get<std::string>().c_str()), // list of filenames 
            bkg, // plot label
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
    std::ofstream out("backgrounds_out.json");
    out << std::setw(4) << out_json << endl;

    printTimeElapsed(time_begin);
}
