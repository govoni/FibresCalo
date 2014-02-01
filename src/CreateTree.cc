#include "CreateTree.hh"
#include <algorithm>

using namespace std ;

CreateTree* CreateTree::fInstance = NULL ;


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


CreateTree::CreateTree (TString name, float tower_side)
{
  if ( fInstance )
  {
    return ;
  }

  this->fInstance = this ;
  this->fname     = name ;
  this->ftree     = new TTree (name,name) ;

  this->GetTree ()->Branch ("Event",             &this->Event,           "Event/I") ;
  this->GetTree ()->Branch ("depositedEnergy",   &this->depositedEnergy, "depositedEnergy/F") ;
  this->GetTree ()->Branch ("leakageEnergy",     &this->leakageEnergy,   "leakageEnergy/F") ;

  inputMomentum = new vector<float> (4, 0.) ; 
  this->GetTree ()->Branch ("inputMomentum",     "vector<float>",   &inputMomentum) ;

  inputInitialPosition = new vector<float> (3, 0.) ; 
  this->GetTree ()->Branch ("inputInitialPosition",     "vector<float>",   &inputInitialPosition) ;

  depositedEnergies = new vector<float> () ; 
  this->GetTree ()->Branch ("depositedEnergies", "vector<float>",   &depositedEnergies) ;

  depositFibres = new vector<int> () ; 
  this->GetTree ()->Branch ("depositFibres", "vector<int>",   &depositFibres) ;
  
  float side = 4 * tower_side ;
  float precision = 0.1 ; // mm
  leakeage = new TH2F ("leakeage", "leakeage", 
                       int (side / precision), -1 * side, side, 
                       int (side / precision), -1 * side, side) ;

  fibresPosition = new TNtuple ("fibresPosition", "fibresPosition", "N:x:y") ;

  this->Clear () ;  
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


CreateTree::~CreateTree ()
{}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void
CreateTree::AddEnergyDeposit (int index, float deposit)
{
  // find if it exists already
  vector<int>::const_iterator where = find (depositFibres->begin (), 
                                            depositFibres->end (), index) ;
  if (depositFibres->end () == where) 
    {
      depositFibres->push_back (index) ;
      depositedEnergies->push_back (deposit) ;
    }   
  else
    {
      depositedEnergies->at (where - depositFibres->begin ()) += deposit ;    
    }
  return ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int CreateTree::Fill () 
{ 
  return this->GetTree ()->Fill () ; 
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


bool CreateTree::Write (TFile * outfile)
{
  outfile->cd () ;
  ftree->Write () ;
  fibresPosition->Write () ;
  leakeage->Write () ;
  return true ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void CreateTree::Clear ()
{
  Event	= 0 ;
  depositedEnergy = 0. ;
  leakageEnergy = 0. ;
  for (int i = 0 ; i < 4 ; ++i) 
    {
      inputMomentum->at (i) = 0. ;
    }
  for (int i = 0 ; i < 3 ; ++i) 
    {
      inputInitialPosition->at (i) = 0. ;
    }
  depositedEnergies->clear () ;
  depositFibres->clear () ;
}
