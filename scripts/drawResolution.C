

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
  TVirtualFitter::SetDefaultFitter ("Minuit2") ;
  TFile * f = new TFile (inputFile) ;

  // x, y deposit pattern drawings
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

  TH3F * h_ErecoOverEgen_vs_impact = (TH3F *) f->Get ("h_ErecoOverEgen_vs_impact") ;

  TProfile2D * proj_xy = h_ErecoOverEgen_vs_impact->Project3DProfile("xy") ;
  proj_xy->GetXaxis ()->SetTitle ("x") ;
  proj_xy->GetYaxis ()->SetTitle ("y") ;
  TCanvas * c_xy = new TCanvas () ;
  proj_xy->Draw ("colz") ;

  TH2D * proj_yz = h_ErecoOverEgen_vs_impact->Project3D ("zy") ;
  proj_yz->GetXaxis ()->SetTitle ("y") ;
  proj_yz->GetYaxis ()->SetTitle ("z") ;
  TCanvas * c_yz = new TCanvas () ;
  proj_yz->Draw ("colz") ;

  TH2D * proj_xz = h_ErecoOverEgen_vs_impact->Project3D ("zx") ;
  proj_xz->GetXaxis ()->SetTitle ("x") ;
  proj_xz->GetYaxis ()->SetTitle ("z") ;
  TCanvas * c_xz = new TCanvas () ;
  proj_xz->Draw ("colz") ;

  // get the resolution trend
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
  
  TH2F * sigmaEoverE_vs_Ebeam = (TH2F *) f->Get ("sigmaEoverE_vs_Ebeam") ;

  sigmaEoverE_vs_Ebeam->SetStats (0) ;
  sigmaEoverE_vs_Ebeam->GetXaxis ()->SetTitle ("E_{beam}") ;
  sigmaEoverE_vs_Ebeam->GetYaxis ()->SetTitle ("#sigma_{E} / E") ;
  sigmaEoverE_vs_Ebeam->FitSlicesY () ;

  TH1D * mean = (TH1D *) gDirectory->Get ("sigmaEoverE_vs_Ebeam_1") ;
  mean->SetMarkerStyle (4) ;
  mean->GetYaxis ()->SetTitle ("bias / E") ;

  TH1D * sigma = (TH1D *) gDirectory->Get ("sigmaEoverE_vs_Ebeam_2") ;
  sigma->SetMarkerStyle (4) ;
  sigma->GetYaxis ()->SetTitle ("#sigma_{E} / E") ;
  sigma->Draw () ;

  TCanvas * c0 = new TCanvas () ;
  sigmaEoverE_vs_Ebeam->Draw ("colz") ;
  c0->Print ("resolution_raw.pdf", "pdf") ;

  gStyle->SetOptStat (0000000) ;
  gStyle->SetOptFit (1111) ;

  TF1 * resolution = new TF1 ("resolution", resol , 0, 200., 3) ;
  resolution->SetLineColor (kBlack) ;
  resolution->SetParLimits (0, 0., 100.) ;  
  resolution->SetParLimits (1, 0., 100.) ;  
  resolution->SetParLimits (2, 0., 2. * sigma->GetBinContent (sigma->GetNbinsX () - 1 )) ;  
  resolution->SetParameter (2, sigma->GetBinContent (sigma->GetNbinsX () - 1 )) ;
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
  
  TCanvas * c1 = new TCanvas () ;
  c1->DrawFrame (0, 0, 80, 0.1) ;

  sigma->Draw ("sameS") ;
  resolution->Draw ("same") ;
  noise->Draw ("same") ;
  stochastic->Draw ("same") ;
  constant->Draw ("same") ;
  
  TLegend * leg = new TLegend (0.28,0.6,0.6,0.94) ;
  leg->SetFillColor (kWhite) ;
  leg->AddEntry (resolution, "resolution","l") ;
  leg->AddEntry (constant, "constant term","l") ;
  leg->AddEntry (noise, "noise term","l") ;
  leg->AddEntry (stochastic, "stochastic term","l") ;
  leg->Draw();

  c1->Print ("resolution_fit.pdf", "pdf") ;

  TCanvas * c2 = new TCanvas () ;
  mean->Draw () ;
  c2->Print ("bias.pdf", "pdf") ;
}  

