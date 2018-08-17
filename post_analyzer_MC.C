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


// TODO:
// create var_1 and var_2 for the appropriate variables, in order to make things slicker with the higher order variables.
// Pass the appropriate variables to the selection functions
// Update the selection functions so that they, you know, do something
// Write a function that uses "helper.h" to create all the variables in a single line
// Find a way to include the final state as a string into the call for everything, so the program is final-state flexible
// Figure out how/where to store the declarations of the histograms, and fill them in a nice way. Ask Jithin for help.
// Comment everything so people understand what the hell is going on.



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
	post_analyzer_MC t(argv[1]);
	t.Loop(maxevents,reportEvery, argv[2]);
	return 0;
}




void post_analyzer_MC::Loop(Long64_t maxevents, int reportEvery, const char* save_name){
	if (fChain==0) return;

	Long64_t nentries = fChain->GetEntriesFast();
   	Long64_t nentriesToCheck = nentries;
   	if (maxevents != -1LL && nentries > maxevents)
   	nentriesToCheck = maxevents;
   	Long64_t nTotal = nentriesToCheck;

  	int nInspected;
  	nInspected = 0;
  	double nInspected_genWeighted;
  	nInspected_genWeighted = 0.0;
	Long64_t nbytes = 0, nb = 0;
    int lept_num_1=-1;
    int lept_num_2=-1;

   	for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++) {
      		Long64_t i = LoadTree(jentry);
      		if (i < 0) break;
      		nb = fChain->GetEntry(jentry);   nbytes += nb;

      		if (jentry%reportEvery == 0){
         		std::cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck-1)<<std::endl;
         		fflush(stdout);
      		}


        if ((string(save_name)).rfind("WJets2J",0)==0){
            if (genHT>200){
                continue;
            }
        }
        
        double weight=1.0;
        genWeight > 0.0 ? event_weight *= genWeight/fabs(genWeight) : event_weight = 0.0;


		fillEvent(events, weight);

        // met filter selection
		if (!(metFilters==0)) continue;
		fillEvent(events, weight);

        
        // trigger selection
        if (!(HLTEleMuX>>trig_num&1==1)) continue;
        // FIXME: define the variable "trig_num" based on the final state that is being used
		fillEvent(events, weight);

        // FIXME: define the variable "final_state", to be passed from the command line, ideally
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
		fillEvent(events, weight);

        
        if (final_state=="mutau"){
            weight=weight*ID_SF(muPt->at(lept_num_1), muEta->at(lept_num_1), SF_histo)*trigger_SF(muPt->at(lept_num_1), muEta->at(lept_num_1), SF_histo);
        }
        
        if (final_state=="mutau" or final_state=="etau"){
            lept_num_2 = isTau(variables);
        }
        else if (final_state=="tautau"){
            //lept_num_2 = isSecondTau(variables);
        }
        
        // second lepton selection
		if (lept_num_2<0) continue;
		fillEvent(events, weight);

        // extra lepton rejection
        if (final_state=="mutau" && rejectElectron==true) continue;
        else if (final_state=="etau" && rejectMuon==true) continue;
        else if (final_state=="tautau" && (rejectElectron==true || rejectMuon==true)) continue;
		fillEvent(events, weight);

        // charge requirement selection
        // FIXME: define charge_1 and charge_2
        if (!(charge_1+charge_2==0)) continue;
		fillEvent(events, weight);

		makeHigherVariables(pt, eta, etc);

        
        // dR selection
		if (dr<0.3) continue;
		fillEvent(events, weight);

        // bjet selection
		if (BjetVeto(variables)==true) continue;
		fillEvent(events, weight);

/*
		if (visible pt cut) continue;
		fillEvent(events, weight);

		if (met cut) continue;
		fillEvent(events, weight);

		if (invariant mass cut) continue;
		fillEvent(events, weight);
*/
 
	}


	writeHistos(filename);

}
		

	




















