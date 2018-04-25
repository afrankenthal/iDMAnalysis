#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)

infn = '../ptreso.root'
import sys
if len(sys.argv)>1: infn = sys.argv[1]
inf = ROOT.TFile(infn)
allMuName = [x.GetName() for x in inf.allMuPtResolution.GetListOfKeys()]
intrees = {mu: inf.allMuPtResolution.Get(mu) for mu in allMuName}

from root_numpy import tree2array
inarrays = {mu: tree2array(intrees[mu]) for mu in allMuName}

import matplotlib.pyplot as plt
fig = plt.figure()
ax = fig.add_subplot(1,1,1)
for mu in allMuName:
    ax.hist(inarrays[mu]['res'], bins=50, range=(-0.2,1.2), normed=True, histtype='step', label=mu)
ax.legend(loc='best')

props = {
    'title': r'$p_{T}$ resolution of different muons',
    'xlabel': r'$p_{T}$ resolution',
    'ylabel': 'A.U.'
}
ax.set(**props)

import os
if not os.path.isdir('../test/skimResults'):
	os.mkdir('../test/skimResults')
outfn = '../test/skimResults/' + 'allMuPtReso.pdf'
if len(sys.argv)>2:
	outfn = '../test/skimResults/' + sys.argv[2]
plt.savefig(outfn)