using std::cout, std::endl, std::map, std::vector;

bool readCutBit(uint32_t cuts, int bit) { return cuts & (1 << bit); }

void cutflowTable() {

    map<TString, TString> files { 
        {"5p25", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-5p25_dMchi-0p5_ctau-100.root"},
        {"52p5", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-52p5_dMchi-5p0_ctau-100.root"},
        {"6p0", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-6p0_dMchi-2p0_ctau-100.root"}, 
        {"60p0", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-60p0_dMchi-20p0_ctau-100.root"}
    };
    map<TString, vector<UInt_t> > cutsIncl {
        {"5p25", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        {"52p5", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        {"6p0", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        {"60p0", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
    };
    map<TString, vector<UInt_t>> cutsExcl {
        {"5p25", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        {"52p5", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        {"6p0", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        {"60p0", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
    };

    vector<int> cutOrder { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};

    for (auto & file : files) {

        TFile * f = TFile::Open(file.second.c_str());
        TTree * recoT = (TTree*)f->Get("ntuples_gbm/recoT");

        uint32_t cuts;
        recoT->SetBranchAddress("cuts", &cuts);

        int nEntries = recoT->GetEntries();
        for (int i = 0; i < nEntries; i++) {
            recoT->GetEntry(i);
            bool cumPass = true;
            for (auto j : cutOrder) {
                if (readCutBit(cuts, j))
                    cutsExcl[file.first][j]++;
                else
                    cumPass = false;
                if (readCutBit(cuts, j) && cumPass)
                    cutsIncl[file.first][j]++;
            }
        }
    }

    cout << "inclusive 5p25 52p5 6p0 60p0" << endl;
    for (auto j : cutOrder) {
        cout << "cut " << j << ": ";
        for (auto file : cutsIncl) {
            cout << cutsIncl[file.first][j] << " ";
        }
        cout << endl;
    }
    cout << endl << "exclusive 5p25 52p5 6p0 60p0" << endl;
    for (auto j : cutOrder) {
        cout << "cut " << j << ": ";
        for (auto file : cutsIncl) {
            cout << cutsExcl[file.first][j] << " ";
        }
        cout << endl;
    }
}
