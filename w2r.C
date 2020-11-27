// convert binary data from CAEN WaveDump to ROOT format
void w2r(int run=0, int ch=0)
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

	cout<<"Processing channel "<<ch<<endl;
	while (input->good() && input->tellg()<fsize) {
		input->read(reinterpret_cast<char*>(&len),4); // size of data [bytes]
		input->read(reinterpret_cast<char*>(&tmp),4); // board id
		input->read(reinterpret_cast<char*>(&tmp),4); // VME specific
		input->read(reinterpret_cast<char*>(&cha),4); // channel id
		input->read(reinterpret_cast<char*>(&evt),4); // event id
		input->read(reinterpret_cast<char*>(&t), 4);

		if (evt%1000==0) cout<<"Processing event "<<evt<<endl;
		n = (len-24)/2; // number of 16-bit long waveform samples
		for (int j=0; j<n; j++) input->read(reinterpret_cast<char*>(&s[j]),2);

		tree->Fill();
	}
	input->close();
	tree->Write("", TObject::kOverwrite);
	output->Close();
}
