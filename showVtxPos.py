#! /usr/bin/env python

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

data = {
    0.01: ROOT.TFile('histo_ctau-0p01.root'),
    0.1 : ROOT.TFile('histo_ctau-0p1.root'),
    1.  : ROOT.TFile('histo_ctau-1.root')
}

# data = {
#     13: ROOT.TFile('histo_ctau-13mm.root'),
#     130: ROOT.TFile('histo_ctau-130mm.root'),
#     1300: ROOT.TFile('histo_ctau-1300mm.root')
# }

# data = {
#     0.0001: ROOT.TFile('histo_sa_10.root'),
#     0.001: ROOT.TFile('histo_sa_11.root'),
#     0.01: ROOT.TFile('histo_sa_12.root')
# }

muonCollection = ['GenMuon', 'RecoMuon', 'StandAloneMuon',
'RefittedStandAloneMuon', 'DisplacedStandAloneMuon']

color = [600, 1, 616]

def getLimit(pool, percentage=0.01):
    leftNum = int(len(pool)*percentage)
    rightNum = int(len(pool)*(1.-percentage))
    return (sorted(pool)[leftNum], sorted(pool)[rightNum])

ofn = 'output.pdf'
hists = {}
for i, mu in enumerate(muonCollection):

    c = ROOT.TCanvas(mu, mu, 1200, 300)
    c.Divide(3, 1)
    leg = ROOT.TLegend(0.72 ,0.78, 0.9, 0.9)
    leg.SetBorderSize(0)
    leg.SetFillStyle(1)
    leg.SetFillColor(0)

    c.cd(1)
    c.GetPad(1).SetLogy(True)
    for jd, d in enumerate(sorted(data.keys(), reverse=True)):
        histData = [p.dxy for p in data[d].Get('decaylengthana/'+mu)]
        xmax = getLimit(histData)[1]
        hid = mu+'_dxy_'+str(d)
        hists[hid] = ROOT.TH1D(hid, mu+' dxy', 100, 0, xmax)
        hists[hid].SetDirectory(ROOT.gROOT)
        for p in histData: hists[hid].Fill(p)
        if hists[hid].Integral() > 0: hists[hid].Scale(1./hists[hid].Integral(), 'width')
        hists[hid].SetLineColor(color[jd])
        hists[hid].GetXaxis().SetTitle('dxy [cm]')
        hists[hid].GetYaxis().SetTitle('A.U.')
        hists[hid].Draw('HIST' if jd==0 else 'HIST SAME')
        leg.AddEntry(hists[hid], 'c#tau: %s cm'%str(d), 'l')
    leg.Draw()
    
    c.cd(2)
    c.GetPad(2).SetLogy(True)
    for jd, d in enumerate(data):
        histData = [p.dz for p in data[d].Get('decaylengthana/'+mu)]
        xmin, xmax = getLimit(histData)
        hid = mu+'_dz_'+str(d)
        hists[hid] = ROOT.TH1D(hid, mu+' dz', 100, xmin, xmax)
        hists[hid].SetDirectory(ROOT.gROOT)
        for p in histData: hists[hid].Fill(p)
        if hists[hid].Integral() > 0: hists[hid].Scale(1./hists[hid].Integral(), 'width')
        hists[hid].SetLineColor(color[jd])
        hists[hid].GetXaxis().SetTitle('dxy [cm]')
        hists[hid].GetYaxis().SetTitle('A.U.')
        hists[hid].Draw('HIST' if jd==0 else 'HIST SAME')
    leg.Draw()

    c.cd(3)
    c.GetPad(3).SetLogy(True)
    for jd, d in enumerate(data):
        histData = [p.dl for p in data[d].Get('decaylengthana/'+mu)]
        xmax = getLimit(histData)[1]
        hid = mu+'_dl_'+str(d)
        hists[hid] = ROOT.TH1D(hid, mu+ ' dl', 100, 0, xmax)
        hists[hid].SetDirectory(ROOT.gROOT)
        for p in histData: hists[hid].Fill(p)
        if hists[hid].Integral() > 0: hists[hid].Scale(1./hists[hid].Integral(), 'width')
        hists[hid].SetLineColor(color[jd])
        hists[hid].GetXaxis().SetTitle('dxy [cm]')
        hists[hid].GetYaxis().SetTitle('A.U.')
        hists[hid].Draw('HIST' if jd==0 else 'HIST SAME')
    leg.Draw()

    if i==0: c.Print(ofn+'(')
    elif i==len(muonCollection)-1: c.Print(ofn+')')
    else: c.Print(ofn)