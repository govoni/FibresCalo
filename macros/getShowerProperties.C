int getShowerProperties (TString inputFileName, float beamEnergy = 1., float moduleHalfWidth = 200) // GeV, mm
{
  TFile f (inputFileName) ;
  TNtuple * stepDeposits = (TNtuple *) f.Get ("stepDeposits") ;

  TH1F * h_E_vs_R = new TH1F ("h_E_vs_R", "h_E_vs_R", 300, 0., 60.) ;
  TH1F * h_E_vs_L = new TH1F ("h_E_vs_L", "h_E_vs_L", int (20 * moduleHalfWidth), 0., 2 * moduleHalfWidth) ;

  Float_t x, y, z, E ;
  stepDeposits->SetBranchAddress ("x", &x) ;
  stepDeposits->SetBranchAddress ("y", &y) ;
  stepDeposits->SetBranchAddress ("z", &z) ;
  stepDeposits->SetBranchAddress ("E", &E) ;

//  TH3F h ("h", "h", 100, -5, 5, 100, -5, 5, 1000, - 100, 100) ;
//  stepDeposits->Draw ("z:y:x>>h") ;

  TVector3 direction (0.0523, 0.0523, 0.9986) ;
  direction *= 1. / direction.Mag () ;

  //PG loop on the TNtuple
  for (int i = 0 ; i < stepDeposits->GetEntries () ; ++i)
    {
      if (i % 100000 == 0) cout << "event " << i << endl ;
      stepDeposits->GetEntry (i) ;
      TVector3 point (x, y, z + moduleHalfWidth) ;
      double projection = direction.Dot (point) ;
      double distance = sqrt (point.Mag2 () - projection * projection) ;
      h_E_vs_R->Fill (distance, E) ;
      h_E_vs_L->Fill (projection, E) ;
    }

  TH1F * longitudinalContainment = h_E_vs_L->Clone ("longitudinalContainment") ;
  longitudinalContainment->Reset () ;
  float tot = 0. ;
  for (int i = 1 ; i <= h_E_vs_L->GetNbinsX () ; ++i)
    {
      tot += h_E_vs_L->GetBinContent (i) ;
      longitudinalContainment->SetBinContent (i, tot / beamEnergy) ;
    }

  TFile * f_out = new TFile (TString ("summ_") + inputFileName, "recreate") ;
  f_out->cd () ;
  h_E_vs_R->Write () ;
  h_E_vs_L->Write () ;
  longitudinalContainment->Write () ;
  f_out->Close () ;

  return 0 ;
  

}