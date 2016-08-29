using namespace WAGE;

void test()
{
  WF *wf = new WF;
  for (int i=0;i<50;i++)wf->smpl.push_back(0);
  wf->smpl.push_back(-1);
  for(int i=0;i<5;i++)wf->smpl.push_back(i);
  for (int i=0;i<50;i++)wf->smpl.push_back(4);
  vector<double> out=wf->Filter();
  int n=106;
  double x[106],y[106];
  for(int i=0;i<n;i++){
    x[i]=out[i];
    y[i]=i;
  }
  //for(int i=0;i<out.size();i++)y.push_back(i);
  TGraph *g=new TGraph(out.size(),y,x);
  g->Draw("AC*");
}
