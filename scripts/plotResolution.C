
TRandom3 * gRandom ;


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


// LY = photons per MeV in the fibre
float smearEnergy (float energy, float LY = 10000, float acceptance = 0.1, float efficiency = 0.1)
{
  LY *= 1000 ; // 1000 MeV = 1 GeV
  int nPh = floor (energy * acceptance * efficiency * LY) ;
  float result = gRandom->Poisson (nPh) ;
  return result / LY ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void plotResolution (TString inputFile)
{
   // init tree and basic vars
   // ---- ---- ---- ---- ---- ---- ----

   int fNtowersOnSide = 49 ;
   float maxEnergy = 1. ; // gev
   TFile * f = new TFile (inputFile) ;
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
   std::vector<float> * inputInitialPosition = new std::vector<float> (3) ; 
   t->SetBranchAddress ("inputInitialPosition", &inputInitialPosition) ;
  
   TH1F h_ErecoOverEgen ("h_ErecoOverEgen", "ErecoOverEgen", 400, 0, 0.002) ;
   TH2F h_ErecoOverEgen_vs_Ebeam ("h_ErecoOverEgen_vs_Ebeam", "h_ErecoOverEgen_vs_Ebeam", 
                                   45, 5, 50, 200, 0, 0.002) ;
   TH3F h_ErecoOverEgen_vs_impact ("h_ErecoOverEgen_vs_impact", "h_ErecoOverEgen_vs_impact", 
                                   40, -2., 2., 40, -2., 2., 400, 0, 0.002) ;

   TH1F h_EdepOverEgen ("h_EdepOverEgen", "EdepOverEgen", 100, 0, 1) ;
   TH2F h_EdepOverEgen_vs_Ebeam ("h_EdepOverEgen_vs_Ebeam", "h_EdepOverEgen_vs_Ebeam", 
                                  45, 5, 50, 100, 0, 1) ;

   TH1F h_sigmaEoverE ("sigmaEoverE", "sigmaEoverE", 100, -1., 1.) ;
   TH2F h_sigmaEoverE_vs_Ebeam ("sigmaEoverE_vs_Ebeam", "sigmaEoverE_vs_Ebeam", 
                                 45, 5, 50, 100, -1., 1.) ;

   Int_t nentries = tree->GetEntries () ;
   cout << "found " << nentries << " events\n" ;
   //PG loop over events
   for (Int_t iEntry = 0 ; iEntry < nentries ; ++iEntry) 
     {
        if (iEntry % 100 == 0) cout << "reading event " << iEntry << endl ;
        t->GetEvent (iEntry) ;
        float observedEnergy = 0. ;
        float fullEnergy = 0. ;
        //PG loop over fibres
        for (int j = 0 ; j < fNtowersOnSideSQ ; ++j)
          {
            observedEnergy += smearEnergy (depositedEnergies->at (j)) ;
            fullEnergy += totalEnergies->at (j) ;
          } //PG loop over fibres
        float beamEnergy = inputMomentum->at (3) ;
        float ErecoOverEgen = observedEnergy / beamEnergy ;
        h_ErecoOverEgen.Fill (ErecoOverEgen) ;
        h_ErecoOverEgen_vs_Ebeam.Fill (beamEnergy, ErecoOverEgen) ;
        h_ErecoOverEgen_vs_impact.Fill (inputInitialPosition->at (0),
            inputInitialPosition->at (1), ErecoOverEgen) ;  
        float EdepOverEgen = fullEnergy / beamEnergy ;
        h_EdepOverEgen.Fill (EdepOverEgen) ;
        h_EdepOverEgen_vs_Ebeam.Fill (beamEnergy, EdepOverEgen) ;

        if (EdepOverEgen < 0.7) continue ;

        // 1/3.75307e-04 = 2664.48534133
        float sigmaEoverE = (observedEnergy * 2664.48534133 - beamEnergy) / beamEnergy ;
        h_sigmaEoverE.Fill (sigmaEoverE) ;
        h_sigmaEoverE_vs_Ebeam.Fill (beamEnergy, sigmaEoverE) ;
     } //PG loop over events

   TFile outfile ("plotResolution.root", "recreate") ;
   outfile.cd () ;
   h_ErecoOverEgen.Write () ;
   h_ErecoOverEgen_vs_Ebeam.Write () ;
   h_ErecoOverEgen_vs_impact.Write () ;
   h_EdepOverEgen.Write () ;
   h_EdepOverEgen_vs_Ebeam.Write () ;
   h_sigmaEoverE.Write () ;
   h_sigmaEoverE_vs_Ebeam.Write () ;

   outfile.Close () ;

   return ;
}

/*
Calibration factor for the small beam hitting in a perpendicular manner
the tungsten, at the crossing of four towers
  EXT PARAMETER                                   STEP         FIRST   
  NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE 
   1  Constant     1.25579e+03   2.02423e+01   1.16628e-01   1.83615e-06
   2  Mean         3.52692e-04   4.27726e-07   2.29938e-09   9.59803e+01
   3  Sigma        2.13923e-05   3.38120e-07   2.51361e-05  -3.62594e-03


3 deg tilt in x and y
  EXT PARAMETER                                   STEP         FIRST   
  NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE 
   1  Constant     5.57106e+03   3.23771e+01   3.04749e-01   5.08917e-07
   2  Mean         3.75307e-04   8.13579e-08   9.98260e-10   1.09851e+04
   3  Sigma        1.82467e-05   6.86340e-08   1.14167e-05  -6.49054e-01


*/
