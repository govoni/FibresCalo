// g++ -Wall -o studyERes `root-config --cflags --glibs` dict.o setTDRStyle.cc studyERes.cpp

#include "setTDRStyle.h"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"



int absMat = 1;
float WFrac = 0.5;
int fibMat = 4;
float fibRad = 0.4;
float fibDist = 1.2;

std::string extraLabel = "_25X0";
std::string stageoutFolder = "/store/user/abenagli/FibresCalo/Apr11_3degTilt" + extraLabel + "/";



int main(int argc, char** argv)
{
  std::string particle = "e-";
  if( argc > 1 ) particle = std::string(argv[1]);
  
  if( argc > 2 ) absMat  = atoi(argv[2]);
  if( argc > 3 ) WFrac   = atof(argv[3]);
  if( argc > 4 ) fibMat  = atoi(argv[4]);
  if( argc > 5 ) fibRad  = atof(argv[5]);
  if( argc > 6 ) fibDist = atof(argv[6]);
  
  std::string label = Form("absMat%d-%.2f_fibMat%d_fibRad%.1f_fibDist%.1f",absMat,WFrac,fibMat,fibRad,fibDist);
  
  setTDRStyle();
  
  std::vector<int> energies;
  energies.push_back(1);
  //energies.push_back(10);
  //energies.push_back(30);
  //energies.push_back(50);
  //energies.push_back(100);
  //energies.push_back(300);
  
  
  
  //----------------------
  // define histogram maps
  
  std::map<int,TH1F*> map_E_fibres;
  std::map<int,TH1F*> map_E_fibres_absorber;
  
  std::map<int,TH1F*> h_r_M;
  std::map<int,TH1F*> h_X_0;
  
  std::map<int,TProfile*> p_radCont;
  std::map<int,TProfile*> p_longCont;
  
  
  //---------------
  // loop on energies  
  for(unsigned int energyIt = 0; energyIt < energies.size(); ++energyIt)
  {
    int energy = energies.at(energyIt);
    
    
    //-----------
    // open files
    TChain* tree = new TChain("tree","tree");
    tree -> Add( Form("~/eos/cms/%s/%s/run_%s_%dGeV/out*.root",stageoutFolder.c_str(),label.c_str(),particle.c_str(),energy) );
    std::cout << ">>> adding tree from file "
              << Form("~/eos/cms/%s/%s/run_%s_%dGeV/out*.root",stageoutFolder.c_str(),label.c_str(),particle.c_str(),energy)
              << "   nEvents: " << tree->GetEntries()
              << std::endl;
    
    if( tree->GetEntries() == 0 ) return -1;
    
    
    //----------------
    // define branches
    float depositedEnergyFibres;
    float depositedEnergyAbsorber;
    float Radial_stepLength;
    float Longitudinal_stepLength;
    float Radial_ion_energy_absorber[5000];
    float Longitudinal_ion_energy_absorber[5000];
    float PrimaryParticleX[1000];
    float PrimaryParticleY[1000];
    float PrimaryParticleZ[1000];
    float PrimaryParticleE[1000];
    std::vector<float>* inputInitialPosition = new std::vector<float>;
    
    tree -> SetBranchStatus("*",0);
    
    tree -> SetBranchStatus("depositedEnergyFibres",           1); tree -> SetBranchAddress("depositedEnergyFibres",           &depositedEnergyFibres);
    tree -> SetBranchStatus("depositedEnergyAbsorber",         1); tree -> SetBranchAddress("depositedEnergyAbsorber",         &depositedEnergyAbsorber);
    tree -> SetBranchStatus("Radial_stepLength",               1); tree -> SetBranchAddress("Radial_stepLength",               &Radial_stepLength);
    tree -> SetBranchStatus("Longitudinal_stepLength",         1); tree -> SetBranchAddress("Longitudinal_stepLength",         &Longitudinal_stepLength);   
    tree -> SetBranchStatus("Radial_ion_energy_absorber",      1); tree -> SetBranchAddress("Radial_ion_energy_absorber",       Radial_ion_energy_absorber);
    tree -> SetBranchStatus("Longitudinal_ion_energy_absorber",1); tree -> SetBranchAddress("Longitudinal_ion_energy_absorber", Longitudinal_ion_energy_absorber);
    tree -> SetBranchStatus("PrimaryParticleX",                1); tree -> SetBranchAddress("PrimaryParticleX",                 PrimaryParticleX);
    tree -> SetBranchStatus("PrimaryParticleY",                1); tree -> SetBranchAddress("PrimaryParticleY",                 PrimaryParticleY);
    tree -> SetBranchStatus("PrimaryParticleZ",                1); tree -> SetBranchAddress("PrimaryParticleZ",                 PrimaryParticleZ);
    tree -> SetBranchStatus("PrimaryParticleE",                1); tree -> SetBranchAddress("PrimaryParticleE",                 PrimaryParticleE);
    tree -> SetBranchStatus("inputInitialPosition",            1); tree -> SetBranchAddress("inputInitialPosition",            &inputInitialPosition);    
    
    //------------------
    // define histograms
    
    map_E_fibres[energy] = new TH1F(Form("h_E_fibres_%02dGeV",energy),"",5000,0.,energy);
    map_E_fibres[energy] -> Sumw2();
    
    map_E_fibres_absorber[energy] = new TH1F(Form("h_E_fibres_absorber_%02dGeV",energy),"",5000,0.,energy);
    map_E_fibres_absorber[energy] -> Sumw2();
    
    h_r_M[energy] = new TH1F(Form("h_r_M_%dGeV",energy),"",5000,0.,1000.);
    h_r_M[energy] -> Sumw2();
    
    h_X_0[energy] = new TH1F(Form("h_X_0_%dGeV",energy),"",2000,0.,2000.);
    h_X_0[energy] -> Sumw2();
    
    p_radCont[energy] = new TProfile(Form("p_radCont_%dGeV",energy),"",5000.,0.,2500.);
    p_longCont[energy] = new TProfile(Form("p_longCont_%dGeV",energy),"",5000.,0.,2500.);
    
    
    
    //---------------
    // loop on events
    //int entryMax = 1000;
    int entryMax = tree->GetEntries();
    for(int entry = 0; entry < entryMax; ++entry)
    {
      tree -> GetEntry(entry);
      if( entry%1 == 0 ) std::cout << ">>>>>> entry: " << entry << "\r" << std::flush;
      
      
      // radial containment
      float r_M = 0.;
      float sum = 0.;
      for(int i = 0; i < 5000; ++i)
      {
        sum += Radial_ion_energy_absorber[i];
        p_radCont[energy] -> Fill( i*2500./5000., sum/(depositedEnergyFibres+depositedEnergyAbsorber) );
        
        if( sum/(depositedEnergyFibres+depositedEnergyAbsorber) < 0.90 )
        {
          r_M = i * Radial_stepLength;
        }
      }
      h_r_M[energy] -> Fill(r_M);
      
      
      // longitudinal containment
      sum = 0;
      for(int i = 0; i < 5000; ++i)
      {
        sum += Longitudinal_ion_energy_absorber[i];
        p_longCont[energy] -> Fill( i*2500./5000., sum/(depositedEnergyFibres+depositedEnergyAbsorber) );
      }
      
      float X_0 = 0.;
      for(int iStep = 0; iStep < 1000; ++iStep)
      {
        if( 1.*PrimaryParticleE[iStep]/energy > 0.3679 )
        {
          X_0 = PrimaryParticleZ[iStep] - inputInitialPosition->at(2);
        }
      }
      h_X_0[energy] -> Fill(X_0);
      
      
      //----------------
      // fill histograms
      map_E_fibres[energy]          -> Fill( depositedEnergyFibres );
      map_E_fibres_absorber[energy] -> Fill( depositedEnergyFibres+depositedEnergyAbsorber );
    }
  }
  
  
  //---------
  // drawings
  TFile* outFile = TFile::Open(Form("studyERes_%s%s_%s.root",label.c_str(),extraLabel.c_str(),particle.c_str()),"RECREATE");
  outFile -> cd();
  
  for(unsigned int energyIt = 0; energyIt < energies.size(); ++energyIt)  
  {
    int energy = energies.at(energyIt);
    
    outFile -> mkdir(Form("%dGeV",energy));
    outFile -> cd(Form("%dGeV",energy));
    
    map_E_fibres[energy]          -> Write();
    map_E_fibres_absorber[energy] -> Write();
    
    h_r_M[energy] -> Write();
    h_X_0[energy] -> Write();
    
    p_radCont[energy] -> Write();
    p_longCont[energy] -> Write();
    
    outFile -> cd();
  }
  
  outFile -> Close();
}
