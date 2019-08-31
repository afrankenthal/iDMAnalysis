#include <string.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include <TDatime.h>
#include <TCollection.h>
#include <TSystemFile.h>
#include <TSystemDirectory.h>
#include <TString.h>
#include <TChain.h>
#include <TApplication.h>
#include <TCut.h>
#include <TH1F.h>
#include <TSelector.h>

#include "utils/common.C"
using namespace common;
#include "utils/json.hpp"
using json = nlohmann::json;
#include "utils/cxxopts.hpp"
#include "TSelectors/mainAnalysisSelectorData.h"
#include "TSelectors/mainAnalysisSelectorMC.h"

using std::cout, std::endl, std::map, std::vector;

namespace macro {

    bool mMainAnalysis(map<TString, SampleInfo> samples, json cfg) {

        map<TString, vector<Double_t>> cutsInclusive;
        map<TString, vector<Double_t>> cutsGroupInclusive;

        map<TString, TH1Info*> histos_info;

        // macro options

        json plots_cfg = cfg["plots"];
        for (auto & plot : plots_cfg) 
            histos_info[plot["name"].get<std::string>()] = new TH1Info{
                    plot["quantity"].get<std::string>(),
                    plot["name"].get<std::string>(), 
                    plot["title"].get<std::string>(),
                    plot["nbins"].get<int>(),
                    plot["lowX"].get<double>(),
                    plot["highX"].get<double>()
                    };

        TString outfilename = TString(cfg["outfilename"].get<std::string>());
        TString region = TString(cfg["region"].get<std::string>());
        //TString outfilename = TString(cfg["outfilename"].get<std::string>());
        //TString custom_cut = TString(cfg["param"].get<std::string>());
        //Float_t dR_cut = cfg["value"].get<float>(); 
        //Int_t dR_cut_mask = 1, nHighPtJets_cut_mask = 1;
        //if (custom_cut.Contains("dR"))
        //    dR_cut_mask = 2;
        //else 
        //    nHighPtJets_cut_mask = 2;


        Float_t lumi = 59.97 * 1000; // 1/pb

        // TODO re-run latest ntuplizer with MC so can have single selector class
        mainAnalysisSelectorMC * MCSelector = (mainAnalysisSelectorMC*)TSelector::GetSelector("TSelectors/mainAnalysisSelectorMC.C+");
        mainAnalysisSelectorData * dataSelector = (mainAnalysisSelectorData*)TSelector::GetSelector("TSelectors/mainAnalysisSelectorData.C+");

        MCSelector->SetHistos(histos_info);


        for (auto const & [sample, props] : samples) {

            cout << "sample: " << sample << endl; //", sum_gen_wgt: " << props.sum_gen_wgt << endl;
            if (props.sum_gen_wgt < 0.1 && props.sum_gen_wgt > 0) continue;

            TChain * data_reco = new TChain("ntuples_gbm/recoT");
            TChain * data_gen = new TChain("ntuples_gbm/genT");
            int count = 0;
            for (auto const & filename : props.filenames) {
                if (count++ >= props.limit_num_files && props.limit_num_files != -1) continue;
                data_reco->Add(filename.Data());
                data_gen->Add(filename.Data());
            }

            if (props.sum_gen_wgt >= 0) {
                data_reco->AddFriend(data_gen);
            }

            vector<double> cutflow;

            if (props.sum_gen_wgt >= 0) { // MC

                MCSelector->SetParams(props.sum_gen_wgt, props.xsec, lumi, region);
                data_reco->Process(MCSelector, outfilename);//"mainAnalysisSelectorMC.C+", outfilename);

                cutflow = MCSelector->GetCutflow();
                cutsInclusive.insert(std::make_pair(sample, cutflow));
                if (cutsGroupInclusive.find(props.group) == cutsGroupInclusive.end())
                    cutsGroupInclusive.insert(std::make_pair(props.group, cutflow));
                else
                    std::transform(cutsGroupInclusive[props.group].begin(), cutsGroupInclusive[props.group].end(), cutflow.begin(), cutsGroupInclusive[props.group].begin(), std::plus<double>( ));
                    //cutsGroupInclusive[sample] += cutflow;

                //for (auto val : cutflow)
                //    cout << val << " ";
                //cout << endl;
            }
            else { // data
                
                data_reco->Process(dataSelector);//"mainAnalysisSelectorData.C+");

                //cutflow = DataSelector->GetCutflow();
            }

        }
        cout << std::setprecision(1) << std::fixed;

        cout << "Inclusive cutflow:" << endl;
        for (auto group : cutsGroupInclusive)
            cout << " " << group.first;
        auto first = cutsGroupInclusive.begin();
        size_t vec_size = first->second.size();
        for (size_t cut = 0; cut < vec_size; cut++) {
            cout << endl << "cut " << cut;
            for (auto const & group : cutsGroupInclusive) 
                cout << " & " << cutsGroupInclusive[group.first][cut];
            cout << " \\\\ ";
        }

        return 0;
    }
}
