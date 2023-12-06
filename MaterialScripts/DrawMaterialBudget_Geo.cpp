 //TH2F *X0 = gGeoManager->GetTopVolume()->LegoPlot(180,0.,180.,180,0.,360.);
// TH2F *TGeoChecker::LegoPlot This function is defined in TGeoChecker
// https://root.cern.ch/doc/master/TGeoChecker_8cxx_source.html#l02040 line 20240
// gGeoManager->RandomRays(1000, 0., 0., 0.);
// Code to Extract Material Budget: Shyam Kumar shyam.kumar@ba.infn.it

void DrawMaterialBudget_Geo(const char* name="epic_craterlake_angres_nobeampipe"){

	TGeoManager::Import(Form("/eic/u/mposik/Detector1/EPIC/epic/%s.root",name));
	//TGeoManager::Import(Form("./Root_Geometry/%s.root",name));
	string filename= name;
	const size_t period_idx = filename.rfind('.');
  if (std::string::npos != period_idx)
  {
    filename.erase(period_idx);
  }
  
     TString output_plot_name=filename+"_MB";
 
     gGeoManager->SetVisLevel(5);
  
     TCanvas *cradlen = new TCanvas("c","c",0,52,1400,1000);
   	cradlen->SetGridy();
   	//cradlen->SetLogz();
	cradlen->SetMargin(0.08, 0.13 ,0.10,0.07);

     TH2F *histX0 = gGeoManager->GetMasterVolume()->LegoPlot(90,0.,180.,180,0.,360.);
     histX0->SetTitle("Effective radiation length (X/X_{0}) vs #theta (deg) & #varphi (deg);#varphi (deg);#theta (deg);X/X_{0}");
     histX0->GetXaxis()->CenterTitle();
     histX0->GetYaxis()->CenterTitle();
     histX0->GetZaxis()->CenterTitle();
     histX0->GetYaxis()->SetTitleOffset(1.0);
     histX0->GetZaxis()->SetTitleOffset(0.80);
    // histX0->GetZaxis()->SetRangeUser(1.0e-4,1.);
     histX0->SetStats(kFALSE);
     gStyle->SetPalette(kInvertedDarkBodyRadiator);
     histX0->Draw("colz");
     cradlen->Update();
     
     TPaletteAxis *palette = (TPaletteAxis*)histX0->GetListOfFunctions()->FindObject("palette");
     double y1 = palette->GetY1NDC();
     double y2 = palette->GetY2NDC();
     double x1 = palette->GetX1NDC();
     double x2 = palette->GetX2NDC();
     palette->SetX1NDC(x1+0.02);
     palette->SetX2NDC(x2+0.02);

     cradlen->Print(Form("%s.png",output_plot_name.Data())); 
     
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
 //  viewer->DoDraw();
  // viewer->SavePicture(Form("%s.gif",geoname.Data()));
  TString topvolume = gGeoManager->GetMasterVolume()->GetName();
  viewer->SavePicture(Form("%s_geo.png",topvolume.Data()));
   
}
