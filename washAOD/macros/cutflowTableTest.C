
using std::cout, std::endl, std::map, std::vector;

bool readCutBit(uint32_t cuts, int bit) { return cuts & (1 << bit); }

void cutflowTableTest() {

    map<TString, TString> files { 
        {"test", "./test.root"}
    };
    map<TString, vector<UInt_t>> cutsIncl {
        {"test", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }
    };
    map<TString, vector<UInt_t>> cutsExcl {
        {"test", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }
    };

    vector<int> cutOrder { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};

    for (auto & file : files) {

        TFile * f = TFile::Open(file.second.Data());
        TTree * recoT = (TTree*)f->Get("SREffi_gbm/recoT");

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
        for (auto file : cutsExcl) {
            cout << cutsExcl[file.first][j] << " ";
        }
        cout << endl;
    }
}
