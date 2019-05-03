#include <cstdio>
using namespace std;

#include <UNIC/Units.h>
using namespace UNIC;

#include "Electrode.h"
using namespace TOWARD;

void Electrode::SetStatus(const char* status)
{
   if (status[0]=='d') st = kDead;
   else if (status[0]=='n') st = kNoisy;
   else if (status[0]=='o') st = kOk;
   else st = kUnknown;
}

//------------------------------------------------------------------------------

const char* Electrode::GetStatus() const
{
   if (st==kDead) return "dead";
   else if (st==kNoisy) return "noisy";
   else if (st==kOk) return "ok";
   else return "unknown";
}

//------------------------------------------------------------------------------

void Electrode::Dump() const
{
   if (id>=0) Info("Dump", "ch %2d: electrode %2d (voltage = %4.0f V, %s)", 
         ch, id, v/volt, GetStatus());
   else if (id==-1) Info("Dump", "ch %2d: empty", ch);
   else if (id==-10) Info("Dump", "ch %2d: coincidence", ch);
   else if (id==-20) Info("Dump", "ch %2d: clock", ch);
   else Info("Dump", "ch %2d: unknown", ch);
}

//------------------------------------------------------------------------------

