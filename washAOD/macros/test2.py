import ROOT
ROOT.gROOT.SetBatch(True)

inf = ROOT.TFile('../test/gentrigrecoeffirun/histo_0p1cm.root')

c = ROOT.TCanvas('c','',600,500)
denom = ROOT.TH1F('d','',50,0,120)
numer = ROOT.TH1F('n','',50,0,120)

for mu in inf.trigeffiana.Get('Accepted/displacedStandAloneMuons'):
  denom.Fill(mu.pt)
for mu in inf.trigeffiana.Get('HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx_v10/displacedStandAloneMuons'):
  numer.Fill(mu.pt)

denom.SetLineColor(ROOT.kRed)
numer.SetLineColor(ROOT.kBlue)
denom.Draw()
numer.Draw('same')

c.SaveAs('test2.pdf')
