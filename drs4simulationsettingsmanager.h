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
    DSimpleXMLNode *m_phsParentNode;
    DSimpleXMLNode *m_pulseShapeParentNode;

    DSimpleXMLNode *m_ltParentNode;
    DSimpleXMLNode *m_ltDistr1ParentNode;
    DSimpleXMLNode *m_ltDistr2ParentNode;
    DSimpleXMLNode *m_ltDistr3ParentNode;
    DSimpleXMLNode *m_ltDistr4ParentNode;
    DSimpleXMLNode *m_ltDistr5ParentNode;

    //electronics:
    DSimpleXMLNode *m_tts_detectorAInNsNode;
    DSimpleXMLNode *m_tts_detectorBInNsNode;
    DSimpleXMLNode *m_timingResolution_boardInNsNode;

    //phs:
    DSimpleXMLNode *m_sigmaPHS_A_511Node;
    DSimpleXMLNode *m_sigmaPHS_B_511Node;

    DSimpleXMLNode *m_sigmaPHS_A_1274Node;
    DSimpleXMLNode *m_sigmaPHS_B_1274Node;

    DSimpleXMLNode *m_meanPHS_A_511Node;
    DSimpleXMLNode *m_meanPHS_B_511Node;

    DSimpleXMLNode *m_meanPHS_A_1274Node;
    DSimpleXMLNode *m_meanPHS_B_1274Node;

    DSimpleXMLNode *m_arrivalTimeSpreadInNsNode;

    //log-normal pulse-shape:
    DSimpleXMLNode *m_riseTimeInNS;
    DSimpleXMLNode *m_pulseWidthInNS;

    //lifetimes:
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

    float ttsDetectorAInNs() const;
    float ttsDetectorBInNs() const;
    float timingResolutionMeasurementUnitInNs() const;

    float sigmaPHS511keV_A() const;
    float meanPHS511keV_A() const;

    float sigmaPHS511keV_B() const;
    float meanPHS511keV_B() const;

    float sigmaPHS1274keV_A() const;
    float meanPHS1274keV_A() const;

    float sigmaPHS1274keV_B() const;
    float meanPHS1274keV_B() const;

    float arrivalTimeSpreadInNs() const;

    float riseTimeInNs() const;
    float pulseWidthInNs() const;

    void setRiseTimeInNs(float riseTime);
    void setPulseWidthInNs(float width);

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
