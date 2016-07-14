#include <cstdlib>
#include <fstream>
using namespace std;

#include <dirent.h>
#include <sys/stat.h>

#include <TSystem.h>

#include <UNIC/Units.h>
using namespace UNIC;

#include "WF.h"
#include "WFs.h"
using namespace WAGE;

//------------------------------------------------------------------------------

void WFs::Initialize(const char* db)
{
   fDB=db;
   struct stat st;
   if (stat(db,&st)!=0) { // if not exist
      Info("Initialize", "cannot open %s", db);
      Info("Initialize", "check $WAGESYS");
      const char *root = gSystem->Getenv("WAGESYS");
      if (root==0) {
         Warning("Initialize", "$WAGESYS is not set, return");
         return;
      }
      Info("Initialize", "$WAGESYS=%s",root);
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

   wf.SetClass("WAGE::WF",nch);
   for (Int_t i=0; i<n; i++) {
      if (ch[i]->d_name[0]=='.' || i>=nch+2) {
         free(ch[i]);
         continue;
      }

      WF *aWF = Map(atoi(ch[i]->d_name));
      if (aWF) {
         if (aWF->ode.id>=0) {
            LoadTimeOffset(aWF);
            LoadVoltage(aWF);
            LoadStatus(aWF);
         }
         aWF->ode.Dump();
      }

      free(ch[i]);
   }
   free(ch);
}

//------------------------------------------------------------------------------

WF* WFs::Map(int ch)
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

   WF *aWF = 0;
   Int_t runnum, id;
   while (file>>runnum>>id) {
      if (run<runnum) continue;
      aWF = (WF*) wf.ConstructedAt(ch);
      break;
   }

   file.close();

   if (!aWF) {
      Error("Map", "no electrode mapped to ch %d in run %d", ch, run);
      Error("Map", "return 0");
      return 0;
   }

   aWF->ode.ch = ch;
   aWF->ode.id = id;
   return aWF;
}

//------------------------------------------------------------------------------

void WFs::LoadTimeOffset(WF* aWF)
{
   if (!aWF) {
      Error("LoadTimeOffset", "NULL pointer to electrode, return");
      return;
   }

   ifstream file(Form("%s/%d/dt.txt", fDB.Data(), aWF->ode.ch));
   if (!(file.is_open())) {
      Error("LoadTimeOffset", "cannot read %s/%d/dt.txt", 
            fDB.Data(), aWF->ode.ch);
      Error("LoadTimeOffset", "return");
      return;
   }

   Int_t runnum;
   double dt;
   while (file>>runnum>>dt) {
      if (run<runnum) continue;
      aWF->ode.dt = dt*ns;
      break;
   }

   file.close();
}

//------------------------------------------------------------------------------

void WFs::LoadVoltage(WF* aWF)
{
   if (!aWF) {
      Error("LoadVoltage", "NULL pointer to electrode, return");
      return;
   }

   ifstream file(Form("%s/%d/voltage.txt", fDB.Data(), aWF->ode.ch));
   if (!(file.is_open())) {
      Error("LoadVoltage", "cannot read %s/%d/voltage.txt",
            fDB.Data(), aWF->ode.ch);
      Error("LoadVoltage", "return");
      return;
   }

   Int_t runnum;
   double voltage;
   while (file>>runnum>>voltage) {
      if (run<runnum) continue;
      aWF->ode.v = voltage;
      break;
   }

   file.close();
}

//------------------------------------------------------------------------------

void WFs::LoadStatus(WF* aWF)
{
   if (!aWF) {
      Error("LoadStatus", "NULL pointer to electrode, return");
      return;
   }

   ifstream file(Form("%s/%d/status.txt", fDB.Data(), aWF->ode.ch));
   if (!(file.is_open())) {
      Error("LoadStatus", "cannot read %s/%d/status.txt",
            fDB.Data(), aWF->ode.ch);
      Error("LoadStatus", "return");
      return;
   }

   Int_t runnum;
   TString st;
   while (file>>runnum>>st) {
      if (run<runnum) continue;
      aWF->ode.SetStatus(st);
      break;
   }

   file.close();
}

//------------------------------------------------------------------------------

WF* WFs::At(unsigned short i) const
{
   if (i>=wf.GetEntries()) {
      Warning("operator[]", 
            "index %d >= %d (max), return 0", i, wf.GetEntries());
      return 0;
   }
   return (WF*) wf.At(i);
}

//------------------------------------------------------------------------------

