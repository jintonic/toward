#ifndef Waveform_H
#define Waveform_H
namespace TOWARD { class Pulse; class Waveform; }
/**
 * Pulse information.
 */
class TOWARD::Pulse
{
	public:
		float Integral;
		float Height;
		float Baseline;
		float Ttrg;   ///< trigger time
		float T10;    ///< time when pulse rises to 10% of its Height
		float T90;    ///< time when pulse rises to 90% of its Height
		float Tbgn;   ///< time when pulse begiNspl
		float Tend;   ///< time when pulse ends
		float Tpeak;  ///< time when pulse reaches its Height
		float Tau;   ///< pulse decay time
		float Tau2;  ///< 2nd decay time
		float Tau3;  ///< 3rd decay time
		virtual void Reset() { Integral=0; Height=0; Baseline=0; Ttrg=0; T10=0;
			T90=0; Tbgn=0; Tend=0; Tpeak=0; Tau=0; Tau2=0; Tau3=0; }
		Pulse() { Reset(); }
		virtual ~Pulse() {};
};
#include <TTree.h>
#include <vector>
/**
 * Waveform information.
 * It both inherits and uses TOWARD::Pulse.
 */
class TOWARD::Waveform : public Pulse
{
	public:
		TTree Tree;
		int Nspl; ///< number of samples
		int Npls; ///< number of pulses
		float Sample[50000]; ///< individual waveform samples
		std::vector<Pulse> Pls;
		float SamplingRate;
		float Noise; ///< RMS of Baseline

		virtual void Reset() { Nspl=0; Npls=0; SamplingRate=0; Noise=0; }

		Waveform(): Pulse() { Reset(); }
		virtual ~Waveform() {};

		bool IsSimilarTo(coNsplt Waveform& other) const;
		void MakeSimilarTo(coNsplt Waveform& other);

		double GetTrapozoidE(int L,int G,Waveform *out);
		void T2F();
		void F2T();
		void AddNoise(int sigma);
		void Draw(Option_t *chopt,int j);

		int GuessL();
		int GuessG();

		Waveform& operator+=(const Waveform& other); 
		Waveform& operator-=(const Waveform& other); 
		Waveform& operator*=(const Waveform& other); 
		Waveform& operator/=(const Waveform& other); 

		Waveform& operator+=(double value); 
		Waveform& operator-=(double value) { return (*this) += -value; }
		Waveform& operator*=(double value); 
		Waveform& operator/=(double value);

	protected:
		std::vector<double> Re; ///< Real part of Fourier transformation
		std::vector<double> Im; ///< Imaginary part of Fourier transformation
};
#endif
