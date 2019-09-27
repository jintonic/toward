#ifndef WFS_H
#define WFS_H

#include <TClonesArray.h>

namespace TOWARD {
   class WF;
   class WFs;
}
/**
 * Information of waveform array
 * It contains some global information common for all channels.
 *
 * It uses TClonesArray instead of inheriting from it because TClonesArray is
 * created merely as a container.
 *
 * It loads electrode database to initialize the TClonesArray of empty
 * waveforms with electrode information. Loading database is done here instead
 * of in the TOWARD::Electrode class to avoid passing run, file name and location
 * to TOWARD::Electrode.
 */
class TOWARD::WFs : public TObject
{
   private:
      WF* Map(int ch); ///< map ch to WF::electrode
      void LoadTimeOffset(WF* wf); ///< load dt offset to WF::electrode
      void LoadVoltage(WF* wf); ///< load voltage to a WF::electrode
      void LoadStatus(WF* wf); ///< load status to a WF::electrode

   protected:
      /**
       * Path to electrode database
       */
      TString fDB; //! /path/to/electrode/

   public:
      /**
       * Run number
       */
      int run; // run number
      /**
       * Sub run number
       */
      short sub; // sub run number
      /**
       * Event id
       */
      int evt; // event id
      /**
       * Trigger count
       */
      unsigned int cnt; // trigger count
      /**
       * Seconds to Unix epoch
       * Valid till year 2038
       */
      unsigned int sec; // Linux time of event in second
      /**
       * Total number of available channels
       * Filled after loading electrode database
       */
      unsigned short nch; // number of available channels
      /**
       * Maximal number of samples of a waveform
       * Filled after loading run header
       */
      unsigned short nmax; // max number of samples of a wf
      /**
       * Number of samples after non-0-suppressed window
       * Filled after loading run header
       */
      unsigned short nfw; // # of samples after non-0-suppressed window
      /**
       * Number of samples before non-0-suppressed window
       * Filled after loading run header
       */
      unsigned short nbw; // # of samples before non-0-suppressed window
      /**
       * Threshold for software 0-suppression
       * unit: ADC count
       */
      unsigned short thr; // threshold for software 0-suppression
      /**
       * Waveform array
       */
      TClonesArray wf; // waveform array

   public:
      WFs(int run=999999) : TObject(), run(run), sub(-1), evt(-1), cnt(0),
      sec(0), nch(8), nmax(0), nfw(0), nbw(0), thr(0) {};
      virtual ~WFs() {};

      void Initialize(const char *db="/path/to/electrode");

      WF* At(unsigned short i) const;
      WF* operator[](unsigned short i) const { return At(i); }

      ClassDef(WFs,1);
};

#endif
