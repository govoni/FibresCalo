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
  int fNtowersOnSideSQ ;
  
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
  float depositedEnergy ;
  float inputMomentum[4] ; // Px Py Pz E
  float * depositedEnergies ;

} ;
