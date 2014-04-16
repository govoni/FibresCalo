// g++ -Wall -o studyUniformity `root-config --cflags --glibs` dict.o setTDRStyle.cc studyUniformity.cpp

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
#include "TCanvas.h"
#include "TProfile2D.h"
#include "TGraphErrors.h"
#include "TEllipse.h"



int absMat = 1;
int fibMat = 4;
float fibRad = 0.4;
float fibDist = 1.2;

std::string stageoutFolder = "/store/user/abenagli/FibresCalo/March6_3degTilt/";
//std::string stageoutFolder = "/store/user/govoni/upgrade/FibresCalo/March6_3degTilt/";



int main(int argc, char** argv)
{
  std::string particle = "e-";
  if( argc > 1 ) particle = std::string(argv[1]);

  if( argc > 2 ) absMat  = atoi(argv[2]);
  if( argc > 3 ) fibMat  = atoi(argv[3]);
  if( argc > 4 ) fibRad  = atof(argv[4]);
  if( argc > 5 ) fibDist = atof(argv[5]);

  std::string label = Form("absMat%d_fibMat%d_fibRad%.1f_fibDist%.1f",absMat,fibMat,fibRad,fibDist);
  
  setTDRStyle();
  
  std::vector<int> energies;
  energies.push_back(1);
  energies.push_back(10);
  energies.push_back(30);
  energies.push_back(50);
  energies.push_back(100);
  energies.push_back(300);
  
  
  int nBinsX = 20;
  float xMin = -1.3*fibDist;
  float xMax = +1.3*fibDist;
  float binWidthX = (xMax-xMin) / nBinsX;
  
  int nBinsY = 20;
  float yMin = -1.*fibDist;
  float yMax = +1.*fibDist;
  float binWidthY = (yMax-yMin) / nBinsY;
  
  
  //----------------------
  // define histogram maps
  std::map<int,TProfile2D*> p_E_fibers_Y_vs_X;
  
  std::map<int,std::vector<TH1F*> > map_E_fibers_vsX;
  std::map<int,std::vector<TH1F*> > map_E_fibers_vsY;
  
  std::vector<TEllipse*> fibres;
  
  
  //---------------
  // loop on energies  
  for(unsigned int energyIt = 0; energyIt < energies.size(); ++energyIt)
  {
    int energy = energies.at(energyIt);
    
    
    //-----------
    // open files
    TChain* tree = new TChain("tree","tree");
    tree -> Add( Form("~/eos/cms/%s/%s/run_%s_%dGeV/out*.root",stageoutFolder.c_str(),label.c_str(),particle.c_str(),energy) );
    std::cout << "reading file "
              << Form("~/eos/cms/%s/%s/run_%s_%dGeV/out*.root",stageoutFolder.c_str(),label.c_str(),particle.c_str(),energy)
              << std::endl;
    std::cout << ">>> energy: " << energy << "   nEvents: " << tree->GetEntries() << std::endl;
    
    TChain* fibresPosition = new TChain("fibresPosition","fibresPosition");
    fibresPosition -> Add( Form("~/eos/cms/%s/%s/run_%s_%dGeV/out_0.root",stageoutFolder.c_str(),label.c_str(),particle.c_str(),energy) );
    
    
    //----------------
    // define branches
    float InitialPositionX;
    float InitialPositionY;
    
    std::vector<float>* inputInitialPosition = new std::vector<float>;
    float depositedEnergyFibres;
    
    tree -> SetBranchStatus("*",0);
    
    tree->SetBranchStatus("inputInitialPosition", 1); tree->SetBranchAddress("inputInitialPosition", &inputInitialPosition);
    tree->SetBranchStatus("depositedEnergyFibres",1); tree->SetBranchAddress("depositedEnergyFibres",&depositedEnergyFibres);
    
    int fibN;
    float fibX, fibY;
    
    fibresPosition -> SetBranchAddress("N",&fibN);
    fibresPosition -> SetBranchAddress("x",&fibX);
    fibresPosition -> SetBranchAddress("y",&fibY);
    
    
    //------------------
    // define histograms
    
    p_E_fibers_Y_vs_X[energy] = new TProfile2D(Form("p_E_fibers_Y_vs_X_%dGeV",energy),"",nBinsX,xMin,xMax,nBinsY,yMin,yMax);
    
    std::vector<TH1F*> dummyX;
    std::vector<TH1F*> dummyY;
    for(int bin = 0; bin < nBinsX; ++bin)
    {
      dummyX.push_back( new TH1F(Form("h_E_fibers_vsX_xBin%d_%02dGeV",bin,energy),"",1000,0.,energy) );
      dummyX.at(bin) -> Sumw2();
      
      dummyY.push_back( new TH1F(Form("h_E_fibers_vsY_xBin%d_%02dGeV",bin,energy),"",1000,0.,energy) );
      dummyY.at(bin) -> Sumw2();
    }
    map_E_fibers_vsX[energy] = dummyX;
    map_E_fibers_vsY[energy] = dummyY;
    
    
    //---------------
    // loop on events
    //int entryMax = 1000;
    int entryMax = tree->GetEntries();
    for(int entry = 0; entry < entryMax; ++entry)
    {
      tree -> GetEntry(entry);
      
      if( entry%1 == 0 ) std::cout << ">>>>>> entry: " << entry << "\r" << std::flush;
      
      
      if( energyIt == 0 && entry == 0 )
      {
        for(int fibIt = 0; fibIt < fibresPosition->GetEntries(); ++fibIt)
        {
          fibresPosition -> GetEntry(fibIt);
          if( fibX >= xMin && fibX <= xMax && fibY >= yMin && fibY <= yMax)
          {
            TEllipse* fibre = new TEllipse(fibX,fibY,fibRad);
            fibre -> SetFillColor(0);
            fibre -> SetFillStyle(0);
            fibres.push_back( fibre );
          }
        }
      }
      
      
      InitialPositionX = inputInitialPosition -> at(0);
      InitialPositionY = inputInitialPosition -> at(1);
      
      
      int binX = (InitialPositionX-xMin) / binWidthX;
      int binY = (InitialPositionY-yMin) / binWidthY;
      
      
      //----------------
      // fill histograms
      
      p_E_fibers_Y_vs_X[energy] -> Fill(InitialPositionX,InitialPositionY,depositedEnergyFibres);
      (map_E_fibers_vsX[energy]).at(binX) -> Fill( depositedEnergyFibres );
      (map_E_fibers_vsY[energy]).at(binY) -> Fill( depositedEnergyFibres );
    }
  }
  
  
  //---------
  // drawings
  TFile* outFile = TFile::Open(Form("studyUniformity_%s_%s.root",label.c_str(),particle.c_str()),"RECREATE");
  outFile -> cd();
  
  for(unsigned int energyIt = 0; energyIt < energies.size(); ++energyIt)  
  {
    int energy = energies.at(energyIt);
    
    outFile -> mkdir(Form("%dGeV",energy));
    outFile -> cd(Form("%dGeV",energy));
    
    p_E_fibers_Y_vs_X[energy] -> Write(Form("p_E_fibers_Y_vs_X_%dGeV",energy));
    
    TGraphErrors* g_vsX = new TGraphErrors();
    TGraphErrors* g_vsY = new TGraphErrors();
    
    for(int bin = 0; bin < nBinsX; ++bin)
    {
      g_vsX -> SetPoint(bin,xMin+bin*binWidthX,(map_E_fibers_vsX[energy]).at(bin)->GetMean());
      g_vsX -> SetPointError(bin,0.,(map_E_fibers_vsX[energy]).at(bin)->GetMeanError());
      
      g_vsY -> SetPoint(bin,yMin+bin*binWidthY,(map_E_fibers_vsY[energy]).at(bin)->GetMean());
      g_vsY -> SetPointError(bin,0.,(map_E_fibers_vsY[energy]).at(bin)->GetMeanError());
      
      //(map_E_fibers_vsX[energy]).at(bin) -> Write();
      //(map_E_fibers_vsY[energy]).at(bin) -> Write();
    }
    
    g_vsX -> Write(Form("g_E_fibers_vsX_%dGeV",energy));
    g_vsY -> Write(Form("g_E_fibers_vsY_%dGeV",energy));
    
    outFile -> cd("");
  }
  
  for(unsigned int fibIt = 0; fibIt < fibres.size(); ++fibIt)
    (fibres.at(fibIt)) -> Write( Form("fibre_%d",fibIt) );
  
  outFile -> Close();
}
