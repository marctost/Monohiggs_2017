#!/bin/bash
#rm plots/png/estimate.csv

histNumber="0"
#for ((i=1;i<=6;i+=1)
for i in {0..3}

do
    echo $i
    histNumber=$i
#    python makeplot_mutauh.py Tau_Pt_ $histNumber pT tau -logYaxis logy
    python makeplot_mutauh.py Tau_Pt_ $histNumber pT tau -logYaxis logy
    python makeplot_mutauh.py Muon_Pt_ $histNumber pT mu -logYaxis logy
    python makeplot_mutauh.py leadingJetPt_ $histNumber pT jet -logYaxis logy
    python makeplot_mutauh.py h_dPhi_ $histNumber '#Delta' 'phi' -logYaxis logy 
    python makeplot_mutauh.py VisibleMass_ $histNumber 'm_{vis}' '_' -logYaxis logy
    python makeplot_mutauh.py pfMET_ $histNumber MET '_' -logYaxis logy
    python makeplot_mutauh.py HiggsPt_ $histNumber pT 'vector sum' -logYaxis logy
#python makeplot_mutauh.py pTsum pT 'scalar sum' -logYaxis logy
#python makeplot_mutauh.py mttot_1 total 'transverse mass' -logYaxis logy
    python makeplot_mutauh.py Mt_ $histNumber 'mu-met' ' transverse mass' -logYaxis logy 
#python makeplot_mutauh.py deltaR '#Delta' R -logYaxis logy
#python makeplot_mutauh.py mjj mjj -logYaxis logy
#python makeplot_mutauh.py tauIso tau iso -logYaxis logy
#python makeplot_mutauh.py taudecay tau decay_mode -logYaxis logy
#python makeplot_mutauh.py iso_1 ele iso -logYaxis logy
    python makeplot_mutauh.py Tau_eta_ $histNumber tau '#eta' -logYaxis logy
    python makeplot_mutauh.py Muon_eta_ $histNumber mu '#eta' -logYaxis logy
    python makeplot_mutauh.py Tau_phi_ $histNumber tau '#phi' -logYaxis logy
    python makeplot_mutauh.py Muon_phi_ $histNumber mu '#phi' -logYaxis logy
#python makeplot_mutauh.py dphi_eMet '#Delta phi' 'ele-Met' -logYaxis logy
#python makeplot_mutauh.py dphi_tauMet '#Delta phi' 'tau-Met' -logYaxis logy
# python makeplot_unroll.py visMmet visible mass -logYaxis logy
    python makeplot_mutauh.py nVtx_ $histNumber npv " " -logYaxis logy
#python makeplot_mutauh.py nJet_ njets " " 


done

python makeplot_mutauh.py Events_level_ _ events eachlevel -logYaxis logy
python makeplot_mutauh.py MET _0  MET '_' -logYaxis logy
python makeplot_mutauh.py MET _1  MET '_' -logYaxis logy
python makeplot_mutauh.py MET _2  MET '_' -logYaxis logy
python makeplot_mutauh.py MET _3  MET '_' -logYaxis logy
python makeplot_mutauh.py MET _4  MET '_' -logYaxis logy


