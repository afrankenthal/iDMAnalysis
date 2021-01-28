#ifndef TDRSTYLE_HH
#define TDRSTYLE_HH

#include <TPad.h>
#include <TROOT.h>
#include <TStyle.h>

// tdrGrid: Turns the grid lines on (true) or off (false)

//void tdrGrid(bool gridOn) {
//  tdrStyle->SetPadGridX(gridOn);
//  tdrStyle->SetPadGridY(gridOn);
//}

// fixOverlay: Redraws the axis

void fixOverlay(); 

void setTDRStyle(); 

#endif // TDRSTYLE_HH
