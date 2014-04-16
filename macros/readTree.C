{
   gROOT->Reset();
   int fNtowersOnSide = 49 ;
   float maxEnergy = 1. ; // gev
   TFile *f = new TFile ("outTot_square_49x49.root") ;
   TTree * t = (TTree *) f->Get ("tree") ;
   double depositedEnergy ;
   int fNtowersOnSideSQ = fNtowersOnSide * fNtowersOnSide ;
   std::vector<float> * depositedEnergies = new std::vector<float> (fNtowersOnSideSQ) ; 
   t->SetBranchAddress ("depositedEnergies", &depositedEnergies) ;
   t->Branch ("depositedEnergy", &depositedEnergy, "depositedEnergy/D") ;

   TH2F * h_depositedEnergies = new TH2F ("h_depositedEnergies", "energy distribution", 
             fNtowersOnSide, 0., fNtowersOnSide, fNtowersOnSide, 0., fNtowersOnSide) ;
   TH1F * h_depositedEnergies1D = new TH1F ("h_depositedEnergies1D", "energy distribution", 100, 0., maxEnergy) ;
   TH1F * h_depositedEnergies_radial = new TH1F ("h_depositedEnergies_radial", "energy distribution", fNtowersOnSide, 0., fNtowersOnSide) ;
   Int_t nentries = tree->GetEntries () ;
   for (Int_t i = 0 ; i < nentries ; ++i) 
     {
        tree->GetEvent(i);
        for (int j = 0 ; j < fNtowersOnSideSQ ; ++j)
          {
            h_depositedEnergies->Fill (j/fNtowersOnSide + 0.5, j%fNtowersOnSide + 0.5, depositedEnergies->at (j)) ;
            h_depositedEnergies1D->Fill (depositedEnergies->at (j)) ;
            radius = (j/fNtowersOnSide - 0.5 * fNtowersOnSide) *  (j/fNtowersOnSide - 0.5 * fNtowersOnSide) +
                     (j%fNtowersOnSide - 0.5 * fNtowersOnSide) *  (j%fNtowersOnSide - 0.5 * fNtowersOnSide) ;
            radius = sqrt (radius) ;
            h_depositedEnergies_radial->Fill (radius, depositedEnergies->at (j)) ;
          }
     }
   h_depositedEnergies->SetStats (0) ;
   h_depositedEnergies->Draw ("lego") ;
//   h_depositedEnergies->Draw ("colz") ;
//   h_depositedEnergies1D->Draw () ;
   h_depositedEnergies_radial->Draw () ;
}
