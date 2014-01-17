#include "CreateTree.hh"

using namespace std ;

CreateTree* CreateTree::fInstance = NULL ;


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


CreateTree::CreateTree (TString name, int NtowersOnSide)
{
  if ( fInstance )
  {
    return ;
  }

  std::cout << "CONSTRUCTING CreateTree with " << NtowersOnSide << " elements per side\n" ;   
  this->fNtowersOnSide = NtowersOnSide ;
  this->fNtowersOnSideSQ = NtowersOnSide * NtowersOnSide ;
  this->fInstance = this ;
  this->fname     = name ;
  this->ftree     = new TTree (name,name) ;

  this->GetTree ()->Branch ("Event",             &this->Event,           "Event/I") ;
  this->GetTree ()->Branch ("depositedEnergy",   &this->depositedEnergy, "depositedEnergy/D") ;
  inputMomentum = new std::vector<float> (4, 0.) ; 
  this->GetTree ()->Branch ("inputMomentum",     "std::vector<float>",   &inputMomentum) ;
  inputInitialPosition = new std::vector<float> (3, 0.) ; 
  this->GetTree ()->Branch ("inputInitialPosition",     "std::vector<float>",   &inputInitialPosition) ;
  depositedEnergies = new std::vector<float> (fNtowersOnSideSQ, 0.) ; 
  this->GetTree ()->Branch ("depositedEnergies", "std::vector<float>",   &depositedEnergies) ;
  totalEnergies = new std::vector<float> (fNtowersOnSideSQ, 0.) ; 
  this->GetTree ()->Branch ("totalEnergies", "std::vector<float>",   &totalEnergies) ;

  this->Clear () ;  
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


CreateTree::~CreateTree ()
{}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int CreateTree::Fill () 
{ 
  return this->GetTree ()->Fill () ; 
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


bool CreateTree::Write ()
{
  TString filename = this->GetName () ;
  filename+=".root" ;
  TFile* file = new TFile (filename,"RECREATE") ;
  this->GetTree ()->Write () ;
  file->Write () ;
  file->Close () ;
  return true ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void CreateTree::Clear ()
{
  Event	= 0 ;
  depositedEnergy = 0. ;
  for (int i = 0 ; i < 4 ; ++i) 
    {
      inputMomentum->at (i) = 0. ;
    }
  for (int i = 0 ; i < 3 ; ++i) 
    {
      inputInitialPosition->at (i) = 0. ;
    }
  for (int i = 0 ; i < fNtowersOnSideSQ ; ++i) 
    {
      depositedEnergies->at (i) = 0. ;
      totalEnergies->at (i) = 0. ;
    }
}
