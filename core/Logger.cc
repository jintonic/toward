#include "Logger.h"
using namespace GEE;

int Logger::gVerbosity = 0;

int Logger::GetVerbosity()
{
   if (verbosity<=0) return gVerbosity;
   else return verbosity;
}
