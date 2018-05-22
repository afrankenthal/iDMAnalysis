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

import sys
muName = sys.argv[1]
# standAloneMuons, refittedStandAloneMuons,
# displacedStandAloneMuons, globalMuons,
# displacedGlobalMuons, recoMuons

effs = {}
for k, inf in infs.iteritems():
    denom = ROOT.TH1F('denom{0}'.format(k),'',50,0,120)
    numer = ROOT.TH1F('numer{0}'.format(k),'',50,0,120)

    for mu in inf.trigeffiana.Get('Accepted/{0}'.format(muName)):
        denom.Fill(mu.pt)
    for mu in inf.trigeffiana.Get('HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx_v10/{0}'.format(muName)):
        numer.Fill(mu.pt)

    effs[k] = ROOT.TEfficiency(numer, denom)
    effs[k].SetTitle('{0}cm'.format(k))


def extractTEff(effi):
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

    return {'x':xx, 'y':yy, 'xerr':xe, 'yerr':ye}

data = {i: extractTEff(ie) for i, ie in effs.iteritems()}

import matplotlib.pyplot as plt
plt.style.use('ggplot')


fig = plt.figure(0)
for l in sorted(data.keys()):
    plt.errorbar(x=data[l]['x'], y=data[l]['y'],
                 xerr=data[l]['xerr'], yerr=data[l]['yerr'],
                 fmt='o', label=str(l)+' cm')
plt.legend(loc='best', prop={'size': 8})
plt.title('HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx_v10')
plt.xlabel('{0} pT [GeV]'.format(muName))
plt.ylabel('Efficiency')
axes = plt.gca()
axes.set_ylim([0,1])

outfn = '../test/gentrigrecoeffiResults/trigEffi_{0}.pdf'.format(muName)

import os
if not os.path.isdir('../test/gentrigrecoeffiResults'):
    os.mkdir('../test/gentrigrecoeffiResults')
plt.savefig(outfn)
