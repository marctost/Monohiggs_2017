# Monohiggs_2017
Repo for post-analyzer and plotting tools used in the 2017 mono-Higgs analysis.

To download:
* cmsrel CMSSW_9_4_4
* cd src/
* cmsenv
* git clone https://github.com/marctost/Monohiggs_2017

----------------------------------------
Explanation of all the programs:
* The post-analyzer itself is called "post_analyzer." There are two of them, one for data and one for the MC. It uses functions stored in the various header files.
* myHelper.h makes all the higher order variabls, like dR and visible mass. Written by someone else.
* higher_vars.h creates a data structure that, using myHelper, makes it easy to access all the higher order variables. Called in a single line in post-analyzer.
* selections.h is where all the selections are stored, including trigger, b-jet veto and all the lepton stuff. This will be the most manupulated file of them all, consequently it is layed out to be easy to look at to minimize mistakes.
* scale_factors.h applies scale factors to the analyzer.
* cross_sections.py contains the cross sections of the various MC samples, will be used with the plotter, eventually.
