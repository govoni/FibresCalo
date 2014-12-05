#ifndef CreateTree_H
#define CreateTree_H 1

#include <iostream>
#include <vector>
#include <map>

#include "TH2F.h"
#include "TH3F.h"
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"



class CreateTree
{
private:
  
  TTree*  ftree ;
  TString fname ;
  
public:
  
  CreateTree (TString name,
              const std::vector<float>& attL) ;
  ~CreateTree () ;
  
  TTree*             GetTree  () const { return ftree ; } ;
  TString            GetName  () const { return fname ; } ;
  void               AddEnergyDeposit (int index, float deposit, std::map<int,float>& depositAtt) ;
  void               AddScintillationPhoton (int index) ;
  void               AddCerenkovPhoton (int index) ;
  int                Fill     () ;
  bool               Write    (TFile *) ;
  void               Clear    () ;
  static CreateTree* Instance () { return fInstance ; } ;
  static CreateTree* fInstance ;
  
  int   Event ;
  
  std::vector<float> * inputMomentum ; // Px Py Pz E
  std::vector<float> * inputInitialPosition ; // x, y, z
  
  float depositedEnergyTotal ;
  float depositedEnergyFibres ;
  float depositedEnergyAbsorber ;
  float depositedEnergyFibres_post ;
  float depositedEnergyAbsorber_post ;
  float depositedEnergyWorld ;
  std::vector<float> * depositedEnergyFibresAtt ;
  
  float totalTrackLengthFibres ;
  float totalTrackLengthOverThFibres ;
  
  int tot_phot_cer;
  int tot_phot_cer_post;
  int tot_gap_phot_cer;
  int tot_det_phot_cer;
  std::vector<float> * tot_gap_photFast_cer;
  std::vector<float> * tot_det_photFast_cer;
  
  // energy deposited in each fibre of a tower
  std::vector<float> * depositedEnergies ;
  std::vector<std::vector<float> > * depositedEnergiesAtt ;
  // index of the fibre where the deposit happens
  std::vector<int> * depositFibres ; 
  
  // scintillation photons produced in each fibre of a tower
  std::vector<int> * scintillationPhotons ;
  // index of the fibre where the deposit happens
  std::vector<int> * scintillationFibres ; 
  
  // cerenkov photons produced in each fibre of a tower
  std::vector<int> * cerenkovPhotons ;
  // index of the fibre where the deposit happens
  std::vector<int> * cerenkovFibres ; 
  
  float Radial_stepLength;
  float Longitudinal_stepLength;
  float Radial_ion_energy_absorber[5000];
  float Longitudinal_ion_energy_absorber[5000];
  
  float PrimaryParticleX[1000];
  float PrimaryParticleY[1000];
  float PrimaryParticleZ[1000];
  float PrimaryParticleE[1000];
  
  // histograms
  //TH1F* h_phot_cer_lambda;
  //TH1F* h_phot_cer_E;
  //TH1F* h_phot_cer_time;
  
  //TH1F* h_phot_cer_gap_lambda;
  //TH1F* h_phot_cer_gap_E;
  //TH1F* h_phot_cer_gap_time;
  
  //std::vector<TH1F*> h_photFast_cer_gap_lambda;
  //std::vector<TH1F*> h_photFast_cer_gap_E;
  //std::vector<TH1F*> h_photFast_cer_gap_time;
  
  // to be filled at the beginning of the event generation only
  TNtuple * fibresPosition ;
  TTree * attenuationLengths ;
  
  std::vector<float> * attLengths;
};

#endif
