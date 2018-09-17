#!/bin/sh
export pwd=/nfs_scratch/tost/CMSSW_9_4_4/src/analyzer/mutau
cat>Job_${6}.sh<<EOF
#!/bin/sh
source /cvmfs/cms.cern.ch/cmsset_default.sh 
cd /nfs_scratch/tost/CMSSW_9_4_4/src/analyzer/mutau
cmsenv
cd \${_CONDOR_SCRATCH_DIR}
./${1} ${2} ${3} ${4} ${5}
EOF

chmod 775 Job_${6}.sh

cat>condor_${6}<<EOF
x509userproxy = /tmp/x509up_u10034
universe = vanilla
Executable = Job_${6}.sh
Notification         = never
WhenToTransferOutput = On_Exit
ShouldTransferFiles  = yes
Requirements = (TARGET.UidDomain == "hep.wisc.edu" && TARGET.HAS_CMS_HDFS)
on_exit_remove       = (ExitBySignal == FALSE && (ExitCode == 0 || ExitCode == 42 || NumJobStarts>3))
+IsFastQueueJob      = True
getenv = true
request_memory       = 1992
request_disk         = 2048000
Transfer_Input_Files = ${1}, /nfs_scratch/tost/CMSSW_9_4_4/src/analyzer/mutau/RunBCDEF_mc_ID.root, /nfs_scratch/tost/CMSSW_9_4_4/src/analyzer/mutau/EfficienciesAndSF_RunBtoF_Nov17Nov2017.root
output               = junk\$(Process)_${6}.out
error                = junk\$(Process)_${6}.err
Log                  = junk\$(Process)_${6}.log
Queue
EOF

condor_submit condor_${6}

