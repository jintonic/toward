#include "Logger.h"
using namespace WAGE;

int Logger::gVerbosity = 0;

int Logger::GetVerbosity()
{
   if (verbosity<=0) return gVerbosity;
   else return verbosity;
}
