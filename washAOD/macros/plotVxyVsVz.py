#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)
import numpy as np

data = {
    0.1: '../test/simplevtx/histo_0p1cm.root',
    1   : '../test/simplevtx/histo_1cm.root',
    10  : '../test/simplevtx/histo_10cm.root',
    50  : '../test/simplevtx/histo_50cm.root',
    100 : '../test/simplevtx/histo_100cm.root',
    300 : '../test/simplevtx/histo_300cm.root'
}

# infn = '../test/simplevtx/histo_300cm.root'
infs = {l: ROOT.TFile(data[l]) for l in data.keys()}

muName = 'globalMuons'
import sys
if len(sys.argv)>1: muName = sys.argv[1]
try:
	intrees = {l: infs[l].simpvtx.Get(muName) for l in data.keys()}
except:
	print '"{0}" not exists as a branch in TTree.'.format(muName)
	print 'Available branches are:'
	print [x.GetName() for x in infs[0].simpvtx.GetListOfBranches()]
	raise

from root_numpy import tree2array
inarrays = {l: tree2array(intrees[l]) for l in data.keys()}

import matplotlib.pyplot as plt
plt.rc('figure', figsize=(60, 10))
plt.set_cmap('GnBu')
fig, axes = plt.subplots(1, len(data), sharex=True, sharey=True)

# add an overall plot to insert xlabel, ylabel and title
fig.add_subplot(111, frameon=False)
plt.tick_params(labelcolor='none', top='off', bottom='off', left='off', right='off')
plt.grid(False)
plt.xlabel('vz [cm]', fontsize=18)
plt.ylabel('vxy [cm]', fontsize=18)
plt.title(muName, fontsize=24)

for i, l in enumerate(sorted(inarrays.keys())):
    axes[i].hist2d(np.fabs(inarrays[l]['vz']), inarrays[l]['vxy'], bins=50,
                   range=[[0,120], [0,120]], normed=True)
    axes[i].set_title('{0} cm'.format(l), fontsize=18)
plt.subplots_adjust(wspace=0, hspace=0)


import os
if not os.path.isdir('../test/simplevtxResults'):
	os.mkdir('../test/simplevtxResults')
outfn = '../test/simplevtxResults/VxyVsVz_{0}.pdf'.format(muName)
plt.savefig(outfn)