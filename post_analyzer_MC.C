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
#include "higher_vars.h"


// TODO:
// Pass the appropriate variables to the selection functions
// Update the selection functions so that they, you know, do something
// Find a way to include the final state as a string into the call for everything, so the program is final-state flexible
// Figure out how/where to store the declarations of the histograms, and fill them in a nice way. Ask Jithin for help.
// Comment everything so people understand what the hell is going on.
// Bring everything in the MC analyzer into the data analyzer, with the necesary changes



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
        

        // housekeeping, making sure that we use the inclusive W+jets sample correctly.
        if ((string(save_name)).rfind("WJets2J",0)==0){
            if (genHT>200){
                continue;
            }
        }
        
        // Making sure that no events have negative weights
        double weight=1.0;
        genWeight > 0.0 ? event_weight *= genWeight/fabs(genWeight) : event_weight = 0.0;


		fillEvent(events, weight);

        // met filter selection
		if (!(metFilters==0)) continue;
		fillEvent(events, weight);

        
        
        
        // trigger selection
        // function "trig_num" is in the selections header, indicates which trigger is to be used
        if (!(HLTEleMuX>>trig_num(final_state)&1==1)) continue;
		fillEvent(events, weight);

        if (final_state=="mutau"){
            lept_num_1 = isMuon(nMu, muPt, muEta, muIDbit, muDz, muD0, muPFNeuIso, muPFPhoIso, muPFPUIso, muPFChIso);
        }
        else if (final_state=="etau"){
            lept_num_1 = isElectron(nEle, elePt, eleEta, eteIDbit, elePFNeuIso, elePFPhoIso, elePFPUIso, elePFChIso);
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
            lept_num_2 = isTau(nTau, tauPt, tauEta, tauDecayMode, tauDz, tauByMVA6TightElectronRejection, tauByLooseMuonRejection3, tauByTightIsolationMVArun2v1DBoldDMwLT);
        }
        else if (final_state=="tautau"){
            //lept_num_2 = isSecondTau(variables);
        }
        
        // second lepton selection
		if (lept_num_2<0) continue;
		fillEvent(events, weight);

        // extra lepton rejection
        bool rejectEle = rejectElectron(nEle, elePt, eleEta, eleD0, eleDz, eleIDbit, elePFNeuIso, elePFPhoIso, elePFPUIso, elePFChIso);
        bool rejectMu = rejectMuon(nMu, muPt, muEta, muD0, muDz, muIDbit, muPFNeuIso, muPFPhoIso, muPFPUIso, muPFChIso);
        
        if (final_state=="mutau" && rejectEle==true) continue;
        else if (final_state=="etau" && rejectMu==true) continue;
        else if (final_state=="tautau" && (rejectElectron==true || rejectMuon==true)) continue;
		fillEvent(events, weight);

        // charge requirement selection
        if (!(charge_1->at(lept_num_1)+charge_2->at(lept_num_2)==0)) continue;
		fillEvent(events, weight);

        
        // this variable is a structure, and has the contents: vis_pt, inv_mass, mt_total and dr. Used like: higher_vars.mt_total
        variables_t higher_vars = makeHigherVariables(pt_1->at(lept_num_1), pt_2->at(lept_num_2), eta_1->at(lept_num_1), eta_2->at(lept_num_2), phi_1->at(lept_num_1), phi_2->at(lept_num_2), charge_1->at(lept_num_1), charge_2->at(lept_num_2), energy_1->at(lept_num_1), energy_2->at(lept_num_2), pfMET, pfMETPhi);

        
        // dR selection
		if (higher_vars.dr<0.3) continue;
		fillEvent(events, weight);

        // bjet selection
		if (BjetVeto(nJet, jetBtag)==true) continue;
		fillEvent(events, weight);

/*
		if (higher_vars.vis_pt  ) continue;
		fillEvent(events, weight);

		if (pfMET  ) continue;
		fillEvent(events, weight);

		if (higher_vars.inv_mass  ) continue;
		fillEvent(events, weight);
*/
 
	}


	writeHistos(filename);

}
		

	




















