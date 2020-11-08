/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2020 Danny Petschke
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
*****************************************************************************
**
** related publications:
**
** when using DDRS4PALS for your research purposes please cite:
**
** DDRS4PALS: A software for the acquisition and simulation of lifetime spectra using the DRS4 evaluation board:
** https://www.sciencedirect.com/science/article/pii/S2352711019300676
**
** and
**
** Data on pure tin by Positron Annihilation Lifetime Spectroscopy (PALS) acquired with a semi-analog/digital setup using DDRS4PALS
** https://www.sciencedirect.com/science/article/pii/S2352340918315142?via%3Dihub
**
** when using the integrated simulation tool /DLTPulseGenerator/ of DDRS4PALS for your research purposes please cite:
**
** DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses
** https://www.sciencedirect.com/science/article/pii/S2352711018300530
**
** Update (v1.1) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses
** https://www.sciencedirect.com/science/article/pii/S2352711018300694
**
** Update (v1.2) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses
** https://www.sciencedirect.com/science/article/pii/S2352711018301092
**
** Update (v1.3) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses
** https://www.sciencedirect.com/science/article/pii/S235271101930038X
**/

#include "drs4pulsegenerator.h"

using namespace DLifeTime;

static DRS4PulseGenerator *__sharedInstancePulseGenerator = DNULLPTR;

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
        if ((m_error & DLTErrorType::INVALID_SUM_OF_PDS_IRF_INTENSITIES))
            errorStr += "<li><font color=\"Red\">invalid sum of irf intensities: PDS</font></li>";
        if ((m_error & DLTErrorType::INVALID_SUM_OF_MU_IRF_INTENSITIES))
            errorStr += "<li><font color=\"Red\">invalid sum of irf intensities: MU</font></li>";
        if ((m_error & DLTErrorType::INVALID_VOLTAGE_BASELINE_JITTER))
            errorStr += "<li><font color=\"Red\">invalid voltage aperture baseline jitter value</font></li>";
        if ((m_error & DLTErrorType::INVALID_VOLTAGE_RND_NOISE))
            errorStr += "<li><font color=\"Red\">invalid voltage random noise value</font></li>";
        if ((m_error & DLTErrorType::INVALID_TIME_NONLINEARITY_FIXED_APERTURE_JITTER))
            errorStr += "<li><font color=\"Red\">invalid time axis (fixed) pattern aperture jitter value</font></li>";
        if ((m_error & DLTErrorType::INVALID_TIME_NONLINEARITY_RND_APERTURE_JITTER))
            errorStr += "<li><font color=\"Red\">invalid time axis random aperture jitter value</font></li>";
        if ((m_error & DLTErrorType::INVALID_DIGITIZATION_DEPTH))
            errorStr += "<li><font color=\"Red\">invalid digitization depth value</font></li>";
    }

    errorStr += "</b></lu>";

    return errorStr;
}

void DRS4PulseGenerator::update()
{
    QMutexLocker locker(&m_mutex);

    //PHS:
    m_phsDistribution.meanOfStartA = -DRS4SimulationSettingsManager::sharedInstance()->meanPHS1274keV_A();
    m_phsDistribution.meanOfStartB = -DRS4SimulationSettingsManager::sharedInstance()->meanPHS1274keV_B();

    m_phsDistribution.meanOfStopA = -DRS4SimulationSettingsManager::sharedInstance()->meanPHS511keV_A();
    m_phsDistribution.meanOfStopB = -DRS4SimulationSettingsManager::sharedInstance()->meanPHS511keV_B();

    m_phsDistribution.stddevOfStartA = -DRS4SimulationSettingsManager::sharedInstance()->sigmaPHS1274keV_A();
    m_phsDistribution.stddevOfStartB = -DRS4SimulationSettingsManager::sharedInstance()->sigmaPHS1274keV_B();

    m_phsDistribution.stddevOfStopA = -DRS4SimulationSettingsManager::sharedInstance()->sigmaPHS511keV_A();
    m_phsDistribution.stddevOfStopB = -DRS4SimulationSettingsManager::sharedInstance()->sigmaPHS511keV_B();

    //Device-Info:
    m_deviceInfo.ATS = DRS4SimulationSettingsManager::sharedInstance()->arrivalTimeSpreadInNs();    
    m_deviceInfo.numberOfCells = kNumberOfBins; //fixed for DDRS4PALS
    m_deviceInfo.sweep = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();

    /* PDS-A-1 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_1_enabled() ) {
        m_deviceInfo.irfA.irf1PDS.enabled = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_1_enabled();
        m_deviceInfo.irfA.irf1PDS.intensity = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_1_intensity();
        m_deviceInfo.irfA.irf1PDS.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_1_shift();
        m_deviceInfo.irfA.irf1PDS.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_1_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_1_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfA.irf1PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_1_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfA.irf1PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_1_functionType() == "CAUCHY" )
            m_deviceInfo.irfA.irf1PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_1_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfA.irf1PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
           m_deviceInfo.irfA.irf1PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
         m_deviceInfo.irfA.irf1PDS = IGNORE_DLTIRF;
    }

    /* PDS-A-2 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_2_enabled() ) {
        m_deviceInfo.irfA.irf2PDS.enabled = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_2_enabled();
        m_deviceInfo.irfA.irf2PDS.intensity = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_2_intensity();
        m_deviceInfo.irfA.irf2PDS.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_2_shift();
        m_deviceInfo.irfA.irf2PDS.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_2_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_2_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfA.irf2PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_2_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfA.irf2PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_2_functionType() == "CAUCHY" )
            m_deviceInfo.irfA.irf2PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_2_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfA.irf2PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
           m_deviceInfo.irfA.irf2PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
         m_deviceInfo.irfA.irf2PDS = IGNORE_DLTIRF;
    }

    /* PDS-A-3 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_3_enabled() ) {
        m_deviceInfo.irfA.irf3PDS.enabled = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_3_enabled();
        m_deviceInfo.irfA.irf3PDS.intensity = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_3_intensity();
        m_deviceInfo.irfA.irf3PDS.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_3_shift();
        m_deviceInfo.irfA.irf3PDS.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_3_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_3_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfA.irf3PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_3_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfA.irf3PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_3_functionType() == "CAUCHY" )
            m_deviceInfo.irfA.irf3PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_3_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfA.irf3PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfA.irf3PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfA.irf3PDS = IGNORE_DLTIRF;
    }

    /* PDS-A-4 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_4_enabled() ) {
        m_deviceInfo.irfA.irf4PDS.enabled = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_4_enabled();
        m_deviceInfo.irfA.irf4PDS.intensity = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_4_intensity();
        m_deviceInfo.irfA.irf4PDS.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_4_shift();
        m_deviceInfo.irfA.irf4PDS.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_4_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_4_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfA.irf4PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_4_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfA.irf4PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_4_functionType() == "CAUCHY" )
            m_deviceInfo.irfA.irf4PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_4_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfA.irf4PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfA.irf4PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfA.irf4PDS = IGNORE_DLTIRF;
    }

    /* PDS-A-5 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_5_enabled() ) {
        m_deviceInfo.irfA.irf5PDS.enabled = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_5_enabled();
        m_deviceInfo.irfA.irf5PDS.intensity = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_5_intensity();
        m_deviceInfo.irfA.irf5PDS.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_5_shift();
        m_deviceInfo.irfA.irf5PDS.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_5_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_5_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfA.irf5PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_5_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfA.irf5PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_5_functionType() == "CAUCHY" )
            m_deviceInfo.irfA.irf5PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorA_irf_5_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfA.irf5PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfA.irf5PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfA.irf5PDS = IGNORE_DLTIRF;
    }


    /* PDS-B-1 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_1_enabled() ) {
        m_deviceInfo.irfB.irf1PDS.enabled = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_1_enabled();
        m_deviceInfo.irfB.irf1PDS.intensity = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_1_intensity();
        m_deviceInfo.irfB.irf1PDS.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_1_shift();
        m_deviceInfo.irfB.irf1PDS.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_1_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_1_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfB.irf1PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_1_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfB.irf1PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_1_functionType() == "CAUCHY" )
            m_deviceInfo.irfB.irf1PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_1_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfB.irf1PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfB.irf1PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfB.irf1PDS = IGNORE_DLTIRF;
    }

    /* PDS-B-2 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_2_enabled() ) {
        m_deviceInfo.irfB.irf2PDS.enabled = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_2_enabled();
        m_deviceInfo.irfB.irf2PDS.intensity = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_2_intensity();
        m_deviceInfo.irfB.irf2PDS.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_2_shift();
        m_deviceInfo.irfB.irf2PDS.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_2_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_2_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfB.irf2PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_2_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfB.irf2PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_2_functionType() == "CAUCHY" )
            m_deviceInfo.irfB.irf2PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_2_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfB.irf2PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfB.irf2PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfB.irf2PDS = IGNORE_DLTIRF;
    }

    /* PDS-B-3 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_3_enabled() ) {
        m_deviceInfo.irfB.irf3PDS.enabled = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_3_enabled();
        m_deviceInfo.irfB.irf3PDS.intensity = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_3_intensity();
        m_deviceInfo.irfB.irf3PDS.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_3_shift();
        m_deviceInfo.irfB.irf3PDS.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_3_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_3_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfB.irf3PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_3_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfB.irf3PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_3_functionType() == "CAUCHY" )
            m_deviceInfo.irfB.irf3PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_3_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfB.irf3PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfB.irf3PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfB.irf3PDS = IGNORE_DLTIRF;
    }

    /* PDS-B-4 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_4_enabled() ) {
        m_deviceInfo.irfB.irf4PDS.enabled = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_4_enabled();
        m_deviceInfo.irfB.irf4PDS.intensity = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_4_intensity();
        m_deviceInfo.irfB.irf4PDS.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_4_shift();
        m_deviceInfo.irfB.irf4PDS.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_4_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_4_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfB.irf4PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_4_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfB.irf4PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_4_functionType() == "CAUCHY" )
            m_deviceInfo.irfB.irf4PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_4_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfB.irf4PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfB.irf4PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfB.irf4PDS = IGNORE_DLTIRF;
    }

    /* PDS-B-5 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_5_enabled() ) {
        m_deviceInfo.irfB.irf5PDS.enabled = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_5_enabled();
        m_deviceInfo.irfB.irf5PDS.intensity = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_5_intensity();
        m_deviceInfo.irfB.irf5PDS.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_5_shift();
        m_deviceInfo.irfB.irf5PDS.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_5_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_5_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfB.irf5PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_5_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfB.irf5PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_5_functionType() == "CAUCHY" )
            m_deviceInfo.irfB.irf5PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->detectorB_irf_5_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfB.irf5PDS.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfB.irf5PDS.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfB.irf5PDS = IGNORE_DLTIRF;
    }


    /* MU-1 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_1_enabled() ) {
        m_deviceInfo.irfMU.irf1MU.enabled = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_1_enabled();
        m_deviceInfo.irfMU.irf1MU.intensity = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_1_intensity();
        m_deviceInfo.irfMU.irf1MU.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_1_shift();
        m_deviceInfo.irfMU.irf1MU.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_1_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_1_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfMU.irf1MU.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_1_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfMU.irf1MU.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_1_functionType() == "CAUCHY" )
            m_deviceInfo.irfMU.irf1MU.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_1_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfMU.irf1MU.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfMU.irf1MU.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfMU.irf1MU = IGNORE_DLTIRF;
    }

    /* MU-2 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_2_enabled() ) {
        m_deviceInfo.irfMU.irf2MU.enabled = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_2_enabled();
        m_deviceInfo.irfMU.irf2MU.intensity = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_2_intensity();
        m_deviceInfo.irfMU.irf2MU.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_2_shift();
        m_deviceInfo.irfMU.irf2MU.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_2_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_2_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfMU.irf2MU.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_2_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfMU.irf2MU.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_2_functionType() == "CAUCHY" )
            m_deviceInfo.irfMU.irf2MU.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_2_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfMU.irf2MU.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfMU.irf2MU.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfMU.irf2MU = IGNORE_DLTIRF;
    }

    /* MU-3 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_3_enabled() ) {
        m_deviceInfo.irfMU.irf3MU.enabled = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_3_enabled();
        m_deviceInfo.irfMU.irf3MU.intensity = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_3_intensity();
        m_deviceInfo.irfMU.irf3MU.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_3_shift();
        m_deviceInfo.irfMU.irf3MU.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_3_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_3_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfMU.irf3MU.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_3_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfMU.irf3MU.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_3_functionType() == "CAUCHY" )
            m_deviceInfo.irfMU.irf3MU.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_3_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfMU.irf3MU.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfMU.irf3MU.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfMU.irf3MU = IGNORE_DLTIRF;
    }

    /* MU-4 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_4_enabled() ) {
        m_deviceInfo.irfMU.irf4MU.enabled = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_4_enabled();
        m_deviceInfo.irfMU.irf4MU.intensity = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_4_intensity();
        m_deviceInfo.irfMU.irf4MU.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_4_shift();
        m_deviceInfo.irfMU.irf4MU.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_4_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_4_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfMU.irf4MU.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_4_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfMU.irf4MU.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_4_functionType() == "CAUCHY" )
            m_deviceInfo.irfMU.irf4MU.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_4_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfMU.irf4MU.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfMU.irf4MU.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfMU.irf4MU = IGNORE_DLTIRF;
    }

    /* MU-5 */
    if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_5_enabled() ) {
        m_deviceInfo.irfMU.irf5MU.enabled = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_5_enabled();
        m_deviceInfo.irfMU.irf5MU.intensity = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_5_intensity();
        m_deviceInfo.irfMU.irf5MU.relativeShift = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_5_shift();
        m_deviceInfo.irfMU.irf5MU.uncertainty = DRS4SimulationSettingsManager::sharedInstance()->mu_irf_5_uncertainty();

        if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_5_functionType() == "GAUSSIAN" )
            m_deviceInfo.irfMU.irf5MU.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_5_functionType() == "LOGNORMAL" )
            m_deviceInfo.irfMU.irf5MU.functionType = DLifeTime::DLTDistributionFunction::Function::LOG_NORMAL;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_5_functionType() == "CAUCHY" )
            m_deviceInfo.irfMU.irf5MU.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else if ( DRS4SimulationSettingsManager::sharedInstance()->mu_irf_5_functionType() == "LORENTZIAN" )
            m_deviceInfo.irfMU.irf5MU.functionType = DLifeTime::DLTDistributionFunction::Function::LORENTZIAN_CAUCHY;
        else
            m_deviceInfo.irfMU.irf5MU.functionType = DLifeTime::DLTDistributionFunction::Function::GAUSSIAN;
    } else {
        m_deviceInfo.irfMU.irf5MU = IGNORE_DLTIRF;
    }

    /* Pulse-Info */
    m_pulseInfo.amplitude = -500.0f; // fixed for DDRS4PALS software
    m_pulseInfo.delay = DRS4SettingsManager::sharedInstance()->triggerDelayInNs();
    m_pulseInfo.isPositiveSignalPolarity = false;

    if (DRS4SimulationSettingsManager::sharedInstance()->isDigitizationEnabled()) {
        m_pulseInfo.digitizationInfo.enabled = true;
        m_pulseInfo.digitizationInfo.digitizationDepth = DRS4SimulationSettingsManager::sharedInstance()->digitizationDepthInBit();
    }
    else
        m_pulseInfo.digitizationInfo = IGNORE_DIGITIZATION;

    /* Pulse - A */
    m_pulseInfo.pulseA.riseTime = DRS4SimulationSettingsManager::sharedInstance()->riseTimeInNs_A();
    m_pulseInfo.pulseA.pulseWidth = DRS4SimulationSettingsManager::sharedInstance()->pulseWidthInNs_A();

    if (DRS4SimulationSettingsManager::sharedInstance()->isBaselineOffsetJitterEnabled_A()) {
        m_pulseInfo.pulseA.baselineOffsetJitterInfoV.enabled = true;
        m_pulseInfo.pulseA.baselineOffsetJitterInfoV.meanOfBaselineOffsetJitter = DRS4SimulationSettingsManager::sharedInstance()->baselineOffsetJitterMean_in_mV_A();
        m_pulseInfo.pulseA.baselineOffsetJitterInfoV.stddevOfBaselineOffsetJitter = DRS4SimulationSettingsManager::sharedInstance()->baselineOffsetJitterStddev_in_mV_A();
    }
    else
        m_pulseInfo.pulseA.baselineOffsetJitterInfoV = IGNORE_BASELINE_JITTER_V;

    if (DRS4SimulationSettingsManager::sharedInstance()->isRandomNoiseEnabled_A()) {
        m_pulseInfo.pulseA.randomNoiseInfoV.enabled = true;
        m_pulseInfo.pulseA.randomNoiseInfoV.rndNoise = DRS4SimulationSettingsManager::sharedInstance()->randomNoise_in_mV_A();
    }
    else
        m_pulseInfo.pulseA.randomNoiseInfoV = IGNORE_RND_NOISE_V;

    if (DRS4SimulationSettingsManager::sharedInstance()->isTimeAxisNonlinearityEnabled_A()) {
        m_pulseInfo.pulseA.timeAxisNonLinearityInfoT.enabled = true;
        m_pulseInfo.pulseA.timeAxisNonLinearityInfoT.fixedPatternApertureJitter = DRS4SimulationSettingsManager::sharedInstance()->nonlinearityFixedApertureJitter_in_ns_A();
        m_pulseInfo.pulseA.timeAxisNonLinearityInfoT.rndApertureJitter = DRS4SimulationSettingsManager::sharedInstance()->nonlinearityRandomApertureJitter_in_ns_A();
    }
    else
        m_pulseInfo.pulseA.timeAxisNonLinearityInfoT = IGNORE_AXIS_NONLINEARITY_T;

    /* Pulse - B */
    m_pulseInfo.pulseB.riseTime = DRS4SimulationSettingsManager::sharedInstance()->riseTimeInNs_B();
    m_pulseInfo.pulseB.pulseWidth = DRS4SimulationSettingsManager::sharedInstance()->pulseWidthInNs_B();

    if (DRS4SimulationSettingsManager::sharedInstance()->isBaselineOffsetJitterEnabled_B()) {
        m_pulseInfo.pulseB.baselineOffsetJitterInfoV.enabled = true;
        m_pulseInfo.pulseB.baselineOffsetJitterInfoV.meanOfBaselineOffsetJitter = DRS4SimulationSettingsManager::sharedInstance()->baselineOffsetJitterMean_in_mV_B();
        m_pulseInfo.pulseB.baselineOffsetJitterInfoV.stddevOfBaselineOffsetJitter = DRS4SimulationSettingsManager::sharedInstance()->baselineOffsetJitterStddev_in_mV_B();
    }
    else
        m_pulseInfo.pulseB.baselineOffsetJitterInfoV = IGNORE_BASELINE_JITTER_V;

    if (DRS4SimulationSettingsManager::sharedInstance()->isRandomNoiseEnabled_B()) {
        m_pulseInfo.pulseB.randomNoiseInfoV.enabled = true;
        m_pulseInfo.pulseB.randomNoiseInfoV.rndNoise = DRS4SimulationSettingsManager::sharedInstance()->randomNoise_in_mV_B();
    }
    else
        m_pulseInfo.pulseB.randomNoiseInfoV = IGNORE_RND_NOISE_V;

    if (DRS4SimulationSettingsManager::sharedInstance()->isTimeAxisNonlinearityEnabled_B()) {
        m_pulseInfo.pulseB.timeAxisNonLinearityInfoT.enabled = true;
        m_pulseInfo.pulseB.timeAxisNonLinearityInfoT.fixedPatternApertureJitter = DRS4SimulationSettingsManager::sharedInstance()->nonlinearityFixedApertureJitter_in_ns_B();
        m_pulseInfo.pulseB.timeAxisNonLinearityInfoT.rndApertureJitter = DRS4SimulationSettingsManager::sharedInstance()->nonlinearityRandomApertureJitter_in_ns_B();
    }
    else
        m_pulseInfo.pulseB.timeAxisNonLinearityInfoT = IGNORE_AXIS_NONLINEARITY_T;

    /* Simulation-Input */
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
    m_generator(DNULLPTR),
    m_type(LogNormalPulse),
    m_error(DLifeTime::NONE_ERROR) {}

DRS4PulseGenerator::~DRS4PulseGenerator()
{
    DDELETE_SAFETY(m_generator);
    DDELETE_SAFETY(__sharedInstancePulseGenerator);
}
