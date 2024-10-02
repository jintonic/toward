// Integrate waveforms in [min, max). If min, max are not specified,
//   min = threshold crossing point - front
//   max = downward zero crossing point + back
// If tau>0, overshoot will be corrected
//   tau: time constant of PMT electronics
//   t_i: time constant of scintillation emission
// overshoot correction = pulse height *
//   {exp(t/t_i) - [t_i exp(-t/tau) - tau exp(-t/t_i)]/(t_i - tau)}
void integrate(const char* run="2020/02/02140956", int ch=1, int min=0, int max=0,
	 	int front=50, int back=10, double tau=0, double t_i=245)
{
	TFile *file = new TFile(Form("%s/wave%d.root",run,ch),"update");
	TTree *t = (TTree*) file->Get("t");
	int n, tt; float a, s[99999] = {0}, ds[99999] = {0};
	t->SetBranchAddress("s",&s); // waveform samples
	t->SetBranchAddress("n",&n); // number of samples
	t->SetBranchAddress("tt",&tt); // trigger position
	TBranch *bint = t->Branch("a",&a,"a/F"); // area in [min, max)
	TBranch *brds = t->Branch("ds",ds,"ds[n]/F"); // correction of s
	TBranch *bmax = t->Branch("max",&max,"max/I");
	TBranch *bmin = t->Branch("min",&min,"min/I");

	bool search4boundaries=false; t->GetEntry(0);
	if (min<=0 || max<=min || max>=n) // if not specified correctly
			search4boundaries=true; // search for boundary by ourselves

	int nevt=t->GetEntries();
	cout<<"Processing "<<nevt<<" events..."<<endl;
	for (int i=0; i<nevt; i++) {
		t->GetEntry(i); if (i%5000==0) cout<<"Processing event "<<i<<endl;

		if (tau>0) { // correct overshoot
			float height=(s[tt+60]+s[tt+61]+s[tt+62]+s[tt+63]+s[tt+64])/5;
			for (int j=tt; j<n; j++) {
				ds[j]=height*(exp(-(j-tt)/t_i)-exp(-(j-tt)/tau))/(1-tau/t_i);
				s[j]+=ds[j];
			}
		}

		if (search4boundaries) {
			min=tt-front;
			if (min<0) min=0; if (tt<0) tt=0;
			// search for downward zero crossing point
			max=0;
			for (int j=tt; j<n; j++) { if (s[j]<0) max=j+back; if (max>0) break; }
			if (max==0) max=n;
		}

		a=0; for (int j=min; j<max; j++) a+=s[j];
		bint->Fill(); brds->Fill(); bmin->Fill(); bmax->Fill();
	}
	t->Write("",TObject::kOverwrite);
	file->Close();
	cout<<"File "<<file->GetName()<<" saved"<<endl;
}

