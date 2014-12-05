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
#include "MyMaterials.hh"
#include "LedFiberTiming.hh"
#include "DetectorParameterisation.hh"

#include "G4Material.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4UniformMagField.hh"



class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  //! ctor
  DetectorConstruction  () ;
  DetectorConstruction  (const string& configFileName) ;
  
  //! dtor
  ~DetectorConstruction () ;
  
  //! construct method
  G4VPhysicalVolume* Construct () ;
  
  //! other methods
  G4double GetModule_z () const { return module_z ; } ;
  
  void initializeMaterials () ;
  void ConstructField () ;
  
  Fiber* GetFiber() { return &fib ; } ;
  
  
private:
  G4bool    checkOverlaps ;
  
  G4double  expHall_x ;
  G4double  expHall_y ;
  G4double  expHall_z ;
  
  G4int    world_material ;    // world material
  
  G4int    abs_material ;    // absorber material
  G4double W_fraction ;      // fraction of Tungsten in the alloy
  G4double hole_radius ;     // radius of the holes
  G4double module_z ;        // will be set as fibre length
  G4double module_xy ;       // size of the calo tower containing fibres
  G4int    postshower;       // flag to place a postshower behind the module
  
  G4double margin ;               // minimum distance between fibres and tower sides
  G4int    nFibresAlongX ;        // number of fibres along the Y side of the calo tower
  G4int    nFibresAlongY ;        // number of fibres along the Y side of the calo tower
  G4double fibreDistanceAlongX;
  G4double fibreDistanceAlongY;
  G4double startX;
  G4double startY;
  // FIXME put this in, in future
  //G4Double  tolerance ;            // minimum distance between fibre and module side
  
  G4int    fibre_scheme ;
  G4int    fibre_material ;
  G4double fibre_cladRIndex;
  G4int    fibre_isSquare;
  G4double fibre_radius ;
  G4double fibre_length ;      
  G4double fibre_distance ;    // distance between fibres
  G4double fibre_absLength ;   // absorption length in the fiber
  
  G4int gap_material ;
  G4double gap_l ;
  
  G4int det_material ;
  G4double det_l ;
    
  G4double depth ;
  
  std::vector<G4double> attLengths;
  
  G4UniformMagField * B_field ;
  G4bool   B_field_IsInitialized ; 
  G4double B_field_intensity ;     // magnetic field, in units of Tesla
  
  Fiber fib ;
  
  //Materials
  G4Material* WoMaterial ;
  G4Material* AbMaterial ;
  G4Material* CoMaterial ;
  G4Material* ClMaterial ;
  G4Material* GaMaterial ;
  G4Material* DeMaterial ;
} ;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*DetectorConstruction_h*/
