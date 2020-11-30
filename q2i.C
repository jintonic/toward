// The script
// - smooths the input charge pulses using 5-point smoothing:
//   https://terpconnect.umd.edu/%7Etoh/spectrum/Smoothing.html
// - converts the input charge pulses to current ones using
//   central-difference differentiation described in 
//   https://terpconnect.umd.edu/%7Etoh/spectrum/Differentiation.html
// - gets the charge pulse height using a trapezoidal filter with
//   l: rise time of the filter, m: flat top of the filter, and
//   M: decay time of the input pulse
//   https://nukephysik101.wordpress.com/2020/03/20/trapezoid-filter/

void q2i(int run=0, int ch=0, int l=1000, int m=500, float M=11480)
{
	TFile *input = new TFile(Form("run/%d/wave%d.root",run,ch));
	TTree *tadc = (TTree*) input->Get("t");
	int ns; float a[99999] = {0};
	tadc->SetBranchAddress("s",&a); // charge samples in ADC units
	tadc->SetBranchAddress("n",&ns); // number of samples

	TFile *output = new TFile(Form("run/%d/current%d.root",run,ch), "recreate");
	TTree * t = new TTree("t", Form("current waveforms from ch %d",ch));
	int t5, t10, t50, t90;
	float h, s[99999], d[99999], p[99999], q[99999], i[99999];
 	t->Branch("n",&ns,"n/I"); // number of samples
 	t->Branch("h",&h,"h/F"); // calculated height of the input pulse
 	t->Branch("t5",&t5,"t5/I"); // position of 5% rise time
 	t->Branch("t10",&t10,"t10/I"); // position of 10% rise time
 	t->Branch("t50",&t50,"t50/I"); // position of 50% rise time
 	t->Branch("t90",&t90,"t90/I"); // position of 90% rise time
 	t->Branch("s",s,"s[n]/F"); // samples of trapezoidal filter response
 	t->Branch("i",i,"i[n]/F"); // samples of the current pulse

	int nevt=tadc->GetEntries();
	for (int ievt=0; ievt<nevt; ievt++) {
		tadc->GetEntry(ievt);
		if (ievt%100==0) cout<<"Processing event "<<ievt<<endl;
		
		// 5-point smoothing
		q[0]=a[0]; q[1]=a[1]; q[2]=a[2];
		q[ns-3]=a[ns-3]; q[ns-2]=a[ns-2]; q[ns-1]=a[ns-1];
		for (int n=3; n<ns-3; n++)
		 	q[n]=(a[n+3]+3.*a[n+2]+6.*a[n+1]+7.*a[n]+6.*a[n-1]+3.*a[n-2]+a[n-3])/27;
		
		// trapezoidal filtering
		p[0] = d[0] = q[0]; s[0] = p[0] + d[0]*M;
		for (int n=1; n<ns; n++) {
			if (n<l) d[n] = q[n];
			else if (n<m+l) d[n] = q[n] - q[n-l]; 
			else if (n<m+2*l) d[n] = q[n] - q[n-m-l] - q[n-l];
			else d[n] = q[n] - q[n-m-l] - q[n-l] + q[n-m-2*l];
			p[n] = p[n-1] + d[n];
			s[n] = s[n-1] + p[n] + d[n]*M;
		}
		for (int n=0; n<ns; n++) s[n]/=l*M; // rescale

		// central-difference differentiation
		i[0]=i[ns-1]=0; for (int n=1; n<ns-1; n++) i[n]=(q[n+1]-q[n-1])/2;

		// get other parameters
		h=0; for (int n=m+2*l; n<ns; n++) { if (h<s[n]) h=s[n]; }
		for (int n=m+2*l; n<ns; n++) if (q[n]>0.05*h) { t5=n; break; }
		for (int n=m+2*l; n<ns; n++) if (q[n]>0.1*h) { t10=n; break; }
		for (int n=m+2*l; n<ns; n++) if (q[n]>0.5*h) { t50=n; break; }
		for (int n=m+2*l; n<ns; n++) if (q[n]>0.9*h) { t90=n; break; }

		t->Fill();
	}
	cout<<nevt<<" events processed"<<endl;
	t->Write("",TObject::kOverwrite);

	output->Close();
}
