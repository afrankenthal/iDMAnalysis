#ifndef COMMON_H
#define COMMON_H

namespace common {

    typedef struct SignalInfo {
        TString m1;
        TString dmchi;
        TString delta;
        int color;
    } SignalInfo;

    typedef struct SampleInfo {
        std::vector<TString> filenames;
        TString label;
        Float_t xsec;
        Float_t sum_gen_wgt;
        Int_t limit_num_files;
        Float_t weight;
        TString group;
        int color;
        int style;
    } SampleInfo;

    void printTimeElapsed(TDatime begin) {
        TDatime end;
        std::cout << "Time elapsed: " << (end.Convert() - begin.Convert()) << " seconds"
            << " or " << (end.Convert() - begin.Convert())/3600.0 << " hours" << std::endl;
    }

    //vector<TString> listFiles(SampleInfo & sample, const char *dirname="", const char *ext=".root") {
    std::vector<TString> listFiles(const char *dirname="", const char *ext=".root") {
        TSystemDirectory dir(dirname, dirname); 
        // First check if dirname is actually just a single .root file
        if (TString(dirname).EndsWith(TString(ext)))
            return std::vector<TString>{TString(dirname)};
        std::vector<TString> filenames;
        TList *files = dir.GetListOfFiles(); 
        if (files) { 
            TSystemFile *file; 
            TString fname; 
            TIter next(files); 
            while ((file=(TSystemFile*)next())) { 
                fname = file->GetName(); 
                if (!file->IsDirectory() && fname.EndsWith(ext)) 
                    filenames.push_back(Form("%s/%s", dirname, fname.Data()));
                    //sample.filenames.push_back(Form("%s/%s", dirname, fname.Data()));
            }
        } 
        return filenames;
    }

    bool readCutBit(uint32_t cuts, int bit) { return cuts & (1 << bit); }

}

#endif // COMMON_H
