#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)

infn = '../ptreso.root'
import sys
if len(sys.argv)>1: infn = sys.argv[1]
inf = ROOT.TFile(infn)
muName = 'RecoMuon'
if len(sys.argv)>2: muName = sys.argv[2]
try:
	intree = inf.allMuPtResolution.Get(muName)
except:
	print '"{0}" not exists as a branch in TTree.'.format(muName)
	print 'Available branches are:'
	print [x.GetName() for x in inf.allMuPtResolution.GetListOfBranches()]
	raise

from root_numpy import tree2array
inarray = tree2array(intree)

import matplotlib.pyplot as plt
plt.rc('figure', figsize=(25, 10))
fig = plt.figure()
ax0 = fig.add_subplot(1,2,1)
ax0.hist(inarray['res'], bins=50, range=(-0.2,1), normed=True, label=muName)
props0 = {
    'title': r'$p_{T}$ resolution',
    'xlabel': 'resolution',
    'ylabel': 'A.U.'
}
ax0.set(**props0)
ax0.legend(loc='best')

ax1 = fig.add_subplot(1,2,2)
h = ax1.hist2d(inarray['pt'], inarray['res'], bins=50,
        range=[[0,200], [-0.2,1]], normed=True)

props1 = {
    'title': r'$p_{T}$ resolution v.s. $p_{T}$',
    'xlabel': r'$p_{T}$ [GeV]',
    'ylabel': 'resolution'
}
ax1.set(**props1)


plt.colorbar(h[3])

import os
if not os.path.isdir('../test/skimResults'):
	os.mkdir('../test/skimResults')
outfn = '../test/skimResults/resoVsPt_{0}.pdf'.format(muName)
plt.savefig(outfn)