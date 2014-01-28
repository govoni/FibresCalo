TFile * f_W_100 = new TFile ("summ_test_RM_100GeV_W.root")
TH1F * LC_W_100 = (TH1F *) f_W_100->Get ("longitudinalContainment")
LC_W_100->SetName ("LC_W_100")
LC_W_100->SetTitle ("")
LC_W_100->SetLineColor (kGreen + 2)
LC_W_100->Scale (1. / 50)

TFile * f_W_50 = new TFile ("summ_test_RM_50GeV_W.root")
TH1F * LC_W_50 = (TH1F *) f_W_50->Get ("longitudinalContainment")
LC_W_50->SetName ("LC_W_50")
LC_W_50->SetTitle ("")
LC_W_50->SetLineColor (kBlue)
LC_W_50->Scale (1. / 100)

TFile * f_W_30 = new TFile ("summ_test_RM_30GeV_W.root")
TH1F * LC_W_30 = (TH1F *) f_W_30->Get ("longitudinalContainment")
LC_W_30->SetName ("LC_W_30")
LC_W_30->SetTitle ("")
LC_W_30->SetLineColor (kRed)
LC_W_30->Scale (1. / 100)

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

TFile * f_Pb_100 = new TFile ("summ_test_RM_100GeV_Pb.root")
TH1F * LC_Pb_100 = (TH1F *) f_Pb_100->Get ("longitudinalContainment")
LC_Pb_100->SetName ("LC_Pb_100")
LC_Pb_100->SetTitle ("")
LC_Pb_100->SetLineColor (kGreen + 2)
LC_Pb_100->SetLineStyle (2)
LC_Pb_100->Scale (1. / 50)

TFile * f_Pb_50 = new TFile ("summ_test_RM_50GeV_Pb.root")
TH1F * LC_Pb_50 = (TH1F *) f_Pb_50->Get ("longitudinalContainment")
LC_Pb_50->SetName ("LC_Pb_50")
LC_Pb_50->SetTitle ("")
LC_Pb_50->SetLineColor (kBlue)
LC_Pb_50->SetLineStyle (2)
LC_Pb_50->Scale (1. / 50)

TFile * f_Pb_30 = new TFile ("summ_test_RM_30GeV_Pb.root")
TH1F * LC_Pb_30 = (TH1F *) f_Pb_30->Get ("longitudinalContainment")
LC_Pb_30->SetName ("LC_Pb_30")
LC_Pb_30->SetTitle ("")
LC_Pb_30->SetLineColor (kRed)
LC_Pb_30->SetLineStyle (2)
LC_Pb_30->Scale (1. / 50)

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

TCanvas * c1 = new TCanvas ("c1","c1",700, 700)
TH1F * fr_c1 = (TH1F *) c1->DrawFrame (0,0,400,1.05)
fr_c1->GetXaxis ()->SetTitle ("shower depth (mm)")
fr_c1->GetYaxis ()->SetTitle ("E_{dep} / E_{beam}")

LC_W_30->Draw ("same")
LC_W_50->Draw ("same")
LC_W_100->Draw ("same")

LC_Pb_30->Draw ("same")
LC_Pb_50->Draw ("same")
LC_Pb_100->Draw ("same")


TLegend * leg = new TLegend (0.5,0.1,0.9,0.4) ;
leg->SetFillColor (kWhite) ;
leg->AddEntry (LC_W_30, "W 30 GeV","l") ;
leg->AddEntry (LC_W_50, "W 50 GeV","l") ;
leg->AddEntry (LC_W_100, "W 100 GeV","l") ;
leg->AddEntry (LC_Pb_30, "Pb 30 GeV","l") ;
leg->AddEntry (LC_Pb_50, "Pb 50 GeV","l") ;
leg->AddEntry (LC_Pb_100, "Pb 100 GeV","l") ;
leg->Draw();

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

TH1F * LP_W_100 = (TH1F *) f_W_100->Get ("h_E_vs_L") ;
LP_W_100->SetName ("LP_W_100")                     ;
LP_W_100->SetTitle ("")                            ;
LP_W_100->Rebin (25)                               ;
LP_W_100->SetLineColor (kGreen + 2)                ;
LP_W_100->Scale (1. / ( 50 * 100))                 ;

TH1F * LP_W_50 = (TH1F *) f_W_50->Get ("h_E_vs_L")   ;
LP_W_50->SetName ("LP_W_50")                       ;
LP_W_50->SetTitle ("")                             ;
LP_W_50->Rebin (25)                                ;
LP_W_50->SetLineColor (kBlue)                      ;
LP_W_50->Scale (1. / ( 100 * 50))                  ;

TH1F * LP_W_30 = (TH1F *) f_W_30->Get ("h_E_vs_L")   ;
LP_W_30->SetName ("LP_W_30")                       ;
LP_W_30->SetTitle ("")                             ;
LP_W_30->Rebin (25)                                ;
LP_W_30->SetLineColor (kRed)                       ;
LP_W_30->Scale (1. / (100 * 30))                   ;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

TH1F * LP_Pb_100 = (TH1F *) f_Pb_100->Get ("h_E_vs_L") ;
LP_Pb_100->SetName ("LP_Pb_100")                    ;
LP_Pb_100->SetTitle ("")                            ;
LP_Pb_100->Rebin (25)                               ;
LP_Pb_100->SetLineColor (kGreen + 2)                ;
LP_Pb_100->SetLineStyle (2)                         ;
LP_Pb_100->Scale (1. / (50 * 100))                  ;

TH1F * LP_Pb_50 = (TH1F *) f_Pb_50->Get ("h_E_vs_L")   ;
LP_Pb_50->SetName ("LP_Pb_50")                      ;
LP_Pb_50->SetTitle ("")                             ;
LP_Pb_50->Rebin (25)                                ;
LP_Pb_50->SetLineColor (kBlue)                      ;
LP_Pb_50->SetLineStyle (2)                          ;
LP_Pb_50->Scale (1. / ( 50 * 50))                   ;

TH1F * LP_Pb_30 = (TH1F *) f_Pb_30->Get ("h_E_vs_L")   ;
LP_Pb_30->SetName ("LP_Pb_30")                      ;
LP_Pb_30->SetTitle ("")                             ;
LP_Pb_30->Rebin (25)                                ;
LP_Pb_30->SetLineColor (kRed)                       ;
LP_Pb_30->SetLineStyle (2)                          ;
LP_Pb_30->Scale (1. / (50 * 30))                    ;

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

TCanvas * c2 = new TCanvas ("c2","c2",700, 700)                 ;
c2->SetLogy () ;
TH1F * fr_c2 = (TH1F *) c2->DrawFrame (0,0.00001,400,0.05)       ;
fr_c2->GetXaxis ()->SetTitle ("shower depth (mm)")              ;
fr_c2->GetYaxis ()->SetTitle ("1/ E_{beam} #times dE_{dep}/dx") ;

LP_W_30->Draw ("same")                                          ;
LP_W_50->Draw ("same")                                          ;
LP_W_100->Draw ("same")                                         ;

LP_Pb_30->Draw ("same")                                         ;
LP_Pb_50->Draw ("same")                                         ;
LP_Pb_100->Draw ("same")                                        ;

TLegend * leg = new TLegend (0.5,0.6,0.9,0.9) ;                 ;
leg->SetFillColor (kWhite) ;                                    ;
leg->AddEntry (LP_W_30, "W 30 GeV","l") ;                       ;
leg->AddEntry (LP_W_50, "W 50 GeV","l") ;                       ;
leg->AddEntry (LP_W_100, "W 100 GeV","l") ;                     ;
leg->AddEntry (LP_Pb_30, "Pb 30 GeV","l") ;                     ;
leg->AddEntry (LP_Pb_50, "Pb 50 GeV","l") ;                     ;
leg->AddEntry (LP_Pb_100, "Pb 100 GeV","l") ;                   ;
leg->Draw();                                                    ;


