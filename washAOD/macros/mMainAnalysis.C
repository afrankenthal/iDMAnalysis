#include "mMainAnalysis.h"

namespace macro {

    bool process(map<TString, SampleInfo> samples, vector<CutInfo> cuts_info, json cfg) {

        // for main analysis macro cuts vector needs to be non-empty
        if (cuts_info.size() == 0) {
            cout << "Error! Main analysis requires specification of cuts. None found. Returning..." << endl;
            return 0;
        }

        setTDRStyle();

        map<TString, vector<Double_t>> cutsInclusive;
        map<TString, vector<Double_t>> cutsGroupInclusive;

        map<TString, THInfo*> histos_info;

        // macro options

        json plots_cfg = cfg["plots"];
        for (auto & plot : plots_cfg) 
            histos_info[plot["name"].get<std::string>()] = new THInfo{
                    plot["quantity"].get<std::string>(),
                    plot["groups"].get<std::vector<std::string>>(),
                    plot["cuts"].get<std::vector<int>>(),
                    plot["type"].get<std::string>(),
                    plot["name"].get<std::string>(), 
                    plot["title"].get<std::string>(),
                    plot["nbinsX"].get<int>(),
                    plot["lowX"].get<double>(),
                    plot["highX"].get<double>(),
                    // optional params --> 2D plot
                    plot.value("nMinus1CutDescription", ""),
                    plot.value("quantity2", ""),
                    plot.value("nbinsY", -1), 
                    plot.value("lowY", -1.0),
                    plot.value("highY", -1.0)
                    };

        TString out_filename = TString(cfg["outfilename"].get<std::string>());
        TString region = TString(cfg["region"].get<std::string>());
//	int year = cfg["year"].get<int>();
        
	map<TString,map<int, map<common::MODE, map<int, vector<TH1*>>>>> all_hstacks; // THStack objects, indices: name of hist, mode (bkg/data/sig), cut number
        //map<TString, map<common::MODE, map<int, vector<ROOT::RDF::RResultPtr<TH1D>>>>> all_hstacks; // THStack objects, indices: name of hist, mode (bkg/data/sig), cut number
        for (auto & [name, info] : histos_info) {
            for (auto cut : info->cuts) {
                all_hstacks[name][2017][common::BKG][cut] = vector<TH1*>();
                all_hstacks[name][2017][common::DATA][cut] = vector<TH1*>();
                all_hstacks[name][2017][common::SIGNAL][cut] = vector<TH1*>();
                all_hstacks[name][2018][common::BKG][cut] = vector<TH1*>();
                all_hstacks[name][2018][common::DATA][cut] = vector<TH1*>();
                all_hstacks[name][2018][common::SIGNAL][cut] = vector<TH1*>();
                //all_hstacks[name][common::BKG][cut] = vector<TH1*>();
                //all_hstacks[name][common::DATA][cut] = vector<TH1*>();
               // all_hstacks[name][common::SIGNAL][cut] = vector<TH1*>();
            }
        }

//        //Float_t lumi = 14.7 * 1000; // 1/pb
//        Float_t lumi;
//        if (year == 2017){
//		lumi = 41.53 * 1000; // 1/pb
//	}
//        if (year == 2016){
//		lumi = 35.92 * 1000; // 1/pb
//	}
//	else {
//        	lumi = 59.74 * 1000; // 1/pb
//	}
        //Float_t lumi = 29.41 * 1000; // 1/pb

        // This way of loading the selector forces ACliC compilation, used with old rudimentary build system (compile_macro.sh in scripts)
        //mainAnalysisSelector * MCSelector = (mainAnalysisSelector*)TSelector::GetSelector("TSelectors/mainAnalysisSelector.C+");
        //mainAnalysisSelector * dataSelector = (mainAnalysisSelector*)TSelector::GetSelector("TSelectors/mainAnalysisSelector.C+");

        // This way just uses the new CMake build system which already compiles the selector (see CMakeLists.txt)
        mainAnalysisSelector * MCSelector = new mainAnalysisSelector();
        mainAnalysisSelector * dataSelector = new mainAnalysisSelector();

        MCSelector->SetMode(common::SIGNAL);
        dataSelector->SetMode(common::DATA);

        MCSelector->SetHistos(histos_info);
        dataSelector->SetHistos(histos_info);

        RDFAnalysis * dfAnalysis = new RDFAnalysis();
        dfAnalysis->SetHistos(histos_info);
        dfAnalysis->SetCuts(cuts_info);

        // Process each sample at a time (e.g. QCD_HT100-200)
        // TODO make this parallel
        // My first unsuccessful attempt:
        //#pragma omp parallel for num_threads(4)
        //for (int i = 0; i < samples.size(); i++) {
        //    //std::cout << "Thread number " << omp_get_thread_num() << std::endl;
        //    auto samplesIt = samples.begin();
        //    advance(samplesIt, i);
        //    auto & sample = samplesIt->first;
        //    auto & props = samplesIt->second;
        for (auto const & [sample, props] : samples) { 
            //bool isData = (props.sum_gen_wgt < 0);
            bool isData = (props.mode == common::DATA);
            bool doSubsetOnly = (props.limit_num_files != -1);
	    int year = props.year;

            cout << "Sample: " << sample << endl;
            if (props.sum_gen_wgt < 0.1 && !isData) continue; // don't have data yet for these samples, continue

            TChain * data_reco = new TChain("ntuples_gbm/recoT");
            TChain * data_gen = new TChain("ntuples_gbm/genT");
            int count = 0;
            for (auto const & filename : props.filenames) {
                if (count++ >= props.limit_num_files && doSubsetOnly) continue;
                data_reco->Add(filename.Data());
                data_gen->Add(filename.Data());
            }

            mainAnalysisSelector * currentSelector;

            if (!isData) {
                data_reco->AddFriend(data_gen);
                currentSelector = MCSelector;
            }
            else  {
                currentSelector = dataSelector;
            }
            
            //currentSelector->SetHistos(histos_info);

            //currentSelector->SetParams(props,/* lumi,*/ region); // obsolete
            dfAnalysis->SetParams(props, /*lumi,*/ region/*,year*/);
            
            // Use RDataFrame instead
            //data_reco->Process(currentSelector);

            dfAnalysis->Process(data_reco);

            vector<ROOT::RDF::RResultPtr<ROOT::RDF::RDFDetail::SumReturnType_t<double>>> cutflow_ptr;
            cutflow_ptr = dfAnalysis->GetCutflow();
            vector<double> cutflow;
            //cutflow = currentSelector->GetCutflow();
            for (auto cut : cutflow_ptr)
                cutflow.push_back(cut.GetValue());

            // if first sample in group, create entry
            if (cutsGroupInclusive.find(props.group) == cutsGroupInclusive.end())
                cutsGroupInclusive.insert(std::make_pair(props.group, cutflow));
            else // otherwie, sum std::vectors element-wise
                std::transform(cutsGroupInclusive[props.group].begin(), cutsGroupInclusive[props.group].end(), 
                        cutflow.begin(), cutsGroupInclusive[props.group].begin(), std::plus<double>());

            //map<TString, map<int, TH1*>> sample_histos = currentSelector->GetHistograms();
            map<TString, map<int, ROOT::RDF::RResultPtr<TH1D>>> all_sample_histos_1D = dfAnalysis->GetHistograms1D();
            map<TString, map<int, ROOT::RDF::RResultPtr<TH2D>>> all_sample_histos_2D = dfAnalysis->GetHistograms2D();
            for (auto & [histo_name, histos_by_cut] : all_sample_histos_2D) {
                for (auto & [cut, histo] : histos_by_cut) {
                    bool kFound = false;
                    for (TH1 * existing_histo : all_hstacks[histo_name][props.year][props.mode][cut]) {
                        if (TString(existing_histo->GetName()) == TString(histo->GetName())) { // hist already exists in stack
                            histo->Sumw2();
                            existing_histo->Add(histo.GetPtr());
                            kFound = true;
                        }
                    }
                    if (!kFound) { // first time hist is referenced, so add to stack
                        TH1 * h_pointer = (TH1*)(histo.GetPtr())->Clone();
                        h_pointer->Sumw2();
                        h_pointer->SetDirectory(0);
                        all_hstacks[histo_name][props.year][props.mode][cut].push_back(h_pointer);
                    }
                }
            }
            for (auto & [histo_name, histos_by_cut] : all_sample_histos_1D) {
                for (auto & [cut, histo] : histos_by_cut) {
                    bool kFound = false;
                    for (TH1 * existing_histo : all_hstacks[histo_name][props.year][props.mode][cut]) {
                        if (TString(existing_histo->GetName()) == TString(histo->GetName())) { // hist already exists in stack
                            histo->Sumw2();
                            existing_histo->Add(histo.GetPtr());
                            kFound = true;
                        }
                    }
                    if (!kFound) { // first time hist is referenced, so add to stack
                        TH1 * h_pointer = (TH1*)(histo.GetPtr())->Clone();
                        h_pointer->Sumw2();
                        h_pointer->SetDirectory(0);
                        if (props.mode == common::BKG) {
                            h_pointer->SetFillColor(common::group_plot_info[props.group].color);
                        }
                        else if (props.mode == common::DATA) {
                            h_pointer->SetMarkerColor(common::group_plot_info[props.group].color);
                            h_pointer->SetMarkerStyle(common::group_plot_info[props.group].style);
                            h_pointer->SetMarkerSize(0.9);
                        }
                        else if (props.mode == common::SIGNAL) {
                            h_pointer->SetLineColor(common::group_plot_info[props.group].color);
                            h_pointer->SetLineStyle(common::group_plot_info[props.group].style);
                            h_pointer->SetLineWidth(2);
                            h_pointer->SetMarkerSize(0);
                        }
                        all_hstacks[histo_name][props.year][props.mode][cut].push_back(h_pointer);
                    }
                }
            }
        }

        // Save histograms into THStack objects

	bool mult_year = false;
	int cur_year=0;
        TFile * outfile = new TFile(out_filename, "RECREATE");
        //for (auto & [plot_name, modes] : all_hstacks) {
        for (auto & [plot_name, years] : all_hstacks) {
        for (auto & [year, modes] : years) {
		int newyear = year;
            for (auto & [mode, cuts] : modes) {
                for (auto & [cut, hist_vec] : cuts) {
                    THStack * hstack;
                    if (mode == common::BKG)
                        hstack = new THStack(Form("%s_cut%d-BKG-yr%d", plot_name.Data(), cut, year), histos_info[plot_name]->title);
                    else if (mode == common::DATA)
                        hstack = new THStack(Form("%s_cut%d-DATA-yr%d", plot_name.Data(), cut, year), histos_info[plot_name]->title);
                    else if (mode == common::SIGNAL)
                        hstack = new THStack(Form("%s_cut%d-SIGNAL-yr%d", plot_name.Data(), cut, year), histos_info[plot_name]->title);
                    //sort by "smallest integral first"
                    std::sort(hist_vec.begin(), hist_vec.end(),
                           [](TH1 *a, TH1 *b) { return a->Integral() < b->Integral(); });
                    //std::sort(hist_vec.begin(), hist_vec.end(),
                     //       [](auto & a, auto & b) { return a->Integral() < b->Integral(); });
                    for (auto hist : hist_vec)
                        hstack->Add(hist);
                    if (hstack->GetNhists() > 0){
                        hstack->Write();
			if (cur_year ==0){
				cur_year=newyear;
			}
			else{ if(cur_year != newyear){mult_year=true;}}
		    }

                    if (!cuts_info[cut].efficiency.Contains("none")){
                        THStack * hstack2;
                        if (mode == common::BKG)
                             hstack2 = new THStack(Form("%s_%scut%d-BKG-yr%d", plot_name.Data(),cuts_info[cut].efficiency.Data(), cut, year), histos_info[plot_name]->title);
                        else if (mode == common::DATA)
                             hstack2 = new THStack(Form("%s_%scut%d-DATA-yr%d", plot_name.Data(),cuts_info[cut].efficiency.Data(), cut, year), histos_info[plot_name]->title);
                        else if (mode == common::SIGNAL)
                             hstack2 = new THStack(Form("%s_%scut%d-SIGNAL-yr%d", plot_name.Data(),cuts_info[cut].efficiency, cut, year), histos_info[plot_name]->title);


                    	//sort by "smallest integral first"
                     	std::sort(hist_vec.begin(), hist_vec.end(),
                        	[](TH1 *a, TH1 *b) { return a->Integral() < b->Integral(); });
                        for (auto hist : hist_vec)
                            hstack2->Add(hist);
                        if (hstack2->GetNhists() > 0){
                            hstack2->Write();
			    if (cur_year ==0){
				cur_year=newyear;
			    }
			else{ if(cur_year != newyear){mult_year=true;}}
			}
                    }
                }
            }
	}
        }
	if(mult_year){
        for (auto & [plot_name, years] : all_hstacks) {
        for (auto & [year, modes] : years) {
		int yearsum = 1718;
		if (year == 2018) {continue;} //avoid double counting
            for (auto & [mode, cuts] : modes) {
                for (auto & [cut, hist_vecx] : cuts) {
                    THStack * hstack;
                    if (mode == common::BKG)
                        hstack = new THStack(Form("%s_cut%d-BKG-yr%d", plot_name.Data(), cut, yearsum), histos_info[plot_name]->title);
                    else if (mode == common::DATA)
                        hstack = new THStack(Form("%s_cut%d-DATA-yr%d", plot_name.Data(), cut, yearsum), histos_info[plot_name]->title);
                    else if (mode == common::SIGNAL)
                        hstack = new THStack(Form("%s_cut%d-SIGNAL-yr%d", plot_name.Data(), cut, yearsum), histos_info[plot_name]->title);
                    //sort by "smallest integral first"
                    vector<TH1*> hist_vec = hist_vecx;
                    vector<TH1*> hist_vec1 = all_hstacks[plot_name][2017][mode][cut];
		    for (auto hist1 : hist_vec){
			TString name1;
			TString name2 = hist1->GetName();
			name1 = (((TObjString*)(name2.Tokenize("y")->At(0)))->String());
			TList *thislist = new TList;
		    for (auto hist2 : hist_vec1){
			TString name1x;
			TString name2x = hist2->GetName();
			name1x = (((TObjString*)(name2x.Tokenize("y")->At(0)))->String());
			if (name1.EqualTo(name1x)) {
			thislist->Add(hist2);
			//std::cout<< "names: "<< name1.Data() << "namesx: "<< name1x.Data()<<std::endl;	
			}
			//name2.Tokenize(name1, 0, "_yr")
			//std::cout<< "name1: "<<name1.Data() <<std::endl;
			//std::cout<< "name2: "<<name2.Data() <<std::endl;
			}
			hist1->Merge(thislist);
		    }
		    //hist_vec.insert(hist_vec.end(),hist_vec1.begin(),hist_vec1.end());
                    std::sort(hist_vec.begin(), hist_vec.end(),
                           [](TH1 *a, TH1 *b) { return a->Integral() < b->Integral(); });
                    //std::sort(hist_vec.begin(), hist_vec.end(),
                     //       [](auto & a, auto & b) { return a->Integral() < b->Integral(); });
                    for (auto hist : hist_vec)
                        hstack->Add(hist);
                    if (hstack->GetNhists() > 0){
                        hstack->Write();
		    }
		}
		}
	}
	}
	}
        outfile->Close();

        // Print Latex cutflow table to file
        
        out_filename.ReplaceAll(".root", "_cutflow.txt");
        std::ofstream cutflow_file(out_filename.Data());

        cutflow_file << "Cut# & Description & Data & Bkg";
        for (auto const & [group, all_cut_numbers] : cutsGroupInclusive)
            if (group != TString("data"))
                cutflow_file << " & " << group;

        auto first_vec = cutsGroupInclusive.begin();
        size_t vec_size = (cutsGroupInclusive.size() > 0) ? first_vec->second.size() : 0;

        for (size_t cut = 0; cut < vec_size; cut++) {
            cutflow_file << endl << "cut" << cut << " & " << cuts_info[cut].description;

            Double_t total_background = 0.0;
            for (auto const & [group, all_cut_numbers] : cutsGroupInclusive)
                if (group != TString("data") && (!group.Contains("sig_"))) // only sum bkgs
                    total_background += all_cut_numbers[cut];

            Double_t total_data = (cutsGroupInclusive.find("data") != cutsGroupInclusive.end()) ? 
                cutsGroupInclusive["data"][cut] : 0.0;
            cutflow_file << " & " << total_data;
            cutflow_file << " & " << total_background;

            for (auto const & [group, all_cut_numbers] : cutsGroupInclusive)
                if (group != TString("data"))
                    cutflow_file << " & " << all_cut_numbers[cut];

            cutflow_file << " \\\\ ";
        }

        cutflow_file.close();
        cutflow_file.clear();
        
        // Print csv cutflow table to file and cout

        out_filename.ReplaceAll(".txt", ".csv");
        cutflow_file.open(out_filename.Data());

        cout << "Inclusive cutflow:" << endl;
        cout << "Cut# Description Data Bkg";
        cutflow_file << "Cut# Description Data Bkg";
        for (auto const & [group, all_cut_numbers] : cutsGroupInclusive) {
            if (group == TString("data")) continue;
            cout << " " << group;
            cutflow_file << " " << group;
        }

        first_vec = cutsGroupInclusive.begin();
        vec_size = (cutsGroupInclusive.size() > 0) ? first_vec->second.size() : 0;

        cout << std::setprecision(1) << std::fixed;
        for (size_t cut = 0; cut < vec_size; cut++) {
            cout << endl << "cut" << cut << " " << "\"" << cuts_info[cut].description << "\"";
            cutflow_file << endl << "cut" << cut << " " << "\"" << cuts_info[cut].description << "\"";

            Double_t total_background = 0.0;
            for (auto const & [group, all_cut_numbers] : cutsGroupInclusive)
                if (group != TString("data") && (!group.Contains("sig_"))) // only sum bkgs
                    total_background += all_cut_numbers[cut];

            Double_t total_data = (cutsGroupInclusive.find("data") != cutsGroupInclusive.end()) ? 
                cutsGroupInclusive["data"][cut] : 0.0;
            cutflow_file << " " << total_data;
            cutflow_file << " " << total_background;
            cout << " " << total_data;
            cout << " " << total_background;

            for (auto const & [group, all_cut_numbers] : cutsGroupInclusive) {
                if (group == TString("data")) continue;
                cout << " " << all_cut_numbers[cut];
                cutflow_file << " " << all_cut_numbers[cut];
            }
        }

        return 0;
    }
}
