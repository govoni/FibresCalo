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
#include "TProfile2D.h"
#include "TRandom3.h"



int absMat = 1;
float WFrac = 0.5;
int fibMat = 4;
float fibRad = 0.4;
float fibDist = 1.2;

std::string extraLabel = "_3degTilt_25X0_cerenkov_clad1.0";
std::string stageoutFolder = "~/eos/cms/store/user/abenagli/FibresCalo/Apr24" + extraLabel + "/";

float LY = 1000;
float eff = 0.01;
float PDE = 0.0932574;



int main(int argc, char** argv)
{
  std::string particle = "e-";
  if( argc > 1 ) particle = std::string(argv[1]);
  
  if( argc > 2 ) absMat  = atoi(argv[2]);
  if( argc > 3 ) WFrac   = atof(argv[3]);
  if( argc > 4 ) fibMat  = atoi(argv[4]);
  if( argc > 5 ) fibRad  = atof(argv[5]);
  if( argc > 6 ) fibDist = atof(argv[6]);
  
  float m = 1. / (1.5 * sqrt(3)/2. * fibDist);
  
  std::string label = Form("absMat%d-%.2f_fibMat%d_fibRad%.1f_fibDist%.1f",absMat,WFrac,fibMat,fibRad,fibDist);
  
  setTDRStyle();
  
  std::vector<int> energies;
  energies.push_back(1);
  energies.push_back(5);
  energies.push_back(10);
  energies.push_back(30);
  energies.push_back(50);
  energies.push_back(100);
  energies.push_back(300);
  
  std::vector<float>* attLengths = new std::vector<float>;
  
  TRandom3 r;
  
  
  
  //----------------------
  // define histogram maps
  
  std::map<int,TH1F*> map_E_fibres_absorber;
  
  std::map<int,TH1F*> map_E_fibres;
  std::map<int,TH1F*> map_Npe_fibres;
  std::map<int,TH1F*> map_NpeSmear_fibres;
  std::map<int,std::map<int,TH1F*> > map_EAtt_fibres;
  std::map<int,std::map<int,TH1F*> > map_NpeAtt_fibres;
  std::map<int,std::map<int,TH1F*> > map_NpeSmearAtt_fibres;
  std::map<int,std::map<int,TProfile*> > map_NpeAtt_fibres_vs_dist;
  
  std::map<int,TProfile*> map_E_fibres_vs_dist;
  std::map<int,TProfile2D*> map_E_fibres_vs_initialPosition;
  
  std::map<int,TH1F*> map_NCerPhot;
  std::map<int,TH2F*> map_NCerPhot_vs_E;
  std::map<int,std::map<int,TH1F*> > map_NCerPhotAtt_gap;
  std::map<int,std::map<int,TH1F*> > map_NCerPhotSmearAtt_gap;
  std::map<int,std::map<int,TH2F*> > map_NCerPhotAtt_gap_vs_NCerPhot;
  std::map<int,std::map<int,TProfile*> > map_NCerPhotAtt_gap_vs_dist;
  std::map<int,std::map<int,TProfile*> > map_effPhotAtt_gap_vs_NCerPhot;
  
  std::map<int,TProfile*> map_NCerPhot_vs_dist;
  std::map<int,TProfile2D*> map_NCerPhot_vs_initialPosition;
  
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
    int nFiles = tree -> Add( Form("%s/%s/run_%s_%dGeV/out*.root",stageoutFolder.c_str(),label.c_str(),particle.c_str(),energy) );
    int nEntries = tree->GetEntries();
    std::cout << ">>> adding " << nFiles << " trees from files "
              << Form("%s/%s/run_%s_%dGeV/out*.root",stageoutFolder.c_str(),label.c_str(),particle.c_str(),energy)
              << "   nEvents: " << nEntries
              << std::endl;
    
    if( nEntries == 0 ) continue;
    
    TChain* attenuationLengths = new TChain("attenuationLengths","attenuationLengths");
    attenuationLengths -> Add( Form("%s/%s/run_%s_%dGeV/out*.root",stageoutFolder.c_str(),label.c_str(),particle.c_str(),energy) );    
    
    
    //----------------
    // define branches
    attenuationLengths -> SetBranchAddress("attLengths",&attLengths);
    attenuationLengths -> GetEntry(0);
    
    float depositedEnergyAbsorber;
    float depositedEnergyFibres;
    std::vector<float>* depositedEnergyFibresAtt = new std::vector<float>;
    int tot_phot_cer;
    std::vector<float>* tot_gap_photFast_cer = new std::vector<float>;
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
    
    tree -> SetBranchStatus("depositedEnergyAbsorber",         1); tree -> SetBranchAddress("depositedEnergyAbsorber",         &depositedEnergyAbsorber);
    tree -> SetBranchStatus("depositedEnergyFibres",           1); tree -> SetBranchAddress("depositedEnergyFibres",           &depositedEnergyFibres);
    tree -> SetBranchStatus("depositedEnergyFibresAtt",        1); tree -> SetBranchAddress("depositedEnergyFibresAtt",        &depositedEnergyFibresAtt);
    tree -> SetBranchStatus("tot_phot_cer",                    1); tree -> SetBranchAddress("tot_phot_cer",                    &tot_phot_cer);
    tree -> SetBranchStatus("tot_gap_photFast_cer",            1); tree -> SetBranchAddress("tot_gap_photFast_cer",            &tot_gap_photFast_cer);
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
    
    map_E_fibres_absorber[energy] = new TH1F(Form("h_E_fibres_absorber_%03dGeV",energy),"",50000,0.,energy);
    map_E_fibres_absorber[energy] -> Sumw2();
    
    map_E_fibres[energy] = new TH1F(Form("h_E_fibres_%03dGeV",energy),"",50000,0.,energy);
    map_E_fibres[energy] -> Sumw2();
    
    map_Npe_fibres[energy] = new TH1F(Form("h_Npe_fibres_%03dGeV",energy),"",50000,0.,energy*LY*1000*eff);
    map_Npe_fibres[energy] -> Sumw2();
    
    map_NpeSmear_fibres[energy] = new TH1F(Form("h_NpeSmear_fibres_%03dGeV",energy),"",50000,0.,energy*LY*1000*eff);
    map_NpeSmear_fibres[energy] -> Sumw2();
    
    map_NCerPhot[energy] = new TH1F(Form("h_NCerPhot_%03dGeV",energy),"",5000,0.,40000*energy);
    map_NCerPhot[energy] -> Sumw2();
    
    map_NCerPhot_vs_E[energy] = new TH2F(Form("h_NCerPhot_vs_E_%03dGeV",energy),"",500,0.,energy,500,0.,40000*energy);
    map_NCerPhot_vs_E[energy] -> Sumw2();
    
    
    for(unsigned int attLengthIt = 0; attLengthIt < attLengths->size(); ++attLengthIt)
    {
      int attLength = int( attLengths->at(attLengthIt) );
      
      (map_EAtt_fibres[energy])[attLength] = new TH1F(Form("h_EAttLength%04dmm_fibres_%03dGeV",attLength,energy),"",50000,0.,energy);
      (map_EAtt_fibres[energy])[attLength] -> Sumw2();
      
      (map_NpeAtt_fibres[energy])[attLength] = new TH1F(Form("h_NpeAttLength%04dmm_fibres_%03dGeV",attLength,energy),"",50000,0.,energy*LY*1000*eff);
      (map_NpeAtt_fibres[energy])[attLength] -> Sumw2();
      
      (map_NpeSmearAtt_fibres[energy])[attLength] = new TH1F(Form("h_NpeSmearAttLength%04dmm_fibres_%03dGeV",attLength,energy),"",50000,0.,energy*LY*1000*eff);
      (map_NpeSmearAtt_fibres[energy])[attLength] -> Sumw2();
      
      (map_NpeAtt_fibres_vs_dist[energy])[attLength] = new TProfile(Form("p_NpeAttLength%04dmm_fibres_vs_dist_%03dGeV",attLength,energy),"",200,-2.*1.0*fibDist,2.*1.0*fibDist);
      (map_NpeAtt_fibres_vs_dist[energy])[attLength] -> Sumw2();
      
      (map_NCerPhotAtt_gap[energy])[attLength] = new TH1F(Form("h_NCerPhotAttLength%04dmm_gap_%03dGeV",attLength,energy),"",5000,0.,4000.*energy);
      (map_NCerPhotAtt_gap[energy])[attLength] -> Sumw2();
      
      (map_NCerPhotSmearAtt_gap[energy])[attLength] = new TH1F(Form("h_NCerPhotSmearAttLength%04dmm_gap_%03dGeV",attLength,energy),"",5000,0.,4000.*energy);
      (map_NCerPhotSmearAtt_gap[energy])[attLength] -> Sumw2();
      
      (map_NCerPhotAtt_gap_vs_NCerPhot[energy])[attLength] = new TH2F(Form("h_NCerPhotAttLength%04dmm_gap_vs_NCerPhot_%03dGeV",attLength,energy),"",500,0.,40000.*energy,500,0.,4000*energy);
      (map_NCerPhotAtt_gap_vs_NCerPhot[energy])[attLength] -> Sumw2();
      
      (map_NCerPhotAtt_gap_vs_dist[energy])[attLength] = new TProfile(Form("p_NCerPhotAttLength%04dmm_gap_vs_dist_%03dGeV",attLength,energy),"",200,-2.*1.0*fibDist,2.*1.0*fibDist);
      (map_NCerPhotAtt_gap_vs_dist[energy])[attLength] -> Sumw2();
      
      (map_effPhotAtt_gap_vs_NCerPhot[energy])[attLength] = new TProfile(Form("p_effPhotAttLength%04dmm_gap_vs_NCerPhot_%03dGeV",attLength,energy),"",500,0.,40000.*energy);
      (map_effPhotAtt_gap_vs_NCerPhot[energy])[attLength] -> Sumw2();
    }
    
    map_E_fibres_vs_dist[energy] = new TProfile(Form("p_E_fibres_vs_dist_%03dGeV",energy),"",200,-2.*1.0*fibDist,2.*1.0*fibDist);
    map_E_fibres_vs_initialPosition[energy] = new TProfile2D(Form("p2_E_fibres_vs_initialPosition_%03dGeV",energy),"",200,-1.3*fibDist,1.3*fibDist,200,-1.0*fibDist,1.0*fibDist);
    
    map_NCerPhot_vs_dist[energy] = new TProfile(Form("p_NCerPhot_vs_dist_%03dGeV",energy),"",200,-2.*1.0*fibDist,2.*1.0*fibDist);
    map_NCerPhot_vs_initialPosition[energy] = new TProfile2D(Form("p2_NCerPhot_vs_initialPosition_%03dGeV",energy),"",200,-1.3*fibDist,1.3*fibDist,200,-1.0*fibDist,1.0*fibDist);
    
    h_r_M[energy] = new TH1F(Form("h_r_M_%03dGeV",energy),"",5000,0.,1000.);
    h_r_M[energy] -> Sumw2();
    
    h_X_0[energy] = new TH1F(Form("h_X_0_%03dGeV",energy),"",2000,0.,2000.);
    h_X_0[energy] -> Sumw2();
    
    p_radCont[energy] = new TProfile(Form("p_radCont_%03dGeV",energy),"",5000.,0.,2500.);
    p_longCont[energy] = new TProfile(Form("p_longCont_%03dGeV",energy),"",5000.,0.,2500.);
    
    
    
    //---------------
    // loop on events
    //int entryMax = 1000;
    int entryMax = nEntries;
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
      
      map_E_fibres_absorber[energy]  -> Fill( depositedEnergyFibres+depositedEnergyAbsorber );
      
      map_E_fibres[energy]        -> Fill( depositedEnergyFibres );
      map_Npe_fibres[energy]      -> Fill( depositedEnergyFibres*LY*1000*eff );
      map_NpeSmear_fibres[energy] -> Fill( r.Poisson(depositedEnergyFibres*LY*1000*eff) );
      map_NCerPhot[energy]        -> Fill( tot_phot_cer );
      map_NCerPhot_vs_E[energy]   -> Fill( depositedEnergyFibres,tot_phot_cer );
      
      //float dist = fabs(inputInitialPosition->at(1));
      float dist = fabs(inputInitialPosition->at(1)-m*inputInitialPosition->at(0))/sqrt(1+m*m);
      if( inputInitialPosition->at(1) > inputInitialPosition->at(0)*m ) dist *= -1;
      
      for(unsigned int attLengthIt = 0; attLengthIt < attLengths->size(); ++attLengthIt)
      {
        int attLength = int( attLengths->at(attLengthIt) );
        (map_EAtt_fibres[energy])[attLength] -> Fill( depositedEnergyFibresAtt->at(attLengthIt) );
        (map_NpeAtt_fibres[energy])[attLength] -> Fill( depositedEnergyFibresAtt->at(attLengthIt)*LY*1000*eff );
        (map_NpeSmearAtt_fibres[energy])[attLength] -> Fill( r.Poisson(depositedEnergyFibresAtt->at(attLengthIt)*LY*1000*eff) );
        (map_NpeAtt_fibres_vs_dist[energy])[attLength] -> Fill( dist,depositedEnergyFibresAtt->at(attLengthIt)*LY*1000*eff );
        
        (map_NCerPhotAtt_gap[energy])[attLength] -> Fill( tot_gap_photFast_cer->at(attLengthIt) );
        (map_NCerPhotSmearAtt_gap[energy])[attLength] -> Fill( r.Poisson(tot_gap_photFast_cer->at(attLengthIt)*PDE) );
        (map_NCerPhotAtt_gap_vs_NCerPhot[energy])[attLength] -> Fill(tot_phot_cer,tot_gap_photFast_cer->at(attLengthIt) );
        (map_NCerPhotAtt_gap_vs_dist[energy])[attLength] -> Fill(dist,tot_gap_photFast_cer->at(attLengthIt) );
        (map_effPhotAtt_gap_vs_NCerPhot[energy])[attLength] -> Fill(tot_phot_cer,tot_gap_photFast_cer->at(attLengthIt)/tot_phot_cer );
      }

      map_E_fibres_vs_dist[energy] -> Fill(dist,depositedEnergyFibres);
      map_E_fibres_vs_initialPosition[energy] -> Fill(inputInitialPosition->at(0),inputInitialPosition->at(1),depositedEnergyFibres);
      map_NCerPhot_vs_dist[energy] -> Fill(dist,tot_phot_cer);
      map_NCerPhot_vs_initialPosition[energy] -> Fill(inputInitialPosition->at(0),inputInitialPosition->at(1),tot_phot_cer);
    }
  }
  
  
  //---------
  // drawings
  TFile* outFile = TFile::Open(Form("studyERes_%s%s_%s.root",label.c_str(),extraLabel.c_str(),particle.c_str()),"RECREATE");
  outFile -> cd();
  
  for(unsigned int energyIt = 0; energyIt < energies.size(); ++energyIt)  
  {
    int energy = energies.at(energyIt);
    
    outFile -> mkdir(Form("%03dGeV",energy));
    outFile -> cd(Form("%03dGeV",energy));
    
    map_E_fibres_absorber[energy]  -> Write();
    
    map_E_fibres[energy]           -> Write();
    map_Npe_fibres[energy]         -> Write();
    map_NpeSmear_fibres[energy]    -> Write();
    map_NCerPhot[energy]           -> Write();
    map_NCerPhot_vs_E[energy]      -> Write();
    
    for(unsigned int attLengthIt = 0; attLengthIt < attLengths->size(); ++attLengthIt)
    {
      int attLength = int( attLengths->at(attLengthIt) );
      
      (map_EAtt_fibres[energy])[attLength] -> Write();
      (map_NpeAtt_fibres[energy])[attLength] -> Write();
      (map_NpeSmearAtt_fibres[energy])[attLength] -> Write();
      (map_NpeAtt_fibres_vs_dist[energy])[attLength] -> Write();
      
      (map_NCerPhotAtt_gap[energy])[attLength] -> Write();
      (map_NCerPhotSmearAtt_gap[energy])[attLength] -> Write();
      (map_NCerPhotAtt_gap_vs_NCerPhot[energy])[attLength] -> Write();
      (map_NCerPhotAtt_gap_vs_dist[energy])[attLength] -> Write();
      (map_effPhotAtt_gap_vs_NCerPhot[energy])[attLength] -> Write();
    }
    
    map_E_fibres_vs_dist[energy] -> Write();
    map_E_fibres_vs_initialPosition[energy] -> Write();
    map_NCerPhot_vs_dist[energy] -> Write();
    map_NCerPhot_vs_initialPosition[energy] -> Write();
    
    float en = map_E_fibres[energy] -> GetMean();
    TH1F* h_E_fibres_vs_dist = new TH1F(Form("h_E_fibres_vs_dist_%03dGeV",energy),"",100,en-0.5*en,en+0.5*en);
    for(int bin = 1; bin <= map_E_fibres_vs_dist[energy]->GetNbinsX(); ++bin)
      if( map_E_fibres_vs_dist[energy]->GetBinContent(bin) > 0. ) h_E_fibres_vs_dist -> Fill( map_E_fibres_vs_dist[energy]->GetBinContent(bin) );
    h_E_fibres_vs_dist -> Write();
    
    float num = map_NCerPhot[energy] -> GetMean();
    TH1F* h_NCerPhot_vs_dist = new TH1F(Form("h_NCerPhot_vs_dist_%03dGeV",energy),"",100,num-0.5*num,num+0.5*num);
    for(int bin = 1; bin <= map_NCerPhot_vs_dist[energy]->GetNbinsX(); ++bin)
      if( map_NCerPhot_vs_dist[energy]->GetBinContent(bin) > 0. ) h_NCerPhot_vs_dist -> Fill( map_NCerPhot_vs_dist[energy]->GetBinContent(bin) );
    h_NCerPhot_vs_dist -> Write();
    
    for(unsigned int attLengthIt = 0; attLengthIt < attLengths->size(); ++attLengthIt)
    {
      int attLength = int( attLengths->at(attLengthIt) );

      en = (map_NpeAtt_fibres[energy])[attLength] -> GetMean();
      TH1F* h_NpeAtt_fibres_vs_dist = new TH1F(Form("h_NpeAttLength%04dmm_fibres_vs_dist_%03dGeV",attLength,energy),"",100,en-0.5*en,en+0.5*en);
      for(int bin = 1; bin <= (map_NpeAtt_fibres_vs_dist[energy])[attLength]->GetNbinsX(); ++bin)
        if( (map_NpeAtt_fibres_vs_dist[energy])[attLength]->GetBinContent(bin) > 0. )
          h_NpeAtt_fibres_vs_dist -> Fill( (map_NpeAtt_fibres_vs_dist[energy])[attLength]->GetBinContent(bin) );
      h_NpeAtt_fibres_vs_dist -> Write();
      
      num = (map_NCerPhotAtt_gap[energy])[attLength] -> GetMean();
      TH1F* h_NCerPhotAtt_gap_vs_dist = new TH1F(Form("h_NCerPhotAttLength%04dmm_gap_vs_dist_%03dGeV",attLength,energy),"",100,num-0.5*num,num+0.5*num);
      for(int bin = 1; bin <= (map_NCerPhotAtt_gap_vs_dist[energy])[attLength]->GetNbinsX(); ++bin)
        if( (map_NCerPhotAtt_gap_vs_dist[energy])[attLength]->GetBinContent(bin) > 0. )
          h_NCerPhotAtt_gap_vs_dist -> Fill( (map_NCerPhotAtt_gap_vs_dist[energy])[attLength]->GetBinContent(bin) );
      h_NCerPhotAtt_gap_vs_dist -> Write();
    }
    
    h_r_M[energy] -> Write();
    h_X_0[energy] -> Write();
    
    p_radCont[energy] -> Write();
    p_longCont[energy] -> Write();
    
    outFile -> cd();
  }
  
  outFile -> Close();
}
