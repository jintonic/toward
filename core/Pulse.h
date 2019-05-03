#ifndef PULSE_H
#define PULSE_H

#include <TObject.h>

namespace TOWARD { class Pulse; }

class TOWARD::Pulse : public TObject
{
   public:
      enum {
         kSaturated = BIT(14),
      };

      /**
       * Begin position in the waveform
       */
      int bgn; // begin position in the waveform
      /**
       * End position in the waveform
       */
      int end; // end position in the waveform
      /**
       * Position of threshold crossing point
       */
      int ithr; // position of threshold crossing point
      /**
       * Position of max height
       */
      int ih; // position of max height
      /**
       * Pulse height
       */
      double h; // pulse height
      /**
       * Area covered by pulse
       */
      double a; // area covered by pulse

      Pulse() : TObject(), bgn(0), end(0), ithr(0), ih(0), h(0), a(0) {};
      Pulse(const Pulse& pls);
      Pulse& operator=(const Pulse& pls);
      virtual ~Pulse() {};

      ClassDef(Pulse,1);
};

#endif
