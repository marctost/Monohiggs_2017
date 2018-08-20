#include <string>
#include <sstream>
#include <utility>
#include <stdio.h>
#include <TF1.h>
#include <TDirectoryFile.h>
#include <TRandom3.h>
#include "TLorentzVector.h"
#include "TString.h"
#include "TLegend.h"
#include "TH1F.h"
#include "TKey.h"
#include "THashList.h"
#include "THStack.h"
#include "TPaveLabel.h"
#include "TFile.h"
#include <math.h>
#include <vector>
#include "myHelper.h"
#include "selections.h"
#include "scale_factors.h"



int main(int argc, const char* argv[]){
	Long64_t maxEvents = atof(argv[3]);
	if (maxEvents < -1LL){
		std::cout<<"Please enter a valid value for maxEvents (parameter 3)."<<std::endl;
    		return 1;
  	}
 	int reportEvery = atof(argv[4]);
  	if (reportEvery < 1){
		std::cout<<"Please enter a valid value for reportEvery (parameter 4)."<<std::endl;
    		return 1;
  	}
	post_analyzer_data t(argv[1]);
	t.Loop(maxevents,reportEvery, argv[2]);
	return 0;
}




void post_analyzer_data::Loop(Long64_t maxevents, int reportEvery, const char* save_name){
	if (fChain==0) return;

	Long64_t nentries = fChain->GetEntries();
   	Long64_t nentriesToCheck = nentries;
   	if (maxevents != -1LL && nentries > maxevents)
   	nentriesToCheck = maxevents;
   	Long64_t nTotal = nentriesToCheck;


	Long64_t nbytes = 0, nb = 0;

   	for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++) {
      		Long64_t i = LoadTree(jentry);
      		if (i < 0) break;
      		nb = fChain->GetEntry(jentry);   nbytes += nb;

      		if (jentry%reportEvery == 0){
         		std::cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck-1)<<std::endl;
         		fflush(stdout);
      		}

        // Create variables for easier use later, using a lepton number instead of name. More flexible
        vector<float> pt_1;
        vector<float> pt_2 = tauPt;
        vector<float> eta_1;
        vector<float> eta_2 = tauEta;
        vector<float> phi_1;
        vector<float> phi_2 = tauPhi;
        vector<float> charge_1;
        vector<float> charge_2 = tauCharge;
        vector<float> energy_1;
        vector<float> energy_2 = tauEnergy;
        if (final_state=="mutau"){
            pt_1 = muPt;
            eta_1 = muEta;
            phi_1 = muPhi;
            charge_1 = muCharge;
            energy_1 = muEn;
        }
        if (final_state=="etau"){
            pt_1 = elePt;
            eta_1 = eleEta;
            phi_1 = elePhi;
            charge_1 = eleCharge;
            energy_1 = eleEn;
        }
        
        
        
        fillEvent(events, higher_vars);
        
        // met filter selection
        if (!(metFilters==0)) continue;
        fillEvent(events);
        
        
        // trigger selection
        // function "trig_num" is in the selections header, indicates which trigger is to be used
        if (!(HLTEleMuX>>trig_num(final_state)&1==1)) continue;
        fillEvent(events);
        
        if (final_state=="mutau"){
            lept_num_1 = isMuon(variables);
        }
        else if (final_state=="etau"){
            lept_num_1 = isElectron(variables);
        }
        else if (final_state=="tautau"){
            //lept_num_1 = isFirstTau(variables);
        }
        
        
        // first lepton selection
        if (lept_num_1<0) continue;
        fillEvent(events);
        
        if (final_state=="mutau" or final_state=="etau"){
            lept_num_2 = isTau(variables);
        }
        else if (final_state=="tautau"){
            //lept_num_2 = isSecondTau(variables);
        }
        
        // second lepton selection
        if (lept_num_2<0) continue;
        fillEvent(events);
        
        // extra lepton rejection
        if (final_state=="mutau" && rejectElectron==true) continue;
        else if (final_state=="etau" && rejectMuon==true) continue;
        else if (final_state=="tautau" && (rejectElectron==true || rejectMuon==true)) continue;
        fillEvent(events);
        
        // charge requirement selection
        if (!(charge_1+charge_2==0)) continue;
        fillEvent(events);
        
        
        // this variable is a structure, and has the contents: vis_pt, inv_mass, mt_total and dr. Used like: higher_vars.mt_total
        variables_t higher_vars = makeHigherVariables(pt_1->at(lept_num_1), pt_2->at(lept_num_2), eta_1->at(lept_num_1), eta_2->at(lept_num_2), phi_1->at(lept_num_1), phi_2->at(lept_num_2), charge_1->at(lept_num_1), charge_2->at(lept_num_2), energy_1->at(lept_num_1), energy_2->at(lept_num_2), pfMET, pfMETPhi);
        
        
        // dR selection
        if (higher_vars.dr<0.3) continue;
        fillEvent(events);
        
        // bjet selection
        if (BjetVeto(variables)==true) continue;
        fillEvent(events);
        
        /*
         if (higher_vars.vis_pt  ) continue;
         fillEvent(events);
         
         if (pfMET  ) continue;
         fillEvent(events);
         
         if (higher_vars.inv_mass  ) continue;
         fillEvent(events);
         */
        
    }
    
    
    writeHistos(filename);
    
}
		

	



















