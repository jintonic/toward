#ifndef TOWARD_Digitizer
#define TOWARD_Digitizer
namespace TOWARD { class Digitizer; }
/**
 * DAQ related information.
 */
class TOWARD::Digitizer
{
   public:
      enum EnumChStatus { kUnknown=-100, kDead=-20, kNoisy=-10, kOk=0 };
      EnumChStatus st; ///< Digitizer channel status
      /**
       * Digitizer channel number.
       * = 100*100*(crate number) + 100*(module number) + channel number,
       * where crate, module and channel numbers all start from 0.
       */
      short ch;
      double dt; ///< = ch trg time - time when sensor is fired
      Digitizer(): st(kOk), ch(0), dt(0) {}; ///< Default constructor
};
#endif

