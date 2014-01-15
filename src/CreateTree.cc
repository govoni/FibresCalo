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
  this->depositedEnergies = new float (fNtowersOnSideSQ) ;
  this->fInstance = this ;
  this->fname     = name ;
  this->ftree     = new TTree (name,name) ;
  
  this->GetTree ()->Branch ("Event",             &this->Event,             "Event/I") ;
  this->GetTree ()->Branch ("depositedEnergy",   &this->depositedEnergy,   "depositedEnergy/F") ;
  this->GetTree ()->Branch ("inputMomentum",     &this->inputMomentum,     "inputMomentum[4]/F") ;
  char text[30] ;
  sprintf (text, "depositedEnergies[%d]/F", fNtowersOnSideSQ) ;
  this->GetTree ()->Branch ("depositedEnergies", &this->depositedEnergies, text) ;
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
      inputMomentum[i] = 0. ;
    }
  for (int i = 0 ; i < fNtowersOnSideSQ ; ++i) 
    {
      depositedEnergies[i] = 0. ;
    }
}
