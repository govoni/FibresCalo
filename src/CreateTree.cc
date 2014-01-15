#include "CreateTree.hh"



CreateTree* CreateTree::fInstance = NULL ;


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


CreateTree::CreateTree (TString name)
{
  if ( fInstance )
  {
    return ;
  }

  this->fInstance = this ;
  this->fname     = name ;
  this->ftree     = new TTree (name,name) ;
  
  this->GetTree ()->Branch ("Event",           &this->Event,           "Event/I") ;
  this->GetTree ()->Branch ("depositedEnergy", &this->depositedEnergy, "depositedEnergy/F") ;
  this->GetTree ()->Branch ("inputMomentum",   &this->inputMomentum,   "inputMomentum[4]/F") ;
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


void CreateTree::addInputParticle (float ptx, float pty, float ptz, float E)
{
  inputMomentum[0] = ptx ;
  inputMomentum[1] = pty ;
  inputMomentum[2] = ptz ;
  inputMomentum[3] = E ;
  return ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void CreateTree::addEnergy (float energy)
{
  depositedEnergy += energy ;
  return ;
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
}
