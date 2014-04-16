
.L scripts/cumul.C

TFile * f_Pb_50_large = new TFile ("summ_test_RM_50GeV_Pb_large.root") ;
TH1F * RP_Pb_50_large = (TH1F *) f_Pb_50_large->Get ("h_E_vs_R") ;

TFile * f_W_50_large = new TFile ("summ_test_RM_50GeV_W_large.root") ;
TH1F * RP_W_50_large = (TH1F *) f_W_50_large->Get ("h_E_vs_R") ;

RP_W_50_large->Scale (1./(50*50)) ;
RP_Pb_50_large->Scale (1./(50*50)) ;

TCanvas * c2 = new TCanvas ("c2","c2",700, 700) ;
c2->SetLogy () ;
TH1F * fr_c2 = (TH1F *) c2->DrawFrame (0, 0.0001, 40, 0.07) ;
fr_c2->GetXaxis ()->SetTitle ("shower radius (mm)") ;
fr_c2->GetYaxis ()->SetTitle ("1/ E_{beam} #times dE_{dep}/dx") ;

RP_W_50_large->SetLineColor (kBlue) ;

RP_W_50_large->Draw ("same") ;
RP_Pb_50_large->Draw ("same") ;

TLegend * leg = new TLegend (0.5,0.75,0.9,0.9) ;
leg->SetFillColor (kWhite) ;
leg->AddEntry (RP_W_50_large, "W 50 GeV","l") ;
leg->AddEntry (RP_Pb_50_large, "Pb 50 GeV","l") ;
leg->Draw () ;

cout << "Moliere R W = " << radius (RP_W_50_large) << endl   ;
cout << "Moliere R Pb = " << radius (RP_Pb_50_large) << endl   ;





