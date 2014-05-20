//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes, nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: DetectorConstruction.cc, v 1.18 2010-10-23 19:27:38 gum Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 
//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

#include "DetectorConstruction.hh"
#include "G4MagneticField.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4ClassicalRK4.hh"
#include "G4ExplicitEuler.hh"
#include "G4ChordFinder.hh"
#include "G4EqMagElectricField.hh"
#include "G4PropagatorInField.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SubtractionSolid.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"


#include "DetectorConstruction.hh"
#include <G4TransportationManager.hh>
#include <G4MagneticField.hh>
#include <G4UniformMagField.hh>
#include <G4FieldManager.hh>
#include "CreateTree.hh"
#include <algorithm>
#include <string>
#include <sstream>

using namespace CLHEP;



DetectorConstruction::DetectorConstruction (const string& configFileName)
{
  //---------------------------------------
  //------------- Parameters --------------
  //---------------------------------------
  
  ConfigFile config (configFileName) ;
  
  config.readInto (checkOverlaps, "checkOverlaps") ;
  
  config.readInto (world_material, "world_material") ;
  
  config.readInto (abs_material, "abs_material") ;
  config.readInto (W_fraction, "W_fraction") ;
  config.readInto (hole_radius, "hole_radius") ;
  config.readInto (module_xy, "module_xy") ;
  
  config.readInto (fibre_material, "fibre_material") ;
  config.readInto (fibre_cladRIndex, "fibre_cladRIndex") ;
  config.readInto (fibre_radius, "fibre_radius") ;
  config.readInto (fibre_length, "fibre_length") ;
  config.readInto (fibre_distance, "fibre_distance") ;
  config.readInto (fibre_absLength, "fibre_absLength") ;
  
  config.readInto (gap_l, "gap_l") ;  
  config.readInto (gap_material, "gap_material") ;
  
  config.readInto (det_l, "det_l") ;  
  config.readInto (det_material, "det_material") ;
    
  config.readInto (depth, "depth") ;
  
  B_field_intensity = config.read<double>("B_field_intensity") * tesla ;
  
  margin = max (0.25 * fibre_distance, 2 * fibre_radius) ;
  NfibresAlongY = floor ((module_xy - 2 * margin - 0.5 * fibre_distance) / fibre_distance) + 1 ;
  module_z = fibre_length;

  expHall_x = module_xy * 3 ; 
  expHall_y = module_xy * 3 ; 
  expHall_z = module_z * 3 ;

  B_field_IsInitialized = false ;
  
  initializeMaterials () ;
}


//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


DetectorConstruction::~DetectorConstruction ()
{}


//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


G4VPhysicalVolume* DetectorConstruction::Construct ()
{
  G4cout << ">>>>>> DetectorConstruction::Construct ()::begin <<<<<<" << G4endl ;
  
  
  //------------------------------------
  //------------- Geometry -------------
  //------------------------------------
  
  
  // The experimental Hall
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * worldS = new G4Box ("worldS", 0.5 * expHall_x, 0.5 * expHall_y, 0.5 * expHall_z) ;
  G4LogicalVolume * worldLV = new G4LogicalVolume (worldS, WoMaterial, "worldLV", 0, 0, 0) ;
  G4VPhysicalVolume * worldPV = new G4PVPlacement (0, G4ThreeVector (), worldLV, "World", 0, false, 0, checkOverlaps) ;
  
  
  // The embedding absorber, mimicking the rest of material around
  //   put an embedder four times as the detector in each direction, and fill it with absorber, 
  //   to quickly stop leaking showers and get an idea of where it would leak laterally
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  //G4VSolid * embedderS = new G4Box ("embedder", 0.5 * expHall_x, 0.5 * expHall_y, module_z) ;
  //G4LogicalVolume * embedderLV = new G4LogicalVolume (embedderS, AbMaterial, "embedderLV", 0, 0, 0) ;
  //new G4PVPlacement (0, G4ThreeVector (0., 0., 0.5 * module_z), embedderLV, "embedderPV", worldLV, false, 0, checkOverlaps) ;
  
  
  // The calorimeter
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * calorimeterS = new G4Box ("calorimeterS", 0.5 * module_xy, 0.5 * module_xy, 0.5 * module_z) ;
  G4LogicalVolume * calorimeterLV = new G4LogicalVolume (calorimeterS, WoMaterial, "calorimeterLV") ;  
  new G4PVPlacement (0, G4ThreeVector (0., 0., 0.), calorimeterLV, "calorimeterPV", worldLV, false, 0, checkOverlaps) ;
  
  
  // The absorber
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * absorberS = new G4Box ("absorberS", 0.5 * module_xy, 0.5 * module_xy, 0.5 * module_z) ;
  G4LogicalVolume * absorberLV = new G4LogicalVolume (absorberS, AbMaterial, "absorberLV") ;
  new G4PVPlacement (0, G4ThreeVector (0., 0., 0.), absorberLV, "absorberPV", calorimeterLV, false, 0, checkOverlaps) ;
  
  
  // The holes
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * holeS = new G4Tubs ("holeS", fibre_radius, fibre_radius+hole_radius, 0.5*fibre_length, 0.*deg, 360.*deg) ;    
  G4LogicalVolume * holeLV = new G4LogicalVolume (holeS, WoMaterial, "holeLV") ;  
  
  
  // the fibres
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * fibreS = new G4Tubs ("fibreS", 0., fibre_radius, 0.5*fibre_length, 0.*deg, 360.*deg) ;    
  G4LogicalVolume * fibreLV = new G4LogicalVolume (fibreS, ClMaterial, "fibreLV") ;
  
  
  // triangular-based fibres matrix filling
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  
  int count_x = 0 ; // for the staggering
  float start_x = 0.5 * (module_xy - floor ((module_xy - 2 * margin) / (fibre_distance * 0.8660)) * (fibre_distance * 0.8660)) ;
  float start_y = 0.5 * (module_xy - fibre_distance * (NfibresAlongY - 0.5)) ;
  
  // loop on x direction
  for (float x = -0.5*module_xy+start_x; x <= 0.5*module_xy-margin; x += fibre_distance*0.8660)
  {
    // loop on y direction 
    int count_y = 0 ; // for the staggering
    
    for (float y = - 0.5 * module_xy + start_y; count_y < NfibresAlongY; y += fibre_distance)
    {
      float y_c = y + 0.5 * fibre_distance * (count_x % 2) ; // fibres staggering
      int index = count_x * NfibresAlongY + count_y ;
      CreateTree::Instance ()->fibresPosition->Fill (index, x, y_c);
      
      std::string name = Form("holePV %d",index);
      new G4PVPlacement (0,G4ThreeVector (x, y_c, 0.), holeLV, name, absorberLV, false, 0, checkOverlaps) ;
            
      name = Form("fibrePV %d",index);
      new G4PVPlacement (0,G4ThreeVector (x, y_c, 0.), fibreLV, name, absorberLV, false, 0, checkOverlaps) ;
      
      ++count_y ;
    } // loop on y direction
    
    ++count_x ;   
  } // loop on x direction
  
  
  // fibre gap for photon counting
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * gapLayerS = new G4Box ("gapLayerS", 0.5*module_xy, 0.5*module_xy, 0.5*depth) ;
  G4VSolid * gapS      = new G4Box (     "gapS", 0.5*module_xy, 0.5*module_xy, 0.5*(gap_l-depth)) ;
  G4LogicalVolume * gapLayerLV = new G4LogicalVolume (gapLayerS, GaMaterial, "gapLayerLV") ;
  G4LogicalVolume * gapLV      = new G4LogicalVolume (gapS,      GaMaterial,      "gapLV") ;
  new G4PVPlacement (0, G4ThreeVector (0., 0., 0.5*module_z+0.5*depth),          gapLayerLV, "gapLayerPV", worldLV, false, 0, checkOverlaps) ;
  new G4PVPlacement (0, G4ThreeVector (0., 0., 0.5*module_z+depth+0.5*(gap_l-depth)), gapLV,      "gapPV", worldLV, false, 0, checkOverlaps) ;
  
  
  // Si detector for photon counting
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * detLayerS = new G4Box ("detLayerS", 0.5*module_xy, 0.5*module_xy, 0.5*depth) ;
  G4VSolid * detS      = new G4Box (     "detS", 0.5*module_xy, 0.5*module_xy, 0.5*(det_l-depth)) ;
  G4LogicalVolume * detLayerLV = new G4LogicalVolume (detLayerS, DeMaterial, "detLayerLV") ;
  G4LogicalVolume * detLV      = new G4LogicalVolume (detS,      DeMaterial,      "detLV") ;
  new G4PVPlacement (0, G4ThreeVector (0., 0., 0.5*module_z+gap_l+0.5*depth),          detLayerLV, "detLayerPV", worldLV, false, 0, checkOverlaps) ;
  new G4PVPlacement (0, G4ThreeVector (0., 0., 0.5*module_z+gap_l+depth+0.5*(det_l-depth)), detLV,      "detPV", worldLV, false, 0, checkOverlaps) ;
  
  
  
  //-----------------------------------------------------
  //------------- Visualization attributes --------------
  //-----------------------------------------------------
  
  G4Colour  white   (1.00, 1.00, 1.00) ;  // white
  G4Colour  grey    (0.50, 0.50, 0.50) ;  // grey
  G4Colour  black   (0.00, 0.00, 0.00) ;  // black
  G4Colour  red     (1.00, 0.00, 0.00) ;  // red
  G4Colour  green   (0.00, 1.00, 0.00) ;  // green
  G4Colour  blue    (0.00, 0.00, 1.00) ;  // blue
  G4Colour  cyan    (0.00, 1.00, 1.00) ;  // cyan
  G4Colour  air     (0.90, 0.94, 1.00) ;  // cyan
  G4Colour  magenta (1.00, 0.00, 1.00) ;  // magenta 
  G4Colour  yellow  (1.00, 1.00, 0.00) ;  // yellow
  G4Colour  brass   (0.80, 0.60, 0.40) ;  // brass
  G4Colour  brown   (0.70, 0.40, 0.10) ;  // brown
  
  G4VisAttributes* VisAttWorld = new G4VisAttributes (black) ;
  VisAttWorld->SetVisibility (true) ;
  VisAttWorld->SetForceWireframe (true) ;
  worldLV->SetVisAttributes (VisAttWorld) ;
  
  //G4VisAttributes* VisAttEmbedder = new G4VisAttributes (red) ;
  //VisAttEmbedder->SetVisibility (true) ;
  //VisAttEmbedder->SetForceWireframe (true) ;
  //embedderLV->SetVisAttributes (VisAttEmbedder) ;
  
  G4VisAttributes* VisAttCalorimeter = new G4VisAttributes (yellow) ;
  VisAttCalorimeter->SetVisibility (true) ;
  VisAttCalorimeter->SetForceWireframe (true) ;
  calorimeterLV->SetVisAttributes (VisAttCalorimeter) ;
    
  G4VisAttributes* VisAttAbsorber = new G4VisAttributes (brass) ;
  VisAttAbsorber->SetVisibility (true) ;
  VisAttAbsorber->SetForceWireframe (true) ;
  absorberLV->SetVisAttributes (VisAttAbsorber) ;
  
  G4VisAttributes* VisAttHole = new G4VisAttributes(air);
  VisAttHole->SetVisibility(true);
  VisAttHole->SetForceWireframe(true);
  holeLV->SetVisAttributes(VisAttHole);
    
  G4VisAttributes* VisAttfibre = new G4VisAttributes (green) ;
  VisAttfibre->SetVisibility (true) ;
  VisAttfibre->SetForceWireframe (false) ;
  fibreLV->SetVisAttributes (VisAttfibre) ;  
  
  G4VisAttributes* VisAttGapLayer = new G4VisAttributes(red);
  VisAttGapLayer->SetVisibility(true);
  VisAttGapLayer->SetForceWireframe(false);
  gapLayerLV->SetVisAttributes(VisAttGapLayer);
  
  G4VisAttributes* VisAttGap = new G4VisAttributes(blue);
  VisAttGap->SetVisibility(true);
  VisAttGap->SetForceWireframe(false);
  gapLV->SetVisAttributes(VisAttGap);
  
  G4VisAttributes* VisAttDetLayer = new G4VisAttributes(red);
  VisAttDetLayer->SetVisibility(true);
  VisAttDetLayer->SetForceWireframe(false);
  detLayerLV->SetVisAttributes(VisAttDetLayer);
  
  G4VisAttributes* VisAttDet = new G4VisAttributes(gray);
  VisAttDet->SetVisibility(true);
  VisAttDet->SetForceWireframe(false);
  detLV->SetVisAttributes(VisAttDet);
    
  //PG call the magnetic field initialisation
  if (B_field_intensity > 0.1 * tesla) ConstructField () ; 
  
  
  
  //-----------------------------------------------
  //------------- Fast photon timing --------------
  //-----------------------------------------------
  
  std::vector<std::pair<double,double> > rIndVecCore;
  std::vector<std::pair<double,double> > rIndVecClad;
  std::vector<std::pair<double,double> > rIndVecAir;
  std::vector<std::pair<double,double> > rIndVecGap;
  
  G4MaterialPropertyVector* mpVec;
  
  mpVec = ClMaterial->GetMaterialPropertiesTable()->GetProperty("RINDEX");
  for(unsigned int it = 0; it < mpVec->GetVectorLength(); ++it)
  {
    std::pair<double,double> dummy(mpVec->GetLowEdgeEnergy(it)/eV,(*mpVec)[it]);
    rIndVecCore.push_back(dummy);
  }
  
  mpVec = WoMaterial->GetMaterialPropertiesTable()->GetProperty("RINDEX");
  for(unsigned int it = 0; it < mpVec->GetVectorLength(); ++it)
  {
    std::pair<double,double> dummy(mpVec->GetLowEdgeEnergy(it)/eV,(*mpVec)[it]);
    std::pair<double,double> dummy2(mpVec->GetLowEdgeEnergy(it)/eV,fibre_cladRIndex);
    rIndVecAir.push_back(dummy);
    rIndVecClad.push_back(dummy2);
  }
  
  mpVec = GaMaterial->GetMaterialPropertiesTable()->GetProperty("RINDEX");
  for(unsigned int it = 0; it < mpVec->GetVectorLength(); ++it)
  {
    std::pair<double,double> dummy(mpVec->GetLowEdgeEnergy(it)/eV,(*mpVec)[it]);
    rIndVecGap.push_back(dummy);
  }
  
  
  fib = FiberInit(fibre_length,fibre_radius,CreateTree::Instance()->attLengths,rIndVecCore,rIndVecClad,rIndVecAir,rIndVecGap) ;
  
  
  
  G4cout << ">>>>>> DetectorConstruction::Construct ()::end <<< " << G4endl ;
  return worldPV ;
}


//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


void DetectorConstruction::initializeMaterials ()
{
  // define materials
  
  
  WoMaterial = NULL ;
  if      ( world_material == 1 ) WoMaterial = MyMaterials::Air () ;
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid fibre world material specifier " << world_material << G4endl ;
    exit (-1) ;
  }
  G4cout << "Wo. material: "<< WoMaterial << G4endl ;
  
  
  AbMaterial = NULL ;
  if      ( abs_material == 1 ) AbMaterial = MyMaterials::Brass () ;
  else if ( abs_material == 2 ) AbMaterial = MyMaterials::Tungsten () ;
  else if ( abs_material == 3 ) AbMaterial = MyMaterials::Lead () ;
  else if ( abs_material == 4 ) AbMaterial = MyMaterials::Iron () ;
  else if ( abs_material == 5 ) AbMaterial = MyMaterials::Aluminium () ;
  else if ( abs_material == 6 ) AbMaterial = MyMaterials::CopperTungstenAlloy(W_fraction) ;
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid absorber material specifier " << abs_material << G4endl ;
    exit (-1) ;
  }
  G4cout << "Ab. material: "<< AbMaterial << G4endl ;
  
  
  ClMaterial = NULL ;
  if      ( fibre_material == 1 ) ClMaterial = MyMaterials::Quartz () ;
  else if ( fibre_material == 2 ) ClMaterial = MyMaterials::SiO2_Ce () ;
  else if ( fibre_material == 3 ) ClMaterial = MyMaterials::DSB_Ce () ;
  else if ( fibre_material == 4 ) ClMaterial = MyMaterials::LuAG_Ce () ;
  else if ( fibre_material == 5 ) ClMaterial = MyMaterials::YAG_Ce () ;
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid fibre clad material specifier " << fibre_material << G4endl ;
    exit (-1) ;
  }
  G4cout << "Cl. material: "<< ClMaterial << G4endl ;
  
  
  GaMaterial = NULL;
  if     ( gap_material == 1 ) GaMaterial = MyMaterials::Air();
  else if( gap_material == 2 ) GaMaterial = MyMaterials::OpticalGrease();
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid gap material specifier " << gap_material << G4endl;
    exit(-1);
  }
  G4cout << "Gap material: " << gap_material << G4endl;
  
  
  DeMaterial = NULL;
  if( det_material == 1 ) DeMaterial = MyMaterials::Silicon();
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid detector material specifier " << det_material << G4endl;
    exit(-1);
  }
  G4cout << "Detector material: " << det_material << G4endl;
  
  
  
  if( fibre_absLength >= 0 )
  {
    const G4int nEntries_ABS = 2;
    G4double PhotonEnergy_ABS[nEntries_ABS] = { 1.*eV, 10.*eV };
    G4double Absorption[nEntries_ABS] = { fibre_absLength*mm, fibre_absLength*mm };
    
    ClMaterial->GetMaterialPropertiesTable()->RemoveProperty("ABSLENGTH");
    ClMaterial->GetMaterialPropertiesTable()->AddProperty("ABSLENGTH",PhotonEnergy_ABS,Absorption,nEntries_ABS);
  }
  
}


//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


void DetectorConstruction::ConstructField () 
{
  G4cout << ">>>>>> DetectorConstruction::ConstructField ()::begin <<<<<<" << G4endl ;
  static G4TransportationManager * trMgr = G4TransportationManager::GetTransportationManager () ; 
  
  // A field object is held by a field manager
  // Find the global Field Manager
  G4FieldManager * globalFieldMgr = trMgr->GetFieldManager () ;
  
  if (!B_field_IsInitialized)
  {
    // magnetic field parallel to the beam direction (w/ tilt)
    G4ThreeVector fieldVector (
      0.0522 * B_field_intensity, 
      0.0522 * B_field_intensity, 
      0.9973 * B_field_intensity
      ) ;   
    
    B_field = new G4UniformMagField (fieldVector) ; 
    globalFieldMgr->SetDetectorField (B_field) ;
    globalFieldMgr->CreateChordFinder (B_field) ;
    globalFieldMgr->GetChordFinder ()->SetDeltaChord (0.005 * mm) ;
    B_field_IsInitialized = true ;
  }
  G4cout << ">>>>>> DetectorConstruction::ConstructField ()::end <<< " << G4endl ;
  return ;
}
