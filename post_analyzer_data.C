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

  	int nInspected;
  	nInspected = 0;
  	double nInspected_genWeighted;
  	nInspected_genWeighted = 0.0;
	Long64_t nbytes = 0, nb = 0;

   	for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++) {
      		Long64_t i = LoadTree(jentry);
      		if (i < 0) break;
      		nb = fChain->GetEntry(jentry);   nbytes += nb;

      		if (jentry%reportEvery == 0){
         		std::cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck-1)<<std::endl;
         		fflush(stdout);
      		}


        //FIXME: need a way to declare all the histograms. It might just have to be a huge block, but something slicker (in a different file) would be better


		fillEvent(events);

		if (doesnt pass met filter) continue;
		fillEvent(events);

		if (doesnt pass trigger) continue;
		fillEvent(events);

		if (no lepton_1) continue;
                applySF(lepton_1);
		fillEvent(events);

		if (no lepton_2) continue;
		applySF(lepton_2);
		fillEvent(events);

		if (found third lepton) continue;
		fillEvent(events);

        if (charge isnt opposite) continue;
		fillEvent(events);

		makeHigherVariables(pt, eta, etc);

		if (dr<0.3) continue;
		fillEvent(events);

		if (found bjet) continue;
		fillEvent(events);

		if (visible pt cut) continue;
		fillEvent(events);

		if (met cut) continue;
		fillEvent(events);

		if (invariant mass cut) continue;
		fillEvent(events);

	}


	writeHistos(filename);

		

	




















