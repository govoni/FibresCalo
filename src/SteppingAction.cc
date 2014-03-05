#include "SteppingAction.hh"

#include "G4SteppingManager.hh"
#include "G4SDManager.hh"
#include "G4EventManager.hh"
#include "EventAction.hh"
#include "G4ProcessManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4UnitsTable.hh"
#include "CreateTree.hh"
#include "MyMaterials.hh"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

using namespace std ;


int to_int (string name)
{
  int Result ;             // int which will contain the result
  stringstream convert (name) ;
  string dummy ;           
  convert >> dummy ;       
  convert >> Result ;
  return Result ;
}


//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


SteppingAction::SteppingAction ()
{}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


SteppingAction::~SteppingAction ()
{}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void SteppingAction::UserSteppingAction (const G4Step * theStep)
{
  G4Track* theTrack = theStep->GetTrack () ;
  G4ParticleDefinition* particleType = theTrack->GetDefinition () ;
  
  G4StepPoint * thePrePoint  = theStep->GetPreStepPoint () ;
  G4StepPoint * thePostPoint = theStep->GetPostStepPoint () ;
  const G4ThreeVector & thePrePosition  = thePrePoint->GetPosition () ;
  const G4ThreeVector & thePostPosition = thePostPoint->GetPosition () ;
  G4VPhysicalVolume * thePrePV  = thePrePoint->GetPhysicalVolume () ;
  G4VPhysicalVolume * thePostPV = thePostPoint->GetPhysicalVolume () ;
  G4String thePrePVName  = "" ; if ( thePrePV )  thePrePVName  = thePrePV  -> GetName () ;
  G4String thePostPVName = "" ; if ( thePostPV ) thePostPVName = thePostPV -> GetName () ;
  
  G4int nStep = theTrack -> GetCurrentStepNumber();
  
  
  
  // optical photon
  if( particleType == G4OpticalPhoton::OpticalPhotonDefinition() )
  {
    G4String processName = theTrack->GetCreatorProcess()->GetProcessName();
    
    if( (theTrack->GetLogicalVolumeAtVertex()->GetName().contains("Fiber")) && (nStep == 1) && (processName == "Cerenkov") )
    {
      string fiberName (thePrePVName.data ()) ;
      int index = to_int (fiberName) ;
      CreateTree::Instance ()->AddCerenkovPhoton (index) ;
    }
    
    theTrack->SetTrackStatus(fKillTrackAndSecondaries);
  } // optical photon
  
  
  
  // non optical photon
  else
  {
    G4double energy = theStep->GetTotalEnergyDeposit () - theStep->GetNonIonizingEnergyDeposit();
    
    // FIXME put a zero-suppression threshold
    if ( energy == 0. ) return ;
    
    
    if (thePrePVName.contains ("Fiber"))
    {
      string fiberName (thePrePVName.data ()) ;
      int index = to_int (fiberName) ;
      CreateTree::Instance ()->AddEnergyDeposit (index, energy/GeV) ;
    }
    else if (thePrePVName == "embedderPV")
    {
      CreateTree::Instance ()->leakeage->Fill ( thePrePosition.x (), thePrePosition.y (), energy/GeV) ;
    }  
    
    
    if( thePrePVName == "absorberPV" || thePrePVName.contains("Fiber") )
    {
      G4int iRadius = sqrt( pow(thePrePosition.x()/mm-CreateTree::Instance()->inputInitialPosition->at(0),2) +
                            pow(thePrePosition.y()/mm-CreateTree::Instance()->inputInitialPosition->at(1),2) ) / CreateTree::Instance()->Radial_stepLength;
      if( iRadius < 5000 ) CreateTree::Instance()->Radial_ion_energy_absorber[iRadius] += energy/GeV;
      
      G4int iDepth = (thePrePosition.z()/mm - CreateTree::Instance()->inputInitialPosition->at(2)) / CreateTree::Instance()->Longitudinal_stepLength;
      if( iDepth < 5000 ) CreateTree::Instance()->Longitudinal_ion_energy_absorber[iDepth] += energy/GeV;
    }
    
  } // non optical photon
  
  return ;
}
