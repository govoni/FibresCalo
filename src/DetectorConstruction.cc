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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"



DetectorConstruction::DetectorConstruction (const string& configFileName)
{
  readConfigFile (configFileName) ;
  
  
  
  //---------------------------------------
  //------------- Parameters --------------
  //---------------------------------------
  
  initializeMaterials () ;
  
  expHall_x = expHall_y = expHall_z = 1*m ;
  
  module_x = module_xy ;
  module_y = module_xy ;
  
  fiber_length = module_z ;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction ()
{}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct ()
{
  G4cout << ">>>>>> DetectorConstruction::Construct ()::begin <<<<<<" << G4endl ;
  
  //------------------------------------
  //------------- Geometry -------------
  //------------------------------------
  
  // The experimental Hall
  // ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * worldS = new G4Box ("World", 0.5*expHall_x, 0.5*expHall_y, 0.5*expHall_z) ;
  G4LogicalVolume * worldLV = new G4LogicalVolume (worldS, MyMaterials::Air (), "World", 0, 0, 0) ;
  G4VPhysicalVolume * worldPV = new G4PVPlacement (0, G4ThreeVector (), worldLV, "World", 0, false, 0, true) ;

  // The block of material after the calorimeter
  // ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * postShowerS = new G4Box ("postShowerS", 0.5 * NtowersOnSide * module_xy, 0.5 * NtowersOnSide * module_xy, 0.5 * module_z) ;
  G4LogicalVolume * postShowerLV = new G4LogicalVolume (postShowerS, AbMaterial, "postShowerLV") ;
  G4VPhysicalVolume * postShowerPV = new G4PVPlacement (0, G4ThreeVector (0., 0., module_z), postShowerLV, "postShowerPV", worldLV, false, 0, true) ;
    
  // The calorimeter
  // ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * calorS = new G4Box ("CalorimeterS", 0.5 * NtowersOnSide * module_xy, 0.5 * NtowersOnSide * module_xy, 0.5 * module_z) ;
  G4LogicalVolume * calorLV = new G4LogicalVolume (calorS, MyMaterials::Air (), "CalorimeterLV") ;
  G4VPhysicalVolume * calorPV = new G4PVPlacement (0, G4ThreeVector (), calorLV, "CalorimeterPV", worldLV, false, 0, true) ;

  // row of towers, that gets replicated to fill a row
  // ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * rowS = new G4Box ("rowS", 0.5 * NtowersOnSide * module_xy, 0.5 * module_xy, 0.5 * module_z) ;
  G4LogicalVolume * rowLV = new G4LogicalVolume (rowS, MyMaterials::Air (), "rowLV") ;
  G4VPhysicalVolume * matrixPV = new G4PVReplica ("rowPV", rowLV, calorLV, kYAxis, NtowersOnSide, module_xy) ;
  
  // single tower, that gets replicated to fill a row
  // ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * towerS = new G4Box ("towerS", 0.5 * module_xy, 0.5 * module_xy, 0.5 * module_z) ;
  G4LogicalVolume * towerLV = new G4LogicalVolume (towerS, MyMaterials::Air (), "towerLV") ;
  G4VPhysicalVolume * rowPV = new G4PVReplica ("towerPV", towerLV, rowLV, kXAxis, NtowersOnSide, module_xy) ;
   
  // the absorber
  G4VSolid * holeS                   = new G4Tubs ("holeS", 0., fiberClad_radius, 0.5 * fiber_length, 0.*deg, 360.*deg) ;
  G4VSolid * absorberS               = new G4SubtractionSolid ("absorberS", towerS, holeS) ;
  G4LogicalVolume   * absoLV         = new G4LogicalVolume (absorberS, AbMaterial, "AbsorberLV") ;
  G4VPhysicalVolume * fAbsorberPV    = new G4PVPlacement (0, G4ThreeVector (0., 0., 0.), absoLV, "AbsorberPV", towerLV, false, 0, true) ;

  // the fibres
  G4VSolid * fiberCladS              = new G4Tubs ("FiberCladS", fiberCore_radius, fiberClad_radius, 0.5*fiber_length, 0.*deg, 360.*deg) ;    
  G4LogicalVolume * fiberCladLV      = new G4LogicalVolume (fiberCladS   , ClMaterial, "FiberCladLV") ;    
  G4VPhysicalVolume * fiberCladPV    = new G4PVPlacement (0, G4ThreeVector (0., 0., 0.), fiberCladLV, "FiberCladPV", towerLV, false, 0, true) ;

  G4VSolid * fiberCoreOutS           = new G4Tubs ("FiberCoreOutS", fiberCore_radius * 0.9999, fiberCore_radius, 0.5*fiber_length, 0.*deg, 360.*deg) ;
  G4LogicalVolume * fiberCoreOutLV   = new G4LogicalVolume (fiberCoreOutS, CoMaterial, "FiberCoreOutLV") ;
  G4VPhysicalVolume * fiberCoreOutPV = new G4PVPlacement (0, G4ThreeVector (0., 0., 0.), fiberCoreOutLV, "FiberCoreOutPV", towerLV, false, 0, true) ;

  G4VSolid * fiberCoreInsS           = new G4Tubs ("FiberCoreInsS", 0., fiberCore_radius * 0.9999, 0.5*fiber_length, 0.*deg, 360.*deg) ;
  G4LogicalVolume * fiberCoreInsLV   = new G4LogicalVolume (fiberCoreInsS, CoMaterial, "FiberCoreInsLV") ;
  G4VPhysicalVolume * fiberCoreInsPV = new G4PVPlacement (0, G4ThreeVector (0., 0., 0.), fiberCoreInsLV, "FiberCoreInsPV", towerLV, false, 0, true) ;


  //-----------------------------------------------------
  //------------- Visualization attributes --------------
  //-----------------------------------------------------
  
  G4Colour  white   (1.0, 1.0, 1.0) ;  // white
  G4Colour  gray    (0.5, 0.5, 0.5) ;  // gray
  G4Colour  black   (0.0, 0.0, 0.0) ;  // black
  G4Colour  red     (1.0, 0.0, 0.0) ;  // red
  G4Colour  green   (0.0, 1.0, 0.0) ;  // green
  G4Colour  blue    (0.0, 0.0, 1.0) ;  // blue
  G4Colour  cyan    (0.0, 1.0, 1.0) ;  // cyan
  G4Colour  magenta (1.0, 0.0, 1.0) ;  // magenta 
  G4Colour  yellow  (1.0, 1.0, 0.0) ;  // yellow
  G4Colour  brass   (0.8, 0.6, 0.4) ;  // brass
  G4Colour  brown   (0.7, 0.4, 0.1) ;  // brown
  
  G4VisAttributes* VisAttWorld = new G4VisAttributes (white) ;
  VisAttWorld->SetVisibility (true) ;
  VisAttWorld->SetForceWireframe (true) ;
  worldLV->SetVisAttributes (VisAttWorld) ;
  
  G4VisAttributes* VisAttCalor = new G4VisAttributes (yellow) ;
  VisAttCalor->SetVisibility (true) ;
  VisAttCalor->SetForceWireframe (true) ;
  calorLV->SetVisAttributes (VisAttCalor) ;

  G4VisAttributes* VisAttPostShower = new G4VisAttributes (yellow) ;
  VisAttPostShower->SetVisibility (false) ;
  VisAttPostShower->SetForceWireframe (true) ;
  postShowerLV->SetVisAttributes (VisAttPostShower) ;

  G4VisAttributes* VisAttAbsorber = new G4VisAttributes (gray) ;
  VisAttAbsorber->SetVisibility (true) ;
  VisAttAbsorber->SetForceWireframe (false) ;
  absoLV->SetVisAttributes (VisAttAbsorber) ;
 
  G4VisAttributes* VisAttFiberCore = new G4VisAttributes (green) ;
  VisAttFiberCore->SetVisibility (true) ;
  VisAttFiberCore->SetForceWireframe (false) ;
  fiberCoreInsLV->SetVisAttributes (VisAttFiberCore) ;  
  fiberCoreOutLV->SetVisAttributes (VisAttFiberCore) ;  
 
  G4VisAttributes* VisAttFiberClad = new G4VisAttributes (cyan) ;
  VisAttFiberClad->SetVisibility (true) ;
  VisAttFiberClad->SetForceWireframe (false) ;
  fiberCladLV->SetVisAttributes (VisAttFiberClad) ;  

  G4VisAttributes* VisAttLayer = new G4VisAttributes (blue) ;
  VisAttLayer->SetVisibility (false) ;
  VisAttLayer->SetForceWireframe (false) ;
  towerLV->SetVisAttributes (VisAttLayer) ;

  G4VisAttributes* VisAttRow = new G4VisAttributes (blue) ;
  VisAttRow->SetVisibility (false) ;
  VisAttRow->SetForceWireframe (false) ;
  rowLV->SetVisAttributes (VisAttLayer) ;

  G4cout << ">>>>>> DetectorConstruction::Construct ()::end <<< " << G4endl ;
  return worldPV ;
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::readConfigFile (string configFileName)
{	
  ConfigFile config (configFileName) ;
  
  config.readInto (module_xy, "module_xy") ;
  config.readInto (module_z, "module_z") ;
  config.readInto (NtowersOnSide, "NtowersOnSide") ;

  config.readInto (abs_material, "abs_material") ;
  
  config.readInto (fiberCore_material, "fiberCore_material") ;
  config.readInto (fiberCore_radius, "fiberCore_radius") ;
  config.readInto (fiberClad_material, "fiberClad_material") ;
  config.readInto (fiberClad_radius, "fiberClad_radius") ;
  config.readInto (fiber_length, "fiber_length") ;
  
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::initializeMaterials ()
{
  // define materials
  
  AbMaterial = NULL ;
  if     ( abs_material == 1 ) AbMaterial = MyMaterials::Brass () ;
  else if ( abs_material == 2 ) AbMaterial = MyMaterials::Tungsten () ;
  else if ( abs_material == 3 ) AbMaterial = MyMaterials::Lead () ;
  else if ( abs_material == 4 ) AbMaterial = MyMaterials::Iron () ;
  else if ( abs_material == 5 ) AbMaterial = MyMaterials::Aluminium () ;
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid absorber material specifier " << abs_material << G4endl ;
    exit (-1) ;
  }
  G4cout << "Ab. material: "<< AbMaterial << G4endl ;
    
  CoMaterial = NULL ;
  if      ( fiberCore_material == 1 ) CoMaterial = MyMaterials::Quartz () ;
  else if ( fiberCore_material == 2 ) CoMaterial = MyMaterials::SiO2_Ce () ;
  else if ( fiberCore_material == 3 ) CoMaterial = MyMaterials::DSB_Ce () ;
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid fiber core material specifier " << fiberCore_material << G4endl ;
    exit (-1) ;
  }
  G4cout << "Co. material: "<< CoMaterial << G4endl ;
  
  ClMaterial = NULL ;
  if      ( fiberClad_material == 1 ) ClMaterial = MyMaterials::Quartz () ;
  else if ( fiberClad_material == 2 ) ClMaterial = MyMaterials::SiO2_Ce () ;
  else if ( fiberClad_material == 3 ) ClMaterial = MyMaterials::DSB_Ce () ;
  else
  {
    G4cerr << "<DetectorConstructioninitializeMaterials>: Invalid fiber clad material specifier " << fiberClad_material << G4endl ;
    exit (-1) ;
  }
  G4cout << "Cl. material: "<< ClMaterial << G4endl ;
  
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void DetectorConstruction::fillPolygon (std::vector<G4TwoVector>& theBase, const float& side, const float& chamfer)
{
  // wrt the centre (0, 0)
  double delta = side - chamfer * 0.707106781188 ;
  theBase.push_back (G4TwoVector (side, delta)) ;
  theBase.push_back (G4TwoVector (delta, side)) ;
  theBase.push_back (G4TwoVector (-1 * delta, side)) ;
  theBase.push_back (G4TwoVector (-1 * side, delta)) ;
  theBase.push_back (G4TwoVector (-1 * side, -1 * delta)) ;
  theBase.push_back (G4TwoVector (-1 * delta, -1 * side)) ;
  theBase.push_back (G4TwoVector (delta, -1 * side)) ;
  theBase.push_back (G4TwoVector (side, -1 * delta)) ;
  return ;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void DetectorConstruction::fillHexagon (std::vector<G4TwoVector>& hexagon, const float& radius)
  {
    // wrt the centre (0, 0)
    for (int i = 0 ; i < 6 ; ++i)
      {
        double angle = i * 3.14 / 3. ; // FIXME put the constant for PI
        hexagon.push_back (G4TwoVector (radius * cos (angle), radius * sin (angle))) ;
      }
    return ;
  }


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void DetectorConstruction::fillSquare (std::vector<G4TwoVector>& square, const float& halfSide)
  {
    // wrt the centre (0, 0)
    square.push_back (G4TwoVector ( 1 * halfSide , -1 * halfSide)) ;
    square.push_back (G4TwoVector ( 1 * halfSide ,  1 * halfSide)) ;
    square.push_back (G4TwoVector (-1 * halfSide ,  1 * halfSide)) ;
    square.push_back (G4TwoVector (-1 * halfSide , -1 * halfSide)) ;
    return ;
  }
