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
  return (gussthebegin-GussG())/2;
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
  return gusstheend-gussthebegin;
}

//------------------------------------------------------------------------------
std::vector<double> WF::Filter(int L,int G)
{
  if(L==-1)L=GussL();
  if(G==-1)G=GussG()*2;
  int n=smpl.size();
  if(2*L+G>n)
  {
    Warning("Filter","too large Length or Gap");
    return smpl;
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
  return needed;
}
//------------------------------------------------------------------------------
#include <cmath>
#include <iostream>
using namespace std;
std::vector<double> WF::FT()
{
  std::vector<double> needed=Filter();
  std::vector<double> out; 
  for(int i=0;i<(int)needed.size();i++)
  {
    double xk=0;
    for(int j=0;j<(int)needed.size()-1;j++)
    {
      double po=pow(-1,2.0/(double)needed.size()*i*j);
      if (!(po==1||po==-1))po=-1; 
      xk+=po*needed[j];
    }
    out.push_back(xk);
  }
  return out;
}
