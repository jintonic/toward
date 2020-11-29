// Convert binary data from CAEN WaveDump to ROOT format. Arguments:
// - run: run number
// - ch: channel number
// - thr: threshold in ADC unit
// - polarity: polarity of triggered pulse (1: posivity, -1: negative)
// - nbase: number of samples for baseline calculation
// - ssize: size of a sample value in byte
void w2r(int run=0, int ch=0, int thr=10,
	 	int polarity=1, int nbase=100, int ssize=2)
{
	ifstream *input = new ifstream(Form("run/%d/wave%d.dat",run,ch), ios::binary);
	input->seekg(0, ios::end); // move getter to the end of file
	int fsize = input->tellg();// get input file size
	input->seekg(0, ios::beg); // move getter back to the beginning

	TFile *output = new TFile(Form("run/%d/wave%d.root",run,ch), "recreate");
	TTree *tree = new TTree("t", Form("waveforms from channel %d",ch));
	short s[99999]= {0}; // waveform samples (16-bit integers)
	int len, n, tmp, cha, evt, t;
	tree->Branch("n", &n, "n/I");  // number of samples
	tree->Branch("s", s, "s[n]/S");// waveform samples
	tree->Branch("t", &t, "t/I");  // trigger time tag
	tree->Branch("thr", &thr, "thr/I");  // trigger threshold in ADC unit

	cout<<"Processing channel "<<ch<<endl;
	while (input->good() && input->tellg()<fsize) {
		input->read(reinterpret_cast<char*>(&len),4); // size of data [bytes]
		input->read(reinterpret_cast<char*>(&tmp),4); // board id
		input->read(reinterpret_cast<char*>(&tmp),4); // VME specific
		input->read(reinterpret_cast<char*>(&cha),4); // channel id
		input->read(reinterpret_cast<char*>(&evt),4); // event id
		input->read(reinterpret_cast<char*>(&t), 4);

		if (evt%1000==0) cout<<"Processing event "<<evt<<endl;
		n = (len-24)/ssize; // number of waveform samples
		for (int j=0; j<n; j++) {
			input->read(reinterpret_cast<char*>(&s[j]),2);
			if (polarity==-1) s[j]*=polarity;
		}

		tree->Fill();
	}
	input->close();
	tree->Write("", TObject::kOverwrite);
	output->Close();
}
