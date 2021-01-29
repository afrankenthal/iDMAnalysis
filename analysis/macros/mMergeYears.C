#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string.h>
#include <vector>
using std::cout, std::endl, std::map, std::vector;

#include <TApplication.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TCollection.h>
#include <TCut.h>
#include <TDatime.h>
#include <TFile.h>
#include <TH1F.h>
#include <THStack.h>
#include <TLegend.h>
#include <TString.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>

#include "../utils/CMS_lumi.h"
#include "../utils/common.h"
using namespace common;
#include "../utils/cxxopts.hpp"
#include "../utils/json.hpp"
using json = nlohmann::json;
#include "../utils/tdrstyle.h"

namespace macro {

    void mergeStacks(THStack * hs1, THStack * hs2) {
        TIter next((TList*)(((THStack*)hs1)->GetHists()));
        TH1 * h;
        while ((h = (TH1*)next())) {
            TIter next2((TList*)(((THStack*)hs2)->GetHists()));
            TH1 * h2;
            while ((h2 = (TH1*)next2())) {
                if (TString(h->GetName()) == TString(h2->GetName())) {
                    TList * list = new TList();
                    list->Add(h2);
                    h->Merge(list);
                }
            }
        }
    }

    extern "C" bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {
        setTDRStyle();

        // macro options
        vector<std::string> in_filenames = cfg["infilenames"].get<std::vector<std::string>>();

        if (in_filenames.size() < 2) {
            cout << "ERROR! Need at least 2 input files to merge. Exiting..." << endl;
            return 0;
        }

        TString out_filename = TString(cfg["outfilename"].get<std::string>());
        if (out_filename == TString(""))
            out_filename = "merged_stacks.root";
        	
        map<TString, THStack*> all_stacks;

        for (auto in_filename : in_filenames) {
            cout << endl << "Opening file " << in_filename << endl << endl;
            auto * in_file = new TFile(in_filename.c_str());
            for (auto && keyAsObj : *in_file->GetListOfKeys()) {
                auto key = (TKey*)keyAsObj;
                if (TString(key->GetClassName()) != "THStack") continue;
                TString hs_name = TString(key->GetName());
                if (hs_name.Contains("num") or hs_name.Contains("denom")) continue; // mMake1DEffPlotsFromFile handles these            
                cout << "Processing " << hs_name << ", class " << key->GetClassName() << endl;
                THStack * hs = (THStack*)in_file->Get(hs_name);
                TString hs_basename = ((TObjString*)(hs_name.Tokenize("-")->At(0)))->String();
                TString hs_suffix = ((TObjString*)(hs_name.Tokenize("-")->At(1)))->String();

                if (all_stacks.find(hs_name) != all_stacks.end()) { // merge existing stack
                    mergeStacks(all_stacks[hs_name], hs);
                }
                else { // first time this stack is seen
                    all_stacks[hs_name] = hs;
                }
            }
            in_file->Close();
        }

        TFile * out_file;
        if (out_filename == "" || out_filename.Data() == in_filenames[0].c_str())
            out_file = new TFile(in_filenames[0].c_str(), "UPDATE");
        else
            out_file = new TFile(out_filename, "RECREATE");

        out_file->cd();
        // Write updated stacks to ROOT file
        for (auto & pair : all_stacks) {
            pair.second->Write();
        }

        out_file->Close();

        return 0;
    }
}
