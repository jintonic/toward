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
