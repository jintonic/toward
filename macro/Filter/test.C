using namespace WAGE;
using namespace std;

void test()
{
  WF *wf = new WF;
  /*for (int i=0;i<500;i++)wf->smpl.push_back(0);
  wf->smpl.push_back(0);
  for(int i=0;i<50;i++)wf->smpl.push_back(i);
  for (int i=0;i<500;i++)wf->smpl.push_back(50);
  */
  for(int i=0;i<2000;i++)wf->smpl.push_back(cos((double)i/2000*2*3.1415926*10));
  wf->Draw("Al",0);
  //wf->AddNoise(1);
  wf->Draw("samel",0);

  wf->T2F();
  TCanvas *c = new TCanvas;
  wf->Draw("Al",1);
  wf->Draw("same",2);

  wf->F2T();
  TCanvas *c = new TCanvas;
  WF * tr=new WF();
  wf->Draw("Al",0);

  wf->GetTrapozoidE(-1,-1,tr);
  TCanvas *d = new TCanvas;
  tr->Draw("al",0);
}
