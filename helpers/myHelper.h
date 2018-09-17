#ifndef MYHELPER_H
#define	MYHELPER_H

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

float norm_F(float x, float y){
    return sqrt(x*x+y*y);
}

float TMass_F(float pt3lep, float px3lep, float py3lep, float met, float metPhi) {
    return sqrt(pow(pt3lep + met, 2) - pow(px3lep + met * cos(metPhi), 2) - pow(py3lep + met * sin(metPhi), 2));
}
float TotTMass_F(TLorentzVector a, TLorentzVector b, TLorentzVector met) {
    return (a + b+ met).M();
}
float deltaPhi(float a, float b) {
    float result = a - b;
    while (result > M_PI) result -= 2 * M_PI;
    while (result <= -M_PI) result += 2 * M_PI;
    return fabs(result);
}

float dPhi(TLorentzVector a, TLorentzVector b) {
    float result = a.Phi() - b.Phi();
    while (result > M_PI) result -= 2 * M_PI;
    while (result <= -M_PI) result += 2 * M_PI;
    return fabs(result);
}

float dR(float l1eta, float l1phi, float l2eta, float l2phi) {
    float deta = l1eta - l2eta;
    float dphi = deltaPhi(l1phi, l2phi);
    return sqrt(deta * deta + dphi * dphi);
}


float TMass_F(float et1, float et2, float px1, float px2, float py1, float py2) {
    //std::cout<<"mt "<<pow(et1 + et2, 2)<<" "<<pow(px1 + px2, 2)<<" "<<pow(py1 + py2, 2)<<" "<<pow(et1 + et2, 2) - pow(px1 + px2, 2) - pow(py1 + py2, 2)<<std::endl;
    return sqrt(pow(et1 + et2, 2) - pow(px1 + px2, 2) - pow(py1 + py2, 2));

}

double InvarMass_F(float e1, float e2, float px1, float px2, float py1, float py2, float pz1, float pz2) {
    return sqrt(pow(e1 + e2, 2) - pow(px1 + px2, 2) - pow(py1 + py2, 2) - pow(pz1 + pz2, 2));
}

double pTvecsum_F(float px1, float px2, float py1, float py2) {
    return sqrt( pow(px1 + px2, 2) + pow(py1 + py2, 2));
}



#endif	/* MYHELPER_H */

