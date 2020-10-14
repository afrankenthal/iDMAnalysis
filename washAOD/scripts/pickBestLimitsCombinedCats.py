#!/usr/bin/env python

import json
from ROOT import TGraph,TCanvas,gStyle,TGraphAsymmErrors,TLegend,TLatex
from ROOT import kBlack,kBlue,kGreen,kOrange,kRed
from rootpy.interactive import wait
import glob
from array import array
import sys
from plotCommon import *

def drawCMSLogo(myC, energy, lumi):
        myC.cd()

        #tlatex = TLatex()
        #baseSize = 25
        #tlatex.SetNDC()
        #tlatex.SetTextAngle(0)
        #tlatex.SetTextColor(1)
        #tlatex.SetTextFont(61)
        #tlatex.SetTextAlign(11)
        #tlatex.SetTextSize(0.0375)
        #tlatex.DrawLatex(0.215, 0.89, "CMS")
        #tlatex.SetTextFont(52)
        #tlatex.SetTextSize(0.0285)
        #tlatex.DrawLatex(0.215, 0.852, "Preliminary")
        #tlatex.SetTextFont(43)
        #tlatex.SetTextSize(21)
        #lumiString = "%.1f" % lumi
        #Lumi = "" + lumiString + " pb^{-1} ("+str(energy)+" TeV)"
        #if lumi > 1000:
        #        lumiString = "%.1f" % (lumi/1000.0)
        #        Lumi = "" + lumiString + " fb^{-1} ("+str(energy)+" TeV)"
        #tlatex.SetTextAlign(31)
        #tlatex.DrawLatex(0.87, 0.96, Lumi)
        #tlatex.SetTextAlign(11)

        CMS_lumi(myC, "137.2 fb^{-1} (13 TeV)", 10)

def make1DLimitPlot(xtitle, xvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory, alphaD, xvar):
    gStyle.SetOptTitle(0)

    axisTitleSize = 0.041
    axisTitleOffset = 1.2
    axisTitleSizeRatioX   = 0.18
    axisLabelSizeRatioX   = 0.12
    axisTitleOffsetRatioX = 0.94
    axisTitleSizeRatioY   = 0.15
    axisLabelSizeRatioY   = 0.108
    axisTitleOffsetRatioY = 0.32

    leftMargin   = 0.15
    rightMargin  = 0.12
    topMargin    = 0.05
    bottomMargin = 0.14
    bottomMargin2 = 0.22

    #c1 = TCanvas() #'c1', '', 200, 10, 700, 500 )
    c1 = TCanvas( "c1", "c2", 200, 10, 700, 600 )

    c1.SetHighLightColor(2)
    c1.SetFillColor(0)
    c1.SetBorderMode(0)
    c1.SetBorderSize(2)
    c1.SetLeftMargin(leftMargin)
    c1.SetRightMargin(rightMargin)
    c1.SetTopMargin(topMargin)
    c1.SetBottomMargin(bottomMargin)
    c1.SetFrameBorderMode(0)
    c1.SetFrameBorderMode(0)
    c1.SetLogy(1)
    #c1.SetLogx(1)
    c1.SetTickx(1)
    c1.SetTicky(1)
    #c1.SetGridx(True);
    #c1.SetGridy(True);
    if "c#tau" in xtitle:
        c1.SetLogx(1);

    exp2sigma_xsec = TGraphAsymmErrors(len(xvals), array('d', xvals), array('d', exp), \
           array('d', [0]), array('d', [0]), array('d', exp2minus), array('d', exp2plus))
    exp2sigma_xsec.Sort()
    exp2sigma_xsec.Draw('A3')
    exp2sigma_xsec.SetFillStyle(1001);
    exp2sigma_xsec.SetFillColor(kOrange);
    exp2sigma_xsec.SetLineColor(kOrange);
    exp2sigma_xsec.GetYaxis().SetRangeUser(1e-4, 1e3);
    if xvar == 'mass':
        exp2sigma_xsec.GetXaxis().SetLimits(1, 80)
    elif xvar == 'ctau':
        exp2sigma_xsec.GetXaxis().SetLimits(0.1, 100)
    else:
        exp2sigma_xsec.GetXaxis().SetLimits(0.8*min(xvals),1.1*max(xvals));
    #exp2sigma_xsec.GetXaxis().SetTitle('m_{1} [GeV]')
    #exp2sigma_xsec.GetXaxis().SetTitle('c#tau [cm]')
    exp2sigma_xsec.GetXaxis().SetTitle(xtitle)
    exp2sigma_xsec.GetXaxis().SetTitleOffset(axisTitleOffset)
    exp2sigma_xsec.GetYaxis().SetTitle('95% C.L. #sigma [pb]')
    exp2sigma_xsec.GetYaxis().SetTitleOffset(axisTitleOffset+0.1)

    exp1sigma_xsec = TGraphAsymmErrors(len(xvals), array('d', xvals), array('d', exp), \
            array('d', [0]), array('d', [0]), array('d', exp1minus), array('d', exp1plus))
    exp1sigma_xsec.Sort()
    exp1sigma_xsec.SetFillStyle(1001);
    exp1sigma_xsec.SetFillColor(kGreen+1);
    exp1sigma_xsec.SetLineColor(kGreen+1);
    exp1sigma_xsec.Draw('3 SAME')

    exp_xsec = TGraph(len(xvals), array('d', xvals), array('d', exp))
    exp_xsec.Sort()
    exp_xsec.SetLineWidth(2)
    exp_xsec.SetLineStyle(1)
    exp_xsec.SetLineColor(kRed)
    exp_xsec.Draw('C SAME')

    obs_xsec = TGraph(len(xvals), array('d', xvals), array('d', obs))
    obs_xsec.Sort()
    obs_xsec.SetLineWidth(3)
    obs_xsec.SetLineColor(kBlack)
    obs_xsec.SetMarkerColor(kBlack)
    obs_xsec.SetMarkerStyle(20)
    obs_xsec.SetMarkerSize(1)
    obs_xsec.Draw('PC SAME')

    theory_xsec = TGraph(len(xvals), array('d', xvals), array('d', theory))
    theory_xsec.Sort()
    theory_xsec.SetLineWidth(2)
    theory_xsec.SetLineStyle(8)
    theory_xsec.SetLineColor(kBlue)
    theory_xsec.Draw('C SAME')

    leg = TLegend(0.50,0.70,0.8,0.90);
    leg.SetBorderSize(0);
    leg.SetFillStyle(0);
    leg.AddEntry(theory_xsec, "Theory", "l");
    leg.AddEntry(obs_xsec, "Observed Limit", "pl");
    leg.AddEntry(exp_xsec, "Expected Limit", "l");
    leg.AddEntry(exp1sigma_xsec, "#pm 1 std. dev.", "f");
    leg.AddEntry(exp2sigma_xsec, "#pm 2 std. dev.", "f");
    leg.Draw()

    drawCMSLogo(c1, 13, 122450)

    if alphaD != '':
        aDtext = TLatex()
        #baseSize = 25
        aDtext.SetNDC()
        aDtext.SetTextAngle(0)
        aDtext.SetTextColor(1)
        #aDtext.SetTextFont(61)
        #aDtext.SetTextAlign(11)
        aDtext.SetTextSize(0.0375)
        aDtext.DrawLatex(0.7, 0.9, "#alpha_{D} = " + alphaD)

    # TCanvas.Update() draws the frame, after which one can change it
    # c1.Update()
    # c1.Modified()
    # c1.Update()

    c1.RedrawAxis()
    c1.Draw()
    
    wait(True)


def main():

    if len(sys.argv) < 3:
        print "ERROR! Need at least 2 arguments"
        exit(1)

    signal_cfg_filename = sys.argv[1]
    working_dir = sys.argv[2]


    if 'EM' in signal_cfg_filename:
        alphaD = '#alpha_{EM}'
    elif '0p1' in signal_cfg_filename:
        alphaD = '0.1'
    else:
        alphaD = ''

    with open(signal_cfg_filename) as f:
        samples = json.load(f)

    limits_files = glob.glob(working_dir + "/limits*.json")

    for sample, cfg in samples.items():
        group = cfg["group"]

        min_limit = 10**6

        for limits_file in limits_files:
            #if 'cut25' not in limits_file: continue
            if group+'.' in limits_file:

                with open(limits_file) as f:
                    limit = json.load(f)

                if "120.0" not in limit: continue
                if "obs" not in limit["120.0"]: continue
                if "exp0" not in limit["120.0"]: continue
                if "exp-1" not in limit["120.0"]: continue
                if "exp+1" not in limit["120.0"]: continue
                if "exp-2" not in limit["120.0"]: continue
                if "exp+2" not in limit["120.0"]: continue
                min_limit = limit["120.0"]["obs"]


                cfg["obs_mu"] = min_limit
                cfg["obs_xsec"] = cfg["obs_mu"] * cfg["xsec"]
                cfg["exp_xsec"] = limit["120.0"]["exp0"] * cfg["xsec"]
                cfg["exp1minus_xsec"] = cfg["exp_xsec"] - limit["120.0"]["exp-1"] * cfg["xsec"]
                cfg["exp1plus_xsec"] = limit["120.0"]["exp+1"] * cfg["xsec"] - cfg["exp_xsec"]
                cfg["exp2minus_xsec"] = cfg["exp_xsec"] - limit["120.0"]["exp-2"] * cfg["xsec"]
                cfg["exp2plus_xsec"] = limit["120.0"]["exp+2"] * cfg["xsec"] - cfg["exp_xsec"]

                y = 15. * 3.141592 / (4. * (1./137))
                y *= 1.98 * 10**(-14) / (cfg["ctau_cm"] * cfg["delta_fraction"]**5 * cfg["m1"])
                cfg["y"] = y

                print group, "y ", cfg["y"]
                print group, " obs_mu ", cfg["obs_mu"], \
                        "theory ", cfg["xsec"], \
                        " observed ", cfg["obs_xsec"], \
                        " exp ", cfg["exp_xsec"], \
                        " exp +/- 1 sigma ", cfg["exp1plus_xsec"], ", ", cfg["exp1minus_xsec"], \
                        " exp +/- 2 sigma ", cfg["exp2plus_xsec"], ", ", cfg["exp2minus_xsec"]

    print "Plotting m1 = 5 GeV, delta = 0.1"
    # Make 1D constant-m1 limit plot
    xvals = []
    obs = []
    exp = []
    exp1minus = []
    exp1plus = []
    exp2minus = []
    exp2plus = []
    theory = []
    for sample, cfg in samples.items():
        if cfg["delta_fraction"] != 0.1: continue
        if cfg["m1"] != 5: continue

        # Check if combine produced a limit for this sample
        if "obs_xsec" not in cfg: continue

        xvals.append(float(cfg["ctau_cm"]))
        obs.append(float(cfg["obs_xsec"]))
        exp.append(float(cfg["exp_xsec"]))
        exp1minus.append(float(cfg["exp1minus_xsec"]))
        exp1plus.append(float(cfg["exp1plus_xsec"]))
        exp2minus.append(float(cfg["exp2minus_xsec"]))
        exp2plus.append(float(cfg["exp2plus_xsec"]))
        theory.append(float(cfg["xsec"]))

    if len(obs) > 0:
        make1DLimitPlot("c#tau [cm]", xvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory, alphaD, 'ctau')

    print "Plotting m1 = 50 GeV, delta = 0.1"
    # Make 1D constant-m1 limit plot
    xvals = []
    obs = []
    exp = []
    exp1minus = []
    exp1plus = []
    exp2minus = []
    exp2plus = []
    theory = []
    for sample, cfg in samples.items():
        if cfg["delta_fraction"] != 0.1: continue
        if cfg["m1"] != 50: continue

        # Check if combine produced a limit for this sample
        if "obs_xsec" not in cfg: continue

        xvals.append(float(cfg["ctau_cm"]))
        obs.append(float(cfg["obs_xsec"]))
        exp.append(float(cfg["exp_xsec"]))
        exp1minus.append(float(cfg["exp1minus_xsec"]))
        exp1plus.append(float(cfg["exp1plus_xsec"]))
        exp2minus.append(float(cfg["exp2minus_xsec"]))
        exp2plus.append(float(cfg["exp2plus_xsec"]))
        theory.append(float(cfg["xsec"]))

    if len(obs) > 0:
        make1DLimitPlot("c#tau [cm]", xvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory, alphaD, 'ctau')

    print "Plotting m1 = 50 GeV, delta = 0.4"
    # Make 1D constant-m1 limit plot
    xvals = []
    obs = []
    exp = []
    exp1minus = []
    exp1plus = []
    exp2minus = []
    exp2plus = []
    theory = []
    for sample, cfg in samples.items():
        if cfg["delta_fraction"] != 0.4: continue
        if cfg["m1"] != 50: continue

        # Check if combine produced a limit for this sample
        if "obs_xsec" not in cfg: continue

        xvals.append(float(cfg["ctau_cm"]))
        obs.append(float(cfg["obs_xsec"]))
        exp.append(float(cfg["exp_xsec"]))
        exp1minus.append(float(cfg["exp1minus_xsec"]))
        exp1plus.append(float(cfg["exp1plus_xsec"]))
        exp2minus.append(float(cfg["exp2minus_xsec"]))
        exp2plus.append(float(cfg["exp2plus_xsec"]))
        theory.append(float(cfg["xsec"]))

    if len(obs) > 0:
        make1DLimitPlot("c#tau [cm]", xvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory, alphaD, 'ctau')

    print "Plotting m1 = 5 GeV, delta = 0.4"
    # Make 1D constant-m1 limit plot
    xvals = []
    obs = []
    exp = []
    exp1minus = []
    exp1plus = []
    exp2minus = []
    exp2plus = []
    theory = []
    for sample, cfg in samples.items():
        if cfg["delta_fraction"] != 0.4: continue
        if cfg["m1"] != 5: continue

        # Check if combine produced a limit for this sample
        if "obs_xsec" not in cfg: continue
        print "limit exists for sample ", sample

        xvals.append(float(cfg["ctau_cm"]))
        obs.append(float(cfg["obs_xsec"]))
        exp.append(float(cfg["exp_xsec"]))
        exp1minus.append(float(cfg["exp1minus_xsec"]))
        exp1plus.append(float(cfg["exp1plus_xsec"]))
        exp2minus.append(float(cfg["exp2minus_xsec"]))
        exp2plus.append(float(cfg["exp2plus_xsec"]))
        theory.append(float(cfg["xsec"]))

    if len(obs) > 0:
        make1DLimitPlot("c#tau [cm]", xvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory, alphaD, 'ctau')

    print "Plotting ctau = 10 mm, delta = 0.1"
    ## Make 1D constant-ctau limit plot
    xvals = []
    obs = []
    exp = []
    exp1minus = []
    exp1plus = []
    exp2minus = []
    exp2plus = []
    theory = []
    for sample, cfg in samples.items():
        if cfg["ctau_cm"] != 1: continue
        if cfg["delta_fraction"] != 0.1: continue

        # Check if combine produced a limit for this sample
        if "obs_xsec" not in cfg: continue

        xvals.append(float(cfg["m1"]))
        obs.append(float(cfg["obs_xsec"]))
        exp.append(float(cfg["exp_xsec"]))
        exp1minus.append(float(cfg["exp1minus_xsec"]))
        exp1plus.append(float(cfg["exp1plus_xsec"]))
        exp2minus.append(float(cfg["exp2minus_xsec"]))
        exp2plus.append(float(cfg["exp2plus_xsec"]))
        theory.append(float(cfg["xsec"]))

    if len(obs) > 0:
        make1DLimitPlot("m_{1} [GeV]", xvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory, alphaD, 'mass')

    print "Plotting ctau = 100 mm, delta = 0.4"
    ## Make 1D constant-ctau limit plot
    xvals = []
    obs = []
    exp = []
    exp1minus = []
    exp1plus = []
    exp2minus = []
    exp2plus = []
    theory = []
    for sample, cfg in samples.items():
        if cfg["ctau_cm"] != 10: continue
        if cfg["delta_fraction"] != 0.4: continue

        # Check if combine produced a limit for this sample
        if "obs_xsec" not in cfg: continue

        xvals.append(float(cfg["m1"]))
        obs.append(float(cfg["obs_xsec"]))
        exp.append(float(cfg["exp_xsec"]))
        exp1minus.append(float(cfg["exp1minus_xsec"]))
        exp1plus.append(float(cfg["exp1plus_xsec"]))
        exp2minus.append(float(cfg["exp2minus_xsec"]))
        exp2plus.append(float(cfg["exp2plus_xsec"]))
        theory.append(float(cfg["xsec"]))

    if len(obs) > 0:
        make1DLimitPlot("m_{1} [GeV]", xvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory, alphaD, 'mass')


if __name__ == "__main__":
    main()

