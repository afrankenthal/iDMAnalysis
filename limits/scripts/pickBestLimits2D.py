#!/usr/bin/env python

import json
import numpy as np
from ROOT import TCanvas,gStyle,TLegend,TH2D,TLatex,TGraph
from ROOT import TColor,kBlack,kBlue,kGreen,kOrange,kRed,kViridis
from rootpy.interactive import wait
import glob
from array import array
import sys
from plotCommon import *
#import tdrstyle
#tdrstyle.setTDRStyle()

def plotLimitBoundary(c1, xvals, yvals, zvals, style=[1,1]):
    limit_boundaries_m1 = []
    limit_boundaries_ctau = []

    for m1 in [1,3,5,10,20,30,40,50,60,80]:
        limit_xvals = []
        limit_yvals = []

        for xval,yval,zval in zip(xvals, yvals, zvals):
            if xval != m1: continue

            limit_xvals.append(yval)
            limit_yvals.append(zval)

        if len(limit_yvals) < 2:
            print "No limits for m1 = ", m1, " --> setting to infinity (1k)"

            limit_boundaries_m1.append(m1)
            limit_boundaries_ctau.append(1000)
            continue

        sorted_limit_xvals = [x for _,x in sorted(zip(limit_xvals,limit_yvals))]
        sorted_limit_yvals = [y for y,_ in sorted(zip(limit_xvals,limit_yvals))]
        #print "sorted limit xvals: ", sorted_limit_xvals
        #print "sorted limit yvals: ", sorted_limit_yvals

        graph = TGraph(len(sorted_limit_xvals), array('d', sorted_limit_xvals), array('d', sorted_limit_yvals))
        print "boundary ctau/y with limit r (mu) @ 1.0 for m1 = ", m1, " is ", graph.Eval(1.0)
        #c2 = TCanvas("c2", "c2", 200, 10, 700, 600)
        #c2.SetLogx()
        #c2.SetLogy()
        #graph.SetMarkerSize(2)
        #graph.SetMarkerStyle(20)
        #graph.Draw("AP")
        #wait(True)
        
        limit_boundaries_m1.append(m1)
        limit_boundaries_ctau.append(graph.Eval(1.0))

    graph_limit_boundary = TGraph(len(limit_boundaries_ctau), array('d',limit_boundaries_m1), array('d',limit_boundaries_ctau))
    graph_limit_boundary.SetLineStyle(style[0])
    graph_limit_boundary.SetLineColor(style[1])
    graph_limit_boundary.SetLineWidth(3)
    graph_limit_boundary.Draw("LSAME")


def make2DLimitPlot(xtitle, xvals, yvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory):
    gStyle.SetOptTitle(0)
    gStyle.SetOptStat(0)

    axisTitleSize = 0.041
    axisTitleOffset = 1.2

    leftMargin   = 0.15
    rightMargin  = 0.18
    topMargin    = 0.06
    bottomMargin = 0.14

    c1 = TCanvas("c1", "c1", 200, 10, 700, 600)
    c1.SetHighLightColor(2)
    c1.SetFillColor(0)
    c1.SetBorderMode(0)
    c1.SetBorderSize(2)
    c1.SetLeftMargin(leftMargin)
    c1.SetRightMargin(rightMargin)
    c1.SetTopMargin(topMargin)
    c1.SetBottomMargin(bottomMargin)
    c1.SetFrameBorderMode(0)
    c1.SetLogy(1)
    c1.SetLogx(0)
    c1.SetLogz(1);
    c1.SetTickx(1)
    c1.SetTicky(1)

    stops = np.array([0.00, 0.34, 0.61, 0.84, 1.00])
    red= np.array([0.50, 0.50, 1.00, 1.00, 1.00])
    green = np.array([ 0.50, 1.00, 1.00, 0.60, 0.50])
    blue = np.array([1.00, 1.00, 0.50, 0.40, 0.50])
    TColor.CreateGradientColorTable(len(stops), stops, red, green, blue, 255)
    gStyle.SetNumberContours(255)

    print "list of m1s:", xvals
    print "list of ys: ", yvals
    print "list of obs limits: ", obs

    widthx = sorted(xvals)[0]/10
    widthy = sorted(yvals)[0]/10

    lowXs = [xval - widthx/2 for xval in xvals]
    highXs = [xval + widthx/2 for xval in xvals]
    lowYs = [yval - widthy/2 for yval in yvals]
    highYs = [yval + widthy/2 for yval in yvals]
    binsX = sorted(list(dict.fromkeys(lowXs + highXs)))
    binsY = sorted(list(dict.fromkeys(lowYs + highYs)))
    binsX = [(binsX[i]+binsX[i+1])/2 if i != len(binsX)-1 else binsX[i] for i in range(1,len(binsX)-1, 2)]
    binsY = [(binsY[i]+binsY[i+1])/2 if i != len(binsY)-1 else binsY[i] for i in range(1,len(binsY)-1, 2)]
    binsX.insert(0, sorted(lowXs)[0])
    binsX.insert(len(binsX), sorted(highXs)[-1])
    binsY.insert(0, sorted(lowYs)[0])
    binsY.insert(len(binsY), sorted(highYs)[-1])
    
    print "binsX ", binsX
    print "binsY ", binsY
    
    obs_xsec = TH2D('', '', len(binsX)-1, array('d', binsX), len(binsY)-1, array('d', binsY))

    obs_xsec.GetXaxis().SetTitleSize(axisTitleSize)
    obs_xsec.GetXaxis().SetTitleOffset(axisTitleOffset - 0.2)
    obs_xsec.GetXaxis().SetTitle("m_{1} [GeV]")

    obs_xsec.GetYaxis().SetTitleSize(axisTitleSize)
    obs_xsec.GetYaxis().SetTitleOffset(axisTitleOffset + 0.1)
    obs_xsec.GetYaxis().SetTitle("y = #epsilon^{2} #alpha_{D} (m_{1}/m_{A'})^{4}")
    #obs_xsec.GetYaxis().SetTitle("c#tau [cm]")

    obs_xsec.GetZaxis().SetTitleSize(axisTitleSize - 0.005)
    obs_xsec.GetZaxis().SetTitleOffset(axisTitleOffset + 0.1)
    obs_xsec.GetZaxis().SetTitle("#sigma_{95% CL} [pb]")

    obs_xsec.FillN(len(xvals), array('d', xvals), array('d', yvals), array('d', obs))

    obs_xsec.GetZaxis().SetRangeUser(1e-4, 1e1);
    obs_xsec.Draw("COLZ")
    obs_xsec.Draw("TEXT SAME")

    obs_mu = [x/y for x, y in zip(obs, theory)]
    plotLimitBoundary(c1, xvals, yvals, obs_mu, [1, kBlack])
    #obs_mu_cont = obs_xsec.Clone()
    #obs_mu_cont.Reset()
    #obs_mu_cont.FillN(len(xvals), array('d', xvals), array('d', yvals), array('d', obs_mu))
    #obs_mu_cont.SetContour(1);
    #obs_mu_cont.SetContourLevel(0, 1);
    #obs_mu_cont.SetLineColor(kBlack);
    #obs_mu_cont.SetLineWidth(3);
    #obs_mu_cont.Draw("CONT3 SAME")

    exp_mu = [x/y for x, y in zip(exp, theory)]
    plotLimitBoundary(c1, xvals, yvals, exp_mu, [2, kRed])
    #exp_mu_cont = obs_xsec.Clone()
    #exp_mu_cont.Reset()
    #exp_mu_cont.FillN(len(xvals), array('d', xvals), array('d', yvals), array('d', exp_mu))
    #exp_mu_cont.SetContour(1);
    #exp_mu_cont.SetContourLevel(0, 1);
    #exp_mu_cont.SetLineColor(kRed);
    #exp_mu_cont.SetLineStyle(2);
    #exp_mu_cont.SetLineWidth(3);
    #exp_mu_cont.Draw("CONT3 SAME")

    CMS_lumi(c1, "106.71 fb^{-1} (13 TeV)", 0)

    #exp2sigma_xsec = TGraphAsymmErrors(len(xvals), array('d', xvals), array('d', exp), \
    #       array('d', [0]), array('d', [0]), array('d', exp2minus), array('d', exp2plus))
    #exp2sigma_xsec.Sort()
    #exp2sigma_xsec.Draw('A3')
    #exp2sigma_xsec.SetFillStyle(1001);
    #exp2sigma_xsec.SetFillColor(kOrange);
    #exp2sigma_xsec.SetLineColor(kOrange);
    #exp2sigma_xsec.GetYaxis().SetRangeUser(0.001,100);
    #exp2sigma_xsec.GetXaxis().SetLimits(0.8*min(xvals),1.1*max(xvals));
    ##exp2sigma_xsec.GetXaxis().SetTitle('m_{1} [GeV]')
    ##exp2sigma_xsec.GetXaxis().SetTitle('c#tau [cm]')
    #exp2sigma_xsec.GetXaxis().SetTitle(xtitle)
    #exp2sigma_xsec.GetYaxis().SetTitle('95% C.L. #sigma(pp #rightarrow #chi_{1} #chi_{1} #mu^{+} #mu_{-}) [pb]')

    #exp1sigma_xsec = TGraphAsymmErrors(len(xvals), array('d', xvals), array('d', exp), \
    #        array('d', [0]), array('d', [0]), array('d', exp1minus), array('d', exp1plus))
    #exp1sigma_xsec.Sort()
    #exp1sigma_xsec.SetFillStyle(1001);
    #exp1sigma_xsec.SetFillColor(kGreen+1);
    #exp1sigma_xsec.SetLineColor(kGreen+1);
    #exp1sigma_xsec.Draw('3 SAME')

    #exp_xsec = TGraph(len(xvals), array('d', xvals), array('d', exp))
    #exp_xsec.Sort()
    #exp_xsec.SetLineWidth(2)
    #exp_xsec.SetLineStyle(1)
    #exp_xsec.SetLineColor(kRed)
    #exp_xsec.Draw('C SAME')

    #obs_xsec = TGraph(len(xvals), array('d', xvals), array('d', obs))
    #obs_xsec.Sort()
    #obs_xsec.SetLineWidth(2)
    #obs_xsec.SetLineColor(kBlack)
    #obs_xsec.SetMarkerColor(kBlack)
    #obs_xsec.SetMarkerStyle(20)
    #obs_xsec.SetMarkerSize(1)
    #obs_xsec.Draw('PC SAME')

    #theory_xsec = TGraph(len(xvals), array('d', xvals), array('d', theory))
    #theory_xsec.Sort()
    #theory_xsec.SetLineWidth(2)
    #theory_xsec.SetLineStyle(8)
    #theory_xsec.SetLineColor(kBlue)
    #theory_xsec.Draw('C SAME')

    #leg = TLegend(0.50,0.65,0.8,0.85);
    #leg.SetBorderSize(0);
    #leg.SetFillStyle(0);
    #leg.AddEntry(theory_xsec, "Theory", "l");
    #leg.AddEntry(obs_xsec, "Observed Limit", "pl");
    #leg.AddEntry(exp_xsec, "Expected Limit", "l");
    #leg.AddEntry(exp1sigma_xsec, "#pm 1 std. dev.", "f");
    #leg.AddEntry(exp2sigma_xsec, "#pm 2 std. dev.", "f");
    #leg.Draw()

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

    with open(signal_cfg_filename) as f:
        samples = json.load(f)

    with open(working_dir + "/data_for_combine.json") as f:
        input_data = json.load(f)

    limits_files = glob.glob(working_dir + "/limits*.json")

    for sample, cfg in samples.items():

        group = cfg["group"]

        min_limit = 10**6

        for limits_file in limits_files:
            #if 'cut27' not in limits_file: continue
            if group+'.' in limits_file:

                with open(limits_file) as f:
                    limit = json.load(f)

                if "120.0" not in limit: continue
                if "obs" not in limit["120.0"]: continue
                if limit["120.0"]["obs"] > min_limit: continue
                if "exp0" not in limit["120.0"]: continue
                if "exp-1" not in limit["120.0"]: continue
                if "exp+1" not in limit["120.0"]: continue
                if "exp-2" not in limit["120.0"]: continue
                if "exp+2" not in limit["120.0"]: continue

                min_limit = limit["120.0"]["obs"]
            
                plot_base_name = limits_file.split('limits_')[1].split('.')[0]
                cfg["x_edge"] = input_data[plot_base_name]["x_edge"]
                cfg["y_edge"] = input_data[plot_base_name]["y_edge"]

                cfg["best_cut"] = [segment for segment in limits_file.split('_') if 'cut' in segment][0]
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

        if "obs_xec" not in cfg: continue
        print "Sample: ", sample
        print "Best cut: ", cfg["best_cut"]
        print "x_edge: ", cfg["x_edge"], ", y_edge: ", cfg["y_edge"]
        print "y: ", cfg["y"]
        print  " obs_mu ", cfg["obs_mu"], \
                "theory ", cfg["xsec"], \
                " observed ", cfg["obs_xsec"], \
                " exp ", cfg["exp_xsec"], \
                " exp +/- 1 sigma ", cfg["exp1plus_xsec"], ", ", cfg["exp1minus_xsec"], \
                " exp +/- 2 sigma ", cfg["exp2plus_xsec"], ", ", cfg["exp2minus_xsec"]
        print ''

    # Make 2D constant-delta limit plot
    xvals = []
    yvals = []
    obs = []
    exp = []
    exp1minus = []
    exp1plus = []
    exp2minus = []
    exp2plus = []
    theory = []
    for sample, cfg in samples.items():
        if cfg["delta_fraction"] != 0.4: continue

        # Check if combine produced limit for this sample
        if "obs_xsec" not in cfg: continue

        xvals.append(float(cfg["m1"]))
        #yvals.append(float(cfg["ctau_cm"]))
        yvals.append(float(cfg["y"]))
        obs.append(float(cfg["obs_xsec"]))
        exp.append(float(cfg["exp_xsec"]))
        exp1minus.append(float(cfg["exp1minus_xsec"]))
        exp1plus.append(float(cfg["exp1plus_xsec"]))
        exp2minus.append(float(cfg["exp2minus_xsec"]))
        exp2plus.append(float(cfg["exp2plus_xsec"]))
        theory.append(float(cfg["xsec"]))

    print "delta = 0.4"
    make2DLimitPlot("m_1 [GeV]", xvals, yvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory)

    # Make 2D constant-delta limit plot
    xvals = []
    yvals = []
    obs = []
    exp = []
    exp1minus = []
    exp1plus = []
    exp2minus = []
    exp2plus = []
    theory = []
    for sample, cfg in samples.items():
        if cfg["delta_fraction"] != 0.1: continue

        # Check if combine produced limit for this sample
        if "obs_xsec" not in cfg: continue

        xvals.append(float(cfg["m1"]))
        #yvals.append(float(cfg["ctau_cm"]))
        yvals.append(float(cfg["y"]))
        obs.append(float(cfg["obs_xsec"]))
        exp.append(float(cfg["exp_xsec"]))
        exp1minus.append(float(cfg["exp1minus_xsec"]))
        exp1plus.append(float(cfg["exp1plus_xsec"]))
        exp2minus.append(float(cfg["exp2minus_xsec"]))
        exp2plus.append(float(cfg["exp2plus_xsec"]))
        theory.append(float(cfg["xsec"]))

    print "delta = 0.1"
    make2DLimitPlot("m_1 [GeV]", xvals, yvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory)



if __name__ == "__main__":
    main()

