void makeAllCutflowPlots() {

    // Usage: cutflowPlots(branch_variable, axis_description, nbins, lowx, highx, save=true/false)

    gROOT->LoadMacro("cutflowPlots.C");
    gROOT->ProcessLine("cutflowPlots(\"reco_mu_pt[0]\", \"Leading muon pT [GeV]\", 50, 0, 30, true)");
    gROOT->ProcessLine("cutflowPlots(\"reco_mu_eta[0]\", \"Leading muon eta\", 10, -2.5, 2.5, true)");


}
