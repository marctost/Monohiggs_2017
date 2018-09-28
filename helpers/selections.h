#ifndef selections_H
#define	selections_H

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
#include <vector>
#include "vector"

using namespace std;


int isMuon(Int_t num, vector<float>* muPt, vector<float>* muEta, vector<int>* muIDbit, vector<float>* muDz, vector<float>* muD0, vector<float>* muPFNeuIso, vector<float>* muPFPhoIso, vector<float>* muPFPUIso, vector<float>* muPFChIso, TString final_state){
    int number = -1;
    int extra = 0;
    for (int counter=0; counter<num; counter++){
        float thisiszero=0;
        float muPhoPU = muPFNeuIso->at(counter) + muPFPhoIso->at(counter) - 0.5*muPFPUIso->at(counter);
        float mu_iso_tight = (muPFChIso->at(counter) + TMath::Max(thisiszero,muPhoPU))/(muPt->at(counter));
        float iso_val = 0.15;
        if (final_state=="mutau_WCR") iso_val=0.25;
        // Selections are here
        if ((muPt->at(counter)>10) && (fabs(muEta->at(counter))<2.4) && (muIDbit->at(counter)>>0&1==1) && (muDz->at(counter)<0.2) && (muD0->at(counter)<0.045) && mu_iso_tight<iso_val){
            number = counter;
            extra++;
        }
    }
    if (extra>1){
        // Skip any events where there are multiple accepted muons
        number = -1;
    }
    return number;
}



int isElectron(Int_t num, vector<float>* elePt, vector<float>* eleEta, vector<short unsigned int>* eleIDbit, vector<float>* elePFNeuIso, vector<float>* elePFPhoIso, vector<float>*elePFPUIso, vector<float>* elePFChIso){
    int number = -1;
    int extra = 0;
    for (int counter=0; counter<num; counter++){
        float thisiszero=0;
        float elePhoPU = elePFNeuIso->at(counter) + elePFPhoIso->at(counter) - 0.5*elePFPUIso->at(counter);
        float ele_iso_tight = (elePFChIso->at(counter) + TMath::Max(thisiszero,elePhoPU))/(elePt->at(counter));
        
        if ( elePt->at(counter)>40 && fabs(eleEta->at(counter))<2.1 && eleIDbit->at(counter)>>3&1==1 && ele_iso_tight<0.1){
            number = counter;
        }
    }
    if (extra>1){
        number = -1;
    }
    return number;
}



int isTau(Int_t num, vector<float>* tauPt, vector<float>* tauEta, vector<int>* tauDecayMode, vector<float>* tauDz, vector<bool>* tauByMVA6TightElectronRejection, vector<bool>* tauByLooseMuonRejection3, vector<bool>* tauByLooseIsolationMVArun2v1DBoldDMwLT, vector<bool>* tauByTightIsolationMVArun2v1DBoldDMwLT, TString final_state){
    int number = -1;
    for (int counter=0; counter<num; counter++){
      bool kinematic = false;    
      bool tauId = false;
      bool decayModeCut = false;
      bool tauIsolation = false;   
      if( tauPt->at(counter) > 20 && fabs( tauEta->at(counter))< 2.3 && tauDz->at(counter)<0.2 )kinematic = true;
      if( tauByMVA6TightElectronRejection->at(counter) == 1 && tauByLooseMuonRejection3->at(counter) == 1) tauId = true;  
      if( (tauByLooseIsolationMVArun2v1DBoldDMwLT->at(counter)==1) && final_state=="mutau_WCR") tauIsolation = true;
      if( (tauByTightIsolationMVArun2v1DBoldDMwLT->at(counter)==1) && final_state!="mutau_WCR") tauIsolation = true;
      if (final_state=="mutau"){
          if( tauDecayMode->at(counter)==1 || tauDecayMode->at(counter)==3 ) decayModeCut = true;  
      }
      else if (final_state=="mutau_WCR"){
          if ( tauDecayMode->at(counter) > 0) decayModeCut = true;
      }
      if(tauId==true && kinematic==true && tauIsolation==true && decayModeCut==true){    
	number = counter;
      }  

    }
    return number;
}




bool rejectMuon(Int_t num, vector<float>* muPt, vector<float>* muEta, vector<float>* muD0, vector<float>* muDz, vector<int>* muIDbit, vector<float>* muPFNeuIso, vector<float>* muPFPhoIso, vector<float>* muPFPUIso, vector<float>* muPFChIso){
    bool reject=false;
    for (int counter=0; counter<num; counter++){
        float thisiszero=0;
        float muPhoPU = muPFNeuIso->at(counter) + muPFPhoIso->at(counter) - 0.5*muPFPUIso->at(counter);
        float mu_iso = (muPFChIso->at(counter) + TMath::Max(thisiszero,muPhoPU))/(muPt->at(counter));
        
        if (muPt->at(counter)>10 && fabs(muEta->at(counter))<2.4 && muD0->at(counter)<0.045 && muDz->at(counter)<0.2 && muIDbit->at(counter)>>0&1==1 && mu_iso<0.3 ){
            reject=true;
        }
    }
    return reject;
}



bool rejectElectron(Int_t num, vector<float>* elePt, vector<float>* eleEta, vector<float>* eleD0, vector<float>* eleDz, vector<unsigned short>* eleIDbit, vector<float>* elePFNeuIso, vector<float>* elePFPhoIso, vector<float>*elePFPUIso, vector<float>* elePFChIso){
    bool reject=false;
    for (int counter=0; counter<num; counter++){
        float thisiszero=0;
        float elePhoPU = elePFNeuIso->at(counter) + elePFPhoIso->at(counter) - 0.5*elePFPUIso->at(counter);
        float ele_iso = (elePFChIso->at(counter) + TMath::Max(thisiszero,elePhoPU))/(elePt->at(counter));
        
        
        if (elePt->at(counter)>10 && fabs(eleEta->at(counter))<2.5 && eleD0->at(counter)<0.045 && eleDz->at(counter)<0.2 && eleIDbit->at(counter)>>0&1==1 && ele_iso<0.3 ){
            reject=true;
        }
    }
    return reject;
}

bool BjetVeto(Int_t num, vector<float>* jetBtag){

    bool reject=false;
    for (int counter=0; counter<num; counter++){
        if (jetBtag->at(counter)>0.8838){
            // Going to accept the -1 values right now, since we don't know what to do about them.
            reject = true;
        }
    }

    return reject;
}





#endif	/* selections_H */
