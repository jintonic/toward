// get voltage pulse height using a trapezoidal filter
// l: rise time of the filter, m: flat top of the filter
// M: decay time of the input voltage pulse [ns]
void getVoltagePulseHeight(int run=0, int cha=0,
		int l=1000, int m=500, float M=11480)
{
	TFile *input = new TFile(Form("run/%d/wave%d.root",run,cha));
	TTree *wf = (TTree*) input->Get("t");
	int k; short v[99999] = {0};
	wf->SetBranchAddress("s",&v); // voltage samples
	wf->SetBranchAddress("n",&k); // number of samples

	TFile *output = new TFile(Form("run/%d/shaped%d.root",run,cha), "recreate");
	TTree * t = new TTree("t", Form("shaped wfs from ch %d",cha));
	int tp, t5, t10, t50, t90; float b, db, h, s[99999], d[99999], p[99999];
 	t->Branch("n",&k,"n/I"); // number of samples of trapezoidal filter response
 	t->Branch("b",&b,"b/F"); // baseline of the input signal
 	t->Branch("h",&h,"h/F"); // calculated height of the input pulse
 	t->Branch("db",&db,"db/F"); // RMS of baseline of the input signal
 	t->Branch("tp",&tp,"tp/I"); // position of the maximal sample (T_peak)
 	t->Branch("t5",&t5,"t5/I"); // position of 5% rise time
 	t->Branch("t10",&t10,"t10/I"); // position of 10% rise time
 	t->Branch("t50",&t50,"t50/I"); // position of 50% rise time
 	t->Branch("t90",&t90,"t90/I"); // position of 90% rise time
 	t->Branch("s",s,"s[n]/F"); // samples of trapezoidal filter response

	int nevt=wf->GetEntries();
	cout<<nevt<<" events from channel "<<cha<<" in run "<<run<<endl;
	for (int ievt=0; ievt<nevt; ievt++) {
		wf->GetEntry(ievt);
		if (ievt%100==0) cout<<ievt<<" events processed"<<endl;
		
		b=db=h=0; // reset parameters
	 	for (int i=0; i<100; i++) { b+=v[i]; } b/=100; // get baseline
		
		// https://nukephysik101.wordpress.com/2020/03/20/trapezoid-filter/
		p[0] = d[0] = v[0]-b; s[0] = p[0] + d[0]*M;
		for (int n=1; n<k; n++) {
			if (n<l) d[n] = v[n]-b;
			else if (n>=l && n<m+l) d[n] = v[n] - v[n-l]; 
			else if (n>=m+l && n<m+2*l) d[n] = v[n] - v[n-m-l] - v[n-l] + b;
			else d[n] = v[n] - v[n-m-l] - v[n-l] + v[n-m-2*l];

			p[n] = p[n-1] + d[n];
			s[n] = s[n-1] + p[n] + d[n]*M;
			
			if (h<v[n]) { h=v[n]; tp=n; } // search for T_peak
			if (n<=100) db+=(v[n]-b)*(v[n]-b); db=sqrt(db/100);
		}
		for (int i=0; i<k; i++) s[i]/=l*M;
		for (int i=m+2*l; i<k; i++) if ((v[i]-b)>0.05*(h-b)) { t5=i; break; }
		for (int i=m+2*l; i<k; i++) if ((v[i]-b)>0.1*(h-b)) { t10=i; break; }
		for (int i=m+2*l; i<k; i++) if ((v[i]-b)>0.5*(h-b)) { t50=i; break; }
		for (int i=m+2*l; i<k; i++) if ((v[i]-b)>0.9*(h-b)) { t90=i; break; }
		h=0; for (int i=t90+l; i<t90+l+100; i++) { h+=s[i]; } h/=100;

		t->Fill();
	}
	t->Write("",TObject::kOverwrite);

	output->Close();
}
