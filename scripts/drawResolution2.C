

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


float addPointToGraph (TH1F * histo, TGraphErrors * meanG, TGraphErrors * sigmaG, float energy, TString basename)
{
  TString plotname = histo->GetName () ;
  plotname += "_can" ;
  TCanvas * c = new TCanvas (plotname, plotname, 400, 400) ;
  histo->Fit ("gaus") ;
  plotname += ".png" ;
  
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
  c->Print (basename + plotname, "png") ;
  
  return sigmaV ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int drawResolution2 (TString basename)
{
  TFile * f_10 = new TFile (basename + TString ("10_0.root")) ;
  TH1F * h_10 = (TH1F *) f_10->Get ("sigmaEoverE") ;
  h_10->SetName ("resol_10") ;
  TFile * f_30 = new TFile (basename + TString ("30_0.root")) ;
  TH1F * h_30 = (TH1F *) f_30->Get ("sigmaEoverE") ;
  h_30->SetName ("resol_30") ;
  TFile * f_50 = new TFile (basename + TString ("50_0.root")) ;
  TH1F * h_50 = (TH1F *) f_50->Get ("sigmaEoverE") ;
  h_50->SetName ("resol_50") ;
  TFile * f_80 = new TFile (basename + TString ("80_0.root")) ;
  TH1F * h_80 = (TH1F *) f_80->Get ("sigmaEoverE") ;
  h_80->SetName ("resol_80") ;
  TFile * f_120 = new TFile (basename + TString ("120_0.root")) ;
  TH1F * h_120 = (TH1F *) f_120->Get ("sigmaEoverE") ;
  h_120->SetName ("resol_120") ;
  TFile * f_160 = new TFile (basename + TString ("160_0.root")) ;
  TH1F * h_160 = (TH1F *) f_160->Get ("sigmaEoverE") ;
  h_160->SetName ("resol_160") ;
  TFile * f_250 = new TFile (basename + TString ("250_0.root")) ;
  TH1F * h_250 = (TH1F *) f_250->Get ("sigmaEoverE") ;
  h_250->SetName ("resol_250") ;
  
  TGraphErrors * meanG = new TGraphErrors () ; 
  TGraphErrors * sigmaG = new TGraphErrors () ; 
  float initConstTerm = 0.02 ;
  initConstTerm = addPointToGraph (h_10, meanG, sigmaG, 10, basename) ;
  initConstTerm = addPointToGraph (h_30, meanG, sigmaG, 30, basename) ;
  initConstTerm = addPointToGraph (h_50, meanG, sigmaG, 50, basename) ;
  initConstTerm = addPointToGraph (h_80, meanG, sigmaG, 80, basename) ;
  initConstTerm = addPointToGraph (h_120, meanG, sigmaG, 120, basename) ;
  initConstTerm = addPointToGraph (h_160, meanG, sigmaG, 160, basename) ;
  initConstTerm = addPointToGraph (h_250, meanG, sigmaG, 250, basename) ;
  
  sigmaG->SetMarkerStyle (8) ;
  
  TF1 * resolution = new TF1 ("resolution", resol , 0, 300., 3) ;
  resolution->SetLineColor (kBlack) ;
  resolution->SetParLimits (0, 0., 100.) ;  
  resolution->FixParameter (0, 0.) ;  
  resolution->SetParLimits (1, 0., 100.) ;
  resolution->SetParLimits (2, 0., 2. * initConstTerm) ;  
  resolution->SetParameter (2, initConstTerm) ;
  sigmaG->Fit (resolution) ;
  
  TF1 * noise = new TF1 ("noise", "[0] / x" ,0, 300.) ;
  float noise_val = resolution->GetParameter (0) ;
  noise->SetParameter (0, noise_val) ;
  noise->SetLineColor (kRed) ;
  TF1 * stochastic = new TF1 ("stochastic", "[0] / TMath::Sqrt(x)" ,0, 300.) ;
  float stochastic_val = resolution->GetParameter (1) ;
  stochastic->SetParameter (0, stochastic_val) ;
  stochastic->SetLineColor (kBlue + 1) ;
  TF1 * constant = new TF1 ("constant", "[0]" ,0, 300.) ;
  float constant_val = resolution->GetParameter (2) ;
  constant->SetParameter (0, constant_val) ;
  constant->SetLineColor (kGreen + 2) ;
  
  TCanvas * c1 = new TCanvas () ;
  TH1F * bkg = c1->DrawFrame (0, 0, 300, 0.15) ;
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

  c1->Print (basename + "resolution_fit.png", "png") ;
  


}