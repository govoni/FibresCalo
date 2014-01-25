
TFile f_1 ("summ_test_RM_1GeV_abs.root")
TH1F * h_L_1 = (TH1F *) f_1->Get ("h_E_vs_L")
h_L_1->SetName ("h_L_1")
//h_L_1->Scale (1./1) 

TFile f_5 ("summ_test_RM_5GeV_abs.root")
TH1F * h_L_5 = (TH1F *) f_5->Get ("h_E_vs_L")
h_L_5->SetName ("h_L_5")
h_L_5->SetLineColor (kOrange)
//h_L_5->Scale (1./5) 

TFile f_10 ("summ_test_RM_10GeV_abs.root")
TH1F * h_L_10 = (TH1F *) f_10->Get ("h_E_vs_L")
h_L_10->SetName ("h_L_10")
h_L_10->SetLineColor (kGreen + 2)
//h_L_10->Scale (1./10) 

TFile f_30 ("summ_test_RM_30GeV_abs.root")
TH1F * h_L_30 = (TH1F *) f_30->Get ("h_E_vs_L")
h_L_30->SetName ("h_L_30")
h_L_30->SetLineColor (kBlue)
//h_L_30->Scale (1./30) 

TFile f_50 ("summ_test_RM_50GeV_abs.root")
TH1F * h_L_50 = (TH1F *) f_50->Get ("h_E_vs_L")
h_L_50->SetName ("h_L_50")
h_L_50->SetLineColor (kRed)
//h_L_50->Scale (1./50) 

h_L_50->SetStats (0)
h_L_50->SetTitle ("")
h_L_50->GetXaxis ()->SetTitle ("longitudinal direction (mm)") ;
h_L_50->GetYaxis ()->SetTitle ("energy deposit (GeV)") ;

h_L_50->Rebin (20)
h_L_30->Rebin (20)
h_L_20->Rebin (20)
h_L_10->Rebin (20)
h_L_5->Rebin (20)
h_L_1->Rebin (20)

h_L_50->DrawNormalized ()
h_L_50->DrawNormalized ("same")
h_L_30->DrawNormalized ("same")
h_L_10->DrawNormalized ("same")
h_L_5->DrawNormalized ("same")
h_L_1->DrawNormalized ("same")

leg = new TLegend (0.6, 0.6, 0.95, 0.95) ;
leg->AddEntry (h_L_1, "1 GeV","l") ;
leg->AddEntry (h_L_5, "5 GeV","l") ;
leg->AddEntry (h_L_10, "10 GeV","l") ;
leg->AddEntry (h_L_30, "30 GeV","l") ;
leg->AddEntry (h_L_50, "50 GeV","l") ;
leg->Draw () ;

