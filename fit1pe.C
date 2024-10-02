void Create1PEdistr(const char* run, int fixedMin, int fixedMax, int channel)
{
   TChain *t = new TChain("t");
   t->Add(Form("%s/wave%d.root",run,channel));
	 int n;
	 float s[1000], a, h;
	 t->SetBranchAddress("n",&n);
	 t->SetBranchAddress("s",&s);
	 t->SetBranchAddress("a",&a);
	 t->SetBranchAddress("h",&h);

   TFile *output = new TFile(Form("pe%d.root",channel),"recreate");
   TH1D *hpe = new TH1D("hpe","",450,-150,300);

   int min=99999, max=0; // integration range (will be updated automatically)
   int nevt = t->GetEntries();
   if (nevt>50000) nevt=50000; // no need to load more than this
   for(int i=0; i<nevt; i++) {
      if (i%5000==0) cout<<"now event "<<i<<endl;
      t->GetEntry(i);
			min=fixedMin; max=fixedMax;
			if (a/h>12) continue;
			if (h<5) continue;
      double total=0;
      for (int j=min; j<max; j++) total+=s[j];
      hpe->Fill(total);
   }
   output->Write();
   output->Close();
}

// overlap many WFs in one plot to check location of 1 PE pulses by eyes
void DrawWFs(const char* run, int channel)
{
   TChain *t = new TChain("t");
   t->Add(Form("%s/wave%d.root",run,channel));
   t->Draw("s:Iteration$", "a/h<12","l",200,0);
   TText *text = new TText(.8,.8,Form("%s",run)); text->SetNDC(); text->Draw();
   gPad->Print(Form("wf%d.png",channel));
}

// integration range is not fixed by default
void fit1pe(const char *run, int fixedMin=0, int fixedMax=0, int channel=0)
{
   if (fixedMin!=0 && fixedMax!=0) DrawWFs(run, channel);
   Create1PEdistr(run, fixedMin, fixedMax, channel);
   //Fit1PEdistr(run);
}

