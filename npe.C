// integrate waveforms in [min, max)
void npe(int run=0, int ch=2, int min=0, int max=0)
{
	if (min<=0 || max<=min) {
		cout<<"Usage:"<<endl;
		cout<<"root npe.C'(int run,int channel,int min,int max)'"<<endl;
		cout<<"where 0 < min < max < number of samples"<<endl;
	 	return;
	}

	TFile *file = new TFile(Form("run/%d/wave%d.root",run,ch),"update");
	TTree *t = (TTree*) file->Get("t");
	int n; float a, s[99999] = {0};
	t->SetBranchAddress("s",&s); // waveform samples
	t->SetBranchAddress("n",&n); // number of samples
	TBranch *b = t->Branch("a",&a,"a/F"); // area in [min, max)

	int nevt=t->GetEntries();
	cout<<"Processing "<<nevt<<" events..."<<endl;
	for(int i=0; i<nevt; i++) {
		t->GetEntry(i); if (i%5000==0) cout<<"Processing event "<<i<<endl;
		if (min<0) min=0; if (max>n) max=n;
		a=0; for (int j=min; j<max; j++) a+=s[j];
		b->Fill();
	}
	cout<<nevt<<" events processed"<<endl;
	t->Write("",TObject::kOverwrite);
	file->Close();
}

