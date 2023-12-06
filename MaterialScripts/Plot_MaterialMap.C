// Material Map by Shyam Kumar based on Root examples
void Plot_MaterialMap(const char* name="epic_craterlake_angres_invert")
{
	TGeoManager::Import(Form("/direct/eic+u/mposik/Detector1/EPIC/epic/%s.root",name));

	string filename= name;
	const size_t period_idx = filename.rfind('.');
  if (std::string::npos != period_idx)
  {
    filename.erase(period_idx);
  }

	string output_plot_name=filename+"_MB.pdf";
  TCanvas *cradlen = new TCanvas("c","c",0,52,1400,1000);
  cradlen->SetGridy();
  cradlen->SetLogz();
	cradlen->SetMargin(0.08, 0.13 ,0.10,0.07);

   Int_t nphi = 180;  Double_t phimin = 0.; Double_t phimax = 360.; 
   Int_t ntheta = 90; Double_t themin = 0.; Double_t themax = 180.;
   
   TH2F *hist = new TH2F("hMaterialBudget", "",nphi, phimin, phimax, ntheta, themin, themax);
 
   Double_t degrad = TMath::Pi()/180.; // degree to Rad
   Double_t theta, phi, step, matprop, x;
   Double_t start[3];  Double_t dir[3]; // start position and direction
   TGeoNode *startnode, *endnode;
   Int_t i;  // loop index for phi
   Int_t j;  // loop index for theta
   Int_t ntot = ntheta * nphi;
   Int_t n10 = ntot/10;
   Int_t igen = 0, iloop=0;
   printf("=== Lego plot sph. => nrays=%i\n", ntot);
   for (i=1; i<=nphi; i++) {
      for (j=1; j<=ntheta; j++) {
         igen++;
         if (n10) {
            if ((igen%n10) == 0) printf("%i percent\n", Int_t(100*igen/ntot));
         }
         x = 0;
         theta = hist->GetYaxis()->GetBinCenter(j);
         phi   = hist->GetXaxis()->GetBinCenter(i)+1E-3;
         start[0] = start[1] = start[2] = 1E-3;
         dir[0]=TMath::Sin(theta*degrad)*TMath::Cos(phi*degrad);
         dir[1]=TMath::Sin(theta*degrad)*TMath::Sin(phi*degrad);
         dir[2]=TMath::Cos(theta*degrad);
         gGeoManager->InitTrack(&start[0], &dir[0]);
         startnode = gGeoManager->GetCurrentNode();
      //   cout<<"Start Node: "<<startnode->GetName()<<endl;
         if (gGeoManager->IsOutside()) startnode=0;
         if (startnode) {
            matprop = startnode->GetVolume()->GetMaterial()->GetRadLen();
         } else {
            matprop = 0.;
         }
         gGeoManager->FindNextBoundary();
         // find where we end-up
         endnode = gGeoManager->Step();
         step = gGeoManager->GetStep();
         while (step<1E10) {
            // now see if we can make an other step
            iloop=0;
            while (!gGeoManager->IsEntering()) {
               iloop++;
               gGeoManager->SetStep(1E-3);
               step += 1E-3;
               endnode = gGeoManager->Step();
            }
            if (iloop>1000) printf("%i steps\n", iloop);
            if (matprop>0) {
               x += step/matprop;
            }
            if (endnode==0 && step>1E10) break;
            // generate an extra step to cross boundary
            startnode = endnode;
            if (startnode) {
               matprop = startnode->GetVolume()->GetMaterial()->GetRadLen();
            } else {
               matprop = 0.;
            }
 
            gGeoManager->FindNextBoundary();
            endnode = gGeoManager->Step();
            step = gGeoManager->GetStep();
         }
         hist->Fill(phi, theta, x);
      }
   }
    hist->SetTitle("Effective radiation length (X/X_{0}) vs #theta (deg) & #varphi (deg);#varphi (deg);#theta (deg);X/X_{0}");
    hist->GetXaxis()->CenterTitle();
    hist->GetYaxis()->CenterTitle();
    hist->GetZaxis()->CenterTitle();
    hist->GetYaxis()->SetTitleOffset(1.0);
    hist->GetZaxis()->SetTitleOffset(0.80);
    hist->GetZaxis()->SetRangeUser(0.,150.);
    gStyle->SetPalette(kInvertedDarkBodyRadiator);
    hist->SetStats(kFALSE);
    hist->Draw("colz");
    cradlen->Update();
    TPaletteAxis *palette = (TPaletteAxis*)hist->GetListOfFunctions()->FindObject("palette");
    double y1 = palette->GetY1NDC();
    double y2 = palette->GetY2NDC();
    double x1 = palette->GetX1NDC();
    double x2 = palette->GetX2NDC();
    palette->SetX1NDC(x1+0.02);
    palette->SetX2NDC(x2+0.02);
    cradlen->Print(output_plot_name.data());
    output_plot_name = filename+"_MB.png";
    cradlen->Print(output_plot_name.data());    
}

