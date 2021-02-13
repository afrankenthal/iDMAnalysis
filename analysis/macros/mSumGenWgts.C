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

        // macro options

        vector<std::string> sample_config_filenames = cfg["sample_config_filenames"].get<vector<std::string>>();
        int limit_num_files = cfg["limit_num_files"].get<int>();

        if (sample_config_filenames.size() == 0) {
            cout << "Error! Need at least one sample config file. Exiting..." << endl;
            return 0;
        }
            
        for (auto sample_config_filename : sample_config_filenames) {

            cout << endl << "Processing file " << sample_config_filename << endl << endl;

            std::ifstream json_file(sample_config_filename);
            json in_json;
            json_file >> in_json;
            json out_json = in_json;

            for (auto const & [sample, props] : samples) {

                // make sure this sample is in the current file
                if (out_json.find(sample.Data()) == out_json.end())
                    continue;

                // only run over samples with no sum_gen_wgts calculated yet (== 0)
                if (props.sum_gen_wgt > 0.1) continue;

                cout << "Processing sample " << sample << ", " << props.filenames.size() << " files" << endl;

                TChain * data_gen = new TChain("ntuples_gbm/genT");

                int count = 0;
                for (auto const & filename : props.filenames) {
                    if (count++ >= limit_num_files && limit_num_files != -1) continue;
                    data_gen->Add(filename);
                }

                data_gen->SetBranchStatus("*", 0);
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

            TString out_filename = TString(sample_config_filename).ReplaceAll(".json", "_weights.json");
            std::ofstream out(out_filename);
            out << std::setw(4) << out_json << endl;
        }
        return 0;
    }

} // end namespace macro