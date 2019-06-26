void makeAllCutflowPlots() {

    // For batch mode run with:
    // root -l -b -x makeAllCutflowPlots.C 
    // and set the save option to true
    //
    // For graphical mode omit the "-b" and optionally set save option to false

    // syntax: cutflowPlots(branch_variable, axis_description, nbins, lowx, highx, save=boolean)

    gROOT->LoadMacro("cutflowPlots.C");
    gROOT->ProcessLine("cutflowPlots(\"reco_mu_pt[0]\", \"Leading muon pT [GeV]\", 50, 0, 30, true)");
    gROOT->ProcessLine("cutflowPlots(\"reco_mu_eta[0]\", \"Leading muon eta\", 10, -2.5, 2.5, true)");
    gROOT->ProcessLine("cutflowPlots(\"reco_mu_phi[0]\", \"Leading muon phi\", 10, -3.2, 3.2, true)");
    gROOT->ProcessLine("cutflowPlots(\"reco_PF_jet_pt[0]\", \"Leading jet pT [GeV]\", 50, 0, 500, true)");
    gROOT->ProcessLine("cutflowPlots(\"reco_PF_jet_eta[0]\", \"Leading jet eta\", 10, -2.5, 2.5, true)");
    gROOT->ProcessLine("cutflowPlots(\"reco_PF_jet_phi[0]\", \"Leading jet phi\", 10, -3.2, 3.2, true)");
    gROOT->ProcessLine("cutflowPlots(\"reco_PF_MET_pt[0]\", \"MET pT [GeV]\", 50, 0, 500, true)");
    gROOT->ProcessLine("cutflowPlots(\"reco_PF_MET_phi[0]\", \"MET phi\", 10, -3.2, 3.2, true)");


}
