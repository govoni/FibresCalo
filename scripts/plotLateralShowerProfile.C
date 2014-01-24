.L scripts/cumul.C

TFile f_1 ("summ_test_RM_1GeV_abs.root")
TH1F * h_R_1 = (TH1F *) f_1->Get ("h_E_vs_R")
h_R_1->SetName ("h_R_1")
cout << "energy 1, R = " << radius (h_R_1) << endl ;
//h_R_1->Scale (1./1) 

TFile f_5 ("summ_test_RM_5GeV_abs.root")
TH1F * h_R_5 = (TH1F *) f_5->Get ("h_E_vs_R")
h_R_5->SetName ("h_R_5")
h_R_5->SetLineColor (kOrange)
cout << "energy 5, R = " << radius (h_R_5) << endl ;
//h_R_5->Scale (1./5) 

TFile f_10 ("summ_test_RM_10GeV_abs.root")
TH1F * h_R_10 = (TH1F *) f_10->Get ("h_E_vs_R")
h_R_10->SetName ("h_R_10")
h_R_10->SetLineColor (kGreen + 2)
cout << "energy 10, R = " << radius (h_R_10) << endl ;
//h_R_10->Scale (1./10) 

TFile f_30 ("summ_test_RM_30GeV_abs.root")
TH1F * h_R_30 = (TH1F *) f_30->Get ("h_E_vs_R")
h_R_30->SetName ("h_R_30")
h_R_30->SetLineColor (kBlue)
cout << "energy 30, R = " << radius (h_R_30) << endl ;
//h_R_30->Scale (1./30) 

TFile f_50 ("summ_test_RM_50GeV_abs.root")
TH1F * h_R_50 = (TH1F *) f_50->Get ("h_E_vs_R")
h_R_50->SetName ("h_R_50")
h_R_50->SetLineColor (kRed)
cout << "energy 50, R = " << radius (h_R_50) << endl ;
//h_R_50->Scale (1./50) 

h_R_50->SetStats (0)
h_R_50->SetTitle ("")
h_R_50->GetXaxis ()->SetTitle ("transverse direction (mm)") ;
h_R_50->GetYaxis ()->SetTitle ("energy deposit (GeV)") ;

h_R_50->Draw ()
h_R_50->Draw ("same")
h_R_30->Draw ("same")
h_R_10->Draw ("same")
h_R_5->Draw ("same")

leg = new TLegend (0.6, 0.6, 0.95, 0.95) ;
leg->AddEntry (h_R_1, "1 GeV","l") ;
leg->AddEntry (h_R_5, "5 GeV","l") ;
leg->AddEntry (h_R_10, "10 GeV","l") ;
leg->AddEntry (h_R_30, "30 GeV","l") ;
leg->AddEntry (h_R_50, "50 GeV","l") ;
leg->Draw () ;

h_R_1->Draw ("same")
