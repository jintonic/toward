void b2r(int run=1, int nch=1)
{
	ifstream *input[9999] = {0}; int fsize[9999] = {0};
	for (int i=0; i<nch; i++) {
		input[i] = new ifstream(Form("tier0/%d/wave%d.dat",run,i), ios::binary); 
		input[i]->seekg(0, ios::end); // move getter to the end of file
		fsize[i] = input[i]->tellg(); // get input file size
		input[i]->seekg(0, ios::beg); // move getter back to the beginning
	}

	TFile *output = new TFile(Form("tier1/%d.root",run), "recreate");
	TTree *c[9999]= {0}; // each tree contains waveforms from a channel
	short s[99999]= {0}; // waveform samples (16-bit integers)
	int len, n, tmp, cha, evt, t;
	for (int i=0; i<nch; i++) {
		c[i] = new TTree(Form("c%d",i), Form("waveform from channel %d",i));
		c[i]->Branch("n", &n, "n/I"); // number of samples
		c[i]->Branch("s", s, "s[n]/S"); // waveform samples
		c[i]->Branch("t", &t, "t/I"); // trigger time tag
	}

	for (int i=0; i<nch; i++) {
		cout<<"process channel "<<i<<endl;
		while (input[i]->good() && input[i]->tellg()<fsize[i]) {
			input[i]->read(reinterpret_cast<char*>(&len),4); // size of data [bytes]
			input[i]->read(reinterpret_cast<char*>(&tmp),4); // board id
			input[i]->read(reinterpret_cast<char*>(&tmp),4); // VME specific
			input[i]->read(reinterpret_cast<char*>(&cha),4); // channel id
			input[i]->read(reinterpret_cast<char*>(&evt),4); // event id
			input[i]->read(reinterpret_cast<char*>(&t), 4);
			if (evt%1000==0) cout<<evt<<" events processed"<<endl;

			n = (len-24)/2; // number of 16-bit long waveform samples
			for (int j=0; j<n; j++) input[i]->read(reinterpret_cast<char*>(&s[j]),2);

			c[i]->Fill();
		}
		input[i]->close();
	}

	for (int i=0; i<nch; i++) c[i]->Write("", TObject::kOverwrite);
	output->Close();
}
