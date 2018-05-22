#!/usr/bin/env python

# ploting trigger efficiency of 
# different dark photon decay length samples

import ROOT
ROOT.gROOT.SetBatch(True)
import numpy as np

data = {
    0.1: '../test/gentrigrecoeffirun/histo_0p1cm.root',
    1   : '../test/gentrigrecoeffirun/histo_1cm.root',
    10  : '../test/gentrigrecoeffirun/histo_10cm.root',
    50  : '../test/gentrigrecoeffirun/histo_50cm.root',
    100 : '../test/gentrigrecoeffirun/histo_100cm.root',
    300 : '../test/gentrigrecoeffirun/histo_300cm.root'
}

infs = {l: ROOT.TFile(data[l]) for l in data.keys()}

denom = ROOT.TH1F('denom','',50,0,120)
numer = ROOT.TH1F('numer','',50,0,120)

inf = infs[100]
effs = {}
# for k, inf in infs.iteritems():
for mu in inf.trigeffiana.Get('Accepted/globalMuons'):
    denom.Fill(mu.pt)
for mu in inf.trigeffiana.Get('HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx_v10/globalMuons'):
    numer.Fill(mu.pt)
effi = ROOT.TEfficiency(numer, denom)
    # effs[k].SetTitle('{0}cm'.format(k))

# def extractTEff(effi):
c = ROOT.TCanvas('c','', 600,500)
effi.Draw('ap')
ROOT.gPad.Update()

asymm = effi.GetPaintedGraph()
tot = effi.GetTotalHistogram()
nBins = tot.GetNbinsX()

xx = np.array([tot.GetXaxis().GetBinCenter(i) for i in xrange(1, nBins)])
yy = np.array([effi.GetEfficiency(effi.GetGlobalBin(i)) for i in xrange(1, nBins)])
xe = np.array([asymm.GetErrorX(i) for i in xrange(1, nBins)])
ye = np.array([asymm.GetErrorY(i) for i in xrange(1, nBins)])

    # c.Draw()
    # c.Print('../test/gentrigrecoeffiResults/test{0}.pdf'.format(effi.GetTitle()))
data = {'x':xx, 'y':yy, 'xerr':xe, 'yerr':ye}

# data = {i: extractTEff(ie) for i, ie in effs.iteritems()}
import matplotlib.pyplot as plt
plt.style.use('bmh')
fig = plt.figure(0)
# for l in sorted(data.keys()):
plt.errorbar(x=data['x'], y=data['y'],
             xerr=data['xerr'], yerr=data['yerr'],
             fmt='o', label='10 cm')
plt.legend(loc='best')
plt.title('trigger efficiency')
plt.xlabel('pT [GeV]')
plt.ylabel('Efficiency')

outfn = '../test/gentrigrecoeffiResults/testplt.pdf'

import os
if not os.path.isdir('../test/gentrigrecoeffiResults'):
    os.mkdir('../test/gentrigrecoeffiResults')
plt.savefig(outfn)


# outfn = '../test/gentrigrecoeffiResults/trigEffi.pdf'
# c.Print(outfn)