TH2D * TH2Subset(TH2D *h,Int_t binx1, Int_t binx2, Int_t biny1, Int_t biny2)
{
   Double_t x1 = h->GetXaxis()->GetBinLowEdge(binx1);
   Double_t x2 = h->GetXaxis()->GetBinLowEdge(binx2)+h->GetXaxis()->GetBinWidth(binx2);
   Double_t y1 = h->GetYaxis()->GetBinLowEdge(biny1);
   Double_t y2 = h->GetYaxis()->GetBinLowEdge(biny2)+h->GetYaxis()->GetBinWidth(biny2);
   Int_t    n1 = binx1+binx2-1;
   Int_t    n2 = biny1+biny2-1;
   
   Int_t nbinsx = h->GetNbinsX();
   Int_t nbinsy = h->GetNbinsY();
   
   TH2D *hs = new TH2D(h->GetName(),
                       h->GetTitle(),
                       n1, x1, x2,
                       n2, y1, y2);
   
   Double_t content, x, y;

   for (Int_t i=binx1; i<=binx2; i++) {
      for (Int_t j=biny1; j<=biny2; j++) {
         content = h->GetBinContent(i,j);
         x = h->GetXaxis()->GetBinCenter(i);
         y = h->GetYaxis()->GetBinCenter(j);
         hs->Fill(x, y, content);
      }
   }

   return hs;
}
