#!/usr/bin/env python

# ploting gen muons' vertex (vz) of 
# different dark photon decay length samples

import ROOT
ROOT.gROOT.SetBatch(True)

data = {
    0.1: '../test/gentrigrecoeffirun/histo_0p1cm.root',
    1   : '../test/gentrigrecoeffirun/histo_1cm.root',
    10  : '../test/gentrigrecoeffirun/histo_10cm.root',
    50  : '../test/gentrigrecoeffirun/histo_50cm.root',
    100 : '../test/gentrigrecoeffirun/histo_100cm.root',
    300 : '../test/gentrigrecoeffirun/histo_300cm.root'
}

infs = {l: ROOT.TFile(data[l]) for l in data.keys()}

from root_numpy import tree2array
genMuTrees = {l : infs[l].genmuana.Get('genMu') for l in data.keys()}
inarrays = {l : tree2array(genMuTrees[l]) for l in data.keys()}

import matplotlib.pyplot as plt
fig = plt.figure()
ax = fig.add_subplot(1,1,1)
for l in sorted(inarrays.keys()):
    ax.hist(inarrays[l]['vz'], bins=50, range=(-100,100), normed=True,
        label=str(l)+' cm', histtype='step')

props = {
    'title': ' gen mu vertex vz',
    'xlabel': 'vz [cm]',
    'ylabel': 'A.U.'
}
ax.set(**props)
ax.legend(loc='best')

import os
if not os.path.isdir('../test/gentrigrecoeffiResults'):
    os.mkdir('../test/gentrigrecoeffiResults')
outfn = '../test/gentrigrecoeffiResults/genMuVz.pdf'
plt.savefig(outfn)
