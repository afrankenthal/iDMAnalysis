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

extern const TString cmsText;
extern const Double_t cmsTextFont;

extern const bool writeExtraText;
extern const TString extraText;
extern const Double_t extraTextFont;

extern const Double_t lumiTextSize;
extern const Double_t lumiTextOffset;
extern const Double_t cmsTextSize;
extern const Double_t cmsTextOffset;

extern const Double_t relPosX;
extern const Double_t relPosY;
extern const Double_t relExtraDY;

extern const Double_t extraOverCmsTextSize;

extern TString lumi_13TeV;
//extern const TString lumi_8TeV;
//extern const TString lumi_7TeV;
//extern const TString lumi_sqrtS;

//extern bool drawLogo      = false;

void CMS_lumi( TPad* pad, int iPeriod=3, int iPosX=10 );

#endif // CMS_LUMI_HH
