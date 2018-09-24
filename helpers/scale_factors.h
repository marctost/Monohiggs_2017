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

float mu_ID_SF(float pt, float eta){
    TFile *muIDSFfile = TFile::Open("RunBCDEF_mc_ID.root");
    TH2D *histo = (TH2D*) muIDSFfile->Get("NUM_TightID_DEN_genTracks_pt_abseta");
    Int_t bin_x = histo->GetXaxis()->FindBin(pt);
    Int_t bin_y = histo->GetYaxis()->FindBin(eta);
    float SF = histo->GetBinContent(bin_x, bin_y);
    muIDSFfile->Close();
    return SF;
}

float mu_trigger_SF(float pt, float eta){
    TFile *muTriggerSFfile = TFile::Open("EfficienciesAndSF_RunBtoF_Nov17Nov2017.root");
    TH2D *histo = (TH2D*) muTriggerSFfile->Get("IsoMu27_PtEtaBins/efficienciesMC/abseta_pt_MC");
    Int_t bin_x = histo->GetXaxis()->FindBin(eta);
    Int_t bin_y = histo->GetYaxis()->FindBin(pt);
    float SF = histo->GetBinContent(bin_x, bin_y);
    muTriggerSFfile->Close();
    return SF;
}

float tau_scale(){
    float tau_SF = 0.89;
    return tau_SF;
}

float ele_Zvtx_SF(){
    float ele_SF = 0.991;
    return ele_SF;
}

float ele_ID_SF(float pt, float eta){
    TFile *eleIDSFfile = TFile::Open("ele_ID.root");
    TH2D *histo = (TH2D*) eleIDSFfile->Get("EGamma_SF2D");
    Int_t bin_x = histo->GetXaxis()->FindBin(eta);
    Int_t bin_y = histo->GetYaxis()->FindBin(pt);
    float SF = histo->GetBinContent(bin_x, bin_y);
    eleIDSFfile->Close();
    return SF;
}

float ele_eff_SF(float pt, float eta){
    TFile *eleEffSFfile = TFile::Open("ele_efficiency_Tight94X.root");
    TH2D *histo = (TH2D*) eleEffSFfile->Get("EGamma_SF2D");
    Int_t bin_x = histo->GetXaxis()->FindBin(eta);
    Int_t bin_y = histo->GetYaxis()->FindBin(pt);
    float SF = histo->GetBinContent(bin_x, bin_y);
    eleEffSFfile->Close();
    return SF;
}









#endif	/* scale_factors_H */
