static const float ns = 1;

#include <TRandom.h>
#include <cmath>
using namespace std;
class SiPM1PE
{
	public:
		int Nsmpl;
		std::vector<float> Sample;

		float H;
		int Tbgn;

		float TauFast;
		float TauSlow;

		float SigmaT;
		float SigmaH;

		SiPM1PE(float h=5, int t=20) : Nsmpl(1000), H(h), Tbgn(t), TauFast(1*ns), TauSlow(300*ns),
		SigmaT(5*ns), SigmaH(1) {
			Sample.clear();
			for (int i=0; i<Tbgn; i++) {
				float s = gRandom->Gaus(0,SigmaH);
				Sample.push_back(s);
			}
			for (int i=Tbgn; i<Nsmpl; i++) {
				float s = exp(-(i-Tbgn)/TauFast);
				Sample.push_back(H*s+gRandom->Gaus(0,SigmaH));
			}
		}
};

const int maxNpls=100;
const int maxNch=1000;
const int maxNsmpl=10000;

#include <TTree.h>
#include <TFile.h>
int main(int argc, char** argv)
{
	int Npls=1; ///< number of pulses in a waveform
	int Nch=2;  ///< number of electronic channels
	int Nsmpl=1000;///< number of samples in a waveform

	float Area[maxNpls]={0};   ///< area of pulse
	float Height[maxNpls]={0}; ///< height of pulse
	float Ttrg[maxNpls]={0};   ///< trigger time of pulse
	float T10[maxNpls]={0};    ///< time when pulse rises to 10% of its Height
	float T90[maxNpls]={0};    ///< time when pulse rises to 90% of its Height
	float Tbgn[maxNpls]={0};   ///< time when pulse begins
	float Tend[maxNpls]={0};   ///< time when pulse ends
	float Tpeak[maxNpls]={0};  ///< time when pulse reaches its Height
	float Tdecay[maxNpls]={0}; ///< decay time of pulses
	float Sample[maxNsmpl]={0};///< value of sample

	TFile *output = new TFile("output.root","recreate");
	TTree* ch[maxNch] = {0}; ///< data from electronic channels
	for (int i=0; i<Nch; i++) {
		ch[i] = new TTree(Form("c%zu",i),Form("waveform in channel %zu",i));
		ch[i]->Branch("np",&Npls, "np/i");
		ch[i]->Branch("ns",&Nsmpl,"ns/i");
		ch[i]->Branch("a",Area,  "a[np]/F");
		ch[i]->Branch("h",Height,"h[np]/F");
		ch[i]->Branch("t",Ttrg,  "t[np]/F");
		ch[i]->Branch("t1",T10,  "t1[np]/F");
		ch[i]->Branch("t9",T90,  "t9[np]/F");
		ch[i]->Branch("tb",Tbgn,  "tb[np]/F");
		ch[i]->Branch("te",Tend,  "te[np]/F");
		ch[i]->Branch("ts",Tpeak, "ts[np]/F");
		ch[i]->Branch("td",Tdecay,"td[np]/F");
		ch[i]->Branch("s",Sample,"s[ns]/F");
	}

	//TString input(argv[1]);
	//TFile output(input.Replace(7,3,"root"),"recreate");
	//CAENDT5751 daq(argv[1]); // create TFile

	for (int n=0; n<10; n++) {
		for (int i=0; i<Nch; i++) {
			SiPM1PE wf(gRandom->Uniform(500),gRandom->Integer(800));
			for (int j=0; j<Nsmpl; j++) Sample[j]=wf.Sample.at(j);
			ch[i]->Fill();
		}
	}

	output->Write();
}
