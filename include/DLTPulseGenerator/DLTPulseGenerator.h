/*******************************************************************************************
**
** Copyright (c) 2017 - 2022 Dr. Danny Petschke. All rights reserved.
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

#define COMPILE_AS_LIBRARY			 0

#ifndef DLTPULSEGENERATOR_H
#define DLTPULSEGENERATOR_H

#include <iostream>
#include <random>
#include <array>
#include <vector>
#include <cmath>
#include <memory>
#include <cstdint>

#define DLTPULSEGENERATOR_VERSION_RELEASE_DATE "28.08.2022"

#define DLTPULSEGENERATOR_MAJOR_VERSION 1
#define DLTPULSEGENERATOR_MINOR_VERSION 4

#if COMPILE_AS_LIBRARY == 1
#define DLTPULSEGENERATOR_EXPORT				   __declspec(dllexport) 
#define DLTPULSEGENERATOR_EXPORT_C		extern "C" __declspec(dllexport)
#else
#define DLTPULSEGENERATOR_EXPORT		/**/
#define DLTPULSEGENERATOR_EXPORT_C		/**/
#endif

#define IGNORE_LT_DISTRIBUTION		{false, DLifeTime::DLTDistributionFunction::Function::GAUSSIAN, 0.0f, 0.0f, 0, 0.0f}
#define IGNORE_DLTIRF				{false, DLifeTime::DLTDistributionFunction::Function::GAUSSIAN, 0.0f, 0.0f, 0.0f, 0.0f}

#define IGNORE_DIGITIZATION			{false, 14}
#define IGNORE_RND_NOISE_V			{false, 0.0}
#define IGNORE_BASELINE_JITTER_V	{false, 0.0, 0.0}
#define IGNORE_AXIS_NONLINEARITY_T  {false, 0.0, 0.0}

#define DLTPulseInfo_Demo			{5.0f, 0.165f, {true, 0.0, 5.0}, {true, 0.35}, {true, 0.015, 0.002}}

#define DLTIRF_PDS_DEMO				{{true, DLifeTime::DLTDistributionFunction::Function::GAUSSIAN, 1.0f, 0.084932901f, 0.0f, 0.0f}, IGNORE_DLTIRF, IGNORE_DLTIRF, IGNORE_DLTIRF, IGNORE_DLTIRF}
#define DLTIRF_MU_DEMO				{{true, DLifeTime::DLTDistributionFunction::Function::GAUSSIAN, 1.0f, 0.0025f, 0.0f, 0.0f}, IGNORE_DLTIRF, IGNORE_DLTIRF, IGNORE_DLTIRF, IGNORE_DLTIRF}

#define DLTSetup_DEMO				{ DLTIRF_PDS_DEMO, DLTIRF_PDS_DEMO, DLTIRF_MU_DEMO, 0.25f, 200.0f, 1024 }
#define DLTPulse_DEMO				{ DLTPulseInfo_Demo, DLTPulseInfo_Demo, {true, 14}, 500.0f, 65.0f, true }
#define DLTPHS_DEMO					{ true, 190.0f, 90.0f, 150.0f, 25.0f, 190.0f, 90.0f, 150.0f, 25.0f, std::vector<double>(), std::vector<double>(), 0., 0, std::vector<double>(), std::vector<double>(), 0., 0 }
#define DLTSimulationInput_DEMO		{ true, true, true, false, false, 0.160f, 0.420f, 3.2f, 0.0f, 0.0f, 0.25f, 0.25f, 0.5f, 0.0f, 0.0f, {true, DLifeTime::DLTDistributionFunction::Function::GAUSSIAN, 0.02f, 0.0f, 10000, 0.005f}, IGNORE_LT_DISTRIBUTION, IGNORE_LT_DISTRIBUTION, IGNORE_LT_DISTRIBUTION, IGNORE_LT_DISTRIBUTION, 0.25f, 0.05f, true }

#define DLTPulseGeneratorDEMO		DLTSimulationInput_DEMO, DLTPHS_DEMO, DLTSetup_DEMO, DLTPulse_DEMO

namespace DLifeTime {
struct DLTPULSEGENERATOR_EXPORT DLTDistributionFunction {
	enum Function : int {
		UNKNOWN			  = -1,
		GAUSSIAN		  = 0,
		LOG_NORMAL		  = 1,
		LORENTZIAN_CAUCHY = 2
	};
};

/** The digitization resolution Q (of an ADC) is in general defined as: Q = 2*amplitude/(2^digitizationDepth) 
**  The variable 'amplitude' is given in structure 'DLTPulse' and represents the overall readout range of the vertical axis (= voltage axis). 
**  Note: A factor 2 scales the readout range symmetrically around a zero baseline. 
**/

typedef struct {
	bool enabled;

	unsigned int digitizationDepth;  // [bit]
} DLTPULSEGENERATOR_EXPORT DLTPulseDigitizationInfo;

/* voltage axis random noise (Gaussian distributed) */

typedef struct {
	bool enabled;

	double rndNoise;  // stddev [mV]
} DLTPULSEGENERATOR_EXPORT DLTPulseRandomNoiseInfo;

/* voltage axis baseline-offset jitter (Gaussian distributed) */

typedef struct {
	bool enabled;

	double meanOfBaselineOffsetJitter;    //        [mV]
	double stddevOfBaselineOffsetJitter;  // stddev [mV]
} DLTPULSEGENERATOR_EXPORT DLTPulseBaselineOffsetJitterInfo;

/* time axis non-linearity (Gaussian distributed): aperture jitter (fixed and random) */

typedef struct {
	bool enabled;

	double fixedPatternApertureJitter;    // stddev [ns]
	double rndApertureJitter;			  // stddev [ns]
} DLTPULSEGENERATOR_EXPORT DLTPulseTimeAxisNonlinearityInfo;

/** The detector output pulses originating from photomultipliers or (avalanche) photodiodes
**  can mathematically be described by a log-normal distribution function:
**
**  U(t) = A*exp(-0.5*(ln(t/triseTime)/w)�)
**
**  A        - amplitude (or voltage range e.g. 0.5V)
**  w        - pulseWidth
**  riseTime - time needed to rise the pulse from 0 [mV] to the max. amplitude (A)
**/

typedef struct {
	double riseTime;	// [ns] 
	double pulseWidth;	// [ns]

	/* voltage domain */

	DLTPulseBaselineOffsetJitterInfo baselineOffsetJitterInfoV;
	DLTPulseRandomNoiseInfo		     randomNoiseInfoV;

	/* time domain */

	DLTPulseTimeAxisNonlinearityInfo timeAxisNonLinearityInfoT;

} DLTPULSEGENERATOR_EXPORT DLTPulseInfo;

typedef struct {
	DLTPulseInfo			 pulseA;
	DLTPulseInfo			 pulseB;

	DLTPulseDigitizationInfo digitizationInfo;
	
	double					 amplitude;	// [mV]
	double					 delay;		// [ns]

	bool					 isPositiveSignalPolarity; // (isPositiveSignalPolarity == true) ? A = |A| : A = -A
} DLTPULSEGENERATOR_EXPORT DLTPulse;


typedef struct {
	bool enabled;

	DLTDistributionFunction::Function functionType;
	double intensity;		// [ns]
	double uncertainty;		// [ns]
	double relativeShift;	// [ns]
	double param;			// [a.u.] -> reserved for future implementations.
} DLTPULSEGENERATOR_EXPORT DLTIRF;

/* The intrument response function (IRF) of PDS A (B) can be described by a linear combination of up to five functions. */

typedef struct {
	DLTIRF irf1PDS;
	DLTIRF irf2PDS;
	DLTIRF irf3PDS;
	DLTIRF irf4PDS;
	DLTIRF irf5PDS;
} DLTPULSEGENERATOR_EXPORT DLTIRF_PDS;

/* The intrument response function (IRF) of MU can be described by a linear combination of up to five functions. */

typedef struct {
	DLTIRF irf1MU;
	DLTIRF irf2MU;
	DLTIRF irf3MU;
	DLTIRF irf4MU;
	DLTIRF irf5MU;
} DLTPULSEGENERATOR_EXPORT DLTIRF_MU;

/** Simplified setup consisting of ...
**
** (PDS) Photo Detection System (e.g. photomultiplier tubes)
** (MU)  Measure Unit (e.g. DRS4 eval. board)			
**/

typedef struct {
	DLTIRF_PDS irfA;		// intrument response function (IRF) of Photo - Detection System (PDS) - Detector A
	DLTIRF_PDS irfB;		// intrument response function (IRF) of Photo - Detection System (PDS) - Detector B

	DLTIRF_MU irfMU;		// intrument response function of Measure - Unit (MU)
	
	double ATS;				// arrival-time spread (ATS) [ns]

	double sweep;			// digitized pulses (class DPulseF) are stored in the sweep [ns]
	int numberOfCells;		// one cell represents the class DPointF which is stored in the class DPulseF
} DLTPULSEGENERATOR_EXPORT DLTSetup;

/** Pulse height spectrum (PHS): distribution of detector output pulse amplitudes 
**	
**  The PHS can be generated by two variants managed by the parameter 'useGaussianModels' ...
**   
**  (1) 'useGaussianModels' = true: 
**  
**  The PHS will be approximated by a linear combination of two Gaussian distribution functions for the start and stop quanta, respectively.
**  The 'mean...' indicated parameter (i.e. mean of Gaussians)  should be in the region of ... 
**  
**  0 < mean < max(amplitude) for positive polarity and
**  min(amplitude) < mean < 0 for negative polarity.
**
**  (2) 'useGaussianModels' = false:
** 
**  The PHS will be generated based on real input data for the start and stop quanta, respectively.
**  The 'distribution...' indicated vectors contain the data of the respective probabilities in abritrary units supposed to be arranged equidistantly.
**  The resolution parameters 'resolutionMilliVoltPerStepA' and 'resolutionMilliVoltPerStepB' scale the probability corresponding amplitude axes.
**  The 'resolution...' must be positive values eventhough the polarity is set to negative values.
**  The 'gridNumber...' indicating parameters will up/downscale the given distribution to the desired resolution. 
** 
**  Example: vector consists of 1000 values which results in a maximum amplitude of 500 mV when setting the resolution to 0.5 mV. By setting the 'gridNumber' to 10000 the resulting resolution of the generated amplitudes 
**  will be 0.05 mV.
**/

typedef struct {
	bool useGaussianModels;

	// if 'useGaussianModels' == true:

    double meanOfStartA;   // [mV]
    double meanOfStopA;    // [mV]
    double stddevOfStartA; // [mV]
    double stddevOfStopA;  // [mV]

	double meanOfStartB;   // [mV]
	double meanOfStopB;    // [mV]
	double stddevOfStartB; // [mV]
	double stddevOfStopB;  // [mV]

	// if 'useGaussianModels' == false:
		
	std::vector<double> distributionStartA;
	std::vector<double> distributionStopA;
	double resolutionMilliVoltPerStepA;
	int gridNumberA;

	std::vector<double> distributionStartB;
	std::vector<double> distributionStopB;
	double resolutionMilliVoltPerStepB;
	int gridNumberB;
} DLTPULSEGENERATOR_EXPORT DLTPHS;

/** Each component to be simulated has to be explicitely enabled by setting the related variable 'ltX_activated' true. ('X' indicates the id of the component)
**  The sum of all enabled intensities must be equal to one (= 100%).
**  The lifetimes are generated to receive the start and stop pulses alternately from detector A and B
**  by setting the variable 'isStartStopAlternating' = true.
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

    double intensityOfPromptOccurrance;
    double intensityOfBackgroundOccurrance;

	bool isStartStopAlternating;
} DLTPULSEGENERATOR_EXPORT DLTSimulationInput;


class DLTPULSEGENERATOR_EXPORT DLTPointF {
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

class DLTPULSEGENERATOR_EXPORT DLTPulseF {
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

enum DLTPULSEGENERATOR_EXPORT DLTErrorType : DLTError {
	NONE_ERROR										= 0x00000000,

    NO_LIFETIMES_TO_SIMULATE						= 0x00000001,

	SWEEP_INVALID									= 0x00000002,
	NUMBER_OF_CELLS_INVALID							= 0x00000004,
	PDS_UNCERTAINTY_INVALID							= 0x00000008,
	MU_UNCERTAINTY_INVALID							= 0x00000010,
	PULSE_RISE_TIME_INVALID							= 0x00000020,
	PULSE_WIDTH_INVALID								= 0x00000040,
	DELAY_INVALID									= 0x00000080,
    DELAY_LARGER_THAN_SWEEP							= 0x00000100,
    INTENSITY_OF_LIFETIME_BELOW_ZERO				= 0x00000200,
    INTENSITY_OF_BKGRD_BELOW_ZERO					= 0x00000400,
    INTENSITY_OF_PROMT_BELOW_ZERO					= 0x00000800,
	INVALID_SUM_OF_WEIGTHS							= 0x00001000,
    AMPLITUDE_AND_PULSE_POLARITY_MISFIT				= 0x00002000,
    AMPLITUDE_AND_PHS_MISFIT						= 0x00004000,
	INVALID_LIFETIME_DISTRIBUTION_INPUT				= 0x00008000,
	INVALID_SUM_OF_PDS_IRF_INTENSITIES				= 0x00010000,
	INVALID_SUM_OF_MU_IRF_INTENSITIES				= 0x00020000,
	INVALID_VOLTAGE_BASELINE_JITTER					= 0x00040000,
	INVALID_VOLTAGE_RND_NOISE						= 0x00080000,
	INVALID_TIME_NONLINEARITY_FIXED_APERTURE_JITTER = 0x00100000,
	INVALID_TIME_NONLINEARITY_RND_APERTURE_JITTER	= 0x00200000,
	INVALID_DIGITIZATION_DEPTH						= 0x00400000,
	INVALID_PHS_GRID								= 0x00800000,
	INVALID_PHS_RESOLUTION							= 0x01000000
};

class DLTPULSEGENERATOR_EXPORT DLTCallback {
public:
   DLTCallback() {}
   virtual ~DLTCallback() {}

   virtual void onEvent(DLTError error) {}
};

class DLTPULSEGENERATOR_EXPORT DLTPulseGenerator {
    DLTSetup		   m_setupInfo;
    DLTPulse		   m_pulseInfo;
	DLTPHS			   m_phsDistribution;

    DLTSimulationInput m_simulationInput;

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
	void initPulseGenerator(DLTError *error, DLTCallback *callback = nullptr);
	void initLTGenerator(DLTError *error, DLTCallback *callback = nullptr);
	void initIRFGenerator(DLTError *error, DLTCallback *callback = nullptr);
	void initPHS(DLTError *error, DLTCallback *callback = nullptr);
    
	double estimateFWHM();
	double nextLifeTime(bool *bPromt, bool *bValid);
	double uncertaintyA();
	double uncertaintyB();
	void addUncertaintyMU(double *val);

private:
    int m_eventCounter;

    class DLTPulseGeneratorPrivate;
	class DLTDistributionManager;
	class DLTInterpolator;
    std::unique_ptr<DLTPulseGeneratorPrivate> m_privatePtr;
};

} //end namespace

/** The following class 'DLT_C_WRAPPER' is supposed to be used to access DLTPulseGenerator from Ansi C functions
**	in order to make the functionality of this library accessible to programming languages such as  
**  Matlab or Python.
**  
**	An example for Python 'pyDLTPulseGenerator' can be found in the related repository.
**/

class DLT_C_WRAPPER : public DLifeTime::DLTCallback {
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

DLTPULSEGENERATOR_EXPORT_C extern bool emitPulse(double trigger_in_mV_A, double trigger_in_mV_B); 

DLTPULSEGENERATOR_EXPORT_C extern double getTimeA(int index);
DLTPULSEGENERATOR_EXPORT_C extern double getTimeB(int index);
DLTPULSEGENERATOR_EXPORT_C extern double getVoltageA(int index);
DLTPULSEGENERATOR_EXPORT_C extern double getVoltageB(int index);

DLTPULSEGENERATOR_EXPORT_C extern int getNumberOfCells();

DLTPULSEGENERATOR_EXPORT_C extern DLifeTime::DLTError getLastError();

DLTPULSEGENERATOR_EXPORT_C extern void init();
DLTPULSEGENERATOR_EXPORT_C extern void update();

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

DLTPULSEGENERATOR_EXPORT_C extern void setRiseTimeA(double riseTime_in_nanoSeconds);
DLTPULSEGENERATOR_EXPORT_C extern void setPulseWidthA(double pulseWidth);

DLTPULSEGENERATOR_EXPORT_C extern void setPulseBaselineOffsetJitterA(bool enabled, double meanOfBaselineJitter_in_mV, double uncertaintyOfBaselineJitter_in_mV);
DLTPULSEGENERATOR_EXPORT_C extern void setPulseRandomNoiseA(bool enabled, double uncertainty_in_mV);
DLTPULSEGENERATOR_EXPORT_C extern void setPulseTimeAxisNonlinearityA(bool enabled, double uncertainty_fixedPatternApertureJitter_in_ns, double uncertainty_randomApertureJitter_in_ns);

DLTPULSEGENERATOR_EXPORT_C extern void setRiseTimeB(double riseTime_in_nanoSeconds);
DLTPULSEGENERATOR_EXPORT_C extern void setPulseWidthB(double pulseWidth);

DLTPULSEGENERATOR_EXPORT_C extern void setPulseBaselineOffsetJitterB(bool enabled, double meanOfBaselineJitter_in_mV, double uncertaintyOfBaselineJitter_in_mV);
DLTPULSEGENERATOR_EXPORT_C extern void setPulseRandomNoiseB(bool enabled, double uncertainty_in_mV);
DLTPULSEGENERATOR_EXPORT_C extern void setPulseTimeAxisNonlinearityB(bool enabled, double uncertainty_fixedPatternApertureJitter_in_ns, double uncertainty_randomApertureJitter_in_ns);

DLTPULSEGENERATOR_EXPORT_C extern void setDigitizationInfo(bool enabled, unsigned int digitizationDepth_in_bit);

DLTPULSEGENERATOR_EXPORT_C extern void setDelay(double delay_in_nanoSeconds);
DLTPULSEGENERATOR_EXPORT_C extern void setAmplitude(double amplitude_in_milliVolt);
DLTPULSEGENERATOR_EXPORT_C extern void setUsingPositiveSignalPolarity(bool isPositiveSignalPolarity);

DLTPULSEGENERATOR_EXPORT_C extern void setPHSFromGaussianModel(bool enabled);

DLTPULSEGENERATOR_EXPORT_C extern void setStartOfA(double meanOfStart_A_in_milliVolt, double sigmaOfStart_A_in_milliVolt);
DLTPULSEGENERATOR_EXPORT_C extern void setStartOfB(double meanOfStart_B_in_milliVolt, double sigmaOfStart_B_in_milliVolt);
DLTPULSEGENERATOR_EXPORT_C extern void setStopOfA(double meanOfStop_A_in_milliVolt, double sigmaOfStop_A_in_milliVolt);
DLTPULSEGENERATOR_EXPORT_C extern void setStopOfB(double meanOfStop_B_in_milliVolt, double sigmaOfStop_B_in_milliVolt);

DLTPULSEGENERATOR_EXPORT_C extern void setPHSDistributionOfA(double resolution, int gridNumber, double distributionStart[], int sizeOfStart, double distributionStop[], int sizeOfStop);
DLTPULSEGENERATOR_EXPORT_C extern void setPHSDistributionOfB(double resolution, int gridNumber, double distributionStart[], int sizeOfStart, double distributionStop[], int sizeOfStop);

DLTPULSEGENERATOR_EXPORT_C extern void setLifeTime_1(bool lt1_activated, double tau1_in_nanoSeconds, double intensity1, bool lt1_distributionActivated, DLifeTime::DLTDistributionFunction::Function functionType, double param1, int gridNumber, double gridIncrement);
DLTPULSEGENERATOR_EXPORT_C extern void setLifeTime_2(bool lt2_activated, double tau2_in_nanoSeconds, double intensity2, bool lt2_distributionActivated, DLifeTime::DLTDistributionFunction::Function functionType, double param1, int gridNumber, double gridIncrement);
DLTPULSEGENERATOR_EXPORT_C extern void setLifeTime_3(bool lt3_activated, double tau3_in_nanoSeconds, double intensity3, bool lt3_distributionActivated, DLifeTime::DLTDistributionFunction::Function functionType, double param1, int gridNumber, double gridIncrement);
DLTPULSEGENERATOR_EXPORT_C extern void setLifeTime_4(bool lt4_activated, double tau4_in_nanoSeconds, double intensity4, bool lt4_distributionActivated, DLifeTime::DLTDistributionFunction::Function functionType, double param1, int gridNumber, double gridIncrement);
DLTPULSEGENERATOR_EXPORT_C extern void setLifeTime_5(bool lt5_activated, double tau5_in_nanoSeconds, double intensity5, bool lt5_distributionActivated, DLifeTime::DLTDistributionFunction::Function functionType, double param1, int gridNumber, double gridIncrement);

DLTPULSEGENERATOR_EXPORT_C extern void setStartStopAlternating(bool alternating);

#endif // DLTPULSEGENERATOR_H
