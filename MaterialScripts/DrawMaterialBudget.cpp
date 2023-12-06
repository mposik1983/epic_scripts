//TH2F *X0 = gGeoManager->GetTopVolume()->LegoPlot(180,0.,180.,180,0.,360.);
// TH2F *TGeoChecker::LegoPlot This function is defined in TGeoChecker
// https://root.cern.ch/doc/master/TGeoChecker_8cxx_source.html#l02040 line 20240
// gGeoManager->RandomRays(1000, 0., 0., 0.);
// Code to Extract Material Budget: Shyam Kumar shyam.kumar@ba.infn.it
int color=0;
int GetColor();
void DrawMaterialBudget(const char* name="11Oct2023_detector_geometry_ver2"){

     ofstream myfile;
     myfile.open ("materialbudget.txt");
     Bool_t drawvolume = 0;
     gSystem->Exec("rm -rf Output");
     gSystem->Exec("mkdir Output");
     TFile *myRootFile = new TFile("Output/myRootFile.root","RECREATE");
  	TGeoManager::Import(Form("/eic/u/mposik/Detector1/my_git/MPGD_Design/Geometry/MicroMegasBarrel/%s.root",name));
	string filename= name;
	const size_t period_idx = filename.rfind('.');
  if (std::string::npos != period_idx)
  {
    filename.erase(period_idx);
  }
  
  string output_plot_name=filename+"_";
 
   gGeoManager->SetVisLevel(5);
  // TopNode has many nodes access this way
  TObjArray* allnodes = gGeoManager->GetTopNode()->GetNodes(); 
  const int nNodes = allnodes->GetEntries();
  
   TCanvas *cradlen[nNodes];  TH2F *histX0[nNodes];
   TCanvas *cradlen1D[nNodes]; TH1F *hMb_1d[nNodes];
   TH1F *hTotMaterial;
   // Draw Matertial Map 1D Superposition
   TCanvas *cmap = new TCanvas("cmap","cmap",0,52,1400,1000);
   cmap->SetMargin(0.08, 0.22 ,0.10,0.07);
   cmap->SetLogy();
   cmap->SetGridx();    cmap->SetGridy();
   TLegend *l= new TLegend(0.79,0.1,0.95,0.9);
   l->SetTextSize(0.018);
   l->SetBorderSize(0);
  //   l->SetTextColor(tmpcolor);
  int startnode = 0; int endnode = 13;
    
  for(Int_t i=startnode; i<endnode;i++){
  
    cradlen[i] = new TCanvas(Form("c%d",i),Form("c%d",i),0,52,1400,1000);
   	cradlen[i]->SetGridy();
   	//cradlen[i]->SetLogz();
	cradlen[i]->SetMargin(0.08, 0.15 ,0.10,0.07);
	
    TGeoNode* node= (TGeoNode*)allnodes->At(i);
     TString volname = node->GetVolume()->GetName();
     
    // Each node is volume assembly
    // TGeoVolumeAssembly *vol = new TGeoVolumeAssembly(node->GetName());
     histX0[i] = gGeoManager->GetVolume(volname)->LegoPlot(180,0.,180.,360,0.,360.);
     histX0[i]->SetName(Form("h_Mb%d",i));
     histX0[i]->SetTitleSize(0.03);
     histX0[i]->SetTitle(Form("%s;#varphi (deg);#theta (deg);X/X_{0}",volname.Data()));
     histX0[i]->GetXaxis()->CenterTitle();
     histX0[i]->GetYaxis()->CenterTitle();
     histX0[i]->GetZaxis()->CenterTitle();
     histX0[i]->GetYaxis()->SetTitleOffset(1.0);
     histX0[i]->GetZaxis()->SetTitleOffset(0.80);
    // histX0[i]->GetZaxis()->SetRangeUser(1.0e-4,5.);
     //gStyle->SetPalette(kInvertedDarkBodyRadiator);
     gStyle->SetPalette(kBird);
     histX0[i]->SetStats(kFALSE);
     cradlen[i]->cd();
     histX0[i]->Draw("colz");
     cradlen[i]->Update();
     
     TPaletteAxis *palette = (TPaletteAxis*)histX0[i]->GetListOfFunctions()->FindObject("palette");
     double y1 = palette->GetY1NDC();
     double y2 = palette->GetY2NDC();
     double x1 = palette->GetX1NDC();
     double x2 = palette->GetX2NDC();
     palette->SetX1NDC(x1+0.02);
     palette->SetX2NDC(x2+0.02);

     cradlen[i]->Print(Form("Output/%s_%s.png",output_plot_name.data(),volname.Data()));
 
     cradlen1D[i] = new TCanvas(Form("c1d%d",i),Form("c1d%d",i),0,52,1400,1000);
	   cradlen1D[i]->SetMargin(0.08, 0.13 ,0.10,0.07);
	   cradlen1D[i]->SetLogy();
	   cradlen1D[i]->SetGridy();
	   cradlen1D[i]->SetGridy();
    
     hMb_1d[i] = (TH1F*) histX0[i]->ProjectionY(Form("Mb_1d_theta%d",i),1,-1,"");
     hMb_1d[i]->SetStats(kFALSE);
     hMb_1d[i]->SetLineWidth(2);
     hMb_1d[i]->SetTitleSize(0.03);
     hMb_1d[i]->SetTitle(Form("%s",volname.Data()));
     hMb_1d[i]->GetYaxis()->SetTitle("X/X_{0}");
     hMb_1d[i]->GetXaxis()->CenterTitle();
     hMb_1d[i]->GetYaxis()->CenterTitle();
     hMb_1d[i]->GetYaxis()->SetTitleOffset(1.0);
     hMb_1d[i]->Scale(1./histX0[i]->GetNbinsX());
     cradlen1D[i]->cd();
     hMb_1d[i]->Draw("hist");
     hMb_1d[i]->Write(Form("h_1D_%d",i));
     myRootFile->Write();
     int bin_90deg = hMb_1d[i]->GetXaxis()->FindBin(90.0);
     myfile << "Volume Name: \t" << volname.Data()<<" \t Material Budget: "<<hMb_1d[i]->GetBinContent(bin_90deg)<<"\n";
     cradlen1D[i]->Print(Form("Output/1D%s_%s.png",output_plot_name.data(),volname.Data()));
    int tmpcolor = GetColor();
     if (hMb_1d[i]->GetEntries()==0) continue; 
     hMb_1d[i]->GetYaxis()->SetRangeUser(1.0e-5,10.);
     hMb_1d[i]->SetLineColor(tmpcolor);
     hMb_1d[i]->SetMarkerStyle(21);
     hMb_1d[i]->SetMarkerSize(1.0);
     hMb_1d[i]->SetMarkerColor(tmpcolor);
     l->AddEntry(hMb_1d[i],hMb_1d[i]->GetTitle(),"p");
     cmap->cd();
     hMb_1d[i]->Draw("hist-same");
     
    if(drawvolume){
    new TCanvas;
    gGeoManager->GetTopNode()->Draw("ogl");
    // GLviewer setting you can choose different setting
   // 0 - no clip, 1 - clip plane, 2 - clip box
   TGLViewer *viewer = (TGLViewer *)gPad->GetViewer3D();
   viewer->ColorSet().Background().SetColor(kWhite);
   viewer->GetClipSet()->SetClipType(TGLClip::kClipPlane); // Other options are kClipNone = 0, kClipPlane, kClipBox    
   viewer->ColorSet().Background().SetColor(kWhite);
   viewer->SetGuideState(TGLUtil::kAxesOrigin, kTRUE, kFALSE, 0); // EAxesType { kAxesNone, kAxesEdge, kAxesOrigin }
   viewer->UseDarkColorSet(); 
   viewer->RefreshPadEditor(viewer);	
   viewer->CurrentCamera().RotateRad(-0.5, 0.5);
   viewer->DoDraw();
   Double_t center[3] ={0.,0.,0.};
   Double_t *cent; cent = center;
   viewer->SetOrthoCamera(TGLViewer::kCameraOrthoZOX,3.0, 3.0, cent,1.0,0.); // kCameraPerspXOZ, kCameraPerspYOZ, kCameraPerspXOY, kCameraOrthoXOY,
   viewer->SetResetCamerasOnUpdate(1);
   //kCameraOrthoXOZ, kCameraOrthoZOY, kCameraOrthoXnOY, kCameraOrthoXnOZ,kCameraOrthoZnOY
  // v->CurrentCamera().RotateRad(-.7, 0.5);	
   viewer->CurrentCamera().Zoom(3,1,1);
   //viewer->CurrentCamera().Configure(30., 0, center, -1.0, 0.5);	
  //viewer->CurrentCamera().RotateRad(-1.0, 0.5);
  viewer->RequestDraw(TGLRnrCtx::kLODHigh);
 //  viewer->DoDraw();
  // viewer->SavePicture(Form("%s.gif",geoname.Data()));
  viewer->SavePicture(Form("Output/%s_geo.png",volname.Data())); 
  } 
 } 
  myfile.close();
  gStyle->SetOptTitle(0);
  l->Draw("same");
  cmap->Update();
  cmap->Print(Form("Output/Superimpose1D%s.png",output_plot_name.data()));
}

Int_t GetColor(){
//int color = id+1;
++color;
if ((color%10)==0) ++color;
else if ((color%10)==8) color+=3;
else if ((color%10)==9) color+=2;
return color;
}
