/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2018 Danny Petschke
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see http://www.gnu.org/licenses/.
**
*****************************************************************************
**
**  @author: Danny Petschke
**  @contact: danny.petschke@uni-wuerzburg.de
**
*****************************************************************************/

#include "drs4pulsegenerator.h"

using namespace DLifeTime;

DRS4PulseGenerator *__sharedInstancePulseGenerator = nullptr;

DRS4PulseGenerator *DRS4PulseGenerator::sharedInstance()
{
    if ( !__sharedInstancePulseGenerator )
        __sharedInstancePulseGenerator = new DRS4PulseGenerator();

    return __sharedInstancePulseGenerator;
}

DPulseSimulationType DRS4PulseGenerator::type() const
{
    QMutexLocker locker(&m_mutex);

    return m_type;
}

QString DRS4PulseGenerator::getErrorString() const
{
    QMutexLocker locker(&m_mutex);

    QString errorStr = "<lu><b>";

    if ((m_error == DLTErrorType::NONE_ERROR))
        errorStr += "<li><font color=\"Green\">no error(s)</font></li>";
    else {
        if ((m_error & DLTErrorType::NO_LIFETIMES_TO_SIMULATE))
            errorStr += "<li><font color=\"Red\">no lifetimes to simulate</font></li>";
        if ((m_error & DLTErrorType::SWEEP_INVALID))
            errorStr += "<li><font color=\"Red\">invalid sweep</font></li>";
        if ((m_error & DLTErrorType::PDS_UNCERTAINTY_INVALID))
            errorStr += "<li><font color=\"Red\">invalid PDS uncertainty</font></li>";
        if ((m_error & DLTErrorType::MU_UNCERTAINTY_INVALID))
            errorStr += "<li><font color=\"Red\">invalid MU uncertainty</font></li>";
        if ((m_error & DLTErrorType::PULSE_RISE_TIME_INVALID))
            errorStr += "<li><font color=\"Red\">invalid pulse rise time</font></li>";
        if ((m_error & DLTErrorType::PULSE_WIDTH_INVALID))
            errorStr += "<li><font color=\"Red\">invalid pulse width</font></li>";
        if ((m_error & DLTErrorType::DELAY_INVALID))
            errorStr += "<li><font color=\"Red\">invalid delay</font></li>";
        if ((m_error & DLTErrorType::DELAY_LARGER_THAN_SWEEP))
            errorStr += "<li><font color=\"Red\">delay larger than sweep</font></li>";
        if ((m_error & DLTErrorType::INTENSITY_OF_LIFETIME_BELOW_ZERO))
            errorStr += "<li><font color=\"Red\">negative intensity of one (or all) lifetime component(s)</font></li>";
        if ((m_error & DLTErrorType::INTENSITY_OF_BKGRD_BELOW_ZERO))
            errorStr += "<li><font color=\"Red\">weight of background occurrences below zero</font></li>";
        if ((m_error & DLTErrorType::INTENSITY_OF_PROMT_BELOW_ZERO))
            errorStr += "<li><font color=\"Red\">weight of prompt events below zero</font></li>";
        if ((m_error & DLTErrorType::INVALID_SUM_OF_WEIGTHS))
            errorStr += "<li><font color=\"Red\">no lifetimes to simulate</font></li>";
        if ((m_error & DLTErrorType::AMPLITUDE_AND_PULSE_POLARITY_MISFIT))
            errorStr += "<li><font color=\"Red\">amplitude does not fit the polarity</font></li>";
        if ((m_error & DLTErrorType::AMPLITUDE_AND_PHS_MISFIT))
            errorStr += "<li><font color=\"Red\">misfit of amplitude and PHS</font></li>";
        if ((m_error & DLTErrorType::INVALID_LIFETIME_DISTRIBUTION_INPUT))
            errorStr += "<li><font color=\"Red\">invalid lifetime distribution input</font></li>";
    }

    errorStr += "</b></lu>";

    return errorStr;
}

void DRS4PulseGenerator::update()
{
    QMutexLocker locker(&m_mutex);

    //PHS:
    m_phsDistribution.meanOfStartA = DRS4SimulationSettingsManager::sharedInstance()->meanPHS1274keV_A();
    m_phsDistribution.meanOfStartB = DRS4SimulationSettingsManager::sharedInstance()->meanPHS1274keV_B();

    m_phsDistribution.meanOfStopA = DRS4SimulationSettingsManager::sharedInstance()->meanPHS511keV_A();
    m_phsDistribution.meanOfStopB = DRS4SimulationSettingsManager::sharedInstance()->meanPHS511keV_B();

    m_phsDistribution.stddevOfStartA = DRS4SimulationSettingsManager::sharedInstance()->sigmaPHS1274keV_A();
    m_phsDistribution.stddevOfStartB = DRS4SimulationSettingsManager::sharedInstance()->sigmaPHS1274keV_B();

    m_phsDistribution.stddevOfStopA = DRS4SimulationSettingsManager::sharedInstance()->sigmaPHS511keV_A();
    m_phsDistribution.stddevOfStopB = DRS4SimulationSettingsManager::sharedInstance()->sigmaPHS511keV_B();

    //Device-Info:
    m_deviceInfo.ATS = DRS4SimulationSettingsManager::sharedInstance()->arrivalTimeSpreadInNs();
    m_deviceInfo.MUUncertainty = DRS4SimulationSettingsManager::sharedInstance()->timingResolutionMeasurementUnitInNs();
    m_deviceInfo.numberOfCells = 1024; //fixed
    m_deviceInfo.sweep = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();
    m_deviceInfo.PDSUncertaintyA = DRS4SimulationSettingsManager::sharedInstance()->ttsDetectorAInNs();
    m_deviceInfo.PDSUncertaintyB = DRS4SimulationSettingsManager::sharedInstance()->ttsDetectorBInNs();

    //Pulse-Info:
    m_pulseInfo.amplitude = 500.0f; //fixed
    m_pulseInfo.delay = DRS4SettingsManager::sharedInstance()->triggerDelayInNs();
    m_pulseInfo.pulseWidth = DRS4SimulationSettingsManager::sharedInstance()->pulseWidthInNs();
    m_pulseInfo.isPositiveSignalPolarity = true;
    m_pulseInfo.riseTime = DRS4SimulationSettingsManager::sharedInstance()->riseTimeInNs();

    //Simulation-Input:
    m_simulationInput.lt1_activated = DRS4SimulationSettingsManager::sharedInstance()->isLT1activated();
    m_simulationInput.lt2_activated = DRS4SimulationSettingsManager::sharedInstance()->isLT2activated();
    m_simulationInput.lt3_activated = DRS4SimulationSettingsManager::sharedInstance()->isLT3activated();
    m_simulationInput.lt4_activated = DRS4SimulationSettingsManager::sharedInstance()->isLT4activated();
    m_simulationInput.lt5_activated = DRS4SimulationSettingsManager::sharedInstance()->isLT5activated();

    m_simulationInput.intensity1 = DRS4SimulationSettingsManager::sharedInstance()->intensityLT1();
    m_simulationInput.intensity2 = DRS4SimulationSettingsManager::sharedInstance()->intensityLT2();
    m_simulationInput.intensity3 = DRS4SimulationSettingsManager::sharedInstance()->intensityLT3();
    m_simulationInput.intensity4 = DRS4SimulationSettingsManager::sharedInstance()->intensityLT4();
    m_simulationInput.intensity5 = DRS4SimulationSettingsManager::sharedInstance()->intensityLT5();

    m_simulationInput.tau1 = DRS4SimulationSettingsManager::sharedInstance()->tau1InNs();
    m_simulationInput.tau2 = DRS4SimulationSettingsManager::sharedInstance()->tau2InNs();
    m_simulationInput.tau3 = DRS4SimulationSettingsManager::sharedInstance()->tau3InNs();
    m_simulationInput.tau4 = DRS4SimulationSettingsManager::sharedInstance()->tau4InNs();
    m_simulationInput.tau5 = DRS4SimulationSettingsManager::sharedInstance()->tau5InNs();

    DLifeTime::DLTDistributionInfo distrInfo1;
    DLifeTime::DLTDistributionInfo distrInfo2;
    DLifeTime::DLTDistributionInfo distrInfo3;
    DLifeTime::DLTDistributionInfo distrInfo4;
    DLifeTime::DLTDistributionInfo distrInfo5;

    if ( DRS4SimulationSettingsManager::sharedInstance()->isLT1DistributionEnabled() ) {
        distrInfo1.enabled = DRS4SimulationSettingsManager::sharedInstance()->isLT1DistributionEnabled();
        distrInfo1.gridIncrement = DRS4SimulationSettingsManager::sharedInstance()->gridLT1DistributionIncrementInNs();
        distrInfo1.gridNumber = DRS4SimulationSettingsManager::sharedInstance()->gridLT1DistributionCount();
        distrInfo1.param1 = DRS4SimulationSettingsManager::sharedInstance()->uncertaintyLT1DistributionInNs();

        if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT1Distribution() == "GAUSSIAN" )
            distrInfo1.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT1Distribution() == "LOGNORMAL" )
            distrInfo1.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT1Distribution() == "CAUCHY" )
            distrInfo1.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT1Distribution() == "LORENTZIAN" )
            distrInfo1.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            distrInfo1.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        distrInfo1 = IGNORE_LT_DISTRIBUTION;
    }

    if ( DRS4SimulationSettingsManager::sharedInstance()->isLT2DistributionEnabled() ) {
        distrInfo2.enabled = DRS4SimulationSettingsManager::sharedInstance()->isLT2DistributionEnabled();
        distrInfo2.gridIncrement = DRS4SimulationSettingsManager::sharedInstance()->gridLT2DistributionIncrementInNs();
        distrInfo2.gridNumber = DRS4SimulationSettingsManager::sharedInstance()->gridLT2DistributionCount();
        distrInfo2.param1 = DRS4SimulationSettingsManager::sharedInstance()->uncertaintyLT2DistributionInNs();

        if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT2Distribution() == "GAUSSIAN" )
            distrInfo2.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT2Distribution() == "LOGNORMAL" )
            distrInfo2.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT2Distribution() == "CAUCHY" )
            distrInfo2.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT2Distribution() == "LORENTZIAN" )
            distrInfo2.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            distrInfo2.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        distrInfo2 = IGNORE_LT_DISTRIBUTION;
    }

    if ( DRS4SimulationSettingsManager::sharedInstance()->isLT3DistributionEnabled() ) {
        distrInfo3.enabled = DRS4SimulationSettingsManager::sharedInstance()->isLT3DistributionEnabled();
        distrInfo3.gridIncrement = DRS4SimulationSettingsManager::sharedInstance()->gridLT3DistributionIncrementInNs();
        distrInfo3.gridNumber = DRS4SimulationSettingsManager::sharedInstance()->gridLT3DistributionCount();
        distrInfo3.param1 = DRS4SimulationSettingsManager::sharedInstance()->uncertaintyLT3DistributionInNs();

        if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT3Distribution() == "GAUSSIAN" )
            distrInfo3.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT3Distribution() == "LOGNORMAL" )
            distrInfo3.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT3Distribution() == "CAUCHY" )
            distrInfo3.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT3Distribution() == "LORENTZIAN" )
            distrInfo3.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            distrInfo3.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        distrInfo3 = IGNORE_LT_DISTRIBUTION;
    }

    if ( DRS4SimulationSettingsManager::sharedInstance()->isLT4DistributionEnabled() ) {
        distrInfo4.enabled = DRS4SimulationSettingsManager::sharedInstance()->isLT4DistributionEnabled();
        distrInfo4.gridIncrement = DRS4SimulationSettingsManager::sharedInstance()->gridLT4DistributionIncrementInNs();
        distrInfo4.gridNumber = DRS4SimulationSettingsManager::sharedInstance()->gridLT4DistributionCount();
        distrInfo4.param1 = DRS4SimulationSettingsManager::sharedInstance()->uncertaintyLT4DistributionInNs();

        if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT4Distribution() == "GAUSSIAN" )
            distrInfo4.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT4Distribution() == "LOGNORMAL" )
            distrInfo4.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT4Distribution() == "CAUCHY" )
            distrInfo4.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT4Distribution() == "LORENTZIAN" )
            distrInfo4.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            distrInfo4.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        distrInfo4 = IGNORE_LT_DISTRIBUTION;
    }

    if ( DRS4SimulationSettingsManager::sharedInstance()->isLT5DistributionEnabled() ) {
        distrInfo5.enabled = DRS4SimulationSettingsManager::sharedInstance()->isLT5DistributionEnabled();
        distrInfo5.gridIncrement = DRS4SimulationSettingsManager::sharedInstance()->gridLT5DistributionIncrementInNs();
        distrInfo5.gridNumber = DRS4SimulationSettingsManager::sharedInstance()->gridLT5DistributionCount();
        distrInfo5.param1 = DRS4SimulationSettingsManager::sharedInstance()->uncertaintyLT5DistributionInNs();

        if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT5Distribution() == "GAUSSIAN" )
            distrInfo5.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT5Distribution() == "LOGNORMAL" )
            distrInfo5.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT5Distribution() == "CAUCHY" )
            distrInfo5.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->functionTypeLT5Distribution() == "LORENTZIAN" )
            distrInfo5.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            distrInfo5.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        distrInfo5 = IGNORE_LT_DISTRIBUTION;
    }

    m_simulationInput.tau1Distribution = distrInfo1;
    m_simulationInput.tau2Distribution = distrInfo2;
    m_simulationInput.tau3Distribution = distrInfo3;
    m_simulationInput.tau4Distribution = distrInfo4;
    m_simulationInput.tau5Distribution = distrInfo5;

    m_simulationInput.intensityOfBackgroundOccurrance = DRS4SimulationSettingsManager::sharedInstance()->backgroundIntensity();
    m_simulationInput.intensityOfPromtOccurrance = DRS4SimulationSettingsManager::sharedInstance()->coincidenceIntensity();

    m_simulationInput.isStartStopAlternating = true;

    DDELETE_SAFETY(m_generator);
    m_generator = new DLTPulseGenerator(m_simulationInput, m_phsDistribution, m_deviceInfo, m_pulseInfo, this);
}

bool DRS4PulseGenerator::receiveGeneratedPulsePair(float *pulseATime, float *pulseAVoltage, float *pulseBTime, float *pulseBVoltage)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_generator )
        return false;

    if ( !pulseATime
         || !pulseAVoltage
         || !pulseBTime
         || !pulseBVoltage )
        return false;

    const int fractDouble = 1/sizeof(double);

    double tChannel0d[kNumberOfBins] = {0}; std::fill(tChannel0d, tChannel0d + sizeof(tChannel0d)*fractDouble, 0);
    double tChannel1d[kNumberOfBins] = {0}; std::fill(tChannel1d, tChannel1d + sizeof(tChannel1d)*fractDouble, 0);

    double waveChannel0d[kNumberOfBins] = {0}; std::fill(waveChannel0d, waveChannel0d + sizeof(waveChannel0d)*fractDouble, 0);
    double waveChannel1d[kNumberOfBins] = {0}; std::fill(waveChannel1d, waveChannel1d + sizeof(waveChannel1d)*fractDouble, 0);

    DLifeTime::DLTPulseF pulseA(tChannel0d, waveChannel0d);
    DLifeTime::DLTPulseF pulseB(tChannel1d, waveChannel1d);

    if ( m_generator->emitPulses(&pulseA,
                                                 &pulseB,
                                                 DRS4SettingsManager::sharedInstance()->triggerLevelAmV(),
                                                 DRS4SettingsManager::sharedInstance()->triggerLevelBmV()) )
    {
        if ( pulseA.size() != pulseB.size() )
            return false;

        for ( int i = 0 ; i < kNumberOfBins ; ++ i ) {
            pulseATime[i] = tChannel0d[i];
            pulseBTime[i] = tChannel1d[i];
            pulseAVoltage[i] = waveChannel0d[i];
            pulseBVoltage[i] = waveChannel1d[i];
        }

        return true;
    }

    return false;
}

DRS4PulseGenerator::DRS4PulseGenerator() :
    m_generator(nullptr),
    m_type(LogNormalPulse),
    m_error(DLifeTime::NONE_ERROR) {}

DRS4PulseGenerator::~DRS4PulseGenerator()
{
    DDELETE_SAFETY(m_generator);
    DDELETE_SAFETY(__sharedInstancePulseGenerator);
}
