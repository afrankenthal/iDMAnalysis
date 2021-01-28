void calc_sum_gen_wgts(std::string filename="") {
    if (filename == "") {
        std::cout << "ERROR! Need a list of filenames to open." << std::endl;
        return;
    }

    std::ifstream inputFile(filename);
    std::vector<std::string> fileList;
    std::string line;

    if(!inputFile) {
        std::cerr << "File list could not be opened\n";
        return 1;
    }

    while(std::getline(inputFile, line)) {
        fileList.push_back(line);
    }

    std::cout << "Number of files to be analyzed: " << fileList.size() << "\n";

    std::vector<std::string>::const_iterator it(fileList.begin());
    std::vector<std::string>::const_iterator end(fileList.end());
    for(; it != end; ++it) {
        //std::ifstream inputTxt(it->c_str());
        auto * rootfile = TFile::Open(it->c_str());
        if(!rootfile) {
            std::cerr << "ROOT file could not be opened:" << *it << "\n";
            return 1;
        }

        auto * gen_tree = (TTree*)rootfile->Get("ntuples_gbm/genT");

        gen_tree->Draw("1>>htemp", "gen_wgt*(1==1)", "goff");

        auto * htemp = (TH1F*)gDirectory->Get("htemp");

        std::cout << "file: " << it->c_str() << ", sum_gen_wgt: " << htemp->Integral() << std::endl;
    }
}
