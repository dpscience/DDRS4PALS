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

#ifndef DDRS4SIMULATIONSETTINGSMANAGER_H
#define DDRS4SIMULATIONSETTINGSMANAGER_H

#include "DLib.h"

#include <QMutex>
#include <QMutexLocker>

class DRS4SimulationSettingsManager
{
    QString m_fileName;

    DSimpleXMLNode *m_parentNode;

    DSimpleXMLNode *m_electronicsNode;

    /* irf-A */
    DSimpleXMLNode *m_irfPDSA_parentNode;
            DSimpleXMLNode *m_irfPDSA_1_parentNode;

                    DSimpleXMLNode *m_irfPDSA_1_enabled;
                    DSimpleXMLNode *m_irfPDSA_1_functionType;
                    DSimpleXMLNode *m_irfPDSA_1_intensity;
                    DSimpleXMLNode *m_irfPDSA_1_uncertainty;
                    DSimpleXMLNode *m_irfPDSA_1_relativeShift;

            DSimpleXMLNode *m_irfPDSA_2_parentNode;

                    DSimpleXMLNode *m_irfPDSA_2_enabled;
                    DSimpleXMLNode *m_irfPDSA_2_functionType;
                    DSimpleXMLNode *m_irfPDSA_2_intensity;
                    DSimpleXMLNode *m_irfPDSA_2_uncertainty;
                    DSimpleXMLNode *m_irfPDSA_2_relativeShift;

            DSimpleXMLNode *m_irfPDSA_3_parentNode;

                    DSimpleXMLNode *m_irfPDSA_3_enabled;
                    DSimpleXMLNode *m_irfPDSA_3_functionType;
                    DSimpleXMLNode *m_irfPDSA_3_intensity;
                    DSimpleXMLNode *m_irfPDSA_3_uncertainty;
                    DSimpleXMLNode *m_irfPDSA_3_relativeShift;

            DSimpleXMLNode *m_irfPDSA_4_parentNode;

                    DSimpleXMLNode *m_irfPDSA_4_enabled;
                    DSimpleXMLNode *m_irfPDSA_4_functionType;
                    DSimpleXMLNode *m_irfPDSA_4_intensity;
                    DSimpleXMLNode *m_irfPDSA_4_uncertainty;
                    DSimpleXMLNode *m_irfPDSA_4_relativeShift;

            DSimpleXMLNode *m_irfPDSA_5_parentNode;

                    DSimpleXMLNode *m_irfPDSA_5_enabled;
                    DSimpleXMLNode *m_irfPDSA_5_functionType;
                    DSimpleXMLNode *m_irfPDSA_5_intensity;
                    DSimpleXMLNode *m_irfPDSA_5_uncertainty;
                    DSimpleXMLNode *m_irfPDSA_5_relativeShift;

     /* irf-B */
     DSimpleXMLNode *m_irfPDSB_parentNode;
            DSimpleXMLNode *m_irfPDSB_1_parentNode;

                    DSimpleXMLNode *m_irfPDSB_1_enabled;
                    DSimpleXMLNode *m_irfPDSB_1_functionType;
                    DSimpleXMLNode *m_irfPDSB_1_intensity;
                    DSimpleXMLNode *m_irfPDSB_1_uncertainty;
                    DSimpleXMLNode *m_irfPDSB_1_relativeShift;

            DSimpleXMLNode *m_irfPDSB_2_parentNode;

                    DSimpleXMLNode *m_irfPDSB_2_enabled;
                    DSimpleXMLNode *m_irfPDSB_2_functionType;
                    DSimpleXMLNode *m_irfPDSB_2_intensity;
                    DSimpleXMLNode *m_irfPDSB_2_uncertainty;
                    DSimpleXMLNode *m_irfPDSB_2_relativeShift;

            DSimpleXMLNode *m_irfPDSB_3_parentNode;

                    DSimpleXMLNode *m_irfPDSB_3_enabled;
                    DSimpleXMLNode *m_irfPDSB_3_functionType;
                    DSimpleXMLNode *m_irfPDSB_3_intensity;
                    DSimpleXMLNode *m_irfPDSB_3_uncertainty;
                    DSimpleXMLNode *m_irfPDSB_3_relativeShift;

            DSimpleXMLNode *m_irfPDSB_4_parentNode;

                    DSimpleXMLNode *m_irfPDSB_4_enabled;
                    DSimpleXMLNode *m_irfPDSB_4_functionType;
                    DSimpleXMLNode *m_irfPDSB_4_intensity;
                    DSimpleXMLNode *m_irfPDSB_4_uncertainty;
                    DSimpleXMLNode *m_irfPDSB_4_relativeShift;

            DSimpleXMLNode *m_irfPDSB_5_parentNode;

                    DSimpleXMLNode *m_irfPDSB_5_enabled;
                    DSimpleXMLNode *m_irfPDSB_5_functionType;
                    DSimpleXMLNode *m_irfPDSB_5_intensity;
                    DSimpleXMLNode *m_irfPDSB_5_uncertainty;
                    DSimpleXMLNode *m_irfPDSB_5_relativeShift;

      /* irf-MU */
      DSimpleXMLNode *m_irfMU_parentNode;

             DSimpleXMLNode *m_irfMU_1_parentNode;

                     DSimpleXMLNode *m_irfMU_1_enabled;
                     DSimpleXMLNode *m_irfMU_1_functionType;
                     DSimpleXMLNode *m_irfMU_1_intensity;
                     DSimpleXMLNode *m_irfMU_1_uncertainty;
                     DSimpleXMLNode *m_irfMU_1_relativeShift;

             DSimpleXMLNode *m_irfMU_2_parentNode;

                     DSimpleXMLNode *m_irfMU_2_enabled;
                     DSimpleXMLNode *m_irfMU_2_functionType;
                     DSimpleXMLNode *m_irfMU_2_intensity;
                     DSimpleXMLNode *m_irfMU_2_uncertainty;
                     DSimpleXMLNode *m_irfMU_2_relativeShift;

             DSimpleXMLNode *m_irfMU_3_parentNode;

                     DSimpleXMLNode *m_irfMU_3_enabled;
                     DSimpleXMLNode *m_irfMU_3_functionType;
                     DSimpleXMLNode *m_irfMU_3_intensity;
                     DSimpleXMLNode *m_irfMU_3_uncertainty;
                     DSimpleXMLNode *m_irfMU_3_relativeShift;

              DSimpleXMLNode *m_irfMU_4_parentNode;

                     DSimpleXMLNode *m_irfMU_4_enabled;
                     DSimpleXMLNode *m_irfMU_4_functionType;
                     DSimpleXMLNode *m_irfMU_4_intensity;
                     DSimpleXMLNode *m_irfMU_4_uncertainty;
                     DSimpleXMLNode *m_irfMU_4_relativeShift;

              DSimpleXMLNode *m_irfMU_5_parentNode;

                     DSimpleXMLNode *m_irfMU_5_enabled;
                     DSimpleXMLNode *m_irfMU_5_functionType;
                     DSimpleXMLNode *m_irfMU_5_intensity;
                     DSimpleXMLNode *m_irfMU_5_uncertainty;
                     DSimpleXMLNode *m_irfMU_5_relativeShift;

    /* pulse-shape */
    DSimpleXMLNode *m_pulseShapeParentNode;

        /* digitization */
        DSimpleXMLNode *m_pulseShapeDigitizationParentNode;

            DSimpleXMLNode *m_pulseShapeDigitization_enabled_Node;
            DSimpleXMLNode *m_pulseShapeDigitization_depth_in_bit_Node;

        /* pulse A */
        DSimpleXMLNode *m_pulseShape_A_ParentNode;

            DSimpleXMLNode *m_pulseShapeRiseTimeInNS_A_Node;
            DSimpleXMLNode *m_pulseShapeWidthInNS_A_Node;

                DSimpleXMLNode *m_pulseShapeBaselineJitter_A_ParentNode;

                    DSimpleXMLNode *m_pulseShapeBaselineJitter_enabled_A_Node;
                    DSimpleXMLNode *m_pulseShapeBaselineJitter_mean_A_Node;
                    DSimpleXMLNode *m_pulseShapeBaselineJitter_stddev_A_Node;

                DSimpleXMLNode *m_pulseShapeRandomNoise_A_ParentNode;

                    DSimpleXMLNode *m_pulseShapeRandomNoise_enabled_A_Node;
                    DSimpleXMLNode *m_pulseShapeRandomNoise_stddev_A_Node;

                DSimpleXMLNode *m_pulseShapeTimeNonlinearity_A_ParentNode;

                    DSimpleXMLNode *m_pulseShapeNonLinearity_enabled_A_Node;
                    DSimpleXMLNode *m_pulseShapeNonLinearity_fixedApertureJitter_A_Node;
                    DSimpleXMLNode *m_pulseShapeNonLinearity_randomApertureJitter_A_Node;


          /* pulse B */
          DSimpleXMLNode *m_pulseShape_B_ParentNode;

             DSimpleXMLNode *m_pulseShapeRiseTimeInNS_B_Node;
             DSimpleXMLNode *m_pulseShapeWidthInNS_B_Node;

                DSimpleXMLNode *m_pulseShapeBaselineJitter_B_ParentNode;

                     DSimpleXMLNode *m_pulseShapeBaselineJitter_enabled_B_Node;
                     DSimpleXMLNode *m_pulseShapeBaselineJitter_mean_B_Node;
                     DSimpleXMLNode *m_pulseShapeBaselineJitter_stddev_B_Node;

                 DSimpleXMLNode *m_pulseShapeRandomNoise_B_ParentNode;

                     DSimpleXMLNode *m_pulseShapeRandomNoise_enabled_B_Node;
                     DSimpleXMLNode *m_pulseShapeRandomNoise_stddev_B_Node;

                 DSimpleXMLNode *m_pulseShapeTimeNonlinearity_B_ParentNode;

                     DSimpleXMLNode *m_pulseShapeNonLinearity_enabled_B_Node;
                     DSimpleXMLNode *m_pulseShapeNonLinearity_fixedApertureJitter_B_Node;
                     DSimpleXMLNode *m_pulseShapeNonLinearity_randomApertureJitter_B_Node;

    /* phs */
    DSimpleXMLNode *m_phsParentNode;

        DSimpleXMLNode *m_sigmaPHS_A_511Node;
        DSimpleXMLNode *m_sigmaPHS_B_511Node;

        DSimpleXMLNode *m_sigmaPHS_A_1274Node;
        DSimpleXMLNode *m_sigmaPHS_B_1274Node;

        DSimpleXMLNode *m_meanPHS_A_511Node;
        DSimpleXMLNode *m_meanPHS_B_511Node;

        DSimpleXMLNode *m_meanPHS_A_1274Node;
        DSimpleXMLNode *m_meanPHS_B_1274Node;

    DSimpleXMLNode *m_arrivalTimeSpreadInNsNode;

    /* lifetimes (distributions) */
    DSimpleXMLNode *m_ltParentNode;

        DSimpleXMLNode *m_ltDistr1ParentNode;
        DSimpleXMLNode *m_ltDistr2ParentNode;
        DSimpleXMLNode *m_ltDistr3ParentNode;
        DSimpleXMLNode *m_ltDistr4ParentNode;
        DSimpleXMLNode *m_ltDistr5ParentNode;

    DSimpleXMLNode *m_backgroundI;
    DSimpleXMLNode *m_coincidenceI;

    DSimpleXMLNode *m_enableLT1;
    DSimpleXMLNode *m_enableLT2;
    DSimpleXMLNode *m_enableLT3;
    DSimpleXMLNode *m_enableLT4;
    DSimpleXMLNode *m_enableLT5;

    DSimpleXMLNode *m_tau1_NS;
    DSimpleXMLNode *m_tau2_NS;
    DSimpleXMLNode *m_tau3_NS;
    DSimpleXMLNode *m_tau4_NS;
    DSimpleXMLNode *m_tau5_NS;

    DSimpleXMLNode *m_I1_NS;
    DSimpleXMLNode *m_I2_NS;
    DSimpleXMLNode *m_I3_NS;
    DSimpleXMLNode *m_I4_NS;
    DSimpleXMLNode *m_I5_NS;

        DSimpleXMLNode *m_tau1_isDistributionEnabled;
        DSimpleXMLNode *m_tau2_isDistributionEnabled;
        DSimpleXMLNode *m_tau3_isDistributionEnabled;
        DSimpleXMLNode *m_tau4_isDistributionEnabled;
        DSimpleXMLNode *m_tau5_isDistributionEnabled;

        DSimpleXMLNode *m_tau1_distrUncertainty_NS;
        DSimpleXMLNode *m_tau2_distrUncertainty_NS;
        DSimpleXMLNode *m_tau3_distrUncertainty_NS;
        DSimpleXMLNode *m_tau4_distrUncertainty_NS;
        DSimpleXMLNode *m_tau5_distrUncertainty_NS;

        DSimpleXMLNode *m_tau1_distrFunctionType;
        DSimpleXMLNode *m_tau2_distrFunctionType;
        DSimpleXMLNode *m_tau3_distrFunctionType;
        DSimpleXMLNode *m_tau4_distrFunctionType;
        DSimpleXMLNode *m_tau5_distrFunctionType;

        DSimpleXMLNode *m_tau1_distrGridNumber;
        DSimpleXMLNode *m_tau2_distrGridNumber;
        DSimpleXMLNode *m_tau3_distrGridNumber;
        DSimpleXMLNode *m_tau4_distrGridNumber;
        DSimpleXMLNode *m_tau5_distrGridNumber;

        DSimpleXMLNode *m_tau1_distrGridIncrement;
        DSimpleXMLNode *m_tau2_distrGridIncrement;
        DSimpleXMLNode *m_tau3_distrGridIncrement;
        DSimpleXMLNode *m_tau4_distrGridIncrement;
        DSimpleXMLNode *m_tau5_distrGridIncrement;

    mutable QMutex m_mutex;

public:
    static DRS4SimulationSettingsManager *sharedInstance();

    DSimpleXMLNode *parentNode() const;

    QString fileName() const;

    QString xmlContent() const;

    bool load(const QString& path);
    bool save(const QString& path);


    bool detectorA_irf_1_enabled() const;
    QString detectorA_irf_1_functionType() const;
    float detectorA_irf_1_intensity() const;
    float detectorA_irf_1_uncertainty() const;
    float detectorA_irf_1_shift() const;

    bool detectorA_irf_2_enabled() const;
    QString detectorA_irf_2_functionType() const;
    float detectorA_irf_2_intensity() const;
    float detectorA_irf_2_uncertainty() const;
    float detectorA_irf_2_shift() const;

    bool detectorA_irf_3_enabled() const;
    QString detectorA_irf_3_functionType() const;
    float detectorA_irf_3_intensity() const;
    float detectorA_irf_3_uncertainty() const;
    float detectorA_irf_3_shift() const;

    bool detectorA_irf_4_enabled() const;
    QString detectorA_irf_4_functionType() const;
    float detectorA_irf_4_intensity() const;
    float detectorA_irf_4_uncertainty() const;
    float detectorA_irf_4_shift() const;

    bool detectorA_irf_5_enabled() const;
    QString detectorA_irf_5_functionType() const;
    float detectorA_irf_5_intensity() const;
    float detectorA_irf_5_uncertainty() const;
    float detectorA_irf_5_shift() const;


    bool detectorB_irf_1_enabled() const;
    QString detectorB_irf_1_functionType() const;
    float detectorB_irf_1_intensity() const;
    float detectorB_irf_1_uncertainty() const;
    float detectorB_irf_1_shift() const;

    bool detectorB_irf_2_enabled() const;
    QString detectorB_irf_2_functionType() const;
    float detectorB_irf_2_intensity() const;
    float detectorB_irf_2_uncertainty() const;
    float detectorB_irf_2_shift() const;

    bool detectorB_irf_3_enabled() const;
    QString detectorB_irf_3_functionType() const;
    float detectorB_irf_3_intensity() const;
    float detectorB_irf_3_uncertainty() const;
    float detectorB_irf_3_shift() const;

    bool detectorB_irf_4_enabled() const;
    QString detectorB_irf_4_functionType() const;
    float detectorB_irf_4_intensity() const;
    float detectorB_irf_4_uncertainty() const;
    float detectorB_irf_4_shift() const;

    bool detectorB_irf_5_enabled() const;
    QString detectorB_irf_5_functionType() const;
    float detectorB_irf_5_intensity() const;
    float detectorB_irf_5_uncertainty() const;
    float detectorB_irf_5_shift() const;


    bool mu_irf_1_enabled() const;
    QString mu_irf_1_functionType() const;
    float mu_irf_1_intensity() const;
    float mu_irf_1_uncertainty() const;
    float mu_irf_1_shift() const;

    bool mu_irf_2_enabled() const;
    QString mu_irf_2_functionType() const;
    float mu_irf_2_intensity() const;
    float mu_irf_2_uncertainty() const;
    float mu_irf_2_shift() const;

    bool mu_irf_3_enabled() const;
    QString mu_irf_3_functionType() const;
    float mu_irf_3_intensity() const;
    float mu_irf_3_uncertainty() const;
    float mu_irf_3_shift() const;

    bool mu_irf_4_enabled() const;
    QString mu_irf_4_functionType() const;
    float mu_irf_4_intensity() const;
    float mu_irf_4_uncertainty() const;
    float mu_irf_4_shift() const;

    bool mu_irf_5_enabled() const;
    QString mu_irf_5_functionType() const;
    float mu_irf_5_intensity() const;
    float mu_irf_5_uncertainty() const;
    float mu_irf_5_shift() const;


    float sigmaPHS511keV_A() const;
    float meanPHS511keV_A() const;

    float sigmaPHS511keV_B() const;
    float meanPHS511keV_B() const;

    float sigmaPHS1274keV_A() const;
    float meanPHS1274keV_A() const;

    float sigmaPHS1274keV_B() const;
    float meanPHS1274keV_B() const;


    float arrivalTimeSpreadInNs() const;

    /* pulse */
    unsigned int digitizationDepthInBit() const;
    bool isDigitizationEnabled() const;

    /* pulse A */
    float riseTimeInNs_A() const;
    float pulseWidthInNs_A() const;

    bool isBaselineOffsetJitterEnabled_A() const;
    double baselineOffsetJitterMean_in_mV_A() const;
    double baselineOffsetJitterStddev_in_mV_A() const;

    bool isRandomNoiseEnabled_A() const;
    double randomNoise_in_mV_A() const;

    bool isTimeAxisNonlinearityEnabled_A() const;
    double nonlinearityFixedApertureJitter_in_ns_A() const;
    double nonlinearityRandomApertureJitter_in_ns_A() const;


    /* pulse B */
    float riseTimeInNs_B() const;
    float pulseWidthInNs_B() const;

    bool isBaselineOffsetJitterEnabled_B() const;
    double baselineOffsetJitterMean_in_mV_B() const;
    double baselineOffsetJitterStddev_in_mV_B() const;

    bool isRandomNoiseEnabled_B() const;
    double randomNoise_in_mV_B() const;

    bool isTimeAxisNonlinearityEnabled_B() const;
    double nonlinearityFixedApertureJitter_in_ns_B() const;
    double nonlinearityRandomApertureJitter_in_ns_B() const;


    float backgroundIntensity() const;
    float coincidenceIntensity() const;

    bool isLT1activated() const;
    bool isLT2activated() const;
    bool isLT3activated() const;
    bool isLT4activated() const;
    bool isLT5activated() const;

    float tau1InNs() const;
    float tau2InNs() const;
    float tau3InNs() const;
    float tau4InNs() const;
    float tau5InNs() const;

    float intensityLT1() const;
    float intensityLT2() const;
    float intensityLT3() const;
    float intensityLT4() const;
    float intensityLT5() const;

    bool isLT1DistributionEnabled() const;
    bool isLT2DistributionEnabled() const;
    bool isLT3DistributionEnabled() const;
    bool isLT4DistributionEnabled() const;
    bool isLT5DistributionEnabled() const;

    QString functionTypeLT1Distribution() const;
    QString functionTypeLT2Distribution() const;
    QString functionTypeLT3Distribution() const;
    QString functionTypeLT4Distribution() const;
    QString functionTypeLT5Distribution() const;

    float uncertaintyLT1DistributionInNs() const;
    float uncertaintyLT2DistributionInNs() const;
    float uncertaintyLT3DistributionInNs() const;
    float uncertaintyLT4DistributionInNs() const;
    float uncertaintyLT5DistributionInNs() const;

    int gridLT1DistributionCount() const;
    int gridLT2DistributionCount() const;
    int gridLT3DistributionCount() const;
    int gridLT4DistributionCount() const;
    int gridLT5DistributionCount() const;

    float gridLT1DistributionIncrementInNs() const;
    float gridLT2DistributionIncrementInNs() const;
    float gridLT3DistributionIncrementInNs() const;
    float gridLT4DistributionIncrementInNs() const;
    float gridLT5DistributionIncrementInNs() const;

private:
    DRS4SimulationSettingsManager();
    virtual ~DRS4SimulationSettingsManager();
};

#endif // DDRS4SIMULATIONSETTINGSMANAGER_H
