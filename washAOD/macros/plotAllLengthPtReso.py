#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)

data = {
	0.01: '../test/skimrun/histo_0p1cm.root',
	1   : '../test/skimrun/histo_1cm.root',
	10  : '../test/skimrun/histo_10cm.root',
	50  : '../test/skimrun/histo_50cm.root',
	100 : '../test/skimrun/histo_100cm.root',
	300 : '../test/skimrun/histo_300cm.root'
}

infs = {l: ROOT.TFile(data[l]) for l in data.keys()}
muName = 'RecoMuon'
import sys
if len(sys.argv)>1: muName = sys.argv[1]
try:
	intrees = {l: infs[l].allMuPtResolution.Get(muName) for l in data.keys()}
except:
	print '"{0}" not exists as a branch in TTree.'.format(muName)
	print 'Available branches are:'
	print [x.GetName() for x in infs[1].allMuPtResolution.GetListOfBranches()]
	raise

from root_numpy import tree2array
inarrays = {l: tree2array(intrees[l]) for l in data.keys()}

import matplotlib.pyplot as plt
fig = plt.figure()
ax = fig.add_subplot(1,1,1)
for l in sorted(inarrays.keys()):
	ax.hist(inarrays[l]['res'], bins=50, range=(-0.2,1), normed=True,
		label=str(l)+' cm', histtype='step')
props = {
    'title': muName + r' $p_{T}$ resolution',
    'xlabel': 'resolution',
    'ylabel': 'A.U.'
}
ax.set(**props)
ax.legend(loc='best')

import os
if not os.path.isdir('../test/skimResults'):
	os.mkdir('../test/skimResults')
outfn = '../test/skimResults/allLengthPtReso_{0}.pdf'.format(muName)
plt.savefig(outfn)