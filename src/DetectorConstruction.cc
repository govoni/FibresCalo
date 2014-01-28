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
#include "CreateTree.hh"
#include <algorithm>
#include <string>
#include <sstream>

using namespace std ;

string to_string (int num)
{
  string Result;           // string which will contain the result
  ostringstream convert;   // stream used for the conversion
  convert << num;          // insert the textual representation of 'Number' in the characters in the stream
  Result = convert.str (); // set 'Result' to the contents of the stream
  return Result ;
}


//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


DetectorConstruction::DetectorConstruction (const string& configFileName)
{
  ConfigFile config (configFileName) ;
  
  config.readInto (fibres_distance, "fibres_distance") ;
  config.readInto (module_z, "module_z") ;
//  config.readInto (NfibresOnSide, "NfibresOnSide") ;
  config.readInto (tower_side, "tower_side") ;

  config.readInto (abs_material, "abs_material") ;
  
  config.readInto (fiberCore_material, "fiberCore_material") ;
  config.readInto (fiberCore_radius, "fiberCore_radius") ;
  config.readInto (fiberClad_material, "fiberClad_material") ;
  config.readInto (fiberClad_radius, "fiberClad_radius") ;
  config.readInto (fiber_length, "fiber_length") ;
  
  
  //---------------------------------------
  //------------- Parameters --------------
  //---------------------------------------
  
  initializeMaterials () ;
  
//  tower_side = (NfibresOnSide - 1) * fibres_distance + 4 * fiberClad_radius ;

  margin = max (0.25 * fibres_distance, 2 * fiberClad_radius) ;
  NfibresAlongY = floor ((tower_side - 2 * margin - 0.5 * fibres_distance) / fibres_distance) + 1 ;
  fiber_length = module_z ;

  expHall_x = tower_side * 4 ; 
  expHall_y = tower_side * 4 ; 
  expHall_z = module_z * 8 ;

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
  G4VSolid * worldS = new G4Box ("World", 0.5*expHall_x, 0.5*expHall_y, 0.5*expHall_z) ;
  G4LogicalVolume * worldLV = new G4LogicalVolume (worldS, MyMaterials::Air (), "World", 0, 0, 0) ;
  G4VPhysicalVolume * worldPV = new G4PVPlacement (0, G4ThreeVector (), worldLV, "World", 0, false, 0, true) ;

  // The embedding absorber, mimicking the rest of material around
  //   put an embedder four times as the detector in each direction, and fill it with absorber, 
  //   to quickly stop leaking showers and get an idea of where it would leak laterally
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * embedderS = new G4Box ("embedder", 0.5*expHall_x, 0.5*expHall_y, 0.25*expHall_z) ;
  G4LogicalVolume * embedderLV = new G4LogicalVolume (embedderS, AbMaterial, "embedderLV", 0, 0, 0) ;
  G4VPhysicalVolume * embedderPV = new G4PVPlacement (0, G4ThreeVector (0., 0., 0.25*expHall_z), embedderLV, "embedderPV", worldLV, false, 0, true) ;

  // The calorimeter
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * absorberS = new G4Box ("absorberS", 0.5 * tower_side, 0.5 * tower_side, 0.5 * module_z) ;
  G4LogicalVolume * absorberLV = new G4LogicalVolume (absorberS, AbMaterial, "absorberLV") ;
  G4VPhysicalVolume * absorberPV = new G4PVPlacement (0, G4ThreeVector (), absorberLV, "absorberPV", embedderLV, false, 0, true) ;

  // the fibres
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  G4VSolid * fiberCladS              = new G4Tubs ("FiberCladS", 0., fiberClad_radius, 0.5*fiber_length, 0.*deg, 360.*deg) ;    
  G4LogicalVolume * fiberCladLV      = new G4LogicalVolume (fiberCladS, ClMaterial, "FiberCladLV") ;    

/*
// removed for the time being, as not sure how to get the energy deposit of each of them

  G4VSolid * fiberCoreOutS           = new G4Tubs ("FiberCoreOutS", fiberCore_radius * 0.9999, fiberCore_radius, 0.5*fiber_length, 0.*deg, 360.*deg) ;
  G4LogicalVolume * fiberCoreOutLV   = new G4LogicalVolume (fiberCoreOutS, CoMaterial, "FiberCoreOutLV") ;
  G4VPhysicalVolume * fiberCoreOutPV = new G4PVPlacement (0, G4ThreeVector (0., 0., 0.), fiberCoreOutLV, "FiberCoreOutPV", fiberCladLV, false, 0, true) ;

  G4VSolid * fiberCoreInsS           = new G4Tubs ("FiberCoreInsS", 0., fiberCore_radius * 0.9999, 0.5*fiber_length, 0.*deg, 360.*deg) ;
  G4LogicalVolume * fiberCoreInsLV   = new G4LogicalVolume (fiberCoreInsS, CoMaterial, "FiberCoreInsLV") ;
  G4VPhysicalVolume * fiberCoreInsPV = new G4PVPlacement (0, G4ThreeVector (0., 0., 0.), fiberCoreInsLV, "FiberCoreInsPV", fiberCladLV, false, 0, true) ;
*/

  // triangular-based fibres matrix filling
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  int count_x = 0 ; // for the staggering
  float start_x = 0.5 * (tower_side - 
                         floor ((tower_side - 2 * margin) / (fibres_distance * 0.8660))
                            * (fibres_distance * 0.8660)) ;
  float start_y = 0.5 * (tower_side - fibres_distance * (NfibresAlongY - 0.5)) ;
  // loop on x direction
  for (float x = - 0.5 * tower_side + start_x ; 
       x < 1.01 * (0.5 * tower_side - margin) ; 
       x += fibres_distance * 0.8660)
    {
      // loop on y direction 
      int count_y = 0 ; // for the staggering
      for (float y = - 0.5 * tower_side + start_y ; 
           count_y < NfibresAlongY ; 
           y += fibres_distance)
        {
          float y_c = y + 0.5 * fibres_distance * (count_x % 2) ; // fibres staggering
          int index = count_x * NfibresAlongY + count_y ;
          CreateTree::Instance ()->fibresPosition->Fill (index, x, y_c) ;
          string name = "FiberCladPV " + to_string (index) ;
          new G4PVPlacement (
              0,                           // rotation
              G4ThreeVector (x, y_c, 0.),  // translation
              fiberCladLV, name, absorberLV, false, 0, true) ;
          ++count_y ;
        } // loop on y direction
      ++count_x ;   
    } // loop on x direction

/*
  find a way to determine the energy deposit position

*/

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
  
  G4VisAttributes* VisAttWorld = new G4VisAttributes (black) ;
  VisAttWorld->SetVisibility (true) ;
  VisAttWorld->SetForceWireframe (true) ;
  worldLV->SetVisAttributes (VisAttWorld) ;
  
  G4VisAttributes* VisAttEmbedder = new G4VisAttributes (red) ;
  VisAttEmbedder->SetVisibility (true) ;
  VisAttEmbedder->SetForceWireframe (true) ;
  embedderLV->SetVisAttributes (VisAttEmbedder) ;
  
  G4VisAttributes* VisAttAbsorber = new G4VisAttributes (gray) ;
  VisAttAbsorber->SetVisibility (true) ;
  VisAttAbsorber->SetForceWireframe (true) ;
  absorberLV->SetVisAttributes (VisAttAbsorber) ;
 
/*
  G4VisAttributes* VisAttFiberCore = new G4VisAttributes (green) ;
  VisAttFiberCore->SetVisibility (true) ;
  VisAttFiberCore->SetForceWireframe (false) ;
  fiberCoreInsLV->SetVisAttributes (VisAttFiberCore) ;  
  fiberCoreOutLV->SetVisAttributes (VisAttFiberCore) ;  
*/
 
  G4VisAttributes* VisAttFiberClad = new G4VisAttributes (cyan) ;
  VisAttFiberClad->SetVisibility (true) ;
  VisAttFiberClad->SetForceWireframe (false) ;
  fiberCladLV->SetVisAttributes (VisAttFiberClad) ;  

  G4cout << ">>>>>> DetectorConstruction::Construct ()::end <<< " << G4endl ;
  return worldPV ;
}


//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


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


//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


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


//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


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


//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 


void DetectorConstruction::fillSquare (std::vector<G4TwoVector>& square, const float& halfSide)
  {
    // wrt the centre (0, 0)
    square.push_back (G4TwoVector ( 1 * halfSide , -1 * halfSide)) ;
    square.push_back (G4TwoVector ( 1 * halfSide ,  1 * halfSide)) ;
    square.push_back (G4TwoVector (-1 * halfSide ,  1 * halfSide)) ;
    square.push_back (G4TwoVector (-1 * halfSide , -1 * halfSide)) ;
    return ;
  }
