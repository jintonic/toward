// Convert binary output from Struck SIS3316 to ROOT format
// Ref. https://github.com/gcrich/binaryToROOT-NGM3316
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
	int n, len, cha, evt, ttt, tt, th, tl;
	float b, db, h, l; bool is;
	TTree* t[128]={0}; // assume there are maximally 128 channels
	for (int i=0; i<nm*nc; i++) {
		t[i] = new TTree(Form("c%d",i), Form("SIS3316 channel %d",i));
		t[i]->Branch("n", &n, "n/I"); // baseline (pedestal)
		t[i]->Branch("s", s, "s[n]/F"); // waveform samples [n]
	}

	int word[65536]; // assume data from one channel is less than 65536 words
	char* byte = (char*) word; // index to count in byte instead of word
	input->read(byte,400); // get file header (400 bytes)
	for (int i=0; i<3; i++) printf("  file hdr word %d: 0x%08x\n",i,word[i]);
	while (input->good() && input->tellg()<fsize-80) { // 80: packet + mo + ch hdr
		input->read(byte,40); // get packet header (40 bytes), filled with 0xabba
		for (int i=0;i<1;i++) printf("packet hdr word %d: 0x%x\n",i,word[i]);
		for (int mo=0; mo<nm; mo++) { // loop over each module (card)
			input->read(byte,8); // get module header (8 bytes), including card number
			for (int i=0;i<1;i++) printf("module hdr word %d: 0x%08x\n",i,word[i]);
			for (int ch=0; ch<16; ch++) { // loop over each channel
				input->read(byte,32); // get channel header (8 words)
				int size=word[7]; if (size==0) continue; // skip any empty channel
				printf("size of channel data block: %u words\n", size);
				input->read(byte,word[7]*4); // get one block of channel data
				printf("channel: %u, ", short((*word&0xfff0)>>4));
				bitset<4> format(*byte); cout<<"format bits: "<<format<<endl;
				unsigned long timestamp = (unsigned long)((*word&0xffff0000)<<16);
				timestamp = timestamp | word[1];
				printf("time stamp: %lu\n", timestamp);
			}
		}
		break;
	}
	input->close();

	//for (int i=0; i<nm*nc; i++) t[i]->Write("", TObject::kOverwrite);
	//cout<<t[0]->GetEntries()<<" events saved in "<<output->GetName()<<endl;
	output->Close();
}
