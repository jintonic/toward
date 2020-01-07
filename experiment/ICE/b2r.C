const int maxNpls=100;
const int maxNch=1000;
const int maxNsmpl=10000;

int main(int argc, char** argv)
{
   int Npls; ///< number of pulses in a waveform
   int Nch;  ///< number of electronic channels
   int Nsmpl;///< number of samples in a waveform

   double Area[maxNpls]={0};   ///< area of pulse
   double Height[maxNpls]={0}; ///< height of pulse
   double Ttrg[maxNpls]={0};   ///< trigger time of pulse
   double T10[maxNpls]={0};    ///< time when pulse rises to 10% of its Height
   double T90[maxNpls]={0};    ///< time when pulse rises to 90% of its Height
   double Tbgn[maxNpls]={0};   ///< time when pulse begins
   double Tend[maxNpls]={0};   ///< time when pulse ends
   double Tsummit[maxNpls]={0};///< time when pulse reaches its Height
   double Tdecay[maxNpls]={0}; ///< decay time of pulses
   double Sample[maxNsmpl]={0};///< value of sample
   
   for (int i=0; i<daq.Nch(); i++) {
      c[i]->Branch("np",&Npls, "np/i");
      c[i]->Branch("ns",&Nsmpl,"ns/i");
      c[i]->Branch("a",Area,"a[np]/F");
      c[i]->Branch("h",Height,"h[np]/F");
      c[i]->Branch("t",Ttrg,"t[np]/F");
      c[i]->Branch("t1",T1,"t1[np]/F");
      c[i]->Branch("t9",T9,"t9[np]/F");
      c[i]->Branch("tb",Tb,"tb[np]/F");
      c[i]->Branch("te",Te,"te[np]/F");
      c[i]->Branch("ts",Ts,"ts[np]/F");
      c[i]->Branch("td",Td,"td[np]/F");
      c[i]->Branch("s",Sample,"s[ns]/F");
   }
   TString input(argv[1]);
   TFile output(input.Replace(7,3,"root"),"recreate");
   CAENDT5751 daq(argv[1]); // create TFile

   const int N = daq.GetN(); 
   for(size_t i=0; i<daq.GetNevt(); i++) {
      for(size_t ch=0; ch<daq.GetNch(); ch++) {
         daq.Scan(ch);
      }
   }
    
}
