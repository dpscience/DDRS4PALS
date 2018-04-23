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

#include "drs4settingsmanager.h"

static DRS4SettingsManager *__sharedInstanceSettingsManager = nullptr;

DRS4SettingsManager::DRS4SettingsManager() :
    m_negativeLifetimes(false),
    m_isPositivSignal(false),
    m_ignoreBusy(true),
    m_cfdLevelA(0.25f),
    m_cfdLevelB(0.25f),
    m_startCell(0),
    m_stopCell(0),
    m_burstMode(false),
    m_offsetNsAB(5),
    m_offsetNsBA(5),
    m_offsetNsCoincidence(5),
    m_offsetNsMerged(5),
    m_scalerNsAB(30),
    m_scalerNsBA(30),
    m_scalerNsCoincidence(30),
    m_scalerNsMerged(30),
    m_channelCountAB(4096),
    m_channelCountBA(4096),
    m_channelCountCoincidence(4096),
    m_channelCountMerged(4096),
    m_triggerSource((1<<8)|(1<<9)), //AND logic (Chn 1 && Chn 2)
    m_isTriggerPolarityPositive(false),
    m_triggerLevelA(-15),
    m_triggerLevelB(-15),
    m_triggerDelayInNS(0),
    m_comment(""),
    m_lastSaveDate(QDateTime::currentDateTime().toString()),
    m_channelNumberA(0),
    m_channelNumberB(1),
    m_startAChannelMin(0),
    m_startBChannelMin(0),
    m_stopAChannelMin(0),
    m_stopBChannelMin(0),
    m_startAChannelMax(1024),
    m_startBChannelMax(1024),
    m_stopAChannelMax(1024),
    m_stopBChannelMax(1024),
    m_bForceCoincidence(false),
    m_meanNs(0.0f),
    m_fitIter(40),
    m_fitIterMerged(40),
    m_fitIterAB(40),
    m_fitIterBA(40),
    m_sweepInNanoSec(200.0f),
    m_freqInGHz(5.12f),
    m_bPulseAreaFilterEnabledPlot(true),
    m_bPulseAreaFilterEnabled(false),
    m_pulseAreaBinningA(500),
    m_pulseAreaBinningB(500),
    m_pulseAreaNormA(5.0f),
    m_pulseAreaNormB(5.0f),
    m_pulseAreaLeftUpper_A(250.0f),
    m_pulseAreaRightUpper_A(250.0f),
    m_pulseAreaLeftLower_A(250.0f),
    m_pulseAreaRightLower_A(250.0f),
    m_pulseAreaLeftUpper_B(250.0f),
    m_pulseAreaRightUpper_B(250.0f),
    m_pulseAreaLeftLower_B(250.0f),
    m_pulseAreaRightLower_B(250.0f),
    m_fileName(""),
    m_persistance_leftAInNs(2.0f),
    m_persistance_leftBInNs(2.0f),
    m_persistance_rightAInNs(2.0f),
    m_persistance_rightBInNs(2.0f),
    m_persistanceEnabled(false)
{
    m_parentNode = new DSimpleXMLNode("DDRS4PALS");

    m_versionNode = new DSimpleXMLNode("file-version");
    m_versionNode->setValue((int)VERSION_SETTINGS_FILE);

    m_generalSettingsNode = new DSimpleXMLNode("general-board-settings");
    m_cfdDeterminationSettingsNode = new DSimpleXMLNode("timing-algorithm-settings");

    m_cfdAlgorithmTypeNode = new DSimpleXMLNode("algorithm-type");
    m_cfdAlgorithmTypeNode->setValue(DRS4InterpolationType::type::spline);

    m_cfdAlgorithmType_polynomialInterpolationNode = new DSimpleXMLNode("type-1-polynomial-interpolation");

        m_polynomialIntraSamplingPointsNode = new DSimpleXMLNode("intra-sampling-points");
        m_polynomialIntraSamplingPointsNode->setValue(10);

    m_cfdAlgorithmType_splineAndFittingInterpolationNode = new DSimpleXMLNode("type-2-spline-interpolation");

        m_splineTypeNode = new DSimpleXMLNode("spline-type");

                m_splineTypeLinearNode = new DSimpleXMLNode("linear");
                    m_splineTypeLinearEnabledNode = new DSimpleXMLNode("enabled?");
                    m_splineTypeLinearEnabledNode->setValue(true);

                m_splineTypeCubicNode = new DSimpleXMLNode("alglib-cubic");
                    m_splineTypeCubicEnabledNode = new DSimpleXMLNode("enabled?");
                    m_splineTypeCubicEnabledNode->setValue(false);

                m_splineTypeAkimaNode = new DSimpleXMLNode("alglib-akima");
                    m_splineTypeAkimaEnabledNode = new DSimpleXMLNode("enabled?");
                    m_splineTypeAkimaEnabledNode->setValue(false);

                m_splineTypeCatmullRomNode = new DSimpleXMLNode("alglib-catmull-rom");
                    m_splineTypeCatmullRomEnabledNode = new DSimpleXMLNode("enabled?");
                    m_splineTypeCatmullRomEnabledNode->setValue(false);

                m_splineTypeMonotoneNode = new DSimpleXMLNode("alglib-monotone");
                    m_splineTypeMonotoneEnabledNode = new DSimpleXMLNode("enabled?");
                    m_splineTypeMonotoneEnabledNode->setValue(false);

                m_splineTypeTKCubicNode = new DSimpleXMLNode("tk-cubic");
                    m_splineTypeTKCubicEnabledNode = new DSimpleXMLNode("enabled?");
                    m_splineTypeTKCubicEnabledNode->setValue(false);

        m_splineIntraSamplingPointsNode = new DSimpleXMLNode("intra-sampling-points");
        m_splineIntraSamplingPointsNode->setValue(10);

    m_phsSettingsNode = new DSimpleXMLNode("phs-settings");
    m_spectrumSettingsNode = new DSimpleXMLNode("spectrum-settings");
    m_abSpecNode = new DSimpleXMLNode("spectrum-AB");
    m_baSpecNode = new DSimpleXMLNode("spectrum-BA");
    m_mergedSpecNode = new DSimpleXMLNode("spectrum-Merged");
    m_promptSpecNode = new DSimpleXMLNode("spectrum-Prompt");
    m_areaFilterSettingsNode = new DSimpleXMLNode("area-filter-settings");
    m_medianFilterSettingsNode = new DSimpleXMLNode("median-filter-settings");
    m_persistancePlotSettingsNode = new DSimpleXMLNode("persistance-plot-settings");

    m_burstModeNode = new DSimpleXMLNode("burst-mode?");
    m_burstModeNode->setValue(false);

    m_negativeLifetimesNode = new DSimpleXMLNode("negative-lifetimes?");
    m_negativeLifetimesNode->setValue(m_negativeLifetimes);

    m_ignoreBusyNode = new DSimpleXMLNode("ignore-busy-state?");
    m_ignoreBusyNode->setValue(m_ignoreBusy);

    m_cfdLevelANode = new DSimpleXMLNode("CFD-level-detector-A");
    m_cfdLevelANode->setValue(m_cfdLevelA);
    m_cfdLevelBNode = new DSimpleXMLNode("CFD-level-detector-B");
    m_cfdLevelBNode->setValue(m_cfdLevelB);

    m_sweepInNanoSecNode = new DSimpleXMLNode("sweep-in-ns");
    m_sweepInNanoSecNode->setValue(m_sweepInNanoSec);
    m_freqInGHzNode = new DSimpleXMLNode("sample-speed-in-GHz");
    m_freqInGHzNode->setValue(m_freqInGHz);

    m_startCellNode = new DSimpleXMLNode("ROI-start-cell");
    m_startCellNode->setValue(m_startCell);
    m_stopCellNode = new DSimpleXMLNode("ROI-stop-cell");
    m_stopCellNode->setValue(m_stopCell);

    m_offsetNsABNode = new DSimpleXMLNode("offset-AB-spectrum-in-ns");
    m_offsetNsABNode->setValue(m_offsetNsAB);
    m_offsetNsBANode = new DSimpleXMLNode("offset-BA-spectrum-in-ns");
    m_offsetNsBANode->setValue(m_offsetNsBA);
    m_offsetNsCoincidenceNode = new DSimpleXMLNode("offset-Prompt-spectrum-in-ns");
    m_offsetNsCoincidenceNode->setValue(m_offsetNsCoincidence);
    m_offsetNsMergedNode = new DSimpleXMLNode("offset-Merged-spectrum-in-ns");
    m_offsetNsMergedNode->setValue(m_offsetNsMerged);

    m_scalerNsABNode = new DSimpleXMLNode("scale-AB-spectrum-in-ns");
    m_scalerNsABNode->setValue(m_scalerNsAB);
    m_scalerNsBANode = new DSimpleXMLNode("scale-BA-spectrum-in-ns");
    m_scalerNsBANode->setValue(m_scalerNsBA);
    m_scalerNsCoincidenceNode = new DSimpleXMLNode("scale-Prompt-spectrum-in-ns");
    m_scalerNsCoincidenceNode->setValue(m_scalerNsCoincidence);
    m_scalerNsMergedNode = new DSimpleXMLNode("scale-Merged-spectrum-in-ns");
    m_scalerNsMergedNode->setValue(m_scalerNsMerged);

    m_channelCountABNode = new DSimpleXMLNode("number-of-channels-AB-spectrum");
    m_channelCountABNode->setValue(m_channelCountAB);
    m_channelCountBANode = new DSimpleXMLNode("number-of-channels-BA-spectrum");
    m_channelCountBANode->setValue(m_channelCountBA);
    m_channelCountCoincidenceNode = new DSimpleXMLNode("number-of-channels-Prompt-spectrum");
    m_channelCountCoincidenceNode->setValue(m_channelCountCoincidence);
    m_channelCountMergedNode = new DSimpleXMLNode("number-of-channels-Merged-spectrum");
    m_channelCountMergedNode->setValue(m_channelCountMerged);

    m_triggerSourceNode = new DSimpleXMLNode("trigger-source");
    m_triggerSourceNode->setValue(m_triggerSource);
    m_isPositivSignalNode = new DSimpleXMLNode("positive-signal-polarity?");
    m_isPositivSignalNode->setValue(m_isPositivSignal);

    m_isTriggerPolarityPositiveNode = new DSimpleXMLNode("trigger-polarity?");
    m_isTriggerPolarityPositiveNode->setValue(m_isTriggerPolarityPositive);
    m_triggerLevelANode = new DSimpleXMLNode("trigger-level-channel-A-in-mV");
    m_triggerLevelANode->setValue(m_triggerLevelA);
    m_triggerLevelBNode = new DSimpleXMLNode("trigger-level-channel-B-in-mV");
    m_triggerLevelBNode->setValue(m_triggerLevelB);
    m_triggerDelayInNSNode = new DSimpleXMLNode("trigger-delay-in-ns");
    m_triggerDelayInNSNode->setValue(m_triggerDelayInNS);

    m_commentNode = new DSimpleXMLNode("user-comment");
    m_commentNode->setValue(m_comment);
    m_lastSaveDateNode = new DSimpleXMLNode("last-save-date");
    m_lastSaveDateNode->setValue(m_lastSaveDate);

    m_channelNumberANode = new DSimpleXMLNode("analog-input-channel-A");
    m_channelNumberANode->setValue(m_channelNumberA);
    m_channelNumberBNode = new DSimpleXMLNode("analog-input-channel-B");
    m_channelNumberBNode->setValue(m_channelNumberB);

    m_startAChannelMinNode = new DSimpleXMLNode("phs-start-channel-min-A");
    m_startAChannelMinNode->setValue(m_startAChannelMin);

    m_startBChannelMinNode = new DSimpleXMLNode("phs-start-channel-min-B");
    m_startBChannelMinNode->setValue(m_startBChannelMin);

    m_startAChannelMaxNode = new DSimpleXMLNode("phs-start-channel-max-A");
    m_startAChannelMaxNode->setValue(m_startAChannelMax);

    m_startBChannelMaxNode = new DSimpleXMLNode("phs-start-channel-max-B");
    m_startBChannelMaxNode->setValue(m_startBChannelMax);

    m_stopAChannelMinNode = new DSimpleXMLNode("phs-stop-channel-min-A");
    m_stopAChannelMinNode->setValue(m_stopAChannelMin);

    m_stopBChannelMinNode = new DSimpleXMLNode("phs-stop-channel-min-B");
    m_stopBChannelMinNode->setValue(m_stopBChannelMin);

    m_stopAChannelMaxNode = new DSimpleXMLNode("phs-stop-channel-max-A");
    m_stopAChannelMaxNode->setValue(m_stopAChannelMax);

    m_stopBChannelMaxNode = new DSimpleXMLNode("phs-stop-channel-max-B");
    m_stopBChannelMaxNode->setValue(m_stopBChannelMax);

    m_bForceCoincidenceNode = new DSimpleXMLNode("force-stop-stop-Prompt-spectrum");
    m_bForceCoincidenceNode->setValue(m_bForceCoincidence);

    m_meanNSNode = new DSimpleXMLNode("arrival-time-spread-in-ns");
    m_meanNSNode->setValue(m_meanNs);

    m_fitIterNode = new DSimpleXMLNode("Prompt-spectrum-fit-iterations");
    m_fitIterNode->setValue(m_fitIter);

    m_fitIterMergedNode = new DSimpleXMLNode("Merged-spectrum-fit-iterations");
    m_fitIterMergedNode->setValue(m_fitIterMerged);

    m_fitIterABNode = new DSimpleXMLNode("AB-spectrum-fit-iterations");
    m_fitIterABNode->setValue(m_fitIterAB);

    m_fitIterBANode = new DSimpleXMLNode("BA-spectrum-fit-iterations");
    m_fitIterBANode->setValue(m_fitIterBA);

    m_pulseAreaFilerEnabledNode = new DSimpleXMLNode("pulse-area-filter?");
    m_pulseAreaFilerEnabledNode->setValue(m_bPulseAreaFilterEnabled);

    m_pulseAreaFilerEnabledPlotNode = new DSimpleXMLNode("pulse-area-filter-plot?");
    m_pulseAreaFilerEnabledPlotNode->setValue(m_bPulseAreaFilterEnabledPlot);

    m_pulseAreaBinningANode = new DSimpleXMLNode("pulse-area-filter-binning-A");
    m_pulseAreaBinningANode->setValue(m_pulseAreaBinningA);

    m_pulseAreaBinningBNode = new DSimpleXMLNode("pulse-area-filter-binning-B");
    m_pulseAreaBinningBNode->setValue(m_pulseAreaBinningB);

    m_pulseAreaNormANode = new DSimpleXMLNode("pulse-area-filter-nomalization-fraction-in-%-A");
    m_pulseAreaNormANode->setValue(m_pulseAreaNormA);

    m_pulseAreaNormBNode = new DSimpleXMLNode("pulse-area-filter-nomalization-fraction-in-%-B");
    m_pulseAreaNormBNode->setValue(m_pulseAreaNormB);

    m_pulseAreaLeftUpper_A_Node = new DSimpleXMLNode("pulse-area-filter-upper-left-bounding-A");
    m_pulseAreaLeftUpper_A_Node->setValue(m_pulseAreaLeftUpper_A);

    m_pulseAreaRightUpper_A_Node = new DSimpleXMLNode("pulse-area-filter-upper-right-bounding-A");
    m_pulseAreaRightUpper_A_Node->setValue(m_pulseAreaRightUpper_A);

    m_pulseAreaLeftLower_A_Node = new DSimpleXMLNode("pulse-area-filter-lower-left-bounding-A");
    m_pulseAreaLeftLower_A_Node->setValue(m_pulseAreaLeftLower_A);

    m_pulseAreaRightLower_A_Node = new DSimpleXMLNode("pulse-area-filter-lower-right-bounding-A");
    m_pulseAreaRightLower_A_Node->setValue(m_pulseAreaRightLower_A);

    m_pulseAreaLeftUpper_B_Node = new DSimpleXMLNode("pulse-area-filter-upper-left-bounding-B");
    m_pulseAreaLeftUpper_B_Node->setValue(m_pulseAreaLeftUpper_B);

    m_pulseAreaRightUpper_B_Node = new DSimpleXMLNode("pulse-area-filter-upper-right-bounding-B");
    m_pulseAreaRightUpper_B_Node->setValue(m_pulseAreaRightUpper_B);

    m_pulseAreaLeftLower_B_Node = new DSimpleXMLNode("pulse-area-filter-lower-left-bounding-B");
    m_pulseAreaLeftLower_B_Node->setValue(m_pulseAreaLeftLower_B);

    m_pulseAreaRightLower_B_Node = new DSimpleXMLNode("pulse-area-filter-lower-right-bounding-B");
    m_pulseAreaRightLower_B_Node->setValue(m_pulseAreaRightLower_B);

    m_medianFilterActivated_A_Node = new DSimpleXMLNode("median-filter-A?");
    m_medianFilterWindowSize_A_Node = new DSimpleXMLNode("median-filter-window-size-A");

    m_medianFilterActivated_B_Node = new DSimpleXMLNode("median-filter-B?");
    m_medianFilterWindowSize_B_Node = new DSimpleXMLNode("median-filter-window-size-B");

    m_persistanceEnabled_Node = new DSimpleXMLNode("persistance-plot?");
    m_persistanceEnabled_Node->setValue(m_persistanceEnabled);

    m_persistanceUsingRefB_A_Node = new DSimpleXMLNode("persistance-plot-using-CFD-B-as-reference-A?");
    m_persistanceUsingRefB_A_Node->setValue(false);

    m_persistanceUsingRefA_B_Node = new DSimpleXMLNode("persistance-plot-using-CFD-A-as-reference-B?");
    m_persistanceUsingRefA_B_Node->setValue(false);

    m_persistance_leftAInNs_Node = new DSimpleXMLNode("persistance-plot-left-of-maxValue-in-ns-A");
    m_persistance_leftAInNs_Node->setValue(m_persistance_leftAInNs);

    m_persistance_leftBInNs_Node = new DSimpleXMLNode("persistance-plot-left-of-maxValue-in-ns-B");
    m_persistance_leftBInNs_Node->setValue(m_persistance_leftBInNs);

    m_persistance_rightAInNs_Node = new DSimpleXMLNode("persistance-plot-right-of-maxValue-in-ns-A");
    m_persistance_rightAInNs_Node->setValue(m_persistance_rightAInNs);

    m_persistance_rightBInNs_Node = new DSimpleXMLNode("persistance-plot-right-of-maxValue-in-ns-B");
    m_persistance_rightBInNs_Node->setValue(m_persistance_rightBInNs);


    (*m_parentNode) << m_lastSaveDateNode
                    << m_versionNode
                    << m_burstModeNode
                    << m_negativeLifetimesNode
                    << m_bForceCoincidenceNode
                    << m_commentNode;

    (*m_generalSettingsNode) << m_channelNumberANode
                             << m_channelNumberBNode
                             << m_ignoreBusyNode
                             << m_triggerSourceNode
                             << m_isTriggerPolarityPositiveNode
                             << m_triggerLevelANode
                             << m_triggerLevelBNode
                             << m_triggerDelayInNSNode
                             << m_isPositivSignalNode
                             << m_startCellNode
                             << m_stopCellNode
                             << m_sweepInNanoSecNode
                             << m_freqInGHzNode;

    (*m_phsSettingsNode) << m_startAChannelMinNode
                         << m_startBChannelMinNode
                         << m_startAChannelMaxNode
                         << m_startBChannelMaxNode
                         << m_stopAChannelMinNode
                         << m_stopBChannelMinNode
                         << m_stopAChannelMaxNode
                         << m_stopBChannelMaxNode;

    (*m_abSpecNode) << m_offsetNsABNode << m_scalerNsABNode << m_channelCountABNode << m_fitIterABNode;
    (*m_baSpecNode) << m_offsetNsBANode << m_scalerNsBANode << m_channelCountBANode << m_fitIterBANode;
    (*m_mergedSpecNode) << m_offsetNsMergedNode << m_scalerNsMergedNode << m_channelCountMergedNode << m_fitIterMergedNode << m_meanNSNode;
    (*m_promptSpecNode) << m_offsetNsCoincidenceNode << m_scalerNsCoincidenceNode << m_channelCountCoincidenceNode << m_fitIterNode;

    (*m_spectrumSettingsNode) << m_abSpecNode
                              << m_baSpecNode
                              << m_mergedSpecNode
                              << m_promptSpecNode;

    (*m_cfdAlgorithmType_polynomialInterpolationNode) << m_polynomialIntraSamplingPointsNode;

    (*m_splineTypeLinearNode) << m_splineTypeLinearEnabledNode;
    (*m_splineTypeCubicNode) << m_splineTypeCubicEnabledNode;
    (*m_splineTypeAkimaNode) << m_splineTypeAkimaEnabledNode;
    (*m_splineTypeCatmullRomNode) << m_splineTypeCatmullRomEnabledNode;
    (*m_splineTypeMonotoneNode) << m_splineTypeMonotoneEnabledNode;
    (*m_splineTypeTKCubicNode) << m_splineTypeTKCubicEnabledNode;

    (*m_splineTypeNode) << m_splineTypeLinearNode
                        << m_splineTypeCubicNode
                        << m_splineTypeAkimaNode
                        << m_splineTypeCatmullRomNode
                        << m_splineTypeMonotoneNode
                        << m_splineTypeTKCubicNode;
    (*m_cfdAlgorithmType_splineAndFittingInterpolationNode) << m_splineTypeNode
                                                            << m_splineIntraSamplingPointsNode;

    (*m_cfdDeterminationSettingsNode) << m_cfdLevelANode
                                      << m_cfdLevelBNode
                                      << m_cfdAlgorithmTypeNode
                                      << m_cfdAlgorithmType_polynomialInterpolationNode
                                      << m_cfdAlgorithmType_splineAndFittingInterpolationNode;

    (*m_areaFilterSettingsNode) << m_pulseAreaFilerEnabledNode
                                << m_pulseAreaFilerEnabledPlotNode
                                << m_pulseAreaBinningANode
                                << m_pulseAreaBinningBNode
                                << m_pulseAreaNormANode
                                << m_pulseAreaNormBNode
                                << m_pulseAreaLeftUpper_A_Node
                                << m_pulseAreaRightUpper_A_Node
                                << m_pulseAreaLeftLower_A_Node
                                << m_pulseAreaRightLower_A_Node
                                << m_pulseAreaLeftUpper_B_Node
                                << m_pulseAreaRightUpper_B_Node
                                << m_pulseAreaLeftLower_B_Node
                                << m_pulseAreaRightLower_B_Node;

    (*m_medianFilterSettingsNode) << m_medianFilterActivated_A_Node
                                  << m_medianFilterWindowSize_A_Node
                                  << m_medianFilterActivated_B_Node
                                  << m_medianFilterWindowSize_B_Node;

    (*m_persistancePlotSettingsNode) << m_persistanceEnabled_Node
                                     << m_persistanceUsingRefB_A_Node
                                     << m_persistanceUsingRefA_B_Node
                                     << m_persistance_leftAInNs_Node
                                     << m_persistance_leftBInNs_Node
                                     << m_persistance_rightAInNs_Node
                                     << m_persistance_rightBInNs_Node;

    (*m_parentNode) << m_generalSettingsNode
                    << m_cfdDeterminationSettingsNode
                    << m_phsSettingsNode
                    << m_spectrumSettingsNode
                    << m_areaFilterSettingsNode
                    << m_medianFilterSettingsNode
                    << m_persistancePlotSettingsNode;
}

DRS4SettingsManager::~DRS4SettingsManager()
{
    DDELETE_SAFETY(m_parentNode);
    DDELETE_SAFETY(__sharedInstanceSettingsManager);
}

DRS4SettingsManager *DRS4SettingsManager::sharedInstance()
{
    if ( !__sharedInstanceSettingsManager )
        __sharedInstanceSettingsManager = new DRS4SettingsManager;

    return __sharedInstanceSettingsManager;
}

bool DRS4SettingsManager::load(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    DSimpleXMLReader reader(path);

    DSimpleXMLTag tag;
    reader.readFromFile(&tag, false);

    bool ok = false;

    const DSimpleXMLTag pTag = tag.getTag("DDRS4PALS", &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pGeneralSettingsTag = pTag.getTag(m_generalSettingsNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pPHSSettingsTag = pTag.getTag(m_phsSettingsNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pCFDSettingsTag = pTag.getTag(m_cfdDeterminationSettingsNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pCFDAlgorithmPolynomialTag = pCFDSettingsTag.getTag(m_cfdAlgorithmType_polynomialInterpolationNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pCFDAlgorithmSplineTag = pCFDSettingsTag.getTag(m_cfdAlgorithmType_splineAndFittingInterpolationNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pCFDAlgorithmSplineLinearTag = pCFDAlgorithmSplineTag.getTag(m_splineTypeLinearNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pCFDAlgorithmSplineCubicTag = pCFDAlgorithmSplineTag.getTag(m_splineTypeCubicNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pCFDAlgorithmSplineAkimaTag = pCFDAlgorithmSplineTag.getTag(m_splineTypeAkimaNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pCFDAlgorithmSplineCatmullRomTag = pCFDAlgorithmSplineTag.getTag(m_splineTypeCatmullRomNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pCFDAlgorithmSplineMonotoneTag = pCFDAlgorithmSplineTag.getTag(m_splineTypeMonotoneNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pCFDAlgorithmSplineTKCubicTag = pCFDAlgorithmSplineTag.getTag(m_splineTypeTKCubicNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pSpectrumSettingsTag = pTag.getTag(m_spectrumSettingsNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pMergedSpectrumSettingsTag = pSpectrumSettingsTag.getTag(m_mergedSpecNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pABSpectrumSettingsTag = pSpectrumSettingsTag.getTag(m_abSpecNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pBASpectrumSettingsTag = pSpectrumSettingsTag.getTag(m_baSpecNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pPromptSpectrumSettingsTag = pSpectrumSettingsTag.getTag(m_promptSpecNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pAreaFilterSettingsTag = pTag.getTag(m_areaFilterSettingsNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pMedianFilterSettingsTag = pTag.getTag(m_medianFilterSettingsNode, &ok);

    if ( !ok )
        return false;

    const DSimpleXMLTag pPersistanceSettingsTag = pTag.getTag(m_persistancePlotSettingsNode, &ok);

    if ( !ok )
        return false;


    QVariant version = pTag.getValueAt(m_versionNode, &ok);

    if (!ok)
        m_versionNode->setValue((int)VERSION_SETTINGS_FILE);
    else {
        const int versionInt = version.toInt(&ok);
        if (!ok)
            m_versionNode->setValue((int)VERSION_SETTINGS_FILE);
        else {
            if (versionInt != VERSION_SETTINGS_FILE) {
                /* nothing TODO yet! */
            }
        }
    }

    m_lastSaveDateNode->setValue(pTag.getValueAt(m_lastSaveDateNode, &ok));
    if  ( !ok ) m_lastSaveDateNode->setValue(m_lastSaveDate);

    m_commentNode->setValue(pTag.getValueAt(m_commentNode, &ok));
    if  ( !ok ) m_commentNode->setValue(m_comment);

    m_bForceCoincidenceNode->setValue(pTag.getValueAt(m_bForceCoincidenceNode, &ok));
    if  ( !ok ) m_bForceCoincidenceNode->setValue(m_bForceCoincidence);

    m_negativeLifetimesNode->setValue(pTag.getValueAt(m_negativeLifetimesNode, &ok));
    if  ( !ok ) m_negativeLifetimesNode->setValue(m_negativeLifetimes);

    m_burstModeNode->setValue(pTag.getValueAt(m_burstModeNode, &ok));
    if  ( !ok ) m_burstModeNode->setValue(m_burstMode);


    /* phs-settings */

    m_startAChannelMinNode->setValue(pPHSSettingsTag.getValueAt(m_startAChannelMinNode, &ok));
    if  ( !ok ) m_startAChannelMinNode->setValue(m_startAChannelMin);

    m_startBChannelMinNode->setValue(pPHSSettingsTag.getValueAt(m_startBChannelMinNode, &ok));
    if  ( !ok ) m_startBChannelMinNode->setValue(m_startBChannelMin);

    m_stopAChannelMinNode->setValue(pPHSSettingsTag.getValueAt(m_stopAChannelMinNode, &ok));
    if  ( !ok ) m_stopAChannelMinNode->setValue(m_stopAChannelMin);

    m_stopBChannelMinNode->setValue(pPHSSettingsTag.getValueAt(m_stopBChannelMinNode, &ok));
    if  ( !ok ) m_stopBChannelMinNode->setValue(m_stopBChannelMin);


    m_startAChannelMaxNode->setValue(pPHSSettingsTag.getValueAt(m_startAChannelMaxNode, &ok));
    if  ( !ok ) m_startAChannelMaxNode->setValue(m_startAChannelMax);

    m_startBChannelMaxNode->setValue(pPHSSettingsTag.getValueAt(m_startBChannelMaxNode, &ok));
    if  ( !ok ) m_startBChannelMaxNode->setValue(m_startBChannelMax);

    m_stopAChannelMaxNode->setValue(pPHSSettingsTag.getValueAt(m_stopAChannelMaxNode, &ok));
    if  ( !ok ) m_stopAChannelMaxNode->setValue(m_stopAChannelMax);

    m_stopBChannelMaxNode->setValue(pPHSSettingsTag.getValueAt(m_stopBChannelMaxNode, &ok));
    if  ( !ok ) m_stopBChannelMaxNode->setValue(m_stopBChannelMax);

    /* AB spectrum */

    m_offsetNsABNode->setValue(pABSpectrumSettingsTag.getValueAt(m_offsetNsABNode, &ok));
    if  ( !ok ) m_offsetNsABNode->setValue(m_offsetNsAB);

    m_scalerNsABNode->setValue(pABSpectrumSettingsTag.getValueAt(m_scalerNsABNode, &ok));
    if  ( !ok ) m_scalerNsABNode->setValue(m_scalerNsAB);

    m_channelCountABNode->setValue(pABSpectrumSettingsTag.getValueAt(m_channelCountABNode, &ok));
    if  ( !ok ) m_channelCountABNode->setValue(m_channelCountAB);

    m_fitIterABNode->setValue(pABSpectrumSettingsTag.getValueAt(m_fitIterABNode, &ok));
    if  ( !ok ) m_fitIterABNode->setValue(m_fitIterAB);

    /* BA spectrum */

    m_offsetNsBANode->setValue(pBASpectrumSettingsTag.getValueAt(m_offsetNsBANode, &ok));
    if  ( !ok ) m_offsetNsBANode->setValue(m_offsetNsBA);

    m_scalerNsBANode->setValue(pBASpectrumSettingsTag.getValueAt(m_scalerNsBANode, &ok));
    if  ( !ok ) m_scalerNsBANode->setValue(m_scalerNsBA);

    m_channelCountBANode->setValue(pBASpectrumSettingsTag.getValueAt(m_channelCountBANode, &ok));
    if  ( !ok ) m_channelCountBANode->setValue(m_channelCountBA);

    m_fitIterBANode->setValue(pBASpectrumSettingsTag.getValueAt(m_fitIterBANode, &ok));
    if  ( !ok ) m_fitIterBANode->setValue(m_fitIterBA);

    /* Prompt spectrum */

    m_offsetNsCoincidenceNode->setValue(pPromptSpectrumSettingsTag.getValueAt(m_offsetNsCoincidenceNode, &ok));
    if  ( !ok ) m_offsetNsCoincidenceNode->setValue(m_offsetNsCoincidence);

    m_scalerNsCoincidenceNode->setValue(pPromptSpectrumSettingsTag.getValueAt(m_scalerNsCoincidenceNode, &ok));
    if  ( !ok ) m_scalerNsCoincidenceNode->setValue(m_scalerNsCoincidence);

    m_channelCountCoincidenceNode->setValue(pPromptSpectrumSettingsTag.getValueAt(m_channelCountCoincidenceNode, &ok));
    if  ( !ok ) m_channelCountCoincidenceNode->setValue(m_channelCountCoincidence);

    m_fitIterNode->setValue(pPromptSpectrumSettingsTag.getValueAt(m_fitIterNode, &ok));
    if  ( !ok ) m_fitIterNode->setValue(m_fitIter);

    /* Merged spectrum */

    m_offsetNsMergedNode->setValue(pMergedSpectrumSettingsTag.getValueAt(m_offsetNsMergedNode, &ok));
    if  ( !ok ) m_offsetNsMergedNode->setValue(m_offsetNsMerged);

    m_scalerNsMergedNode->setValue(pMergedSpectrumSettingsTag.getValueAt(m_scalerNsMergedNode, &ok));
    if  ( !ok ) m_scalerNsMergedNode->setValue(m_scalerNsMerged);

    m_channelCountMergedNode->setValue(pMergedSpectrumSettingsTag.getValueAt(m_channelCountMergedNode, &ok));
    if  ( !ok ) m_channelCountMergedNode->setValue(m_channelCountMerged);

    m_fitIterMergedNode->setValue(pMergedSpectrumSettingsTag.getValueAt(m_fitIterMergedNode, &ok));
    if  ( !ok ) m_fitIterMergedNode->setValue(m_fitIterMerged);

    m_meanNSNode->setValue(pMergedSpectrumSettingsTag.getValueAt(m_meanNSNode, &ok));
    if  ( !ok ) m_meanNSNode->setValue(m_meanNs);

    /* General settings */

    m_channelNumberANode->setValue(pGeneralSettingsTag.getValueAt(m_channelNumberANode, &ok));
    if  ( !ok ) m_channelNumberANode->setValue(m_channelNumberA);

    m_channelNumberBNode->setValue(pGeneralSettingsTag.getValueAt(m_channelNumberBNode, &ok));
    if  ( !ok ) m_channelNumberBNode->setValue(m_channelNumberB);

    m_triggerSourceNode->setValue(pGeneralSettingsTag.getValueAt(m_triggerSourceNode, &ok));
    if  ( !ok ) m_triggerSourceNode->setValue(m_triggerSource);

    m_isPositivSignalNode->setValue(pGeneralSettingsTag.getValueAt(m_isPositivSignalNode, &ok));
    if  ( !ok ) m_isPositivSignalNode->setValue(m_isPositivSignal);

    m_isTriggerPolarityPositiveNode->setValue(pGeneralSettingsTag.getValueAt(m_isTriggerPolarityPositiveNode, &ok));
    if  ( !ok ) m_isTriggerPolarityPositiveNode->setValue(m_isTriggerPolarityPositive);

    m_triggerLevelANode->setValue(pGeneralSettingsTag.getValueAt(m_triggerLevelANode, &ok));
    if  ( !ok ) m_triggerLevelANode->setValue(m_triggerLevelA);

    m_triggerLevelBNode->setValue(pGeneralSettingsTag.getValueAt(m_triggerLevelBNode, &ok));
    if  ( !ok ) m_triggerLevelBNode->setValue(m_triggerLevelB);

    m_triggerDelayInNSNode->setValue(pGeneralSettingsTag.getValueAt(m_triggerDelayInNSNode, &ok));
    if  ( !ok ) m_triggerDelayInNSNode->setValue(m_triggerDelayInNS);

    m_startCellNode->setValue(pGeneralSettingsTag.getValueAt(m_startCellNode, &ok));
    if  ( !ok ) m_startCellNode->setValue(m_startCell);

    m_stopCellNode->setValue(pGeneralSettingsTag.getValueAt(m_stopCellNode, &ok));
    if  ( !ok ) m_stopCellNode->setValue(m_stopCell);

    m_ignoreBusyNode->setValue(pGeneralSettingsTag.getValueAt(m_ignoreBusyNode, &ok));
    if  ( !ok ) m_ignoreBusyNode->setValue(m_ignoreBusy);

    m_sweepInNanoSecNode->setValue(pGeneralSettingsTag.getValueAt(m_sweepInNanoSecNode, &ok));
    if ( !ok ) m_sweepInNanoSecNode->setValue(m_sweepInNanoSec);

    m_freqInGHzNode->setValue(pGeneralSettingsTag.getValueAt(m_freqInGHzNode, &ok));
    if ( !ok ) m_freqInGHzNode->setValue(m_freqInGHz);

    /* Timing determination */

    m_cfdLevelANode->setValue(pCFDSettingsTag.getValueAt(m_cfdLevelANode, &ok));
    if  ( !ok ) m_cfdLevelANode->setValue(m_cfdLevelA);

    m_cfdLevelBNode->setValue(pCFDSettingsTag.getValueAt(m_cfdLevelBNode, &ok));
    if  ( !ok ) m_cfdLevelBNode->setValue(m_cfdLevelB);

    m_cfdAlgorithmTypeNode->setValue(pCFDSettingsTag.getValueAt(m_cfdAlgorithmTypeNode, &ok));
    if  ( !ok ) m_cfdAlgorithmTypeNode->setValue(DRS4InterpolationType::type::spline);

    m_polynomialIntraSamplingPointsNode->setValue(pCFDAlgorithmPolynomialTag.getValueAt(m_polynomialIntraSamplingPointsNode, &ok));
    if  ( !ok ) m_polynomialIntraSamplingPointsNode->setValue(10);

    m_splineIntraSamplingPointsNode->setValue(pCFDAlgorithmSplineTag.getValueAt(m_splineIntraSamplingPointsNode, &ok));
    if  ( !ok ) m_splineIntraSamplingPointsNode->setValue(10);

    m_splineTypeLinearEnabledNode->setValue(pCFDAlgorithmSplineLinearTag.getValueAt(m_splineTypeLinearEnabledNode, &ok));
    if  ( !ok ) m_splineTypeLinearEnabledNode->setValue(true);

    m_splineTypeCubicEnabledNode->setValue(pCFDAlgorithmSplineCubicTag.getValueAt(m_splineTypeCubicEnabledNode, &ok));
    if  ( !ok ) m_splineTypeCubicEnabledNode->setValue(false);

    m_splineTypeAkimaEnabledNode->setValue(pCFDAlgorithmSplineAkimaTag.getValueAt(m_splineTypeAkimaEnabledNode, &ok));
    if  ( !ok ) m_splineTypeAkimaEnabledNode->setValue(false);

    m_splineTypeCatmullRomEnabledNode->setValue(pCFDAlgorithmSplineCatmullRomTag.getValueAt(m_splineTypeCatmullRomEnabledNode, &ok));
    if  ( !ok ) m_splineTypeCatmullRomEnabledNode->setValue(false);

    m_splineTypeMonotoneEnabledNode->setValue(pCFDAlgorithmSplineMonotoneTag.getValueAt(m_splineTypeMonotoneEnabledNode, &ok));
    if  ( !ok ) m_splineTypeMonotoneEnabledNode->setValue(false);

    m_splineTypeTKCubicEnabledNode->setValue(pCFDAlgorithmSplineTKCubicTag.getValueAt(m_splineTypeTKCubicEnabledNode, &ok));
    if  ( !ok ) m_splineTypeTKCubicEnabledNode->setValue(false);

    /* Area filter */

    m_pulseAreaFilerEnabledNode->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaFilerEnabledNode, &ok));
    if (!ok) m_pulseAreaFilerEnabledNode->setValue(m_bPulseAreaFilterEnabled);

    m_pulseAreaFilerEnabledPlotNode->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaFilerEnabledPlotNode, &ok));
    if (!ok) m_pulseAreaFilerEnabledPlotNode->setValue(m_bPulseAreaFilterEnabledPlot);

    m_pulseAreaBinningANode->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaBinningANode, &ok));
    if (!ok) m_pulseAreaBinningANode->setValue(m_pulseAreaBinningA);

    m_pulseAreaBinningBNode->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaBinningBNode, &ok));
    if (!ok) m_pulseAreaBinningBNode->setValue(m_pulseAreaBinningB);

    m_pulseAreaNormANode->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaNormANode, &ok));
    if (!ok) m_pulseAreaNormANode->setValue(m_pulseAreaNormA);

    m_pulseAreaNormBNode->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaNormBNode, &ok));
    if (!ok) m_pulseAreaNormBNode->setValue(m_pulseAreaNormB);

    m_pulseAreaLeftLower_A_Node->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaLeftLower_A_Node, &ok));
    if (!ok) m_pulseAreaLeftLower_A_Node->setValue(m_pulseAreaLeftLower_A);

    m_pulseAreaRightLower_A_Node->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaRightLower_A_Node, &ok));
    if (!ok) m_pulseAreaRightLower_A_Node->setValue(m_pulseAreaRightLower_A);

    m_pulseAreaLeftUpper_A_Node->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaLeftUpper_A_Node, &ok));
    if (!ok) m_pulseAreaLeftUpper_A_Node->setValue(m_pulseAreaLeftUpper_A);

    m_pulseAreaRightUpper_A_Node->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaRightUpper_A_Node, &ok));
    if (!ok) m_pulseAreaRightUpper_A_Node->setValue(m_pulseAreaRightUpper_A);

    m_pulseAreaLeftLower_B_Node->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaLeftLower_B_Node, &ok));
    if (!ok) m_pulseAreaLeftLower_B_Node->setValue(m_pulseAreaLeftLower_B);

    m_pulseAreaRightLower_B_Node->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaRightLower_B_Node, &ok));
    if (!ok) m_pulseAreaRightLower_B_Node->setValue(m_pulseAreaRightLower_B);

    m_pulseAreaLeftUpper_B_Node->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaLeftUpper_B_Node, &ok));
    if (!ok) m_pulseAreaLeftUpper_B_Node->setValue(m_pulseAreaLeftUpper_B);

    m_pulseAreaRightUpper_B_Node->setValue(pAreaFilterSettingsTag.getValueAt(m_pulseAreaRightUpper_B_Node, &ok));
    if (!ok) m_pulseAreaRightUpper_B_Node->setValue(m_pulseAreaRightUpper_B);

    /* Median filter */

    m_medianFilterActivated_A_Node->setValue(pMedianFilterSettingsTag.getValueAt(m_medianFilterActivated_A_Node, &ok));
    if (!ok) m_medianFilterActivated_A_Node->setValue(false);

    m_medianFilterActivated_B_Node->setValue(pMedianFilterSettingsTag.getValueAt(m_medianFilterActivated_B_Node, &ok));
    if (!ok) m_medianFilterActivated_B_Node->setValue(false);

    m_medianFilterWindowSize_A_Node->setValue(pMedianFilterSettingsTag.getValueAt(m_medianFilterWindowSize_A_Node, &ok));
    if (!ok) m_medianFilterWindowSize_A_Node->setValue(3);

    m_medianFilterWindowSize_B_Node->setValue(pMedianFilterSettingsTag.getValueAt(m_medianFilterWindowSize_B_Node, &ok));
    if (!ok) m_medianFilterWindowSize_B_Node->setValue(3);

    /* Persistance */

    m_persistanceUsingRefB_A_Node->setValue(pPersistanceSettingsTag.getValueAt(m_persistanceUsingRefB_A_Node, &ok));
    if (!ok) m_persistanceUsingRefB_A_Node->setValue(false);

    m_persistanceUsingRefA_B_Node->setValue(pPersistanceSettingsTag.getValueAt(m_persistanceUsingRefA_B_Node, &ok));
    if (!ok) m_persistanceUsingRefA_B_Node->setValue(false);

    m_persistance_leftAInNs_Node->setValue(pPersistanceSettingsTag.getValueAt(m_persistance_leftAInNs_Node, &ok));
    if (!ok) m_persistance_leftAInNs_Node->setValue(m_persistance_leftAInNs);

    m_persistance_leftBInNs_Node->setValue(pPersistanceSettingsTag.getValueAt(m_persistance_leftBInNs_Node, &ok));
    if (!ok) m_persistance_leftBInNs_Node->setValue(m_persistance_leftBInNs);

    m_persistance_rightAInNs_Node->setValue(pPersistanceSettingsTag.getValueAt(m_persistance_rightAInNs_Node, &ok));
    if (!ok) m_persistance_rightAInNs_Node->setValue(m_persistance_rightAInNs);

    m_persistance_rightBInNs_Node->setValue(pPersistanceSettingsTag.getValueAt(m_persistance_rightBInNs_Node, &ok));
    if (!ok) m_persistance_rightBInNs_Node->setValue(m_persistance_rightBInNs);

    m_persistanceEnabled_Node->setValue(pPersistanceSettingsTag.getValueAt(m_persistanceEnabled_Node, &ok));
    if (!ok) m_persistanceEnabled_Node->setValue(m_persistanceEnabled);


    m_fileName = path;

    return true;
}

bool DRS4SettingsManager::save(const QString &path, bool autosave)
{
    setLastSaveDate(QDateTime::currentDateTime().toString());

    /* always override with the latest version */
    m_versionNode->setValue((int)VERSION_SETTINGS_FILE);

    QMutexLocker locker(&m_mutex);

    DSimpleXMLWriter writer(path);

    const bool ok = writer.writeToFile(m_parentNode, false);

    if (ok && !autosave)
        m_fileName = path;

    return ok;
}

DSimpleXMLNode *DRS4SettingsManager::parentNode() const
{
    return m_parentNode;
}

void DRS4SettingsManager::setForceCoincidence(bool force)
{
    QMutexLocker locker(&m_mutex);

    m_bForceCoincidence = force;
    m_bForceCoincidenceNode->setValue(force);
}

void DRS4SettingsManager::setBurstMode(bool on)
{
    QMutexLocker locker(&m_mutex);

    m_burstModeNode->setValue(on);
    m_burstMode = on;
}

void DRS4SettingsManager::setStartCell(int startCell)
{
    QMutexLocker locker(&m_mutex);

    m_startCellNode->setValue(startCell);
    m_startCell = startCell;
}

void DRS4SettingsManager::setStopCell(int stopCell)
{
    QMutexLocker locker(&m_mutex);

    m_stopCellNode->setValue(stopCell);
    m_stopCell = stopCell;
}

void DRS4SettingsManager::setPositivSignal(bool positiv)
{
    QMutexLocker locker(&m_mutex);

    m_isPositivSignalNode->setValue(positiv);
    m_isPositivSignal = positiv;
}

void DRS4SettingsManager::setNegativeLifetimeAccepted(bool accepted)
{
    QMutexLocker locker(&m_mutex);

    m_negativeLifetimesNode->setValue(accepted);
    m_negativeLifetimes = accepted;
}

void DRS4SettingsManager::setIgnoreBusyState(bool ignore)
{
    QMutexLocker locker(&m_mutex);

    m_ignoreBusyNode->setValue(ignore);
    m_ignoreBusy = ignore;
}

void DRS4SettingsManager::setCFDLevelA(double level)
{
    QMutexLocker locker(&m_mutex);

    m_cfdLevelANode->setValue(level);
    m_cfdLevelA = level;
}

void DRS4SettingsManager::setCFDLevelB(double level)
{
    QMutexLocker locker(&m_mutex);

    m_cfdLevelBNode->setValue(level);
    m_cfdLevelB = level;
}

void DRS4SettingsManager::setOffsetInNSAB(double offset)
{
    QMutexLocker locker(&m_mutex);

    m_offsetNsABNode->setValue(offset);
    m_offsetNsAB = offset;
}

void DRS4SettingsManager::setOffsetInNSBA(double offset)
{
    QMutexLocker locker(&m_mutex);

    m_offsetNsBANode->setValue(offset);
    m_offsetNsBA = offset;
}

void DRS4SettingsManager::setOffsetInNSCoincidence(double offset)
{
    QMutexLocker locker(&m_mutex);

    m_offsetNsCoincidenceNode->setValue(offset);
    m_offsetNsCoincidence = offset;
}

void DRS4SettingsManager::setOffsetInNSMerged(double offset)
{
    QMutexLocker locker(&m_mutex);

    m_offsetNsMergedNode->setValue(offset);
    m_offsetNsMerged = offset;
}

void DRS4SettingsManager::setScalerInNSAB(double scaler)
{
    QMutexLocker locker(&m_mutex);

    m_scalerNsABNode->setValue(scaler);
    m_scalerNsAB = scaler;
}

void DRS4SettingsManager::setScalerInNSBA(double scaler)
{
    QMutexLocker locker(&m_mutex);

    m_scalerNsBANode->setValue(scaler);
    m_scalerNsBA = scaler;
}

void DRS4SettingsManager::setScalerInNSCoincidence(double scaler)
{
    QMutexLocker locker(&m_mutex);

    m_scalerNsCoincidenceNode->setValue(scaler);
    m_scalerNsCoincidence = scaler;
}

void DRS4SettingsManager::setScalerInNSMerged(double scaler)
{
    QMutexLocker locker(&m_mutex);

    m_scalerNsMergedNode->setValue(scaler);
    m_scalerNsMerged = scaler;
}

void DRS4SettingsManager::setChannelCntAB(int count)
{
    QMutexLocker locker(&m_mutex);

    m_channelCountABNode->setValue(count);
    m_channelCountAB = count;
}

void DRS4SettingsManager::setChannelCntBA(int count)
{
    QMutexLocker locker(&m_mutex);

    m_channelCountBANode->setValue(count);
    m_channelCountBA = count;
}

void DRS4SettingsManager::setChannelCntCoincindence(int count)
{
    QMutexLocker locker(&m_mutex);

    m_channelCountCoincidenceNode->setValue(count);
    m_channelCountCoincidence = count;
}

void DRS4SettingsManager::setChannelCntMerged(int count)
{
    QMutexLocker locker(&m_mutex);

    m_channelCountMergedNode->setValue(count);
    m_channelCountMerged = count;
}

void DRS4SettingsManager::setTriggerSource(int triggerSource)
{
    QMutexLocker locker(&m_mutex);

    m_triggerSourceNode->setValue(triggerSource);
    m_triggerSource = triggerSource;
}

void DRS4SettingsManager::setTriggerPolarityPositive(bool positive)
{
    QMutexLocker locker(&m_mutex);

    m_isTriggerPolarityPositiveNode->setValue(positive);
    m_isTriggerPolarityPositive = positive;
}

void DRS4SettingsManager::setTriggerLevelAmV(double triggerLevelInmV)
{
    QMutexLocker locker(&m_mutex);

    m_triggerLevelANode->setValue(triggerLevelInmV);
    m_triggerLevelA = triggerLevelInmV;
}

void DRS4SettingsManager::setTriggerLevelBmV(double triggerLevelInmV)
{
    QMutexLocker locker(&m_mutex);

    m_triggerLevelBNode->setValue(triggerLevelInmV);
    m_triggerLevelB = triggerLevelInmV;
}

void DRS4SettingsManager::setTriggerDelayInNs(double delayInNs)
{
    QMutexLocker locker(&m_mutex);

    m_triggerDelayInNSNode ->setValue(delayInNs);
    m_triggerDelayInNS = delayInNs;
}

void DRS4SettingsManager::setComment(const QString &comment)
{
    QMutexLocker locker(&m_mutex);

    m_commentNode->setValue(comment);
    m_comment = comment;
}

void DRS4SettingsManager::setLastSaveDate(const QString &lastSaveDate)
{
    QMutexLocker locker(&m_mutex);

    m_lastSaveDateNode->setValue(lastSaveDate);
    m_lastSaveDate = lastSaveDate;
}

void DRS4SettingsManager::setChannelNumberA(int channelNumber)
{
    QMutexLocker locker(&m_mutex);

    m_channelNumberANode->setValue(channelNumber);
    m_channelNumberA = channelNumber;
}

void DRS4SettingsManager::setChannelNumberB(int channelNumber)
{
    QMutexLocker locker(&m_mutex);

    m_channelNumberBNode->setValue(channelNumber);
    m_channelNumberB = channelNumber;
}

void DRS4SettingsManager::setStartChannelA(int min, int max)
{
    QMutexLocker locker(&m_mutex);

    m_startAChannelMin = min;
    m_startAChannelMax = max;

    m_startAChannelMinNode->setValue(min);
    m_startAChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStartChannelB(int min, int max)
{
    QMutexLocker locker(&m_mutex);

    m_startBChannelMin = min;
    m_startBChannelMax = max;

    m_startBChannelMinNode->setValue(min);
    m_startBChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStopChannelA(int min, int max)
{
    QMutexLocker locker(&m_mutex);

    m_stopAChannelMin = min;
    m_stopAChannelMax = max;

    m_stopAChannelMinNode->setValue(min);
    m_stopAChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStopChannelB(int min, int max)
{
    QMutexLocker locker(&m_mutex);

    m_stopBChannelMin = min;
    m_stopBChannelMax = max;

    m_stopBChannelMinNode->setValue(min);
    m_stopBChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStartChannelBMin(int min)
{
    QMutexLocker locker(&m_mutex);

    m_startBChannelMin = min;
    m_startBChannelMinNode->setValue(min);
}

void DRS4SettingsManager::setStartChannelAMax(int max)
{
    QMutexLocker locker(&m_mutex);

    m_startAChannelMax = max;
    m_startAChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStartChannelBMax(int max)
{
    QMutexLocker locker(&m_mutex);

    m_startBChannelMax = max;
    m_startBChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStopChannelAMin(int min)
{
    QMutexLocker locker(&m_mutex);

    m_stopAChannelMin = min;
    m_stopAChannelMinNode->setValue(min);
}

void DRS4SettingsManager::setStopChannelBMin(int min)
{
    QMutexLocker locker(&m_mutex);

    m_stopBChannelMin = min;
    m_stopBChannelMinNode->setValue(min);
}

void DRS4SettingsManager::setStopChannelAMax(int max)
{
    QMutexLocker locker(&m_mutex);

    m_stopAChannelMax = max;
    m_stopAChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStopChannelBMax(int max)
{
    QMutexLocker locker(&m_mutex);

    m_stopBChannelMax = max;
    m_stopBChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setMeanCableDelayInNs(double ns)
{
    QMutexLocker locker(&m_mutex);

    m_meanNs = ns;
    m_meanNSNode->setValue(ns);
}

void DRS4SettingsManager::setFitIterations(int count)
{
    QMutexLocker locker(&m_mutex);

    m_fitIter = count;
    m_fitIterNode->setValue(count);
}

void DRS4SettingsManager::setFitIterationsMerged(int count)
{
    QMutexLocker locker(&m_mutex);

    m_fitIterMerged = count;
    m_fitIterMergedNode->setValue(count);
}

void DRS4SettingsManager::setFitIterationsAB(int count)
{
    QMutexLocker locker(&m_mutex);

    m_fitIterAB = count;
    m_fitIterABNode->setValue(count);
}

void DRS4SettingsManager::setFitIterationsBA(int count)
{
    QMutexLocker locker(&m_mutex);

    m_fitIterBA = count;
    m_fitIterBANode->setValue(count);
}

void DRS4SettingsManager::setSweepInNanoseconds(double ns)
{
    QMutexLocker locker(&m_mutex);

    m_sweepInNanoSec = ns;
    m_sweepInNanoSecNode->setValue(ns);
}

void DRS4SettingsManager::setSampleSpeedInGHz(double ghz)
{
    QMutexLocker locker(&m_mutex);

    m_freqInGHz = ghz;
    m_freqInGHzNode->setValue(ghz);
}

void DRS4SettingsManager::setPulseAreaFilterNormalizationA(double percentage)
{
    QMutexLocker locker(&m_mutex);

    m_pulseAreaNormA = percentage;
    m_pulseAreaNormANode->setValue(percentage);
}

void DRS4SettingsManager::setPulseAreaFilterNormalizationB(double percentage)
{
    QMutexLocker locker(&m_mutex);

    m_pulseAreaNormB = percentage;
    m_pulseAreaNormBNode->setValue(percentage);
}

void DRS4SettingsManager::setPulseAreaFilterBinningA(int binning)
{
    QMutexLocker locker(&m_mutex);

    m_pulseAreaBinningA = binning;
    m_pulseAreaBinningANode->setValue(binning);
}

void DRS4SettingsManager::setPulseAreaFilterBinningB(int binning)
{
    QMutexLocker locker(&m_mutex);

    m_pulseAreaBinningB = binning;
    m_pulseAreaBinningBNode->setValue(binning);
}

void DRS4SettingsManager::setPulseAreaFilterLimitUpperLeftA(double value)
{
    QMutexLocker locker(&m_mutex);

    m_pulseAreaLeftUpper_A = value;
    m_pulseAreaLeftUpper_A_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitUpperRightA(double value)
{
    QMutexLocker locker(&m_mutex);

    m_pulseAreaRightUpper_A = value;
    m_pulseAreaRightUpper_A_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitLowerLeftA(double value)
{
    QMutexLocker locker(&m_mutex);

    m_pulseAreaLeftLower_A = value;
    m_pulseAreaLeftLower_A_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitLowerRightA(double value)
{
    QMutexLocker locker(&m_mutex);

    m_pulseAreaRightLower_A = value;
    m_pulseAreaRightLower_A_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitUpperLeftB(double value)
{
    QMutexLocker locker(&m_mutex);

    m_pulseAreaLeftUpper_B = value;
    m_pulseAreaLeftUpper_B_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitUpperRightB(double value)
{
    QMutexLocker locker(&m_mutex);

    m_pulseAreaRightUpper_B = value;
    m_pulseAreaRightUpper_B_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitLowerLeftB(double value)
{
    QMutexLocker locker(&m_mutex);

    m_pulseAreaLeftLower_B = value;
    m_pulseAreaLeftLower_B_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitLowerRightB(double value)
{
    QMutexLocker locker(&m_mutex);

    m_pulseAreaRightLower_B = value;
    m_pulseAreaRightLower_B_Node->setValue(value);
}

void DRS4SettingsManager::setPersistanceEnabled(bool activated)
{
    QMutexLocker locker(&m_mutex);

    m_persistanceEnabled = activated;
    m_persistanceEnabled_Node->setValue(activated);
}

void DRS4SettingsManager::setPersistanceLeftInNsOfA(double value)
{
    QMutexLocker locker(&m_mutex);

    m_persistance_leftAInNs = value;
    m_persistance_leftAInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPersistanceLeftInNsOfB(double value)
{
    QMutexLocker locker(&m_mutex);

    m_persistance_leftBInNs = value;
    m_persistance_leftBInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPersistanceRightInNsOfA(double value)
{
    QMutexLocker locker(&m_mutex);

    m_persistance_rightAInNs = value;
    m_persistance_rightAInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPersistanceRightInNsOfB(double value)
{
    QMutexLocker locker(&m_mutex);

    m_persistance_rightBInNs = value;
    m_persistance_rightBInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPersistanceUsingCFDBAsRefForA(bool on)
{
    QMutexLocker locker(&m_mutex);

    m_persistanceUsingRefB_A_Node->setValue(on);
}

void DRS4SettingsManager::setPersistanceUsingCFDAAsRefForB(bool on)
{
    QMutexLocker locker(&m_mutex);

    m_persistanceUsingRefA_B_Node->setValue(on);
}

void DRS4SettingsManager::setInterpolationType(const DRS4InterpolationType::type &type)
{
    QMutexLocker locker(&m_mutex);

    m_cfdAlgorithmTypeNode->setValue(type);
}

void DRS4SettingsManager::setSplineInterpolationType(const DRS4SplineInterpolationType::type &type)
{
    QMutexLocker locker(&m_mutex);

    m_splineTypeLinearEnabledNode->setValue(false);
    m_splineTypeCubicEnabledNode->setValue(false);
    m_splineTypeAkimaEnabledNode->setValue(false);
    m_splineTypeCatmullRomEnabledNode->setValue(false);
    m_splineTypeMonotoneEnabledNode->setValue(false);
    m_splineTypeTKCubicEnabledNode->setValue(false);

    switch (type) {
    case DRS4SplineInterpolationType::type::linear: {
        m_splineTypeLinearEnabledNode->setValue(true);
    }
        break;
    case DRS4SplineInterpolationType::type::cubic: {
        m_splineTypeCubicEnabledNode->setValue(true);
    }
        break;
    case DRS4SplineInterpolationType::type::akima: {
        m_splineTypeAkimaEnabledNode->setValue(true);
    }
        break;
    case DRS4SplineInterpolationType::type::catmullRom: {
        m_splineTypeCatmullRomEnabledNode->setValue(true);
    }
        break;
    case DRS4SplineInterpolationType::type::monotone: {
        m_splineTypeMonotoneEnabledNode->setValue(true);
    }
        break;
    case DRS4SplineInterpolationType::type::tk_cubic: {
        m_splineTypeTKCubicEnabledNode->setValue(true);
    }
        break;
    default: {
        m_splineTypeLinearEnabledNode->setValue(true);
    }
    }
}

void DRS4SettingsManager::setSplineIntraSamplingCounts(int counts)
{
    QMutexLocker locker(&m_mutex);

    m_splineIntraSamplingPointsNode->setValue(counts);
}

void DRS4SettingsManager::setPolynomialSamplingCounts(int counts)
{
    QMutexLocker locker(&m_mutex);

    m_polynomialIntraSamplingPointsNode->setValue(counts);
}

void DRS4SettingsManager::setMedianFilterAEnabled(bool enabled)
{
    QMutexLocker locker(&m_mutex);

    m_medianFilterActivated_A_Node->setValue(enabled);
}

void DRS4SettingsManager::setMedianFilterBEnabled(bool enabled)
{
    QMutexLocker locker(&m_mutex);

    m_medianFilterActivated_B_Node->setValue(enabled);
}

void DRS4SettingsManager::setMedianFilterWindowSizeA(int size)
{
    QMutexLocker locker(&m_mutex);

    m_medianFilterWindowSize_A_Node->setValue(size);
}

void DRS4SettingsManager::setMedianFilterWindowSizeB(int size)
{
    QMutexLocker locker(&m_mutex);

    m_medianFilterWindowSize_B_Node->setValue(size);
}

bool DRS4SettingsManager::medianFilterAEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_medianFilterActivated_A_Node->getValue().toBool();
}

bool DRS4SettingsManager::medianFilterBEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_medianFilterActivated_B_Node->getValue().toBool();
}

int DRS4SettingsManager::medianFilterWindowSizeA() const
{
    QMutexLocker locker(&m_mutex);

    return m_medianFilterWindowSize_A_Node->getValue().toInt();
}

int DRS4SettingsManager::medianFilterWindowSizeB() const
{
    QMutexLocker locker(&m_mutex);

    return m_medianFilterWindowSize_B_Node->getValue().toInt();
}

DRS4InterpolationType::type DRS4SettingsManager::interpolationType() const
{
    QMutexLocker locker(&m_mutex);

    bool ok = false;
    DRS4InterpolationType::type type = (DRS4InterpolationType::type)m_cfdAlgorithmTypeNode->getValue().toInt(&ok);

    if (!ok)
        type = DRS4InterpolationType::type::spline;

    return type;
}

DRS4SplineInterpolationType::type DRS4SettingsManager::splineInterpolationType() const
{
    QMutexLocker locker(&m_mutex);

    if ( m_splineTypeLinearEnabledNode->getValue().toBool() )
        return DRS4SplineInterpolationType::type::linear;
    else if ( m_splineTypeCubicEnabledNode->getValue().toBool() )
        return DRS4SplineInterpolationType::type::cubic;
    else if ( m_splineTypeAkimaEnabledNode->getValue().toBool() )
        return DRS4SplineInterpolationType::type::akima;
    else if ( m_splineTypeCatmullRomEnabledNode->getValue().toBool() )
        return DRS4SplineInterpolationType::type::catmullRom;
    else if ( m_splineTypeMonotoneEnabledNode->getValue().toBool() )
        return DRS4SplineInterpolationType::type::monotone;
    else if ( m_splineTypeTKCubicEnabledNode->getValue().toBool() )
        return DRS4SplineInterpolationType::type::tk_cubic;

    return DRS4SplineInterpolationType::type::cubic;
}

int DRS4SettingsManager::splineIntraSamplingCounts() const
{
    QMutexLocker locker(&m_mutex);

    bool ok = false;
    int points = m_splineIntraSamplingPointsNode->getValue().toInt(&ok);

    if (!ok)
        points = 10;

    return points;
}

int DRS4SettingsManager::polynomialSamplingCounts() const
{
    QMutexLocker locker(&m_mutex);

    bool ok = false;
    int points = m_polynomialIntraSamplingPointsNode->getValue().toInt(&ok);

    if (!ok)
        points = 10;

    return points;
}

bool DRS4SettingsManager::isPersistanceEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_persistanceEnabled_Node->getValue().toBool();
}

double DRS4SettingsManager::persistanceLeftInNsOfA() const
{
    QMutexLocker locker(&m_mutex);

    return m_persistance_leftAInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::persistanceLeftInNsOfB() const
{
    QMutexLocker locker(&m_mutex);

    return m_persistance_leftBInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::persistanceRightInNsOfA() const
{
    QMutexLocker locker(&m_mutex);

    return m_persistance_rightAInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::persistanceRightInNsOfB() const
{
    QMutexLocker locker(&m_mutex);

    return m_persistance_rightBInNs_Node->getValue().toDouble();
}

bool DRS4SettingsManager::persistanceUsingCFDBAsRefForA() const
{
    QMutexLocker locker(&m_mutex);

    return m_persistanceUsingRefB_A_Node->getValue().toBool();
}

bool DRS4SettingsManager::persistanceUsingCFDAAsRefForB() const
{
    QMutexLocker locker(&m_mutex);

    return m_persistanceUsingRefA_B_Node->getValue().toBool();
}

bool DRS4SettingsManager::isPulseAreaFilterEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaFilerEnabledNode->getValue().toBool();
}

bool DRS4SettingsManager::isPulseAreaFilterPlotEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaFilerEnabledPlotNode->getValue().toBool();
}

double DRS4SettingsManager::pulseAreaFilterNormalizationA() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaNormANode->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterNormalizationB() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaNormBNode->getValue().toDouble();
}

int DRS4SettingsManager::pulseAreaFilterBinningA() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaBinningANode->getValue().toInt();
}

int DRS4SettingsManager::pulseAreaFilterBinningB() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaBinningBNode->getValue().toInt();
}

double DRS4SettingsManager::pulseAreaFilterLimitUpperLeftA() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaLeftUpper_A_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitUpperRightA() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaRightUpper_A_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitLowerLeftA() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaLeftLower_A_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitLowerRightA() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaRightLower_A_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitUpperLeftB() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaLeftUpper_B_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitUpperRightB() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaRightUpper_B_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitLowerLeftB() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaLeftLower_B_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitLowerRightB() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAreaRightLower_B_Node->getValue().toDouble();
}

void DRS4SettingsManager::setPulseAreaFilterEnabled(bool enabled)
{
    QMutexLocker locker(&m_mutex);

    m_bPulseAreaFilterEnabled = enabled;
    m_pulseAreaFilerEnabledNode->setValue(enabled);
}

void DRS4SettingsManager::setPulseAreaFilterPlotEnabled(bool enabled)
{
    QMutexLocker locker(&m_mutex);

    m_bPulseAreaFilterEnabledPlot = enabled;
    m_pulseAreaFilerEnabledPlotNode->setValue(enabled);
}

void DRS4SettingsManager::setStartChannelAMin(int min)
{
    QMutexLocker locker(&m_mutex);

    m_startAChannelMin = min;
    m_startAChannelMinNode->setValue(min);
}

bool DRS4SettingsManager::isPositiveSignal() const
{
    QMutexLocker locker(&m_mutex);

    return m_isPositivSignalNode->getValue().toBool();
}

bool DRS4SettingsManager::isNegativeLTAccepted() const
{
    QMutexLocker locker(&m_mutex);

    return m_negativeLifetimesNode->getValue().toBool();
}

bool DRS4SettingsManager::ignoreBusyState() const
{
    QMutexLocker locker(&m_mutex);

    return m_ignoreBusyNode->getValue().toBool();
}

double DRS4SettingsManager::cfdLevelA() const
{
    QMutexLocker locker(&m_mutex);

    return m_cfdLevelANode->getValue().toDouble();
}

double DRS4SettingsManager::cfdLevelB() const
{
    QMutexLocker locker(&m_mutex);

    return m_cfdLevelBNode->getValue().toDouble();
}

int DRS4SettingsManager::startCell() const
{
    QMutexLocker locker(&m_mutex);

    return m_startCellNode->getValue().toInt();
}

int DRS4SettingsManager::stopCell() const
{
    QMutexLocker locker(&m_mutex);

    return m_stopCellNode->getValue().toInt();
}

bool DRS4SettingsManager::isforceCoincidence() const
{
    QMutexLocker locker(&m_mutex);

    return m_bForceCoincidenceNode->getValue().toBool();
}

bool DRS4SettingsManager::isBurstMode() const
{
    QMutexLocker locker(&m_mutex);

    return m_burstModeNode->getValue().toBool();
}

double DRS4SettingsManager::offsetInNSAB() const
{
    QMutexLocker locker(&m_mutex);

    return m_offsetNsABNode->getValue().toDouble();
}

double DRS4SettingsManager::offsetInNSBA() const
{
    QMutexLocker locker(&m_mutex);

    return m_offsetNsBANode->getValue().toDouble();
}

double DRS4SettingsManager::offsetInNSCoincidence() const
{
    QMutexLocker locker(&m_mutex);

    return m_offsetNsCoincidenceNode->getValue().toDouble();
}

double DRS4SettingsManager::offsetInNSMerged() const
{
    QMutexLocker locker(&m_mutex);

    return m_offsetNsMergedNode->getValue().toDouble();
}

double DRS4SettingsManager::scalerInNSAB() const
{
    QMutexLocker locker(&m_mutex);

    return m_scalerNsABNode->getValue().toDouble();
}

double DRS4SettingsManager::scalerInNSBA() const
{
    QMutexLocker locker(&m_mutex);

    return m_scalerNsBANode->getValue().toDouble();
}

double DRS4SettingsManager::scalerInNSCoincidence() const
{
    QMutexLocker locker(&m_mutex);

    return m_scalerNsCoincidenceNode->getValue().toDouble();
}

double DRS4SettingsManager::scalerInNSMerged() const
{
    QMutexLocker locker(&m_mutex);

    return m_scalerNsMergedNode->getValue().toDouble();
}

int DRS4SettingsManager::channelCntAB() const
{
    QMutexLocker locker(&m_mutex);

    return m_channelCountABNode->getValue().toInt();
}

int DRS4SettingsManager::channelCntBA() const
{
    QMutexLocker locker(&m_mutex);

    return m_channelCountBANode->getValue().toInt();
}

int DRS4SettingsManager::channelCntCoincindence() const
{
    QMutexLocker locker(&m_mutex);

    return m_channelCountCoincidenceNode->getValue().toInt();
}

int DRS4SettingsManager::channelCntMerged() const
{
    QMutexLocker locker(&m_mutex);

    return m_channelCountMergedNode->getValue().toInt();
}

int DRS4SettingsManager::triggerSource() const
{
    QMutexLocker locker(&m_mutex);

    return m_triggerSourceNode->getValue().toInt();
}

bool DRS4SettingsManager::isTriggerPolarityPositive() const
{
    QMutexLocker locker(&m_mutex);

    return m_isTriggerPolarityPositiveNode->getValue().toBool();
}

double DRS4SettingsManager::triggerLevelAmV() const
{
    QMutexLocker locker(&m_mutex);

    return m_triggerLevelANode->getValue().toDouble();
}

double DRS4SettingsManager::triggerLevelBmV() const
{
    QMutexLocker locker(&m_mutex);

    return m_triggerLevelBNode->getValue().toDouble();
}

double DRS4SettingsManager::triggerDelayInNs() const
{
    QMutexLocker locker(&m_mutex);

    return m_triggerDelayInNSNode->getValue().toDouble();
}

QString DRS4SettingsManager::comment() const
{
    QMutexLocker locker(&m_mutex);

    return m_commentNode->getValue().toString();
}

QString DRS4SettingsManager::lastSaveDate() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastSaveDateNode->getValue().toString();
}

int DRS4SettingsManager::channelNumberA() const
{
    QMutexLocker locker(&m_mutex);

    return m_channelNumberANode->getValue().toInt();
}

int DRS4SettingsManager::channelNumberB() const
{
    QMutexLocker locker(&m_mutex);

    return m_channelNumberBNode->getValue().toInt();
}

int DRS4SettingsManager::startChanneAMin() const
{
    QMutexLocker locker(&m_mutex);

    return m_startAChannelMinNode->getValue().toInt();
}

int DRS4SettingsManager::startChanneAMax() const
{
    QMutexLocker locker(&m_mutex);

    return m_startAChannelMaxNode->getValue().toInt();
}

int DRS4SettingsManager::startChanneBMin() const
{
    QMutexLocker locker(&m_mutex);

    return m_startBChannelMinNode->getValue().toInt();
}

int DRS4SettingsManager::startChanneBMax() const
{
    QMutexLocker locker(&m_mutex);

    return m_startBChannelMaxNode->getValue().toInt();
}

int DRS4SettingsManager::stopChanneAMin() const
{
    QMutexLocker locker(&m_mutex);

    return m_stopAChannelMinNode->getValue().toInt();
}

int DRS4SettingsManager::stopChanneAMax() const
{
    QMutexLocker locker(&m_mutex);

    return m_stopAChannelMaxNode->getValue().toInt();
}

int DRS4SettingsManager::stopChanneBMin() const
{
    QMutexLocker locker(&m_mutex);

    return m_stopBChannelMinNode->getValue().toInt();
}

int DRS4SettingsManager::stopChanneBMax() const
{
    QMutexLocker locker(&m_mutex);

    return m_stopBChannelMaxNode->getValue().toInt();
}

double DRS4SettingsManager::meanCableDelay() const
{
    QMutexLocker locker(&m_mutex);

    return m_meanNSNode->getValue().toDouble();
}

int DRS4SettingsManager::fitIterations() const
{
    QMutexLocker locker(&m_mutex);

    return m_fitIterNode->getValue().toInt();
}

int DRS4SettingsManager::fitIterationsMerged() const
{
    QMutexLocker locker(&m_mutex);

    return m_fitIterMergedNode->getValue().toInt();
}

int DRS4SettingsManager::fitIterationsAB() const
{
    QMutexLocker locker(&m_mutex);

    return m_fitIterABNode->getValue().toInt();
}

int DRS4SettingsManager::fitIterationsBA() const
{
    QMutexLocker locker(&m_mutex);

    return m_fitIterBANode->getValue().toInt();
}

double DRS4SettingsManager::sweepInNanoseconds() const
{
    QMutexLocker locker(&m_mutex);

    return m_sweepInNanoSecNode->getValue().toDouble();
}

double DRS4SettingsManager::sampleSpeedInGHz() const
{
    QMutexLocker locker(&m_mutex);

    return m_freqInGHzNode->getValue().toDouble();
}

QString DRS4SettingsManager::fileName() const
{
    QMutexLocker locker(&m_mutex);

    return m_fileName;
}

QString DRS4SettingsManager::xmlContent() const
{
    QMutexLocker locker(&m_mutex);

    if (m_parentNode)
        return (QString)DSimpleXMLString(m_parentNode);

        return QString("");
}
