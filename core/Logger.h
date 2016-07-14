#ifndef LOGGER_H
#define LOGGER_H

namespace WAGE { class Logger; }

class WAGE::Logger
{
   public:
      static int gVerbosity; ///< global verbosity
      int verbosity; ///< local verbosity

      Logger(): verbosity(0) {};
      virtual ~Logger() {};

      int GetVerbosity();
};

#endif
