void deleteCanvases(TString filename) {
    if (filename == "") {
        std::cout << "No filename!" << endl;
        return;
    }

    TFile * file = new TFile(filename.Data(), "UPDATE");
    
    TIter next((TList*)file->GetListOfKeys());
    TKey * key;
    key = (TKey*)next();
    while (1) {
        cout << "class name " << key->GetClassName() << endl;
        cout << " name " << key->GetName() << endl;
        if (TString(key->GetClassName()) != TString("TCanvas")) {
            key = (TKey*)next();
            continue;
        }
        //TCanvas * c = (TCanvas*)key->ReadObj();
        key->Delete();
        delete key;
        key = (TKey*)next();
        //c->SetDirectory(0);
        //file->Delete(Form("%s;*", c->GetName()));
        //delete c;
        if (!key) break;
    }
    file->Write();
    file->Close();
}
