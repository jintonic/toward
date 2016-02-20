#ifndef LOGGER_H
#define LOGGER_H

namespace GEE { class Logger; }

class GEE::Logger
{
   public:
      static int gVerbosity; ///< global verbosity
      int verbosity; ///< local verbosity

      Logger(): verbosity(0) {};
      virtual ~Logger() {};

      int GetVerbosity();
};

#endif
