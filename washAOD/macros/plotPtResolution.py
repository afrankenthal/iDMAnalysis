#! /usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch(True)

infn = '../ptreso.root'
inf = ROOT.TFile(infn)
intree = inf.allMuPtResolution.Get('RecoMuon')

from root_numpy import tree2array
inarray = tree2array(intree)

import matplotlib.pyplot as plt
# plt.rc('text', usetex=True)
# font_options = {'family':'sans-serif'}
# plt.rc('font', **font_options)
plt.rc('mathtext', **{'fontset': 'stixsans', 'default': 'regular'})
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.hist(inarray['pt'], bins=50, range=(0,200), normed=True,
	alpha=0.75, histtype='step', label=r'recoMu $p_{T}$')
ax.hist(inarray['ptGen'], bins=50, range=(0,200), normed=True,
	alpha=0.75, histtype='step', label=r'genMu $p_{T}$')
ax.legend(loc='best')

props = {
	'title': r'[$p_{T}$]recoMu : genMu',
	'xlabel': r'$p_{T}$ [GeV]',
	'ylabel': 'A.U.'
}
ax.set(**props)

plt.savefig('ptreso.pdf')