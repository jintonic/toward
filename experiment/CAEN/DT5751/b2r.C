#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

#include <sys/stat.h>

#include <TFile.h>
#include <TTree.h>

#include "Reader.h"
using namespace GEE;

/**
 * convert binary file to ROOT format.
 */
void b2r(int run, int sub=1, const char* dir=".")
{
   Reader input(run, sub, dir);
   TFile output(Form("%s.root",input.GetFile()),"recreate");
   TTree t("t",Form("events in run %d, sub run %d", run, sub));
   t.Branch("evt", "GEE::WFs", &input, 32000, 3);

   cout<<input.GetEntries()<<" entries to be processed"<<endl;
   for (int i=0; i<input.GetEntries(); i++) {
      input.GetEntry(i);
      for (int ch=0; ch<input.nch; ch++) input.Scan(ch);
      t.Fill();
      if (i%5000==0 || i==input.GetEntries()-1)
         cout<<"entry "<<i<<" processed"<<endl;
   }
   output.Write("t", TObject::kWriteDelete);
   output.Close();
   cout<<"done"<<endl;
}

int main(int argc, char** argv)
{
   if (argc<2 || argc>4) {
      cout<<"usage: "<<endl;
      cout<<"./b2r run <subrun> </path/to/data/dir>"<<endl;
      return 1;
   }

   int run = atoi(argv[1]);
   if (run<=0 || run>999999) {
      cerr<<"invalid run number: "<<argv[1]<<endl;
      return 1;
   }
   if (argc>2) {
      int sub = atoi(argv[2]);
      if (sub<=0 || sub>999999) {
         cerr<<"invalid sub run number: "<<argv[2]<<endl;
         return 1;
      }
      if (argc>3) {
         struct stat st;
         if (stat(argv[3],&st)!=0) {
            cerr<<"cannot open "<<argv[3]<<endl;
            return 1;
         }
         b2r(run, sub, argv[3]);
         return 0;
      }
      b2r(run, sub);
      return 0;
   }
   b2r(run);
   return 0;
}
