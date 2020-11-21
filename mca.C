// get pulse height using a trapezoidal filter
float getPulseHeight(int n, short *s, int L=500, int G=100)
{
	std::vector<double> mid;
	for (int k=0; k<n; k++) {
		mid.push_back(0);
		for(int i=0; i<L; i++) {
			int m=mid.size();
			if (m-i<=0) break;
			mid[m-i-1]+=s[k];
		}
	}
	for (int i=0; i<L-1; i++) mid.pop_back();
	for (int i=0; i<(int)mid.size(); i++) mid[i]=mid[i]/L;
	std::vector<double> needed;
	for (int i=0; i<n-2*L-G; i++) needed.push_back(mid[i+G+L]-mid[i]);
	double max=needed[0], min=needed[0];
	for (size_t i=0; i<needed.size(); i++) {
		if (needed[i]>max) max=needed[i];
		if (needed[i]<min) min=needed[i];
	}
	return max-min;
}

void mca(int run=1)
{
	TFile *input = new TFile(Form("tier1/%d.root",run));
	int nch = input->GetNkeys(), n;
	TTree *c[999] = {0};
	short s[99999] = {0};
	for (int i=0; i<nch; i++) {
		c[i] = (TTree*) input->Get(Form("c%d",i));
		c[i]->SetBranchAddress("s",&s);
		c[i]->SetBranchAddress("n",&n);
	}

	TFile *output = new TFile(Form("tier2/%d.root",run), "recreate");
	float e=0,t=0;
	TTree *tree[999] = {0};
	for (int i=0; i<nch; i++) {
		tree[i] = new TTree(Form("t%d",i), Form("E & T from ch %d",i));
		tree[i]->Branch("e",&e,"e/F");
		tree[i]->Branch("t",&t,"t/F");
	}

	for (int ch=0; ch<nch; ch++) {
		int nevt=c[ch]->GetEntries(); cout<<nevt<<" events in channel "<<ch<<endl;
		for (int ievt=0; ievt<nevt; ievt++) {
			c[ch]->GetEntry(ievt); if (ievt%1==0) cout<<ievt<<" events processed"<<endl;

			e = getPulseHeight(n,s);
			tree[ch]->Fill();
		}
		tree[ch]->Write("",TObject::kOverwrite);
	}

	output->Close();
}
