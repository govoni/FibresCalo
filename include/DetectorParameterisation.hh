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
// $Id: DetectorParameterisation.hh,v 1.5 2006-06-29 17:53:55 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorParameterisation_h
#define DetectorParameterisation_h 1

#include <iostream>
#include <string>

#include "G4VPVParameterisation.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"
#include "G4Tubs.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"



class FibreParameterisation : public G4VPVParameterisation
{
public:
  
  //! ctor
  FibreParameterisation(const G4double& mxy, const G4double& fr, const G4double& fd, const G4double& fl, G4Material* mat);
  
  //! dtor
  ~FibreParameterisation();
  
  //! other methods
  void ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const;
  
  using G4VPVParameterisation::ComputeDimensions;
  void ComputeDimensions(G4Tubs& fiberSolid, const G4int copyNo, G4VPhysicalVolume* physVol) const;
  
  using G4VPVParameterisation::ComputeSolid;
  G4VSolid* ComputeSolid(const G4int copyNo, G4VPhysicalVolume* physVol) const;
  
  using G4VPVParameterisation::ComputeMaterial;
  G4Material* ComputeMaterial(const G4int copyNo, G4VPhysicalVolume* physVol, const G4VTouchable* parentTouch=0) const;
  
  G4int GetNFibres() const { return NfibresAlongX*NfibresAlongY; };
  
private:
  G4double module_xy;
  G4double fibre_radius;
  G4double fibre_distance;
  G4double fibre_length;
  G4Material* material;
  
  G4double margin;
  G4double start_x;
  G4double start_y;
  G4int NfibresAlongX;
  G4int NfibresAlongY;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



class HoleParameterisation : public G4VPVParameterisation
{
public:
  
  //! ctor
  HoleParameterisation(const G4double& mxy, const G4double& fr, const G4double& hr, const G4double& hd, const G4double& hl, G4Material* mat);
  
  //! dtor
  ~HoleParameterisation();
  
  //! other methods
  void ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const;
  
  using G4VPVParameterisation::ComputeDimensions;
  void ComputeDimensions(G4Tubs& fiberSolid, const G4int copyNo, G4VPhysicalVolume* physVol) const;
  
  using G4VPVParameterisation::ComputeSolid;
  G4VSolid* ComputeSolid(const G4int copyNo, G4VPhysicalVolume* physVol) const;
  
  using G4VPVParameterisation::ComputeMaterial;
  G4Material* ComputeMaterial(const G4int copyNo, G4VPhysicalVolume* physVol, const G4VTouchable* parentTouch=0) const;
  
  G4int GetNHoles() const { return NholesAlongX*NholesAlongY; };
  
private:
  G4double module_xy;
  G4double fibre_radius;
  G4double hole_radius;
  G4double hole_distance;
  G4double hole_length;
  G4Material* material;
  
  G4double margin;
  G4double start_x;
  G4double start_y;
  G4int NholesAlongX;
  G4int NholesAlongY;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif /*DetectorParameterisation_h*/
