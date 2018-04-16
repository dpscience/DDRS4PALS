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
    m_phsParentNode = new DSimpleXMLNode("pulse-height-spectrum-PHS");
    m_pulseShapeParentNode = new DSimpleXMLNode("detector-output-pulse-shape");
    m_ltParentNode = new DSimpleXMLNode("lifetime-spectrum-simulation-input");
    m_ltDistr1ParentNode = new DSimpleXMLNode("lifetime-1-distribution-information");
    m_ltDistr2ParentNode = new DSimpleXMLNode("lifetime-2-distribution-information");
    m_ltDistr3ParentNode = new DSimpleXMLNode("lifetime-3-distribution-information");
    m_ltDistr4ParentNode = new DSimpleXMLNode("lifetime-4-distribution-information");
    m_ltDistr5ParentNode = new DSimpleXMLNode("lifetime-5-distribution-information");

    //PHS:
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

    //Pulse-Shape:
    m_riseTimeInNS = new DSimpleXMLNode("rise-time-in-ns");
    m_riseTimeInNS->setValue(5.0f);
    m_pulseWidthInNS = new DSimpleXMLNode("pulse-width-in-ns");
    m_pulseWidthInNS->setValue(0.165f);

    //LT:
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
    m_enableLT4->setValue(true);
    m_enableLT5 = new DSimpleXMLNode("lifetime-5-enabled?");
    m_enableLT5->setValue(true);

    m_tau1_NS = new DSimpleXMLNode("lifetime-1-tau-in-ns");
    m_tau1_NS->setValue(0.160f);
    m_tau2_NS = new DSimpleXMLNode("lifetime-2-tau-in-ns");
    m_tau2_NS->setValue(0.355f);
    m_tau3_NS = new DSimpleXMLNode("lifetime-3-tau-in-ns");
    m_tau3_NS->setValue(0.420f);
    m_tau4_NS = new DSimpleXMLNode("lifetime-4-tau-in-ns");
    m_tau4_NS->setValue(1.2f);
    m_tau5_NS = new DSimpleXMLNode("lifetime-5-tau-in-ns");
    m_tau5_NS->setValue(3.6f);

    m_I1_NS = new DSimpleXMLNode("lifetime-1-intensity");
    m_I1_NS->setValue(0.2f);
    m_I2_NS = new DSimpleXMLNode("lifetime-2-intensityy");
    m_I2_NS->setValue(0.1f);
    m_I3_NS = new DSimpleXMLNode("lifetime-3-intensity");
    m_I3_NS->setValue(0.2f);
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
    m_tts_detectorAInNsNode = new DSimpleXMLNode("uncertainty-PDS-detector-A-in-ns");
    m_tts_detectorAInNsNode->setValue(0.0849329f);
    m_tts_detectorBInNsNode = new DSimpleXMLNode("uncertainty-PDS-detector-B-in-ns");
    m_tts_detectorBInNsNode->setValue(0.0849329f);
    m_timingResolution_boardInNsNode = new DSimpleXMLNode("uncertainty-MU-in-ns");
    m_timingResolution_boardInNsNode->setValue(0.0025f);
    m_arrivalTimeSpreadInNsNode = new DSimpleXMLNode("arrival-time-spread-ATS-in-ns");
    m_arrivalTimeSpreadInNsNode->setValue(0.25f);


    //setup XML-table:
    (*m_phsParentNode) << m_meanPHS_A_511Node << m_meanPHS_B_511Node << m_meanPHS_A_1274Node << m_meanPHS_B_1274Node
                                    << m_sigmaPHS_A_511Node << m_sigmaPHS_B_511Node << m_sigmaPHS_A_1274Node << m_sigmaPHS_B_1274Node;

    (*m_pulseShapeParentNode) << m_riseTimeInNS << m_pulseWidthInNS;

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

    (*m_electronicsNode) << m_tts_detectorAInNsNode << m_tts_detectorBInNsNode << m_timingResolution_boardInNsNode << m_arrivalTimeSpreadInNsNode;


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
        m_tts_detectorAInNsNode->setValue(0.0849329f);
        m_tts_detectorBInNsNode->setValue(0.0849329f);
        m_timingResolution_boardInNsNode->setValue(0.0025f);
        m_arrivalTimeSpreadInNsNode->setValue(0.25f);
    }
    else
    {
        const double  tts_detectorAInNs = electronicTag.getValueAt(m_tts_detectorAInNsNode, &ok).toDouble(&ok);
        if ( ok )
            m_tts_detectorAInNsNode->setValue(tts_detectorAInNs);
        else
            m_tts_detectorAInNsNode->setValue(0.0849329f);

        const double  tts_detectorBInNs = electronicTag.getValueAt(m_tts_detectorBInNsNode, &ok).toDouble(&ok);
        if ( ok )
            m_tts_detectorBInNsNode->setValue(tts_detectorBInNs);
        else
            m_tts_detectorBInNsNode->setValue(0.0849329f);

        const double  timingResolution_boardInNsNode = electronicTag.getValueAt(m_timingResolution_boardInNsNode, &ok).toDouble(&ok);
        if ( ok )
            m_timingResolution_boardInNsNode->setValue(timingResolution_boardInNsNode);
        else
            m_timingResolution_boardInNsNode->setValue(0.0025f);


        const double  arrivalTimeSpreadInNs = electronicTag.getValueAt(m_arrivalTimeSpreadInNsNode, &ok).toDouble(&ok);
        if ( ok )
            m_arrivalTimeSpreadInNsNode->setValue(arrivalTimeSpreadInNs);
        else
            m_arrivalTimeSpreadInNsNode->setValue(0.25f);
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

    if ( !ok )
    {
        m_riseTimeInNS->setValue(5.0f);
        m_pulseWidthInNS->setValue(0.45f);
    }
    else
    {
        const double  riseTimeInNs = pulseShapeParentTag.getValueAt(m_riseTimeInNS, &ok).toDouble(&ok);
        if ( ok )
            m_riseTimeInNS->setValue(riseTimeInNs);
        else
            m_riseTimeInNS->setValue(5.0f);

        const double  pulseWidthInNs = pulseShapeParentTag.getValueAt(m_pulseWidthInNS, &ok).toDouble(&ok);
        if ( ok )
            m_pulseWidthInNS->setValue(pulseWidthInNs);
        else
            m_pulseWidthInNS->setValue(0.45f);
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

float DRS4SimulationSettingsManager::ttsDetectorAInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tts_detectorAInNsNode->getValue().toFloat();
}

float DRS4SimulationSettingsManager::ttsDetectorBInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_tts_detectorBInNsNode->getValue().toFloat();
}

float DRS4SimulationSettingsManager::timingResolutionMeasurementUnitInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_timingResolution_boardInNsNode->getValue().toFloat();
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

float DRS4SimulationSettingsManager::riseTimeInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_riseTimeInNS->getValue().toFloat();
}

float DRS4SimulationSettingsManager::pulseWidthInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseWidthInNS->getValue().toFloat();
}

void DRS4SimulationSettingsManager::setRiseTimeInNs(float riseTime)
{
    QMutexLocker locker(&m_mutex);

    m_riseTimeInNS->setValue(riseTime);
}

void DRS4SimulationSettingsManager::setPulseWidthInNs(float width)
{
    QMutexLocker locker(&m_mutex);

    m_pulseWidthInNS->setValue(width);
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
