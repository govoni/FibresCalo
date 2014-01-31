/*
 c++ -o plotResolution `root-config --glibs --cflags` -lGenVector -lm \
   scripts/plotResolution.cpp
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <cmath>
#include <cstdlib>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TString.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TRandom.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TVector2.h"

using namespace std ;


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


float distance (float x1, float y1, float x2, float y2)
{
  return sqrt (x1 * x1 + y1 * y1 + x2 * x2 + y2 * y2) ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


// each tower is assigned its geometrical center as position
std::pair<float, float>
getTowerPosition (int index, int totNumOfTowersPerLine, float towerSideLength)
{
  int ix = index % totNumOfTowersPerLine ;
  int iy = index / totNumOfTowersPerLine ;
  float totalSideLength = towerSideLength * totNumOfTowersPerLine ;
  float x = (ix + 0.5) * towerSideLength - 0.5 * totalSideLength ;
  float y = (iy + 0.5) * towerSideLength - 0.5 * totalSideLength ;
  return std::pair<float, float> (x, y) ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


// LY = photons per MeV in the fibre
float smearEnergy (float energy, float LY = 10000, 
                   float acceptance = 0.1, float efficiency = 0.1)
{
  LY *= 1000 ; // 1000 MeV = 1 GeV
  int nPh = floor (energy * acceptance * efficiency * LY) ;
  float result = gRandom->Poisson (nPh) ;
  return result / LY / acceptance / efficiency ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int main (int argc, char ** argv)
{
//  TRandom3 * gRandom ;
//  TString inputFile
  // init tree and basic vars
  // ---- ---- ---- ---- ---- ---- ----

  gROOT->ProcessLine ("#include <vector>") ; 

  if (argc < 2)
    {
      cout << "usage " << argv[0]
           << " inputFileName [calibration] " << endl ;
      exit (1) ;
    }
  
  TString inputFileName = argv[1] ;

  float calibration = -1. ; // from response to total deposited energy
  if (argc == 3)
    calibration = atof (argv[2]) ; // from response to total deposited energy

  cout << "running on  : " << argv[1] << endl ;
  cout << "calibration : " << calibration << endl ;
  
  float LY = 10000000 ; // photons per GeV 
  float acceptance = 0.1 ;
  float efficiency = 0.1 ;

  TFile * inputFile = new TFile (inputFileName) ;

  map<int, TVector2> positions_map ;
  TNtuple * fibresPosition = (TNtuple *) inputFile->Get ("fibresPosition") ;
  Float_t nt_x, nt_y, nt_N ;
  fibresPosition->SetBranchAddress ("N", &nt_N) ;
  fibresPosition->SetBranchAddress ("x", &nt_x) ;
  fibresPosition->SetBranchAddress ("y", &nt_y) ;

  //PG loop on the TNtuple
  for (int i = 0 ; i < fibresPosition->GetEntries () ; ++i)
    {
      fibresPosition->GetEntry (i) ;
      TVector2 point (nt_x, nt_y) ;
      positions_map[nt_N] = point ;
    }

  TTree * tree = (TTree *) inputFile->Get ("tree") ;
  Int_t nentries = tree->GetEntries () ;
  cout << "found " << nentries << " events\n" ;

  
  // prepare input variables
  // ---- ---- ---- ---- ---- ---- ----

  double depositedEnergy ;
  tree->Branch ("depositedEnergy", &depositedEnergy, "depositedEnergy/D") ;
  std::vector<float> * depositedEnergies = new std::vector<float> ; 
  tree->SetBranchAddress ("depositedEnergies", &depositedEnergies) ;
  std::vector<float> * depositFibres = new std::vector<float> ; 
  tree->SetBranchAddress ("depositFibres", &depositFibres) ;
  std::vector<float> * inputMomentum = new std::vector<float> (4) ; 
  tree->SetBranchAddress ("inputMomentum", &inputMomentum) ;
  std::vector<float> * inputInitialPosition = new std::vector<float> (3) ; 
  tree->SetBranchAddress ("inputInitialPosition", &inputInitialPosition) ;

  // get the energy calibration value
  // ---- ---- ---- ---- ---- ---- ----  

  vector<float> calibVect ;

  // loop over entries, for calibration
  for (Int_t iEntry = 0 ; iEntry < min (1000, nentries) ; ++iEntry) 
	{
	  tree->GetEvent (iEntry) ;
      float observedEnergy = 0. ;
      // loop on the towers composing the calorimeter
	  for (int j = 0 ; j < depositedEnergies->size () ; ++j)
	   {
		 observedEnergy += depositedEnergies->at (j) ;
	   } // loop on the towers composing the calorimeter
   
	  float beamEnergy = inputMomentum->at (3) ;
	  float ErecoOverEgen = observedEnergy / beamEnergy ;
	  calibVect.push_back (ErecoOverEgen) ;
	} // loop over entries, for calibration

  float h_min = *(min_element (calibVect.begin (), calibVect.end ())) * 0.7 ;
  float h_max = *(max_element (calibVect.begin (), calibVect.end ())) * 1.3 ;
  TH1F h_ErecoOverEgen_calib ("h_ErecoOverEgen_calib", "h_ErecoOverEgen_calib", 
							  100, h_min, h_max) ;
  for (int i = 0 ; i < calibVect.size () ; ++i) 
    h_ErecoOverEgen_calib.Fill (calibVect.at (i)) ;
  h_ErecoOverEgen_calib.Fit ("gaus", "Q") ;
  calibration = 1. / h_ErecoOverEgen_calib.GetFunction ("gaus")->GetParameter (1) ;

  // output histograms
  // ---- ---- ---- ---- ---- ---- ----  
  
  // distributions for the energy response and calibration
  // .... .... .... .... .... .... ....  
  
  TH1F h_Egen ("h_Egen", "Egen", 200, 0, 200) ;
  TH1F h_ErecoOverEgen ("h_ErecoOverEgen", "ErecoOverEgen", 100, h_min, h_max) ;
  TH2F h_ErecoOverEgen_vs_Ebeam ("h_ErecoOverEgen_vs_Ebeam", "h_ErecoOverEgen_vs_Ebeam", 
                        45, 5, 50, 100, h_min, h_max) ;
  TH3F h_ErecoOverEgen_vs_impact ("h_ErecoOverEgen_vs_impact", "h_ErecoOverEgen_vs_impact", 
                        40, -2., 2., 40, -2., 2., 100, h_min, h_max) ;

  // distributions for the energy resolution
  // .... .... .... .... .... .... ....  

  // energy resolution, with the poissonian smearing
  TH1F h_sigmaEoverE ("sigmaEoverE", "sigmaEoverE", 100, -1., 1.) ;
  TH2F h_sigmaEoverE_vs_Ebeam ("sigmaEoverE_vs_Ebeam", "sigmaEoverE_vs_Ebeam", 
                      45, 5, 50, 500, -1., 1.) ;



  // events analysis
  // ---- ---- ---- ---- ---- ---- ----  

  float x = 0. ; 
  float y = 0. ;

  // loop over entries
  for (Int_t iEntry = 0 ; iEntry < nentries ; ++iEntry) 
    {
      if (iEntry % 100 == 0) 
        cout << "reading event " << iEntry << endl ;
      tree->GetEvent (iEntry) ;
      float observedEnergy = 0. ;
      //PG loop over fibres
      // loop on the towers composing the calorimeter
      for (int j = 0 ; j < depositedEnergies->size () ; ++j)
       {
// removing the poisson smearing to check the stochastic term behaviour w/o that       
//        float result = gRandom->Poisson (floor (
//                   depositedEnergies->at (j) * acceptance * efficiency * LY)) ;
//        result /= LY * acceptance * efficiency ;
//        observedEnergy += result ;
         observedEnergy += depositedEnergies->at (j) ;
       } // loop on the towers composing the calorimeter
       
      float beamEnergy = inputMomentum->at (3) ;
      h_Egen.Fill (beamEnergy) ;
      float ErecoOverEgen = observedEnergy / beamEnergy ;
      h_ErecoOverEgen.Fill (ErecoOverEgen) ;
      h_ErecoOverEgen_vs_Ebeam.Fill (beamEnergy, ErecoOverEgen) ;
      h_ErecoOverEgen_vs_impact.Fill (inputInitialPosition->at (0),
        inputInitialPosition->at (1), ErecoOverEgen) ;  

      float sigmaEoverE = (observedEnergy * calibration - beamEnergy) / beamEnergy ;
      h_sigmaEoverE.Fill (sigmaEoverE) ;
      h_sigmaEoverE_vs_Ebeam.Fill (beamEnergy, sigmaEoverE) ;
    } //PG loop over events

  TString outputFileName = "out_" + inputFileName ;
  TFile outfile (outputFileName, "recreate") ;
  outfile.cd () ;
  h_ErecoOverEgen_calib.Write () ;
  h_Egen.Write () ;
  h_ErecoOverEgen.Write () ;
  h_ErecoOverEgen_vs_Ebeam.Write () ;
  h_ErecoOverEgen_vs_impact.Write () ;
  h_sigmaEoverE.Write () ;
  h_sigmaEoverE_vs_Ebeam.Write () ;
  outfile.Close () ;

  return 0 ;
}

