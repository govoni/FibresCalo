#include "CreateTree.hh"
#include <algorithm>

using namespace std ;

CreateTree* CreateTree::fInstance = NULL ;


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


CreateTree::CreateTree (TString name,
                        const std::vector<float>& attL)
{
  if ( fInstance )
  {
    return ;
  }

  this->fInstance = this ;
  this->fname     = name ;
  this->ftree     = new TTree (name,name) ;
  
  
  this->attLengths = new std::vector<float>;
  for(unsigned int it = 0; it < attL.size(); ++it)
    this->attLengths->push_back( attL.at(it) );
  
  this->GetTree ()->Branch ("Event", &this->Event, "Event/I") ;
  
  
  inputMomentum = new vector<float> (4, 0.) ; 
  inputInitialPosition = new vector<float> (3, 0.) ; 
  depositedEnergyFibresAtt = new vector<float> ();
  depositedEnergies = new vector<float> () ; 
  depositedEnergiesAtt = new vector<vector<float> > () ; 
  tot_gap_photFast_cer = new vector<float> ();
  tot_det_photFast_cer = new vector<float> ();
  depositFibres = new vector<int> () ; 
  cerenkovPhotons = new vector<int> () ; 
  cerenkovFibres = new vector<int> () ; 
  scintillationPhotons = new vector<int> () ; 
  scintillationFibres = new vector<int> () ; 
  
  this->GetTree ()->Branch ("inputMomentum","vector<float>",&inputMomentum) ;
  this->GetTree ()->Branch ("inputInitialPosition","vector<float>",&inputInitialPosition) ;
  
  this->GetTree ()->Branch ("depositedEnergyTotal",        &this->depositedEnergyTotal,                "depositedEnergyTotal/F") ;
  this->GetTree ()->Branch ("depositedEnergyFibres",       &this->depositedEnergyFibres,              "depositedEnergyFibres/F") ;
  this->GetTree ()->Branch ("depositedEnergyAbsorber",     &this->depositedEnergyAbsorber,          "depositedEnergyAbsorber/F") ;
  this->GetTree ()->Branch ("depositedEnergyFibres_post",  &this->depositedEnergyFibres_post,    "depositedEnergyFibres_post/F") ;
  this->GetTree ()->Branch ("depositedEnergyAbsorber_post",&this->depositedEnergyAbsorber_post,"depositedEnergyAbsorber_post/F") ;
  this->GetTree ()->Branch ("depositedEnergyWorld",        &this->depositedEnergyWorld,       "         depositedEnergyWorld/F") ;
  this->GetTree ()->Branch ("depositedEnergyFibresAtt", "vector<float>",&depositedEnergyFibresAtt);
  
  this->GetTree ()->Branch ("totalTrackLengthFibres",       &this->totalTrackLengthFibres,             "totalTrackLengthFibres/F") ;
  this->GetTree ()->Branch ("totalTrackLengthOverThFibres", &this->totalTrackLengthOverThFibres, "totalTrackLengthOverThFibres/F") ;
  
  this->GetTree ()->Branch ("tot_phot_cer",         &this->tot_phot_cer,                 "tot_phot_cer/I") ;
  this->GetTree ()->Branch ("tot_phot_cer_post",    &this->tot_phot_cer_post,       "tot_phot_cer_post/I") ;
  this->GetTree ()->Branch ("tot_gap_phot_cer",     &this->tot_gap_phot_cer,         "tot_gap_phot_cer/I") ;
  this->GetTree ()->Branch ("tot_det_phot_cer",     &this->tot_det_phot_cer,         "tot_det_phot_cer/I") ;
  this->GetTree ()->Branch ("tot_gap_photFast_cer", "vector<float>",&this->tot_gap_photFast_cer) ;
  this->GetTree ()->Branch ("tot_det_photFast_cer", "vector<float>",&this->tot_det_photFast_cer) ;
  
  this->GetTree ()->Branch ("depositedEnergies","vector<float>",&depositedEnergies) ;
  this->GetTree ()->Branch ("depositedEnergiesAtt","vector<vector<float> >",&depositedEnergiesAtt) ;
  this->GetTree ()->Branch ("depositFibres","vector<int>",&depositFibres) ;
  
  this->GetTree ()->Branch ("cerenkovPhotons","vector<int>",&cerenkovPhotons) ;
  this->GetTree ()->Branch ("cerenkovFibres","vector<int>",&cerenkovFibres) ;
  
  this->GetTree ()->Branch ("scintillationPhotons","vector<int>",&scintillationPhotons) ;
  this->GetTree ()->Branch ("scintillationFibres","vector<int>",&scintillationFibres) ;
  
  this->GetTree ()->Branch ("Radial_stepLength",               &Radial_stepLength,                                     "Radial_stepLength/F");
  this->GetTree ()->Branch ("Longitudinal_stepLength",         &Longitudinal_stepLength,                         "Longitudinal_stepLength/F");
  this->GetTree ()->Branch ("Radial_ion_energy_absorber",       Radial_ion_energy_absorber,             "Radial_ion_energy_absorber[5000]/F");
  this->GetTree ()->Branch ("Longitudinal_ion_energy_absorber", Longitudinal_ion_energy_absorber, "Longitudinal_ion_energy_absorber[5000]/F");
  
  this->GetTree()->Branch("PrimaryParticleX",PrimaryParticleX,"PrimaryParticleX[1000]/F");
  this->GetTree()->Branch("PrimaryParticleY",PrimaryParticleY,"PrimaryParticleY[1000]/F");
  this->GetTree()->Branch("PrimaryParticleZ",PrimaryParticleZ,"PrimaryParticleZ[1000]/F");
  this->GetTree()->Branch("PrimaryParticleE",PrimaryParticleE,"PrimaryParticleE[1000]/F");
  
  
  //h_phot_cer_lambda = new TH1F("h_phot_cer_lambda","",1000,250.,1250.);
  //h_phot_cer_E = new TH1F("h_phot_cer_E","",1000,0.,5.);
  //h_phot_cer_time = new TH1F("h_phot_cer_time","",10000,0.,10000.);
  
  //h_phot_cer_gap_lambda = new TH1F("h_phot_cer_gap_lambda","",1000,250.,1250.);
  //h_phot_cer_gap_E = new TH1F("h_phot_cer_gap_E","",1000,0.,5.);
  //h_phot_cer_gap_time = new TH1F("h_phot_cer_gap_time","",10000,0.,10000.);
  
  //for(unsigned int it = 0; it < this->attLengths.size(); ++it)
  //{
  //  int attLength = int( attLengths.at(it) );
  //  h_photFast_cer_gap_lambda[attLength] = new TH1F(Form("h_photFast_cer_gap_lambda_attLength%04d",attLength),"",1000,250.,1250.);
  //  h_photFast_cer_gap_E[attLength] = new TH1F(Form("h_photFast_cer_gap_E_attLength%04d",attLength),"",1000,0.,5.);
  //  h_photFast_cer_gap_time[attLength] = new TH1F(Form("h_photFast_cer_gap_time_attLength%04d",attLength),"",10000,0.,10000.);
  //}
  
  fibresPosition = new TNtuple ("fibresPosition", "fibresPosition", "N:x:y") ;
  
  
  attenuationLengths = new TTree("attenuationLengths", "attenuationLenghts");
  attenuationLengths -> Branch("attLengths","vector<float>",&attLengths);
  
  
  this->Clear () ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


CreateTree::~CreateTree ()
{}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void
CreateTree::AddEnergyDeposit (int index, float deposit, std::map<int,float>& depositAtt)
{
  // find if it exists already
  vector<int>::const_iterator where = find (depositFibres->begin (), 
                                            depositFibres->end (), index) ;
  
  if (depositFibres->end () == where) 
  {
    depositFibres->push_back (index) ;
    depositedEnergies->push_back (deposit) ;
    int i = 0;
    for(std::map<int,float>::const_iterator it = depositAtt.begin(); it != depositAtt.end(); ++it)
    {
      (depositedEnergiesAtt->at(i)).push_back( (depositAtt[it->first]) ) ;
      ++i;
    }
  }
  else
  {
    depositedEnergies->at (where - depositFibres->begin ()) += deposit ;
    int i = 0;
    for(std::map<int,float>::const_iterator it = depositAtt.begin(); it != depositAtt.end(); ++it)
    {
      (depositedEnergiesAtt->at(i)).at (where - depositFibres->begin ()) += depositAtt[it->first] ;
      ++i;
    }
  }
  return ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void
CreateTree::AddScintillationPhoton (int index)
{
  // find if it exists already
  vector<int>::const_iterator where = find (scintillationFibres->begin (), 
                                            scintillationFibres->end (), index) ;
  if (scintillationFibres->end () == where) 
    {
      scintillationFibres->push_back (index) ;
      scintillationPhotons->push_back (1) ;
    }   
  else
    {
      scintillationPhotons->at (where - scintillationFibres->begin ()) += 1 ;    
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
  attenuationLengths->Write() ;
  //h_phot_cer_lambda->Write();
  //h_phot_cer_E->Write();
  //h_phot_cer_time->Write();
  //h_phot_cer_gap_lambda->Write();
  //h_phot_cer_gap_E->Write();
  //h_phot_cer_gap_time->Write();
  //for(unsigned int it = 0; it < attLengths.size(); ++it)
  //{
  //  int attLength = int( attLengths.at(it) );  
  //  h_photFast_cer_gap_lambda[attLength]->Write();
  //  h_photFast_cer_gap_E[attLength]->Write();
  //  h_photFast_cer_gap_time[attLength]->Write();
  //}
  return true ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void CreateTree::Clear ()
{
  Event	= 0 ;
  
  depositedEnergyTotal = 0. ;
  depositedEnergyFibres = 0. ;
  depositedEnergyAbsorber = 0. ;
  depositedEnergyFibres_post = 0. ;
  depositedEnergyAbsorber_post = 0. ;
  depositedEnergyWorld = 0. ;
  depositedEnergyFibresAtt->clear() ;
  depositedEnergyFibresAtt->resize(attLengths->size(),0);
  
  totalTrackLengthFibres = 0.;
  totalTrackLengthOverThFibres = 0.;
  
  tot_phot_cer = 0;
  tot_phot_cer_post = 0;
  tot_det_phot_cer = 0;
  tot_gap_phot_cer = 0;
  tot_gap_photFast_cer->clear();
  tot_gap_photFast_cer->resize(attLengths->size(),0);
  tot_det_photFast_cer->clear();
  tot_det_photFast_cer->resize(attLengths->size(),0);
  
  for (int i = 0 ; i < 4 ; ++i) 
  {
    inputMomentum->at (i) = 0. ;
  }
  for (int i = 0 ; i < 3 ; ++i) 
  {
    inputInitialPosition->at (i) = 0. ;
  }
  
  depositedEnergies->clear () ;
  depositedEnergiesAtt->clear () ;
  depositedEnergiesAtt->resize(attLengths->size());
  depositFibres->clear () ;
  scintillationPhotons->clear () ;
  scintillationFibres->clear () ;
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
