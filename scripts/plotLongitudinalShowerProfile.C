TFile * f_100 = new TFile ("summ_test_RM_100GeV_fibre.root")
TH1F * LC_100 = (TH1F *) f_100->Get ("longitudinalContainment")
LC_100->SetName ("LC_100")
LC_100->SetTitle ("")
LC_100->SetLineColor (kGreen + 2)
LC_100->Scale (1. / 50)

TFile * f_50 = new TFile ("summ_test_RM_50GeV_fibre.root")
TH1F * LC_50 = (TH1F *) f_50->Get ("longitudinalContainment")
LC_50->SetName ("LC_50")
LC_50->SetTitle ("")
LC_50->SetLineColor (kBlue)
LC_50->Scale (1. / 100)

TFile * f_30 = new TFile ("summ_test_RM_30GeV_fibre.root")
TH1F * LC_30 = (TH1F *) f_30->Get ("longitudinalContainment")
LC_30->SetName ("LC_30")
LC_30->SetTitle ("")
LC_30->SetLineColor (kRed)
LC_30->Scale (1. / 100)

TCanvas * c1 = new TCanvas ()
TH1F * fr = (TH1F *) c1->DrawFrame (0,0,400,1.05)
fr->GetXaxis ()->SetTitle ("shower depth (mm)")
fr->GetYaxis ()->SetTitle ("E_{dep} / E_{beam}")
LC_30->Draw ("same")
LC_50->Draw ("same")
LC_100->Draw ("same")

TLegend * leg = new TLegend (0.5,0.1,0.9,0.4) ;
leg->SetFillColor (kWhite) ;
leg->AddEntry (LC_30, "30 GeV","l") ;
leg->AddEntry (LC_50, "50 GeV","l") ;
leg->AddEntry (LC_100, "100 GeV","l") ;
leg->Draw();

