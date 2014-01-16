
TRandom3 * gRandom ;


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


// LY = photons per MeV in the fibre
float smearEnergy (float energy, float LY = 10000, float acceptance = 0.1, float efficiency = 0.1)
{
  int nPh = floor (energy * LY * 1000) ; // 1000 MeV = 1 GeV
  float result = gRandom->Poisson (nPh) ;
  return result * acceptance * efficiency / LY ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void plotResolution (TString inputFile)
{
   // init tree and basic vars
   // ---- ---- ---- ---- ---- ---- ----

   int fNtowersOnSide = 50 ;
   float maxEnergy = 1. ; // gev
   TFile *f = new TFile (inputFile) ;
   TTree * t = (TTree *) f->Get ("tree") ;
   
   // prepare input variables
   // ---- ---- ---- ---- ---- ---- ----

   double depositedEnergy ;
   t->Branch ("depositedEnergy", &depositedEnergy, "depositedEnergy/D") ;
   int fNtowersOnSideSQ = fNtowersOnSide * fNtowersOnSide ;
   std::vector<float> * depositedEnergies = new std::vector<float> (fNtowersOnSideSQ) ; 
   t->SetBranchAddress ("depositedEnergies", &depositedEnergies) ;
   std::vector<float> * totalEnergies = new std::vector<float> (fNtowersOnSideSQ) ; 
   t->SetBranchAddress ("totalEnergies", &totalEnergies) ;
   std::vector<float> * inputMomentum = new std::vector<float> (4) ; 
   t->SetBranchAddress ("inputMomentum", &inputMomentum) ;
  
   TH1F h_ErecoOverEgen ("h_ErecoOverEgen", "ErecoOverEgen", 100, 0, 1) ;
   TH2F h_ErecoOverEgen_vs_Ebeam ("h_ErecoOverEgen_vs_Ebeam", "h_ErecoOverEgen_vs_Ebeam", 45, 5, 50, 100, 0, 1) ;

   Int_t nentries = tree->GetEntries () ;
   cout << "found " << nentries << "events\n" ;
   for (Int_t iEntry = 0 ; iEntry < 100 ; ++iEntry) 
     {
        if (iEntry % 10 == 0) cout << "reading event " << iEntry << endl ;
        tree->GetEvent (iEntry) ;
        float observedEnergy = 0. ;
        for (int j = 0 ; j < fNtowersOnSideSQ ; ++j)
          {
            observedEnergy += smearEnergy (depositedEnergies->at (j)) ;
          }
        float beamEnergy = inputMomentum.at (3) ;
        float ErecoOverEgen = observedEnergy / beamEnergy ;
        h_ErecoOverEgen.Fill (ErecoOverEgen) ;
        h_ErecoOverEgen_vs_Ebeam.Fill (beamEnergy, ErecoOverEgen) ;
        float sigmaEoverE = (observedEnergy - beamEnergy) / beamEnergy ;
     }

   TFile outfile ("plotResolution.root", "recreate") ;
   outfile.cd () ;
   h_ErecoOverEgen.Write () ;
   h_ErecoOverEgen_vs_Ebeam.Write () ;
   outfile.Close () ;
   
   return ;


}
