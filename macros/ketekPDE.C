void ketekPDE()
{
  TGraph* g = new TGraph("ketekPDE.txt");
  
  TGraph* g_nm = new TGraph();
  TGraph* g_eV = new TGraph();
  
  double EMin = 0.1;
  double EMax = 4.9594;
  double LMin = fromEvToNm(EMax);
  double LMax = fromEvToNm(EMin);
  
  
  g_nm -> SetPoint(0,0.,0.);
  g_eV -> SetPoint(0,-10.,0.);
  for(int point = 0; point < g->GetN(); ++point)
  {
    double x,y;
    g -> GetPoint(point,x,y);
    
    g_nm -> SetPoint(point+1,x,0.01*y);
    g_eV -> SetPoint(point+1,fromNmToEv(x),0.01*y);
  }
  g_nm -> SetPoint(g_nm->GetN(),20000,1.);
  g_eV -> SetPoint(g_eV->GetN(),  10.,1.);
    
  
  TCanvas* c_nm = new TCanvas("c_nm","nm");
  g_nm -> GetXaxis() -> SetRangeUser(LMin,LMax);
  g_nm -> GetYaxis() -> SetRangeUser(0.,0.5);
  g_nm -> GetXaxis() -> SetTitle("wavelength (nm)");
  g_nm -> GetYaxis() -> SetTitle("PDE");
  g_nm -> Draw("AP");
  
  TF1* f_nm = new TF1("f_nm",crystalBallHigh,0.,10000.,8);
  f_nm -> SetNpx(10000);
  f_nm -> SetParameters(0.32,428.,95.,0.4,100.,0.3,60.,0);
  f_nm -> FixParameter(5,0.3);
  f_nm -> FixParameter(7,0);
  g_nm -> Fit("f_nm","QNS+","",400.,700.);
  f_nm -> Draw("same");
  
  
  TCanvas* c_eV = new TCanvas("c_eV","eV");
  g_eV -> GetXaxis() -> SetRangeUser(EMin,EMax);
  g_eV -> GetYaxis() -> SetRangeUser(0.,0.5);
  g_eV -> GetXaxis() -> SetTitle("energy (eV)");
  g_eV -> GetYaxis() -> SetTitle("PDE");
  g_eV -> Draw("AP");  
  
  TF1* f_ev = new TF1("f_ev",crystalBallHigh,0.,5.,8);
  f_ev -> SetNpx(10000);
  f_ev -> SetParameters(0.32,428.,95.,0.4,100.,0.3,60.,1);
  for(int parIt = 0; parIt < 8; ++parIt) f_ev -> SetParameter(parIt,f_nm->GetParameter(parIt));
  f_ev -> FixParameter(7,1);
  f_ev -> Draw("same");
  
  
  
  std::cout << "PDE: " << f_ev->Integral(EMin,EMax)/(EMax-EMin) << std::endl;
}



double fromNmToEv(const double& wavelength)
{
  return 1239.84187 / wavelength;
}



double fromEvToNm(const double& energy)
{
  return 1239.84187 / energy;
}



/*** double crystall ball ***/
double crystalBallHigh(double* x, double* par)
{
  //[0] = N
  //[1] = mean
  //[2] = sigma
  //[3] = alpha
  //[4] = n
  //[5] = alpha2
  //[6] = sigma2
  //[7] = flag
  
  double xx = x[0];
  if( par[7] == 1 ) xx = fromNmToEv(xx);
  
  double mean = par[1];
  double sigma = par[2];
  double alpha = par[3];
  double n = par[4];
  double alpha2 = par[5];
  double sigma2 = par[6];
    
  if( (xx-mean)/sigma > fabs(alpha) )  
  {
    double A = pow(n/fabs(alpha), n) * exp(-0.5 * alpha*alpha);
    double B = n/fabs(alpha) - fabs(alpha);
    
    return par[0] * A * pow(B + (xx-mean)/sigma, -1.*n);
  }
  
  else if( (xx-mean)/sigma < -1.*fabs(alpha2) )
  {
    double A = exp( -0.5*alpha2*alpha2 * (1-sigma*sigma/sigma2/sigma2) );
    
    return par[0] * A * exp(-1. * (xx-mean)*(xx-mean) / (2*sigma2*sigma2) );
  }
  
  else
  {
    return par[0] * exp(-1. * (xx-mean)*(xx-mean) / (2*sigma*sigma) );
  } 
}
