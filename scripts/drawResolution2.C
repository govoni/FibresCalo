

float trivialRounding (float val) 
  {
    int result = int (val * 1000) ;
    return result * 0.1 ;
  }


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


Double_t resol (Double_t * x, Double_t * par)
{
  if (x[0] == 0) return 0. ;
  float result = par[0] * par[0] / (x[0] * x[0]) + // noise
                 par[1] * par[1] / x[0] +          // stochastic
                 par[2] * par[2] ;                 // constant
  return sqrt (result) ;                 
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


float addPointToGraph (TH1F * histo, TGraphErrors * meanG, TGraphErrors * sigmaG, float energy)
{
  TString plotname = histo->GetName () ;
  plotname += "_can" ;
  TCanvas * c = new TCanvas (plotname, plotname, 400, 400) ;
  histo->Fit ("gaus") ;
  plotname += ".pdf" ;
  
  float meanV = histo->GetFunction ("gaus")->GetParameter (1) ;
  float meanE = histo->GetFunction ("gaus")->GetParError (1) ;

  int pos = meanG->GetN () ;
  meanG->SetPoint (pos, energy, meanV) ;
  meanG->SetPointError (pos, 0., meanE) ;
  
  float sigmaV = histo->GetFunction ("gaus")->GetParameter (2) ;
  float sigmaE = histo->GetFunction ("gaus")->GetParError (2) ;
  
  int pos = sigmaG->GetN () ;
  sigmaG->SetPoint (pos, energy, sigmaV) ;
  sigmaG->SetPointError (pos, 0., sigmaE) ;

  histo->GetXaxis ()->SetRangeUser (meanV - 4 * sigmaV, meanV + 4 * sigmaV) ;
  histo->Draw () ;
  c.Print (plotname, "pdf") ;
  
  return sigmaV ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int drawResolution2 ()
{
  TFile * f_10 = new TFile ("out_job_fix_10_0.root") ;
  TH1F * h_10 = (TH1F *) f_10->Get ("sigmaEoverE") ;
  h_10->SetName ("resol_10") ;
  TFile * f_30 = new TFile ("out_job_fix_30_0.root") ;
  TH1F * h_30 = (TH1F *) f_30->Get ("sigmaEoverE") ;
  h_30->SetName ("resol_30") ;
  TFile * f_50 = new TFile ("out_job_fix_50_0.root") ;
  TH1F * h_50 = (TH1F *) f_50->Get ("sigmaEoverE") ;
  h_50->SetName ("resol_50") ;
  TFile * f_80 = new TFile ("out_job_fix_80_0.root") ;
  TH1F * h_80 = (TH1F *) f_80->Get ("sigmaEoverE") ;
  h_80->SetName ("resol_80") ;
  
  TGraphErrors * meanG = new TGraphErrors () ; 
  TGraphErrors * sigmaG = new TGraphErrors () ; 
  addPointToGraph (h_10, meanG, sigmaG, 10) ;
  addPointToGraph (h_30, meanG, sigmaG, 30) ;
  addPointToGraph (h_50, meanG, sigmaG, 50) ;
  float initConstTerm = addPointToGraph (h_80, meanG, sigmaG, 80) ;
  
  sigmaG->SetMarkerStyle (8) ;
  
  TF1 * resolution = new TF1 ("resolution", resol , 0, 200., 3) ;
  resolution->SetLineColor (kBlack) ;
  resolution->SetParLimits (0, 0., 100.) ;  
  resolution->FixParameter (0, 0.) ;  
  resolution->SetParLimits (1, 0., 100.) ;
  resolution->SetParLimits (2, 0., 2. * initConstTerm) ;  
  resolution->SetParameter (2, initConstTerm) ;
  sigmaG->Fit (resolution) ;
  
  TF1 * noise = new TF1 ("noise", "[0] / x" ,0, 200.) ;
  float noise_val = resolution->GetParameter (0) ;
  noise->SetParameter (0, noise_val) ;
  noise->SetLineColor (kRed) ;
  TF1 * stochastic = new TF1 ("stochastic", "[0] / TMath::Sqrt(x)" ,0, 200.) ;
  float stochastic_val = resolution->GetParameter (1) ;
  stochastic->SetParameter (0, stochastic_val) ;
  stochastic->SetLineColor (kBlue + 1) ;
  TF1 * constant = new TF1 ("constant", "[0]" ,0, 200.) ;
  float constant_val = resolution->GetParameter (2) ;
  constant->SetParameter (0, constant_val) ;
  constant->SetLineColor (kGreen + 2) ;
  
  TCanvas * c1 = new TCanvas () ;
  TH1F * bkg = c1->DrawFrame (0, 0, 150, 0.15) ;
  bkg->GetXaxis ()->SetTitle ("electron energy (GeV)") ;
  bkg->GetYaxis ()->SetTitle ("energy resolution") ;
  
  sigmaG->Draw ("P") ;
  resolution->Draw ("same") ;
  noise->Draw ("same") ;
  stochastic->Draw ("same") ;
  constant->Draw ("same") ;

  TString name_constant   = "constant term "   ; name_constant   += trivialRounding (constant_val) ;
  TString name_noise      = "noise term "      ; name_noise      += trivialRounding (noise_val) ;
  TString name_stochastic = "stochastic term " ; name_stochastic += trivialRounding (stochastic_val) ;
  
  TLegend * leg = new TLegend (0.28,0.6,0.9,0.94) ;
  leg->SetFillColor (kWhite) ;
  leg->AddEntry (resolution, "resolution"    ,"l") ;
  leg->AddEntry (constant,   name_constant   ,"l") ;
  leg->AddEntry (noise,      name_noise      ,"l") ;
  leg->AddEntry (stochastic, name_stochastic ,"l") ;
  leg->Draw () ;

  c1->Print ("resolution_fit.pdf", "pdf") ;
  


}