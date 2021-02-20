#include "CMS_lumi.h"

void CMS_lumi(TPad * pad, int iPeriod, int iPosX, TString years) {

    const TString  cmsText     = "CMS";
    const Double_t cmsTextFont = 61;  // default is helvetic-bold

    // extraText is either "Simulation" or "Preliminary"
    const Bool_t   writeExtraText  = 1;
    const TString extraText = "Preliminary";
    const Double_t extraTextFont   = 52;  // default is helvetica-italics

    const Bool_t writeLumiText = 1;
    TString lumiText = "(13 TeV)";
    if (writeLumiText) {
        if (lumi_13TeV.find(years) != lumi_13TeV.end()) // lumi is one of standard year combos
            lumiText.Prepend(lumi_13TeV[years]);
        else // lumi is custom (likely incomplete samples)
            lumiText.Prepend(years);
    }

    // text sizes and text offsets with respect to the top frame
    // in unit of the top margin size
    const Double_t lumiTextSize     = 0.6;
    const Double_t lumiTextOffset   = 0.2;
    const Double_t cmsTextSize      = 0.75;
    const Double_t cmsTextOffset    = 0.1;  // only used in outOfFrame version

    const Double_t relPosX    = 0.045;
    const Double_t relPosY    = 0.035;
    const Double_t relExtraDY = 1.2;

    // ratio of "CMS" and extra text size
    const Double_t extraOverCmsTextSize  = 0.76;

    const Bool_t outOfFrame = (iPosX/10 == 0);

    Int_t alignY_=3;
    Int_t alignX_=2;
    if (iPosX    == 0) {alignY_ = 1;}
    if (iPosX/10 <= 1) {alignX_ = 1;}
    if (iPosX/10 == 2) {alignX_ = 2;}
    if (iPosX/10 == 3) {alignX_ = 3;}
    const Int_t align_ = 10*alignX_ + alignY_;

    const Double_t H = pad->GetWh();
    const Double_t W = pad->GetWw();
    const Double_t l = pad->GetLeftMargin();
    const Double_t t = pad->GetTopMargin();
    const Double_t r = pad->GetRightMargin();
    const Double_t b = pad->GetBottomMargin();
    const Double_t e = 0.025;

    TLatex latex;
    latex.SetNDC();
    latex.SetTextAngle(0);
    latex.SetTextColor(kBlack);    

    const Double_t extraTextSize = extraOverCmsTextSize*cmsTextSize;

    latex.SetTextFont(42);
    latex.SetTextAlign(31); 
    latex.SetTextSize(lumiTextSize*t);    
    latex.DrawLatex(1-r,1-t+lumiTextOffset*t,lumiText);

    if (outOfFrame) 
    {
        latex.SetTextFont(cmsTextFont);
        latex.SetTextAlign(11); 
        latex.SetTextSize(cmsTextSize*t);    
        latex.DrawLatex(l,1-t+lumiTextOffset*t,cmsText);
    }

    Double_t posX_ = 0;
    if (iPosX%10 <= 1)  
    {
        posX_ =  l + relPosX*(1-l-r);
    }
    else if (iPosX%10 == 2) 
    {
        posX_ =  l + 0.5*(1-l-r);
    }
    else if (iPosX%10 == 3) 
    {
        posX_ =  1-r - relPosX*(1-l-r);
    }

    Double_t posY_ = 1-t - relPosY*(1-t-b);

    if (!outOfFrame) 
    {
        latex.SetTextFont(cmsTextFont);
        latex.SetTextSize(cmsTextSize*t);
        latex.SetTextAlign(align_);
        latex.DrawLatex(posX_, posY_, cmsText);

        if (writeExtraText) 
        {
            latex.SetTextFont(extraTextFont);
            latex.SetTextAlign(align_);
            latex.SetTextSize(extraTextSize*t);
            latex.DrawLatex(posX_, posY_- relExtraDY*cmsTextSize*t, extraText);
        }
    }

    else if (outOfFrame && writeExtraText)
    {
        if (iPosX == 0) 
        {
            posX_ = l +  relPosX*(1-l-r)+0.07;
            posY_ = 1-t+lumiTextOffset*t;
        }
        latex.SetTextFont(extraTextFont);
        latex.SetTextSize(extraTextSize*t);
        latex.SetTextAlign(align_);
        latex.DrawLatex(posX_, posY_, extraText);
    }
}
