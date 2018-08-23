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
		

void post_analyzer_MC::BookHistos(const char* file2)
{
	fileName = new TFile(file2, "RECREATE");
	//tree = new TTree("ADD","ADD");
	//tree->Branch("event_","std::vector<unsigned int>",&event_);
	//tree->Branch("event_info","std::vector<double>",&event_info);
	fileName->cd();
	
	Float_t PtBins[13]={0.0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200.,300., 400.};
	Float_t MetBins[15]={0.0, 20, 40, 60, 80, 100, 120, 140, 160,180., 200, 300., 400., 600.0,800.0};
	

	//Set up the histos to be filled with method fillHistos
	for(int i=0; i<10; i++)
	{
		char ptbins[100];
		sprintf(ptbins, "_%d", i);
		std::string histname(ptbins);
		h_nVtx[i] = new TH1F(("nVtx"+histname).c_str(), "nVtx",40,0,40);h_nVtx[i]->Sumw2();
		h_nEvents[i] = new TH1F(("nEvents"+histname).c_str(), "nEvents",3,0,3);h_nEvents[i]->Sumw2();
                h_genWeight[i] = new TH1F(("genWeight"+histname).c_str(), "genWeight",10, -10.0, 10.0);h_genWeight[i]->Sumw2();
                h_genHT[i] = new TH1F(("genHT"+histname).c_str(), "genHT",12,PtBins);h_genHT[i]->Sumw2();
		//**************  electrons  **************
		h_electron_En[i] = new TH1F(("Electron_En"+histname).c_str(), "Electron_En",12,PtBins);h_electron_En[i]->Sumw2();
		h_electron_Pt[i] = new TH1F(("Electron_Pt"+histname).c_str(), "Electron_Pt",12,PtBins);h_electron_Pt[i]->Sumw2();
		h_electron_eta[i] = new TH1F(("Electron_eta"+histname).c_str(), "Electron_eta",20,-3.0, 3.0);h_electron_eta[i]->Sumw2();
		h_electron_SCEta[i] = new TH1F(("Electron_SCeta"+histname).c_str(), "Electron_SCeta",20, -3.0, 3.0);h_electron_SCEta[i]->Sumw2();
		h_electron_phi[i] = new TH1F(("Electron_phi"+histname).c_str(), "Electron_phi", 21,-3.14,3.14);h_electron_phi[i]->Sumw2();
		h_electron_SCPhi[i] = new TH1F(("electron_SCphi"+histname).c_str(), "electron_SCphi", 21,-3.14,3.14);h_electron_SCPhi[i]->Sumw2();
		h_electron_IDbit[i] = new TH1F(("electron_ID_bit"+histname).c_str(), "electron_ID_bit",8,0,8);h_electron_IDbit[i]->Sumw2();
		//**************  tau  **************
		h_tau_En[i] = new TH1F(("Tau_En"+histname).c_str(), "Tau_En",12,PtBins);h_tau_En[i]->Sumw2();
		h_tau_Pt[i] = new TH1F(("Tau_Pt"+histname).c_str(), "Tau_Pt",12,PtBins);h_tau_Pt[i]->Sumw2();
		h_tau_eta[i] = new TH1F(("Tau_eta"+histname).c_str(), "Tau_eta",20,-3.0, 3.0);h_tau_eta[i]->Sumw2();

		h_tau_phi[i] = new TH1F(("Tau_phi"+histname).c_str(), "Tau_phi", 21,-3.14,3.14);h_tau_phi[i]->Sumw2();
		//**************  Met  **************
		h_pfMET[i] = new TH1F(("pfMET"+histname).c_str(), "pfMET",14,MetBins);h_pfMET[i]->Sumw2();

		h_dPhi[i] = new TH1F(("h_dPhi"+histname).c_str(),"h_dPhi",20,0,3.15);h_dPhi[i]->Sumw2();
                h_dR[i] = new TH1F(("h_dR"+histname).c_str(),"h_dR",20,0,3.14);h_dR[i]->Sumw2();

		h_nJet[i] = new TH1F(("nJet"+histname).c_str(), "nJet",20,0,20);h_nJet[i]->Sumw2();
		h_leadingJetPt[i] = new TH1F(("leadingJetPt"+histname).c_str(),"leadingJetPt",30,20,1000);h_leadingJetPt[i]->Sumw2();

		h_leadingJetEta[i] = new TH1F(("h_leadingJetEta"+histname).c_str(),"h_leadingJetEta",40,-1.4442,1.4442);h_leadingJetEta[i]->Sumw2();

		//**************  rest  **************

		h_Mt[i]= new TH1F(("Mt"+histname).c_str(),"MT",40,0,1000);h_Mt[i]->Sumw2();
		h_VisibleMass[i]= new TH1F(("VisibleMass"+histname).c_str(),"VisibleMass",40,0,200);h_VisibleMass[i]->Sumw2();
		h_HiggsPt[i]= new TH1F(("HiggsPt"+histname).c_str(),"HiggsPt",12,PtBins);h_HiggsPt[i]->Sumw2();
                h_electronIso[i]= new TH1F(("Electron iso"+histname).c_str(),"Electron iso",40,0.0,1.0);h_electronIso[i]->Sumw2();
                h_tauIso[i]= new TH1F(("Tau iso"+histname).c_str(),"Tau iso",10,-2.0,2.0);h_tauIso[i]->Sumw2();	       

	}
}

//Fill the sequential histos at a particular spot in the sequence
void post_analyzer_MC::fillHistos(int histoNumber, double event_weight,int lep_1_index, int lep_2_index)
{
	//*********** fill lep_1s  ***********
		h_lep_1_En[histoNumber]->Fill((eleEn->at(lep_1_index)),event_weight);
		h_lep_1_Pt[histoNumber]->Fill((elePt->at(lep_1_index)),event_weight);		
		h_lep_1_eta[histoNumber]->Fill(eleEta->at(lep_1_index),event_weight);
		h_lep_1_SCEta[histoNumber]->Fill(eleSCEta->at(lep_1_index),event_weight);
		h_lep_1_phi[histoNumber]->Fill(elePhi->at(lep_1_index),event_weight);
		h_lep_1_SCPhi[histoNumber]->Fill(eleSCPhi->at(lep_1_index),event_weight);
                h_lep_1_IDbit[histoNumber]->Fill( eleIDbit->at(lep_1_index)>>3&1,event_weight);
	
	//*********** fill lep_2s  ***********

		h_lep_2_En[histoNumber]->Fill((lep_2Energy->at(lep_2_index)),event_weight);
		h_lep_2_Pt[histoNumber]->Fill((lep_2Pt->at(lep_2_index)),event_weight);
		h_lep_2_eta[histoNumber]->Fill(lep_2Eta->at(lep_2_index),event_weight);
		h_lep_2_phi[histoNumber]->Fill(lep_2Phi->at(lep_2_index),event_weight);
		
	//*********** fill met  ***********		
		h_pfMET[histoNumber]->Fill(pfMET,event_weight);
		double dPhi_elep_2 = DeltaPhi(elePhi->at(lep_1_index),lep_2Phi->at(lep_2_index));
		h_dPhi[histoNumber]->Fill(dPhi_elep_2,event_weight);
		double dR_elep_2 = dR(lep_1_index,lep_2_index);
		h_dR[histoNumber]->Fill(dR_elep_2,event_weight);


	//*********** fill rest  ***********

		float mT_eMet = TMass_F((elePt->at(lep_1_index)),(elePhi->at(lep_1_index)),pfMET,pfMETPhi  );
		h_Mt[histoNumber]->Fill(mT_eMet,event_weight);
		TLorentzVector myLep_2; 
		myLep_2.SetPtEtaPhiE(lep_2Pt->at(lep_2_index),lep_2Eta->at(lep_2_index),lep_2Phi->at(lep_2_index), lep_2Energy->at(lep_2_index));		
		TLorentzVector myEle; 
		myEle.SetPtEtaPhiE(elePt->at(lep_1_index),eleEta->at(lep_1_index),elePhi->at(lep_1_index), eleEn->at(lep_1_index));
		double visMass_elep_2 = VisMass_F(myLep_2, myEle);
		h_VisibleMass[histoNumber]->Fill(visMass_elep_2,event_weight);
		double HiggsPt = pTvecsum_F(elePt->at(lep_1_index),lep_2Pt->at(lep_2_index),elePhi->at(lep_1_index),lep_2Phi->at(lep_2_index) );
		h_HiggsPt[histoNumber]->Fill(HiggsPt,event_weight);
		h_nVtx[histoNumber]->Fill(nVtx,event_weight);
		h_nEvents[histoNumber]->Fill(isData,event_weight);
		
		h_nJet[histoNumber]->Fill(nJet ,event_weight);
		//		if(jetsize>0){
		//h_leadingJetPt[histoNumber]->Fill(jetPt->at(),event_weight);
		//h_leadingJetPt_300[histoNumber]->Fill(jetPt->at(),event_weight);
		//h_leadingJetEta[histoNumber]->Fill(jetEta->at(),event_weight);
		//}
		//h_genHT[histoNumber]->Fill(genHT);
		//h_genWeight[histoNumber]->Fill(genWeight);
		float rel_Iso = ( elePFChIso->at(lep_1_index) + max( elePFNeuIso->at(lep_1_index) + elePFPhoIso->at(lep_1_index) - 0.5 *elePFPUIso->at(lep_1_index) , 0.0 )) / (elePt->at(lep_1_index));
                h_lep_1_Iso[histoNumber]->Fill(rel_Iso,event_weight);
                h_lep_2_Iso[histoNumber]->Fill(lep_2byTightIsolationMVArun2017v2DBoldDMwLT2017->at(lep_2_index),event_weight);

}

	




















