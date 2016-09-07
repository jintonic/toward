using namespace WAGE;
using namespace std;

void test()
{
  WF *wf = new WF;
  for (int i=0;i<500;i++)wf->smpl.push_back(0);
  wf->smpl.push_back(0);
  for(int i=0;i<50;i++)wf->smpl.push_back(i);
  for (int i=0;i<500;i++)wf->smpl.push_back(50);
  wf->Draw("Al");
  wf->AddNoise(1);
  wf->Draw("samel");

  WF * FT=wf->T2F();
  TCanvas *c = new TCanvas;
  FT->Draw("Al");

  WF * tr=new WF();
  wf->GetTrapozoidE(-1,-1,tr);
  TCanvas *d = new TCanvas;
  tr->Draw("al");
}
