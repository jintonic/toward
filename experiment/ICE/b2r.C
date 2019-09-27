int main(int argc, char** argv)
{
   TString input(argv[1]);
   TFile output(input.Replace(7,3,"root"),"recreate");
   CAENDT5751 daq(argv[1]); // create TFile

   int n = daq.GetN(); 
   for(size_t i=0; i<daq.GetNevt(); i++) {
      for(size_t ch=0; ch<daq.GetNch(); ch++) {
         daq.Scan(ch);
      }
   }
    
}
