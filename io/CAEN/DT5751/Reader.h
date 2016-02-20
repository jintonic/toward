#ifndef READER_H
#define READER_H

#include <fstream>
#include <vector>

#include <WFs.h>
#include <Logger.h>

#include "cfg.h"

class Reader: public GEE::WFs, public GEE::Logger
{
   private:
      ifstream *fRaw;///< pointer to binary file
      TString fPath; ///< path to binary file
      TString fName; ///< name of binary file
      std::vector<size_t> fBegin; ///< beginning of each event
      std::vector<size_t> fSize; ///< size of each event

   public:
      Reader(int run, int sub=1, const char *dir=".");
      virtual ~Reader() { Close(); }

      const char* GetFile() { return fName.Data(); }

      void Close() { if (fRaw) if (fRaw->is_open()) fRaw->close(); }

      int GetEntries() { return fBegin.size(); }
      void GetEntry(int i);

      void LoadIndex();
      void Index();
      void DumpIndex();

      void ReadRunCfg(int i);
      void ReadEvent(int i);
      void ReadWF(unsigned short ch, uint16_t *data);
      /**
       * - calculate pedestal using the first @param nSamples
       * - overwrite integer pedestal given by hardware
       * - remove pedestal
       * - flip waveform if it is negative
       */
      void Calibrate(unsigned short ch, unsigned short nSamples);
};

#endif
