/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2019 Danny Petschke
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

#include "drs4simulationsettingsmanager.h"

DRS4SimulationSettingsManager *__sharedInstanceSimulationSettingsManager = nullptr;

DRS4SimulationSettingsManager *DRS4SimulationSettingsManager::sharedInstance()
{
    if ( !__sharedInstanceSimulationSettingsManager )
        __sharedInstanceSimulationSettingsManager = new DRS4SimulationSettingsManager();

    return __sharedInstanceSimulationSettingsManager;
}

DSimpleXMLNode *DRS4SimulationSettingsManager::parentNode() const
{
    return m_parentNode;
}

QString DRS4SimulationSettingsManager::fileName() const
{
    QMutexLocker locker(&m_mutex);

    return m_fileName;
}

QString DRS4SimulationSettingsManager::xmlContent() const
{
    QMutexLocker locker(&m_mutex);

    if (m_parentNode)
        return (QString)DSimpleXMLString(m_parentNode);

        return QString("");
}

DRS4SimulationSettingsManager::DRS4SimulationSettingsManager()
{
    m_parentNode = new DSimpleXMLNode("DLTPulseGenerator-library-simulation-input");

    m_electronicsNode = new DSimpleXMLNode("hardware-setup");

        m_irfPDSA_parentNode = new DSimpleXMLNode("irf-PDS-A");

            m_irfPDSA_1_parentNode = new DSimpleXMLNode("irf-1");

            m_irfPDSA_1_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfPDSA_1_enabled->setValue(true);
            m_irfPDSA_1_functionType = new DSimpleXMLNode("function-type");
            m_irfPDSA_1_functionType->setValue("GAUSSIAN");
            m_irfPDSA_1_intensity = new DSimpleXMLNode("intensity");
            m_irfPDSA_1_intensity->setValue(1.0);
            m_irfPDSA_1_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfPDSA_1_uncertainty->setValue(0.0849329f);
            m_irfPDSA_1_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfPDSA_1_relativeShift->setValue(0.0);

            m_irfPDSA_2_parentNode = new DSimpleXMLNode("irf-2");

            m_irfPDSA_2_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfPDSA_2_enabled->setValue(false);
            m_irfPDSA_2_functionType = new DSimpleXMLNode("function-type");
            m_irfPDSA_2_functionType->setValue("GAUSSIAN");
            m_irfPDSA_2_intensity = new DSimpleXMLNode("intensity");
            m_irfPDSA_2_intensity->setValue(1.0);
            m_irfPDSA_2_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfPDSA_2_uncertainty->setValue(0.0849329f);
            m_irfPDSA_2_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfPDSA_2_relativeShift->setValue(0.0);


            m_irfPDSA_3_parentNode = new DSimpleXMLNode("irf-3");

            m_irfPDSA_3_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfPDSA_3_enabled->setValue(false);
            m_irfPDSA_3_functionType = new DSimpleXMLNode("function-type");
            m_irfPDSA_3_functionType->setValue("GAUSSIAN");
            m_irfPDSA_3_intensity = new DSimpleXMLNode("intensity");
            m_irfPDSA_3_intensity->setValue(1.0);
            m_irfPDSA_3_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfPDSA_3_uncertainty->setValue(0.0849329f);
            m_irfPDSA_3_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfPDSA_3_relativeShift->setValue(0.0);


            m_irfPDSA_4_parentNode = new DSimpleXMLNode("irf-4");

            m_irfPDSA_4_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfPDSA_4_enabled->setValue(false);
            m_irfPDSA_4_functionType = new DSimpleXMLNode("function-type");
            m_irfPDSA_4_functionType->setValue("GAUSSIAN");
            m_irfPDSA_4_intensity = new DSimpleXMLNode("intensity");
            m_irfPDSA_4_intensity->setValue(1.0);
            m_irfPDSA_4_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfPDSA_4_uncertainty->setValue(0.0849329f);
            m_irfPDSA_4_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfPDSA_4_relativeShift->setValue(0.0);


            m_irfPDSA_5_parentNode = new DSimpleXMLNode("irf-5");

            m_irfPDSA_5_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfPDSA_5_enabled->setValue(false);
            m_irfPDSA_5_functionType = new DSimpleXMLNode("function-type");
            m_irfPDSA_5_functionType->setValue("GAUSSIAN");
            m_irfPDSA_5_intensity = new DSimpleXMLNode("intensity");
            m_irfPDSA_5_intensity->setValue(1.0);
            m_irfPDSA_5_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfPDSA_5_uncertainty->setValue(0.0849329f);
            m_irfPDSA_5_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfPDSA_5_relativeShift->setValue(0.0);


        m_irfPDSB_parentNode = new DSimpleXMLNode("irf-PDS-B");

            m_irfPDSB_1_parentNode = new DSimpleXMLNode("irf-1");

            m_irfPDSB_1_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfPDSB_1_enabled->setValue(true);
            m_irfPDSB_1_functionType = new DSimpleXMLNode("function-type");
            m_irfPDSB_1_functionType->setValue("GAUSSIAN");
            m_irfPDSB_1_intensity = new DSimpleXMLNode("intensity");
            m_irfPDSB_1_intensity->setValue(1.0);
            m_irfPDSB_1_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfPDSB_1_uncertainty->setValue(0.0849329f);
            m_irfPDSB_1_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfPDSB_1_relativeShift->setValue(0.0);


            m_irfPDSB_2_parentNode = new DSimpleXMLNode("irf-2");

            m_irfPDSB_2_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfPDSB_2_enabled->setValue(false);
            m_irfPDSB_2_functionType = new DSimpleXMLNode("function-type");
            m_irfPDSB_2_functionType->setValue("GAUSSIAN");
            m_irfPDSB_2_intensity = new DSimpleXMLNode("intensity");
            m_irfPDSB_2_intensity->setValue(1.0);
            m_irfPDSB_2_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfPDSB_2_uncertainty->setValue(0.0849329f);
            m_irfPDSB_2_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfPDSB_2_relativeShift->setValue(0.0);


            m_irfPDSB_3_parentNode = new DSimpleXMLNode("irf-3");

            m_irfPDSB_3_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfPDSB_3_enabled->setValue(false);
            m_irfPDSB_3_functionType = new DSimpleXMLNode("function-type");
            m_irfPDSB_3_functionType->setValue("GAUSSIAN");
            m_irfPDSB_3_intensity = new DSimpleXMLNode("intensity");
            m_irfPDSB_3_intensity->setValue(1.0);
            m_irfPDSB_3_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfPDSB_3_uncertainty->setValue(0.0849329f);
            m_irfPDSB_3_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfPDSB_3_relativeShift->setValue(0.0);


            m_irfPDSB_4_parentNode = new DSimpleXMLNode("irf-4");

            m_irfPDSB_4_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfPDSB_4_enabled->setValue(false);
            m_irfPDSB_4_functionType = new DSimpleXMLNode("function-type");
            m_irfPDSB_4_functionType->setValue("GAUSSIAN");
            m_irfPDSB_4_intensity = new DSimpleXMLNode("intensity");
            m_irfPDSB_4_intensity->setValue(1.0);
            m_irfPDSB_4_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfPDSB_4_uncertainty->setValue(0.0849329f);
            m_irfPDSB_4_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfPDSB_4_relativeShift->setValue(0.0);


            m_irfPDSB_5_parentNode = new DSimpleXMLNode("irf-5");

            m_irfPDSB_5_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfPDSB_5_enabled->setValue(false);
            m_irfPDSB_5_functionType = new DSimpleXMLNode("function-type");
            m_irfPDSB_5_functionType->setValue("GAUSSIAN");
            m_irfPDSB_5_intensity = new DSimpleXMLNode("intensity");
            m_irfPDSB_5_intensity->setValue(1.0);
            m_irfPDSB_5_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfPDSB_5_uncertainty->setValue(0.0849329f);
            m_irfPDSB_5_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfPDSB_5_relativeShift->setValue(0.0);

        m_irfMU_parentNode = new DSimpleXMLNode("irf-MU");

            m_irfMU_1_parentNode = new DSimpleXMLNode("irf-1");

            m_irfMU_1_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfMU_1_enabled->setValue(true);
            m_irfMU_1_functionType = new DSimpleXMLNode("function-type");
            m_irfMU_1_functionType->setValue("GAUSSIAN");
            m_irfMU_1_intensity = new DSimpleXMLNode("intensity");
            m_irfMU_1_intensity->setValue(1.0);
            m_irfMU_1_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfMU_1_uncertainty->setValue(0.0025f);
            m_irfMU_1_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfMU_1_relativeShift->setValue(0.0);


            m_irfMU_2_parentNode = new DSimpleXMLNode("irf-2");

            m_irfMU_2_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfMU_2_enabled->setValue(false);
            m_irfMU_2_functionType = new DSimpleXMLNode("function-type");
            m_irfMU_2_functionType->setValue("GAUSSIAN");
            m_irfMU_2_intensity = new DSimpleXMLNode("intensity");
            m_irfMU_2_intensity->setValue(1.0);
            m_irfMU_2_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfMU_2_uncertainty->setValue(0.0025f);
            m_irfMU_2_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfMU_2_relativeShift->setValue(0.0);


            m_irfMU_3_parentNode = new DSimpleXMLNode("irf-3");

            m_irfMU_3_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfMU_3_enabled->setValue(false);
            m_irfMU_3_functionType = new DSimpleXMLNode("function-type");
            m_irfMU_3_functionType->setValue("GAUSSIAN");
            m_irfMU_3_intensity = new DSimpleXMLNode("intensity");
            m_irfMU_3_intensity->setValue(1.0);
            m_irfMU_3_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfMU_3_uncertainty->setValue(0.0025f);
            m_irfMU_3_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfMU_3_relativeShift->setValue(0.0);


            m_irfMU_4_parentNode = new DSimpleXMLNode("irf-4");

            m_irfMU_4_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfMU_4_enabled->setValue(false);
            m_irfMU_4_functionType = new DSimpleXMLNode("function-type");
            m_irfMU_4_functionType->setValue("GAUSSIAN");
            m_irfMU_4_intensity = new DSimpleXMLNode("intensity");
            m_irfMU_4_intensity->setValue(1.0);
            m_irfMU_4_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfMU_4_uncertainty->setValue(0.0025f);
            m_irfMU_4_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfMU_4_relativeShift->setValue(0.0);


            m_irfMU_5_parentNode = new DSimpleXMLNode("irf-5");

            m_irfMU_5_enabled = new DSimpleXMLNode("is-enabled?");
            m_irfMU_5_enabled->setValue(false);
            m_irfMU_5_functionType = new DSimpleXMLNode("function-type");
            m_irfMU_5_functionType->setValue("GAUSSIAN");
            m_irfMU_5_intensity = new DSimpleXMLNode("intensity");
            m_irfMU_5_intensity->setValue(1.0);
            m_irfMU_5_uncertainty = new DSimpleXMLNode("uncertainty-in-ns");
            m_irfMU_5_uncertainty->setValue(0.0025f);
            m_irfMU_5_relativeShift = new DSimpleXMLNode("shift-in-ns");
            m_irfMU_5_relativeShift->setValue(0.0);


    //Pulse-Shape:
    m_pulseShapeParentNode = new DSimpleXMLNode("detector-output-pulse-shape");

    m_pulseShapeDigitizationParentNode = new DSimpleXMLNode("pulse-digitization");
    m_pulseShapeDigitization_enabled_Node = new DSimpleXMLNode("is-enabled?");
    m_pulseShapeDigitization_enabled_Node->setValue(true);
    m_pulseShapeDigitization_depth_in_bit_Node = new DSimpleXMLNode("digitization-depth-in-bit");
    m_pulseShapeDigitization_depth_in_bit_Node->setValue(14);

    //Pulse A:
    m_pulseShape_A_ParentNode = new DSimpleXMLNode("detector-A");

    m_pulseShapeRiseTimeInNS_A_Node = new DSimpleXMLNode("rise-time-in-ns");
    m_pulseShapeRiseTimeInNS_A_Node->setValue(5.0f);
    m_pulseShapeWidthInNS_A_Node = new DSimpleXMLNode("pulse-width-in-ns");
    m_pulseShapeWidthInNS_A_Node->setValue(0.165f);

    m_pulseShapeBaselineJitter_A_ParentNode = new DSimpleXMLNode("baseline-offset-jitter");
    m_pulseShapeBaselineJitter_enabled_A_Node = new DSimpleXMLNode("is-enabled?");
    m_pulseShapeBaselineJitter_enabled_A_Node->setValue(true);
    m_pulseShapeBaselineJitter_mean_A_Node = new DSimpleXMLNode("mean-of-baseline-in-mV");
    m_pulseShapeBaselineJitter_mean_A_Node->setValue(0.0f);
    m_pulseShapeBaselineJitter_stddev_A_Node = new DSimpleXMLNode("stddev-of-baseline-in-mV");
    m_pulseShapeBaselineJitter_stddev_A_Node->setValue(5.0f);

    m_pulseShapeRandomNoise_A_ParentNode = new DSimpleXMLNode("random-noise");
    m_pulseShapeRandomNoise_enabled_A_Node = new DSimpleXMLNode("is-enabled?");
    m_pulseShapeRandomNoise_enabled_A_Node->setValue(true);
    m_pulseShapeRandomNoise_stddev_A_Node = new DSimpleXMLNode("noise-in-mV");
    m_pulseShapeRandomNoise_stddev_A_Node->setValue(0.35f);

    m_pulseShapeTimeNonlinearity_A_ParentNode = new DSimpleXMLNode("time-axis-non-linearity");
    m_pulseShapeNonLinearity_enabled_A_Node = new DSimpleXMLNode("is-enabled?");
    m_pulseShapeNonLinearity_enabled_A_Node->setValue(true);
    m_pulseShapeNonLinearity_fixedApertureJitter_A_Node = new DSimpleXMLNode("fixed-aperture-jitter-in-ns");
    m_pulseShapeNonLinearity_fixedApertureJitter_A_Node->setValue(0.005f);
    m_pulseShapeNonLinearity_randomApertureJitter_A_Node = new DSimpleXMLNode("random-aperture-jitter-in-ns");
    m_pulseShapeNonLinearity_randomApertureJitter_A_Node->setValue(0.001f);

    //Pulse B:
    m_pulseShape_B_ParentNode = new DSimpleXMLNode("detector-B");

    m_pulseShapeRiseTimeInNS_B_Node = new DSimpleXMLNode("rise-time-in-ns");
    m_pulseShapeRiseTimeInNS_B_Node->setValue(5.0f);
    m_pulseShapeWidthInNS_B_Node = new DSimpleXMLNode("pulse-width-in-ns");
    m_pulseShapeWidthInNS_B_Node->setValue(0.165f);

    m_pulseShapeBaselineJitter_B_ParentNode = new DSimpleXMLNode("baseline-offset-jitter");
    m_pulseShapeBaselineJitter_enabled_B_Node = new DSimpleXMLNode("is-enabled?");
    m_pulseShapeBaselineJitter_enabled_B_Node->setValue(true);
    m_pulseShapeBaselineJitter_mean_B_Node = new DSimpleXMLNode("mean-of-baseline-in-mV");
    m_pulseShapeBaselineJitter_mean_B_Node->setValue(0.0f);
    m_pulseShapeBaselineJitter_stddev_B_Node = new DSimpleXMLNode("stddev-of-baseline-in-mV");
    m_pulseShapeBaselineJitter_stddev_B_Node->setValue(5.0f);

    m_pulseShapeRandomNoise_B_ParentNode = new DSimpleXMLNode("random-noise");
    m_pulseShapeRandomNoise_enabled_B_Node = new DSimpleXMLNode("is-enabled?");
    m_pulseShapeRandomNoise_enabled_B_Node->setValue(true);
    m_pulseShapeRandomNoise_stddev_B_Node = new DSimpleXMLNode("noise-in-mV");
    m_pulseShapeRandomNoise_stddev_B_Node->setValue(0.35f);

    m_pulseShapeTimeNonlinearity_B_ParentNode = new DSimpleXMLNode("time-axis-non-linearity");
    m_pulseShapeNonLinearity_enabled_B_Node = new DSimpleXMLNode("is-enabled?");
    m_pulseShapeNonLinearity_enabled_B_Node->setValue(true);
    m_pulseShapeNonLinearity_fixedApertureJitter_B_Node = new DSimpleXMLNode("fixed-aperture-jitter-in-ns");
    m_pulseShapeNonLinearity_fixedApertureJitter_B_Node->setValue(0.005f);
    m_pulseShapeNonLinearity_randomApertureJitter_B_Node = new DSimpleXMLNode("random-aperture-jitter-in-ns");
    m_pulseShapeNonLinearity_randomApertureJitter_B_Node->setValue(0.001f);

    //PHS:
    m_phsParentNode = new DSimpleXMLNode("pulse-height-spectrum-PHS");

    m_meanPHS_A_511Node = new DSimpleXMLNode("mean-stop-window-A-in-mV");
    m_meanPHS_A_511Node->setValue(90.0f);
    m_meanPHS_B_511Node = new DSimpleXMLNode("mean-stop-window-B-in-mV");
    m_meanPHS_B_511Node->setValue(90.0f);
    m_meanPHS_A_1274Node = new DSimpleXMLNode("mean-start-window-A-in-mV");
    m_meanPHS_A_1274Node->setValue(190.0f);
    m_meanPHS_B_1274Node = new DSimpleXMLNode("mean-start-window-B-in-mV");
    m_meanPHS_B_1274Node->setValue(190.0f);

    m_sigmaPHS_A_511Node = new DSimpleXMLNode("standard-deviation-stop-window-A-in-mV");
    m_sigmaPHS_A_511Node->setValue(25.0f);
    m_sigmaPHS_B_511Node = new DSimpleXMLNode("standard-deviation-stop-window-B-in-mV");
    m_sigmaPHS_B_511Node->setValue(25.0f);
    m_sigmaPHS_A_1274Node = new DSimpleXMLNode("standard-deviation-start-window-A-in-mV");
    m_sigmaPHS_A_1274Node->setValue(150.0f);
    m_sigmaPHS_B_1274Node = new DSimpleXMLNode("standard-deviation-start-window-B-in-mV");
    m_sigmaPHS_B_1274Node->setValue(150.0f);

    //LT:
    m_ltParentNode = new DSimpleXMLNode("lifetime-spectrum-simulation-input");
    m_ltDistr1ParentNode = new DSimpleXMLNode("lifetime-1-distribution-information");
    m_ltDistr2ParentNode = new DSimpleXMLNode("lifetime-2-distribution-information");
    m_ltDistr3ParentNode = new DSimpleXMLNode("lifetime-3-distribution-information");
    m_ltDistr4ParentNode = new DSimpleXMLNode("lifetime-4-distribution-information");
    m_ltDistr5ParentNode = new DSimpleXMLNode("lifetime-5-distribution-information");

    m_backgroundI = new DSimpleXMLNode("background-intensity");
    m_backgroundI->setValue(0.1f);

    m_coincidenceI = new DSimpleXMLNode("prompt-intensity");
    m_coincidenceI->setValue(0.2f);

    m_enableLT1 = new DSimpleXMLNode("lifetime-1-enabled?");
    m_enableLT1->setValue(true);
    m_enableLT2 = new DSimpleXMLNode("lifetime-2-enabled?");
    m_enableLT2->setValue(true);
    m_enableLT3 = new DSimpleXMLNode("lifetime-3-enabled?");
    m_enableLT3->setValue(true);
    m_enableLT4 = new DSimpleXMLNode("lifetime-4-enabled?");
    m_enableLT4->setValue(false);
    m_enableLT5 = new DSimpleXMLNode("lifetime-5-enabled?");
    m_enableLT5->setValue(false);

    m_tau1_NS = new DSimpleXMLNode("lifetime-1-tau-in-ns");
    m_tau1_NS->setValue(0.160f);
    m_tau2_NS = new DSimpleXMLNode("lifetime-2-tau-in-ns");
    m_tau2_NS->setValue(0.355f);
    m_tau3_NS = new DSimpleXMLNode("lifetime-3-tau-in-ns");
    m_tau3_NS->setValue(1.250f);
    m_tau4_NS = new DSimpleXMLNode("lifetime-4-tau-in-ns");
    m_tau4_NS->setValue(1.2f);
    m_tau5_NS = new DSimpleXMLNode("lifetime-5-tau-in-ns");
    m_tau5_NS->setValue(3.6f);

    m_I1_NS = new DSimpleXMLNode("lifetime-1-intensity");
    m_I1_NS->setValue(0.9f);
    m_I2_NS = new DSimpleXMLNode("lifetime-2-intensityy");
    m_I2_NS->setValue(0.185f);
    m_I3_NS = new DSimpleXMLNode("lifetime-3-intensity");
    m_I3_NS->setValue(0.015f);
    m_I4_NS = new DSimpleXMLNode("lifetime-4-intensity");
    m_I4_NS->setValue(0.3f);
    m_I5_NS = new DSimpleXMLNode("lifetime-5-intensity");
    m_I5_NS->setValue(0.2f);

    m_tau1_isDistributionEnabled = new DSimpleXMLNode("is-enabled?");
    m_tau1_isDistributionEnabled->setValue(false);
    m_tau2_isDistributionEnabled = new DSimpleXMLNode("is-enabled?");
    m_tau2_isDistributionEnabled->setValue(false);
    m_tau3_isDistributionEnabled = new DSimpleXMLNode("is-enabled?");
    m_tau3_isDistributionEnabled->setValue(false);
    m_tau4_isDistributionEnabled = new DSimpleXMLNode("is-enabled?");
    m_tau4_isDistributionEnabled->setValue(false);
    m_tau5_isDistributionEnabled = new DSimpleXMLNode("is-enabled?");
    m_tau5_isDistributionEnabled->setValue(false);

    m_tau1_distrUncertainty_NS = new DSimpleXMLNode("uncertainty-in-ns");
    m_tau1_distrUncertainty_NS->setValue(0.0f);
    m_tau2_distrUncertainty_NS = new DSimpleXMLNode("uncertainty-in-ns");
    m_tau2_distrUncertainty_NS->setValue(0.0f);
    m_tau3_distrUncertainty_NS = new DSimpleXMLNode("uncertainty-in-ns");
    m_tau3_distrUncertainty_NS->setValue(0.0f);
    m_tau4_distrUncertainty_NS = new DSimpleXMLNode("uncertainty-in-ns");
    m_tau4_distrUncertainty_NS->setValue(0.0f);
    m_tau5_distrUncertainty_NS = new DSimpleXMLNode("uncertainty-in-ns");
    m_tau5_distrUncertainty_NS->setValue(0.0f);

    m_tau1_distrFunctionType = new DSimpleXMLNode("function-type");
    m_tau1_distrFunctionType->setValue("GAUSSIAN");
    m_tau2_distrFunctionType = new DSimpleXMLNode("function-type");
    m_tau2_distrFunctionType->setValue("GAUSSIAN");
    m_tau3_distrFunctionType = new DSimpleXMLNode("function-type");
    m_tau3_distrFunctionType->setValue("GAUSSIAN");
    m_tau4_distrFunctionType = new DSimpleXMLNode("function-type");
    m_tau4_distrFunctionType->setValue("GAUSSIAN");
    m_tau5_distrFunctionType = new DSimpleXMLNode("function-type");
    m_tau5_distrFunctionType->setValue("GAUSSIAN");

    m_tau1_distrGridNumber = new DSimpleXMLNode("grid-count");
    m_tau1_distrGridNumber->setValue(0);
    m_tau2_distrGridNumber = new DSimpleXMLNode("grid-count");
    m_tau2_distrGridNumber->setValue(0);
    m_tau3_distrGridNumber = new DSimpleXMLNode("grid-count");
    m_tau3_distrGridNumber->setValue(0);
    m_tau4_distrGridNumber = new DSimpleXMLNode("grid-count");
    m_tau4_distrGridNumber->setValue(0);
    m_tau5_distrGridNumber = new DSimpleXMLNode("grid-count");
    m_tau5_distrGridNumber->setValue(0);

    m_tau1_distrGridIncrement = new DSimpleXMLNode("grid-increment-in-ns");
    m_tau1_distrGridIncrement->setValue(0.0f);
    m_tau2_distrGridIncrement = new DSimpleXMLNode("grid-increment-in-ns");
    m_tau2_distrGridIncrement->setValue(0.0f);
    m_tau3_distrGridIncrement = new DSimpleXMLNode("grid-increment-in-ns");
    m_tau3_distrGridIncrement->setValue(0.0f);
    m_tau4_distrGridIncrement = new DSimpleXMLNode("grid-increment-in-ns");
    m_tau4_distrGridIncrement->setValue(0.0f);
    m_tau5_distrGridIncrement = new DSimpleXMLNode("grid-increment-in-ns");
    m_tau5_distrGridIncrement->setValue(0.0f);

    //electronics:
    m_arrivalTimeSpreadInNsNode = new DSimpleXMLNode("arrival-time-spread-ATS-in-ns");
    m_arrivalTimeSpreadInNsNode->setValue(0.25f);


    //setup XML-table:
    (*m_phsParentNode) << m_meanPHS_A_511Node << m_meanPHS_B_511Node << m_meanPHS_A_1274Node << m_meanPHS_B_1274Node
                                    << m_sigmaPHS_A_511Node << m_sigmaPHS_B_511Node << m_sigmaPHS_A_1274Node << m_sigmaPHS_B_1274Node;

        (*m_pulseShapeDigitizationParentNode) << m_pulseShapeDigitization_enabled_Node << m_pulseShapeDigitization_depth_in_bit_Node;

            (*m_pulseShapeBaselineJitter_A_ParentNode) << m_pulseShapeBaselineJitter_enabled_A_Node << m_pulseShapeBaselineJitter_mean_A_Node << m_pulseShapeBaselineJitter_stddev_A_Node;
            (*m_pulseShapeRandomNoise_A_ParentNode) << m_pulseShapeRandomNoise_enabled_A_Node << m_pulseShapeRandomNoise_stddev_A_Node;
            (*m_pulseShapeTimeNonlinearity_A_ParentNode) << m_pulseShapeNonLinearity_enabled_A_Node << m_pulseShapeNonLinearity_fixedApertureJitter_A_Node << m_pulseShapeNonLinearity_randomApertureJitter_A_Node;

            (*m_pulseShapeBaselineJitter_B_ParentNode) << m_pulseShapeBaselineJitter_enabled_B_Node << m_pulseShapeBaselineJitter_mean_B_Node << m_pulseShapeBaselineJitter_stddev_B_Node;
            (*m_pulseShapeRandomNoise_B_ParentNode) << m_pulseShapeRandomNoise_enabled_B_Node << m_pulseShapeRandomNoise_stddev_B_Node;
            (*m_pulseShapeTimeNonlinearity_B_ParentNode) << m_pulseShapeNonLinearity_enabled_B_Node << m_pulseShapeNonLinearity_fixedApertureJitter_B_Node << m_pulseShapeNonLinearity_randomApertureJitter_B_Node;

        (*m_pulseShape_A_ParentNode) << m_pulseShapeRiseTimeInNS_A_Node << m_pulseShapeWidthInNS_A_Node << m_pulseShapeBaselineJitter_A_ParentNode << m_pulseShapeRandomNoise_A_ParentNode << m_pulseShapeTimeNonlinearity_A_ParentNode;
        (*m_pulseShape_B_ParentNode) << m_pulseShapeRiseTimeInNS_B_Node << m_pulseShapeWidthInNS_B_Node << m_pulseShapeBaselineJitter_B_ParentNode << m_pulseShapeRandomNoise_B_ParentNode << m_pulseShapeTimeNonlinearity_B_ParentNode;

    (*m_pulseShapeParentNode) << m_pulseShape_A_ParentNode << m_pulseShape_B_ParentNode << m_pulseShapeDigitizationParentNode;

        (*m_ltDistr1ParentNode) << m_tau1_isDistributionEnabled << m_tau1_distrFunctionType << m_tau1_distrUncertainty_NS << m_tau1_distrGridNumber << m_tau1_distrGridIncrement;
        (*m_ltDistr2ParentNode) << m_tau2_isDistributionEnabled << m_tau2_distrFunctionType << m_tau2_distrUncertainty_NS << m_tau2_distrGridNumber << m_tau2_distrGridIncrement;
        (*m_ltDistr3ParentNode) << m_tau3_isDistributionEnabled << m_tau3_distrFunctionType << m_tau3_distrUncertainty_NS << m_tau3_distrGridNumber << m_tau3_distrGridIncrement;
        (*m_ltDistr4ParentNode) << m_tau4_isDistributionEnabled << m_tau4_distrFunctionType << m_tau4_distrUncertainty_NS << m_tau4_distrGridNumber << m_tau4_distrGridIncrement;
        (*m_ltDistr5ParentNode) << m_tau5_isDistributionEnabled << m_tau5_distrFunctionType << m_tau5_distrUncertainty_NS << m_tau5_distrGridNumber << m_tau5_distrGridIncrement;

    (*m_ltParentNode) << m_backgroundI << m_coincidenceI
                                  << m_enableLT1 <<  m_enableLT2 << m_enableLT3 << m_enableLT4 << m_enableLT5
                                  << m_tau1_NS << m_I1_NS << m_ltDistr1ParentNode
                                  << m_tau2_NS << m_I2_NS << m_ltDistr2ParentNode
                                  << m_tau3_NS << m_I3_NS << m_ltDistr3ParentNode
                                  << m_tau4_NS << m_I4_NS << m_ltDistr4ParentNode
                                  << m_tau5_NS << m_I5_NS << m_ltDistr5ParentNode;

    (*m_irfPDSA_1_parentNode) << m_irfPDSA_1_enabled << m_irfPDSA_1_functionType << m_irfPDSA_1_intensity << m_irfPDSA_1_uncertainty << m_irfPDSA_1_relativeShift;
    (*m_irfPDSA_2_parentNode) << m_irfPDSA_2_enabled << m_irfPDSA_2_functionType << m_irfPDSA_2_intensity << m_irfPDSA_2_uncertainty << m_irfPDSA_2_relativeShift;
    (*m_irfPDSA_3_parentNode) << m_irfPDSA_3_enabled << m_irfPDSA_3_functionType << m_irfPDSA_3_intensity << m_irfPDSA_3_uncertainty << m_irfPDSA_3_relativeShift;
    (*m_irfPDSA_4_parentNode) << m_irfPDSA_4_enabled << m_irfPDSA_4_functionType << m_irfPDSA_4_intensity << m_irfPDSA_4_uncertainty << m_irfPDSA_4_relativeShift;
    (*m_irfPDSA_5_parentNode) << m_irfPDSA_5_enabled << m_irfPDSA_5_functionType << m_irfPDSA_5_intensity << m_irfPDSA_5_uncertainty << m_irfPDSA_5_relativeShift;

    (*m_irfPDSB_1_parentNode) << m_irfPDSB_1_enabled << m_irfPDSB_1_functionType << m_irfPDSB_1_intensity << m_irfPDSB_1_uncertainty << m_irfPDSB_1_relativeShift;
    (*m_irfPDSB_2_parentNode) << m_irfPDSB_2_enabled << m_irfPDSB_2_functionType << m_irfPDSB_2_intensity << m_irfPDSB_2_uncertainty << m_irfPDSB_2_relativeShift;
    (*m_irfPDSB_3_parentNode) << m_irfPDSB_3_enabled << m_irfPDSB_3_functionType << m_irfPDSB_3_intensity << m_irfPDSB_3_uncertainty << m_irfPDSB_3_relativeShift;
    (*m_irfPDSB_4_parentNode) << m_irfPDSB_4_enabled << m_irfPDSB_4_functionType << m_irfPDSB_4_intensity << m_irfPDSB_4_uncertainty << m_irfPDSB_4_relativeShift;
    (*m_irfPDSB_5_parentNode) << m_irfPDSB_5_enabled << m_irfPDSB_5_functionType << m_irfPDSB_5_intensity << m_irfPDSB_5_uncertainty << m_irfPDSB_5_relativeShift;

    (*m_irfMU_1_parentNode) << m_irfMU_1_enabled << m_irfMU_1_functionType << m_irfMU_1_intensity << m_irfMU_1_uncertainty << m_irfMU_1_relativeShift;
    (*m_irfMU_2_parentNode) << m_irfMU_2_enabled << m_irfMU_2_functionType << m_irfMU_2_intensity << m_irfMU_2_uncertainty << m_irfMU_2_relativeShift;
    (*m_irfMU_3_parentNode) << m_irfMU_3_enabled << m_irfMU_3_functionType << m_irfMU_3_intensity << m_irfMU_3_uncertainty << m_irfMU_3_relativeShift;
    (*m_irfMU_4_parentNode) << m_irfMU_4_enabled << m_irfMU_4_functionType << m_irfMU_4_intensity << m_irfMU_4_uncertainty << m_irfMU_4_relativeShift;
    (*m_irfMU_5_parentNode) << m_irfMU_5_enabled << m_irfMU_5_functionType << m_irfMU_5_intensity << m_irfMU_5_uncertainty << m_irfMU_5_relativeShift;

    (*m_irfPDSA_parentNode) << m_irfPDSA_1_parentNode << m_irfPDSA_2_parentNode << m_irfPDSA_3_parentNode << m_irfPDSA_4_parentNode << m_irfPDSA_5_parentNode;
    (*m_irfPDSB_parentNode) << m_irfPDSB_1_parentNode << m_irfPDSB_2_parentNode << m_irfPDSB_3_parentNode << m_irfPDSB_4_parentNode << m_irfPDSB_5_parentNode;
    (*m_irfMU_parentNode) << m_irfMU_1_parentNode << m_irfMU_2_parentNode << m_irfMU_3_parentNode << m_irfMU_4_parentNode << m_irfMU_5_parentNode;

    (*m_electronicsNode) << m_irfPDSA_parentNode << m_irfPDSB_parentNode << m_irfMU_parentNode << m_arrivalTimeSpreadInNsNode;


    (*m_parentNode) << m_electronicsNode << m_phsParentNode << m_pulseShapeParentNode << m_ltParentNode;
}

DRS4SimulationSettingsManager::~DRS4SimulationSettingsManager()
{
    DDELETE_SAFETY(m_parentNode);
    DDELETE_SAFETY(__sharedInstanceSimulationSettingsManager);
}

bool DRS4SimulationSettingsManager::load(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    DSimpleXMLReader reader(path);

    DSimpleXMLTag tag;

    if (!reader.readFromFile(&tag, false))
        return false;


    bool ok = false;

    const DSimpleXMLTag parentTag = tag.getTag(m_parentNode, &ok);

    /* -----> Electronic Settings <-----*/
    const DSimpleXMLTag electronicTag = parentTag.getTag(m_electronicsNode, &ok);

    if ( !ok )
    {
        m_irfPDSA_1_enabled->setValue(true);
        m_irfPDSA_1_functionType->setValue("GAUSSIAN");
        m_irfPDSA_1_intensity->setValue(1.0);
        m_irfPDSA_1_uncertainty->setValue(0.0849329f);
        m_irfPDSA_1_relativeShift->setValue(0.0);

        m_irfPDSA_2_enabled->setValue(false);
        m_irfPDSA_2_functionType->setValue("GAUSSIAN");
        m_irfPDSA_2_intensity->setValue(1.0);
        m_irfPDSA_2_uncertainty->setValue(0.0849329f);
        m_irfPDSA_2_relativeShift->setValue(0.0);

        m_irfPDSA_3_enabled->setValue(false);
        m_irfPDSA_3_functionType->setValue("GAUSSIAN");
        m_irfPDSA_3_intensity->setValue(1.0);
        m_irfPDSA_3_uncertainty->setValue(0.0849329f);
        m_irfPDSA_3_relativeShift->setValue(0.0);

        m_irfPDSA_4_enabled->setValue(false);
        m_irfPDSA_4_functionType->setValue("GAUSSIAN");
        m_irfPDSA_4_intensity->setValue(1.0);
        m_irfPDSA_4_uncertainty->setValue(0.0849329f);
        m_irfPDSA_4_relativeShift->setValue(0.0);

        m_irfPDSA_5_enabled->setValue(false);
        m_irfPDSA_5_functionType->setValue("GAUSSIAN");
        m_irfPDSA_5_intensity->setValue(1.0);
        m_irfPDSA_5_uncertainty->setValue(0.0849329f);
        m_irfPDSA_5_relativeShift->setValue(0.0);


        m_irfPDSB_1_enabled->setValue(true);
        m_irfPDSB_1_functionType->setValue("GAUSSIAN");
        m_irfPDSB_1_intensity->setValue(1.0);
        m_irfPDSB_1_uncertainty->setValue(0.0849329f);
        m_irfPDSB_1_relativeShift->setValue(0.0);

        m_irfPDSB_2_enabled->setValue(false);
        m_irfPDSB_2_functionType->setValue("GAUSSIAN");
        m_irfPDSB_2_intensity->setValue(1.0);
        m_irfPDSB_2_uncertainty->setValue(0.0849329f);
        m_irfPDSB_2_relativeShift->setValue(0.0);

        m_irfPDSB_3_enabled->setValue(false);
        m_irfPDSB_3_functionType->setValue("GAUSSIAN");
        m_irfPDSB_3_intensity->setValue(1.0);
        m_irfPDSB_3_uncertainty->setValue(0.0849329f);
        m_irfPDSB_3_relativeShift->setValue(0.0);

        m_irfPDSB_4_enabled->setValue(false);
        m_irfPDSB_4_functionType->setValue("GAUSSIAN");
        m_irfPDSB_4_intensity->setValue(1.0);
        m_irfPDSB_4_uncertainty->setValue(0.0849329f);
        m_irfPDSB_4_relativeShift->setValue(0.0);

        m_irfPDSB_5_enabled->setValue(false);
        m_irfPDSB_5_functionType->setValue("GAUSSIAN");
        m_irfPDSB_5_intensity->setValue(1.0);
        m_irfPDSB_5_uncertainty->setValue(0.0849329f);
        m_irfPDSB_5_relativeShift->setValue(0.0);


        m_irfMU_1_enabled->setValue(true);
        m_irfMU_1_functionType->setValue("GAUSSIAN");
        m_irfMU_1_intensity->setValue(1.0);
        m_irfMU_1_uncertainty->setValue(0.0025f);
        m_irfMU_1_relativeShift->setValue(0.0);

        m_irfMU_2_enabled->setValue(false);
        m_irfMU_2_functionType->setValue("GAUSSIAN");
        m_irfMU_2_intensity->setValue(1.0);
        m_irfMU_2_uncertainty->setValue(0.0025f);
        m_irfMU_2_relativeShift->setValue(0.0);

        m_irfMU_3_enabled->setValue(false);
        m_irfMU_3_functionType->setValue("GAUSSIAN");
        m_irfMU_3_intensity->setValue(1.0);
        m_irfMU_3_uncertainty->setValue(0.0025f);
        m_irfMU_3_relativeShift->setValue(0.0);

        m_irfMU_4_enabled->setValue(false);
        m_irfMU_4_functionType->setValue("GAUSSIAN");
        m_irfMU_4_intensity->setValue(1.0);
        m_irfMU_4_uncertainty->setValue(0.0025f);
        m_irfMU_4_relativeShift->setValue(0.0);

        m_irfMU_5_enabled->setValue(false);
        m_irfMU_5_functionType->setValue("GAUSSIAN");
        m_irfMU_5_intensity->setValue(1.0);
        m_irfMU_5_uncertainty->setValue(0.0025f);
        m_irfMU_5_relativeShift->setValue(0.0);

        m_arrivalTimeSpreadInNsNode->setValue(0.25f);
    }
    else
    {
        const double  arrivalTimeSpreadInNs = electronicTag.getValueAt(m_arrivalTimeSpreadInNsNode, &ok).toDouble(&ok);
        if ( ok )
            m_arrivalTimeSpreadInNsNode->setValue(arrivalTimeSpreadInNs);
        else
            m_arrivalTimeSpreadInNsNode->setValue(0.25f);

        bool ok2 = false, ok3 = false;
        bool ok_A_1 = false, ok_A_2 = false, ok_A_3 = false, ok_A_4 = false, ok_A_5 = false;
        bool ok_B_1 = false, ok_B_2 = false, ok_B_3 = false, ok_B_4 = false, ok_B_5 = false;
        bool ok_MU_1 = false, ok_MU_2 = false, ok_MU_3 = false, ok_MU_4 = false, ok_MU_5 = false;

        const DSimpleXMLTag pdsA_irf = electronicTag.getTag(m_irfPDSA_parentNode, &ok);

        const DSimpleXMLTag pdsA_irf_1 = pdsA_irf.getTag(m_irfPDSA_1_parentNode, &ok_A_1);
        const DSimpleXMLTag pdsA_irf_2 = pdsA_irf.getTag(m_irfPDSA_2_parentNode, &ok_A_2);
        const DSimpleXMLTag pdsA_irf_3 = pdsA_irf.getTag(m_irfPDSA_3_parentNode, &ok_A_3);
        const DSimpleXMLTag pdsA_irf_4 = pdsA_irf.getTag(m_irfPDSA_4_parentNode, &ok_A_4);
        const DSimpleXMLTag pdsA_irf_5 = pdsA_irf.getTag(m_irfPDSA_5_parentNode, &ok_A_5);

        const DSimpleXMLTag pdsB_irf = electronicTag.getTag(m_irfPDSB_parentNode, &ok2);

        const DSimpleXMLTag pdsB_irf_1 = pdsB_irf.getTag(m_irfPDSB_1_parentNode, &ok_B_1);
        const DSimpleXMLTag pdsB_irf_2 = pdsB_irf.getTag(m_irfPDSB_2_parentNode, &ok_B_2);
        const DSimpleXMLTag pdsB_irf_3 = pdsB_irf.getTag(m_irfPDSB_3_parentNode, &ok_B_3);
        const DSimpleXMLTag pdsB_irf_4 = pdsB_irf.getTag(m_irfPDSB_4_parentNode, &ok_B_4);
        const DSimpleXMLTag pdsB_irf_5 = pdsB_irf.getTag(m_irfPDSB_5_parentNode, &ok_B_5);

        const DSimpleXMLTag mu_irf = electronicTag.getTag(m_irfMU_parentNode, &ok3);

        const DSimpleXMLTag mu_irf_1 = mu_irf.getTag(m_irfMU_1_parentNode, &ok_MU_1);
        const DSimpleXMLTag mu_irf_2 = mu_irf.getTag(m_irfMU_2_parentNode, &ok_MU_2);
        const DSimpleXMLTag mu_irf_3 = mu_irf.getTag(m_irfMU_3_parentNode, &ok_MU_3);
        const DSimpleXMLTag mu_irf_4 = mu_irf.getTag(m_irfMU_4_parentNode, &ok_MU_4);
        const DSimpleXMLTag mu_irf_5 = mu_irf.getTag(m_irfMU_5_parentNode, &ok_MU_5);

        const bool iok = (ok && ok2 && ok3 && ok_A_1 && ok_A_2 && ok_A_3 && ok_A_4 && ok_A_5 && ok_B_1 && ok_B_2 && ok_B_3 && ok_B_4 && ok_B_5 && ok_MU_1 && ok_MU_2 && ok_MU_3 && ok_MU_4 && ok_MU_5);

        if ( !iok ) {
            m_irfPDSA_1_enabled->setValue(true);
            m_irfPDSA_1_functionType->setValue("GAUSSIAN");
            m_irfPDSA_1_intensity->setValue(1.0);
            m_irfPDSA_1_uncertainty->setValue(0.0849329f);
            m_irfPDSA_1_relativeShift->setValue(0.0);

            m_irfPDSA_2_enabled->setValue(false);
            m_irfPDSA_2_functionType->setValue("GAUSSIAN");
            m_irfPDSA_2_intensity->setValue(1.0);
            m_irfPDSA_2_uncertainty->setValue(0.0849329f);
            m_irfPDSA_2_relativeShift->setValue(0.0);

            m_irfPDSA_3_enabled->setValue(false);
            m_irfPDSA_3_functionType->setValue("GAUSSIAN");
            m_irfPDSA_3_intensity->setValue(1.0);
            m_irfPDSA_3_uncertainty->setValue(0.0849329f);
            m_irfPDSA_3_relativeShift->setValue(0.0);

            m_irfPDSA_4_enabled->setValue(false);
            m_irfPDSA_4_functionType->setValue("GAUSSIAN");
            m_irfPDSA_4_intensity->setValue(1.0);
            m_irfPDSA_4_uncertainty->setValue(0.0849329f);
            m_irfPDSA_4_relativeShift->setValue(0.0);

            m_irfPDSA_5_enabled->setValue(false);
            m_irfPDSA_5_functionType->setValue("GAUSSIAN");
            m_irfPDSA_5_intensity->setValue(1.0);
            m_irfPDSA_5_uncertainty->setValue(0.0849329f);
            m_irfPDSA_5_relativeShift->setValue(0.0);


            m_irfPDSB_1_enabled->setValue(true);
            m_irfPDSB_1_functionType->setValue("GAUSSIAN");
            m_irfPDSB_1_intensity->setValue(1.0);
            m_irfPDSB_1_uncertainty->setValue(0.0849329f);
            m_irfPDSB_1_relativeShift->setValue(0.0);

            m_irfPDSB_2_enabled->setValue(false);
            m_irfPDSB_2_functionType->setValue("GAUSSIAN");
            m_irfPDSB_2_intensity->setValue(1.0);
            m_irfPDSB_2_uncertainty->setValue(0.0849329f);
            m_irfPDSB_2_relativeShift->setValue(0.0);

            m_irfPDSB_3_enabled->setValue(false);
            m_irfPDSB_3_functionType->setValue("GAUSSIAN");
            m_irfPDSB_3_intensity->setValue(1.0);
            m_irfPDSB_3_uncertainty->setValue(0.0849329f);
            m_irfPDSB_3_relativeShift->setValue(0.0);

            m_irfPDSB_4_enabled->setValue(false);
            m_irfPDSB_4_functionType->setValue("GAUSSIAN");
            m_irfPDSB_4_intensity->setValue(1.0);
            m_irfPDSB_4_uncertainty->setValue(0.0849329f);
            m_irfPDSB_4_relativeShift->setValue(0.0);

            m_irfPDSB_5_enabled->setValue(false);
            m_irfPDSB_5_functionType->setValue("GAUSSIAN");
            m_irfPDSB_5_intensity->setValue(1.0);
            m_irfPDSB_5_uncertainty->setValue(0.0849329f);
            m_irfPDSB_5_relativeShift->setValue(0.0);


            m_irfMU_1_enabled->setValue(true);
            m_irfMU_1_functionType->setValue("GAUSSIAN");
            m_irfMU_1_intensity->setValue(1.0);
            m_irfMU_1_uncertainty->setValue(0.0025f);
            m_irfMU_1_relativeShift->setValue(0.0);

            m_irfMU_2_enabled->setValue(false);
            m_irfMU_2_functionType->setValue("GAUSSIAN");
            m_irfMU_2_intensity->setValue(1.0);
            m_irfMU_2_uncertainty->setValue(0.0025f);
            m_irfMU_2_relativeShift->setValue(0.0);

            m_irfMU_3_enabled->setValue(false);
            m_irfMU_3_functionType->setValue("GAUSSIAN");
            m_irfMU_3_intensity->setValue(1.0);
            m_irfMU_3_uncertainty->setValue(0.0025f);
            m_irfMU_3_relativeShift->setValue(0.0);

            m_irfMU_4_enabled->setValue(false);
            m_irfMU_4_functionType->setValue("GAUSSIAN");
            m_irfMU_4_intensity->setValue(1.0);
            m_irfMU_4_uncertainty->setValue(0.0025f);
            m_irfMU_4_relativeShift->setValue(0.0);

            m_irfMU_5_enabled->setValue(false);
            m_irfMU_5_functionType->setValue("GAUSSIAN");
            m_irfMU_5_intensity->setValue(1.0);
            m_irfMU_5_uncertainty->setValue(0.0025f);
            m_irfMU_5_relativeShift->setValue(0.0);
        }
        else
        {
            /* PDS-A */
            bool  enabled_1 = pdsA_irf_1.getValueAt(m_irfPDSA_1_enabled, &ok).toBool();
            if ( ok ) m_irfPDSA_1_enabled->setValue(enabled_1);
            else m_irfPDSA_1_enabled->setValue(false);

            QString  ft_1 = pdsA_irf_1.getValueAt(m_irfPDSA_1_functionType, &ok).toString();
            if ( ok ) m_irfPDSA_1_functionType->setValue(ft_1);
            else m_irfPDSA_1_functionType->setValue("GAUSSIAN");

            float intensity_1 = pdsA_irf_1.getValueAt(m_irfPDSA_1_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_1_intensity->setValue(intensity_1);
            else m_irfPDSA_1_intensity->setValue(1.0);

            float uncertainty_1 = pdsA_irf_1.getValueAt(m_irfPDSA_1_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_1_uncertainty->setValue(uncertainty_1);
            else m_irfPDSA_1_uncertainty->setValue(0.0849329f);

            float shift_1 = pdsA_irf_1.getValueAt(m_irfPDSA_1_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_1_relativeShift->setValue(shift_1);
            else m_irfPDSA_1_relativeShift->setValue(0.0f);

            bool  enabled_2 = pdsA_irf_2.getValueAt(m_irfPDSA_2_enabled, &ok).toBool();
            if ( ok ) m_irfPDSA_2_enabled->setValue(enabled_2);
            else m_irfPDSA_2_enabled->setValue(false);

            QString  ft_2 = pdsA_irf_2.getValueAt(m_irfPDSA_2_functionType, &ok).toString();
            if ( ok ) m_irfPDSA_2_functionType->setValue(ft_2);
            else m_irfPDSA_2_functionType->setValue("GAUSSIAN");

            float intensity_2 = pdsA_irf_2.getValueAt(m_irfPDSA_2_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_2_intensity->setValue(intensity_2);
            else m_irfPDSA_2_intensity->setValue(1.0);

            float uncertainty_2 = pdsA_irf_2.getValueAt(m_irfPDSA_2_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_2_uncertainty->setValue(uncertainty_2);
            else m_irfPDSA_2_uncertainty->setValue(0.0849329f);

            float shift_2 = pdsA_irf_2.getValueAt(m_irfPDSA_2_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_2_relativeShift->setValue(shift_2);
            else m_irfPDSA_2_relativeShift->setValue(0.0f);

            bool  enabled_3 = pdsA_irf_3.getValueAt(m_irfPDSA_3_enabled, &ok).toBool();
            if ( ok ) m_irfPDSA_3_enabled->setValue(enabled_3);
            else m_irfPDSA_3_enabled->setValue(false);

            QString  ft_3 = pdsA_irf_3.getValueAt(m_irfPDSA_3_functionType, &ok).toString();
            if ( ok ) m_irfPDSA_3_functionType->setValue(ft_3);
            else m_irfPDSA_3_functionType->setValue("GAUSSIAN");

            float intensity_3 = pdsA_irf_3.getValueAt(m_irfPDSA_3_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_3_intensity->setValue(intensity_3);
            else m_irfPDSA_3_intensity->setValue(1.0);

            float uncertainty_3 = pdsA_irf_3.getValueAt(m_irfPDSA_3_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_3_uncertainty->setValue(uncertainty_3);
            else m_irfPDSA_3_uncertainty->setValue(0.0849329f);

            float shift_3 = pdsA_irf_3.getValueAt(m_irfPDSA_3_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_3_relativeShift->setValue(shift_3);
            else m_irfPDSA_3_relativeShift->setValue(0.0f);

            bool  enabled_4 = pdsA_irf_4.getValueAt(m_irfPDSA_4_enabled, &ok).toBool();
            if ( ok ) m_irfPDSA_4_enabled->setValue(enabled_4);
            else m_irfPDSA_4_enabled->setValue(false);

            QString  ft_4 = pdsA_irf_4.getValueAt(m_irfPDSA_4_functionType, &ok).toString();
            if ( ok ) m_irfPDSA_4_functionType->setValue(ft_4);
            else m_irfPDSA_4_functionType->setValue("GAUSSIAN");

            float intensity_4 = pdsA_irf_4.getValueAt(m_irfPDSA_4_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_4_intensity->setValue(intensity_4);
            else m_irfPDSA_4_intensity->setValue(1.0);

            float uncertainty_4 = pdsA_irf_4.getValueAt(m_irfPDSA_4_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_4_uncertainty->setValue(uncertainty_4);
            else m_irfPDSA_4_uncertainty->setValue(0.0849329f);

            float shift_4 = pdsA_irf_4.getValueAt(m_irfPDSA_4_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_4_relativeShift->setValue(shift_4);
            else m_irfPDSA_4_relativeShift->setValue(0.0f);

            bool  enabled_5 = pdsA_irf_5.getValueAt(m_irfPDSA_5_enabled, &ok).toBool();
            if ( ok ) m_irfPDSA_5_enabled->setValue(enabled_5);
            else m_irfPDSA_5_enabled->setValue(false);

            QString  ft_5 = pdsA_irf_5.getValueAt(m_irfPDSA_5_functionType, &ok).toString();
            if ( ok ) m_irfPDSA_5_functionType->setValue(ft_5);
            else m_irfPDSA_5_functionType->setValue("GAUSSIAN");

            float intensity_5 = pdsA_irf_5.getValueAt(m_irfPDSA_5_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_5_intensity->setValue(intensity_5);
            else m_irfPDSA_5_intensity->setValue(1.0);

            float uncertainty_5 = pdsA_irf_5.getValueAt(m_irfPDSA_5_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_5_uncertainty->setValue(uncertainty_5);
            else m_irfPDSA_5_uncertainty->setValue(0.0849329f);

            float shift_5 = pdsA_irf_5.getValueAt(m_irfPDSA_5_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSA_5_relativeShift->setValue(shift_5);
            else m_irfPDSA_5_relativeShift->setValue(0.0f);


            /* PDS-B */
            enabled_1 = pdsB_irf_1.getValueAt(m_irfPDSB_1_enabled, &ok).toBool();
            if ( ok ) m_irfPDSB_1_enabled->setValue(enabled_1);
            else m_irfPDSB_1_enabled->setValue(false);

            ft_1 = pdsB_irf_1.getValueAt(m_irfPDSB_1_functionType, &ok).toString();
            if ( ok ) m_irfPDSB_1_functionType->setValue(ft_1);
            else m_irfPDSB_1_functionType->setValue("GAUSSIAN");

            intensity_1 = pdsB_irf_1.getValueAt(m_irfPDSB_1_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_1_intensity->setValue(intensity_1);
            else m_irfPDSB_1_intensity->setValue(1.0);

            uncertainty_1 = pdsB_irf_1.getValueAt(m_irfPDSB_1_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_1_uncertainty->setValue(uncertainty_1);
            else m_irfPDSB_1_uncertainty->setValue(0.0849329f);

            shift_1 = pdsB_irf_1.getValueAt(m_irfPDSB_1_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_1_relativeShift->setValue(shift_1);
            else m_irfPDSB_1_relativeShift->setValue(0.0f);

            enabled_2 = pdsB_irf_2.getValueAt(m_irfPDSB_2_enabled, &ok).toBool();
            if ( ok ) m_irfPDSB_2_enabled->setValue(enabled_2);
            else m_irfPDSB_2_enabled->setValue(false);

            ft_2 = pdsB_irf_2.getValueAt(m_irfPDSB_2_functionType, &ok).toString();
            if ( ok ) m_irfPDSB_2_functionType->setValue(ft_2);
            else m_irfPDSB_2_functionType->setValue("GAUSSIAN");

            intensity_2 = pdsB_irf_2.getValueAt(m_irfPDSB_2_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_2_intensity->setValue(intensity_2);
            else m_irfPDSB_2_intensity->setValue(1.0);

            uncertainty_2 = pdsB_irf_2.getValueAt(m_irfPDSB_2_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_2_uncertainty->setValue(uncertainty_2);
            else m_irfPDSB_2_uncertainty->setValue(0.0849329f);

            shift_2 = pdsB_irf_2.getValueAt(m_irfPDSB_2_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_2_relativeShift->setValue(shift_2);
            else m_irfPDSB_2_relativeShift->setValue(0.0f);

            enabled_3 = pdsB_irf_3.getValueAt(m_irfPDSB_3_enabled, &ok).toBool();
            if ( ok ) m_irfPDSB_3_enabled->setValue(enabled_3);
            else m_irfPDSB_3_enabled->setValue(false);

            ft_3 = pdsB_irf_3.getValueAt(m_irfPDSB_3_functionType, &ok).toString();
            if ( ok ) m_irfPDSB_3_functionType->setValue(ft_3);
            else m_irfPDSB_3_functionType->setValue("GAUSSIAN");

            intensity_3 = pdsB_irf_3.getValueAt(m_irfPDSB_3_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_3_intensity->setValue(intensity_3);
            else m_irfPDSB_3_intensity->setValue(1.0);

            uncertainty_3 = pdsB_irf_3.getValueAt(m_irfPDSB_3_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_3_uncertainty->setValue(uncertainty_3);
            else m_irfPDSB_3_uncertainty->setValue(0.0849329f);

            shift_3 = pdsB_irf_3.getValueAt(m_irfPDSB_3_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_3_relativeShift->setValue(shift_3);
            else m_irfPDSB_3_relativeShift->setValue(0.0f);

            enabled_4 = pdsB_irf_4.getValueAt(m_irfPDSB_4_enabled, &ok).toBool();
            if ( ok ) m_irfPDSB_4_enabled->setValue(enabled_4);
            else m_irfPDSB_4_enabled->setValue(false);

            ft_4 = pdsB_irf_4.getValueAt(m_irfPDSB_4_functionType, &ok).toString();
            if ( ok ) m_irfPDSB_4_functionType->setValue(ft_4);
            else m_irfPDSB_4_functionType->setValue("GAUSSIAN");

            intensity_4 = pdsB_irf_4.getValueAt(m_irfPDSB_4_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_4_intensity->setValue(intensity_4);
            else m_irfPDSB_4_intensity->setValue(1.0);

            uncertainty_4 = pdsB_irf_4.getValueAt(m_irfPDSB_4_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_4_uncertainty->setValue(uncertainty_4);
            else m_irfPDSB_4_uncertainty->setValue(0.0849329f);

            shift_4 = pdsB_irf_4.getValueAt(m_irfPDSB_4_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_4_relativeShift->setValue(shift_4);
            else m_irfPDSB_4_relativeShift->setValue(0.0f);

            enabled_5 = pdsB_irf_5.getValueAt(m_irfPDSB_5_enabled, &ok).toBool();
            if ( ok ) m_irfPDSB_5_enabled->setValue(enabled_5);
            else m_irfPDSB_5_enabled->setValue(false);

            ft_5 = pdsB_irf_5.getValueAt(m_irfPDSB_5_functionType, &ok).toString();
            if ( ok ) m_irfPDSB_5_functionType->setValue(ft_5);
            else m_irfPDSB_5_functionType->setValue("GAUSSIAN");

            intensity_5 = pdsB_irf_5.getValueAt(m_irfPDSB_5_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_5_intensity->setValue(intensity_5);
            else m_irfPDSB_5_intensity->setValue(1.0);

            uncertainty_5 = pdsB_irf_5.getValueAt(m_irfPDSB_5_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_5_uncertainty->setValue(uncertainty_5);
            else m_irfPDSB_5_uncertainty->setValue(0.0849329f);

            shift_5 = pdsB_irf_5.getValueAt(m_irfPDSB_5_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfPDSB_5_relativeShift->setValue(shift_5);
            else m_irfPDSB_5_relativeShift->setValue(0.0f);


            /* MU */
            enabled_1 = mu_irf_1.getValueAt(m_irfMU_1_enabled, &ok).toBool();
            if ( ok ) m_irfMU_1_enabled->setValue(enabled_1);
            else m_irfMU_1_enabled->setValue(false);

            ft_1 = mu_irf_1.getValueAt(m_irfMU_1_functionType, &ok).toString();
            if ( ok ) m_irfMU_1_functionType->setValue(ft_1);
            else m_irfMU_1_functionType->setValue("GAUSSIAN");

            intensity_1 = mu_irf_1.getValueAt(m_irfMU_1_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_1_intensity->setValue(intensity_1);
            else m_irfMU_1_intensity->setValue(1.0);

            uncertainty_1 = mu_irf_1.getValueAt(m_irfMU_1_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_1_uncertainty->setValue(uncertainty_1);
            else m_irfMU_1_uncertainty->setValue(0.0025f);

            shift_1 = mu_irf_1.getValueAt(m_irfMU_1_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_1_relativeShift->setValue(shift_1);
            else m_irfMU_1_relativeShift->setValue(0.0f);

            enabled_2 = mu_irf_2.getValueAt(m_irfMU_2_enabled, &ok).toBool();
            if ( ok ) m_irfMU_2_enabled->setValue(enabled_2);
            else m_irfMU_2_enabled->setValue(false);

            ft_2 = mu_irf_2.getValueAt(m_irfMU_2_functionType, &ok).toString();
            if ( ok ) m_irfMU_2_functionType->setValue(ft_2);
            else m_irfMU_2_functionType->setValue("GAUSSIAN");

            intensity_2 = mu_irf_2.getValueAt(m_irfMU_2_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_2_intensity->setValue(intensity_2);
            else m_irfMU_2_intensity->setValue(1.0);

            uncertainty_2 = mu_irf_2.getValueAt(m_irfMU_2_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_2_uncertainty->setValue(uncertainty_2);
            else m_irfMU_2_uncertainty->setValue(0.0025f);

            shift_2 = mu_irf_2.getValueAt(m_irfMU_2_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_2_relativeShift->setValue(shift_2);
            else m_irfMU_2_relativeShift->setValue(0.0f);

            enabled_3 = mu_irf_3.getValueAt(m_irfMU_3_enabled, &ok).toBool();
            if ( ok ) m_irfMU_3_enabled->setValue(enabled_3);
            else m_irfMU_3_enabled->setValue(false);

            ft_3 = mu_irf_3.getValueAt(m_irfMU_3_functionType, &ok).toString();
            if ( ok ) m_irfMU_3_functionType->setValue(ft_3);
            else m_irfMU_3_functionType->setValue("GAUSSIAN");

            intensity_3 = mu_irf_3.getValueAt(m_irfMU_3_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_3_intensity->setValue(intensity_3);
            else m_irfMU_3_intensity->setValue(1.0);

            uncertainty_3 = mu_irf_3.getValueAt(m_irfMU_3_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_3_uncertainty->setValue(uncertainty_3);
            else m_irfMU_3_uncertainty->setValue(0.0025f);

            shift_3 = mu_irf_3.getValueAt(m_irfMU_3_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_3_relativeShift->setValue(shift_3);
            else m_irfMU_3_relativeShift->setValue(0.0f);

            enabled_4 = mu_irf_4.getValueAt(m_irfMU_4_enabled, &ok).toBool();
            if ( ok ) m_irfMU_4_enabled->setValue(enabled_4);
            else m_irfMU_4_enabled->setValue(false);

            ft_4 = mu_irf_4.getValueAt(m_irfMU_4_functionType, &ok).toString();
            if ( ok ) m_irfMU_4_functionType->setValue(ft_4);
            else m_irfMU_4_functionType->setValue("GAUSSIAN");

            intensity_4 = mu_irf_4.getValueAt(m_irfMU_4_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_4_intensity->setValue(intensity_4);
            else m_irfMU_4_intensity->setValue(1.0);

            uncertainty_4 = mu_irf_4.getValueAt(m_irfMU_4_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_4_uncertainty->setValue(uncertainty_4);
            else m_irfMU_4_uncertainty->setValue(0.0025f);

            shift_4 = mu_irf_4.getValueAt(m_irfMU_4_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_4_relativeShift->setValue(shift_4);
            else m_irfMU_4_relativeShift->setValue(0.0f);

            enabled_5 = mu_irf_5.getValueAt(m_irfMU_5_enabled, &ok).toBool();
            if ( ok ) m_irfMU_5_enabled->setValue(enabled_5);
            else m_irfMU_5_enabled->setValue(false);

            ft_5 = mu_irf_5.getValueAt(m_irfMU_5_functionType, &ok).toString();
            if ( ok ) m_irfMU_5_functionType->setValue(ft_5);
            else m_irfMU_5_functionType->setValue("GAUSSIAN");

            intensity_5 = mu_irf_5.getValueAt(m_irfMU_5_intensity, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_5_intensity->setValue(intensity_5);
            else m_irfMU_5_intensity->setValue(1.0);

            uncertainty_5 = mu_irf_5.getValueAt(m_irfMU_5_uncertainty, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_5_uncertainty->setValue(uncertainty_5);
            else m_irfMU_5_uncertainty->setValue(0.0025f);

            shift_5 = mu_irf_5.getValueAt(m_irfMU_5_relativeShift, &ok).toFloat(&ok);
            if ( ok ) m_irfMU_5_relativeShift->setValue(shift_5);
            else m_irfMU_5_relativeShift->setValue(0.0f);
        }
    }
    /* -----> /Electronic Settings <-----*/

    /* -----> Pulse-Height-Spectrum Settings <-----*/
    const DSimpleXMLTag phsParentTag = parentTag.getTag(m_phsParentNode, &ok);

    if ( !ok )
    {
        m_meanPHS_A_511Node->setValue(90.0f);
        m_meanPHS_B_511Node->setValue(90.0f);
        m_meanPHS_A_1274Node->setValue(190.0f);
        m_meanPHS_B_1274Node->setValue(190.0f);

        m_sigmaPHS_A_511Node->setValue(25.0f);
        m_sigmaPHS_B_511Node->setValue(25.0f);
        m_sigmaPHS_A_1274Node->setValue(150.0f);
        m_sigmaPHS_B_1274Node->setValue(150.0f);
    }
    else
    {
        const double  sigma1274keV_A = phsParentTag.getValueAt(m_sigmaPHS_A_1274Node, &ok).toDouble(&ok);
        if ( ok )
            m_sigmaPHS_A_1274Node->setValue(sigma1274keV_A);
        else
            m_sigmaPHS_A_1274Node->setValue(150.0f);

        const double  sigma1274keV_B = phsParentTag.getValueAt(m_sigmaPHS_B_1274Node, &ok).toDouble(&ok);
        if ( ok )
            m_sigmaPHS_B_1274Node->setValue(sigma1274keV_B);
        else
            m_sigmaPHS_B_1274Node->setValue(150.0f);

        const double  sigma511keV_A = phsParentTag.getValueAt(m_sigmaPHS_A_511Node, &ok).toDouble(&ok);
        if ( ok )
            m_sigmaPHS_A_511Node->setValue(sigma511keV_A);
        else
            m_sigmaPHS_A_511Node->setValue(25.0f);

        const double  sigma511keV_B = phsParentTag.getValueAt(m_sigmaPHS_B_511Node, &ok).toDouble(&ok);
        if ( ok )
            m_sigmaPHS_B_511Node->setValue(sigma511keV_B);
        else
            m_sigmaPHS_B_511Node->setValue(25.0f);

        const double  mean511keV_A = phsParentTag.getValueAt(m_meanPHS_A_511Node, &ok).toDouble(&ok);
        if ( ok )
            m_meanPHS_A_511Node->setValue(mean511keV_A);
        else
            m_meanPHS_A_511Node->setValue(90.0f);

        const double  mean511keV_B = phsParentTag.getValueAt(m_meanPHS_B_511Node, &ok).toDouble(&ok);
        if ( ok )
            m_meanPHS_B_511Node->setValue(mean511keV_B);
        else
            m_meanPHS_B_511Node->setValue(90.0f);

        const double  mean1274keV_A = phsParentTag.getValueAt(m_meanPHS_A_1274Node, &ok).toDouble(&ok);
        if ( ok )
            m_meanPHS_A_1274Node->setValue(mean1274keV_A);
        else
            m_meanPHS_A_1274Node->setValue(190.0f);

        const double  mean1274keV_B = phsParentTag.getValueAt(m_meanPHS_B_1274Node, &ok).toDouble(&ok);
        if ( ok )
            m_meanPHS_B_1274Node->setValue(mean1274keV_B);
        else
            m_meanPHS_B_1274Node->setValue(190.0f);
    }
    /* -----> /Pulse-Height-Spectrum Settings <-----*/

    /* -----> Pulse-Shape Settings <-----*/
    const DSimpleXMLTag pulseShapeParentTag = parentTag.getTag(m_pulseShapeParentNode, &ok);

    if ( !ok ) {
        m_pulseShapeDigitization_enabled_Node->setValue(false);
        m_pulseShapeDigitization_depth_in_bit_Node->setValue(14);


        m_pulseShapeRiseTimeInNS_A_Node->setValue(5.0f);
        m_pulseShapeWidthInNS_A_Node->setValue(0.45);

        m_pulseShapeBaselineJitter_enabled_A_Node->setValue(false);
        m_pulseShapeBaselineJitter_mean_A_Node->setValue(0.0);
        m_pulseShapeBaselineJitter_stddev_A_Node->setValue(0.0);

        m_pulseShapeRandomNoise_enabled_A_Node->setValue(false);
        m_pulseShapeRandomNoise_stddev_A_Node->setValue(0.0);

        m_pulseShapeNonLinearity_enabled_A_Node->setValue(false);
        m_pulseShapeNonLinearity_fixedApertureJitter_A_Node->setValue(0.0);
        m_pulseShapeNonLinearity_randomApertureJitter_A_Node->setValue(0.0);


        m_pulseShapeRiseTimeInNS_B_Node->setValue(5.0f);
        m_pulseShapeWidthInNS_B_Node->setValue(0.45);

        m_pulseShapeBaselineJitter_enabled_B_Node->setValue(false);
        m_pulseShapeBaselineJitter_mean_B_Node->setValue(0.0);
        m_pulseShapeBaselineJitter_stddev_B_Node->setValue(0.0);

        m_pulseShapeRandomNoise_enabled_B_Node->setValue(false);
        m_pulseShapeRandomNoise_stddev_B_Node->setValue(0.0);

        m_pulseShapeNonLinearity_enabled_B_Node->setValue(false);
        m_pulseShapeNonLinearity_fixedApertureJitter_B_Node->setValue(0.0);
        m_pulseShapeNonLinearity_randomApertureJitter_B_Node->setValue(0.0);
    }
    else {
        const DSimpleXMLTag digiParentTag = pulseShapeParentTag.getTag(m_pulseShapeDigitizationParentNode, &ok);

        if (!ok) {
            m_pulseShapeDigitization_enabled_Node->setValue(false);
            m_pulseShapeDigitization_depth_in_bit_Node->setValue(14);
        }
        else {
            const bool  digiEnabled = digiParentTag.getValueAt(m_pulseShapeDigitization_enabled_Node, &ok).toBool();
            if ( ok )
                m_pulseShapeDigitization_enabled_Node->setValue(digiEnabled);
            else
                m_pulseShapeDigitization_enabled_Node->setValue(false);

            const int  digiDepth = digiParentTag.getValueAt(m_pulseShapeDigitization_depth_in_bit_Node, &ok).toInt(&ok);
            if ( ok )
                m_pulseShapeDigitization_depth_in_bit_Node->setValue(digiDepth);
            else
                m_pulseShapeDigitization_depth_in_bit_Node->setValue(14);
        }


        // pulse A

        const DSimpleXMLTag pulseAParentTag = pulseShapeParentTag.getTag(m_pulseShape_A_ParentNode, &ok);

        if (!ok) {
            m_pulseShapeRiseTimeInNS_A_Node->setValue(5.0f);
            m_pulseShapeWidthInNS_A_Node->setValue(0.45);

            m_pulseShapeBaselineJitter_enabled_A_Node->setValue(false);
            m_pulseShapeBaselineJitter_mean_A_Node->setValue(0.0);
            m_pulseShapeBaselineJitter_stddev_A_Node->setValue(0.0);

            m_pulseShapeRandomNoise_enabled_A_Node->setValue(false);
            m_pulseShapeRandomNoise_stddev_A_Node->setValue(0.0);

            m_pulseShapeNonLinearity_enabled_A_Node->setValue(false);
            m_pulseShapeNonLinearity_fixedApertureJitter_A_Node->setValue(0.0);
            m_pulseShapeNonLinearity_randomApertureJitter_A_Node->setValue(0.0);
        }
        else {
            const double  riseTimeInNs = pulseAParentTag.getValueAt(m_pulseShapeRiseTimeInNS_A_Node, &ok).toDouble(&ok);
            if ( ok )
                m_pulseShapeRiseTimeInNS_A_Node->setValue(riseTimeInNs);
            else
                m_pulseShapeRiseTimeInNS_A_Node->setValue(5.0f);

            const double  pulseWidthInNs = pulseAParentTag.getValueAt(m_pulseShapeWidthInNS_A_Node, &ok).toDouble(&ok);
            if ( ok )
                m_pulseShapeWidthInNS_A_Node->setValue(pulseWidthInNs);
            else
                m_pulseShapeWidthInNS_A_Node->setValue(0.45f);

            const DSimpleXMLTag baselineJitterTag = pulseAParentTag.getTag(m_pulseShapeBaselineJitter_A_ParentNode, &ok);

            if (!ok) {
                m_pulseShapeBaselineJitter_enabled_A_Node->setValue(false);
                m_pulseShapeBaselineJitter_mean_A_Node->setValue(0.0);
                m_pulseShapeBaselineJitter_stddev_A_Node->setValue(0.0);
            }
            else {
                const bool  jitterEnabled = baselineJitterTag.getValueAt(m_pulseShapeBaselineJitter_enabled_A_Node, &ok).toBool();
                if ( ok )
                    m_pulseShapeBaselineJitter_enabled_A_Node->setValue(jitterEnabled);
                else
                    m_pulseShapeBaselineJitter_enabled_A_Node->setValue(false);

                const double  mean = baselineJitterTag.getValueAt(m_pulseShapeBaselineJitter_mean_A_Node, &ok).toDouble(&ok);
                if ( ok )
                    m_pulseShapeBaselineJitter_mean_A_Node->setValue(mean);
                else
                    m_pulseShapeBaselineJitter_mean_A_Node->setValue(0.0);

                const double  stddev = baselineJitterTag.getValueAt(m_pulseShapeBaselineJitter_stddev_A_Node, &ok).toDouble(&ok);
                if ( ok )
                    m_pulseShapeBaselineJitter_stddev_A_Node->setValue(stddev);
                else
                    m_pulseShapeBaselineJitter_stddev_A_Node->setValue(0.0);
            }

            const DSimpleXMLTag randomNoiseTag = pulseAParentTag.getTag(m_pulseShapeRandomNoise_A_ParentNode, &ok);

            if (!ok) {
                m_pulseShapeRandomNoise_enabled_A_Node->setValue(false);
                m_pulseShapeRandomNoise_stddev_A_Node->setValue(0.0);
            }
            else {
                const bool  randomNoiseEnabled = randomNoiseTag.getValueAt(m_pulseShapeRandomNoise_enabled_A_Node, &ok).toBool();
                if ( ok )
                    m_pulseShapeRandomNoise_enabled_A_Node->setValue(randomNoiseEnabled);
                else
                    m_pulseShapeRandomNoise_enabled_A_Node->setValue(false);

                const double  stddev = randomNoiseTag.getValueAt(m_pulseShapeRandomNoise_stddev_A_Node, &ok).toDouble(&ok);
                if ( ok )
                    m_pulseShapeRandomNoise_stddev_A_Node->setValue(stddev);
                else
                    m_pulseShapeRandomNoise_stddev_A_Node->setValue(0.0);
            }

            const DSimpleXMLTag nonlinearityTag = pulseAParentTag.getTag(m_pulseShapeTimeNonlinearity_A_ParentNode, &ok);

            if (!ok) {
                m_pulseShapeNonLinearity_enabled_A_Node->setValue(false);
                m_pulseShapeNonLinearity_fixedApertureJitter_A_Node->setValue(0.0);
                m_pulseShapeNonLinearity_randomApertureJitter_A_Node->setValue(0.0);
            }
            else {
                const bool  nonLinearityEnabled = nonlinearityTag.getValueAt(m_pulseShapeNonLinearity_enabled_A_Node, &ok).toBool();
                if ( ok )
                    m_pulseShapeNonLinearity_enabled_A_Node->setValue(nonLinearityEnabled);
                else
                    m_pulseShapeNonLinearity_enabled_A_Node->setValue(false);

                const double  fAstddev = nonlinearityTag.getValueAt(m_pulseShapeNonLinearity_fixedApertureJitter_A_Node, &ok).toDouble(&ok);
                if ( ok )
                    m_pulseShapeNonLinearity_fixedApertureJitter_A_Node->setValue(fAstddev);
                else
                    m_pulseShapeNonLinearity_fixedApertureJitter_A_Node->setValue(0.0);

                const double  rndAstddev = nonlinearityTag.getValueAt(m_pulseShapeNonLinearity_randomApertureJitter_A_Node, &ok).toDouble(&ok);
                if ( ok )
                    m_pulseShapeNonLinearity_randomApertureJitter_A_Node->setValue(rndAstddev);
                else
                    m_pulseShapeNonLinearity_randomApertureJitter_A_Node->setValue(0.0);
            }
        }
        // pulse B

        const DSimpleXMLTag pulseBParentTag = pulseShapeParentTag.getTag(m_pulseShape_B_ParentNode, &ok);

        if (!ok) {
            m_pulseShapeRiseTimeInNS_B_Node->setValue(5.0f);
            m_pulseShapeWidthInNS_B_Node->setValue(0.45);

            m_pulseShapeBaselineJitter_enabled_B_Node->setValue(false);
            m_pulseShapeBaselineJitter_mean_B_Node->setValue(0.0);
            m_pulseShapeBaselineJitter_stddev_B_Node->setValue(0.0);

            m_pulseShapeRandomNoise_enabled_B_Node->setValue(false);
            m_pulseShapeRandomNoise_stddev_B_Node->setValue(0.0);

            m_pulseShapeNonLinearity_enabled_B_Node->setValue(false);
            m_pulseShapeNonLinearity_fixedApertureJitter_B_Node->setValue(0.0);
            m_pulseShapeNonLinearity_randomApertureJitter_B_Node->setValue(0.0);
        }
        else {
            const double  riseTimeInNs = pulseBParentTag.getValueAt(m_pulseShapeRiseTimeInNS_B_Node, &ok).toDouble(&ok);
            if ( ok )
                m_pulseShapeRiseTimeInNS_B_Node->setValue(riseTimeInNs);
            else
                m_pulseShapeRiseTimeInNS_B_Node->setValue(5.0f);

            const double  pulseWidthInNs = pulseBParentTag.getValueAt(m_pulseShapeWidthInNS_B_Node, &ok).toDouble(&ok);
            if ( ok )
                m_pulseShapeWidthInNS_B_Node->setValue(pulseWidthInNs);
            else
                m_pulseShapeWidthInNS_B_Node->setValue(0.45f);

            const DSimpleXMLTag baselineJitterTag = pulseBParentTag.getTag(m_pulseShapeBaselineJitter_B_ParentNode, &ok);

            if (!ok) {
                m_pulseShapeBaselineJitter_enabled_B_Node->setValue(false);
                m_pulseShapeBaselineJitter_mean_B_Node->setValue(0.0);
                m_pulseShapeBaselineJitter_stddev_B_Node->setValue(0.0);
            }
            else {
                const bool  jitterEnabled = baselineJitterTag.getValueAt(m_pulseShapeBaselineJitter_enabled_B_Node, &ok).toBool();
                if ( ok )
                    m_pulseShapeBaselineJitter_enabled_B_Node->setValue(jitterEnabled);
                else
                    m_pulseShapeBaselineJitter_enabled_B_Node->setValue(false);

                const double  mean = baselineJitterTag.getValueAt(m_pulseShapeBaselineJitter_mean_B_Node, &ok).toDouble(&ok);
                if ( ok )
                    m_pulseShapeBaselineJitter_mean_B_Node->setValue(mean);
                else
                    m_pulseShapeBaselineJitter_mean_B_Node->setValue(0.0);

                const double  stddev = baselineJitterTag.getValueAt(m_pulseShapeBaselineJitter_stddev_B_Node, &ok).toDouble(&ok);
                if ( ok )
                    m_pulseShapeBaselineJitter_stddev_B_Node->setValue(stddev);
                else
                    m_pulseShapeBaselineJitter_stddev_B_Node->setValue(0.0);
            }

            const DSimpleXMLTag randomNoiseTag = pulseBParentTag.getTag(m_pulseShapeRandomNoise_B_ParentNode, &ok);

            if (!ok) {
                m_pulseShapeRandomNoise_enabled_B_Node->setValue(false);
                m_pulseShapeRandomNoise_stddev_B_Node->setValue(0.0);
            }
            else {
                const bool  randomNoiseEnabled = randomNoiseTag.getValueAt(m_pulseShapeRandomNoise_enabled_B_Node, &ok).toBool();
                if ( ok )
                    m_pulseShapeRandomNoise_enabled_B_Node->setValue(randomNoiseEnabled);
                else
                    m_pulseShapeRandomNoise_enabled_B_Node->setValue(false);

                const double  stddev = randomNoiseTag.getValueAt(m_pulseShapeRandomNoise_stddev_B_Node, &ok).toDouble(&ok);
                if ( ok )
                    m_pulseShapeRandomNoise_stddev_B_Node->setValue(stddev);
                else
                    m_pulseShapeRandomNoise_stddev_B_Node->setValue(0.0);
            }

            const DSimpleXMLTag nonlinearityTag = pulseBParentTag.getTag(m_pulseShapeTimeNonlinearity_B_ParentNode, &ok);

            if (!ok) {
                m_pulseShapeNonLinearity_enabled_B_Node->setValue(false);
                m_pulseShapeNonLinearity_fixedApertureJitter_B_Node->setValue(0.0);
                m_pulseShapeNonLinearity_randomApertureJitter_B_Node->setValue(0.0);
            }
            else {
                const bool  nonLinearityEnabled = nonlinearityTag.getValueAt(m_pulseShapeNonLinearity_enabled_B_Node, &ok).toBool();
                if ( ok )
                    m_pulseShapeNonLinearity_enabled_B_Node->setValue(nonLinearityEnabled);
                else
                    m_pulseShapeNonLinearity_enabled_B_Node->setValue(false);

                const double  fAstddev = nonlinearityTag.getValueAt(m_pulseShapeNonLinearity_fixedApertureJitter_B_Node, &ok).toDouble(&ok);
                if ( ok )
                    m_pulseShapeNonLinearity_fixedApertureJitter_B_Node->setValue(fAstddev);
                else
                    m_pulseShapeNonLinearity_fixedApertureJitter_B_Node->setValue(0.0);

                const double  rndAstddev = nonlinearityTag.getValueAt(m_pulseShapeNonLinearity_randomApertureJitter_B_Node, &ok).toDouble(&ok);
                if ( ok )
                    m_pulseShapeNonLinearity_randomApertureJitter_B_Node->setValue(rndAstddev);
                else
                    m_pulseShapeNonLinearity_randomApertureJitter_B_Node->setValue(0.0);
            }


        }
    }
     /* -----> /Pulse-Shape Settings <-----*/

    const DSimpleXMLTag ltParentTag = parentTag.getTag(m_ltParentNode, &ok);

    /* -----> Lifetime Settings <-----*/
    if ( !ok )
    {
        m_backgroundI->setValue(0.1f);
        m_coincidenceI->setValue(0.2f);

        m_enableLT1->setValue(true);
        m_enableLT2->setValue(true);
        m_enableLT3->setValue(true);
        m_enableLT4->setValue(true);
        m_enableLT5->setValue(true);

        m_tau1_NS->setValue(0.160f);
        m_tau2_NS->setValue(0.355f);
        m_tau3_NS->setValue(0.420f);
        m_tau4_NS->setValue(1.2f);
        m_tau5_NS->setValue(3.6f);

        m_I1_NS->setValue(0.2f);
        m_I2_NS->setValue(0.1f);
        m_I3_NS->setValue(0.2f);
        m_I4_NS->setValue(0.3f);
        m_I5_NS->setValue(0.2f);

        m_tau1_isDistributionEnabled->setValue(false);
        m_tau1_distrFunctionType->setValue(QString("GAUSSIAN"));
        m_tau1_distrUncertainty_NS->setValue(0.0f);
        m_tau1_distrGridNumber->setValue(0);
        m_tau1_distrGridIncrement->setValue(0.0f);

        m_tau2_isDistributionEnabled->setValue(false);
        m_tau2_distrFunctionType->setValue(QString("GAUSSIAN"));
        m_tau2_distrUncertainty_NS->setValue(0.0f);
        m_tau2_distrGridNumber->setValue(0);
        m_tau2_distrGridIncrement->setValue(0.0f);

        m_tau3_isDistributionEnabled->setValue(false);
        m_tau3_distrFunctionType->setValue(QString("GAUSSIAN"));
        m_tau3_distrUncertainty_NS->setValue(0.0f);
        m_tau3_distrGridNumber->setValue(0);
        m_tau3_distrGridIncrement->setValue(0.0f);

        m_tau4_isDistributionEnabled->setValue(false);
        m_tau4_distrFunctionType->setValue(QString("GAUSSIAN"));
        m_tau4_distrUncertainty_NS->setValue(0.0f);
        m_tau4_distrGridNumber->setValue(0);
        m_tau4_distrGridIncrement->setValue(0.0f);

        m_tau5_isDistributionEnabled->setValue(false);
        m_tau5_distrFunctionType->setValue(QString("GAUSSIAN"));
        m_tau5_distrUncertainty_NS->setValue(0.0f);
        m_tau5_distrGridNumber->setValue(0);
        m_tau5_distrGridIncrement->setValue(0.0f);
    }
    else
    {
        const DSimpleXMLTag lt1DistrTag = ltParentTag.getTag(m_ltDistr1ParentNode, &ok);

        bool distrEnabled = false;
        QString functionType = "GAUSSIAN";
        float distrUncertainty = 0.0f;
        int gridNumber = 0;
        float gridIncrement = 0.0f;

        if ( ok )  {
            distrEnabled = lt1DistrTag.getValueAt(m_tau1_isDistributionEnabled, &ok).toBool();
            if ( !ok ) distrEnabled = false;

            functionType = lt1DistrTag.getValueAt(m_tau1_distrFunctionType, &ok).toString();
            if ( !ok ) functionType = "GAUSSIAN";

            distrUncertainty = lt1DistrTag.getValueAt(m_tau1_distrUncertainty_NS, &ok).toFloat(&ok);
            if ( !ok ) distrUncertainty = 0.0f;

            gridNumber = lt1DistrTag.getValueAt(m_tau1_distrGridNumber, &ok).toInt(&ok);
            if ( !ok ) gridNumber = 0;

            gridIncrement = lt1DistrTag.getValueAt(m_tau1_distrGridIncrement, &ok).toFloat(&ok);
            if ( !ok ) gridIncrement = 0.0f;
        }

        m_tau1_isDistributionEnabled->setValue(distrEnabled);
        m_tau1_distrFunctionType->setValue(functionType);
        m_tau1_distrUncertainty_NS->setValue(distrUncertainty);
        m_tau1_distrGridNumber->setValue(gridNumber);
        m_tau1_distrGridIncrement->setValue(gridIncrement);

        const DSimpleXMLTag lt2DistrTag = ltParentTag.getTag(m_ltDistr2ParentNode, &ok);

        distrEnabled = false;
        functionType = "GAUSSIAN";
        distrUncertainty = 0.0f;
        gridNumber = 0;
        gridIncrement = 0.0f;

        if ( ok )  {
            distrEnabled = lt2DistrTag.getValueAt(m_tau2_isDistributionEnabled, &ok).toBool();
            if ( !ok ) distrEnabled = false;

            functionType = lt2DistrTag.getValueAt(m_tau2_distrFunctionType, &ok).toString();
            if ( !ok ) functionType = "GAUSSIAN";

            distrUncertainty = lt2DistrTag.getValueAt(m_tau2_distrUncertainty_NS, &ok).toFloat(&ok);
            if ( !ok ) distrUncertainty = 0.0f;

            gridNumber = lt2DistrTag.getValueAt(m_tau2_distrGridNumber, &ok).toInt(&ok);
            if ( !ok ) gridNumber = 0;

            gridIncrement = lt2DistrTag.getValueAt(m_tau2_distrGridIncrement, &ok).toFloat(&ok);
            if ( !ok ) gridIncrement = 0.0f;
        }

        m_tau2_isDistributionEnabled->setValue(distrEnabled);
        m_tau2_distrFunctionType->setValue(functionType);
        m_tau2_distrUncertainty_NS->setValue(distrUncertainty);
        m_tau2_distrGridNumber->setValue(gridNumber);
        m_tau2_distrGridIncrement->setValue(gridIncrement);

        const DSimpleXMLTag lt3DistrTag = ltParentTag.getTag(m_ltDistr3ParentNode, &ok);

        distrEnabled = false;
        functionType = "GAUSSIAN";
        distrUncertainty = 0.0f;
        gridNumber = 0;
        gridIncrement = 0.0f;

        if ( ok )  {
            distrEnabled = lt3DistrTag.getValueAt(m_tau3_isDistributionEnabled, &ok).toBool();
            if ( !ok ) distrEnabled = false;

            functionType = lt3DistrTag.getValueAt(m_tau3_distrFunctionType, &ok).toString();
            if ( !ok ) functionType = "GAUSSIAN";

            distrUncertainty = lt3DistrTag.getValueAt(m_tau3_distrUncertainty_NS, &ok).toFloat(&ok);
            if ( !ok ) distrUncertainty = 0.0f;

            gridNumber = lt3DistrTag.getValueAt(m_tau3_distrGridNumber, &ok).toInt(&ok);
            if ( !ok ) gridNumber = 0;

            gridIncrement = lt3DistrTag.getValueAt(m_tau3_distrGridIncrement, &ok).toFloat(&ok);
            if ( !ok ) gridIncrement = 0.0f;
        }

        m_tau3_isDistributionEnabled->setValue(distrEnabled);
        m_tau3_distrFunctionType->setValue(functionType);
        m_tau3_distrUncertainty_NS->setValue(distrUncertainty);
        m_tau3_distrGridNumber->setValue(gridNumber);
        m_tau3_distrGridIncrement->setValue(gridIncrement);

        const DSimpleXMLTag lt4DistrTag = ltParentTag.getTag(m_ltDistr4ParentNode, &ok);

        distrEnabled = false;
        functionType = "GAUSSIAN";
        distrUncertainty = 0.0f;
        gridNumber = 0;
        gridIncrement = 0.0f;

        if ( ok )  {
            distrEnabled = lt4DistrTag.getValueAt(m_tau4_isDistributionEnabled, &ok).toBool();
            if ( !ok ) distrEnabled = false;

            functionType = lt4DistrTag.getValueAt(m_tau4_distrFunctionType, &ok).toString();
            if ( !ok ) functionType = "GAUSSIAN";

            distrUncertainty = lt4DistrTag.getValueAt(m_tau4_distrUncertainty_NS, &ok).toFloat(&ok);
            if ( !ok ) distrUncertainty = 0.0f;

            gridNumber = lt4DistrTag.getValueAt(m_tau4_distrGridNumber, &ok).toInt(&ok);
            if ( !ok ) gridNumber = 0;

            gridIncrement = lt4DistrTag.getValueAt(m_tau4_distrGridIncrement, &ok).toFloat(&ok);
            if ( !ok ) gridIncrement = 0.0f;
        }

        m_tau4_isDistributionEnabled->setValue(distrEnabled);
        m_tau4_distrFunctionType->setValue(functionType);
        m_tau4_distrUncertainty_NS->setValue(distrUncertainty);
        m_tau4_distrGridNumber->setValue(gridNumber);
        m_tau4_distrGridIncrement->setValue(gridIncrement);

        const DSimpleXMLTag lt5DistrTag = ltParentTag.getTag(m_ltDistr5ParentNode, &ok);

        distrEnabled = false;
        functionType = "GAUSSIAN";
        distrUncertainty = 0.0f;
        gridNumber = 0;
        gridIncrement = 0.0f;

        if ( ok )  {
            distrEnabled = lt5DistrTag.getValueAt(m_tau5_isDistributionEnabled, &ok).toBool();
            if ( !ok ) distrEnabled = false;

            functionType = lt5DistrTag.getValueAt(m_tau5_distrFunctionType, &ok).toString();
            if ( !ok ) functionType = "GAUSSIAN";

            distrUncertainty = lt5DistrTag.getValueAt(m_tau5_distrUncertainty_NS, &ok).toFloat(&ok);
            if ( !ok ) distrUncertainty = 0.0f;

            gridNumber = lt5DistrTag.getValueAt(m_tau5_distrGridNumber, &ok).toInt(&ok);
            if ( !ok ) gridNumber = 0;

            gridIncrement = lt5DistrTag.getValueAt(m_tau5_distrGridIncrement, &ok).toFloat(&ok);
            if ( !ok ) gridIncrement = 0.0f;
        }

        m_tau5_isDistributionEnabled->setValue(distrEnabled);
        m_tau5_distrFunctionType->setValue(functionType);
        m_tau5_distrUncertainty_NS->setValue(distrUncertainty);
        m_tau5_distrGridNumber->setValue(gridNumber);
        m_tau5_distrGridIncrement->setValue(gridIncrement);

        const double  bgI = ltParentTag.getValueAt(m_backgroundI, &ok).toDouble(&ok);
        if ( ok )
            m_backgroundI->setValue(bgI);
        else
            m_backgroundI->setValue(0.1f);

        const double  coinc = ltParentTag.getValueAt(m_coincidenceI, &ok).toDouble(&ok);
        if ( ok )
            m_coincidenceI->setValue(coinc);
        else
            m_coincidenceI->setValue(0.2f);

        const double  enabled1 = ltParentTag.getValueAt(m_enableLT1, &ok).toBool();
        if ( ok )
            m_enableLT1->setValue(enabled1);
        else
            m_enableLT1->setValue(true);

        const double  enabled2 = ltParentTag.getValueAt(m_enableLT2, &ok).toBool();
        if ( ok )
            m_enableLT2->setValue(enabled2);
        else
            m_enableLT2->setValue(true);

        const double  enabled3 = ltParentTag.getValueAt(m_enableLT3, &ok).toBool();
        if ( ok )
            m_enableLT3->setValue(enabled3);
        else
            m_enableLT3->setValue(true);

        const double  enabled4 = ltParentTag.getValueAt(m_enableLT4, &ok).toBool();
        if ( ok )
            m_enableLT4->setValue(enabled4);
        else
            m_enableLT4->setValue(true);

        const double  enabled5 = ltParentTag.getValueAt(m_enableLT5, &ok).toBool();
        if ( ok )
            m_enableLT5->setValue(enabled5);
        else
            m_enableLT5->setValue(true);

        const double  I1 = ltParentTag.getValueAt(m_I1_NS, &ok).toDouble(&ok);
        if ( ok )
            m_I1_NS->setValue(I1);
        else
            m_I1_NS->setValue(0.2f);

        const double  I2 = ltParentTag.getValueAt(m_I2_NS, &ok).toDouble(&ok);
        if ( ok )
            m_I2_NS->setValue(I2);
        else
            m_I2_NS->setValue(0.1f);

        const double  I3 = ltParentTag.getValueAt(m_I3_NS, &ok).toDouble(&ok);
        if ( ok )
            m_I3_NS->setValue(I3);
        else
            m_I3_NS->setValue(0.2f);

        const double  I4 = ltParentTag.getValueAt(m_I4_NS, &ok).toDouble(&ok);
        if ( ok )
            m_I4_NS->setValue(I4);
        else
            m_I4_NS->setValue(0.3f);

        const double  I5 = ltParentTag.getValueAt(m_I5_NS, &ok).toDouble(&ok);
        if ( ok )
            m_I5_NS->setValue(I5);
        else
            m_I5_NS->setValue(0.2f);

        const double  lt1InNs = ltParentTag.getValueAt(m_tau1_NS, &ok).toDouble(&ok);
        if ( ok )
            m_tau1_NS->setValue(lt1InNs);
        else
            m_tau1_NS->setValue(0.160f);

        const double  lt2InNs = ltParentTag.getValueAt(m_tau2_NS, &ok).toDouble(&ok);
        if ( ok )
            m_tau2_NS->setValue(lt2InNs);
        else
            m_tau2_NS->setValue(0.355f);

        const double  lt3InNs = ltParentTag.getValueAt(m_tau3_NS, &ok).toDouble(&ok);
        if ( ok )
            m_tau3_NS->setValue(lt3InNs);
        else
            m_tau3_NS->setValue(0.420f);

        const double  lt4InNs = ltParentTag.getValueAt(m_tau4_NS, &ok).toDouble(&ok);
        if ( ok )
            m_tau4_NS->setValue(lt4InNs);
        else
            m_tau4_NS->setValue(1.2f);

        const double  lt5InNs = ltParentTag.getValueAt(m_tau5_NS, &ok).toDouble(&ok);
        if ( ok )
            m_tau5_NS->setValue(lt5InNs);
        else
            m_tau5_NS->setValue(3.6f);
    }
    /* -----> /Lifetime Settings <-----*/


    m_fileName = path;

    return true;
}

bool DRS4SimulationSettingsManager::save(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    DSimpleXMLWriter writer(path);

    const bool ok = writer.writeToFile(m_parentNode, false);

    if (ok)
        m_fileName = path;

    return ok;
}

bool DRS4SimulationSettingsManager::detectorA_irf_1_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_1_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::detectorA_irf_1_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_1_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::detectorA_irf_1_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_1_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorA_irf_1_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_1_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorA_irf_1_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_1_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::detectorA_irf_2_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_2_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::detectorA_irf_2_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_2_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::detectorA_irf_2_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_2_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorA_irf_2_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_2_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorA_irf_2_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_2_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::detectorA_irf_3_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_3_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::detectorA_irf_3_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_3_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::detectorA_irf_3_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_3_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorA_irf_3_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_3_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorA_irf_3_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_3_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::detectorA_irf_4_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_4_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::detectorA_irf_4_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_4_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::detectorA_irf_4_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_4_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorA_irf_4_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_4_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorA_irf_4_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_4_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::detectorA_irf_5_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_5_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::detectorA_irf_5_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_5_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::detectorA_irf_5_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_5_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorA_irf_5_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_5_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorA_irf_5_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSA_5_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::detectorB_irf_1_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_1_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::detectorB_irf_1_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_1_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::detectorB_irf_1_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_1_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorB_irf_1_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_1_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorB_irf_1_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_1_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::detectorB_irf_2_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_2_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::detectorB_irf_2_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_2_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::detectorB_irf_2_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_2_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorB_irf_2_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_2_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorB_irf_2_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_2_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::detectorB_irf_3_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_3_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::detectorB_irf_3_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_3_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::detectorB_irf_3_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_3_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorB_irf_3_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_3_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorB_irf_3_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_3_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::detectorB_irf_4_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_4_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::detectorB_irf_4_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_4_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::detectorB_irf_4_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_4_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorB_irf_4_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_4_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorB_irf_4_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_4_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::detectorB_irf_5_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_5_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::detectorB_irf_5_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_5_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::detectorB_irf_5_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_5_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorB_irf_5_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_5_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::detectorB_irf_5_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfPDSB_5_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::mu_irf_1_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_1_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::mu_irf_1_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_1_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::mu_irf_1_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_1_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::mu_irf_1_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_1_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::mu_irf_1_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_1_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::mu_irf_2_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_2_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::mu_irf_2_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_2_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::mu_irf_2_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_2_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::mu_irf_2_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_2_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::mu_irf_2_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_2_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::mu_irf_3_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_3_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::mu_irf_3_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_3_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::mu_irf_3_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_3_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::mu_irf_3_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_3_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::mu_irf_3_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_3_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::mu_irf_4_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_4_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::mu_irf_4_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_4_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::mu_irf_4_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_4_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::mu_irf_4_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_4_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::mu_irf_4_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_4_relativeShift->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::mu_irf_5_enabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_5_enabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::mu_irf_5_functionType() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_5_functionType->getValue().toString();
}

float DRS4SimulationSettingsManager::mu_irf_5_intensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_5_intensity->getValue().toFloat();
}

float DRS4SimulationSettingsManager::mu_irf_5_uncertainty() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_5_uncertainty->getValue().toFloat();
}

float DRS4SimulationSettingsManager::mu_irf_5_shift() const
{
    QMutexLocker locker(&m_mutex);

    return m_irfMU_5_relativeShift->getValue().toFloat();
}

float DRS4SimulationSettingsManager::sigmaPHS511keV_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_sigmaPHS_A_511Node->getValue().toFloat();
}

float DRS4SimulationSettingsManager::meanPHS511keV_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_meanPHS_A_511Node->getValue().toFloat();
}

float DRS4SimulationSettingsManager::sigmaPHS511keV_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_sigmaPHS_B_511Node->getValue().toFloat();
}

float DRS4SimulationSettingsManager::meanPHS511keV_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_meanPHS_B_511Node->getValue().toFloat();
}

float DRS4SimulationSettingsManager::sigmaPHS1274keV_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_sigmaPHS_A_1274Node->getValue().toFloat();
}

float DRS4SimulationSettingsManager::meanPHS1274keV_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_meanPHS_A_1274Node->getValue().toFloat();
}

float DRS4SimulationSettingsManager::sigmaPHS1274keV_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_sigmaPHS_B_1274Node->getValue().toFloat();
}

float DRS4SimulationSettingsManager::meanPHS1274keV_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_meanPHS_B_1274Node->getValue().toFloat();
}

float DRS4SimulationSettingsManager::arrivalTimeSpreadInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_arrivalTimeSpreadInNsNode->getValue().toFloat();
}

unsigned int DRS4SimulationSettingsManager::digitizationDepthInBit() const
{
    QMutexLocker locker(&m_mutex);

    bool ok = false;
    const unsigned int value = m_pulseShapeDigitization_depth_in_bit_Node->getValue().toInt(&ok);

    if (!ok)
        return 14;

    return value;
}

bool DRS4SimulationSettingsManager::isDigitizationEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeDigitization_enabled_Node->getValue().toBool();
}

float DRS4SimulationSettingsManager::riseTimeInNs_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeRiseTimeInNS_A_Node->getValue().toFloat();
}

float DRS4SimulationSettingsManager::pulseWidthInNs_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeWidthInNS_A_Node->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::isBaselineOffsetJitterEnabled_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeBaselineJitter_enabled_A_Node->getValue().toBool();
}

double DRS4SimulationSettingsManager::baselineOffsetJitterMean_in_mV_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeBaselineJitter_mean_A_Node->getValue().toDouble();
}

double DRS4SimulationSettingsManager::baselineOffsetJitterStddev_in_mV_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeBaselineJitter_stddev_A_Node->getValue().toDouble();
}

bool DRS4SimulationSettingsManager::isRandomNoiseEnabled_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeRandomNoise_enabled_A_Node->getValue().toBool();
}

double DRS4SimulationSettingsManager::randomNoise_in_mV_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeRandomNoise_stddev_A_Node->getValue().toDouble();
}

bool DRS4SimulationSettingsManager::isTimeAxisNonlinearityEnabled_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeNonLinearity_enabled_A_Node->getValue().toBool();
}

double DRS4SimulationSettingsManager::nonlinearityFixedApertureJitter_in_ns_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeNonLinearity_fixedApertureJitter_A_Node->getValue().toDouble();
}

double DRS4SimulationSettingsManager::nonlinearityRandomApertureJitter_in_ns_A() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeNonLinearity_randomApertureJitter_A_Node->getValue().toDouble();
}

float DRS4SimulationSettingsManager::riseTimeInNs_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeRiseTimeInNS_B_Node->getValue().toFloat();
}

float DRS4SimulationSettingsManager::pulseWidthInNs_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeWidthInNS_B_Node->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::isBaselineOffsetJitterEnabled_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeBaselineJitter_enabled_B_Node->getValue().toBool();
}

double DRS4SimulationSettingsManager::baselineOffsetJitterMean_in_mV_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeBaselineJitter_mean_B_Node->getValue().toDouble();
}

double DRS4SimulationSettingsManager::baselineOffsetJitterStddev_in_mV_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeBaselineJitter_stddev_B_Node->getValue().toDouble();
}

bool DRS4SimulationSettingsManager::isRandomNoiseEnabled_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeRandomNoise_enabled_B_Node->getValue().toBool();
}

double DRS4SimulationSettingsManager::randomNoise_in_mV_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeRandomNoise_stddev_B_Node->getValue().toDouble();
}

bool DRS4SimulationSettingsManager::isTimeAxisNonlinearityEnabled_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeNonLinearity_enabled_B_Node->getValue().toBool();
}

double DRS4SimulationSettingsManager::nonlinearityFixedApertureJitter_in_ns_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeNonLinearity_fixedApertureJitter_B_Node->getValue().toDouble();
}

double DRS4SimulationSettingsManager::nonlinearityRandomApertureJitter_in_ns_B() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeNonLinearity_randomApertureJitter_B_Node->getValue().toDouble();
}

float DRS4SimulationSettingsManager::backgroundIntensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_backgroundI->getValue().toFloat();
}

float DRS4SimulationSettingsManager::coincidenceIntensity() const
{
    QMutexLocker locker(&m_mutex);

    return m_coincidenceI->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::isLT1activated() const
{
    QMutexLocker locker(&m_mutex);

    return m_enableLT1->getValue().toBool();
}

bool DRS4SimulationSettingsManager::isLT2activated() const
{
    QMutexLocker locker(&m_mutex);

    return m_enableLT2->getValue().toBool();
}

bool DRS4SimulationSettingsManager::isLT3activated() const
{
    QMutexLocker locker(&m_mutex);

    return m_enableLT3->getValue().toBool();
}

bool DRS4SimulationSettingsManager::isLT4activated() const
{
    QMutexLocker locker(&m_mutex);

    return m_enableLT4->getValue().toBool();
}

bool DRS4SimulationSettingsManager::isLT5activated() const
{
    QMutexLocker locker(&m_mutex);

    return m_enableLT5->getValue().toBool();
}

float DRS4SimulationSettingsManager::tau1InNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau1_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::tau2InNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau2_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::tau3InNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau3_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::tau4InNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau4_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::tau5InNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau5_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::intensityLT1() const
{
    QMutexLocker locker(&m_mutex);

    return m_I1_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::intensityLT2() const
{
    QMutexLocker locker(&m_mutex);

    return m_I2_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::intensityLT3() const
{
    QMutexLocker locker(&m_mutex);

    return m_I3_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::intensityLT4() const
{
    QMutexLocker locker(&m_mutex);

    return m_I4_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::intensityLT5() const
{
    QMutexLocker locker(&m_mutex);

    return m_I5_NS->getValue().toFloat();
}

bool DRS4SimulationSettingsManager::isLT1DistributionEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau1_isDistributionEnabled->getValue().toBool();
}

bool DRS4SimulationSettingsManager::isLT2DistributionEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau2_isDistributionEnabled->getValue().toBool();
}

bool DRS4SimulationSettingsManager::isLT3DistributionEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau3_isDistributionEnabled->getValue().toBool();
}

bool DRS4SimulationSettingsManager::isLT4DistributionEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau4_isDistributionEnabled->getValue().toBool();
}

bool DRS4SimulationSettingsManager::isLT5DistributionEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau5_isDistributionEnabled->getValue().toBool();
}

QString DRS4SimulationSettingsManager::functionTypeLT1Distribution() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau1_distrFunctionType->getValue().toString();
}

QString DRS4SimulationSettingsManager::functionTypeLT2Distribution() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau2_distrFunctionType->getValue().toString();
}

QString DRS4SimulationSettingsManager::functionTypeLT3Distribution() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau3_distrFunctionType->getValue().toString();
}

QString DRS4SimulationSettingsManager::functionTypeLT4Distribution() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau4_distrFunctionType->getValue().toString();
}

QString DRS4SimulationSettingsManager::functionTypeLT5Distribution() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau5_distrFunctionType->getValue().toString();
}

float DRS4SimulationSettingsManager::uncertaintyLT1DistributionInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau1_distrUncertainty_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::uncertaintyLT2DistributionInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau2_distrUncertainty_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::uncertaintyLT3DistributionInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau3_distrUncertainty_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::uncertaintyLT4DistributionInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau4_distrUncertainty_NS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::uncertaintyLT5DistributionInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau5_distrUncertainty_NS->getValue().toFloat();
}

int DRS4SimulationSettingsManager::gridLT1DistributionCount() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau1_distrGridNumber->getValue().toInt();
}

int DRS4SimulationSettingsManager::gridLT2DistributionCount() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau2_distrGridNumber->getValue().toInt();
}

int DRS4SimulationSettingsManager::gridLT3DistributionCount() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau3_distrGridNumber->getValue().toInt();
}

int DRS4SimulationSettingsManager::gridLT4DistributionCount() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau4_distrGridNumber->getValue().toInt();
}

int DRS4SimulationSettingsManager::gridLT5DistributionCount() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau5_distrGridNumber->getValue().toInt();
}

float DRS4SimulationSettingsManager::gridLT1DistributionIncrementInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau1_distrGridIncrement->getValue().toFloat();
}

float DRS4SimulationSettingsManager::gridLT2DistributionIncrementInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau2_distrGridIncrement->getValue().toFloat();
}

float DRS4SimulationSettingsManager::gridLT3DistributionIncrementInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau3_distrGridIncrement->getValue().toFloat();
}

float DRS4SimulationSettingsManager::gridLT4DistributionIncrementInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau4_distrGridIncrement->getValue().toFloat();
}

float DRS4SimulationSettingsManager::gridLT5DistributionIncrementInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tau5_distrGridIncrement->getValue().toFloat();
}
