

Double_t resol (Double_t * x, Double_t * par)
{
  if (x[0] == 0) return 0. ;
  float result = par[0] * par[0] / (x[0] * x[0]) + // noise
                 par[1] * par[1] / x[0] +          // stochastic
                 par[2] * par[2] ;                 // constant
  return sqrt (result) ;                 
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int drawResolution (TString inputFile)
{
  TFile * f = new TFile (inputFile) ;
  TH2F * sigmaEoverE_vs_Ebeam = (TH2F *) f->Get ("sigmaEoverE_vs_Ebeam") ;

  sigmaEoverE_vs_Ebeam->SetStats (0) ;
  sigmaEoverE_vs_Ebeam->GetXaxis ()->SetTitle ("E_{beam}") ;
  sigmaEoverE_vs_Ebeam->GetYaxis ()->SetTitle ("#sigma_{E} / E") ;
  sigmaEoverE_vs_Ebeam->FitSlicesY () ;
  TH1D * mean = (TH1D *) gDirectory->Get ("sigmaEoverE_vs_Ebeam_1") ;
  mean->SetStats (0) ;
  mean->SetMarkerColor (kRed) ;
  mean->SetMarkerStyle (8) ;

  TCanvas * c1 = new TCanvas ;
  sigmaEoverE_vs_Ebeam->Draw ("colz") ;
  mean->Draw ("same") ;
  c1->Print ("response.pdf", "pdf") ;
  
  TH1D * sigma = (TH1D *) gDirectory->Get ("sigmaEoverE_vs_Ebeam_2") ;
  gStyle->SetOptStat (0000000) ;
  gStyle->SetOptFit (1111) ;
  sigma->SetMarkerStyle (4) ;

  TF1 * resolution = new TF1 ("resolution", resol ,0, 200., 3) ;
  sigma->Fit (resolution) ;
  TF1 * noise = new TF1 ("noise", "[0] / x" ,0, 200.) ;
  noise->SetParameter (0, resolution->GetParameter (0)) ;
  noise->SetLineColor (kRed) ;
  TF1 * stochastic = new TF1 ("stochastic", "[0] / TMath::Sqrt(x)" ,0, 200.) ;
  stochastic->SetParameter (0, resolution->GetParameter (1)) ;
  stochastic->SetLineColor (kBlue + 1) ;
  TF1 * constant = new TF1 ("constant", "[0]" ,0, 200.) ;
  constant->SetParameter (0, resolution->GetParameter (2)) ;
  constant->SetLineColor (kGreen + 2) ;
  
  sigma->Draw () ;
  resolution->Draw ("same") ;
  noise->Draw ("same") ;
  stochastic->Draw ("same") ;
  constant->Draw ("same") ;
  
  TLegend * leg = new TLegend (0.12,0.12,0.4,0.36) ;
  leg->SetFillStyle (0) ;
  leg->AddEntry (resolution, "resolution","l") ;
  leg->AddEntry (constant, "constant term","l") ;
  leg->AddEntry (noise, "noise term","l") ;
  leg->AddEntry (stochastic, "stochastic term","l") ;
  leg->Draw();

  
  c1->Print ("resolution.pdf", "pdf") ;

}  

