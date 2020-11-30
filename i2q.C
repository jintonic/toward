// convert the input current pulses to charge ones 
void i2q(int run=0, int ch=0)
{
	TFile *input = new TFile(Form("run/%d/wave%d.root",run,ch));
	TTree *tadc = (TTree*) input->Get("t");
	int ns; float a[99999] = {0};
	tadc->SetBranchAddress("s",&a); // current samples in ADC units
	tadc->SetBranchAddress("n",&ns); // number of samples

	TFile *output = new TFile(Form("run/%d/q%d.root",run,ch), "recreate");
	TTree * t = new TTree("t", Form("charge waveforms from ch %d",ch));
	int th, tl; float h, l, q[99999];
 	t->Branch("h",&h,"h/F"); // highest point of the charge pulse
 	t->Branch("l",&l,"l/F"); // lowest point of the charge pulse
 	t->Branch("n",&ns,"n/I"); // number of samples
 	t->Branch("q",q,"q[n]/F"); // samples of the charge pulse
 	t->Branch("th",&th,"th/I"); // location of the highest point
 	t->Branch("tl",&tl,"tl/I"); // location of the lowest point

	int nevt=tadc->GetEntries();
	for (int ievt=0; ievt<nevt; ievt++) {
		tadc->GetEntry(ievt);
		if (ievt%100==0) cout<<"Processing event "<<ievt<<endl;
		
		q[0]=a[0]; for (int n=1; n<ns; n++) { q[n]=q[n-1]+a[n]; }
		h=-999; for (int n=0; n<ns; n++) { if (h<q[n]) { h=q[n]; th=n; } }
		l=9999; for (int n=0; n<ns; n++) { if (l>q[n]) { l=q[n]; tl=n; } }

		t->Fill();
	}
	cout<<nevt<<" events processed"<<endl;
	t->Write("",TObject::kOverwrite);

	output->Close();
}
