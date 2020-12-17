// Convert binary data from CAEN WaveDump to ROOT format with
// - baseline (pedestal) calculated and subtracted
// - negative pulses flipped to positive ones
// - lowest and highest points saved
// - threshold cross point saved
// - saturation status saved
//
// Arguments:
// - run: run number
// - ch: channel number
// - thr: threshold in ADC unit
// - polarity: polarity of triggered pulse (1: positive, -1: negative)
// - nbase: number of samples for baseline calculation
// - ssize: size of a sample value in byte
// - bits: resolution of digitizer

void w2r(const char* run="2020/11/27/2158", int ch=0, float thr=10,
	 	float polarity=1, int nbase=100, int ssize=2, int bits=10)
{
	ifstream *input = new ifstream(Form("%s/wave%d.dat",run,ch), ios::binary);
	input->seekg(0, ios::end); // move getter to the end of file
	int fsize = input->tellg();// get input file size
	input->seekg(0, ios::beg); // move getter back to the beginning

	TFile *output = new TFile(Form("%s/wave%d.root",run,ch), "recreate");
	TTree *tree = new TTree("t", Form("waveforms from channel %d",ch));
	short adc[99999]= {0}; float s[99999]={0}; // waveform samples
	int n, len, tmp, cha, evt, ttt, tt, th, tl;
	float b, db, h, l; bool is;
	tree->Branch("b", &b, "b/F"); // baseline (pedestal)
	tree->Branch("h", &h, "h/F"); // highest point of a waveform
	tree->Branch("l", &l, "l/F"); // lowest point of a waveform
	tree->Branch("n", &n, "n/I"); // number of samples in a waveform
	tree->Branch("s", s, "s[n]/F"); // waveform samples [n]
	tree->Branch("is", &is, "is/B"); // if a waveform is saturated
	tree->Branch("db", &db, "db/F"); // RMS of baseline
	tree->Branch("tt", &tt, "tt/I"); // location of sample where threshold is crossed
	tree->Branch("tl", &tl, "tl/I"); // location of the lowest sample
	tree->Branch("th", &th, "th/I"); // location of the highest sample
	tree->Branch("ttt", &ttt, "ttt/I"); // trigger time tag from digitizer

	int maxADC=(int)(pow(2, bits) + 0.5); // for saturation check
	while (input->good() && input->tellg()<fsize) {
		input->read(reinterpret_cast<char*>(&len),4); // size of data [bytes]
		input->read(reinterpret_cast<char*>(&tmp),4); // board id
		input->read(reinterpret_cast<char*>(&tmp),4); // VME specific
		input->read(reinterpret_cast<char*>(&cha),4); // channel id
		input->read(reinterpret_cast<char*>(&evt),4); // event id
		input->read(reinterpret_cast<char*>(&ttt),4); // trigger time tag

		if (evt%10000==0) cout<<"Processing event "<<evt<<endl;
		b=db=is=0; h=-9999; l=9999; tt=-1; // reset variables for each new waveform
		n = (len-24)/ssize; // number of waveform samples
		for (int i=0; i<n; i++) {
			// https://stackoverflow.com/questions/40001271
			input->read(reinterpret_cast<char*>(&adc[i]),ssize); s[i]=(float)adc[i];
			if (polarity==-1) { s[i]*=polarity; if (adc[i]==0) is=1; }
		 	else { if (adc[i]==maxADC) is=1; }
			if (i<nbase) b+=s[i];
		}
		b/=nbase; // get baseline
		for (int i=0; i<n; i++) {
			s[i]-=b; if (i<nbase) db+=s[i]*s[i];
			if (h<s[i]) { h=s[i]; th=i; }
			if (l>s[i]) { l=s[i]; tl=i; }
			if (thr!=0 && s[i]>thr && tt==-1) tt=i;
	 	}
		db=sqrt(db/nbase); // get baseline RMS

		tree->Fill();
	}
	input->close();

	tree->Write("", TObject::kOverwrite);
	cout<<tree->GetEntries()<<" events saved in "<<output->GetName()<<endl;
	output->Close();
}
