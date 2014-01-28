#include <iostream>
#include <vector>
#include <map>

#include "TNtuple.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"



class CreateTree
{
private:
  
  TTree*  ftree ;
  TString fname ;
  
public:
  
  CreateTree (TString name, float tower_side) ;
  ~CreateTree () ;
  
  TTree*             GetTree  () const { return ftree ; } ;
  TString            GetName  () const { return fname ; } ;
  void               AddEnergyDeposit (int index, float deposit) ;
  int                Fill     () ;
  bool               Write    (TFile *) ;
  void               Clear    () ;
  static CreateTree* Instance () { return fInstance ; } ;
  static CreateTree* fInstance ;
  
  int   Event ;
  int fNtowersOnSide ;
  int fNtowersOnSideSQ ;
  float depositedEnergy ;
  // energy deposited in a block after the calorimeter
  float leakageEnergy ;
  std::vector<float> * inputMomentum ; // Px Py Pz E
  std::vector<float> * inputInitialPosition ; // x, y, z
  // energy deposited in each fibre of a tower
  std::vector<float> * depositedEnergies ;
  // index of the fibre where the deposit happens
  // from the PV name
  std::vector<int> * depositFibres ; 
  
  TH2F * leakeage ;

  // to be filled at the beginning of the event generation only
  TNtuple * fibresPosition ; 


} ;
