// correct overshoot of a PMT pulse based on eq. 7 in
// https://arxiv.org/abs/1602.06080
void overshoot(const char* dir = "~/git/toward/2021/08/310932/", int ch=0)
{
	// input
	TChain *t = new TChain("t");
	t->Add(Form("%s/wave%d.root", dir, ch));

	int n,tt; float a,x[20000], s[20000] = {0};
	t->SetBranchAddress("n",&n);   // number of samples
	t->SetBranchAddress("s",&s);   // waveform samples
	t->SetBranchAddress("a",&a);   // area
	t->SetBranchAddress("tt",&tt); // trigger position

	// output
	TFile *output = new TFile("overshoot.root","recreate");
	TTree *to = new TTree("t","overshoot parameters");
	double x2, tau, ti, bgn, norm;
	to->Branch("x2", &x2);    // chi square
	to->Branch("ti", &ti);    // tau_i, decay of scintillation emission
	to->Branch("tau", &tau);  // tau, decay of electronics to be corrected
	to->Branch("bgn", &bgn);  // starting point of the pulse
	to->Branch("norm", &norm);// normalization factor

	// model
	TF1 *f = new TF1("f",
			"[0]*([1]*exp(-(x-[3])/[2])-[2]*exp(-(x-[3])/[1]))/([1]-[2])");
	f->SetParNames("Norm","#tau_i", "#tau", "Start");
	f->SetLineColor(kRed);
	//f->SetParameters(200,243,3690,6000);f->Draw();gPad->Print("f.png");//debug

	// event loop
	int nevt=t->GetEntries(); if (nevt>1000) nevt=1000;
	cout<<nevt<<" events to be processed"<<endl;
	for (int i=0; i<nevt; i++) {
		if ((i+1)%100==0) cout<<"now event "<<i+1<<endl;
		t->GetEntry(i); // load data from disk to memory

		// The following settings are from fittings of the 662 keV peak saved in
		// 2021/08/310932/wave0.root, taken with a Hamamatsu R11069 3-inch PMT.
		// They should be tuned for different peaks.
		if(a<36000 || a>51000) continue;
		f->SetParameter(0,210);
		f->SetParameter(1,245);
		f->SetParameter(2,3716);
		f->SetParameter(3,5955);

		for (int j=0; j<n; j++) x[j]=j;
		TGraph g(n,x,s); // take waveform from the input tree
		//g.Draw("al"); gPad->Print("ps.png"); // debug
		g.Fit(f,"QR","goff",tt,n-1); // silent fit from tt to the end of waveform

		// get and save fit results
		x2 = f->GetChisquare()/f->GetNDF();
		norm=f->GetParameter(0);
		ti = f->GetParameter(1);
		tau =f->GetParameter(2);
		bgn =f->GetParameter(3);
		to->Fill();
	}

	to->Write();
	output->Close();
}
