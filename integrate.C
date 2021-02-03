// Integrate waveforms in [min, max). If min, max are not specified,
//   min = threshold crossing point - front
//   max = downward zero crossing point + back
void integrate(const char* run="2020/02140956", int ch=1, int min=0, int max=0,
	 	int front=50, int back=10)
{
	TFile *file = new TFile(Form("%s/wave%d.root",run,ch),"update");
	TTree *t = (TTree*) file->Get("t");
	int n, tt; float a, s[99999] = {0};
	t->SetBranchAddress("s",&s); // waveform samples
	t->SetBranchAddress("n",&n); // number of samples
	t->SetBranchAddress("tt",&tt); // trigger position
	TBranch *bint = t->Branch("a",&a,"a/F"); // area in [min, max)
	TBranch *bmax = t->Branch("max",&max,"max/I");
	TBranch *bmin = t->Branch("min",&min,"min/I");

	bool search4boundaries=false; t->GetEntry(0);
	if (min<=0 || max<=min || max>=n) { // if not specified correctly
		if (tt==-1) { // external trigger, integration range must be specified
			cout<<"run "<<run<<" is taken with external trigger, please use"<<endl;
			cout<<"root npe.C'(int run,int channel,int min,int max)'\nto specify "
				"integration range with 0 < min < max < number of samples"<<endl;
			return;
		} else { // internal trigger
			search4boundaries=true;
		}
	}

	int nevt=t->GetEntries();
	cout<<"Processing "<<nevt<<" events..."<<endl;
	for (int i=0; i<nevt; i++) {
		t->GetEntry(i); if (i%5000==0) cout<<"Processing event "<<i<<endl;
		if (search4boundaries) {
			min=tt-front;
			// search for downward zero crossing point
			max=0;
			for (int j=tt; j<n; j++) { if (s[j]<0) max=j+back; if (max>0) break; }
			if (max==0) max=n;
		}
		a=0; for (int j=min; j<max; j++) a+=s[j];
		bint->Fill(); bmin->Fill(); bmax->Fill();
	}
	t->Write("",TObject::kOverwrite);
	file->Close();
	cout<<"File "<<file->GetName()<<" saved"<<endl;
}

