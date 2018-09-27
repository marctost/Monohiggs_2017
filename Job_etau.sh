#!/bin/sh
source /cvmfs/cms.cern.ch/cmsset_default.sh 
cd /nfs_scratch/tost/CMSSW_9_4_4/src/Monohiggs_2017
cmsenv
cd ${_CONDOR_SCRATCH_DIR}
.//hdfs/store/user/jmadhusu/Data_2017/monoHiggs_data_2017/SingleElectron/crab_sEle_dataset2/180903_143332/0000 data_etau_SE_2_0.root -1 1000 data_etau_SE_2_0 etau
