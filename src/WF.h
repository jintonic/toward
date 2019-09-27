#ifndef WF_H
#define WF_H

#include <vector>

#include "Electrode.h"
#include "Pulse.h"
#include <TGraph.h>

namespace TOWARD { class WF; }
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
class TOWARD::WF : public TObject
{
   public:
      enum {
         kCalibrated = BIT(14),
      };

      /**
       * Save
       */
      void Save();
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
       * Real number part of Fourier transform
       */
      std::vector<double> Rft; //Real part 
      /**
       * Imaginary part of Fourier transform
       */
      std::vector<double> Ift; // Imaginary part
      std::vector<double> s; ///< sample values
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
      double GetTrapozoidE(int L,int G,WF *out);
      void T2F();
      void F2T();
      void AddNoise(int s);
      void Draw(Option_t *chopt,int j);
      
      int GuessL();
      int GuessG();

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
