#include "mMergeYears.h"

using std::cout, std::endl, std::map, std::vector;

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

    bool process([[maybe_unused]] map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {
        setTDRStyle();

        // macro options
        TString in_filename1 = TString(cfg["infilename1"].get<std::string>());
        TString in_filename2 = TString(cfg["infilename2"].get<std::string>());
        TString in_filename3("");
        if (cfg.find("infilename3") != cfg.end())
            in_filename3 = TString(cfg["infilename3"].get<std::string>());

        if (in_filename1 == TString("") || in_filename2 == TString("")) {
            cout << "ERROR! Need at least 2 input files to merge. Exiting..." << endl;
            return 0;
        }
        TString out_filename;
        if (cfg.find("outfilename") != cfg.end()) 
            out_filename = TString(cfg["outfilename"].get<std::string>());
        else 
            out_filename = "merged_stacks.root";

        vector<TFile*> in_files;
        TFile * out_file;
        if (out_filename == "" || out_filename == in_filename1) {
            in_files.push_back(new TFile(in_filename1, "UPDATE"));
            out_file = in_files[0];
        }
        else {
            in_files.push_back(new TFile(in_filename1));
            in_files.push_back(new TFile(in_filename2));
            if (in_filename3 != TString(""))
                in_files.push_back(new TFile(in_filename3));
            out_file = new TFile(out_filename, "RECREATE");
        }
        	
        map<TString, THStack*> all_stacks;

        for (auto * in_file : in_files) {
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
        }

        out_file->cd();
        // Write updated stacks to ROOT file
        for (auto & pair : all_stacks) {
            pair.second->Write();
        }

        //out_file->Write();
        out_file->Close();
        for (auto * in_file : in_files) {
            in_file->Close();
        }

        return 0;
    }
}
