#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>
using std::cout, std::endl, std::map, std::vector;

#include <TChain.h>

#include "../utils/common.h"
using namespace common;
#include "../utils/json.hpp"
using json = nlohmann::json;

namespace macro {

    extern "C" bool process(map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {

        std::ifstream json_file(cfg["sample_config_filename"].get<std::string>());
        int limit_num_files = cfg["limit_num_files"].get<int>();
        std::string out_filename = cfg["output_file"].get<std::string>();
        json in_json;
        json_file >> in_json;
        json out_json = in_json;

        for (auto const & [sample, props] : samples) {

            if (props.sum_gen_wgt > 0.1) continue; // only run over previously empty samples

            cout << "Processing sample " << sample << ", " << props.filenames.size() << " files" << endl;

            TChain * data_gen = new TChain("ntuples_gbm/genT");

            int count = 0;
            for (auto const & filename : props.filenames) {
                if (count++ >= limit_num_files && limit_num_files != -1) continue;
                data_gen->Add(filename);
            }
            data_gen->SetBranchStatus("*",0);
            data_gen->SetBranchStatus("gen_wgt", 1);
            Float_t gen_wgt;
            data_gen->SetBranchAddress("gen_wgt", &gen_wgt);
            data_gen->GetEntries();
            Double_t sum_gen_wgt = 0;
            for (int i = 0; i < data_gen->GetEntries(); i++) {
                data_gen->GetEntry(i);
                sum_gen_wgt += gen_wgt;
            }
            cout << "sample: " << sample << ", sum_gen_wgt: " << sum_gen_wgt << endl;
            out_json[sample.Data()]["sum_gen_wgt"] = sum_gen_wgt;
            out_json[sample.Data()]["limit_num_files"] = limit_num_files;
        }

        if (out_filename == "")
            out_filename = "out.json";
        std::ofstream out(out_filename);
        out << std::setw(4) << out_json << endl;

        return 0;
    }

}
