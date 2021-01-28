#include "TFile.h"
#include "TTree.h"
#include "THStack.h"
#include "TH1F.h"

void puwgt(){
TFile *f=new TFile("../bin/PileupZjets/PUdata.root");
THStack *datastack=(THStack*)f->Get("PUwgt_cut1-DATA-2017");
TH1F * dataHist = (TH1F*)(((TH1F*)datastack->GetStack()->Last())->Clone());
Double_t normdata = dataHist->GetEntries();
dataHist->Scale(1./normdata);

TFile *g1=new TFile("../bin/PileupZjets/PUZjets100.root");
THStack *zjetstack1=(THStack*)g1->Get("PUwgt_cut1-BKG-2017");
TH1F * zJetHist1 = (TH1F*)(((TH1F*)zjetstack1->GetStack()->Last())->Clone());
Double_t norm1 = zJetHist1->GetEntries();
zJetHist1->Scale(1./norm1);

TFile *g2=new TFile("../bin/PileupZjets/PUZjets200.root");
THStack *zjetstack2=(THStack*)g2->Get("PUwgt_cut1-BKG-2017");
TH1F * zJetHist2 = (TH1F*)(((TH1F*)zjetstack2->GetStack()->Last())->Clone());
Double_t norm2 = zJetHist2->GetEntries();
zJetHist2->Scale(1./norm2);

TFile *g3=new TFile("../bin/PileupZjets/PUZjets600.root");
THStack *zjetstack3=(THStack*)g3->Get("PUwgt_cut1-BKG-2017");
TH1F * zJetHist3 = (TH1F*)(((TH1F*)zjetstack3->GetStack()->Last())->Clone());
Double_t norm3 = zJetHist3->GetEntries();
zJetHist3->Scale(1./norm3);

TFile *g4=new TFile("../bin/PileupZjets/PUZjets800.root");
THStack *zjetstack4=(THStack*)g4->Get("PUwgt_cut1-BKG-2017");
TH1F * zJetHist4 = (TH1F*)(((TH1F*)zjetstack4->GetStack()->Last())->Clone());
Double_t norm4 = zJetHist4->GetEntries();
zJetHist4->Scale(1./norm4);

TFile *g5=new TFile("../bin/PileupZjets/PUZjets1200.root");
THStack *zjetstack5=(THStack*)g5->Get("PUwgt_cut1-BKG-2017");
TH1F * zJetHist5 = (TH1F*)(((TH1F*)zjetstack5->GetStack()->Last())->Clone());
Double_t norm5 = zJetHist5->GetEntries();
zJetHist5->Scale(1./norm5);

TFile *g6=new TFile("../bin/PileupZjets/PUZjets2500.root");
THStack *zjetstack6=(THStack*)g6->Get("PUwgt_cut1-BKG-2017");
TH1F * zJetHist6 = (TH1F*)(((TH1F*)zjetstack6->GetStack()->Last())->Clone());
Double_t norm6 = zJetHist6->GetEntries();
zJetHist6->Scale(1./norm6);

//TFile *g=new TFile("../macros/bin/PileupZjets/PUZjets400.root");
//THStack *zjetstack=(THStack*)g->Get("PUwgt_cut1-BKG-2017");
//TH1F * zJetHist = (TH1F*)(((TH1F*)zjetstack->GetStack()->Last())->Clone());
TH1F* zratio1 = (TH1F*)(dataHist->Clone());
TH1F* zratio2 = (TH1F*)(dataHist->Clone());
TH1F* zratio3 = (TH1F*)(dataHist->Clone());
TH1F* zratio4 = (TH1F*)(dataHist->Clone());
TH1F* zratio5 = (TH1F*)(dataHist->Clone());
TH1F* zratio6 = (TH1F*)(dataHist->Clone());
zratio1->Divide(zJetHist1);
zratio2->Divide(zJetHist2);
zratio3->Divide(zJetHist3);
zratio4->Divide(zJetHist4);
zratio5->Divide(zJetHist5);
zratio6->Divide(zJetHist6);

TFile* outfile1 = new TFile("../data/zjetpileup/zjetratio100.root","RECREATE");
zratio1->Write();
outfile1->Close();
TFile* outfile2 = new TFile("../data/zjetpileup/zjetratio200.root","RECREATE");
zratio2->Write();
outfile2->Close();
TFile* outfile3 = new TFile("../data/zjetpileup/zjetratio600.root","RECREATE");
zratio3->Write();
outfile3->Close();
TFile* outfile4 = new TFile("../data/zjetpileup/zjetratio800.root","RECREATE");
zratio4->Write();
outfile4->Close();
TFile* outfile5 = new TFile("../data/zjetpileup/zjetratio1200.root","RECREATE");
zratio5->Write();
outfile5->Close();
TFile* outfile6 = new TFile("../data/zjetpileup/zjetratio2500.root","RECREATE");
zratio6->Write();
outfile6->Close();
}
