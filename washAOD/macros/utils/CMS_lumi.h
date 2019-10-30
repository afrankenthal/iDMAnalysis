#ifndef CMS_LUMI_HH
#define CMS_LUMI_HH

#include <TASImage.h>
#include <TBox.h>
#include <TLatex.h>
#include <TLine.h>
#include <TPad.h>

//
// Global variables
//

extern "C" {
    static const TString cmsText = "CMS";
    static const Double_t cmsTextFont = 61;

    static const bool writeExtraText = 1;
    static const TString extraText = "Preliminary";
    static const Double_t extraTextFont = 52;

    static const Double_t lumiTextSize = 0.6;
    static const Double_t lumiTextOffset = 0.2;
    static const Double_t cmsTextSize = 0.75;
    static const Double_t cmsTextOffset = 0.1;

    static const Double_t relPosX = 0.045;
    static const Double_t relPosY = 0.035;
    static const Double_t relExtraDY = 1.2;

    static const Double_t extraOverCmsTextSize = 0.76;

    static TString lumi_13TeV = "59.74 fb^{-1} ";
}
//extern const TString lumi_8TeV;
//extern const TString lumi_7TeV;
//extern const TString lumi_sqrtS;

//extern bool drawLogo      = false;

void CMS_lumi( TPad* pad, int iPeriod=3, int iPosX=10 );

#endif // CMS_LUMI_HH
