#ifndef scale_factors_H
#define	scale_factors_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <map>
#include <string>
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"
#include "TMath.h"
#include "TRandom3.h"
#include <TLorentzVector.h>

float ID_SF(float pt, float eta, TH2D*& histo){
    Int_t bin_x = histo->GetXaxis()->FindBin(pt);
    Int_t bin_y = histo->GetYaxis()->FindBin(eta);
    float SF = histo->GetBinContent(bin_x, bin_y);
    return SF;
}

float trigger_SF(float pt, float eta, TH2D*& histo){
    Int_t bin_x = histo->GetXaxis()->FindBin(eta);
    Int_t bin_y = histo->GetYaxis()->FindBin(pt);
    float SF = histo->GetBinContent(bin_x, bin_y);
    return SF;
}


#endif	/* scale_factors_H */
