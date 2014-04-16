#include <fstream>
#include <iomanip>


void refractiveIndexes()
{
  TCanvas* c = new TCanvas();
  c -> SetGridx();
  c -> SetGridy();
  
  float xMin = 0.250;
  float xMax = 1.240;
  
  TF1* f_air   = new TF1("n_air","1+0.05792105/(238.0185-pow(x,-2))+0.00167917/(57.362-pow(x,-2))",0.,100.);
  TF1* f_YAG   = new TF1("n_YAG","sqrt(1+2.28200*pow(x,2)/(pow(x,2)-0.01185)+3.27644*pow(x,2)/(pow(x,2)-282.734))",0.,100.);
  TF1* f_SiO2  = new TF1("n_SiO2","sqrt(1+0.6961663*pow(x,2)/(pow(x,2)-pow(0.0684043,2))+0.4079426*pow(x,2)/(pow(x,2)-pow(0.1162414,2))+0.8974794*pow(x,2)/(pow(x,2)-pow(9.896161,2)))",0.,100.);
  TF1* f_LuAG  = new TF1("n_LuAG","sqrt( 1 + 1.47199*pow(x,2)/(pow(x,2)-0.00621359) + 0.845642*pow(x,2)/(pow(x,2)-0.0200432) + 3.82124*pow(x,2)/(pow(x,2)-330.483))",0.,100.);
        
  f_air  -> SetNpx(10000);
  f_YAG  -> SetNpx(10000);
  f_SiO2 -> SetNpx(10000);
  f_LuAG -> SetNpx(10000);

  f_air -> SetMinimum(0.9);
  f_air -> SetMaximum(2.1);
  f_air -> GetXaxis() -> SetRangeUser(xMin,xMax);
  f_air -> GetXaxis() -> SetTitle("#lambda  (um)");
  f_air -> GetYaxis() -> SetTitle("n");
  
  f_air  -> SetLineColor(kCyan);
  f_YAG  -> SetLineColor(kRed);
  f_SiO2 -> SetLineColor(kBlue);
  f_LuAG -> SetLineColor(kGreen);
  
  f_air  -> SetLineWidth(2);
  f_YAG  -> SetLineWidth(2);
  f_SiO2 -> SetLineWidth(2);
  f_LuAG -> SetLineWidth(2);
  
  f_air  -> Draw("");
  f_YAG  -> Draw("same");
  f_SiO2 -> Draw("same");
  f_LuAG -> Draw("same");
  
  
  TText t;
  t.SetTextSize(0.04);
  t.SetTextFont(42);
  t.SetTextAlign(22);
  int nBins = 10;
  float step = (xMax-xMin)/nBins;
  float yt = - f_air->GetMaximum()/5.;   
  for(int i=0; i<nBins+1; ++i) t.DrawText(xMin+i*step,2.13,Form("%.2f",1.23984187/(xMin+i*step)));
  
  
  std::vector<double> x;
  x.push_back(xMin); 
  while(1)
  {
    double val = x.at(x.size()-1);
    if( val+0.025 <= xMax ) x.push_back(val+0.025);
    else break;
  }
  x.push_back(1.23984187);
  x.push_back(12.3984187);
  

  PrintRefrInd(x,f_air,"refrInd_Air.dat");
  PrintRefrInd(x,f_SiO2,"refrInd_SiO2.dat");
  PrintRefrInd(x,f_YAG,"refrInd_YAG.dat");
  PrintRefrInd(x,f_LuAG,"refrInd_LuAG.dat");
}



void PrintRefrInd(const std::vector<double>& x, TF1* f, const std::string& name)
{
  std::ofstream outFile(name.c_str(),std::ios::out);
  
  outFile << "const G4int nEntries_RI = " << x.size() << ";" << std::endl;
  outFile << "G4double PhotonEnergy_RI[nEntries_RI] = " << std::endl;
  
  outFile << "{ ";
  int counter = 1;
  for(int it = x.size()-1; it >= 0; --it)
  {
    if( it > 0 )
      outFile << std::fixed << std::setprecision(4) << std::setw(6) << 1.23984187/x.at(it) << "*eV, ";
    else
      outFile << std::fixed << std::setprecision(4) << std::setw(6) << 1.23984187/x.at(it) << "*eV ";
    
    if( counter%4 == 0 )
    {
      outFile << std::endl;
      outFile << "  ";
    }
    ++counter;
  }
  outFile << "};" << std::endl;
  
  outFile << std::endl;
  outFile << std::endl;
  
  outFile << "G4double RefractiveIndex[nEntries_RI] = " << std::endl;
  
  outFile << "{ ";
  counter = 1;
  for(int it = x.size()-1; it >= 0; --it)
  {
    if( it > 0 && counter == 1 )
      outFile << std::fixed << std::setprecision(4) << std::setw(6) << f->Eval(x.at(it-1)) << ", ";
    else if( it > 0 && counter != 1 )
      outFile << std::fixed << std::setprecision(4) << std::setw(6) << f->Eval(x.at(it)) << "*, ";
    else
      outFile << std::fixed << std::setprecision(4) << std::setw(6) << f->Eval(x.at(it)) << " ";
    
    if( counter%4 == 0 )
    {
      outFile << std::endl;
      outFile << "  ";
    }
    ++counter;
  }
  outFile << "};" << std::endl;
    
  outFile.close();
}
