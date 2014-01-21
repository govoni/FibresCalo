#include <iostream>
#include <vector>
#include <map>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"



class CreateTree
{
private:
  
  TTree*  ftree ;
  TString fname ;
  
public:
  
  CreateTree (TString name, int NtowersOnSide) ;
  ~CreateTree () ;
  
  TTree*             GetTree  () const { return ftree ; } ;
  TString            GetName  () const { return fname ; } ;
  int                Fill     () ;
  bool               Write    () ;
  void               Clear    () ;
  static CreateTree* Instance () { return fInstance ; } ;
  static CreateTree* fInstance ;
  
  int   Event ;
  int fNtowersOnSide ;
  int fNtowersOnSideSQ ;
  double depositedEnergy ;
  std::vector<float> * inputMomentum ; // Px Py Pz E
  std::vector<float> * inputInitialPosition ; // x, y, z
  // energy deposited in each fibre of a tower,
  // sorted by position in the square matrix of the detector
  // the (i,j) index in the matrix becomes NjTot * j + i in this vector
  std::vector<float> * depositedEnergies ;
  // energy deposited in each tower (absorber + fibre),
  // sorted by position in the square matrix of the detector
  // the (i,j) index in the matrix becomes NjTot * j + i in this vector
  std::vector<float> * totalEnergies ; 

} ;
