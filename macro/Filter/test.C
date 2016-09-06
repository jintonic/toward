using namespace WAGE;
using namespace std;

void test()
{
  WF *wf = new WF;
  for (int i=0;i<500;i++)wf->smpl.push_back(0);
  wf->smpl.push_back(0);
  for(int i=0;i<15;i++)wf->smpl.push_back(i);
  for (int i=0;i<500;i++)wf->smpl.push_back(5);
  wf->AddNoise(1);
  WF * FT=wf->T2F();
  WF * tr=new WF();
  wf->GetTrapozoidE(-1,-1,tr);
  //for(int i=0;i<100;i++)cout<<FT->smpl[i]<<endl;
  FT->Draw("AC*");
}
