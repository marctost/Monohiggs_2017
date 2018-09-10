#define post_analyzer_MC_cxx
#include "post_analyzer_MC.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TH1F.h"
#include <iostream>
#include <bitset>
#include <climits>
#include <cstring>
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TStopwatch.h"
#include <algorithm>
#include <vector>
#include <iterator>
#include <list>
#include <set>
#include "TMath.h" //M_PI is in TMath
#include "TRandom3.h"
#include <TLorentzVector.h>
/*
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
#include <vector> */
#include "myHelper.h"
#include "selections.h"
#include "scale_factors.h"
#include "higher_vars.h"


using namespace std;
using std::vector;


int main(int argc, const char* argv[])
{
  std::string save_name = argv[2];
  Long64_t maxEvents = atof(argv[3]);
  int reportEvery = atof(argv[4]);	
  std::string SampleName = argv[5];
  std::string finalState = argv[6];
  
  if (maxEvents < -1LL){
    std::cout<<"Please enter a valid value for maxEvents (parameter 3)."<<std::endl;
    return 1;
  }
  if (reportEvery < 1){
    std::cout<<"Please enter a valid value for reportEvery (parameter 4)."<<std::endl;
    return 1;
  }
  post_analyzer_MC t(argv[1], argv[2]);
  t.Loop(maxEvents,reportEvery, SampleName, save_name, finalState);
  return 0;
}



void post_analyzer_MC::Loop(Long64_t maxevents, int reportEvery, string SampleName, string save_name, string finalState){
	if (fChain==0) return;
	
	TString sample = TString(SampleName);
	TString final_state = TString(finalState);

	Long64_t nentries = fChain->GetEntries();
	std::cout<<"Coming in: "<<std::endl;  
	std::cout<<"nentries:"<<nentries<<std::endl;
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

	int nHLTPassed,n_eventWeight, nSingleTrgPassed, nGoodElectronPassed, nElectronPtPassed, nGoodTauPassed, nTauPtPassed,numberOfEvents,nMETPassed, nDPhiPassed, nqcdden,nqcdnum, nMETFiltersPassed,nLeptonVetoPassed,nPassedBjetVeto,nNoisyCrystals,nDPhiJetMETPassed, nGoodETauPassed,nDeltaRPassed,nPassedThirdLepVeto, nPassedHiggsPtcut, nPassedVisibleMasscut, nPassedMETcut, nFinal_afterSelections;
	nHLTPassed = n_eventWeight = nSingleTrgPassed = nGoodElectronPassed = nElectronPtPassed = nGoodTauPassed = nTauPtPassed= numberOfEvents = nMETPassed = nDPhiPassed = nqcdden= nqcdnum=nMETFiltersPassed= nLeptonVetoPassed=nPassedBjetVeto=nNoisyCrystals=nDPhiJetMETPassed= nGoodETauPassed = nDeltaRPassed= nPassedThirdLepVeto=nPassedHiggsPtcut=nPassedVisibleMasscut=nPassedMETcut=nFinal_afterSelections=0;

	TH1F* h_Events_level= new TH1F("Events_level","Events at each level of selection",15,0,30);  


   	for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++) {
	  
	  event_.clear(); 
	  event_info.clear(); 
	  Long64_t i = LoadTree(jentry);
	  if (i < 0) break;
	  nb = fChain->GetEntry(jentry);   nbytes += nb;
	  
	  if (jentry%reportEvery == 0){
	    std::cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck-1)<<std::endl;
	    fflush(stdout);
	  }
	  
	  
	  // Create variables for easier use later, using a lepton number instead of name. More flexible
	  numberOfEvents++;   
	  vector<float>* pt_1;
	  vector<float>* pt_2 = tauPt;
	  vector<float>* eta_1;
	  vector<float>* eta_2 = tauEta;
	  vector<float>* phi_1;
	  vector<float>* phi_2 = tauPhi;
	  vector<int>* charge_1;
	  vector<float>* charge_2 = tauCharge;
	  vector<float>* energy_1;
	  vector<float>* energy_2 = tauEnergy;
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
	  genWeight > 0.0 ? weight *= genWeight/fabs(genWeight) : weight = 0.0;
	  
	  // met filter selection
            if (!(metFilters==0)) continue;
	    nMETFiltersPassed++; 
	    
        
            // trigger selection
            // function "trig_num" is in the selections header, indicates which trigger is to be used
	    if (!(HLTEleMuX>>trig_num(final_state)&1==1)) continue;
	    nSingleTrgPassed++; 


            if (final_state=="mutau"){
                lept_num_1 = isMuon(nMu, muPt, muEta, muIDbit, muDz, muD0, muPFNeuIso, muPFPhoIso, muPFPUIso, muPFChIso);
            }
            else if (final_state=="etau"){
                lept_num_1 = isElectron(nEle, elePt, eleEta, eleIDbit, elePFNeuIso, elePFPhoIso, elePFPUIso, elePFChIso);
            }
            else if (final_state=="tautau"){
                //lept_num_1 = isFirstTau(variables);
            }
	    
        
            // first lepton selection
            if (lept_num_1<0) continue;
	    nGoodElectronPassed++; 

            if (final_state=="mutau"){
                // weight=weight*ID_SF(muPt->at(lept_num_1), muEta->at(lept_num_1), SF_histo)*trigger_SF(muPt->at(lept_num_1), muEta->at(lept_num_1), SF_histo);
            }
        
            if (final_state=="mutau" or final_state=="etau"){
                lept_num_2 = isTau(nTau, tauPt, tauEta, tauDecayMode, taudz, tauByMVA6TightElectronRejection, tauByLooseMuonRejection3, tauByTightIsolationMVArun2v1DBoldDMwLT);
            }
            else if (final_state=="tautau"){
                //lept_num_2 = isSecondTau(variables);
            }

            // second lepton selection
            if (lept_num_2<0) continue;
            fillHistos(5,weight,lept_num_1, lept_num_2, final_state);
	    nGoodTauPassed++;

            // extra lepton rejection
            bool rejectEle = rejectElectron(nEle, elePt, eleEta, eleD0, eleDz, eleIDbit, elePFNeuIso, elePFPhoIso, elePFPUIso, elePFChIso);
            bool rejectMu = rejectMuon(nMu, muPt, muEta, muD0, muDz, muIDbit, muPFNeuIso, muPFPhoIso, muPFPUIso, muPFChIso);
        
            if (final_state=="mutau" && rejectEle==true) continue;
            else if (final_state=="etau" && rejectMu==true) continue;
            else if (final_state=="tautau" && (rejectEle==true || rejectMu==true)) continue;
	    fillHistos(6,weight,lept_num_1, lept_num_2, final_state);
	    nPassedThirdLepVeto++;


            // charge requirement selection
            if (!(charge_1->at(lept_num_1)+charge_2->at(lept_num_2)==0)) continue;
            fillHistos(7,weight,lept_num_1, lept_num_2, final_state);
	    nGoodETauPassed++;  
        
            // this variable is a structure, and has the contents: vis_pt, inv_mass, mt_total and dr. Used like: higher_vars.mt_total
            variables_t higher_vars = makeHigherVariables(pt_1->at(lept_num_1), pt_2->at(lept_num_2), eta_1->at(lept_num_1), eta_2->at(lept_num_2), phi_1->at(lept_num_1), phi_2->at(lept_num_2), charge_1->at(lept_num_1), charge_2->at(lept_num_2), energy_1->at(lept_num_1), energy_2->at(lept_num_2), pfMET, pfMETPhi);

        
            // dR selection
            if (higher_vars.dr<0.3) continue;
            fillHistos(8,weight,lept_num_1, lept_num_2, final_state);
	    nDeltaRPassed++; 



            // bjet selection
            if (BjetVeto(nJet, jetCSV2BJetTags)==true) continue;
            fillHistos(9,weight,lept_num_1, lept_num_2, final_state);
	    nPassedBjetVeto++;   
    /*
            if (higher_vars.vis_pt  ) continue;
            fillHistos(10,weight,lept_num_1, lept_num_2, final_state);

            if (pfMET  ) continue;
            fillHistos(11,weight,lept_num_1, lept_num_2, final_state);

            if (higher_vars.inv_mass  ) continue;
            fillHistos(12,weight,lept_num_1, lept_num_2, final_state);
    */
     
	//    write_histos();

	h_Events_level->SetBinContent(1, numberOfEvents);
	h_Events_level->SetBinContent(2, nMETFiltersPassed);
	h_Events_level->SetBinContent(3, nSingleTrgPassed);          
	h_Events_level->SetBinContent(4, nGoodElectronPassed);
	h_Events_level->SetBinContent(5, nGoodTauPassed);
	h_Events_level->SetBinContent(8, nPassedThirdLepVeto);	  
	h_Events_level->SetBinContent(6, nGoodETauPassed);
	h_Events_level->SetBinContent(7, nDeltaRPassed);
	h_Events_level->SetBinContent(9, nPassedBjetVeto);
		

	h_Events_level->Scale(weight); 
	tree->Fill(); 
	}
	
	std::cout<<"*******************************************"<<std::endl;
	std::cout<<"****************** new post_analyzer*************************"<<std::endl;
	std::cout<<std::setw(20) <<std::right <<"Initial entries "<<numberOfEvents<<std::endl;
	std::cout<<std::setw(20) <<std::right <<"METFiltersPassed "<<nMETFiltersPassed<<std::endl;
	std::cout<<std::setw(20) <<std::right <<"SingleTrgPassed "<<nSingleTrgPassed<<std::endl;
	std::cout<<std::setw(20) <<std::right <<"GoodElectronPassed "<<nGoodElectronPassed<<std::endl;
	std::cout<<std::setw(20) <<std::right <<"GoodTauPassed "<<nGoodTauPassed<<std::endl;
	std::cout<<std::setw(20) <<std::right <<"PassedThirdLepVeto "<<nPassedThirdLepVeto<<std::endl;
	std::cout<<std::setw(20) <<std::right <<"opp charge "<<nGoodETauPassed<<std::endl;
	std::cout<<std::setw(20) <<std::right <<"DeltaRPassed "<<nDeltaRPassed<<std::endl;
	std::cout<<std::setw(20) <<std::right <<"PassedBjetVeto "<<nPassedBjetVeto<<std::endl;
	std::cout<<"*******************************************"<<std::endl;

}

		

void post_analyzer_MC::BookHistos(const char* file2)
{
  //TFile *fileName;
	fileName = new TFile(file2, "RECREATE");
	tree = new TTree("ADD","ADD");
	tree->Branch("event_","std::vector<unsigned int>",&event_);
	tree->Branch("event_info","std::vector<double>",&event_info);
	fileName->cd();
	
	Float_t PtBins[13]={0.0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200.,300., 400.};
	Float_t MetBins[15]={0.0, 20, 40, 60, 80, 100, 120, 140, 160,180., 200, 300., 400., 600.0,800.0};
	

	//Set up the histos to be filled with method fillHistos
	for(int i=1; i<10; i++)
	{
		char ptbins[100];
		sprintf(ptbins, "_%d", i);
		std::string histname(ptbins);
		h_nVtx[i] = new TH1F(("nVtx"+histname).c_str(), "nVtx",40,0,40);h_nVtx[i]->Sumw2();
		h_nEvents[i] = new TH1F(("nEvents"+histname).c_str(), "nEvents",3,0,3);h_nEvents[i]->Sumw2();
                h_genWeight[i] = new TH1F(("genWeight"+histname).c_str(), "genWeight",10, -10.0, 10.0);h_genWeight[i]->Sumw2();
                h_genHT[i] = new TH1F(("genHT"+histname).c_str(), "genHT",12,PtBins);h_genHT[i]->Sumw2();
		//**************  electrons  **************
		h_lep_1_En[i] = new TH1F(("Lepton_1_En"+histname).c_str(), "Lepton_1_En",12,PtBins);h_lep_1_En[i]->Sumw2();
		h_lep_1_Pt[i] = new TH1F(("Lepton_1_Pt"+histname).c_str(), "Lepton_1_Pt",12,PtBins);h_lep_1_Pt[i]->Sumw2();
		h_lep_1_eta[i] = new TH1F(("Lepton_1_eta"+histname).c_str(), "Lepton_1_eta",20,-3.0, 3.0);h_lep_1_eta[i]->Sumw2();
		h_lep_1_SCEta[i] = new TH1F(("Lepton_1_SCeta"+histname).c_str(), "Lepton_1_SCeta",20, -3.0, 3.0);h_lep_1_SCEta[i]->Sumw2();
		h_lep_1_phi[i] = new TH1F(("Lepton_1_phi"+histname).c_str(), "Lepton_1_phi", 21,-3.14,3.14);h_lep_1_phi[i]->Sumw2();
		h_lep_1_SCPhi[i] = new TH1F(("Lepton_1_SCphi"+histname).c_str(), "Lepton_1_SCphi", 21,-3.14,3.14);h_lep_1_SCPhi[i]->Sumw2();
		h_lep_1_IDbit[i] = new TH1F(("Lepton_1_ID_bit"+histname).c_str(), "Lepton_1_ID_bit",8,0,8);h_lep_1_IDbit[i]->Sumw2();
		//**************  tau  **************
		h_lep_2_En[i] = new TH1F(("Lepton_2_En"+histname).c_str(), "Lepton_2_En",12,PtBins);h_lep_2_En[i]->Sumw2();
		h_lep_2_Pt[i] = new TH1F(("Lepton_2_Pt"+histname).c_str(), "Lepton_2_Pt",12,PtBins);h_lep_2_Pt[i]->Sumw2();
		h_lep_2_eta[i] = new TH1F(("Lepton_2_eta"+histname).c_str(), "Lepton_2_eta",20,-3.0, 3.0);h_lep_2_eta[i]->Sumw2();

		h_lep_2_phi[i] = new TH1F(("Lepton_2_phi"+histname).c_str(), "Lepton_2_phi", 21,-3.14,3.14);h_lep_2_phi[i]->Sumw2();
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
                h_lep_1_Iso[i]= new TH1F(("Lepton_1 iso"+histname).c_str(),"Lepton_1 iso",40,0.0,1.0);h_lep_1_Iso[i]->Sumw2();
                h_tauIso[i]= new TH1F(("Tau iso"+histname).c_str(),"Tau iso",10,-2.0,2.0);h_tauIso[i]->Sumw2();	       

	}
}

//Fill the sequential histos at a particular spot in the sequence
void post_analyzer_MC::fillHistos(int histoNumber, double event_weight,int lep_1_index, int lep_2_index, TString final_state)
{
    
	h_pfMET[histoNumber]->Fill(pfMET,event_weight);
	h_nVtx[histoNumber]->Fill(nVtx, event_weight);
	h_nEvents[histoNumber]->Fill(isData, event_weight);
	h_nJet[histoNumber]->Fill(nJet, event_weight);
 

	cout<<final_state<<endl;
        if (final_state == "etau")
	{
	  cout<<"WE here"<<endl;
	  //*********** fill lep_1s  ***********
	  h_lep_1_En[histoNumber]->Fill((eleEn->at(lep_1_index)),event_weight);
	  h_lep_1_Pt[histoNumber]->Fill((elePt->at(lep_1_index)),event_weight);
	  h_lep_1_eta[histoNumber]->Fill(eleEta->at(lep_1_index),event_weight);
	  h_lep_1_phi[histoNumber]->Fill(elePhi->at(lep_1_index),event_weight);
	  
	  //*********** fill lep_2s  ***********
	  
	  h_lep_2_En[histoNumber]->Fill((tauEnergy->at(lep_2_index)),event_weight);
	  h_lep_2_Pt[histoNumber]->Fill((tauPt->at(lep_2_index)),event_weight);
	  h_lep_2_eta[histoNumber]->Fill(tauEta->at(lep_2_index),event_weight);
	  h_lep_2_phi[histoNumber]->Fill(tauPhi->at(lep_2_index),event_weight);
      
	}
        if (final_state == "mutau")
        {
          cout<<"does this work"<<endl;
          //*********** fill lep_1s  ***********
          h_lep_1_En[histoNumber]->Fill((muEn->at(lep_1_index)),event_weight);
          h_lep_1_Pt[histoNumber]->Fill((muPt->at(lep_1_index)),event_weight);
          h_lep_1_eta[histoNumber]->Fill(muEta->at(lep_1_index),event_weight);
          h_lep_1_phi[histoNumber]->Fill(muPhi->at(lep_1_index),event_weight);

          //*********** fill lep_2s  ***********

          h_lep_2_En[histoNumber]->Fill((tauEnergy->at(lep_2_index)),event_weight);
          h_lep_2_Pt[histoNumber]->Fill((tauPt->at(lep_2_index)),event_weight);
          h_lep_2_eta[histoNumber]->Fill(tauEta->at(lep_2_index),event_weight);
          h_lep_2_phi[histoNumber]->Fill(tauPhi->at(lep_2_index),event_weight);

        }

      /* double dPhi = fabs(elePhi->at(lep_1_index)- tauPhi->at(lep_2_index));
      h_dPhi[histoNumber]->Fill(dPhi,event_weight);
      h_dR[histoNumber]->Fill(higher_variables.dr,event_weight);
      
      h_Mt[histoNumber]->Fill(higher_variables.mt_total, event_weight);
      h_VisibleMass[histoNumber]->Fill(higher_variables.inv_mass, event_weight);
      h_HiggsPt[histoNumber]->Fill(higher_variables.vis_pt, event_weight);
      
      
      float rel_Iso;
      if (final_state=="etau"){
	rel_Iso = ( elePFChIso->at(lep_1_index) + max( elePFNeuIso->at(lep_1_index) + elePFPhoIso->at(lep_1_index) - 0.5 *elePFPUIso->at(lep_1_index) , 0.0 )) / (elePt->at(lep_1_index));
      }
      else if (final_state=="mutau"){
	rel_Iso = (muPFChIso->at(lep_1_index) + TMath::Max(0.0, muPFNeuIso->at(lep_1_index) + muPFPhoIso->at(lep_1_index) - 0.5*muPFPUIso->at(lep_1_index)))/(muPt->at(lep_1_index));
      }
      h_lep_1_Iso[histoNumber]->Fill(rel_Iso,event_weight);
      h_lep_2_Iso[histoNumber]->Fill(tauByTightIsolationMVArun2v1DBoldDMwLT->at(lep_2_index),event_weight);
      */
    
    
}
