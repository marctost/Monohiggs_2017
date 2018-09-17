#ifndef higher_vars_H
#define	higher_vars_H

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
#include "TMath.h" //M_PI is in TMath
#include "TRandom3.h"
#include <TLorentzVector.h>
#include "myHelper.h"


struct variables_t {
    float vis_pt;
    float inv_mass;
    float mt_total;
    float dr;
};


variables_t makeHigherVariables(float pt_1, float pt_2, float eta_1, float eta_2, float phi_1, float phi_2, float charge_1, float charge_2, float energy_1, float energy_2, float pfMET, float pfMETPhi){
    
    TLorentzVector lept_1; lept_1.SetPtEtaPhiE(pt_1, eta_1, phi_1, energy_1);
    TLorentzVector lept_2; lept_2.SetPtEtaPhiE(pt_2, eta_2, phi_2, energy_2);
    TLorentzVector pfMET_h; pfMET_h.SetPtEtaPhiM(pfMET, 0, pfMETPhi, 0);
    
    variables_t use;
    
    use.vis_pt = pTvecsum_F(pt_1*cos(phi_1), pt_2*cos(phi_2), pt_1*sin(phi_1), pt_2*sin(phi_2));
    use.inv_mass = (lept_1+lept_2).M();
    use.mt_total = (lept_1+lept_2+pfMET_h).M();
    use.dr = dR(eta_1, phi_1, eta_2, phi_2);
    
    return use;
    
}


#endif	/* higher_vars_H */

