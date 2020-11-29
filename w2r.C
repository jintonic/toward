// Convert binary data from CAEN WaveDump to ROOT format with
// - negative pulses being flipped to positive ones
// - baseline (pedestal) being calculated and subtracted
// - lowest and highest points being saved
//
// Arguments:
// - run: run number
// - ch: channel number
// - thr: threshold in ADC unit
// - polarity: polarity of triggered pulse (1: positive, -1: negative)
// - nbase: number of samples for baseline calculation
// - ssize: size of a sample value in byte
//
// Reference:
// - https://stackoverflow.com/questions/40001271/c-int-float-to-char-why-get-different-result-using-reinterpret-cast

void w2r(int run=0, int ch=0, float thr=10,
	 	float polarity=1, int nbase=100, int ssize=2)
{
	ifstream *input = new ifstream(Form("run/%d/wave%d.dat",run,ch), ios::binary);
	input->seekg(0, ios::end); // move getter to the end of file
	int fsize = input->tellg();// get input file size
	input->seekg(0, ios::beg); // move getter back to the beginning

	TFile *output = new TFile(Form("run/%d/wave%d.root",run,ch), "recreate");
	TTree *tree = new TTree("t", Form("waveforms from channel %d",ch));
	short adc[99999]= {0}; float s[99999]={0}; // waveform samples
	int n, len, tmp, cha, evt, ttt, t;
	float b, h, l;
	tree->Branch("b", &b, "b/F"); // baseline (pedestal)
	tree->Branch("h", &h, "h/F"); // highest point of a waveform
	tree->Branch("l", &l, "l/F"); // lowest point of a waveform
	tree->Branch("t", &t, "t/I"); // location of sample where threshold is crossed
	tree->Branch("n", &n, "n/I"); // number of samples in a waveform
	tree->Branch("s", s, "s[n]/F"); // waveform samples [n]
	tree->Branch("ttt", &ttt, "ttt/I"); // trigger time tag from digitizer
	tree->Branch("thr", &thr, "thr/F"); // trigger threshold in ADC units

	cout<<"Processing channel "<<ch<<endl;
	while (input->good() && input->tellg()<fsize) {
		input->read(reinterpret_cast<char*>(&len),4); // size of data [bytes]
		input->read(reinterpret_cast<char*>(&tmp),4); // board id
		input->read(reinterpret_cast<char*>(&tmp),4); // VME specific
		input->read(reinterpret_cast<char*>(&cha),4); // channel id
		input->read(reinterpret_cast<char*>(&evt),4); // event id
		input->read(reinterpret_cast<char*>(&ttt),4); // trigger time tag

		if (evt%1000==0) cout<<"Processing event "<<evt<<endl;
		b=0; h=-9999; l=9999; t=-1; // reset variables for each new waveform
		n = (len-24)/ssize; // number of waveform samples
		for (int i=0; i<n; i++) {
			input->read(reinterpret_cast<char*>(&adc[i]),ssize);
			s[i]=(float)adc[i];
			if (polarity==-1) s[i]*=polarity;
			if (i<nbase) b+=s[i];
			if (h<s[i]) h=s[i];
			if (l>s[i]) l=s[i];
			if (s[i]>thr && t==-1) t=i;
		}
		b/=nbase; for (int i=0; i<n; i++) s[i]-=b; h-=b; l-=b;

		tree->Fill();
	}
	input->close();
	tree->Write("", TObject::kOverwrite);
	output->Close();
}
