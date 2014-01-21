    
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
  G4VPhysicalVolume * thePrePV  = thePrePoint->GetPhysicalVolume () ;
  G4VPhysicalVolume * thePostPV = thePostPoint->GetPhysicalVolume () ;
  G4String thePrePVName  = "" ; if ( thePrePV )  thePrePVName  = thePrePV  -> GetName () ;
  G4String thePostPVName = "" ; if ( thePostPV ) thePostPVName = thePostPV -> GetName () ;

  // this is the volume where energy has been deposited
  if (thePostPVName == "FiberCladPV" ||
      thePostPVName == "FiberCoreOutPV" ||
      thePostPVName == "FiberCoreInsPV")
    {
      int index = thePostPoint->GetTouchable ()->GetReplicaNumber (1) 
                   * CreateTree::Instance ()->fNtowersOnSide +
                  thePostPoint->GetTouchable ()->GetReplicaNumber (2) ;

      assert (index < CreateTree::Instance ()->fNtowersOnSideSQ) ;
      assert (index >= 0) ;
      
      // get the deposited energy
      G4double energy = theStep->GetTotalEnergyDeposit () ;
      CreateTree::Instance ()->depositedEnergy += energy/GeV ; 
      CreateTree::Instance ()->depositedEnergies->at (index) += energy/GeV ; 
      CreateTree::Instance ()->totalEnergies->at (index) += energy/GeV ; 
     }  
  if (thePostPVName == "AbsorberPV")
    {
      int index = thePostPoint->GetTouchable ()->GetReplicaNumber (1) 
                   * CreateTree::Instance ()->fNtowersOnSide +
                  thePostPoint->GetTouchable ()->GetReplicaNumber (2) ;

      assert (index < CreateTree::Instance ()->fNtowersOnSideSQ) ;
      assert (index >= 0) ;
      G4double energy = theStep->GetTotalEnergyDeposit () ;
      CreateTree::Instance ()->totalEnergies->at (index) += energy/GeV ; 
    }
  return ;  
}
