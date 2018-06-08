/*******************************************************************************************
**
** Copyright (c) 2017, 2018 Danny Petschke. All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without modification, 
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, 
**	  this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, 
**    this list of conditions and the following disclaimer in the documentation 
**    and/or other materials provided with the distribution.
**
** 3. Neither the name of the copyright holder "Danny Petschke" nor the names of its  
**    contributors may be used to endorse or promote products derived from this software  
**    without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS 
** OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
** COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
** TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
** EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** Contact: danny.petschke@uni-wuerzburg.de
**
***********************************************************************************************/

/* For the compilation as static or linked library set COMPILE_AS_LIBRARY = 1 */
#define COMPILE_AS_LIBRARY					   1

#ifndef DLTPULSEGENERATOR_H
#define DLTPULSEGENERATOR_H

#include <iostream>
#include <random>
#include <array>
#include <vector>
#include <cmath>
#include <memory>
#include <cstdint>

#define DLTPULSEGENERATOR_VERSION_RELEASE_DATE "14.05.2018"

#define DLTPULSEGENERATOR_MAJOR_VERSION 1
#define DLTPULSEGENERATOR_MINOR_VERSION 2

#if COMPILE_AS_LIBRARY == 1
#define DLTPULSEGENERATOR_EXPORT				   __declspec(dllexport) 
#define DLTPULSEGENERATOR_EXPORT_C		extern "C" __declspec(dllexport)
#else
#define DLTPULSEGENERATOR_EXPORT		/**/
#define DLTPULSEGENERATOR_EXPORT_C		/**/
#endif

/* structures for demonstration purposes  */

#define IGNORE_LT_DISTRIBUTION	{false, DLifeTime::DLTDistributionFunction::Function::GAUSSIAN, 0.0f, 0.0f, 0, 0.0f}
#define IGNORE_DLTIRF			{false, DLifeTime::DLTDistributionFunction::Function::GAUSSIAN, 0.0f, 0.0f, 0.0f, 0.0f}

#define DLTIRF_PDS_DEMO			{{true, DLifeTime::DLTDistributionFunction::Function::GAUSSIAN, 1.0f, 0.084932901f, 0.0f, 0.0f}, IGNORE_DLTIRF, IGNORE_DLTIRF, IGNORE_DLTIRF, IGNORE_DLTIRF}
#define DLTIRF_MU_DEMO			{{true, DLifeTime::DLTDistributionFunction::Function::GAUSSIAN, 1.0f, 0.0025f, 0.0f, 0.0f}, IGNORE_DLTIRF, IGNORE_DLTIRF, IGNORE_DLTIRF, IGNORE_DLTIRF}

#define DLTSetup_DEMO			{ DLTIRF_PDS_DEMO, DLTIRF_PDS_DEMO, DLTIRF_MU_DEMO, 0.25f, 200.0f, 1024 }
#define DLTPulse_DEMO			{ 5.0f, 0.165f, 500.0f, 65.0f, true }
#define DLTPHS_DEMO				{ 190.0f, 90.0f, 150.0f, 25.0f, 190.0f, 90.0f, 150.0f, 25.0f }
#define DLTSimulationInput_DEMO { true, true, true, false, false, 0.160f, 0.420f, 3.2f, 0.0f, 0.0f, 0.25f, 0.25f, 0.5f, 0.0f, 0.0f, {true, DLifeTime::DLTDistributionFunction::Function::GAUSSIAN, 0.02f, 0.0f, 10000, 0.005f}, IGNORE_LT_DISTRIBUTION, IGNORE_LT_DISTRIBUTION, IGNORE_LT_DISTRIBUTION, IGNORE_LT_DISTRIBUTION, 0.25f, 0.05f, true }

#define DLTPulseGeneratorDEMO	DLTSimulationInput_DEMO, DLTPHS_DEMO, DLTSetup_DEMO, DLTPulse_DEMO

namespace DLifeTime {
struct DLTPULSEGENERATOR_EXPORT DLTDistributionFunction {
	enum Function : int {
		UNKNOWN			  = -1,
		GAUSSIAN		  = 0,
		LOG_NORMAL		  = 1,
		LORENTZIAN_CAUCHY = 2
	};
};

/** A non-TTL pulse originating from a photomultiplier or photodiode can mathematically be described with the log-normal distribution function:
**
**  U(t) = A*exp(-0.5*(ln(t/triseTime)/w)²) 
**
**  A - amplitude
**  w - pulseWidth
**  riseTime - triseTime - time which is needed from 0 -> max(A)
**/
typedef struct {
	double riseTime;	// [ns] 
	double pulseWidth;	// [ns]
	double amplitude;	// [mV]

	double delay;		// [ns]

	bool isPositiveSignalPolarity; // (isPositiveSignalPolarity == true) ? A = |A| : A = -A
} DLTPULSEGENERATOR_EXPORT DLTPulse;


typedef struct {
	bool enabled;
	DLTDistributionFunction::Function functionType;
	double intensity;		// [ns]
	double uncertainty;		// [ns]
	double relativeShift;	// [ns]
	double param;			// [a.u.] -> reserved for future implementations.
} DLTPULSEGENERATOR_EXPORT DLTIRF;

/* The Intrument response function (IRF) of PDS A (B) can be described by a linear combination of up to five functions. */
typedef struct {
	DLTIRF irf1PDS;
	DLTIRF irf2PDS;
	DLTIRF irf3PDS;
	DLTIRF irf4PDS;
	DLTIRF irf5PDS;
} DLTPULSEGENERATOR_EXPORT DLTIRF_PDS;

/* The Intrument response function (IRF) of MU can be described by a linear combination of up to five functions. */
typedef struct {
	DLTIRF irf1MU;
	DLTIRF irf2MU;
	DLTIRF irf3MU;
	DLTIRF irf4MU;
	DLTIRF irf5MU;
} DLTPULSEGENERATOR_EXPORT DLTIRF_MU;

/** Simplified setup consisting of:
**
** - Photo Detection System (PDS)
** - Measure Unit			(MU)
**/
typedef struct {
	DLTIRF_PDS irfA;		// Intrument response function (IRF) of Photo - Detection System (PDS) - Detector A
	DLTIRF_PDS irfB;		// Intrument response function (IRF) of Photo - Detection System (PDS) - Detector B

	DLTIRF_MU irfMU;		// Intrument response function of Measure - Unit (MU)
	
	double ATS;				// Arrival-Time Spread (ATS) [ns]

	double sweep;			// digitized pulses (class DPulseF) are stored in the sweep [ns]
	int numberOfCells;		// one cell represents the class DPointF which is stored in the class DPulseF
} DLTPULSEGENERATOR_EXPORT DLTSetup;

/** Pulse-Height Spectrum (PHS): distribution of the pulse-amplitude 
**	
**  Mostly relevant for energy resolved measurements, such as Positron-Annihilation-Lifetime Spectroscopy (PALS).
**  The PHS is represented by a linear combination of two Gaussian distribution functions for start and stop, respectively.
**  In case of Photodiodes, the values named as stddev can be set to zero.
**  The mean should be in the range: 
**  
**  0 < mean < max(amplitude) for positive polarity and
**  min(amplitude) < mean < 0 for negative polarity.
**/
typedef struct {
    double meanOfStartA;   // [mV]
    double meanOfStopA;    // [mV]
    double stddevOfStartA; // [mV]
    double stddevOfStopA;  // [mV]

	double meanOfStartB;   // [mV]
	double meanOfStopB;    // [mV]
	double stddevOfStartB; // [mV]
	double stddevOfStopB;  // [mV]
} DLTPULSEGENERATOR_EXPORT DLTPHS;

/** Each component to be simulated must be enabled by setting the related variable ltX_activated true. 
**  The sum of all enabled intensities must be equal one.
**  The lifetimes are generated to receive the start and stop pulses alternately from detector A and B by setting:
**  isStartStopAlternating = true.
**/
typedef struct {
	bool enabled;
	DLTDistributionFunction::Function functionType;
	double param1;			// [ns]
	double param2;			// [a.u.] -> reserved for future implementations.
	int gridNumber;			// [#]
	double gridIncrement;	// [ns]
} DLTPULSEGENERATOR_EXPORT DLTDistributionInfo;

typedef struct {
    bool lt1_activated;
    bool lt2_activated;
    bool lt3_activated;
    bool lt4_activated;
    bool lt5_activated;

    double tau1; // [ns]
    double tau2; // [ns]
    double tau3; // [ns]
    double tau4; // [ns]
    double tau5; // [ns]

    double intensity1;
    double intensity2;
    double intensity3;
    double intensity4;
    double intensity5;

	DLTDistributionInfo tau1Distribution;
	DLTDistributionInfo tau2Distribution;
	DLTDistributionInfo tau3Distribution;
	DLTDistributionInfo tau4Distribution;
	DLTDistributionInfo tau5Distribution;

    double intensityOfPromtOccurrance;
    double intensityOfBackgroundOccurrance;

	bool isStartStopAlternating;
} DLTPULSEGENERATOR_EXPORT DLTSimulationInput;


class DLTPULSEGENERATOR_EXPORT DLTPointF
{
    double m_x;
    double m_y;

public:
    DLTPointF();
    virtual ~DLTPointF();

    void setX(double x);
    void setY(double y);

    void setPoint(double x, double y);
    void setPoint(const DLTPointF& point);

    double x() const;
    double y() const;
};

class DLTPULSEGENERATOR_EXPORT DLTPulseF
{
	std::vector<DLTPointF> *m_vector;

	double *m_time, *m_voltage;
	int m_counter;

public:
	DLTPulseF(double *time = nullptr, double *voltage = nullptr);
	virtual ~DLTPulseF();

    void append(const DLTPointF& dataPoint);
	void clear();

	int size() const;

	DLTPointF at(int index) const;
};

typedef DLTPULSEGENERATOR_EXPORT int32_t DLTError;

enum DLTPULSEGENERATOR_EXPORT DLTErrorType : DLTError
{
	NONE_ERROR							= 0x00000000,

    NO_LIFETIMES_TO_SIMULATE			= 0x00000001,

	SWEEP_INVALID						= 0x00000002,
	NUMBER_OF_CELLS_INVALID				= 0x00000004,
	PDS_UNCERTAINTY_INVALID				= 0x00000008,
	MU_UNCERTAINTY_INVALID				= 0x00000010,
	PULSE_RISE_TIME_INVALID				= 0x00000020,
	PULSE_WIDTH_INVALID					= 0x00000040,
	DELAY_INVALID						= 0x00000080,
    DELAY_LARGER_THAN_SWEEP				= 0x00000100,
    INTENSITY_OF_LIFETIME_BELOW_ZERO	= 0x00000200,
    INTENSITY_OF_BKGRD_BELOW_ZERO		= 0x00000400,
    INTENSITY_OF_PROMT_BELOW_ZERO		= 0x00000800,
	INVALID_SUM_OF_WEIGTHS				= 0x00001000,
    AMPLITUDE_AND_PULSE_POLARITY_MISFIT = 0x00002000,
    AMPLITUDE_AND_PHS_MISFIT			= 0x00004000,
	INVALID_LIFETIME_DISTRIBUTION_INPUT = 0x00008000,
	INVALID_SUM_OF_PDS_IRF_INTENSITIES	= 0x00010000,
	INVALID_SUM_OF_MU_IRF_INTENSITIES	= 0x00020000,
};

class DLTPULSEGENERATOR_EXPORT DLTCallback
{
public:
   DLTCallback() {}
   virtual ~DLTCallback() {}

   virtual void onEvent(DLTError error) {}
};

class DLTPULSEGENERATOR_EXPORT DLTPulseGenerator
{
    DLTSetup		   m_setupInfo;
    DLTPulse		   m_pulseInfo;
	DLTPHS			   m_phsDistribution;

    DLTSimulationInput m_simulationInput;

	double			   m_sampleScaleFactor;

public:
	DLTPulseGenerator(const DLTSimulationInput& simulationInput, 
					  const DLTPHS& phsDistribution, 
					  const DLTSetup& setupInfo, 
					  const DLTPulse& pulseInfo, 
					  DLTCallback *callback = nullptr);

    virtual ~DLTPulseGenerator();

    bool emitPulses(DLTPulseF *pulseA, 
					DLTPulseF *pulseB, 
					double triggerLevelA_in_milliVolt, 
					double triggerLevelB_in_milliVolt);

private:
	void initLTGenerator(DLTError *error, DLTCallback *callback = nullptr);
	void initIRFGenerator(DLTError *error, DLTCallback *callback = nullptr);
    
	double estimateFWHM();
	double nextLifeTime(bool *bPromt, bool *bValid);
	double uncertaintyA();
	double uncertaintyB();
	void addUncertaintyMU(double *val);

private:
    int m_eventCounter;

    class DLTPulseGeneratorPrivate;
	class DLTDistributionManager;
    std::unique_ptr<DLTPulseGeneratorPrivate> m_privatePtr;
};

} //end namespace


/** The class DLT_C_WRAPPER is used as a singleton pattern class to access from Ansi C functions:
**	This provides the access from other programming languages such as matlab (mex-compiler) or python (ctypes-library).
**  
**	An example how to use class DLTPulseGenerator in combination with class DLT_C_WRAPPER is given in python:
**	
**  - pyDLTPulseGenerator -
**/

class DLT_C_WRAPPER : public DLifeTime::DLTCallback
{
	DLifeTime::DLTPulseGenerator *m_accessPtr;
	
public:
	DLT_C_WRAPPER();
	virtual ~DLT_C_WRAPPER();

	static DLT_C_WRAPPER *sharedInstance();

	virtual void onEvent(DLifeTime::DLTError error) override;

	void reinit();
	bool emitPulse(double trigger_in_mV_A, double trigger_in_mV_B);

public:
	DLifeTime::DLTPHS			  m_phsDistribution;
	DLifeTime::DLTSetup			  m_setupInfo;
	DLifeTime::DLTPulse			  m_pulseInfo;
	DLifeTime::DLTSimulationInput m_simulationInput;

	DLifeTime::DLTPulseF	     *m_pulseA, *m_pulseB;

	DLifeTime::DLTError			  m_lastError;
};

DLTPULSEGENERATOR_EXPORT_C extern bool emitPulse(double trigger_in_mV_A, double trigger_in_mV_B); //call this to generate a new pulse!

// to get the values of the pulses call 'emitPulse' before!
DLTPULSEGENERATOR_EXPORT_C extern double getTimeA(int index);
DLTPULSEGENERATOR_EXPORT_C extern double getTimeB(int index);
DLTPULSEGENERATOR_EXPORT_C extern double getVoltageA(int index);
DLTPULSEGENERATOR_EXPORT_C extern double getVoltageB(int index);

DLTPULSEGENERATOR_EXPORT_C extern int getNumberOfCells();

DLTPULSEGENERATOR_EXPORT_C extern DLifeTime::DLTError getLastError();

DLTPULSEGENERATOR_EXPORT_C extern void init();
DLTPULSEGENERATOR_EXPORT_C extern void update();

//equivalent to DLTSetup:
DLTPULSEGENERATOR_EXPORT_C extern void manipulate(DLifeTime::DLTIRF *irf, bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);

DLTPULSEGENERATOR_EXPORT_C extern void setIRF_PDS_A_1(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);
DLTPULSEGENERATOR_EXPORT_C extern void setIRF_PDS_A_2(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);
DLTPULSEGENERATOR_EXPORT_C extern void setIRF_PDS_A_3(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);
DLTPULSEGENERATOR_EXPORT_C extern void setIRF_PDS_A_4(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);
DLTPULSEGENERATOR_EXPORT_C extern void setIRF_PDS_A_5(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);

DLTPULSEGENERATOR_EXPORT_C extern void setIRF_PDS_B_1(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);
DLTPULSEGENERATOR_EXPORT_C extern void setIRF_PDS_B_2(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);
DLTPULSEGENERATOR_EXPORT_C extern void setIRF_PDS_B_3(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);
DLTPULSEGENERATOR_EXPORT_C extern void setIRF_PDS_B_4(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);
DLTPULSEGENERATOR_EXPORT_C extern void setIRF_PDS_B_5(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);

DLTPULSEGENERATOR_EXPORT_C extern void setIRF_MU_1(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);
DLTPULSEGENERATOR_EXPORT_C extern void setIRF_MU_2(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);
DLTPULSEGENERATOR_EXPORT_C extern void setIRF_MU_3(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);
DLTPULSEGENERATOR_EXPORT_C extern void setIRF_MU_4(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);
DLTPULSEGENERATOR_EXPORT_C extern void setIRF_MU_5(bool enabled, DLifeTime::DLTDistributionFunction::Function functionType, double intensity, double uncertainty, double relativeShift);

DLTPULSEGENERATOR_EXPORT_C extern void setATS(double ATS_in_nanoSeconds);
DLTPULSEGENERATOR_EXPORT_C extern void setSweep(double sweep_in_nanoSeconds);
DLTPULSEGENERATOR_EXPORT_C extern void setNumberOfCells(int numberOfCells);

//equivalent to DLTPulse:
DLTPULSEGENERATOR_EXPORT_C extern void setRiseTime(double riseTime_in_nanoSeconds);
DLTPULSEGENERATOR_EXPORT_C extern void setPulseWidth(double pulseWidth);
DLTPULSEGENERATOR_EXPORT_C extern void setDelay(double delay_in_nanoSeconds);
DLTPULSEGENERATOR_EXPORT_C extern void setAmplitude(double amplitude_in_milliVolt);
DLTPULSEGENERATOR_EXPORT_C extern void setUsingPositiveSignalPolarity(bool isPositiveSignalPolarity);

//equivalent to DLTPHSDistribution:
DLTPULSEGENERATOR_EXPORT_C extern void setStartOfA(double meanOfStart_A_in_milliVolt, double sigmaOfStart_A_in_milliVolt);
DLTPULSEGENERATOR_EXPORT_C extern void setStartOfB(double meanOfStart_B_in_milliVolt, double sigmaOfStart_B_in_milliVolt);
DLTPULSEGENERATOR_EXPORT_C extern void setStopOfA(double meanOfStop_A_in_milliVolt, double sigmaOfStop_A_in_milliVolt);
DLTPULSEGENERATOR_EXPORT_C extern void setStopOfB(double meanOfStop_B_in_milliVolt, double sigmaOfStop_B_in_milliVolt);

//equivalent to DLTSimulationInput:
DLTPULSEGENERATOR_EXPORT_C extern void setLifeTime_1(bool lt1_activated, double tau1_in_nanoSeconds, double intensity1, bool lt1_distributionActivated, DLifeTime::DLTDistributionFunction::Function functionType, double param1, int gridNumber, double gridIncrement);
DLTPULSEGENERATOR_EXPORT_C extern void setLifeTime_2(bool lt2_activated, double tau2_in_nanoSeconds, double intensity2, bool lt2_distributionActivated, DLifeTime::DLTDistributionFunction::Function functionType, double param1, int gridNumber, double gridIncrement);
DLTPULSEGENERATOR_EXPORT_C extern void setLifeTime_3(bool lt3_activated, double tau3_in_nanoSeconds, double intensity3, bool lt3_distributionActivated, DLifeTime::DLTDistributionFunction::Function functionType, double param1, int gridNumber, double gridIncrement);
DLTPULSEGENERATOR_EXPORT_C extern void setLifeTime_4(bool lt4_activated, double tau4_in_nanoSeconds, double intensity4, bool lt4_distributionActivated, DLifeTime::DLTDistributionFunction::Function functionType, double param1, int gridNumber, double gridIncrement);
DLTPULSEGENERATOR_EXPORT_C extern void setLifeTime_5(bool lt5_activated, double tau5_in_nanoSeconds, double intensity5, bool lt5_distributionActivated, DLifeTime::DLTDistributionFunction::Function functionType, double param1, int gridNumber, double gridIncrement);

DLTPULSEGENERATOR_EXPORT_C extern void setStartStopAlternating(bool alternating);

#endif // DLTPULSEGENERATOR_H
