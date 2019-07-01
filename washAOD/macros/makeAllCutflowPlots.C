void makeAllCutflowPlots() {

    // For batch mode run with:
    // root -l -b -x makeAllCutflowPlots.C 
    // and set the save option to true
    //
    // For graphical mode omit the "-b" and optionally set save option to false

    // syntax: cutflowPlots(branch_variable, axis_description, nbins, lowx, highx, save=boolean)

    gROOT->LoadMacro("cutflowPlots.C");
    gROOT->ProcessLine("cutflowPlots(\"leading_mu_pt\", \"Leading muon pT [GeV]\", 50, 0, 30, true)");
    gROOT->ProcessLine("cutflowPlots(\"leading_mu_eta\", \"Leading muon eta\", 20, -2.5, 2.5, true)");
    gROOT->ProcessLine("cutflowPlots(\"leading_mu_phi\", \"Leading muon phi\", 20, -3.2, 3.2, true)");
    gROOT->ProcessLine("cutflowPlots(\"leading_jet_pt\", \"Leading jet pT [GeV]\", 50, 0, 500, true)");
    gROOT->ProcessLine("cutflowPlots(\"leading_jet_eta\", \"Leading jet eta\", 20, -2.5, 2.5, true)");
    gROOT->ProcessLine("cutflowPlots(\"leading_jet_phi\", \"Leading jet phi\", 20, -3.2, 3.2, true)");
    gROOT->ProcessLine("cutflowPlots(\"MET_pt\", \"MET pT [GeV]\", 50, 0, 500, true)");
    gROOT->ProcessLine("cutflowPlots(\"MET_phi\", \"MET phi\", 20, -3.2, 3.2, true)");
    gROOT->ProcessLine("cutflowPlots(\"MET_mu_dphi\", \"#Delta#Phi(MET, muons) \", 50, -3.2, 3.2, true)");
    gROOT->ProcessLine("cutflowPlots(\"mu_dR\", \"muon #DeltaR\", 50, 0, 6.5, true)");
    gROOT->ProcessLine("cutflowPlots(\"mu_M2\", \"M_{#mu#mu} [Gev]\", 20, 0, 20, true)");

}
