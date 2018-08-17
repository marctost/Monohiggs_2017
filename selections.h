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


bool trigger(string final_state){
    if (final_state="mutau"){
        
    }
}


int isMuon(){
    int number = -1;
    for (int counter=0; counter<num; counter++){
        if (selections){
            number = counter;
        }
    }
    return number;
}

int isElectron(){
    int number = -1;
    for (int counter=0; counter<num; counter++){
        if (selections){
            number = counter;
        }
    }
    return number;
}

int isTau(){
    int number = -1;
    for (int counter=0; counter<num; counter++){
        if (selections){
            number = counter;
        }
    }
    return number;
}

bool rejectMuon(){
    bool reject=false;
    for (int counter=0; counter<num; counter++){
        if (veto_selections){
            reject=true;
        }
    }
    return reject;
}

bool rejectElectron(){
    bool reject=false;
    for (int counter=0; counter<num; counter++){
        if (veto_selections){
            reject=true;
        }
    }
    return reject;
}

bool BjetVeto(){
    int number = -1;
    bool reject=false;
    for (int counter=0; counter<num; counter++){
        if (selections){
            number = counter;
        }
    }
    if (number>=0){
        reject = true;
    }
    else{
        reject = false;
    }
    return reject;
}





#endif	/* selections_H */
