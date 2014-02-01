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
// * institutes,nor the agencies providing financial support for this *
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
// $Id: DetectorConstruction.hh,v 1.5 2006-06-29 17:53:55 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include <iostream>
#include <string>
#include <fstream>
#include <utility>

#include "ConfigFile.hh"
#include "TString.h"

#include "globals.hh"
#include "G4Material.hh"
#include "MyMaterials.hh"
#include "G4Element.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4Box.hh"
#include "G4ExtrudedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4TwoVector.hh"



class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction  () ;
  DetectorConstruction  (const string& configFileName) ;
  ~DetectorConstruction () ;
  
  G4double GetModule_z () const { return module_z ; } ;
  
  void fillPolygon (std::vector<G4TwoVector>& theBase, const float& side, const float& chamfer) ;
  void fillSquare (std::vector<G4TwoVector>& theBase, const float& side) ;
  void fillHexagon (std::vector<G4TwoVector>& theBase, const float& side) ;
  
  std::pair<G4TwoVector,G4TwoVector> getChamfer  (std::vector<G4TwoVector>& theBase, const int& index) ;
  void initializeMaterials                       () ;
  void ConstructField                            () ;
  
public:
  G4VPhysicalVolume* Construct () ;
  
private:
  G4VPhysicalVolume* fAbsorberPV ;      // the absorber physical volume
  G4VPhysicalVolume* fCrystalPV ;       // the crystal physical volume

  G4bool    checkOverlaps ;
  
  G4double  expHall_x ;
  G4double  expHall_y ;
  G4double  expHall_z ;
            
  G4double  fibres_distance ;      // distance between fibres
  G4double  module_z ;             // length of the calo tower
  G4double  tower_side ;           // size of the calo tower containing fibres
// FIXME put this in, in future
//  G4Double  tolerance ;            // minimum distance between fibre and module side
  G4int     NfibresAlongY ;        // number of fibres along the Y side of the calo tower
  G4double  margin ;               // minimum distance between fibres and tower sides

  G4int    abs_material ;          // absorber material
  
  G4int    fiberCore_material ;
  G4double fiberCore_radius ;
  G4int    fiberClad_material ;
  G4double fiberClad_radius ;
  G4double fiber_length ;          // will be set as module_z
  
//  G4double depth ;
  G4bool   B_field_IsInitialized ; 
  G4double B_field_intensity ;     // magnetic field, in units of Tesla
  
  //Materials
  G4Material* AbMaterial ;
  G4Material* CoMaterial ;
  G4Material* ClMaterial ;
} ;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*DetectorConstruction_h*/
