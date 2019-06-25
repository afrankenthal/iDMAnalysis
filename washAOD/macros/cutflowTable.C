void cutflowTable() {

    std::map<std::string, std::string> files { 
        {"5p25", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-5p25_dMchi-0p5_ctau-100.root"},
        {"52p5", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-52p5_dMchi-5p0_ctau-100.root"},
        {"6p0", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-6p0_dMchi-2p0_ctau-100.root"}, 
        {"60p0", "root://cmsxrootd.fnal.gov///store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis/iDMAnalysis_Mchi-60p0_dMchi-20p0_ctau-100.root"}
    };
    std::map<std::string, std::vector<UInt_t> > cutsIncl {
        {"5p25", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        {"52p5", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        {"6p0", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        {"60p0", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
    };
    std::map<std::string, std::vector<UInt_t> > cutsExcl {
        {"5p25", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        {"52p5", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        {"6p0", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        {"60p0", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
    };

    std::vector<int> cutOrder { 0, 1, 2, 4, 5, 6, 7, 3, 9, 10, 11, 12, 14 };

    for (auto & file : files) {

        TFile * f = TFile::Open(file.second.c_str());
        TTree * recoT = (TTree*)f->Get("SREffi_dsa/reco");

        UInt_t cutsVec[numCuts_];

        recoT->SetBranchAddress("cutsVec", cutsVec);

        int nEntries = recoT->GetEntries();
        for (int i = 0; i < nEntries; i++) {
            recoT->GetEntry(i);
            int cumPass = 1;
            for (auto j : cutOrder) {
                if (cutsVec[j])
                    cutsExcl[file.first][j]++;
                if (cutsVec[j] && cumPass)
                    cutsIncl[file.first][j]++;
                if (!cutsVec[j])
                    cumPass = 0;
            }
        }
    }

    std::cout << "inclusive 5p25 52p5 6p0 60p0" << std::endl;
    for (auto j : cutOrder) {
        std::cout << "cut " << j << ": ";
        for (auto file : cutsIncl) {
            std::cout << cutsIncl[file.first][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << "exclusive 5p25 52p5 6p0 60p0" << std::endl;
    for (auto j : cutOrder) {
        std::cout << "cut " << j << ": ";
        for (auto file : cutsIncl) {
            std::cout << cutsExcl[file.first][j] << " ";
        }
        std::cout << std::endl;
    }
}
