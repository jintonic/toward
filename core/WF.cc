#include <float.h>

#include "WF.h"

using namespace WAGE;

bool WF::IsSimilarTo(const WF& other) const
{
   bool similar = smpl.size()==other.smpl.size() && freq==other.freq;

   return similar;
}

//------------------------------------------------------------------------------

void WF::MakeSimilarTo(const WF& other)
{
   smpl.resize(other.smpl.size());
   freq = other.freq;
}

//------------------------------------------------------------------------------

WF& WF::operator+=(const WF& other)
{
   if (IsSimilarTo(other)==false) {
      Warning("operator+=", 
            "Only similar waveforms can be added together! Do nothing.");
      return *this;
   }

   for (size_t i=0; i<smpl.size(); i++) smpl[i] += other.smpl[i];

   return *this;
}

//------------------------------------------------------------------------------

WF& WF::operator-=(const WF& other)
{
   if (IsSimilarTo(other)==false) {
      Warning("operator-=", 
            "Only a similar waveform can be subtracted! Do nothing.");
      return *this;
   }

   for (size_t i=0; i<smpl.size(); i++) smpl[i] -= other.smpl[i];

   return *this;
}

//------------------------------------------------------------------------------

WF& WF::operator*=(const WF& other)
{
   if (IsSimilarTo(other)==false) {
      Warning("operator*=", 
            "Only similar waveforms can be multiplied! Do nothing.");
      return *this;
   }

   for (size_t i=0; i<smpl.size(); i++) smpl[i] *= other.smpl[i];

   return *this;
}

//------------------------------------------------------------------------------

WF& WF::operator/=(const WF& other)
{
   if(!IsSimilarTo(other)) {
      return *this;
   }

   for (size_t i=0; i<smpl.size(); i++)
      smpl[i] /= (other.smpl[i]==0.0) ? DBL_MIN : other.smpl[i];

   return *this;
}

//------------------------------------------------------------------------------

WF& WF::operator+=(double value)
{
   for (size_t i=0; i<smpl.size(); i++) smpl[i] += value;

   return *this;
}

//------------------------------------------------------------------------------

WF& WF::operator*=(double value)
{
   for (size_t i=0; i<smpl.size(); i++) smpl[i] *= value;

   return *this;
}

//------------------------------------------------------------------------------

WF& WF::operator/=(double value) 
{ 
   if (value==0.0) {
      Warning("operator/", "Cannot be divided by zero! Do nothing.");
      return *this;
   }

   return (*this) *= 1.0/value;
}

//------------------------------------------------------------------------------

void WF::Reset()
{
   ResetBit(kCalibrated);
   freq=0; ped=0; prms=0; ctrg=0;
   ns=0; smpl.resize(0);
   np=0; pls.resize(0);
}

//------------------------------------------------------------------------------

int WF::GussL()
{
  double maxdelta=0;
  int maxdeltalocation=0;
  for(int i=0;i<(int)smpl.size()-1;i++)
  {
    double delta=smpl[i]-smpl[i+1];
    if (delta<0)delta=-delta;
    if(delta>maxdelta)
    {
      maxdelta=delta;
      maxdeltalocation=i;
    }
  }
  int gusstheend=maxdeltalocation;
  int gussthebegin=maxdeltalocation;
  while(smpl[gusstheend+1]>smpl[gusstheend])gusstheend++;
  while(smpl[gussthebegin-1]<smpl[gussthebegin])gussthebegin--;
  return (gussthebegin-GussG())/4;
} 

//------------------------------------------------------------------------------
#include <stdlib.h> 
int WF::GussG()
{
  double maxdelta=0;
  int maxdeltalocation=0;
  for(int i=0;i<(int)smpl.size()-1;i++)
  {
    double delta=smpl[i]-smpl[i+1];
    if (delta<0)delta=-delta;
    if(delta>maxdelta)
    {
      maxdelta=delta;
      maxdeltalocation=i;
    }
  }
  int gusstheend=maxdeltalocation;
  int gussthebegin=maxdeltalocation;
  while(smpl[gusstheend+1]>smpl[gusstheend])gusstheend++;
  while(smpl[gussthebegin-1]<smpl[gussthebegin])gussthebegin--;
  return (gusstheend-gussthebegin)*10;
}

//------------------------------------------------------------------------------
double WF::GetTrapozoidE(int L=-1,int G=-1,WF * out=NULL)
{
  if(L==-1)L=GussL();
  if(G==-1)G=GussG()*2;
  int n=smpl.size();
  if(2*L+G>n)
  {
    Warning("Filter","too large Length or Gap");
    return -1;
  }
  std::vector<double> mid;
  for (int n=0;n<(int)smpl.size();n++)
  {
    mid.push_back(0);
    for(int i=0;i<L;i++)
    {
      int m=mid.size();
      if (m-i<=0)break;
      mid[m-i-1]+=smpl[n];
    }
  }
  for(int i=0;i<L-1;i++)mid.pop_back();
  for(int i=0;i<(int)mid.size();i++)mid[i]=mid[i]/L;
  std::vector<double> needed;
  for (int i=0;i<n-2*L-G;i++)needed.push_back(mid[i+G+L]-mid[i]);
  if(out)
  {
    out->smpl=needed;
  }
  double max=needed[0],min=needed[0];
  for(int i=0;i<needed.size();i++)
  {
    if(needed[i]>max)max=needed[i];
    if(needed[i]<min)min=needed[i];
  }
    
  return max-min;
}
//------------------------------------------------------------------------------
#include <cmath>
#include <iostream>
using namespace std;
WF * WF::T2F()
{
  std::vector<double> needed=smpl;
  std::vector<double> out; 
  const double PI  =3.141592653589793238463;
  for(int i=0;i<(int)needed.size();i++)
  {
    double xk=0;
    for(int j=0;j<(int)needed.size()-1;j++)
    {
      double po=cos(-2*PI*i*j/needed.size());
      xk+=po*needed[j];
    }
    out.push_back(xk);
  }
  WF * re=new WF();
  re->smpl=out;
  return re;
}
//------------------------------------------------------------------------------
void WF::F2T(WF * wf)
{
  std::vector<double> needed=wf->smpl;
  const double PI  =3.141592653589793238463;
  for(int i=0;i<(int)needed.size();i++)
  {
    double xk=0;
    for(int j=0;j<(int)needed.size()-1;j++)
    {
      double po=cos(-2*PI*i*j/needed.size());
      xk+=po*needed[j];
    }
    smpl.push_back(xk/needed.size());
  }
}
//------------------------------------------------------------------------------
#include <TRandom3.h>
void WF::AddNoise(int s)
{
  TRandom3 *r=new TRandom3();
  for(int i=0;i<(int)smpl.size();i++)smpl[i]+=r->Gaus(0,s);
}
//------------------------------------------------------------------------------
void WF::Draw(Option_t *chopt="")
{
  int n=smpl.size();
  double *x,*y;
  x=new double[n];
  y=new double[n];
  for(int i=0;i<n;i++){
    x[i]=smpl[i];
    y[i]=i;
  }
  TGraph *g=new TGraph(smpl.size(),y,x);
  g->Draw(chopt);
  delete []x;
  delete []y;

}
