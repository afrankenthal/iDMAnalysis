#! /usr/bin/env python

# Usage: python plotAcceptance.py <input ROOT file>

import os
import sys
import array
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)


if len(sys.argv) != 2 or not sys.argv[1].endswith('.root'):
    sys.exit('Wrong input! -> {0}'.format(str(sys.argv)))

def getLimit(pool, percentage=0.01):
    leftNum = int(len(pool)*percentage)
    rightNum = int(len(pool)*(1.-percentage))
    return (sorted(pool)[leftNum], sorted(pool)[rightNum])

def fractionedBinning(limit, frac=(0.02, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.7)):
    xmin, xmax = limit
    assert(xmin < xmax)
    bins = [xmin+f*(xmax-xmin) for f in frac]
    bins = [xmin] + bins + [xmax]
    return array.array('d', bins)


class Efficiency(object):
    """docstring for Efficiency"""
    def __init__(self,name, denom, numer):
        super(Efficiency, self).__init__()
        self.name = name
        self.denom = denom
        self.denomD = {}
        self.denomH = {}

        self.numer = numer
        self.numerD = {}
        self.numerH = {}
        self.effi = {}

    def getName(self):
        return self.name

    def getEfficiency(self, var, varbin=False, startFromZero=True):
        existVars = ('pt', 'eta', 'phi', 'dxy')
        var = var.lower()
        if var not in existVars:
            errMsg = 'ERROR! "{0}" efficiency is not available!\n{1}'
            sys.exit(errMsg.format(var, str(existVars)))
        if varbin and not startFromZero:
            errMsg = 'Warning: usually variable binning does not \
            make sense if distribution NOT starting from zero.\nExiting\
             for now..'
            sys.exit(errMsg)
        

        if var == 'pt':
            self.denomD[var] = [p.pt for p in self.denom]
            self.numerD[var] = [p.pt for p in self.numer]
            varsim = 'p_{T}'
        elif var == 'eta':
            self.denomD[var] = [p.eta for p in self.denom]
            self.numerD[var] = [p.eta for p in self.numer]
            varsim = '#eta'
        elif var == 'phi':
            self.denomD[var] = [p.phi for p in self.denom]
            self.numerD[var] = [p.phi for p in self.numer]
            varsim = '#phi'
        elif var == 'dxy':
            self.denomD[var] = [p.dxy for p in self.denom]
            self.numerD[var] = [p.dxy for p in self.numer]
            varsim = 'dxy'

        xlimitup = max(getLimit(self.denomD[var])[1], getLimit(self.numerD[var])[1])
        xlimitdown = min(getLimit(self.denomD[var])[0], getLimit(self.numerD[var])[0])
        if startFromZero: xlimitdown=0
        if varbin:
            vbins = fractionedBinning((0, xlimitup))
            self.denomH[var] = ROOT.TH1D(self.name+'_'+var+'_d', self.name+' denominator', len(vbins)-1, vbins)
            self.numerH[var] = ROOT.TH1D(self.name+'_'+var+'_n', self.name+' numerator', len(vbins)-1, vbins)
        else:
            self.denomH[var] = ROOT.TH1D(self.name+'_'+var+'_d', self.name+' denominator', 20, xlimitdown, xlimitup)
            self.numerH[var] = ROOT.TH1D(self.name+'_'+var+'_n', self.name+' numerator', 20, xlimitdown, xlimitup)
        for _ in self.denomD[var]: self.denomH[var].Fill(_)
        for _ in self.numerD[var]: self.numerH[var].Fill(_)
        # http://steve.cooleysekula.net/goingupalleys/2011/08/09/python-and-root-tricks-efficiency-graphs/
        self.effi[var] = ROOT.TGraphAsymmErrors()
        self.effi[var].Divide(self.numerH[var], self.denomH[var], "cl=0.683 b(1,1) mode")
        self.effi[var].SetMaximum(1.1)
        self.effi[var].SetMinimum(0)
        return self.effi[var]





inf = ROOT.TFile(sys.argv[1])
color = [600, 1, 616, 2, 9]


outputFn = 'trigger_eff_'+os.path.basename(sys.argv[1]).replace('root','pdf')
trigCollection = ['DST_DoubleMu3_noVtx_CaloScouting_v4',
'HLT_TrkMu12_DoubleTrkMu5NoFiltersNoVtx_v4',
'HLT_TrkMu16_DoubleTrkMu6NoFiltersNoVtx_v10',
'HLT_TrkMu17_DoubleTrkMu8NoFiltersNoVtx_v11']
muonCollection = ['standAloneMuons',
'refittedStandAloneMuons',
'displacedStandAloneMuons',
'recoMuons']

for j, mu in enumerate(muonCollection):
    cc = ROOT.TCanvas(mu, mu, 400, 300)
    cc.Divide(2,2)

    Effs = []
    Effs.append(Efficiency('Acceptance',
                           denom=inf.Get('trigeffiana/Total/'+mu),
                           numer=inf.Get('trigeffiana/Accepted/'+mu)))
    for trig in trigCollection:
        Effs.append(Efficiency(trig,
                               numer=inf.Get('trigeffiana/'+trig+'/'+mu),
                               denom=inf.Get('trigeffiana/Accepted/'+mu)))

    # TLegend
    leg = ROOT.TLegend(0.4 ,0.3, 0.9, 0.55)
    leg.SetBorderSize(0)
    leg.SetFillStyle(1) # 0 is transparent
    leg.SetFillColor(0)

    for k, e in enumerate(Effs):
        cc.cd(1)
        pteff = e.getEfficiency('pt', varbin=True)
        pteff.SetLineColor(color[k])
        pteff.SetTitle('%s Efficiency vs p_{T};p_{T} [GeV];#epsilon' % (mu))
        leg.AddEntry(pteff, e.getName(), 'l')
        pteff.Draw('ap' if k==0 else 'p')
        cc.cd(2)
        etaeff = e.getEfficiency('eta', startFromZero=False)
        etaeff.SetLineColor(color[k])
        etaeff.SetTitle('%s Efficiency vs #eta;#eta;#epsilon' % (mu))
        etaeff.Draw('ap' if k==0 else 'p')
        cc.cd(3)
        phieff = e.getEfficiency('phi', startFromZero=False)
        phieff.SetLineColor(color[k])
        phieff.SetTitle('%s Efficiency vs #phi;#phi;#epsilon' % (mu))
        phieff.Draw('ap' if k==0 else 'p')
        cc.cd(4)
        dxyeff = e.getEfficiency('dxy', varbin=True)
        dxyeff.SetLineColor(color[k])
        dxyeff.SetTitle('%s Efficiency vs dxy;dxy [cm];#epsilon' % (mu))
        dxyeff.Draw('ap' if k==0 else 'p')
    for ic in range(4):
        cc.cd(ic+1)
        # leg.SetTextFont(62)
        leg.SetTextSize(0.025)
        leg.Draw()


    if j==0: cc.Print(outputFn+'(')
    elif j==len(muonCollection)-1: cc.Print(outputFn+')')
    else: cc.Print(outputFn)

