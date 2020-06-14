TString formatName(TString name, TString f) {
    TString forward = (f.Contains("f") ? "forward" : "backward");
    if (name.Contains("52p5"))
        return TString(Form("S/#sqrt{B}, %s, (50, 55) GeV, 100 mm", forward.Data()));
    if (name.Contains("5p25"))
        return TString(Form("S/#sqrt{B}, %s, (5, 5.5) GeV, 1000 mm", forward.Data()));
    if (name.Contains("6p0"))
        return TString(Form("S/#sqrt{B}, %s, (5, 7) GeV, 10 mm", forward.Data()));
    if (name.Contains("60p0"))
        return TString(Form("S/#sqrt{B}, %s, (50, 70) GeV, 1 mm", forward.Data()));
    return TString("");
}

void scanSensitivity(TString filename_bkg, TString filename_sig = "") {
    gROOT->LoadMacro("tdrstyle.C");
    gROOT->ProcessLine("setTDRStyle();");

    if (filename_bkg == "") {
        std::cout << "ERRRO! Need at least one filename!" << endl;
        return;
    }

    TFile * file_bkg = new TFile(filename_bkg.Data(), "READ");
    TFile * file_sig;
    if (filename_sig != "")
        file_sig = new TFile(filename_sig.Data(), "READ");
    else
        file_sig = file_bkg;
    
    TIter next((TList*)file_bkg->GetListOfKeys());
    TKey * key;
    while ((key = (TKey*)next())) {
        if (TString(key->GetClassName()) != TString("THStack") || TString(key->GetName()).Contains("SIGNAL")) {
            continue;
        }
        if (!TString(key->GetName()).Contains("cut27"))// &&
        //!TString(key->GetName()).Contains("cut27") &&
        //!TString(key->GetName()).Contains("cut28") &&
        //!TString(key->GetName()).Contains("cut29") &&
        //!TString(key->GetName()).Contains("cut30"))
            continue;
        if (!TString(key->GetName()).Contains("jets"))
            continue;
        //cout << "class name " << key->GetClassName() << endl;
        cout << " name " << key->GetName() << endl;
        TString name = key->GetName();
        name.ReplaceAll("-BKG", "");

        THStack * hs_bkg = (THStack*)key->ReadObj();
        THStack * hs_sig = (THStack*)file_sig->Get(name + TString("-SIGNAL"));

        TString title = hs_sig->GetTitle();

        TIter next2(hs_bkg->GetHists());
        cout << "BKG" << endl;
        TH1F * h;
        std::map<TString, TH1*> hcs_bkg;
        std::map<TString, double> integrals_bkg;
        while ((h = (TH1F*)next2())) {
            cout << " hist name : " << h->GetName() << endl;
            cout << " hist nentries : " << h->GetEntries() << endl;
            TH1 * hc = (TH1*)h->GetCumulative();
            hcs_bkg[TString(h->GetName())] = hc;
            integrals_bkg[TString(h->GetName())] = h->Integral(0, h->GetNbinsX());
        }

        TIter next3(hs_sig->GetHists());
        cout << "SIGNAL" << endl;
        std::map<TString, TH1*> hcs_sig;
        std::map<TString, double> integrals_sig;
        while ((h = (TH1F*)next3())) {
            cout << " hist name : " << h->GetName() << endl;
            cout << " hist nentries : " << h->GetEntries() << endl;
            TH1 * hc = (TH1*)h->GetCumulative();
            hcs_sig[TString(h->GetName())] = hc;
            integrals_sig[TString(h->GetName())] = h->Integral(0,h->GetNbinsX());
        }

        // Find optimal bin for each signal point

        int nbins = -1;
        if (hcs_sig.size() > 0)
            nbins = hcs_sig.begin()->second->GetNbinsX();

        std::map<int, double> tot_bkg_forward;
        std::map<int, double> tot_bkg_backward;
        for (int i = 1; i <= nbins; i++) {
            tot_bkg_forward[i] = 0.0;
            tot_bkg_backward[i] = 0.0;
            for (auto & [name, hc] : hcs_bkg) {
                tot_bkg_forward[i] += hc->GetBinContent(i);
                tot_bkg_backward[i] += integrals_bkg[name] - hc->GetBinContent(i);
            }
            //for (auto & [name, hc] : hcs_sig) {
            //    hc->SetBinContent(i, hc->GetBinContent(i)/sqrt(tot_bkg));
            //}
        }

        std::map<TString, int> best_bin_forward;
        std::map<TString, double> best_ratio_forward;
        std::map<TString, int> best_bin_backward;
        std::map<TString, double> best_ratio_backward;
        for (auto & [name, hc] : hcs_sig) {
            best_ratio_forward[name] = 0.0;
            best_ratio_backward[name] = 0.0;
        }

        std::map<TString, TH1F*> ratios_forward;
        std::map<TString, TH1F*> ratios_backward;
        for (int i = 1; i <= nbins; i++) {
            for (auto & [name, hc] : hcs_sig) {
                if (i == 1) {
                    ratios_forward[name] = (TH1F*)hc->Clone();
                    ratios_forward[name]->Reset();
                    ratios_forward[name]->SetName(Form("ratio_forward_%s", name.Data()));
                    ratios_forward[name]->SetTitle(formatName(name,"f"));
                    //ratios_forward[name] = new TH1F(Form("ratio_forward_%s", name.Data()), formatName(name,"f") , nbins, hc->GetBinLowEdge(1), hc->GetBinHighEdge(nbins));
                    //ratios_backward[name] = new TH1F(Form("ratio_backward_%s", name.Data()), formatName(name,"b"), nbins, hc->GetBinLowEdge(1), hc->GetBinLowEdge(nbins+1));
                    ratios_backward[name] = (TH1F*)hc->Clone();
                    ratios_backward[name]->Reset();
                    ratios_backward[name]->SetName(Form("ratio_backward_%s", name.Data()));
                    ratios_backward[name]->SetTitle(formatName(name,"b"));
                }
                // check forward
                ratios_forward[name]->SetBinContent(i, hc->GetBinContent(i)/sqrt(tot_bkg_forward[i]+0.0001));
                if (ratios_forward[name]->GetBinContent(i) > best_ratio_forward[name]) {
                    best_ratio_forward[name] = ratios_forward[name]->GetBinContent(i);
                    best_bin_forward[name] = i;
                }
                // check backward
                ratios_backward[name]->SetBinContent(i, (integrals_sig[name] - hc->GetBinContent(i))/sqrt(tot_bkg_backward[i]+0.0001));
                if (ratios_backward[name]->GetBinContent(i) > best_ratio_backward[name]) {
                    best_ratio_backward[name] = ratios_backward[name]->GetBinContent(i);
                    best_bin_backward[name] = i;
                }
            }
        }

        for (auto & [name, hc] : hcs_sig) {
            cout << "Best forward bin for signal " << name << ": " << best_bin_forward[name];
            cout << ", corresponds to bin center " << hc->GetBinCenter(best_bin_forward[name]) << endl;
            cout << "Best backward bin for signal " << name << ": " << best_bin_backward[name];
            cout << ", corresponds to bin center " << hc->GetBinCenter(best_bin_backward[name]) << endl;
        }

        int color = 0;
        THStack * hs_forward = new THStack(Form("%s_forward", name.Data()), Form("%s_forward", name.Data()));
        for (auto & [name, hratio] : ratios_forward) {
            hratio->SetDirectory(0);
            if (name.Contains("60p0"))
                color = kBlack;
            else if (name.Contains("5p25"))
                color = kCyan;
            else if (name.Contains("52p5"))
                color = kGreen-4;
            else
                color = kYellow-3;
            hratio->SetLineColor(color);
            hratio->SetLineWidth(2);
            hratio->Scale(1/(hratio->Integral()));
            hs_forward->Add(hratio);
            //hratio->Draw("SAME HIST");
        }
        color = 0;
        THStack * hs_backward = new THStack(Form("%s_backward", name.Data()), Form("%s_backward", name.Data()));
        for (auto & [name, hratio] : ratios_backward) {
            hratio->SetDirectory(0);
            if (name.Contains("60p0"))
                color = kBlack;
            else if (name.Contains("5p25"))
                color = kCyan;
            else if (name.Contains("52p5"))
                color = kGreen-4;
            else
                color = kYellow-3;
            hratio->SetLineColor(color);
            hratio->SetLineWidth(3);
            hratio->SetLineStyle(2);
            hratio->Scale(1/(hratio->Integral()));
            hs_backward->Add(hratio);
            //hratio->Draw("SAME HIST");
        }
        
        TCanvas * c = new TCanvas();
        hs_forward->Draw("NOSTACK HIST");
        hs_forward->SetTitle("");
        hs_forward->GetXaxis()->SetTitle(title);
        hs_forward->GetYaxis()->SetTitle("A. U.");
        c->BuildLegend(0.4, 0.3, 0.4, 0.3);

        TCanvas * c2 = new TCanvas();
        hs_backward->Draw("NOSTACK HIST");
        hs_backward->SetTitle("");
        hs_backward->GetXaxis()->SetTitle(title);
        hs_backward->GetYaxis()->SetTitle("A. U.");
        c2->BuildLegend(0.4, 0.3, 0.4, 0.3);
        //c2->BuildLegend();//0.5, 0.5, 0.88, 0.85);

    }
    //file->Write();
    file_bkg->Close();
    if (filename_sig != "")
        file_sig->Close();
}
