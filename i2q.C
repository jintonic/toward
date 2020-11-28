// The script
//
// - smooths the input charge pulses using 5-point smoothing:
//   https://terpconnect.umd.edu/%7Etoh/spectrum/Smoothing.html
//
// - converts the input charge pulses to current ones using
//   central-difference differentiation described in 
//   https://terpconnect.umd.edu/%7Etoh/spectrum/Differentiation.html
//
// - gets the charge pulse height using a trapezoidal filter with
//   l: rise time of the filter, m: flat top of the filter, and
//   M: decay time of the input pulse
//   https://nukephysik101.wordpress.com/2020/03/20/trapezoid-filter/

void i2q(int run=0, int cha=0)
{
	TFile *input = new TFile(Form("run/%d/wave%d.root",run,cha));
	TTree *tadc = (TTree*) input->Get("t");
	int ns; short a[99999] = {0};
	tadc->SetBranchAddress("s",&a); // current samples in ADC units
	tadc->SetBranchAddress("n",&ns); // number of samples

	TFile *output = new TFile(Form("run/%d/iq%d.root",run,cha), "recreate");
	TTree * t = new TTree("t", Form("i&q wfs from ch %d",cha));
	int tp, trg;
	float b, db, h, q[99999], i[99999];
 	t->Branch("n",&ns,"n/I"); // number of samples
 	t->Branch("b",&b,"b/F"); // baseline of the input signal
 	t->Branch("h",&h,"h/F"); // height of the current pulse
 	t->Branch("db",&db,"db/F"); // RMS of baseline of the input signal
 	t->Branch("tp",&tp,"tp/I"); // position of the maximal sample (T_peak)
 	t->Branch("trg",&trg,"trg/I"); // trigger time
 	t->Branch("q",q,"q[n]/F"); // samples of the charge pulse
 	t->Branch("i",i,"i[n]/F"); // samples of the current pulse

	int nevt=tadc->GetEntries();
	cout<<nevt<<" events from channel "<<cha<<" in run "<<run<<endl;
	for (int ievt=0; ievt<nevt; ievt++) {
		tadc->GetEntry(ievt);
		if (ievt%100==0) cout<<"Processing event "<<ievt<<endl;
		
		b=db=h=0; // reset parameters
	 	for (int n=0; n<100; n++) { b+=a[n]; } b/=100; // get baseline
		for (int n=0; n<100; n++) db+=(a[n]-b)*(a[n]-b); db=sqrt(db/100);

		// 7-point smoothing
		i[0]=a[0]-b; i[1]=a[1]-b; i[2]=a[2]-b;
		i[ns-3]=a[ns-3]-b; i[ns-2]=a[ns-2]-b; i[ns-1]=a[ns-1]-b;
		for (int n=3; n<ns-3; n++) {
		 	i[n]=(a[n+3]+3.*a[n+2]+6.*a[n+1]+7.*a[n]+6.*a[n-1]+3.*a[n-2]+a[n-3])/27-b;
			if (h<i[n]) { h=i[n]; tp=n; }
		}
		
		// integration
		q[0]=i[0];
	 	for (int n=1; n<ns; n++) { q[n]=q[n-1]+i[n]; }

		// get other parameters
		for (int n=0; n<ns; n++) if (i[n]>0.05*h) { trg=n; break; }

		t->Fill();
	}
	t->Write("",TObject::kOverwrite);

	output->Close();
}
