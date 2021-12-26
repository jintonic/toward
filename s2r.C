// Convert binary data from Struck SIS3316 to ROOT format
// 
// Arguments:
// - nm: number of modules (cards)
// - fi: input binary file
// - fo: output ROOT file
void s2r(const int nm = 2, const char* fi =
		"/data3/coherent/data/mars/prodRun1_waveforms/SIS3316Raw_20171127180918_3.bin",
		const char* fo = "sis3316.root")
{
	ifstream *input = new ifstream(Form("%s",fi), ios::binary);
	input->seekg(0, ios::end); // move getter to the end of file
	long int fsize = input->tellg();// get input file size
	input->seekg(0, ios::beg); // move getter back to the beginning

	TFile *output = new TFile(Form("%s",fo), "recreate");
	const int nc = 16; // number of channels in one module (card)
	short adc[99999]= {0}; float s[99999]={0}; // waveform samples
	int n, len, word, cha, evt, ttt, tt, th, tl;
	float b, db, h, l; bool is; char byte[65536];
	TTree *t[128]={0};
	for (int i=0; i<nm*nc; i++) {
		t[i]= new TTree(Form("c%d",i), Form("SIS3316 channel %d",i));
		t[i]->Branch("b", &b, "b/F"); // baseline (pedestal)
		t[i]->Branch("h", &h, "h/F"); // highest point of a waveform
		t[i]->Branch("l", &l, "l/F"); // lowest point of a waveform
		t[i]->Branch("n", &n, "n/I"); // number of samples in a waveform
		t[i]->Branch("s", s, "s[n]/F"); // waveform samples [n]
		t[i]->Branch("is", &is, "is/B"); // if a waveform is saturated
		t[i]->Branch("db", &db, "db/F"); // RMS of baseline
		t[i]->Branch("tt", &tt, "tt/I"); // location of sample where threshold is crossed
		t[i]->Branch("tl", &tl, "tl/I"); // location of the lowest sample
		t[i]->Branch("th", &th, "th/I"); // location of the highest sample
		t[i]->Branch("ttt", &ttt, "ttt/I"); // trigger time tag from digitizer
	}

	ttt=0;
	input->seekg(400); // skip file header
	while (input->good() && input->tellg()<fsize) {
		input->read(byte,40); // spill header (abba)
		for (int i=0; i<10; i++) { // print spill header
			for (int j=0; j<4; j++) printf("%x ", byte[i*4+j]);
			cout<<i<<endl;
		}
		for (int mo=0; mo<2; mo++) {
			input->read(byte,8); // module header
			for (int ch=0; ch<16; ch++) {
				input->read(byte,32); // channel header
				int nw = byte[28]; // number of words
				printf("size of channel %d in module %d: %u words\n", ch, mo, nw);
				input->read(byte,nw*4); // channel data
				//printf("format bits: %u, ch: %u, ",(UChar_t)(word&0xf), (UShort_t)((word&0xfff0)>>4));
				//ULong64_t timestamp = (ULong64_t)(word&0xffff0000) << 16;
				//timestamp = timestamp | (&word)[1];
				//printf("time stamp: %llu\n", timestamp);
			}
		}
		ttt++;
		cout<<"ttt: "<<ttt<<endl;
	}
	input->close();

	for (int i=0; i<nm*nc; i++) t[i]->Write("", TObject::kOverwrite);
	cout<<t[0]->GetEntries()<<" events saved in "<<output->GetName()<<endl;
	output->Close();
}
