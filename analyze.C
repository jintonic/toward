void analyze(const char* run_folder="2020/02140956")
{
	TTreeViewer *viewer = new TTreeViewer;
	viewer->SetScanRedirect(kTRUE); // avoid involving terminal output

	TFile *f; TTree *t;
	for (int ch=0; ch<8; ch++) {
		if (gSystem->Which(run_folder, Form("wave%d.root",ch))) {
			f = new TFile(Form("%s/wave%d.root",run_folder,ch));
			t = (TTree*) f->Get("t"); t->SetName(Form("t%d",ch));
			viewer->AppendTree(t);
		}
	}

	// items (0:x, 1:y, 2:z, 3:cut, 4:scan, users must start at 5)
	viewer->ExpressionItem(5)->SetExpression("b", "~Baseline", kFALSE);
	viewer->ExpressionItem(6)->SetExpression("db", "~Baseline RMS", kFALSE);
	viewer->ExpressionItem(7)->SetExpression("h", "~Pulse height", kFALSE);
	viewer->ExpressionItem(8)->SetExpression("l", "~Lowest point", kFALSE);
	viewer->ExpressionItem(9)->SetExpression("is", "~Saturation flag", kFALSE);
	viewer->ExpressionItem(10)->SetExpression("tt", "~Trigger Time", kFALSE);
	viewer->ExpressionItem(11)->SetExpression("th", "~Location of highest point", kFALSE);
	viewer->ExpressionItem(12)->SetExpression("tl", "~Location of lowest point", kFALSE);
	viewer->ExpressionItem(13)->SetExpression("ttt", "~Trigger time tag", kFALSE);
	viewer->ExpressionItem(14)->SetExpression("a", "~Area", kFALSE);

	// records (for better control)
	TTVSession* session = new TTVSession(viewer);
	viewer->SetSession(session);
	// 1st record
	TTVRecord* record = session->AddRecord(kTRUE);
	session->SetRecordName("db:b");
	record->fX        = "b";
	record->fY        = "db";
	record->fZ        = "";
	record->fCut      = "";
	record->fXAlias   = "~Baseline";
	record->fYAlias   = "~Baseline RMS";
	record->fZAlias   = "-empty-";
	record->fCutAlias = "-empty-";
	record->fOption   = "colz";
	record->fScanRedirected = kFALSE;
	record->fCutEnabled = kTRUE;

	// 2nd
	record = session->AddRecord(kTRUE);
	session->SetRecordName("Waveforms");
	record->fX        = "Iteration$";
	record->fY        = "s";
	record->fZ        = "";
	record->fCut      = "Entry$<10";
	record->fXAlias   = "~Time";
	record->fYAlias   = "~ADC unit";
	record->fZAlias   = "-empty-";
	record->fCutAlias = "~10evts";
	record->fOption   = "l";
	record->fScanRedirected = kFALSE;
	record->fCutEnabled = kTRUE;

	session->First(); // switch to first record

	viewer->Resize(630,330); // otherwise column width would be updated
}
