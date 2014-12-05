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
// $Id: DetectorParameterisation.cc, v 1.18 2010-10-23 19:27:38 gum Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 
//---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

#include "DetectorParameterisation.hh"

using namespace CLHEP;



FibreParameterisation::FibreParameterisation(const G4double& mxy, const G4double& fr, const G4double& fd, const G4double& fl,
                                             G4Material* mat):
  module_xy(mxy),
  fibre_radius(fr),
  fibre_distance(fd),
  fibre_length(fl),
  material(mat)
{
  margin = std::max( 0.25*fibre_distance, 2*fibre_radius);
  
  NfibresAlongX = floor( (module_xy-margin-start_x)/(fibre_distance*0.8660) );
  NfibresAlongY = floor( (module_xy-2*margin-0.5*fibre_distance)/fibre_distance ) + 1 ;
  
  start_x = 0.5 * ( module_xy - floor ((module_xy - 2 * margin)/(fibre_distance * 0.8660)) * (fibre_distance * 0.8660) );
  start_y = 0.5 * ( module_xy - fibre_distance*(NfibresAlongY-0.5) );
}



FibreParameterisation::~FibreParameterisation()
{}



void FibreParameterisation::ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  G4int count_x = floor(copyNo/NfibresAlongY);
  G4int count_y = int(copyNo)%int(NfibresAlongY);
  
  G4double x_c = -0.5*module_xy+start_x + count_x*fibre_distance*0.8660;
  G4double y_c = -0.5*module_xy+start_y + count_y*fibre_distance        + 0.5*fibre_distance*(count_x%2);
  G4double z_c = 0.;
  //CreateTree::Instance()->fibresPosition -> Fill(copyNo,x_c,y_c);
  
  G4ThreeVector origin(x_c,y_c,z_c);
  physVol -> SetTranslation(origin);
  physVol -> SetRotation(0);
}



void FibreParameterisation::ComputeDimensions(G4Tubs& fibreSolid, const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  fibreSolid.SetInnerRadius(0.);
  fibreSolid.SetOuterRadius(fibre_radius);
  fibreSolid.SetZHalfLength(0.5*fibre_length);
  fibreSolid.SetStartPhiAngle(0.*deg);
  fibreSolid.SetDeltaPhiAngle(360.*deg);
}



G4VSolid* FibreParameterisation::ComputeSolid(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  G4VSolid* solid = new G4Tubs("fibreS",0.,fibre_radius,0.5*fibre_length,0.*deg,360.*deg);
  return solid;
}



G4Material* FibreParameterisation::ComputeMaterial(const G4int copyNo, G4VPhysicalVolume* physVol, const G4VTouchable* parentTouch) const
{
  return material;
}







HoleParameterisation::HoleParameterisation(const G4double& mxy, const G4double& fr, const G4double& hr, const G4double& hd, const G4double& hl,
                                           G4Material* mat):
  module_xy(mxy),
  fibre_radius(fr),
  hole_radius(hr),
  hole_distance(hd),
  hole_length(hl),
  material(mat)
{
  margin = std::max( 0.25*hole_distance, 2*hole_radius);
  
  NholesAlongX = floor( (module_xy-margin-start_x)/(hole_distance*0.8660) );
  NholesAlongY = floor( (module_xy-2*margin-0.5*hole_distance)/hole_distance ) + 1 ;
  
  start_x = 0.5 * ( module_xy - floor ((module_xy - 2 * margin)/(hole_distance * 0.8660)) * (hole_distance * 0.8660) );
  start_y = 0.5 * ( module_xy - hole_distance*(NholesAlongY-0.5) );
}



HoleParameterisation::~HoleParameterisation()
{}



void HoleParameterisation::ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  G4int count_x = floor(copyNo/NholesAlongY);
  G4int count_y = int(copyNo)%int(NholesAlongY);
  
  G4double x_c = -0.5*module_xy+start_x + count_x*hole_distance*0.8660;
  G4double y_c = -0.5*module_xy+start_y + count_y*hole_distance        + 0.5*hole_distance*(count_x%2);
  G4double z_c = 0.;
  //CreateTree::Instance()->holesPosition -> Fill(copyNo,x_c,y_c);
  
  G4ThreeVector origin(x_c,y_c,z_c);
  physVol -> SetTranslation(origin);
  physVol -> SetRotation(0);
}



void HoleParameterisation::ComputeDimensions(G4Tubs& holeSolid, const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  holeSolid.SetInnerRadius(fibre_radius);
  holeSolid.SetOuterRadius(fibre_radius+hole_radius);
  holeSolid.SetZHalfLength(0.5*hole_length);
  holeSolid.SetStartPhiAngle(0.*deg);
  holeSolid.SetDeltaPhiAngle(360.*deg);
}



G4VSolid* HoleParameterisation::ComputeSolid(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  G4VSolid* solid = new G4Tubs("holeS",fibre_radius,fibre_radius+hole_radius,0.5*hole_length,0.*deg,360.*deg);
  return solid;
}



G4Material* HoleParameterisation::ComputeMaterial(const G4int copyNo, G4VPhysicalVolume* physVol, const G4VTouchable* parentTouch) const
{
  return material;
}
