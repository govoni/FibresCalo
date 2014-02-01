
TRandom3 * gRandom ;



// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


float distance (float x1, float y1, float x2, float y2)
{
  return sqrt (x1 * x1 + y1 * y1 + x2 * x2 + y2 * y2) ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


// each tower is assigned its geometrical center as position
std::pair<float, float>
getTowerPosition (int index, int totNumOfTowersPerLine, float towerSideLength)
{
  int ix = index % totNumOfTowersPerLine ;
  int iy = index / totNumOfTowersPerLine ;
  float totalSideLength = towerSideLength * totNumOfTowersPerLine ;
  float x = (ix + 0.5) * towerSideLength - 0.5 * totalSideLength ;
  float y = (iy + 0.5) * towerSideLength - 0.5 * totalSideLength ;
  return std::pair<float, float> (x, y) ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


// LY = photons per MeV in the fibre
float smearEnergy (float energy, float LY = 10000, 
                   float acceptance = 0.1, float efficiency = 0.1)
{
  LY *= 1000 ; // 1000 MeV = 1 GeV
  int nPh = floor (energy * acceptance * efficiency * LY) ;
  float result = gRandom->Poisson (nPh) ;
  return result / LY / acceptance / efficiency ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void plotResolution (TString inputFile)
{
   // init tree and basic vars
   // ---- ---- ---- ---- ---- ---- ----

   int fNtowersOnSide = 49 ;
   float maxEnergy = 1. ; // gev
   float towerSideLength = 2 ; // mm
   float totalSideLength = towerSideLength * fNtowersOnSide ;
   int fNtowersOnSideSQ = fNtowersOnSide * fNtowersOnSide ;

   float LY = 10000000 ; // photons per GeV 
   float acceptance = 0.1 ;
   float efficiency = 0.1 ;

   TFile * f = new TFile (inputFile) ;
   TTree * t = (TTree *) f->Get ("tree") ;
   
   // prepare input variables
   // ---- ---- ---- ---- ---- ---- ----

   double depositedEnergy ;
   t->Branch ("depositedEnergy", &depositedEnergy, "depositedEnergy/D") ;
   std::vector<float> * depositedEnergies = new std::vector<float> (fNtowersOnSideSQ) ; 
   t->SetBranchAddress ("depositedEnergies", &depositedEnergies) ;
   std::vector<float> * totalEnergies = new std::vector<float> (fNtowersOnSideSQ) ; 
   t->SetBranchAddress ("totalEnergies", &totalEnergies) ;
   std::vector<float> * inputMomentum = new std::vector<float> (4) ; 
   t->SetBranchAddress ("inputMomentum", &inputMomentum) ;
   std::vector<float> * inputInitialPosition = new std::vector<float> (3) ; 
   t->SetBranchAddress ("inputInitialPosition", &inputInitialPosition) ;

   // output histograms
   // ---- ---- ---- ---- ---- ---- ----   
  
   // distributions for the energy response and calibration
   // .... .... .... .... .... .... ....   
   
   TH1F h_ErecoOverEgen ("h_ErecoOverEgen", "ErecoOverEgen", 400, 0, 0.2) ;
   TH2F h_ErecoOverEgen_vs_Ebeam ("h_ErecoOverEgen_vs_Ebeam", "h_ErecoOverEgen_vs_Ebeam", 
                                   45, 5, 50, 200, 0, 0.002) ;
   TH3F h_ErecoOverEgen_vs_impact ("h_ErecoOverEgen_vs_impact", "h_ErecoOverEgen_vs_impact", 
                                   40, -2., 2., 40, -2., 2., 400, 0, 0.2) ;

   // distributions for the energy containment
   // .... .... .... .... .... .... ....   

   // total deposited energy in the calorimeter, with the poissonian smearing
   TH1F h_EdepOverEgen ("h_EdepOverEgen", "EdepOverEgen", 100, 0, 1) ;
   TH2F h_EdepOverEgen_vs_Ebeam ("h_EdepOverEgen_vs_Ebeam", "h_EdepOverEgen_vs_Ebeam", 
                                  45, 5, 50, 100, 0, 1) ;
   TH3F h_EdepOverEgen_vs_impact ("h_EdepOverEgen_vs_impact", "h_EdepOverEgen_vs_impact", 
                                  40, -2., 2., 40, -2., 2., 400, 0, 0.002) ;

   // distributions for the energy resolution
   // .... .... .... .... .... .... ....   

   // energy resolution, with the poissonian smearing
   TH1F h_sigmaEoverE ("sigmaEoverE", "sigmaEoverE", 100, -1., 1.) ;
   TH2F h_sigmaEoverE_vs_Ebeam ("sigmaEoverE_vs_Ebeam", "sigmaEoverE_vs_Ebeam", 
                                 45, 5, 50, 100, -1., 1.) ;

   // distributions for the Moliere radius
   // .... .... .... .... .... .... ....   

   TH2F h_Edep_vs_R ("h_Edep_vs_R", "h_Edep_vs_R", 150, 0., 15., 500, 0., 50.) ;   
   TH3F h_Edep_vs_impact ("h_Edep_vs_impact", "h_Edep_vs_impact", 
                          100, -5., 5., 100, -5., 5., 500, 0., 50.) ;   

   // events analysis
   // ---- ---- ---- ---- ---- ---- ----   

   Int_t nentries = tree->GetEntries () ;
   cout << "found " << nentries << " events\n" ;

   float x = 0. ; 
   float y = 0. ;

   for (Int_t iEntry = 0 ; iEntry < nentries ; ++iEntry) 
     {
        if (iEntry % 100 == 0) cout << "reading event " << iEntry << endl ;
        t->GetEvent (iEntry) ;
        float observedEnergy = 0. ;
        float fullEnergy = 0. ;
        //PG loop over fibres
        // loop on the towers composing the calorimeter
        for (int j = 0 ; j < fNtowersOnSideSQ ; ++j)
          {
// removing the poisson smearing to check the stochastic term behaviour w/o that          
//            float result = gRandom->Poisson (floor (
//                            depositedEnergies->at (j) * acceptance * efficiency * LY)) ;
//            result /= LY * acceptance * efficiency ;
//            observedEnergy += result ;
            observedEnergy += depositedEnergies->at (j) ;
            fullEnergy += totalEnergies->at (j) ;
            y = (j % fNtowersOnSide + 0.5) * towerSideLength - 0.5 * totalSideLength ;
            x = (j / fNtowersOncSide + 0.5) * towerSideLength - 0.5 * totalSideLength ;
            float radius = sqrt (
                (x - inputInitialPosition->at (0)) * (x - inputInitialPosition->at (0)) +
                (y - inputInitialPosition->at (1)) * (y - inputInitialPosition->at (1))
              ) ;
            h_Edep_vs_R.Fill (radius, totalEnergies->at (j)) ;
            h_Edep_vs_impact.Fill (
                x - inputInitialPosition->at (0),
                y - inputInitialPosition->at (1), 
                totalEnergies->at (j)) ;
          } // loop on the towers composing the calorimeter
          
        float beamEnergy = inputMomentum->at (3) ;
        float ErecoOverEgen = observedEnergy / beamEnergy ;
        h_ErecoOverEgen.Fill (ErecoOverEgen) ;
        h_ErecoOverEgen_vs_Ebeam.Fill (beamEnergy, ErecoOverEgen) ;
        h_ErecoOverEgen_vs_impact.Fill (inputInitialPosition->at (0),
            inputInitialPosition->at (1), ErecoOverEgen) ;  
        float EdepOverEgen = fullEnergy / beamEnergy ;
        h_EdepOverEgen.Fill (EdepOverEgen) ;
        h_EdepOverEgen_vs_Ebeam.Fill (beamEnergy, EdepOverEgen) ;
        h_EdepOverEgen_vs_impact.Fill (inputInitialPosition->at (0),
            inputInitialPosition->at (1), EdepOverEgen) ;  

//        if (EdepOverEgen < 0.7) continue ;

        // 1/3.75307e-02 = 26.6448534133 W, 3 degrees tilt
//        float sigmaEoverE = (observedEnergy * 26.6448534133 - beamEnergy) / beamEnergy ;
        float sigmaEoverE = (observedEnergy * 18.9533936051 - beamEnergy) / beamEnergy ;
        //1/5.27610e-02 = 18.9533936051 Pb, 3 degrees tilt
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
   h_Edep_vs_R.Write () ;
   h_Edep_vs_impact.Write () ;
   outfile.Close () ;

   return ;
}

/*
NB THESE FITS ARE OLD, SINCE ACCEPTANCE AND EFFICIENCY WERE NOT MULTIPLIED
BACK IN THE CALCULATION OF THE ENERGY W/ THE POISSON SMEARING
 
Calibration factor for the small beam hitting in a perpendicular manner
the tungsten, at the crossing of four towers
  EXT PARAMETER                                   STEP         FIRST   
  NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE 
   1  Constant     1.25579e+03   2.02423e+01   1.16628e-01   1.83615e-06
   2  Mean         3.52692e-04   4.27726e-07   2.29938e-09   9.59803e+01
   3  Sigma        2.13923e-05   3.38120e-07   2.51361e-05  -3.62594e-03


Calibration factor for the large beam hitting a tower surface, 
with 3 deg tilt in x and y
  EXT PARAMETER                                   STEP         FIRST   
  NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE 
   1  Constant     5.57106e+03   3.23771e+01   3.04749e-01   5.08917e-07
   2  Mean         3.75307e-04   8.13579e-08   9.98260e-10   1.09851e+04
   3  Sigma        1.82467e-05   6.86340e-08   1.14167e-05  -6.49054e-01

Calibration factor for the Pb absorber
  EXT PARAMETER                                   STEP         FIRST   
  NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE 
   1  Constant     6.12723e+03   3.29126e+01   3.00227e-01  -2.30550e-05
   2  Mean         5.27610e-02   7.92208e-06   9.38113e-08   8.05117e+01
   3  Sigma        1.91448e-03   6.58913e-06   1.01739e-05  -1.54064e-01


*/
