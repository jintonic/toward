#include "iostream"
using namespace std;
#include "Pulse.h"

using namespace TOWARD;

Pulse::Pulse(const Pulse& pls) : TObject(pls),
   bgn(pls.bgn),
   end(pls.end),
   ithr(pls.ithr),
   ih(pls.ih),
   h(pls.h),
   a(pls.a)
{}

//------------------------------------------------------------------------------

Pulse& Pulse::operator=(const Pulse& pls)
{
   if (this==&pls) return *this;
   TObject::operator=(pls);
   bgn = pls.bgn;
   end = pls.end;
   ithr= pls.ithr;
   ih  = pls.ih;
   h   = pls.h;
   a = pls.a;
   return *this;
}
