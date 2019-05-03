#ifndef ELECTRODE_H
#define ELECTRODE_H

#include <TObject.h>

namespace TOWARD { class Electrode; }
/**
 * Electrode information
 * It inherits Info(), Warning(), etc. functions from TObject to dump
 * information in terminal with <TOWARD::Electrode::Function> prefixed.
 */
class TOWARD::Electrode: public TObject
{
   public:
      enum Status {
         kUnknown=-100,
         kDead=-20,
         kNoisy=-10,
         kOk=0,
      };
      /**
       * Electrode status
       */
      Status st; // Electrode status
      /**
       * Electrode id
       * loaded from $TOWARD/cfg/?/mapping.txt in WFs::Initialize()
       * - id >= 0: normal electrode
       * - id == -1: empty channel
       * - id == -10: coincidence
       * - id == -20: clock
       * In case of a normal electrode, id = detector*100 + electrode
       * electrode = 0 for core contact
       */
      short id; // electrode id
      /**
       * Electronics channel number
       * It equals to crate*Nmo*Nch + module*Nch + channel,
       * where crate, module and channel all start from 0.
       */
      short ch; // electronics channel number
      /**
       * Time offset
       */
      double dt; // time offset
      /**
       * Voltage applied
       */
      double v; // voltage applied
      /**
       * Time
       */
      double t; // time
      /**
       * Energy
       */
      double e; // energy

      Electrode(): 
         TObject(), st(kUnknown), id(-1), ch(-1), dt(0), v(0), t(0), e(0) {};
      virtual ~Electrode() {};

      void SetStatus(const char *status);
      const char* GetStatus() const;
      void Dump() const;

      ClassDef(Electrode,1);
};

#endif
