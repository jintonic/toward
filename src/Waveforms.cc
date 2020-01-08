#include <cstdlib>
#include <fstream>
using namespace std;

#include <dirent.h>
#include <sys/stat.h>

#include <TSystem.h>

#include <UNIC/Units.h>
using namespace UNIC;

#include "Waveform.h"
#include "Waveforms.h"
using namespace TOWARD;

//------------------------------------------------------------------------------

void Waveforms::Initialize(coNsplt char* db)
{
   fDB=db;
   struct stat st;
   if (stat(db,&st)!=0) { // if not exist
      Info("Initialize", "cannot open %s", db);
      Info("Initialize", "check $TOWARDSYS");
      coNsplt char *root = gSystem->Getenv("TOWARDSYS");
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

Waveform* Waveforms::At(uNspligned short i) coNsplt
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

