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
  
  CreateTree (TString name) ;
  ~CreateTree () ;
  
  TTree*             GetTree  () const { return ftree ; } ;
  TString            GetName  () const { return fname ; } ;
  int                Fill     () ;
  bool               Write    () ;
  void               Clear    () ;
  static CreateTree* Instance () { return fInstance ; } ;
  
  // feed the info of each single photon to the tree
  void               addInputParticle (float ptx, float pty, float ptz, float E) ;
  void               addEnergy (float energy) ;

  static CreateTree* fInstance ;
  
  int   Event ;
  float depositedEnergy ;
  float inputMomentum[4] ; // Px Py Pz E

} ;
