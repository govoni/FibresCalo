// for some reasons the first function does not work
TH1D * cumul (TH1D * input_h)
{
  TString name = input_h->GetName () ;
  name += "_cdf" ;
  TH1D * output_h = input_h->Clone (name) ;
  output_h->Reset () ;
  Double_t tot = 0. ;
  for (int i = 1 ; i <= input_h->GetNbinsX () ; ++i)
    {
      tot += input_h->GetBinContent (i) ;
      cout << i << " " <<  tot << endl ;
      output_h->SetBinContent (i, tot) ;
    }
  return output_h ;
}

double radius (TH1F * input_h, float total = 1.)
{
  Double_t cumul = 0. ;
  TAxis * xaxis = input_h->GetXaxis () ;
  for (int i = 1 ; i <= input_h->GetNbinsX () ; ++i)
    {
      cumul += input_h->GetBinContent (i) ;
      if ((cumul / total) > 0.9)
        {
          return xaxis->GetBinCenter (i) ;
        } 
    }
  return -1. ;
}