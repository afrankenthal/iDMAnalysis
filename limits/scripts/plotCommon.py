from ROOT import TGraph,TCanvas,gStyle,TGraphAsymmErrors,TLegend,TLatex
from ROOT import kBlack,kBlue,kGreen,kOrange,kRed

cmsText     = "CMS";
cmsTextFont   = 62  

writeExtraText = True
extraText   = "Preliminary"
extraTextFont = 52 

lumiTextSize     = 0.6
lumiTextOffset   = 0.2

cmsTextSize      = 0.75
cmsTextOffset    = 0.1

relPosX    = 0.15
relPosY    = 0.035
relExtraDY = 1.2

extraOverCmsTextSize  = 0.76

lumi_13TeV = "137.19 fb^{-1}"

def CMS_lumi(pad, lumi_sqrtS="", iPosX=0):

    outOfFrame    = False
    if iPosX/10 == 0: outOfFrame = True

    alignY_=3
    alignX_=2
    if iPosX/10 == 0: alignX_=1
    if iPosX    == 0: alignY_=1
    if iPosX/10 == 1: alignX_=1
    if iPosX/10 == 2: alignX_=2
    if iPosX/10 == 3: alignX_=3
    align_ = 10*alignX_ + alignY_

    H = pad.GetWh()
    W = pad.GetWw()
    l = pad.GetLeftMargin()
    t = pad.GetTopMargin()
    r = pad.GetRightMargin()
    b = pad.GetBottomMargin()
    e = 0.025

    pad.cd()

    lumiText = ""
    if lumi_sqrtS == "":
        lumiText += lumi_13TeV
        lumiText += " (13 TeV)"
    else:
        lumiText += lumi_sqrtS
            
    print lumiText

    latex = TLatex()
    latex.SetNDC()
    latex.SetTextAngle(0)
    latex.SetTextColor(kBlack)    
    
    extraTextSize = extraOverCmsTextSize*cmsTextSize
    
    latex.SetTextFont(42)
    latex.SetTextAlign(31) 
    latex.SetTextSize(lumiTextSize*t)    

    latex.DrawLatex(1-r, 1-t+lumiTextOffset*t, lumiText)

    if outOfFrame:
        latex.SetTextFont(cmsTextFont)
        latex.SetTextAlign(11) 
        latex.SetTextSize(cmsTextSize*t)    
        latex.DrawLatex(l,1-t+lumiTextOffset*t,cmsText)
  
    pad.cd()

    posX_ = 0
    if iPosX % 10 <= 1:
        posX_ =  l + relPosX * (1-l-r)
    elif iPosX % 10 == 2:
        posX_ =  l + 0.5 * (1-l-r)
    elif iPosX % 10 == 3:
        posX_ =  1 - r - relPosX * (1-l-r)

    posY_ = 1 - t - relPosY * (1-t-b)

    if not outOfFrame:
        latex.SetTextFont(cmsTextFont)
        latex.SetTextSize(cmsTextSize*t)
        latex.SetTextAlign(align_)
        latex.DrawLatex(posX_, posY_, cmsText)
        if writeExtraText:
            latex.SetTextFont(extraTextFont)
            latex.SetTextAlign(align_)
            latex.SetTextSize(extraTextSize*t)
            latex.DrawLatex(posX_, posY_- relExtraDY*cmsTextSize*t, extraText)
    elif writeExtraText:
        if iPosX == 0:
            posX_ = l +  relPosX * (1 - l - r)
            posY_ = 1 - t + lumiTextOffset*t

        latex.SetTextFont(extraTextFont)
        latex.SetTextSize(extraTextSize * t)
        latex.SetTextAlign(align_)
        latex.DrawLatex(posX_, posY_, extraText)      

    pad.Update()
