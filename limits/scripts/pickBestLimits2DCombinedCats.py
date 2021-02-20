#!/usr/bin/env python

import json
import numpy as np
from scipy import interpolate
import matplotlib.pyplot as plt
from ROOT import TCanvas,gStyle,TLegend,TH2D,TLatex,TGraph
from ROOT import TColor,kBlack,kBlue,kGreen,kOrange,kRed,kViridis
from rootpy.interactive import wait
import glob
from array import array
import sys
from plotCommon import *
#import tdrstyle
#tdrstyle.setTDRStyle()

def find_roots(t, y):
    """
    Given the input signal `y` with samples at times `t`,
    find the times where `y` is 0.

    `t` and `y` must be 1-D numpy arrays.

    Linear interpolation is used to estimate the time `t` between
    samples at which sign changes in `y` occur.

    https://stackoverflow.com/a/39458926
    """
    # Find where y crosses 0.
    transition_indices = np.where(np.sign(y[1:]) != np.sign(y[:-1]))[0]

    # Linearly interpolate the time values where the transition occurs.
    t0 = t[transition_indices]
    t1 = t[transition_indices + 1]
    y0 = y[transition_indices]
    y1 = y[transition_indices + 1]
    slope = (y1 - y0) / (t1 - t0)
    transition_times = t0 - y0/slope

    return transition_times

def plotLimitBoundary(c1, xvals, yvals, zvals, style=[1,1], **kwargs):
    debug = kwargs.pop('debug', False)
    mass_splitting = kwargs.pop('mass_splitting', '0.1')

    limit_boundaries_m1 = []
    limit_boundaries_ctau = []

    if mass_splitting == '0.1':
        m1s = [3,5,10,20,30,40,50,60,80]
    else:
        m1s = [1,3,5,10,20,30,40,50,60,80]

    for m1 in m1s:
        limit_yvals = []
        limit_zvals = []

        for xval,yval,zval in zip(xvals, yvals, zvals):
            if xval != m1: continue

            limit_yvals.append(yval)
            limit_zvals.append(zval)

        sorted_limit_yvals = [y for y,_ in sorted(zip(limit_yvals,limit_zvals), reverse=False)]
        sorted_limit_zvals = [z for _,z in sorted(zip(limit_yvals,limit_zvals), reverse=False)]

        if debug:
            print "sorted limit yvals: ", sorted_limit_yvals
            print "sorted limit zvals: ", sorted_limit_zvals

        if len(limit_zvals) < 3:
            print "Warning! Too few points to interpolate! No limits for m1 = ", m1, " --> setting to infinity (1k)"

            #limit_boundaries_m1.append(m1)
            #limit_boundaries_ctau.append(1000)
            continue
        elif len(limit_zvals) < 4:
            print "Warning! Only 3 points to interpolate! Using linear instead of cubic spline, may not be as accurate"
            boundaries = find_roots(np.log10(sorted_limit_yvals), np.array(sorted_limit_zvals) - 1.0)
        else:
            f = interpolate.UnivariateSpline(np.log10(sorted_limit_yvals), np.log10(sorted_limit_zvals), s=0, k=len(limit_zvals)-1)
            boundaries = f.roots()

        print "[interpolate] boundary ctau/y with limit r (mu) @ 1.0 for m1 = ", m1, " is ", np.power(10, boundaries)
        
        if len(boundaries) > 0:
            limit_boundaries_m1.append(m1)
            limit_boundaries_ctau.append(np.power(10, min(boundaries)))
        elif np.mean(sorted_limit_zvals) < 1.0: # in case boundary wasn't found because we are at resonance (mA' = 90 GeV) so all limits are < 1
            limit_boundaries_m1.append(m1)
            limit_boundaries_ctau.append(1e-10) # set arbitrary low limit in y

    if len(limit_boundaries_ctau) < 3:
        return

    c1.cd()
    graph_limit_boundary = TGraph(len(limit_boundaries_ctau), array('d',limit_boundaries_m1), array('d',limit_boundaries_ctau))
    graph_limit_boundary.SetLineStyle(style[0])
    graph_limit_boundary.SetLineColor(style[1])
    graph_limit_boundary.SetLineWidth(3)
    graph_limit_boundary.Draw("LSAME")

def interpolateObsLimit(xvals, yvals, obs, **kwargs):
    debug = kwargs.pop('debug', False)
    smooth = kwargs.pop('smooth', 0.5)
    epsilon = kwargs.pop('epsilon', 0.1)
    numMassPoints = kwargs.pop('numMassPoints', 10j)
    numYPoints = kwargs.pop('numYPoints', 4j)
    mass_splitting = kwargs.pop('mass_splitting', '0.1')

    if debug:
        print "xvals ", xvals
        print "yvals ", yvals

    interpolated = interpolate.Rbf(xvals, np.log10(yvals), obs, smooth=smooth, epsilon=epsilon)
    xmin = np.min(xvals)
    xmax = np.max(xvals)
    ymin = np.min(np.log10(yvals))
    ymax = np.max(np.log10(yvals))
    #xnew, ynew = np.mgrid[xmin:xmax:numMassPoints, ymin:ymax:numYPoints]
    if mass_splitting == '0.4':
        xnew, ynew = np.meshgrid([1, 3, 5, 8, 10, 20, 30, 40, 50, 60, 80], [-7.5, -8.5, -9.5, -10.5, -12.5]) #np.linspace(ymin,ymax,abs(numYPoints)))
    else:
        xnew, ynew = np.meshgrid([3, 5, 8, 20, 30, 40, 50, 60, 80], [-5.0, -5.5, -6.0, -6.5, -7.0, -7.5, -8.0, -9.0]) #np.linspace(ymin,ymax,abs(numYPoints)))
    xnew = np.transpose(xnew)
    ynew = np.transpose(ynew)
    ynew = np.array(ynew, dtype=float)
    fnew = interpolated(xnew, ynew)
    all_data = np.vstack([xnew.ravel(), np.array(10**(ynew.ravel())), fnew.ravel()])

    if debug:
        print "xnew ", xnew
        print "ynew ", ynew
        print "fnew ", fnew

    return all_data


def make2DLimitPlot(xtitle, xvals, yvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory, **kwargs):
    use_ctau = kwargs.pop('use_ctau', False)
    do_interpolation = kwargs.pop('do_interpolation', True)
    debug = kwargs.pop('debug', True)
    mass_splitting = kwargs.pop('mass_splitting', '0.1')

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
    c1.SetLogx(1)
    c1.SetLogz(1);
    c1.SetTickx(1)
    c1.SetTicky(1)

    stops = np.array([0.00, 0.34, 0.61, 0.84, 1.00])
    red= np.array([0.50, 0.50, 1.00, 1.00, 1.00])
    green = np.array([ 0.50, 1.00, 1.00, 0.60, 0.50])
    blue = np.array([1.00, 1.00, 0.50, 0.40, 0.50])
    TColor.CreateGradientColorTable(len(stops), stops, red, green, blue, 255)
    gStyle.SetNumberContours(255)

    if debug:
        print "list of m1s:", xvals
        print "list of ys: ", yvals
        print "list of obs limits: ", obs

    if do_interpolation:
        data = interpolateObsLimit(xvals, yvals, obs, debug=debug, mass_splitting=mass_splitting)
    else:
        data = np.array([xvals, yvals, obs])

    widthx = sorted(data[0])[0]/10
    widthy = sorted(data[1])[0]/10

    lowXs = [xval - widthx/2 for xval in data[0]]
    highXs = [xval + widthx/2 for xval in data[0]]
    lowYs = [yval - widthy/2 for yval in data[1]]
    highYs = [yval + widthy/2 for yval in data[1]]
    binsX = sorted(list(dict.fromkeys(lowXs + highXs)))
    binsY = sorted(list(dict.fromkeys(lowYs + highYs)))
    binsX = [(binsX[i]+binsX[i+1])/2 if i != len(binsX)-1 else binsX[i] for i in range(1,len(binsX)-1, 2)]
    binsY = [(binsY[i]+binsY[i+1])/2 if i != len(binsY)-1 else binsY[i] for i in range(1,len(binsY)-1, 2)]
    binsX.insert(0, sorted(lowXs)[0])
    binsX.insert(len(binsX), sorted(highXs)[-1])
    binsY.insert(0, sorted(lowYs)[0])
    binsY.insert(len(binsY), sorted(highYs)[-1])
    
    if debug:
        print "binsX ", binsX
        print "binsY ", binsY
    
    obs_xsec = TH2D('', '', len(binsX)-1, array('d', binsX), len(binsY)-1, array('d', binsY))

    obs_xsec.GetXaxis().SetTitleSize(axisTitleSize)
    obs_xsec.GetXaxis().SetTitleOffset(axisTitleOffset - 0.3)
    obs_xsec.GetXaxis().SetTitle("m_{1} [GeV]")
    obs_xsec.GetXaxis().SetMoreLogLabels()

    obs_xsec.GetYaxis().SetTitleSize(axisTitleSize)
    obs_xsec.GetYaxis().SetTitleOffset(axisTitleOffset + 0.1)
    if use_ctau:
        obs_xsec.GetYaxis().SetTitle("c#tau [cm]")
    else:
        obs_xsec.GetYaxis().SetTitle("y = #epsilon^{2} #alpha_{D} (m_{1}/m_{A'})^{4}")

    obs_xsec.GetZaxis().SetTitleSize(axisTitleSize - 0.005)
    obs_xsec.GetZaxis().SetTitleOffset(axisTitleOffset + 0.1)
    obs_xsec.GetZaxis().SetTitle("#sigma_{95% CL} Br(A' #rightarrow #mu#mu) [pb]")

    obs_xsec.FillN(len(data[0]), array('d', data[0]), array('d', data[1]), array('d', data[2]))
    obs_xsec.GetZaxis().SetRangeUser(1e-4, 1e2);

    if do_interpolation:
        obs_xsec.Draw("COLZ")
    else:
        obs_xsec.Draw("COLZ TEXT")

    obs_mu = [x/y for x, y in zip(obs, theory)]
    plotLimitBoundary(c1, xvals, yvals, obs_mu, [1, kBlack], debug=debug, mass_splitting=mass_splitting)

    exp_mu = [x/y for x, y in zip(exp, theory)]
    plotLimitBoundary(c1, xvals, yvals, exp_mu, [2, kRed], debug=debug, mass_splitting=mass_splitting)

    CMS_lumi(c1, "137.2 fb^{-1} (13 TeV)", 0)

    c1.RedrawAxis()
    c1.Draw()
    
    wait(True)

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



def main():

    if len(sys.argv) < 3:
        print "ERROR! Need at least 2 arguments: signal config file and working directory"
        exit(1)

    use_ctau = False

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
                #cfg["x_edge"] = input_data[plot_base_name]["x_edge"]
                #cfg["y_edge"] = input_data[plot_base_name]["y_edge"]

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
        print "y: ", cfg["y"]
        print  " obs_mu ", cfg["obs_mu"], \
                "theory ", cfg["xsec"], \
                " observed ", cfg["obs_xsec"], \
                " exp ", cfg["exp_xsec"], \
                " exp +/- 1 sigma ", cfg["exp1plus_xsec"], ", ", cfg["exp1minus_xsec"], \
                " exp +/- 2 sigma ", cfg["exp2plus_xsec"], ", ", cfg["exp2minus_xsec"]
        print ''


    # Make 2D constant-delta limit plot (delta = 0.1)
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
        if use_ctau:
            yvals.append(float(cfg["ctau_cm"]))
        else:
            yvals.append(float(cfg["y"]))
        obs.append(float(cfg["obs_xsec"]))
        exp.append(float(cfg["exp_xsec"]))
        exp1minus.append(float(cfg["exp1minus_xsec"]))
        exp1plus.append(float(cfg["exp1plus_xsec"]))
        exp2minus.append(float(cfg["exp2minus_xsec"]))
        exp2plus.append(float(cfg["exp2plus_xsec"]))
        theory.append(float(cfg["xsec"]))

    print "delta = 0.1"
    make2DLimitPlot("m_1 [GeV]", xvals, yvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory, use_ctau=use_ctau, mass_splitting='0.1')

    # Make 2D constant-delta limit plot (delta = 0.4)
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
        if use_ctau:
            yvals.append(float(cfg["ctau_cm"]))
        else:
            yvals.append(float(cfg["y"]))
        obs.append(float(cfg["obs_xsec"]))
        exp.append(float(cfg["exp_xsec"]))
        exp1minus.append(float(cfg["exp1minus_xsec"]))
        exp1plus.append(float(cfg["exp1plus_xsec"]))
        exp2minus.append(float(cfg["exp2minus_xsec"]))
        exp2plus.append(float(cfg["exp2plus_xsec"]))
        theory.append(float(cfg["xsec"]))

    print "delta = 0.4"
    make2DLimitPlot("m_1 [GeV]", xvals, yvals, obs, exp, exp1plus, exp1minus, exp2plus, exp2minus, theory, use_ctau=use_ctau, mass_splitting='0.4')



if __name__ == "__main__":
    main()
