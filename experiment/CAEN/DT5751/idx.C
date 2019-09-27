/**
 * index a binary file and dump to stdout
 *
 * format:
 * - one line one event
 * - each line: start length
 */
#include "Reader.h"

void idx(Int_t run=11111, Int_t sub=1, const char* dir="./")
{
   Reader input(run, sub, dir);
   input.DumpIndex();
   input.Close();
}

#include <cstdlib>
#include <iostream>
using namespace std;
#include <sys/stat.h>

int main(int argc, char** argv)
{
   if (argc<2 || argc>4) {
      cout<<"usage: "<<endl;
      cout<<"./idx run <subrun> </path/to/data/dir>"<<endl;
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
         idx(run, sub, argv[3]);
         return 0;
      }
      idx(run, sub);
      return 0;
   }
   idx(run);
   return 0;
}
