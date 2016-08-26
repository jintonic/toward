#ifndef WF_H
#define WF_H

#include <vector>

#include "Electrode.h"
#include "Pulse.h"

namespace WAGE { class WF; }
/**
 * Waveform information.
 *
 * It uses Electrode instead of inheritting it so that tree->Show(1) lists items like
 * wfs.ode.id instead of wfs.id.
 *
 * It contains a vector of Pulse instead of a TClonesArray for simplicity.
 * Nested TClonesArray cannot be split and loose all benifit of TClonesArray:
 * http://root.cern.ch/phpBB3/viewtopic.php?f=3&t=910
 */
class WAGE::WF : public TObject
{
   public:
      enum {
         kCalibrated = BIT(14),
      };

      /**
       * Sampling frequency
       */
      double freq; // sampling frequency
      /**
       * Pedestal
       */
      double ped; // pedestal
      /**
       * RMS of pedestal
       */
      double prms; // RMS of pedestal
      /**
       * 32 bits trigger count
       */
      unsigned int ctrg; // 32 bits trigger count

      /**
       * Waveform samples
       */
      std::vector<double> smpl; // waveform samples
      /**
       * Number of waveform samples
       */
      unsigned short ns; // number of samples
      /**
       * Array of pulses
       */
      std::vector<Pulse> pls; // array of pulses
      /**
       * Number of pulses
       */
      unsigned short np; // number of pulses

      /**
       * Information of related electrode
       */
      Electrode ode; // information of related electrode

      WF(): freq(0), ped(0), prms(0), ctrg(0), ns(0), np(0) {};
      virtual ~WF() {};

      bool IsSimilarTo(const WF& other) const;
      void MakeSimilarTo(const WF& other);
      std::vector<double> Filter(double L=-1,double G=-1);

      WF& operator+=(const WF& other); 
      WF& operator-=(const WF& other); 
      WF& operator*=(const WF& other); 
      WF& operator/=(const WF& other); 

      WF& operator+=(double value); 
      WF& operator-=(double value) { return (*this) += -value; }
      WF& operator*=(double value); 
      WF& operator/=(double value);

      void Reset();

      ClassDef(WF,1);
};

#endif
