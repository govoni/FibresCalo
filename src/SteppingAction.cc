#include "SteppingAction.hh"

using namespace std;
using namespace CLHEP;



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


SteppingAction::SteppingAction (DetectorConstruction* detectorConstruction,
                                const G4int& scint, const G4int& cher) :
  fDetectorConstruction(detectorConstruction),
  propagateScintillation(scint),
  propagateCerenkov(cher)
{}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


SteppingAction::~SteppingAction ()
{}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void SteppingAction::UserSteppingAction (const G4Step * theStep)
{
  G4Track* theTrack = theStep->GetTrack () ;
  G4int trackID = theTrack->GetTrackID();
  TrackInformation* theTrackInfo = (TrackInformation*)(theTrack->GetUserInformation());
  G4ParticleDefinition* particleType = theTrack->GetDefinition () ;
  
  G4StepPoint * thePrePoint  = theStep->GetPreStepPoint () ;
  G4StepPoint * thePostPoint = theStep->GetPostStepPoint () ;
  const G4ThreeVector & thePrePosition  = thePrePoint->GetPosition () ;
  G4VPhysicalVolume * thePrePV  = thePrePoint->GetPhysicalVolume () ;
  G4VPhysicalVolume * thePostPV = thePostPoint->GetPhysicalVolume () ;
  G4String thePrePVName  = "" ; if ( thePrePV )  thePrePVName  = thePrePV  -> GetName () ;
  G4String thePostPVName = "" ; if ( thePostPV ) thePostPVName = thePostPV -> GetName () ;
  
  G4int nStep = theTrack -> GetCurrentStepNumber();
  
  
  
  //-------------------
  // get local position
  G4double global_x = thePrePosition.x()/mm;
  G4double global_y = thePrePosition.y()/mm;
  G4double global_z = thePrePosition.z()/mm;
  
  G4double module_z = fDetectorConstruction->GetModule_z();
  
  G4double local_x = -999999.;
  G4double local_y = -999999.;
  G4double local_z = global_z + 0.5*module_z;
  
  if( thePrePVName.contains("fibre") )
  {
    std::string fibreName( thePrePVName.data() );
    int index = to_int( fibreName );
    
    float N,x_c,y_c;
    CreateTree::Instance()->fibresPosition->SetBranchAddress("N",&N);
    CreateTree::Instance()->fibresPosition->SetBranchAddress("x",&x_c);
    CreateTree::Instance()->fibresPosition->SetBranchAddress("y",&y_c);
    CreateTree::Instance()->fibresPosition->GetEntry(index);
    
    local_x = global_x - x_c;
    local_y = global_y - y_c;
  }
  
  
  
  //-----------------
  // primary particle
  if( trackID == 1 )
  {
    if( nStep-1 < 1000 )
    {
      CreateTree::Instance()->PrimaryParticleX[nStep-1] = thePrePosition.x()/mm;
      CreateTree::Instance()->PrimaryParticleY[nStep-1] = thePrePosition.y()/mm;
      CreateTree::Instance()->PrimaryParticleZ[nStep-1] = thePrePosition.z()/mm;
      CreateTree::Instance()->PrimaryParticleE[nStep-1] = thePrePoint->GetTotalEnergy()/GeV;
    }
  }
  
  
  
  // optical photon
  if( particleType == G4OpticalPhoton::OpticalPhotonDefinition() )
  {
    G4String processName = theTrack->GetCreatorProcess()->GetProcessName();
    
    if( (theTrack->GetLogicalVolumeAtVertex()->GetName().contains("fibre")) && (nStep == 1) && (processName == "Scintillation") )
    {
      string fibreName (thePrePVName.data ()) ;
      int index = to_int (fibreName) ;
      CreateTree::Instance ()->AddScintillationPhoton (index) ;
      
      if( !propagateScintillation ) theTrack->SetTrackStatus(fKillTrackAndSecondaries);      
    }
        
    if( (theTrack->GetLogicalVolumeAtVertex()->GetName().contains("fibre")) && (nStep == 1) && (processName == "Cerenkov") )
    {
      CreateTree::Instance()->tot_phot_cer += 1;
      
      string fibreName (thePrePVName.data ()) ;
      int index = to_int (fibreName) ;
      CreateTree::Instance ()->AddCerenkovPhoton (index) ;
      
      CreateTree::Instance()->h_phot_cer_lambda -> Fill( MyMaterials::fromEvToNm(theTrack->GetTotalEnergy()/eV) );
      CreateTree::Instance()->h_phot_cer_E      -> Fill( theTrack->GetTotalEnergy()/eV );
      CreateTree::Instance()->h_phot_cer_time   -> Fill( thePrePoint->GetGlobalTime()/picosecond );
      
      if( !propagateCerenkov ) theTrack->SetTrackStatus(fKillTrackAndSecondaries);      
    }
    
    if( (theTrack->GetLogicalVolumeAtVertex()->GetName().contains("fibre")) && (processName == "Cerenkov") &&
        (thePrePVName == "gapLayerPV") && (thePostPVName == "gapPV") )
    {
      CreateTree::Instance()->tot_gap_phot_cer += 1;
      
      CreateTree::Instance()->h_phot_cer_gap_lambda -> Fill( MyMaterials::fromEvToNm(theTrack->GetTotalEnergy()/eV) );
      CreateTree::Instance()->h_phot_cer_gap_E      -> Fill( theTrack->GetTotalEnergy()/eV );
      CreateTree::Instance()->h_phot_cer_gap_time   -> Fill( thePrePoint->GetGlobalTime()/picosecond );
      
      theTrack->SetTrackStatus(fKillTrackAndSecondaries);
    }
    
    if( (theTrack->GetLogicalVolumeAtVertex()->GetName().contains("fibre")) && (processName == "Cerenkov") &&
        (thePrePVName == "detLayerPV") && (thePostPVName == "detPV") )
    {
      CreateTree::Instance()->tot_det_phot_cer += 1;
    }
    
    if( (theTrack->GetLogicalVolumeAtVertex()->GetName().contains("fibre")) && (nStep == 1) )
    {    
      //----------------------------------------------------------
      // storing time, energy and position at gap with fast timing
      Photon ph;
      ph.position.SetX(local_x);
      ph.position.SetY(local_y);
      ph.position.SetZ(local_z);
      ph.direction.SetX(theTrack->GetVertexMomentumDirection().x());
      ph.direction.SetY(theTrack->GetVertexMomentumDirection().y());
      ph.direction.SetZ(theTrack->GetVertexMomentumDirection().z());
      ph.dist = (local_z/module_z);
      ph.energy = theTrack->GetTotalEnergy()/eV;
      
      Fiber fib = fDetectorConstruction -> GetFiber();
      Travel trc = GetTimeAndProbability(ph,&fib,theTrackInfo->GetParticleProdTime());
      
      if( trc.prob[0] < 1.E-09 ) theTrack->SetTrackStatus(fKillTrackAndSecondaries);      
    }
  } // optical photon
  
  
  
  // non optical photon
  else
  {
    G4double energy = theStep->GetTotalEnergyDeposit () - theStep->GetNonIonizingEnergyDeposit();
    CreateTree::Instance ()->depositedEnergyTotal += energy/GeV;
    
    
    // FIXME put a zero-suppression threshold
    if ( energy == 0. ) return ;
    
    
    if (thePrePVName.contains ("fibre"))
    {
      CreateTree::Instance ()->depositedEnergyFibres += energy/GeV;
      
      std::map<int,float> depAtt;
      for(unsigned int it = 0; it < CreateTree::Instance()->attLengths.size(); ++it)
      {
        float attLength = CreateTree::Instance()->attLengths.at(it);
        
        CreateTree::Instance()->depositedEnergyFibresAtt->at(it) += energy/GeV*exp(-1.*(module_z-local_z)/attLength);
        depAtt[attLength] = ( energy/GeV*exp(-1.*(module_z-local_z)/attLength) );
      }
      
      string fibreName (thePrePVName.data ()) ;
      int index = to_int (fibreName) ;
      CreateTree::Instance ()->AddEnergyDeposit (index, energy/GeV, depAtt);
      
    }
    else if (thePrePVName == "absorberPV" || thePrePVName.contains("hole"))
    {
      CreateTree::Instance ()->depositedEnergyAbsorber += energy/GeV;
    }
    else if (thePrePVName == "sideshowerPV")
    {
      CreateTree::Instance ()->depositedEnergySide += energy/GeV;
    }
    else if (thePrePVName == "postshowerPV")
    {
      CreateTree::Instance ()->depositedEnergyPost += energy/GeV;
    }
    else if (thePrePVName == "World")
    {
      CreateTree::Instance ()->depositedEnergyWorld += energy/GeV;
    }
    
    
    if( thePrePVName == "absorberPV" || thePrePVName.contains("fibre") || thePrePVName.contains("fibre") )
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
