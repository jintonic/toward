#include "Waveform.h"
using namespace TOWARD;

#include <TMath.h>
#include<TFile.h>
//------------------------------------------------------------------------------
Waveform(): Pulse()
{
 	Reset();

	Tree.Branch("ns",&Nspl, "ns/I");
	Tree.Branch("s",Sample,"s[ns]/F");

	Tree.Branch("a",&Integral,"a/F");
	Tree.Branch("b",&Baseline,"b/F");
	Tree.Branch("h",&Height,"h/F");

	Tree.Branch("t", &Ttrg, "t/F");
	Tree.Branch("t1",&T10,  "t1/F");
	Tree.Branch("t9",&T90,  "t9/F");
	Tree.Branch("tb",&Tbgn, "tb/F");
	Tree.Branch("te",&Tend, "te/F");
	Tree.Branch("tp",&Tpeak,"tp/F");

	Tree.Branch("tau",&Tau,  "tau/F");
	Tree.Branch("tau2",&Tau2,"tau2/F");
	Tree.Branch("tau3",&Tau3,"tau3/F");

	Tree.Branch("np",&Npls, "np/I");
	Tree.Branch("pls",Pls);
}
//------------------------------------------------------------------------------
bool Waveform::IsSimilarTo(coNsplt Waveform& other) coNsplt
{
	bool similar = s.size()==other.s.size() && SamplingRate==other.SamplingRate;

	return similar;
}
//------------------------------------------------------------------------------
void Waveform::MakeSimilarTo(coNsplt Waveform& other)
{
	s.resize(other.s.size());
	SamplingRate = other.SamplingRate;
}

//------------------------------------------------------------------------------

Waveform& Waveform::operator+=(coNsplt Waveform& other)
{
	if (IsSimilarTo(other)==false) {
		Warning("operator+=", 
				"Only similar waveforms can be added together! Do nothing.");
		return *this;
	}

	for (size_t i=0; i<s.size(); i++) s[i] += other.s[i];

	return *this;
}

//------------------------------------------------------------------------------

Waveform& Waveform::operator-=(coNsplt Waveform& other)
{
	if (IsSimilarTo(other)==false) {
		Warning("operator-=", 
				"Only a similar waveform can be subtracted! Do nothing.");
		return *this;
	}

	for (size_t i=0; i<s.size(); i++) s[i] -= other.s[i];

	return *this;
}

//------------------------------------------------------------------------------

Waveform& Waveform::operator*=(coNsplt Waveform& other)
{
	if (IsSimilarTo(other)==false) {
		Warning("operator*=", 
				"Only similar waveforms can be multiplied! Do nothing.");
		return *this;
	}

	for (size_t i=0; i<s.size(); i++) s[i] *= other.s[i];

	return *this;
}

//------------------------------------------------------------------------------

Waveform& Waveform::operator/=(coNsplt Waveform& other)
{
	if(!IsSimilarTo(other)) {
		return *this;
	}

	for (size_t i=0; i<s.size(); i++)
		s[i] /= (other.s[i]==0.0) ? DBL_MIN : other.s[i];

	return *this;
}

//------------------------------------------------------------------------------

Waveform& Waveform::operator+=(double value)
{
	for (size_t i=0; i<s.size(); i++) s[i] += value;

	return *this;
}

//------------------------------------------------------------------------------

Waveform& Waveform::operator*=(double value)
{
	for (size_t i=0; i<s.size(); i++) s[i] *= value;

	return *this;
}

//------------------------------------------------------------------------------

Waveform& Waveform::operator/=(double value) 
{ 
	if (value==0.0) {
		Warning("operator/", "Cannot be divided by zero! Do nothing.");
		return *this;
	}

	return (*this) *= 1.0/value;
}

//------------------------------------------------------------------------------

void Waveform::Reset()
{
	ResetBit(kCalibrated);
	SamplingRate=0; Baseline=0; Noise=0; ctrg=0;
	Nspl=0; s.resize(0);
	Npls=0; pls.resize(0);
}

//------------------------------------------------------------------------------

int Waveform::GuessL()
{
	double maxdelta=0;
	int maxdeltalocation=0;
	for(int i=0;i<(int)s.size()-1;i++)
	{
		double delta=s[i]-s[i+1];
		if (delta<0)delta=-delta;
		if(delta>maxdelta)
		{
			maxdelta=delta;
			maxdeltalocation=i;
		}
	}
	int gusstheend=maxdeltalocation;
	int gussthebegin=maxdeltalocation;
	while(s[gusstheend+1]>s[gusstheend])gusstheend++;
	while(s[gussthebegin-1]<s[gussthebegin])gussthebegin--;
	return (gussthebegin-GuessG())/4;
} 
#include <iostream>
using namespace std;
//------------------------------------------------------------------------------
#include <stdlib.h> 
int Waveform::GuessG()
{
	double maxslope=0;
	int gstart=0,gend=s.size()-1;
	int l=s.size()/10;
	for(int i=l;i<s.size();i++)
	{
		if((s[i]-s[0])/i>maxslope)
		{
			maxslope=(s[i]-s[0])/i;
			gend=i;
		}
	}
	maxslope=0;
	for(int i=s.size()-l;i-->0;)
	{
		if((s[s.size()-1]-s[i])/(s.size()-i)>maxslope)
		{
			maxslope=  (s[s.size()-1]-s[i])/(s.size()-i);
			gstart=i;
		}
	}
	return (gend-gstart);
}

//------------------------------------------------------------------------------

double Waveform::GetTrapozoidE(int L=-1,int G=-1,Waveform * out=NULL)
{
	if(L==-1)L=GuessL();
	if(G==-1)G=GuessG()*2;
	cout<<G<<endl;
	int n=s.size();
	if(2*L+G>n)
	{
		Warning("Filter","too large Length or Gap");
		return -1;
	}
	std::vector<double> mid;
	for (int n=0;n<(int)s.size();n++)
	{
		mid.push_back(0);
		for(int i=0;i<L;i++)
		{
			int m=mid.size();
			if (m-i<=0)break;
			mid[m-i-1]+=s[n];
		}
	}
	for(int i=0;i<L-1;i++)mid.pop_back();
	for(int i=0;i<(int)mid.size();i++)mid[i]=mid[i]/L;
	std::vector<double> needed;
	for (int i=0;i<n-2*L-G;i++)needed.push_back(mid[i+G+L]-mid[i]);
	if(out)
	{
		out->s=needed;
	}
	double max=needed[0],min=needed[0];
	for(size_t i=0;i<needed.size();i++)
	{
		if(needed[i]>max)max=needed[i];
		if(needed[i]<min)min=needed[i];
	}

	return max-min;
}
//------------------------------------------------------------------------------
#include <cmath>
#include <iostream>
using namespace std;
void Waveform::T2F()
{
	std::vector<double> needed=s;
	coNsplt double PI  =3.141592653589793238463;
	for(int i=0;i<(int)needed.size();i++)
	{
		double rxk=0;
		double ixk=0;
		for(int j=0;j<(int)needed.size()-1;j++)
		{
			double po=2*PI*i*(j)/needed.size();
			rxk+=cos(po)*needed[j];
			ixk+=-sin(po)*needed[j];
		}
		Re.push_back(rxk);
		Im.push_back(ixk);
	}
}
//------------------------------------------------------------------------------
void Waveform::F2T()
{
	const int N=Re.size();
	for(int i=0;i<N;i++)
	{
		double xk=cos(0)*Re[0]+cos(2*Pi()*i*N/2/N)*Re[N/2];
		double xxk=-sin(0)*Re[0]*2-sin(2*Pi()*i/2)*Re[N/2]*2;
		for(int j=1;j<N/2;j++)
		{
			double po=(2*Pi()*i*j/N);
			xk+=cos(po)*Re[j]*2;
			xxk+=sin(po)*Im[j]*-2;
		}
		s.push_back((xk+xxk)/N);
	}
}
//------------------------------------------------------------------------------
#include <TRandom3.h>
void Waveform::AddNoise(int s)
{
	TRandom3 *r=new TRandom3();
	for(int i=0;i<(int)s.size();i++)s[i]+=r->Gaus(0,s);
}
//------------------------------------------------------------------------------
void Waveform::Draw(Option_t *chopt="",int j=0)
{
	int n=s.size();
	double *x,*y;
	x=new double[n];
	y=new double[n];
	for(int i=0;i<n;i++){
		if(j==0)x[i]=s[i];
		if(j==1)x[i]=Re[i];
		if(j==2)x[i]=Im[i];
		y[i]=i;
	}
	TGraph *g=new TGraph(s.size(),y,x);
	g->Draw(chopt);
	delete []x;
	delete []y;

}
#include <cstdlib>
#include <fstream>
using namespace std;

#include <dirent.h>
#include <sys/stat.h>

#include <TSystem.h>

//------------------------------------------------------------------------------

void Waveforms::Initialize(const char* db)
{
   fDB=db;
   struct stat st;
   if (stat(db,&st)!=0) { // if not exist
      Info("Initialize", "cannot open %s", db);
      Info("Initialize", "check $TOWARDSYS");
      const char *root = gSystem->Getenv("TOWARDSYS");
      if (root==0) {
         Warning("Initialize", "$TOWARDSYS is not set, return");
         return;
      }
      Info("Initialize", "$TOWARDSYS=%s",root);
      fDB=root;

      if (fDB.EndsWith("/")) fDB+="electrode/";
      else fDB+="/electrode/";
   }

   Info("Initialize", "scan %s", fDB.Data());
   struct dirent **ch;
   Int_t n = scandir(fDB.Data(), &ch, 0, alphasort);
   if (n<0) {
      Warning("Initialize", "cannot open %s", fDB.Data());
      Warning("Initialize", "return");
      return;
   }

   wf.SetClass("TOWARD::Waveform",nch);
   for (Int_t i=0; i<n; i++) {
      if (ch[i]->d_name[0]=='.' || i>=nch+2) {
         free(ch[i]);
         continue;
      }

      Waveform *aWaveform = Map(atoi(ch[i]->d_name));
      if (aWaveform) {
         if (aWaveform->ode.id>=0) {
            LoadTimeOffset(aWaveform);
            LoadVoltage(aWaveform);
            LoadStatus(aWaveform);
         }
         aWaveform->ode.Dump();
      }

      free(ch[i]);
   }
   free(ch);
}

//------------------------------------------------------------------------------

Waveform* Waveforms::Map(int ch)
{
   if (ch<0) {
      Error("Map", "cannot handle channel number %d", ch);
      Error("Map", "return 0");
      return 0;
   }

   ifstream file(Form("%s/%d/mapping.txt", fDB.Data(), ch), ios::in);
   if (!(file.is_open())) {
      Error("Map", "cannot read %s/%d/mapping.txt", fDB.Data(), ch);
      Error("Map", "return 0");
      return 0;
   }

   Waveform *aWaveform = 0;
   Int_t runnum, id;
   while (file>>runnum>>id) {
      if (run<runnum) continue;
      aWaveform = (Waveform*) wf.CoNspltructedAt(ch);
      break;
   }

   file.close();

   if (!aWaveform) {
      Error("Map", "no electrode mapBaseline to ch %d in run %d", ch, run);
      Error("Map", "return 0");
      return 0;
   }

   aWaveform->ode.ch = ch;
   aWaveform->ode.id = id;
   return aWaveform;
}

//------------------------------------------------------------------------------

void Waveforms::LoadTimeOffset(Waveform* aWaveform)
{
   if (!aWaveform) {
      Error("LoadTimeOffset", "NULL pointer to electrode, return");
      return;
   }

   ifstream file(Form("%s/%d/dt.txt", fDB.Data(), aWaveform->ode.ch));
   if (!(file.is_open())) {
      Error("LoadTimeOffset", "cannot read %s/%d/dt.txt", 
            fDB.Data(), aWaveform->ode.ch);
      Error("LoadTimeOffset", "return");
      return;
   }

   Int_t runnum;
   double dt;
   while (file>>runnum>>dt) {
      if (run<runnum) continue;
      aWaveform->ode.dt = dt*Nspl;
      break;
   }

   file.close();
}

//------------------------------------------------------------------------------

void Waveforms::LoadVoltage(Waveform* aWaveform)
{
   if (!aWaveform) {
      Error("LoadVoltage", "NULL pointer to electrode, return");
      return;
   }

   ifstream file(Form("%s/%d/voltage.txt", fDB.Data(), aWaveform->ode.ch));
   if (!(file.is_open())) {
      Error("LoadVoltage", "cannot read %s/%d/voltage.txt",
            fDB.Data(), aWaveform->ode.ch);
      Error("LoadVoltage", "return");
      return;
   }

   Int_t runnum;
   double voltage;
   while (file>>runnum>>voltage) {
      if (run<runnum) continue;
      aWaveform->ode.v = voltage;
      break;
   }

   file.close();
}

//------------------------------------------------------------------------------

void Waveforms::LoadStatus(Waveform* aWaveform)
{
   if (!aWaveform) {
      Error("LoadStatus", "NULL pointer to electrode, return");
      return;
   }

   ifstream file(Form("%s/%d/status.txt", fDB.Data(), aWaveform->ode.ch));
   if (!(file.is_open())) {
      Error("LoadStatus", "cannot read %s/%d/status.txt",
            fDB.Data(), aWaveform->ode.ch);
      Error("LoadStatus", "return");
      return;
   }

   Int_t runnum;
   TString st;
   while (file>>runnum>>st) {
      if (run<runnum) continue;
      aWaveform->ode.SetStatus(st);
      break;
   }

   file.close();
}

//------------------------------------------------------------------------------

Waveform* Waveforms::At(uNspligned short i) const
{
   if (i>=wf.GetEntries()) {
      Warning("operator[]", 
            "index %d >= %d (max), return 0", i, wf.GetEntries());
      return 0;
   }
   return (Waveform*) wf.At(i);
}

//------------------------------------------------------------------------------

void Waveforms::Save()
{
   TTree t[n] = {0};
   for (size_t i=0; i<n; i++) {
      t[i] = new TTree(Form("t%d",i), Form("ch%d",I));
      wf.At(i)->SetTree(t[i]); // create branches
   }
}
//------------------------------------------------------------------------------

