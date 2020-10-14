#!/usr/bin/env python

import json
import math
import numpy as np
from ROOT import TGraph,TCanvas,gStyle,TGraphAsymmErrors,TLegend,TLatex, TChain, gPad
from ROOT import kBlack,kBlue,kGreen,kOrange,kRed
from ROOT import TFile, TH1F, TEfficiency
#from rootpy.interactive import wait
import glob
from array import array
import sys
import os
#from plotCommon import *


def make_canvas(name,xtitle,mass,rat1,rat2,rat4,rat5,rat6,zoom):
  if zoom:
    can = TCanvas(name+"_zoom")
    #rat1.GetYaxis().SetLimits(0.8,1.05)
  else:
    can = TCanvas(name)
    #rat1.GetYaxis().SetLimits(0.,1.05)
  gStyle.SetOptStat(0)
  if mass == "5":
    m1 = "5"
    m2 = "5.5"
  if mass == "6":
    m1 = "5"
    m2 = "7"
  if mass == "50":
    m1 = "50"
    m2 = "55"
  if mass == "60":
    m1 = "50"
    m2 = "70"
  rat1.SetTitle("DSA ID Efficiency: iDM(m1,m2) = (%s,%s) GeV"%(m1,m2))
  rat1.GetXaxis().SetTitle(xtitle)
  rat1.GetYaxis().SetTitle("Efficiency")
  rat1.SetMaximum(1.05)
  rat1.SetLineColor(kBlack)
  rat1.SetMarkerColor(kBlack)
  rat1.SetMarkerStyle(8)
  rat1.Draw()
  rat2.SetLineColor(kRed)
  rat2.SetMarkerColor(kRed)
  rat2.SetMarkerStyle(8)
  rat2.Draw("Same")
  rat4.SetLineColor(kBlue)
  rat4.SetMarkerColor(kBlue)
  rat4.SetMarkerStyle(8)
  rat4.Draw("Same")
  rat5.SetLineColor(kGreen)
  rat5.SetMarkerColor(kGreen)
  rat5.SetMarkerStyle(8)
  rat5.Draw("Same")
#  rat6.SetLineColor(kOrange)
#  rat6.SetMarkerColor(kOrange)
#  rat6.SetMarkerStyle(8)
#  rat6.Draw("Same")
  leg = TLegend(.125,.15,.325,.25)
  leg.AddEntry(rat1,"p_{T} resolution < 1","l")
  leg.AddEntry(rat2,"+ Planes > 1","l")
  leg.AddEntry(rat4,"+ #chi^{2} <2.5","l")
  leg.AddEntry(rat5,"+ Hits >12 (18 if CSC=0)","l")
  leg.SetBorderSize(0)
  leg.SetFillStyle(0)
#  leg.AddEntry(rat6,"+ |#eta| <2.4","l")
  leg.Draw("Same")
  gPad.SetTickx()
  gPad.SetTicky()
  can.Modified()
  can.Update()
  if zoom:
    rat1.SetMinimum(0.8)
  else:
    rat1.SetMinimum(0.)
  return (can,leg)

def gen_match(gen_index,gen_return):
  if len(gen_index) ==0:
  #  print("return:",gen_return)
    return gen_return
  if len(gen_index[0]) ==0:
   # print("return:",gen_return)
    return gen_return
  min_dr = 9999
  used_gen = -1
  used_reco = -1
  for gen_i,gens in enumerate(gen_index):
    for reco_i, (recos,(dpt,dmu)) in enumerate(gens):
      if recos < min_dr and dpt< 0.3 and dmu > 0.01:
        min_dr = recos
        used_gen = gen_i
        used_reco = reco_i
        #print("Used: ",min_dr,used_gen,used_reco)
  if min_dr > 0.04:
    #print("return:",gen_return)
    return gen_return
  gen_return.append((used_reco,min_dr,np.sqrt(dmu)))
  new_index1 = np.delete(gen_index,used_gen,axis=0)
  new_index = np.delete(new_index1,used_reco, axis=1)
  return gen_match(new_index,gen_return)

def main():

#  if len(sys.argv) < 2:
#    print("ERROR! Need at least 1 arguments")
#    exit(1)
#  print("running")
#  signal_cfg_filename = sys.argv[1]

  #pt_bins = [0.,1.,2.,3.,4.,5.,6.,7.,8.,9., 10., 11.,12.,13.,14.,15.,16.,17.,18.,19., 20., 25., 30., 35.,40., 45., 50., 60., 70., 80., 90., 100.]
  pt_bins = [5.,6.,7.,8.,9.,10.,20.,30.,40.,50.]
  #pt_bins = [0.,1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,20.,25.,30.,40.,50.,100.]
  pt_denom =TH1F('pt_denom','pt_denom', len(pt_bins)-1, array('d',pt_bins)) 
  pt_num1 = TH1F('pt_num1','pt_num1',   len(pt_bins)-1, array('d',pt_bins)) 
  pt_num2 = TH1F('pt_num2','pt_num2',   len(pt_bins)-1, array('d',pt_bins)) 
  pt_num3 = TH1F('pt_num3','pt_num3',   len(pt_bins)-1, array('d',pt_bins)) 
  pt_num4 = TH1F('pt_num4','pt_num4',   len(pt_bins)-1, array('d',pt_bins)) 
  pt_num5 = TH1F('pt_num5','pt_num5',   len(pt_bins)-1, array('d',pt_bins)) 
  pt_num6 = TH1F('pt_num6','pt_num6',   len(pt_bins)-1, array('d',pt_bins)) 

  eta_bins = [-2.4, -2.1, -1.6, -1.2, -0.9, -0.3, -0.2, 0.2, 0.3, 0.9, 1.2, 1.6, 2.1, 2.4]
  eta_denom = TH1F("eta_denom","eta_denom",len(eta_bins)-1,array('d',eta_bins))
  eta_num1 = TH1F("eta_num1","eta_num1",len(eta_bins)-1,array('d',eta_bins))
  eta_num2 = TH1F("eta_num2","eta_num2",len(eta_bins)-1,array('d',eta_bins))
  eta_num3 = TH1F("eta_num3","eta_num3",len(eta_bins)-1,array('d',eta_bins))
  eta_num4 = TH1F("eta_num4","eta_num4",len(eta_bins)-1,array('d',eta_bins))
  eta_num5 = TH1F("eta_num5","eta_num5",len(eta_bins)-1,array('d',eta_bins))
  eta_num6 = TH1F("eta_num6","eta_num6",len(eta_bins)-1,array('d',eta_bins))

  phi_denom = TH1F("phi_denom","phi_denom",10,-3.2,3.2)
  phi_num1 = TH1F("phi_num1","phi_num1",10,-3.2,3.2)
  phi_num2 = TH1F("phi_num2","phi_num2",10,-3.2,3.2)
  phi_num3 = TH1F("phi_num3","phi_num3",10,-3.2,3.2)
  phi_num4 = TH1F("phi_num4","phi_num4",10,-3.2,3.2)
  phi_num5 = TH1F("phi_num5","phi_num5",10,-3.2,3.2)
  phi_num6 = TH1F("phi_num6","phi_num6",10,-3.2,3.2)
  
  plane_denom = TH1F("plane_denom","plane_denom",5,0,5)
  plane_num1 = TH1F("plane_num1","plane_num1",5,0,5)
  plane_num2 = TH1F("plane_num2","plane_num2",5,0,5)
  plane_num3 = TH1F("plane_num3","plane_num3",5,0,5)
  plane_num4 = TH1F("plane_num4","plane_num4",5,0,5)
  plane_num5 = TH1F("plane_num5","plane_num5",5,0,5)
  plane_num6 = TH1F("plane_num6","plane_num6",5,0,5)
  
  hits_denom = TH1F("hits_denom","hits_denom",25,10,35)
  hits_num1 = TH1F("hits_num1","hits_num1",25,10,35)
  hits_num2 = TH1F("hits_num2","hits_num2",25,10,35)
  hits_num3 = TH1F("hits_num3","hits_num3",25,10,35)
  hits_num4 = TH1F("hits_num4","hits_num4",25,10,35)
  hits_num5 = TH1F("hits_num5","hits_num5",25,10,35)
  hits_num6 = TH1F("hits_num6","hits_num6",25,10,35)
  
  chi_denom = TH1F("chi_denom","chi_denom",25,0,4)
  chi_num1 = TH1F("chi_num1","chi_num1",25,0,4)
  chi_num2 = TH1F("chi_num2","chi_num2",25,0,4)
  chi_num3 = TH1F("chi_num3","chi_num3",25,0,4)
  chi_num4 = TH1F("chi_num4","chi_num4",25,0,4)
  chi_num5 = TH1F("chi_num5","chi_num5",25,0,4)
  chi_num6 = TH1F("chi_num6","chi_num6",25,0,4)
  
  res_denom = TH1F("res_denom","res_denom",20,0,1)
  res_num1 = TH1F("res_num1","res_num1",20,0,1)
  res_num2 = TH1F("res_num2","res_num2",20,0,1)
  res_num3 = TH1F("res_num3","res_num3",20,0,1)
  res_num4 = TH1F("res_num4","res_num4",20,0,1)
  res_num5 = TH1F("res_num5","res_num5",20,0,1)
  res_num6 = TH1F("res_num6","res_num6",20,0,1)

  vertex_denom = TH1F("vertex_denom","vertex_denom",40,10,50)
  vertex_num1 = TH1F("vertex_num1","vertex_num1",40,10,50)
  vertex_num2 = TH1F("vertex_num2","vertex_num2",40,10,50)
  vertex_num3 = TH1F("vertex_num3","vertex_num3",40,10,50)
  vertex_num4 = TH1F("vertex_num4","vertex_num4",40,10,50)
  vertex_num5 = TH1F("vertex_num5","vertex_num5",40,10,50)
  vertex_num6 = TH1F("vertex_num6","vertex_num6",40,10,50)

  dr_denom = TH1F("dr_denom","dr_denom",25,0,5)
  dr_num1 = TH1F("dr_num1","dr_num1",25,0,5)
  dr_num2 = TH1F("dr_num2","dr_num2",25,0,5)
  dr_num3 = TH1F("dr_num3","dr_num3",25,0,5)
  dr_num4 = TH1F("dr_num4","dr_num4",25,0,5)
  dr_num5 = TH1F("dr_num5","dr_num5",25,0,5)
  dr_num6 = TH1F("dr_num6","dr_num6",25,0,5)



  if len(sys.argv) < 2:
    mass = "50"
    #mass = "5"
    #mass = "6"
    #mass = "60"
  else:
    mass = sys.argv[1]
#    print("ERROR! Need at least 1 arguments")
#    exit(1)
#  print("running")
#  signal_cfg_filename = sys.argv[1]
  allfiles = []
### 52p5
  if mass == "50":
    outname = "dsa_ID_eff_50"
    for life in [1]:#,10,100,1000]:
      for job in range(1,13):
        allfiles.append("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis_fourteenthrun/Mchi-52p5_dMchi-5p0_ctau-{0}/job{1}_iDMAnalysis_Mchi-52p5_dMchi-5p0_ctau-{0}.root".format(life,job))
## 5p25
  if mass == "5":
    outname = "dsa_ID_eff_5"
    for job in range(1,245):
      allfiles.append("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis_fourteenthrun/Mchi-5p25_dMchi-0p5_ctau-{0}/job{1}_iDMAnalysis_Mchi-5p25_dMchi-0p5_ctau-{0}.root".format(1,job))
    for life in [10,100,1000]:
      for job in range(1,25):
        allfiles.append("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis_fourteenthrun/Mchi-5p25_dMchi-0p5_ctau-{0}/job{1}_iDMAnalysis_Mchi-5p25_dMchi-0p5_ctau-{0}.root".format(life,job))

## 6p0
  if mass == "6":
    outname = "dsa_ID_eff_6"
    for life in [1,10,100,1000]:
      for job in range(1,25):
        allfiles.append("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis_fourteenthrun/Mchi-6p0_dMchi-2p0_ctau-{0}/job{1}_iDMAnalysis_Mchi-6p0_dMchi-2p0_ctau-{0}.root".format(life,job))
### 60p0
  if mass == "60":
    outname = "dsa_ID_eff_60"
    for life in [1,10,100,1000]:
      for job in range(1,26):
        allfiles.append("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis_fourteenthrun/Mchi-60p0_dMchi-20p0_ctau-{0}/job{1}_iDMAnalysis_Mchi-60p0_dMchi-20p0_ctau-{0}.root".format(life,job))

  print(allfiles)
  #if True:
    #g = TFile.Open("root://cmseos.fnal.gov//store/group/lpcmetx/iDM/Ntuples/2018/signal/iDMAnalysis_fourteenthrun/Mchi-52p5_dMchi-5p0_ctau-10/job1_iDMAnalysis_Mchi-52p5_dMchi-5p0_ctau-10.root")
  for afile in allfiles:
    g = TFile.Open(afile)
    g1 = g.ntuples_gbm
    reco = g1.Get("recoT")
    gen = g1.Get("genT")
    for i,(event_r,event_g) in enumerate(zip(reco,gen)):
      #if not (event_r.reco_PF_HEM_flag and (event_r.MET_filters_fail_bits == 0)): 
      #  continue 
      gen_index =[]
      for k,eta_g in enumerate(event_g.gen_eta):
        if abs(event_g.gen_ID[k]) != 13:
          continue
        gen_dr = []
        phi_g = event_g.gen_phi[k]
        dr_index = -1
        dmu_min=9999
        for k1,eta_g1 in enumerate(event_g.gen_eta):
          if abs(event_g.gen_ID[k1]) != 13:
            continue
          if k1 == k:
            continue
          dmu_eta = eta_g - event_g.gen_eta[k1]
          dmu_phi = abs(phi_g - event_g.gen_phi[k1])
          if dmu_phi > math.pi:
            dmu_phi = dmu_phi - 2*math.pi 
          dmu = dmu_eta*dmu_eta + dmu_phi*dmu_phi
          if dmu< dmu_min:
            dmu_min = dmu
        
        for j,eta_r in enumerate(event_r.reco_dsa_eta):
          phi_r = event_r.reco_dsa_phi[j]
          d_eta = eta_r-eta_g
          d_phi = abs(phi_r-phi_g)
          #if(event_g.gen_pt[k] == 0): 
          #  continue
          #dPt = abs(event_r.reco_dsa_pt[j] -event_g.gen_pt[k])/(event_r.reco_dsa_pt[j] + event_g.gen_pt[k])
          dPt = abs(event_r.reco_dsa_pt[j] -event_g.gen_pt[k])/(0.001 + event_g.gen_pt[k])
          if d_phi > math.pi:
            d_phi = d_phi - 2*math.pi 
          dR2 = d_eta*d_eta + d_phi*d_phi 
          gen_dr.append((dR2,(dPt,dmu_min)))
        gen_index.append(gen_dr)
      #print(gen_index)
      matched = gen_match(gen_index,[]) 
        #######DSA ID CUTFLOW
        
      for first,(j,value,dmu) in enumerate(matched): 
        pass_hit    = (event_r.reco_dsa_trk_n_hits[j] > 12 and event_r.reco_dsa_trk_n_CSC_hits[j] >0) or (event_r.reco_dsa_trk_n_DT_hits[j] > 18 and event_r.reco_dsa_trk_n_CSC_hits[j] == 0)
        pass_chi    = event_r.reco_dsa_trk_chi2[j] < 2.5
        pass_pt     = event_r.reco_dsa_pt[j] > 5
        pass_planes = event_r.reco_dsa_trk_n_planes[j] > 1
        pass_res    = event_r.reco_dsa_pt_err[j]/event_r.reco_dsa_pt[j] < 1
        pass_eta    = abs(event_r.reco_dsa_eta[j]) < 2.4

        if(pass_pt and pass_eta):
          pt_denom.Fill(event_r.reco_dsa_pt[j])
          eta_denom.Fill(event_r.reco_dsa_eta[j])
          phi_denom.Fill(event_r.reco_dsa_phi[j])
          plane_denom.Fill(event_r.reco_dsa_trk_n_planes[j])
          hits_denom.Fill(event_r.reco_dsa_trk_n_hits[j])
          res_denom.Fill(event_r.reco_dsa_pt_err[j]/event_r.reco_dsa_pt[j])
          chi_denom.Fill(event_r.reco_dsa_trk_chi2[j])
          if(first==0):
            dr_denom.Fill(dmu)
            vertex_denom.Fill(event_r.npv)


          if(pass_res):
            pt_num1.Fill(event_r.reco_dsa_pt[j])
            eta_num1.Fill(event_r.reco_dsa_eta[j])
            phi_num1.Fill(event_r.reco_dsa_phi[j])
            plane_num1.Fill(event_r.reco_dsa_trk_n_planes[j])
            hits_num1.Fill(event_r.reco_dsa_trk_n_hits[j])
            res_num1.Fill(event_r.reco_dsa_pt_err[j]/event_r.reco_dsa_pt[j])
            chi_num1.Fill(event_r.reco_dsa_trk_chi2[j])
            if(first==0):
              dr_num1.Fill(dmu)
              vertex_num1.Fill(event_r.npv)
            if(pass_planes):
              pt_num2.Fill(event_r.reco_dsa_pt[j])
              eta_num2.Fill(event_r.reco_dsa_eta[j])
              phi_num2.Fill(event_r.reco_dsa_phi[j])
              plane_num2.Fill(event_r.reco_dsa_trk_n_planes[j])
              hits_num2.Fill(event_r.reco_dsa_trk_n_hits[j])
              res_num2.Fill(event_r.reco_dsa_pt_err[j]/event_r.reco_dsa_pt[j])
              chi_num2.Fill(event_r.reco_dsa_trk_chi2[j])
              if(first==0):
                dr_num2.Fill(dmu)
                vertex_num2.Fill(event_r.npv)
              if(pass_pt):
                pt_num3.Fill(event_r.reco_dsa_pt[j])
                eta_num3.Fill(event_r.reco_dsa_eta[j])
                phi_num3.Fill(event_r.reco_dsa_phi[j])
                plane_num3.Fill(event_r.reco_dsa_trk_n_planes[j])
                hits_num3.Fill(event_r.reco_dsa_trk_n_hits[j])
                res_num3.Fill(event_r.reco_dsa_pt_err[j]/event_r.reco_dsa_pt[j])
                chi_num3.Fill(event_r.reco_dsa_trk_chi2[j])
                if(first==0):
                  dr_num3.Fill(dmu)
                  vertex_num3.Fill(event_r.npv)
                if(pass_hit):
                  pt_num4.Fill(event_r.reco_dsa_pt[j])
                  eta_num4.Fill(event_r.reco_dsa_eta[j])
                  phi_num4.Fill(event_r.reco_dsa_phi[j])
                  plane_num4.Fill(event_r.reco_dsa_trk_n_planes[j])
                  hits_num4.Fill(event_r.reco_dsa_trk_n_hits[j])
                  res_num4.Fill(event_r.reco_dsa_pt_err[j]/event_r.reco_dsa_pt[j])
                  chi_num4.Fill(event_r.reco_dsa_trk_chi2[j])
                  if(first==0):
                    dr_num4.Fill(dmu)
                    vertex_num4.Fill(event_r.npv)
                  if(pass_chi):
                    pt_num5.Fill(event_r.reco_dsa_pt[j])
                    eta_num5.Fill(event_r.reco_dsa_eta[j])
                    phi_num5.Fill(event_r.reco_dsa_phi[j])
                    plane_num5.Fill(event_r.reco_dsa_trk_n_planes[j])
                    hits_num5.Fill(event_r.reco_dsa_trk_n_hits[j])
                    res_num5.Fill(event_r.reco_dsa_pt_err[j]/event_r.reco_dsa_pt[j])
                    chi_num5.Fill(event_r.reco_dsa_trk_chi2[j])
                    if(first==0):
                      dr_num5.Fill(dmu)
                      vertex_num5.Fill(event_r.npv)
                    if(pass_eta):
                      pt_num6.Fill(event_r.reco_dsa_pt[j])
                      eta_num6.Fill(event_r.reco_dsa_eta[j])
                      phi_num6.Fill(event_r.reco_dsa_phi[j])
                      plane_num6.Fill(event_r.reco_dsa_trk_n_planes[j])
                      hits_num6.Fill(event_r.reco_dsa_trk_n_hits[j])
                      res_num6.Fill(event_r.reco_dsa_pt_err[j]/event_r.reco_dsa_pt[j])
                      chi_num6.Fill(event_r.reco_dsa_trk_chi2[j])
                      if(first==0):
                        dr_num6.Fill(dmu)
                        vertex_num6.Fill(event_r.npv)
            

 #pt_denom.Draw()
  newfile = TFile("plots_dsaeff/%s.root"%(outname),"RECREATE")
  pt_denom.Write()
  pt_num1.Write()
  pt_num2.Write()
  pt_num3.Write()
  pt_num4.Write()
  pt_num5.Write()
  pt_num6.Write()
  pt_rat1 = pt_num1.Clone("ptrat1")
  pt_rat1.Sumw2()
  pt_rat1.Divide(pt_denom)
  pt_rat1.Write()
  pt_rat2 = pt_num2.Clone("ptrat2")
  pt_rat2.Sumw2()
  pt_rat2.Divide(pt_denom)
  pt_rat2.Write()
  pt_rat3 = pt_num3.Clone("ptrat3")
  pt_rat3.Sumw2()
  pt_rat3.Divide(pt_denom)
  pt_rat3.Write()
  pt_rat4 = pt_num4.Clone("ptrat4")
  pt_rat4.Sumw2()
  pt_rat4.Divide(pt_denom)
  pt_rat4.Write()
  pt_rat5 = pt_num5.Clone("ptrat5")
  pt_rat5.Sumw2()
  pt_rat5.Divide(pt_denom)
  pt_rat5.Write()
  #pt_rat6 = TEfficiency(pt_num6,pt_denom)
  #pt_rat6.SetDirectory(gDriectory)
  #pt_rat6.Write()
  pt_rat6 = pt_num6.Clone("ptrat6")
  pt_rat6.Sumw2()
  pt_rat6.Divide(pt_denom)
  pt_rat6.Write()
  
  eta_denom.Write()
  eta_num1.Write()
  eta_num2.Write()
  eta_num3.Write()
  eta_num4.Write()
  eta_num5.Write()
  eta_num6.Write()
  eta_rat1 = eta_num1.Clone("etarat1")
  eta_rat1.Sumw2()
  eta_rat1.Divide(eta_denom)
  eta_rat1.Write()
  eta_rat2 = eta_num2.Clone("etarat2")
  eta_rat2.Sumw2()
  eta_rat2.Divide(eta_denom)
  eta_rat2.Write()
  eta_rat3 = eta_num3.Clone("etarat3")
  eta_rat3.Sumw2()
  eta_rat3.Divide(eta_denom)
  eta_rat3.Write()
  eta_rat4 = eta_num4.Clone("etarat4")
  eta_rat4.Sumw2()
  eta_rat4.Divide(eta_denom)
  eta_rat4.Write()
  eta_rat5 = eta_num5.Clone("etarat5")
  eta_rat5.Sumw2()
  eta_rat5.Divide(eta_denom)
  eta_rat5.Write()
  eta_rat6 = eta_num6.Clone("etarat6")
  eta_rat6.Sumw2()
  eta_rat6.Divide(eta_denom)
  eta_rat6.Write()

  phi_denom.Write()
  phi_num1.Write()
  phi_num2.Write()
  phi_num3.Write()
  phi_num4.Write()
  phi_num5.Write()
  phi_num6.Write()
  phi_rat1 = phi_num1.Clone("phirat1")
  phi_rat1.Sumw2()
  phi_rat1.Divide(phi_denom)
  phi_rat1.Write()
  phi_rat2 = phi_num2.Clone("phirat2")
  phi_rat2.Sumw2()
  phi_rat2.Divide(phi_denom)
  phi_rat2.Write()
  phi_rat3 = phi_num3.Clone("phirat3")
  phi_rat3.Sumw2()
  phi_rat3.Divide(phi_denom)
  phi_rat3.Write()
  phi_rat4 = phi_num4.Clone("phirat4")
  phi_rat4.Sumw2()
  phi_rat4.Divide(phi_denom)
  phi_rat4.Write()
  phi_rat5 = phi_num5.Clone("phirat5")
  phi_rat5.Sumw2()
  phi_rat5.Divide(phi_denom)
  phi_rat5.Write()
  phi_rat6 = phi_num6.Clone("phirat6")
  phi_rat6.Sumw2()
  phi_rat6.Divide(phi_denom)
  phi_rat6.Write()
 
  plane_denom.Write()
  plane_num1.Write()
  plane_num2.Write()
  plane_num3.Write()
  plane_num4.Write()
  plane_num5.Write()
  plane_num6.Write()
  plane_rat1 = plane_num1.Clone("planerat1")
  plane_rat1.Sumw2()
  plane_rat1.Divide(plane_denom)
  plane_rat1.Write()
  plane_rat2 = plane_num2.Clone("planerat2")
  plane_rat2.Sumw2()
  plane_rat2.Divide(plane_denom)
  plane_rat2.Write()
  plane_rat3 = plane_num3.Clone("planerat3")
  plane_rat3.Sumw2()
  plane_rat3.Divide(plane_denom)
  plane_rat3.Write()
  plane_rat4 = plane_num4.Clone("planerat4")
  plane_rat4.Sumw2()
  plane_rat4.Divide(plane_denom)
  plane_rat4.Write()
  plane_rat5 = plane_num5.Clone("planerat5")
  plane_rat5.Sumw2()
  plane_rat5.Divide(plane_denom)
  plane_rat5.Write()
  plane_rat6 = plane_num6.Clone("planerat6")
  plane_rat6.Sumw2()
  plane_rat6.Divide(plane_denom)
  plane_rat6.Write()
 
  hits_denom.Write()
  hits_num1.Write()
  hits_num2.Write()
  hits_num3.Write()
  hits_num4.Write()
  hits_num5.Write()
  hits_num6.Write()
  hits_rat1 = hits_num1.Clone("hitsrat1")
  hits_rat1.Sumw2()
  hits_rat1.Divide(hits_denom)
  hits_rat1.Write()
  hits_rat2 = hits_num2.Clone("hitsrat2")
  hits_rat2.Sumw2()
  hits_rat2.Divide(hits_denom)
  hits_rat2.Write()
  hits_rat3 = hits_num3.Clone("hitsrat3")
  hits_rat3.Sumw2()
  hits_rat3.Divide(hits_denom)
  hits_rat3.Write()
  hits_rat4 = hits_num4.Clone("hitsrat4")
  hits_rat4.Sumw2()
  hits_rat4.Divide(hits_denom)
  hits_rat4.Write()
  hits_rat5 = hits_num5.Clone("hitsrat5")
  hits_rat5.Sumw2()
  hits_rat5.Divide(hits_denom)
  hits_rat5.Write()
  hits_rat6 = hits_num6.Clone("hitsrat6")
  hits_rat6.Sumw2()
  hits_rat6.Divide(hits_denom)
  hits_rat6.Write()
 
  res_denom.Write()
  res_num1.Write()
  res_num2.Write()
  res_num3.Write()
  res_num4.Write()
  res_num5.Write()
  res_num6.Write()
  res_rat1 = res_num1.Clone("resrat1")
  res_rat1.Sumw2()
  res_rat1.Divide(res_denom)
  res_rat1.Write()
  res_rat2 = res_num2.Clone("resrat2")
  res_rat2.Sumw2()
  res_rat2.Divide(res_denom)
  res_rat2.Write()
  res_rat3 = res_num3.Clone("resrat3")
  res_rat3.Sumw2()
  res_rat3.Divide(res_denom)
  res_rat3.Write()
  res_rat4 = res_num4.Clone("resrat4")
  res_rat4.Sumw2()
  res_rat4.Divide(res_denom)
  res_rat4.Write()
  res_rat5 = res_num5.Clone("resrat5")
  res_rat5.Sumw2()
  res_rat5.Divide(res_denom)
  res_rat5.Write()
  res_rat6 = res_num6.Clone("resrat6")
  res_rat6.Sumw2()
  res_rat6.Divide(res_denom)
  res_rat6.Write()
 
  dr_denom.Write()
  dr_num1.Write()
  dr_num2.Write()
  dr_num3.Write()
  dr_num4.Write()
  dr_num5.Write()
  dr_num6.Write()
  dr_rat1 = dr_num1.Clone("drrat1")
  dr_rat1.Sumw2()
  dr_rat1.Divide(dr_denom)
  dr_rat1.Write()
  dr_rat2 = dr_num2.Clone("drrat2")
  dr_rat2.Sumw2()
  dr_rat2.Divide(dr_denom)
  dr_rat2.Write()
  dr_rat3 = dr_num3.Clone("drrat3")
  dr_rat3.Sumw2()
  dr_rat3.Divide(dr_denom)
  dr_rat3.Write()
  dr_rat4 = dr_num4.Clone("drrat4")
  dr_rat4.Sumw2()
  dr_rat4.Divide(dr_denom)
  dr_rat4.Write()
  dr_rat5 = dr_num5.Clone("drrat5")
  dr_rat5.Sumw2()
  dr_rat5.Divide(dr_denom)
  dr_rat5.Write()
  dr_rat6 = dr_num6.Clone("drrat6")
  dr_rat6.Sumw2()
  dr_rat6.Divide(dr_denom)
  dr_rat6.Write()
 
  vertex_denom.Write()
  vertex_num1.Write()
  vertex_num2.Write()
  vertex_num3.Write()
  vertex_num4.Write()
  vertex_num5.Write()
  vertex_num6.Write()
  vertex_rat1 = vertex_num1.Clone("vertexrat1")
  vertex_rat1.Sumw2()
  vertex_rat1.Divide(vertex_denom)
  vertex_rat1.Write()
  vertex_rat2 = vertex_num2.Clone("vertexrat2")
  vertex_rat2.Sumw2()
  vertex_rat2.Divide(vertex_denom)
  vertex_rat2.Write()
  vertex_rat3 = vertex_num3.Clone("vertexrat3")
  vertex_rat3.Sumw2()
  vertex_rat3.Divide(vertex_denom)
  vertex_rat3.Write()
  vertex_rat4 = vertex_num4.Clone("vertexrat4")
  vertex_rat4.Sumw2()
  vertex_rat4.Divide(vertex_denom)
  vertex_rat4.Write()
  vertex_rat5 = vertex_num5.Clone("vertexrat5")
  vertex_rat5.Sumw2()
  vertex_rat5.Divide(vertex_denom)
  vertex_rat5.Write()
  vertex_rat6 = vertex_num6.Clone("vertexrat6")
  vertex_rat6.Sumw2()
  vertex_rat6.Divide(vertex_denom)
  vertex_rat6.Write()
 
  chi_denom.Write()
  chi_num1.Write()
  chi_num2.Write()
  chi_num3.Write()
  chi_num4.Write()
  chi_num5.Write()
  chi_num6.Write()
  chi_rat1 = chi_num1.Clone("chirat1")
  chi_rat1.Sumw2()
  chi_rat1.Divide(chi_denom)
  chi_rat1.Write()
  chi_rat2 = chi_num2.Clone("chirat2")
  chi_rat2.Sumw2()
  chi_rat2.Divide(chi_denom)
  chi_rat2.Write()
  chi_rat3 = chi_num3.Clone("chirat3")
  chi_rat3.Sumw2()
  chi_rat3.Divide(chi_denom)
  chi_rat3.Write()
  chi_rat4 = chi_num4.Clone("chirat4")
  chi_rat4.Sumw2()
  chi_rat4.Divide(chi_denom)
  chi_rat4.Write()
  chi_rat5 = chi_num5.Clone("chirat5")
  chi_rat5.Sumw2()
  chi_rat5.Divide(chi_denom)
  chi_rat5.Write()
  chi_rat6 = chi_num6.Clone("chirat6")
  chi_rat6.Sumw2()
  chi_rat6.Divide(chi_denom)
  chi_rat6.Write()



  pt_can,pt_leg = make_canvas("pt_ratall","Muon p_{T} [GeV]",mass,pt_rat1,pt_rat2,pt_rat4,pt_rat5,pt_rat6,False)
  pt_can.cd()
  pt_leg.Draw()
  pt_can.Write()
  pt_can.SaveAs("plots_dsaeff/mass_{0}/pt_ratall.png".format(mass),"png")

  pt_can_zoom,pt_leg_zoom = make_canvas("pt_ratall","Muon p_{T} [GeV]",mass,pt_rat1,pt_rat2,pt_rat4,pt_rat5,pt_rat6,True)
  pt_can_zoom.cd()
  pt_leg_zoom.Draw()
  pt_can_zoom.Write()
  pt_can_zoom.SaveAs("plots_dsaeff/mass_{0}/pt_ratall_zoom.png".format(mass),"png")

  eta_can,eta_leg = make_canvas("eta_ratall","#eta",mass,eta_rat1,eta_rat2,eta_rat4,eta_rat5,eta_rat6,False)
  eta_can.cd()
  eta_leg.Draw()
  eta_can.Write()
  eta_can.SaveAs("plots_dsaeff/mass_{0}/eta_ratall.png".format(mass),"png")

  eta_can_zoom,eta_leg_zoom = make_canvas("eta_ratall","#eta",mass,eta_rat1,eta_rat2,eta_rat4,eta_rat5,eta_rat6,True)
  eta_can_zoom.cd()
  eta_leg_zoom.Draw()
  eta_can_zoom.Write()
  eta_can_zoom.SaveAs("plots_dsaeff/mass_{0}/eta_ratall_zoom.png".format(mass),"png")

  phi_can,phi_leg = make_canvas("phi_ratall","#phi",mass,phi_rat1,phi_rat2,phi_rat4,phi_rat5,phi_rat6,False)
  phi_can.cd()
  phi_leg.Draw()
  phi_can.Write()
  phi_can.SaveAs("plots_dsaeff/mass_{0}/phi_ratall.png".format(mass),"png")

  phi_can_zoom,phi_leg_zoom = make_canvas("phi_ratall","#phi",mass,phi_rat1,phi_rat2,phi_rat4,phi_rat5,phi_rat6,True)
  phi_can_zoom.cd()
  phi_leg_zoom.Draw()
  phi_can_zoom.Write()
  phi_can_zoom.SaveAs("plots_dsaeff/mass_{0}/phi_ratall_zoom.png".format(mass),"png")

  hits_can,hits_leg = make_canvas("hits_ratall","n Hits",mass,hits_rat1,hits_rat2,hits_rat4,hits_rat5,hits_rat6,False)
  hits_can.cd()
  hits_leg.Draw()
  hits_can.Write()
  hits_can.SaveAs("plots_dsaeff/mass_{0}/hits_ratall.png".format(mass),"png")

  hits_can_zoom,hits_leg_zoom = make_canvas("hits_ratall","n Hits",mass,hits_rat1,hits_rat2,hits_rat4,hits_rat5,hits_rat6,True)
  hits_can_zoom.cd()
  hits_leg_zoom.Draw()
  hits_can_zoom.Write()
  hits_can_zoom.SaveAs("plots_dsaeff/mass_{0}/hits_ratall_zoom.png".format(mass),"png")

  chi_can,chi_leg = make_canvas("chi2_ratall","#Chi^{2}",mass,chi_rat1,chi_rat2,chi_rat4,chi_rat5,chi_rat6,False)
  chi_can.cd()
  chi_leg.Draw()
  chi_can.Write()
  chi_can.SaveAs("plots_dsaeff/mass_{0}/chi2_ratall.png".format(mass),"png")

  chi_can_zoom,chi_leg_zoom = make_canvas("chi2_ratall","#Chi^{2}",mass,chi_rat1,chi_rat2,chi_rat4,chi_rat5,chi_rat6,True)
  chi_can_zoom.cd()
  chi_leg_zoom.Draw()
  chi_can_zoom.Write()
  chi_can_zoom.SaveAs("plots_dsaeff/mass_{0}/chi2_ratall_zoom.png".format(mass),"png")

  plane_can,plane_leg = make_canvas("plane_ratall","n Planes",mass,plane_rat1,plane_rat2,plane_rat4,plane_rat5,plane_rat6,False)
  plane_can.cd()
  plane_leg.Draw()
  plane_can.Write()
  plane_can.SaveAs("plots_dsaeff/mass_{0}/planes_ratall.png".format(mass),"png")

  plane_can_zoom,plane_leg_zoom = make_canvas("plane_ratall","n Planes",mass,plane_rat1,plane_rat2,plane_rat4,plane_rat5,plane_rat6,True)
  plane_can_zoom.cd()
  plane_leg_zoom.Draw()
  plane_can_zoom.Write()
  plane_can_zoom.SaveAs("plots_dsaeff/mass_{0}/planes_ratall_zoom.png".format(mass),"png")

  res_can,res_leg = make_canvas("res_ratall","Resolution",mass,res_rat1,res_rat2,res_rat4,res_rat5,res_rat6,False)
  res_can.cd()
  res_leg.Draw()
  res_can.Write()
  res_can.SaveAs("plots_dsaeff/mass_{0}/res_ratall.png".format(mass),"png")

  res_can_zoom,res_leg_zoom = make_canvas("res_ratall","Resolution",mass,res_rat1,res_rat2,res_rat4,res_rat5,res_rat6,True)
  res_can_zoom.cd()
  res_leg_zoom.Draw()
  res_can_zoom.Write()
  res_can_zoom.SaveAs("plots_dsaeff/mass_{0}/res_ratall_zoom.png".format(mass),"png")
  
  dr_can,dr_leg = make_canvas("dr_ratall","dr",mass,dr_rat1,dr_rat2,dr_rat4,dr_rat5,dr_rat6,False)
  dr_can.cd()
  dr_leg.Draw()
  dr_can.Write()
  dr_can.SaveAs("plots_dsaeff/mass_{0}/dr_ratall.png".format(mass),"png")

  dr_can_zoom,dr_leg_zoom = make_canvas("dr_ratall","dr",mass,dr_rat1,dr_rat2,dr_rat4,dr_rat5,dr_rat6,True)
  dr_can_zoom.cd()
  dr_leg_zoom.Draw()
  dr_can_zoom.Write()
  dr_can_zoom.SaveAs("plots_dsaeff/mass_{0}/dr_ratall_zoom.png".format(mass),"png")
 
  vertex_can,vertex_leg = make_canvas("vertex_ratall","n vertices",mass,vertex_rat1,vertex_rat2,vertex_rat4,vertex_rat5,vertex_rat6,False)
  vertex_can.cd()
  vertex_leg.Draw()
  vertex_can.Write()
  vertex_can.SaveAs("plots_dsaeff/mass_{0}/vertex_ratall.png".format(mass),"png")

  vertex_can_zoom,vertex_leg_zoom = make_canvas("vertex_ratall","n vertices",mass,vertex_rat1,vertex_rat2,vertex_rat4,vertex_rat5,vertex_rat6,True)
  vertex_can_zoom.cd()
  vertex_leg_zoom.Draw()
  vertex_can_zoom.Write()
  vertex_can_zoom.SaveAs("plots_dsaeff/mass_{0}/vertex_ratall_zoom.png".format(mass),"png")
  newfile.Close()
  

if __name__ == "__main__":
  main()
