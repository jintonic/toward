using namespace WAGE;
using namespace std;

void test()
{
  WF *wf = new WF;
  TRandom3 *r=new TRandom3();
  for (int i=0;i<50;i++)wf->smpl.push_back(0);
  wf->smpl.push_back(0);
  for(int i=0;i<5;i++)wf->smpl.push_back(i);
  for (int i=0;i<50;i++)wf->smpl.push_back(5);
  for(int i=0;i<wf->smpl.size();i++)wf->smpl[i]+=r->Gaus();
  vector<double> out=wf->FT();
  int n=106;
  double x[106],y[106];
  for(int i=0;i<n;i++){
    x[i]=out[i];
    y[i]=i;
  }
  cout<<x[2]<<endl;
  //for(int i=0;i<out.size();i++)y.push_back(i);
  TGraph *g=new TGraph(out.size(),y,x);
  g->Draw("AC*");
}
