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
  
  this->GetTree ()->Branch ("Event",                  &this->Event,                                    "Event/I") ;
  this->GetTree ()->Branch ("depositedEnergyFibres",  &this->depositedEnergyFibres,    "depositedEnergyFibres/F") ;
  this->GetTree ()->Branch ("depositedEnergyAbsorber",&this->depositedEnergyAbsorber,"depositedEnergyAbsorber/F") ;
  this->GetTree ()->Branch ("leakageEnergy",          &this->leakageEnergy,                    "leakageEnergy/F") ;
  
  inputMomentum = new vector<float> (4, 0.) ; 
  this->GetTree ()->Branch ("inputMomentum","vector<float>",&inputMomentum) ;
  
  inputInitialPosition = new vector<float> (3, 0.) ; 
  this->GetTree ()->Branch ("inputInitialPosition","vector<float>",&inputInitialPosition) ;
  
  depositedEnergies = new vector<float> () ; 
  this->GetTree ()->Branch ("depositedEnergies","vector<float>",&depositedEnergies) ;
  
  depositFibres = new vector<int> () ; 
  this->GetTree ()->Branch ("depositFibres","vector<int>",&depositFibres) ;
  
  cerenkovPhotons = new vector<int> () ; 
  this->GetTree ()->Branch ("cerenkovPhotons","vector<int>",&cerenkovPhotons) ;
  
  cerenkovFibres = new vector<int> () ; 
  this->GetTree ()->Branch ("cerenkovFibres","vector<int>",&cerenkovFibres) ;
  
  this->GetTree ()->Branch ("Radial_stepLength",               &Radial_stepLength,                                     "Radial_stepLength/F");
  this->GetTree ()->Branch ("Longitudinal_stepLength",         &Longitudinal_stepLength,                         "Longitudinal_stepLength/F");
  this->GetTree ()->Branch ("Radial_ion_energy_absorber",       Radial_ion_energy_absorber,             "Radial_ion_energy_absorber[5000]/F");
  this->GetTree ()->Branch ("Longitudinal_ion_energy_absorber", Longitudinal_ion_energy_absorber, "Longitudinal_ion_energy_absorber[5000]/F");
  
  this->GetTree()->Branch("PrimaryParticleX",PrimaryParticleX,"PrimaryParticleX[1000]/F");
  this->GetTree()->Branch("PrimaryParticleY",PrimaryParticleY,"PrimaryParticleY[1000]/F");
  this->GetTree()->Branch("PrimaryParticleZ",PrimaryParticleZ,"PrimaryParticleZ[1000]/F");
  this->GetTree()->Branch("PrimaryParticleE",PrimaryParticleE,"PrimaryParticleE[1000]/F");
  
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


void
CreateTree::AddCerenkovPhoton (int index)
{
  // find if it exists already
  vector<int>::const_iterator where = find (cerenkovFibres->begin (), 
                                            cerenkovFibres->end (), index) ;
  if (cerenkovFibres->end () == where) 
    {
      cerenkovFibres->push_back (index) ;
      cerenkovPhotons->push_back (1) ;
    }   
  else
    {
      cerenkovPhotons->at (where - cerenkovFibres->begin ()) += 1 ;    
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
  depositedEnergyFibres = 0. ;
  depositedEnergyAbsorber = 0. ;
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
  cerenkovPhotons->clear () ;
  cerenkovFibres->clear () ;
  
  Radial_stepLength = 0.;
  Longitudinal_stepLength = 0.;
  for(int i = 0; i < 5000; ++i)
  {
    Radial_ion_energy_absorber[i] = 0.;
    Longitudinal_ion_energy_absorber[i] = 0.;
  }
  
  for(int i = 0; i < 1000; ++i)
  {
    PrimaryParticleX[i] = 0.;
    PrimaryParticleY[i] = 0.;
    PrimaryParticleZ[i] = 0.;
    PrimaryParticleE[i] = 0.;
  }
}
