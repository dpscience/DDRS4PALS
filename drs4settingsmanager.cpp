/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2021 Danny Petschke
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

#include "drs4settingsmanager.h"

DRS4PulseShapeFilterData& DRS4PulseShapeFilterData::operator=(const DRS4PulseShapeFilterData &copy)
{
    m_meanTrace = copy.mean();
    m_stdDevTrace = copy.stddev();

    m_meanTraceDataX = copy.m_meanTraceDataX;
    m_meanTraceDataY = copy.m_meanTraceDataY;

    m_stdDevTraceDataX = copy.m_stdDevTraceDataX;
    m_stdDevTraceDataY = copy.m_stdDevTraceDataY;

    m_meanTraceSpline = copy.m_meanTraceSpline;
    m_stddevTraceSpline = copy.m_stddevTraceSpline;

    if (m_meanTraceDataXArray) {
        delete [] m_meanTraceDataXArray;
        m_meanTraceDataXArray = DNULLPTR;
    }

    if (m_meanTraceDataYArray) {
        delete [] m_meanTraceDataYArray;
        m_meanTraceDataYArray = DNULLPTR;
    }

    if (m_stdDevTraceDataXArray) {
        delete [] m_stdDevTraceDataXArray;
        m_stdDevTraceDataXArray = DNULLPTR;
    }

    if (m_stdDevTraceDataYArray) {
        delete [] m_stdDevTraceDataYArray;
        m_stdDevTraceDataYArray = DNULLPTR;
    }

    m_meanTraceDataXArray = new float[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];
    m_meanTraceDataYArray = new float[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];
    m_stdDevTraceDataXArray = new float[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];
    m_stdDevTraceDataYArray = new float[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];

    std::fill(m_meanTraceDataXArray, m_meanTraceDataXArray + sizeof(m_meanTraceDataXArray)*m_sizeOfFloat, 0);
    std::fill(m_meanTraceDataYArray, m_meanTraceDataYArray + sizeof(m_meanTraceDataYArray)*m_sizeOfFloat, 0);
    std::fill(m_stdDevTraceDataXArray, m_stdDevTraceDataXArray + sizeof(m_stdDevTraceDataXArray)*m_sizeOfFloat, 0);
    std::fill(m_stdDevTraceDataYArray, m_stdDevTraceDataYArray + sizeof(m_stdDevTraceDataYArray)*m_sizeOfFloat, 0);

    if (copy.m_meanTraceDataXArray)
        memcpy(m_meanTraceDataXArray, copy.m_meanTraceDataXArray, __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER*sizeof(float));

    if (copy.m_meanTraceDataYArray)
        memcpy(m_meanTraceDataYArray, copy.m_meanTraceDataYArray, __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER*sizeof(float));

    if (copy.m_stdDevTraceDataXArray)
        memcpy(m_stdDevTraceDataXArray, copy.m_stdDevTraceDataXArray, __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER*sizeof(float));

    if (copy.m_stdDevTraceDataYArray)
        memcpy(m_stdDevTraceDataYArray, copy.m_stdDevTraceDataYArray, __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER*sizeof(float));

    return *this;
}

void DRS4PulseShapeFilterData::setData(const QVector<QPointF> &mean, const QVector<QPointF> &stddev)
{
    setMeanTrace(mean);
    setStddevTrace(stddev);

    const int size = mean.size();

    m_meanTraceDataX.resize(size);
    m_meanTraceDataY.resize(size);

    m_stdDevTraceDataX.resize(size);
    m_stdDevTraceDataY.resize(size);

    int cnt = 0;

    if (m_meanTraceDataXArray) {
        delete [] m_meanTraceDataXArray;
        m_meanTraceDataXArray = DNULLPTR;
    }

    if (m_meanTraceDataYArray) {
        delete [] m_meanTraceDataYArray;
        m_meanTraceDataYArray = DNULLPTR;
    }

    if (m_stdDevTraceDataXArray) {
        delete [] m_stdDevTraceDataXArray;
        m_stdDevTraceDataXArray = DNULLPTR;
    }

    if (m_stdDevTraceDataYArray) {
        delete [] m_stdDevTraceDataYArray;
        m_stdDevTraceDataYArray = DNULLPTR;
    }

    m_meanTraceDataXArray = new float[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];
    m_meanTraceDataYArray = new float[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];
    m_stdDevTraceDataXArray = new float[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];
    m_stdDevTraceDataYArray = new float[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];

    std::fill(m_meanTraceDataXArray, m_meanTraceDataXArray + sizeof(m_meanTraceDataXArray)*m_sizeOfFloat, 0);
    std::fill(m_meanTraceDataYArray, m_meanTraceDataYArray + sizeof(m_meanTraceDataYArray)*m_sizeOfFloat, 0);
    std::fill(m_stdDevTraceDataXArray, m_stdDevTraceDataXArray + sizeof(m_stdDevTraceDataXArray)*m_sizeOfFloat, 0);
    std::fill(m_stdDevTraceDataYArray, m_stdDevTraceDataYArray + sizeof(m_stdDevTraceDataYArray)*m_sizeOfFloat, 0);

    for (QPointF p : m_meanTrace) {
        m_meanTraceDataX[cnt] = p.x();
        m_meanTraceDataY[cnt] = p.y();

        m_stdDevTraceDataX[cnt] = p.x();
        m_stdDevTraceDataY[cnt] = m_stdDevTrace.at(cnt).y();

        m_meanTraceDataXArray[cnt] = p.x();
        m_meanTraceDataYArray[cnt] = p.y();

        m_stdDevTraceDataXArray[cnt] = p.x();
        m_stdDevTraceDataYArray[cnt] = m_stdDevTrace.at(cnt).y();

        cnt ++;
    }

    m_meanTraceSpline.setType(SplineType::Cubic);
    m_stddevTraceSpline.setType(SplineType::Cubic);

    m_meanTraceSpline.setPoints(m_meanTraceDataX, m_meanTraceDataY);
    m_stddevTraceSpline.setPoints(m_stdDevTraceDataX, m_stdDevTraceDataY);
}

bool DRS4PulseShapeFilterData::isInsideBounding(const double &x, const double &y, const double &lowerFraction, const double& upperFraction) const
{
    if (m_meanTrace.isEmpty() || m_stdDevTrace.isEmpty())
        return false;

    const double mean = m_meanTraceSpline(x);
    const double val = m_stddevTraceSpline(x);

    const double stddevUpper = upperFraction*val;
    const double stddevLower = lowerFraction*val;

    const double upperLimit = (mean + stddevUpper);
    const double lowerLimit = (mean - stddevLower);

    return (y <= upperLimit) && (y >= lowerLimit);
}

static DRS4SettingsManager *__sharedInstanceSettingsManager = DNULLPTR;

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
    m_triggerSource(2), //AND logic
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
    m_persistanceEnabled(false),
    m_riseTimeFilterIsPlotEnabled(false),
    m_riseTimeFilterIsActivated(false),
    m_riseTimeFilter_scaleInNs_A(5.0f),
    m_riseTimeFilter_scaleInNs_B(5.0f),
    m_riseTimeFilter_scaleBinningCnt_A(1000),
    m_riseTimeFilter_scaleBinningCnt_B(1000),
    m_riseTimeFilter_leftWindow_A(20),
    m_riseTimeFilter_leftWindow_B(20),
    m_riseTimeFilter_rightWindow_A(980),
    m_riseTimeFilter_rightWindow_B(980),
    m_pulseShapeFilter_numberOfPulsesAcq_A(10000),
    m_pulseShapeFilter_numberOfPulsesAcq_B(10000),
    m_pulseShapeFilter_leftAInNs(2.0f),
    m_pulseShapeFilter_leftBInNs(2.0f),
    m_pulseShapeFilter_rightAInNs(2.0f),
    m_pulseShapeFilter_rightBInNs(2.0f),
    m_pulseShapeFilter_ROIleftAInNs(1.2f),
    m_pulseShapeFilter_ROIleftBInNs(1.2f),
    m_pulseShapeFilter_ROIrightAInNs(1.2f),
    m_pulseShapeFilter_ROIrightBInNs(1.2f),
    m_pulseShapeFilter_StdDevUpperFractA(3.0f),
    m_pulseShapeFilter_StdDevLowerFractA(3.0f),
    m_pulseShapeFilter_StdDevUpperFractB(3.0f),
    m_pulseShapeFilter_StdDevLowerFractB(3.0f),
    m_pulseShapeFilterRecordScheme(DRS4PulseShapeFilterRecordScheme::Scheme::RC_AB),
    m_pulseShapeFilterEnabledA(false),
    m_pulseShapeFilterEnabledB(false),
    m_baseLineCorrectionStartCellA(5),
    m_baseLineCorrectionRegionA(25),
    m_baseLineCorrectionShiftValueA(0.0),
    m_baseLineCorrectionEnabledA(false),
    m_baseLineCorrectionLimitA(3.0),
    m_baseLineCorrectionLimitExceededRejectA(false),
    m_baseLineCorrectionStartCellB(5),
    m_baseLineCorrectionRegionB(25),
    m_baseLineCorrectionShiftValueB(0.0),
    m_baseLineCorrectionEnabledB(false),
    m_baseLineCorrectionLimitB(3.0),
    m_baseLineCorrectionLimitExceededRejectB(false)
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
    m_riseTimeFilterSettingsNode = new DSimpleXMLNode("rise-time-filter-settings");
    m_pulseShapeFilterSettingsNode = new DSimpleXMLNode("pulse-shape-filter-settings");
    m_baseLineFilterSettingsNode = new DSimpleXMLNode("baseline-jitter-correction-settings");
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

    m_riseTimeFilterIsPlotEnabled_Node = new DSimpleXMLNode("rise-time-filter-plot?");
    m_riseTimeFilterIsPlotEnabled_Node->setValue(m_riseTimeFilterIsPlotEnabled);
    m_riseTimeFilterIsActivated_Node = new DSimpleXMLNode("rise-time-filter?");
    m_riseTimeFilterIsActivated_Node->setValue(m_riseTimeFilterIsActivated);

    m_riseTimeFilter_scaleInNs_A_Node = new DSimpleXMLNode("rise-time-filter-scale-in-ns-A");
    m_riseTimeFilter_scaleInNs_A_Node->setValue(m_riseTimeFilter_scaleInNs_A);
    m_riseTimeFilter_scaleInNs_B_Node = new DSimpleXMLNode("rise-time-filter-scale-in-ns-B");
    m_riseTimeFilter_scaleInNs_B_Node->setValue(m_riseTimeFilter_scaleInNs_B);

    m_riseTimeFilter_scaleBinningCnt_A_Node = new DSimpleXMLNode("rise-time-filter-number-of-bins-A");
    m_riseTimeFilter_scaleBinningCnt_A_Node->setValue(m_riseTimeFilter_scaleBinningCnt_A);
    m_riseTimeFilter_scaleBinningCnt_B_Node = new DSimpleXMLNode("rise-time-filter-number-of-bins-B");
    m_riseTimeFilter_scaleBinningCnt_B_Node->setValue(m_riseTimeFilter_scaleBinningCnt_B);

    m_riseTimeFilter_leftWindow_A_Node = new DSimpleXMLNode("rise-time-filter-left-bin-A");
    m_riseTimeFilter_leftWindow_A_Node->setValue(m_riseTimeFilter_leftWindow_A);
    m_riseTimeFilter_leftWindow_B_Node = new DSimpleXMLNode("rise-time-filter-left-bin-B");
    m_riseTimeFilter_leftWindow_B_Node->setValue(m_riseTimeFilter_leftWindow_B);

    m_riseTimeFilter_rightWindow_A_Node = new DSimpleXMLNode("rise-time-filter-right-bin-A");
    m_riseTimeFilter_rightWindow_A_Node->setValue(m_riseTimeFilter_rightWindow_A);
    m_riseTimeFilter_rightWindow_B_Node = new DSimpleXMLNode("rise-time-filter-right-bin-B");
    m_riseTimeFilter_rightWindow_B_Node->setValue(m_riseTimeFilter_rightWindow_B);

    m_pulseShapeFilterEnabledA_Node = new DSimpleXMLNode("pulse-shape-filter-A-enabled?");
    m_pulseShapeFilterEnabledA_Node->setValue(m_pulseShapeFilterEnabledA);

    m_pulseShapeFilterEnabledB_Node = new DSimpleXMLNode("pulse-shape-filter-B-enabled?");
    m_pulseShapeFilterEnabledB_Node->setValue(m_pulseShapeFilterEnabledB);

    m_pulseShapeFilterRecordScheme_Node = new DSimpleXMLNode("pulse-shape-filter-record-scheme");
    m_pulseShapeFilterRecordScheme_Node->setValue(m_pulseShapeFilterRecordScheme);

    m_pulseShapeFilter_numberOfPulsesAcq_A_Node = new DSimpleXMLNode("pulse-shape-filter-number-of-pulses-to-be-recorded-A");
    m_pulseShapeFilter_numberOfPulsesAcq_A_Node->setValue(m_pulseShapeFilter_numberOfPulsesAcq_A);
    m_pulseShapeFilter_numberOfPulsesAcq_B_Node = new DSimpleXMLNode("pulse-shape-filter-number-of-pulses-to-be-recorded-B");
    m_pulseShapeFilter_numberOfPulsesAcq_B_Node->setValue(m_pulseShapeFilter_numberOfPulsesAcq_B);

    m_pulseShapeFilter_leftAInNs_Node = new DSimpleXMLNode("pulse-shape-filter-plot-left-of-CFD-in-ns-A");
    m_pulseShapeFilter_leftAInNs_Node->setValue(m_pulseShapeFilter_leftAInNs);

    m_pulseShapeFilter_leftBInNs_Node = new DSimpleXMLNode("pulse-shape-filter-plot-left-of-CFD-in-ns-B");
    m_pulseShapeFilter_leftBInNs_Node->setValue(m_pulseShapeFilter_leftBInNs);

    m_pulseShapeFilter_rightAInNs_Node = new DSimpleXMLNode("pulse-shape-filter-plot-right-of-CFD-in-ns-A");
    m_pulseShapeFilter_rightAInNs_Node->setValue(m_pulseShapeFilter_rightAInNs);

    m_pulseShapeFilter_rightBInNs_Node = new DSimpleXMLNode("pulse-shape-filter-plot-right-of-CFD-in-ns-B");
    m_pulseShapeFilter_rightBInNs_Node->setValue(m_pulseShapeFilter_rightBInNs);

    m_pulseShapeFilter_ROIleftAInNs_Node = new DSimpleXMLNode("pulse-shape-filter-ROI-left-of-CFD-in-ns-A");
    m_pulseShapeFilter_ROIleftAInNs_Node->setValue(m_pulseShapeFilter_ROIleftAInNs);

    m_pulseShapeFilter_ROIleftBInNs_Node = new DSimpleXMLNode("pulse-shape-filter-ROI-left-of-CFD-in-ns-B");
    m_pulseShapeFilter_ROIleftBInNs_Node->setValue(m_pulseShapeFilter_ROIleftBInNs);

    m_pulseShapeFilter_ROIrightAInNs_Node = new DSimpleXMLNode("pulse-shape-filter-ROI-right-of-CFD-in-ns-A");
    m_pulseShapeFilter_ROIrightAInNs_Node->setValue(m_pulseShapeFilter_ROIrightAInNs);

    m_pulseShapeFilter_ROIrightBInNs_Node = new DSimpleXMLNode("pulse-shape-filter-ROI-right-of-CFD-in-ns-B");
    m_pulseShapeFilter_ROIrightBInNs_Node->setValue(m_pulseShapeFilter_ROIrightBInNs);

    m_pulseShapeFilter_StdDevUpperFractA_Node = new DSimpleXMLNode("pulse-shape-filter-upper-level-stddev-fraction-A");
    m_pulseShapeFilter_StdDevUpperFractA_Node->setValue(m_pulseShapeFilter_StdDevUpperFractA);
    m_pulseShapeFilter_StdDevUpperFractB_Node = new DSimpleXMLNode("pulse-shape-filter-upper-level-stddev-fraction-B");
    m_pulseShapeFilter_StdDevUpperFractB_Node->setValue(m_pulseShapeFilter_StdDevUpperFractB);

    m_pulseShapeFilter_StdDevLowerFractA_Node = new DSimpleXMLNode("pulse-shape-filter-lower-level-stddev-fraction-A");
    m_pulseShapeFilter_StdDevLowerFractA_Node->setValue(m_pulseShapeFilter_StdDevLowerFractA);
    m_pulseShapeFilter_StdDevLowerFractB_Node = new DSimpleXMLNode("pulse-shape-filter-lower-level-stddev-fraction-B");
    m_pulseShapeFilter_StdDevLowerFractB_Node->setValue(m_pulseShapeFilter_StdDevLowerFractB);

    m_pulseShapeFilter_contentDataA_Node = new DSimpleXMLNode("pulse-shape-filter-content-A");
    m_pulseShapeFilter_contentDataB_Node = new DSimpleXMLNode("pulse-shape-filter-content-B");

    m_pulseShapeFilter_meanDataA_Node = new DSimpleXMLNode("pulse-shape-filter-content-mean-trace-A");
    m_pulseShapeFilter_meanDataA_Node->setValue("");

    m_pulseShapeFilter_stddevDataA_Node = new DSimpleXMLNode("pulse-shape-filter-content-stddev-trace-A");
    m_pulseShapeFilter_stddevDataA_Node->setValue("");

    m_pulseShapeFilter_meanDataB_Node = new DSimpleXMLNode("pulse-shape-filter-content-mean-trace-B");
    m_pulseShapeFilter_meanDataB_Node->setValue("");

    m_pulseShapeFilter_stddevDataB_Node = new DSimpleXMLNode("pulse-shape-filter-content-stddev-trace-B");
    m_pulseShapeFilter_stddevDataB_Node->setValue("");

    m_baseLineCorrectionStartCellA_Node = new DSimpleXMLNode("baseline-jitter-correction-start-cell-A");
    m_baseLineCorrectionStartCellA_Node->setValue(m_baseLineCorrectionStartCellA);
    m_baseLineCorrectionRegionA_Node = new DSimpleXMLNode("baseline-jitter-correction-cell-region-A");
    m_baseLineCorrectionRegionA_Node->setValue(m_baseLineCorrectionRegionA);
    m_baseLineCorrectionShiftValueA_Node = new DSimpleXMLNode("baseline-jitter-correction-baseline-value-in-mV-A");
    m_baseLineCorrectionShiftValueA_Node->setValue(m_baseLineCorrectionShiftValueA);
    m_baseLineCorrectionEnabledA_Node = new DSimpleXMLNode("baseline-jitter-correction-enabled-A?");
    m_baseLineCorrectionEnabledA_Node->setValue(m_baseLineCorrectionEnabledA);
    m_baseLineCorrectionLimitA_Node = new DSimpleXMLNode("baseline-filter-rejection-limit-in-%-A");
    m_baseLineCorrectionLimitA_Node->setValue(m_baseLineCorrectionLimitA);
    m_baseLineCorrectionLimitExceededRejectA_Node = new DSimpleXMLNode("baseline-filter-enabled-A?");
    m_baseLineCorrectionLimitExceededRejectA_Node->setValue(m_baseLineCorrectionLimitExceededRejectA);

    m_baseLineCorrectionStartCellB_Node = new DSimpleXMLNode("baseline-jitter-correction-start-cell-B");
    m_baseLineCorrectionStartCellB_Node->setValue(m_baseLineCorrectionStartCellB);
    m_baseLineCorrectionRegionB_Node = new DSimpleXMLNode("baseline-jitter-correction-cell-region-B");
    m_baseLineCorrectionRegionB_Node->setValue(m_baseLineCorrectionRegionB);
    m_baseLineCorrectionShiftValueB_Node = new DSimpleXMLNode("baseline-jitter-correction-baseline-value-in-mV-B");
    m_baseLineCorrectionShiftValueB_Node->setValue(m_baseLineCorrectionShiftValueB);
    m_baseLineCorrectionEnabledB_Node = new DSimpleXMLNode("baseline-jitter-correction-enabled-B?");
    m_baseLineCorrectionEnabledB_Node->setValue(m_baseLineCorrectionEnabledB);
    m_baseLineCorrectionLimitB_Node = new DSimpleXMLNode("baseline-filter-rejection-limit-in-%-B");
    m_baseLineCorrectionLimitB_Node->setValue(m_baseLineCorrectionLimitB);
    m_baseLineCorrectionLimitExceededRejectB_Node = new DSimpleXMLNode("baseline-filter-enabled-B?");
    m_baseLineCorrectionLimitExceededRejectB_Node->setValue(m_baseLineCorrectionLimitExceededRejectB);

    (*m_baseLineFilterSettingsNode) << m_baseLineCorrectionEnabledA_Node << m_baseLineCorrectionEnabledB_Node
                                    << m_baseLineCorrectionStartCellA_Node << m_baseLineCorrectionStartCellB_Node
                                    << m_baseLineCorrectionRegionA_Node << m_baseLineCorrectionRegionB_Node
                                    << m_baseLineCorrectionShiftValueA_Node << m_baseLineCorrectionShiftValueB_Node
                                    << m_baseLineCorrectionLimitA_Node << m_baseLineCorrectionLimitB_Node
                                    << m_baseLineCorrectionLimitExceededRejectA_Node << m_baseLineCorrectionLimitExceededRejectB_Node;

    (*m_pulseShapeFilter_contentDataA_Node) << m_pulseShapeFilter_meanDataA_Node << m_pulseShapeFilter_stddevDataA_Node;
    (*m_pulseShapeFilter_contentDataB_Node) << m_pulseShapeFilter_meanDataB_Node << m_pulseShapeFilter_stddevDataB_Node;

    (*m_pulseShapeFilterSettingsNode) << m_pulseShapeFilterEnabledA_Node
                                      << m_pulseShapeFilterEnabledB_Node
                                      << m_pulseShapeFilterRecordScheme_Node
                                      << m_pulseShapeFilter_numberOfPulsesAcq_A_Node
                                      << m_pulseShapeFilter_numberOfPulsesAcq_B_Node
                                      << m_pulseShapeFilter_leftAInNs_Node
                                      << m_pulseShapeFilter_leftBInNs_Node
                                      << m_pulseShapeFilter_rightAInNs_Node
                                      << m_pulseShapeFilter_rightBInNs_Node
                                      << m_pulseShapeFilter_ROIleftAInNs_Node
                                      << m_pulseShapeFilter_ROIleftBInNs_Node
                                      << m_pulseShapeFilter_ROIrightAInNs_Node
                                      << m_pulseShapeFilter_ROIrightBInNs_Node
                                      << m_pulseShapeFilter_StdDevUpperFractA_Node
                                      << m_pulseShapeFilter_StdDevUpperFractB_Node
                                      << m_pulseShapeFilter_StdDevLowerFractA_Node
                                      << m_pulseShapeFilter_StdDevLowerFractB_Node
                                      << m_pulseShapeFilter_contentDataA_Node
                                      << m_pulseShapeFilter_contentDataB_Node;

    (*m_riseTimeFilterSettingsNode) << m_riseTimeFilterIsActivated_Node
                                    << m_riseTimeFilterIsPlotEnabled_Node
                                    << m_riseTimeFilter_scaleInNs_A_Node
                                    << m_riseTimeFilter_scaleBinningCnt_A_Node
                                    << m_riseTimeFilter_leftWindow_A_Node
                                    << m_riseTimeFilter_rightWindow_A_Node
                                    <<  m_riseTimeFilter_scaleInNs_B_Node
                                     << m_riseTimeFilter_scaleBinningCnt_B_Node
                                     << m_riseTimeFilter_leftWindow_B_Node
                                     << m_riseTimeFilter_rightWindow_B_Node;

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
                    << m_riseTimeFilterSettingsNode
                    << m_pulseShapeFilterSettingsNode
                    << m_baseLineFilterSettingsNode
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
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

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

    if (!ok) {
        m_versionNode->setValue((int)VERSION_SETTINGS_FILE);
    }
    else {
        const int versionInt = version.toInt(&ok);

        if (!ok) {
            m_versionNode->setValue((int)VERSION_SETTINGS_FILE);
        }
        else {
            if ( versionInt >= 2 ) {
                /* rise time filter (since version 1.05 / settings version 2.0) */
                const DSimpleXMLTag pRiseTimeFilterSettingsTag = pTag.getTag(m_riseTimeFilterSettingsNode, &ok);

                if ( !ok )
                    return false;

                m_riseTimeFilterIsActivated_Node->setValue(pRiseTimeFilterSettingsTag.getValueAt(m_riseTimeFilterIsActivated_Node, &ok));
                if (!ok) m_riseTimeFilterIsActivated_Node->setValue(m_riseTimeFilterIsActivated);

                m_riseTimeFilterIsPlotEnabled_Node->setValue(pRiseTimeFilterSettingsTag.getValueAt(m_riseTimeFilterIsPlotEnabled_Node, &ok));
                if (!ok) m_riseTimeFilterIsPlotEnabled_Node->setValue(m_riseTimeFilterIsPlotEnabled);

                m_riseTimeFilter_scaleInNs_A_Node->setValue(pRiseTimeFilterSettingsTag.getValueAt(m_riseTimeFilter_scaleInNs_A_Node, &ok));
                if (!ok) m_riseTimeFilter_scaleInNs_A_Node->setValue(m_riseTimeFilter_scaleInNs_A);

                m_riseTimeFilter_scaleInNs_B_Node->setValue(pRiseTimeFilterSettingsTag.getValueAt(m_riseTimeFilter_scaleInNs_B_Node, &ok));
                if (!ok) m_riseTimeFilter_scaleInNs_B_Node->setValue(m_riseTimeFilter_scaleInNs_B);

                m_riseTimeFilter_scaleBinningCnt_A_Node->setValue(pRiseTimeFilterSettingsTag.getValueAt(m_riseTimeFilter_scaleBinningCnt_A_Node, &ok));
                if (!ok) m_riseTimeFilter_scaleBinningCnt_A_Node->setValue(m_riseTimeFilter_scaleBinningCnt_A);

                m_riseTimeFilter_scaleBinningCnt_B_Node->setValue(pRiseTimeFilterSettingsTag.getValueAt(m_riseTimeFilter_scaleBinningCnt_B_Node, &ok));
                if (!ok) m_riseTimeFilter_scaleBinningCnt_B_Node->setValue(m_riseTimeFilter_scaleBinningCnt_B);

                m_riseTimeFilter_leftWindow_A_Node->setValue(pRiseTimeFilterSettingsTag.getValueAt(m_riseTimeFilter_leftWindow_A_Node, &ok));
                if (!ok) m_riseTimeFilter_leftWindow_A_Node->setValue(m_riseTimeFilter_leftWindow_A);

                m_riseTimeFilter_leftWindow_B_Node->setValue(pRiseTimeFilterSettingsTag.getValueAt(m_riseTimeFilter_leftWindow_B_Node, &ok));
                if (!ok) m_riseTimeFilter_leftWindow_B_Node->setValue(m_riseTimeFilter_leftWindow_B);

                m_riseTimeFilter_rightWindow_A_Node->setValue(pRiseTimeFilterSettingsTag.getValueAt(m_riseTimeFilter_rightWindow_A_Node, &ok));
                if (!ok) m_riseTimeFilter_rightWindow_A_Node->setValue(m_riseTimeFilter_rightWindow_A);

                m_riseTimeFilter_rightWindow_B_Node->setValue(pRiseTimeFilterSettingsTag.getValueAt(m_riseTimeFilter_rightWindow_B_Node, &ok));
                if (!ok) m_riseTimeFilter_rightWindow_B_Node->setValue(m_riseTimeFilter_rightWindow_B);
            }

            if ( versionInt >= 3 ) {
                /* pulse shape filter + baseline jitter correction (since version 1.06 / settings version 3.0) */
                const DSimpleXMLTag pPulseShapeFilterSettingsTag = pTag.getTag(m_pulseShapeFilterSettingsNode, &ok);

                if (!ok) {
                    m_pulseShapeFilter_numberOfPulsesAcq_A_Node->setValue(m_pulseShapeFilter_numberOfPulsesAcq_A);
                    m_pulseShapeFilter_numberOfPulsesAcq_B_Node->setValue(m_pulseShapeFilter_numberOfPulsesAcq_B);
                    m_pulseShapeFilterRecordScheme_Node->setValue(m_pulseShapeFilterRecordScheme);
                    m_pulseShapeFilter_leftAInNs_Node->setValue(m_pulseShapeFilter_leftAInNs);
                    m_pulseShapeFilter_leftBInNs_Node->setValue(m_pulseShapeFilter_leftBInNs);
                    m_pulseShapeFilter_rightAInNs_Node->setValue(m_pulseShapeFilter_rightAInNs);
                    m_pulseShapeFilter_rightBInNs_Node->setValue(m_pulseShapeFilter_rightBInNs);
                    m_pulseShapeFilter_ROIleftAInNs_Node->setValue(m_pulseShapeFilter_ROIleftAInNs);
                    m_pulseShapeFilter_ROIleftBInNs_Node->setValue(m_pulseShapeFilter_ROIleftBInNs);
                    m_pulseShapeFilter_ROIrightAInNs_Node->setValue(m_pulseShapeFilter_ROIrightAInNs);
                    m_pulseShapeFilter_ROIrightBInNs_Node->setValue(m_pulseShapeFilter_ROIrightBInNs);
                    m_pulseShapeFilter_StdDevLowerFractA_Node->setValue(m_pulseShapeFilter_StdDevLowerFractA);
                    m_pulseShapeFilter_StdDevUpperFractA_Node->setValue(m_pulseShapeFilter_StdDevUpperFractA);
                    m_pulseShapeFilter_StdDevLowerFractB_Node->setValue(m_pulseShapeFilter_StdDevLowerFractB);
                    m_pulseShapeFilter_StdDevUpperFractB_Node->setValue(m_pulseShapeFilter_StdDevUpperFractB);

                    setPulseShapeFilterDataA(DRS4PulseShapeFilterData(), false);
                    setPulseShapeFilterDataB(DRS4PulseShapeFilterData(), false);

                    m_pulseShapeFilterEnabledA_Node->setValue(false);
                    m_pulseShapeFilterEnabledB_Node->setValue(false);
                }
                else {
                    m_pulseShapeFilter_numberOfPulsesAcq_A_Node->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_numberOfPulsesAcq_A_Node, &ok));
                    if (!ok) m_pulseShapeFilter_numberOfPulsesAcq_A_Node->setValue(m_pulseShapeFilter_numberOfPulsesAcq_A);

                    m_pulseShapeFilter_numberOfPulsesAcq_B_Node->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_numberOfPulsesAcq_B_Node, &ok));
                    if (!ok) m_pulseShapeFilter_numberOfPulsesAcq_B_Node->setValue(m_pulseShapeFilter_numberOfPulsesAcq_B);

                    m_pulseShapeFilterRecordScheme_Node->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilterRecordScheme_Node, &ok));
                    if (!ok) m_pulseShapeFilterRecordScheme_Node->setValue(m_pulseShapeFilterRecordScheme);

                    m_pulseShapeFilter_leftAInNs_Node ->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_leftAInNs_Node, &ok));
                    if (!ok) m_pulseShapeFilter_leftAInNs_Node->setValue(m_pulseShapeFilter_leftAInNs);

                    m_pulseShapeFilter_leftBInNs_Node ->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_leftBInNs_Node, &ok));
                    if (!ok) m_pulseShapeFilter_leftBInNs_Node->setValue(m_pulseShapeFilter_leftBInNs);

                    m_pulseShapeFilter_rightAInNs_Node ->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_rightAInNs_Node, &ok));
                    if (!ok) m_pulseShapeFilter_rightAInNs_Node->setValue(m_pulseShapeFilter_rightAInNs);

                    m_pulseShapeFilter_rightBInNs_Node ->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_rightBInNs_Node, &ok));
                    if (!ok) m_pulseShapeFilter_rightBInNs_Node->setValue(m_pulseShapeFilter_rightBInNs);

                    m_pulseShapeFilter_ROIleftAInNs_Node ->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_ROIleftAInNs_Node, &ok));
                    if (!ok) m_pulseShapeFilter_ROIleftAInNs_Node->setValue(m_pulseShapeFilter_ROIleftAInNs);

                    m_pulseShapeFilter_ROIleftBInNs_Node ->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_ROIleftBInNs_Node, &ok));
                    if (!ok) m_pulseShapeFilter_ROIleftBInNs_Node->setValue(m_pulseShapeFilter_ROIleftBInNs);

                    m_pulseShapeFilter_ROIrightAInNs_Node ->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_ROIrightAInNs_Node, &ok));
                    if (!ok) m_pulseShapeFilter_ROIrightAInNs_Node->setValue(m_pulseShapeFilter_ROIrightAInNs);

                    m_pulseShapeFilter_ROIrightBInNs_Node ->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_ROIrightBInNs_Node, &ok));
                    if (!ok) m_pulseShapeFilter_ROIrightBInNs_Node->setValue(m_pulseShapeFilter_ROIrightBInNs);

                    m_pulseShapeFilter_StdDevLowerFractA_Node->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_StdDevLowerFractA_Node, &ok));
                    if (!ok) m_pulseShapeFilter_StdDevLowerFractA_Node->setValue(m_pulseShapeFilter_StdDevLowerFractA);

                    m_pulseShapeFilter_StdDevUpperFractA_Node->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_StdDevUpperFractA_Node, &ok));
                    if (!ok) m_pulseShapeFilter_StdDevUpperFractA_Node->setValue(m_pulseShapeFilter_StdDevUpperFractA);

                    m_pulseShapeFilter_StdDevLowerFractB_Node->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_StdDevLowerFractB_Node, &ok));
                    if (!ok) m_pulseShapeFilter_StdDevLowerFractB_Node->setValue(m_pulseShapeFilter_StdDevLowerFractB);

                    m_pulseShapeFilter_StdDevUpperFractB_Node->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilter_StdDevUpperFractB_Node, &ok));
                    if (!ok) m_pulseShapeFilter_StdDevUpperFractB_Node->setValue(m_pulseShapeFilter_StdDevUpperFractB);


                    const DSimpleXMLTag pPulseShapeFilterContentATag = pPulseShapeFilterSettingsTag.getTag(m_pulseShapeFilter_contentDataA_Node, &ok);

                    if (!ok) {
                        setPulseShapeFilterDataA(DRS4PulseShapeFilterData(), false);

                        m_pulseShapeFilterEnabledA_Node->setValue(false);
                    }
                    else {
                        bool ok_1 = false, ok_2 = false;

                        m_pulseShapeFilter_meanDataA_Node->setValue(pPulseShapeFilterContentATag.getValueAt(m_pulseShapeFilter_meanDataA_Node, &ok_1));
                        m_pulseShapeFilter_stddevDataA_Node->setValue(pPulseShapeFilterContentATag.getValueAt(m_pulseShapeFilter_stddevDataA_Node, &ok_2));

                        QVector<QPointF> meanData;
                        QVector<QPointF> stdDevData;

                        if (!ok_1 || !ok_2) {
                            setPulseShapeFilterDataA(DRS4PulseShapeFilterData(), false);

                            m_pulseShapeFilterEnabledA_Node->setValue(false);
                        }
                        else {
                            parsePulseShapeData(m_pulseShapeFilter_meanDataA_Node, &meanData);
                            parsePulseShapeData(m_pulseShapeFilter_stddevDataA_Node, &stdDevData);

                            if (meanData.isEmpty()
                                    || stdDevData.isEmpty()) {
                                setPulseShapeFilterDataA(DRS4PulseShapeFilterData(), false);

                                m_pulseShapeFilterEnabledA_Node->setValue(false);
                            }
                            else {
                                DRS4PulseShapeFilterData filterData;
                                filterData.setData(meanData, stdDevData);

                                setPulseShapeFilterDataA(filterData, false);

                                m_pulseShapeFilterEnabledA_Node->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilterEnabledA_Node, &ok));
                                if (!ok) m_pulseShapeFilterEnabledA_Node->setValue(m_pulseShapeFilterEnabledA);


                            }
                        }
                    }


                    const DSimpleXMLTag pPulseShapeFilterContentBTag = pPulseShapeFilterSettingsTag.getTag(m_pulseShapeFilter_contentDataB_Node, &ok);

                    if (!ok) {
                        setPulseShapeFilterDataB(DRS4PulseShapeFilterData(), false);

                        m_pulseShapeFilterEnabledB_Node->setValue(false);
                    }
                    else {
                        bool ok_1 = false, ok_2 = false;

                        m_pulseShapeFilter_meanDataB_Node->setValue(pPulseShapeFilterContentBTag.getValueAt(m_pulseShapeFilter_meanDataB_Node, &ok_1));
                        m_pulseShapeFilter_stddevDataB_Node->setValue(pPulseShapeFilterContentBTag.getValueAt(m_pulseShapeFilter_stddevDataB_Node, &ok_2));

                        QVector<QPointF> meanData;
                        QVector<QPointF> stdDevData;

                        if (!ok_1 || !ok_2) {
                            setPulseShapeFilterDataB(DRS4PulseShapeFilterData(), false);

                            m_pulseShapeFilterEnabledB_Node->setValue(false);
                        }
                        else {
                            parsePulseShapeData(m_pulseShapeFilter_meanDataB_Node, &meanData);
                            parsePulseShapeData(m_pulseShapeFilter_stddevDataB_Node, &stdDevData);

                            if (meanData.isEmpty() || stdDevData.isEmpty()) {
                                setPulseShapeFilterDataB(DRS4PulseShapeFilterData(), false);

                                m_pulseShapeFilterEnabledB_Node->setValue(false);
                            }
                            else {
                                DRS4PulseShapeFilterData filterData;
                                filterData.setData(meanData, stdDevData);

                                setPulseShapeFilterDataB(filterData, false);

                                m_pulseShapeFilterEnabledB_Node->setValue(pPulseShapeFilterSettingsTag.getValueAt(m_pulseShapeFilterEnabledB_Node, &ok));
                                if (!ok) m_pulseShapeFilterEnabledB_Node->setValue(m_pulseShapeFilterEnabledB);
                            }
                        }
                    }
                }

                const DSimpleXMLTag pBaselineCorrectionSettingsTag = pTag.getTag(m_baseLineFilterSettingsNode, &ok);

                if (!ok) {
                    m_baseLineCorrectionEnabledA_Node->setValue(m_baseLineCorrectionEnabledA);
                    m_baseLineCorrectionStartCellA_Node->setValue(m_baseLineCorrectionStartCellA);
                    m_baseLineCorrectionRegionA_Node->setValue(m_baseLineCorrectionRegionA);
                    m_baseLineCorrectionShiftValueA_Node->setValue(m_baseLineCorrectionShiftValueA);
                    m_baseLineCorrectionLimitA_Node->setValue(m_baseLineCorrectionLimitA);
                    m_baseLineCorrectionLimitExceededRejectA_Node->setValue(m_baseLineCorrectionLimitExceededRejectA);

                    m_baseLineCorrectionEnabledB_Node->setValue(m_baseLineCorrectionEnabledB);
                    m_baseLineCorrectionStartCellB_Node->setValue(m_baseLineCorrectionStartCellB);
                    m_baseLineCorrectionRegionB_Node->setValue(m_baseLineCorrectionRegionB);
                    m_baseLineCorrectionShiftValueB_Node->setValue(m_baseLineCorrectionShiftValueB);
                    m_baseLineCorrectionLimitB_Node->setValue(m_baseLineCorrectionLimitB);
                    m_baseLineCorrectionLimitExceededRejectB_Node->setValue(m_baseLineCorrectionLimitExceededRejectB);
                }
                else {
                    m_baseLineCorrectionEnabledA_Node->setValue(pBaselineCorrectionSettingsTag.getValueAt(m_baseLineCorrectionEnabledA_Node, &ok));
                    if (!ok) m_baseLineCorrectionEnabledA_Node->setValue(m_baseLineCorrectionEnabledA);

                    m_baseLineCorrectionStartCellA_Node->setValue(pBaselineCorrectionSettingsTag.getValueAt(m_baseLineCorrectionStartCellA_Node, &ok));
                    if (!ok) m_baseLineCorrectionStartCellA_Node->setValue(m_baseLineCorrectionStartCellA);

                    m_baseLineCorrectionRegionA_Node->setValue(pBaselineCorrectionSettingsTag.getValueAt(m_baseLineCorrectionRegionA_Node, &ok));
                    if (!ok) m_baseLineCorrectionRegionA_Node->setValue(m_baseLineCorrectionRegionA);

                    m_baseLineCorrectionShiftValueA_Node->setValue(pBaselineCorrectionSettingsTag.getValueAt(m_baseLineCorrectionShiftValueA_Node, &ok));
                    if (!ok) m_baseLineCorrectionShiftValueA_Node->setValue(m_baseLineCorrectionShiftValueA);

                    m_baseLineCorrectionLimitA_Node->setValue(pBaselineCorrectionSettingsTag.getValueAt(m_baseLineCorrectionLimitA_Node, &ok));
                    if (!ok) m_baseLineCorrectionLimitA_Node->setValue(m_baseLineCorrectionLimitA);

                    m_baseLineCorrectionLimitExceededRejectA_Node->setValue(pBaselineCorrectionSettingsTag.getValueAt(m_baseLineCorrectionLimitExceededRejectA_Node, &ok));
                    if (!ok) m_baseLineCorrectionLimitExceededRejectA_Node->setValue(m_baseLineCorrectionLimitExceededRejectA);


                    m_baseLineCorrectionEnabledB_Node->setValue(pBaselineCorrectionSettingsTag.getValueAt(m_baseLineCorrectionEnabledB_Node, &ok));
                    if (!ok) m_baseLineCorrectionEnabledB_Node->setValue(m_baseLineCorrectionEnabledB);

                    m_baseLineCorrectionStartCellB_Node->setValue(pBaselineCorrectionSettingsTag.getValueAt(m_baseLineCorrectionStartCellB_Node, &ok));
                    if (!ok) m_baseLineCorrectionStartCellB_Node->setValue(m_baseLineCorrectionStartCellB);

                    m_baseLineCorrectionRegionB_Node->setValue(pBaselineCorrectionSettingsTag.getValueAt(m_baseLineCorrectionRegionB_Node, &ok));
                    if (!ok) m_baseLineCorrectionRegionB_Node->setValue(m_baseLineCorrectionRegionB);

                    m_baseLineCorrectionShiftValueB_Node->setValue(pBaselineCorrectionSettingsTag.getValueAt(m_baseLineCorrectionShiftValueB_Node, &ok));
                    if (!ok) m_baseLineCorrectionShiftValueB_Node->setValue(m_baseLineCorrectionShiftValueB);

                    m_baseLineCorrectionLimitB_Node->setValue(pBaselineCorrectionSettingsTag.getValueAt(m_baseLineCorrectionLimitB_Node, &ok));
                    if (!ok) m_baseLineCorrectionLimitB_Node->setValue(m_baseLineCorrectionLimitB);

                    m_baseLineCorrectionLimitExceededRejectB_Node->setValue(pBaselineCorrectionSettingsTag.getValueAt(m_baseLineCorrectionLimitExceededRejectB_Node, &ok));
                    if (!ok) m_baseLineCorrectionLimitExceededRejectB_Node->setValue(m_baseLineCorrectionLimitExceededRejectB);
                }
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

#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    DSimpleXMLWriter writer(path);

    const bool ok = writer.writeToFile(m_parentNode, false);

    if (ok && !autosave)
        m_fileName = path;

    return ok;
}

void DRS4SettingsManager::parsePulseShapeData(DSimpleXMLNode *node, QVector<QPointF> *filterData)
{
    if (!node || !filterData)
        return;

    const DString nodeText = (DString)(node->getValue().toString());

    if (nodeText.isEmpty()) {
        filterData->clear();

        return;
    }

    const QStringList list = nodeText.parseBetween2("{", "}");

    if (list.size() <= 0) {
        filterData->clear();

        return;
    }

    QVector<QPointF> data;

    for ( int i = 0 ; i < list.size() ; ++ i ) {
        const QString val = list.at(i);

        if (val.isEmpty()) {
            filterData->clear();

            return;
        }

        const QStringList values = val.split(";");

        if (values.size() != 2) {
            filterData->clear();

            return;
        }

        bool ok = false;

        const double x = QVariant(values.at(0)).toDouble(&ok);

        if (!ok) {
            filterData->clear();

            return;
        }

        const double y = QVariant(values.at(1)).toDouble(&ok);

        if (!ok) {
            filterData->clear();

            return;
        }

        data.append(QPointF(x, y));
    }

    if (data.isEmpty()) {
        filterData->clear();

        return;
    }

    filterData->clear();
    filterData->append(data);
}

DSimpleXMLNode *DRS4SettingsManager::parentNode() const
{
    return m_parentNode;
}

QMutex *DRS4SettingsManager::mutex()
{
    return &m_mutex;
}

void DRS4SettingsManager::setForceCoincidence(bool force)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_bForceCoincidence = force;
    m_bForceCoincidenceNode->setValue(force);
}

void DRS4SettingsManager::setBurstMode(bool on)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_burstModeNode->setValue(on);
    m_burstMode = on;
}

void DRS4SettingsManager::setStartCell(int startCell)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_startCellNode->setValue(startCell);
    m_startCell = startCell;
}

void DRS4SettingsManager::setStopCell(int stopCell)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_stopCellNode->setValue(stopCell);
    m_stopCell = stopCell;
}

void DRS4SettingsManager::setPositivSignal(bool positiv)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_isPositivSignalNode->setValue(positiv);
    m_isPositivSignal = positiv;
}

void DRS4SettingsManager::setNegativeLifetimeAccepted(bool accepted)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_negativeLifetimesNode->setValue(accepted);
    m_negativeLifetimes = accepted;
}

void DRS4SettingsManager::setIgnoreBusyState(bool ignore)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_ignoreBusyNode->setValue(ignore);
    m_ignoreBusy = ignore;
}

void DRS4SettingsManager::setCFDLevelA(double level)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_cfdLevelANode->setValue(level);
    m_cfdLevelA = level;
}

void DRS4SettingsManager::setCFDLevelB(double level)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_cfdLevelBNode->setValue(level);
    m_cfdLevelB = level;
}

void DRS4SettingsManager::setOffsetInNSAB(double offset)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_offsetNsABNode->setValue(offset);
    m_offsetNsAB = offset;
}

void DRS4SettingsManager::setOffsetInNSBA(double offset)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_offsetNsBANode->setValue(offset);
    m_offsetNsBA = offset;
}

void DRS4SettingsManager::setOffsetInNSCoincidence(double offset)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_offsetNsCoincidenceNode->setValue(offset);
    m_offsetNsCoincidence = offset;
}

void DRS4SettingsManager::setOffsetInNSMerged(double offset)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_offsetNsMergedNode->setValue(offset);
    m_offsetNsMerged = offset;
}

void DRS4SettingsManager::setScalerInNSAB(double scaler)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_scalerNsABNode->setValue(scaler);
    m_scalerNsAB = scaler;
}

void DRS4SettingsManager::setScalerInNSBA(double scaler)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_scalerNsBANode->setValue(scaler);
    m_scalerNsBA = scaler;
}

void DRS4SettingsManager::setScalerInNSCoincidence(double scaler)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_scalerNsCoincidenceNode->setValue(scaler);
    m_scalerNsCoincidence = scaler;
}

void DRS4SettingsManager::setScalerInNSMerged(double scaler)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_scalerNsMergedNode->setValue(scaler);
    m_scalerNsMerged = scaler;
}

void DRS4SettingsManager::setChannelCntAB(int count)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_channelCountABNode->setValue(count);
    m_channelCountAB = count;
}

void DRS4SettingsManager::setChannelCntBA(int count)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_channelCountBANode->setValue(count);
    m_channelCountBA = count;
}

void DRS4SettingsManager::setChannelCntCoincindence(int count)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_channelCountCoincidenceNode->setValue(count);
    m_channelCountCoincidence = count;
}

void DRS4SettingsManager::setChannelCntMerged(int count)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_channelCountMergedNode->setValue(count);
    m_channelCountMerged = count;
}

void DRS4SettingsManager::setTriggerSource(int triggerSource)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_triggerSourceNode->setValue(triggerSource);
    m_triggerSource = triggerSource;
}

void DRS4SettingsManager::setTriggerPolarityPositive(bool positive)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_isTriggerPolarityPositiveNode->setValue(positive);
    m_isTriggerPolarityPositive = positive;
}

void DRS4SettingsManager::setTriggerLevelAmV(double triggerLevelInmV)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_triggerLevelANode->setValue(triggerLevelInmV);
    m_triggerLevelA = triggerLevelInmV;
}

void DRS4SettingsManager::setTriggerLevelBmV(double triggerLevelInmV)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_triggerLevelBNode->setValue(triggerLevelInmV);
    m_triggerLevelB = triggerLevelInmV;
}

void DRS4SettingsManager::setTriggerDelayInNs(double delayInNs)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_triggerDelayInNSNode ->setValue(delayInNs);
    m_triggerDelayInNS = delayInNs;
}

void DRS4SettingsManager::setComment(const QString &comment)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_commentNode->setValue(comment);
    m_comment = comment;
}

void DRS4SettingsManager::setLastSaveDate(const QString &lastSaveDate)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_lastSaveDateNode->setValue(lastSaveDate);
    m_lastSaveDate = lastSaveDate;
}

void DRS4SettingsManager::setChannelNumberA(int channelNumber)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_channelNumberANode->setValue(channelNumber);
    m_channelNumberA = channelNumber;
}

void DRS4SettingsManager::setChannelNumberB(int channelNumber)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_channelNumberBNode->setValue(channelNumber);
    m_channelNumberB = channelNumber;
}

void DRS4SettingsManager::setStartChannelA(int min, int max)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_startAChannelMin = min;
    m_startAChannelMax = max;

    m_startAChannelMinNode->setValue(min);
    m_startAChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStartChannelB(int min, int max)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_startBChannelMin = min;
    m_startBChannelMax = max;

    m_startBChannelMinNode->setValue(min);
    m_startBChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStopChannelA(int min, int max)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_stopAChannelMin = min;
    m_stopAChannelMax = max;

    m_stopAChannelMinNode->setValue(min);
    m_stopAChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStopChannelB(int min, int max)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_stopBChannelMin = min;
    m_stopBChannelMax = max;

    m_stopBChannelMinNode->setValue(min);
    m_stopBChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStartChannelBMin(int min)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_startBChannelMin = min;
    m_startBChannelMinNode->setValue(min);
}

void DRS4SettingsManager::setStartChannelAMax(int max)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_startAChannelMax = max;
    m_startAChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStartChannelBMax(int max)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_startBChannelMax = max;
    m_startBChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStopChannelAMin(int min)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_stopAChannelMin = min;
    m_stopAChannelMinNode->setValue(min);
}

void DRS4SettingsManager::setStopChannelBMin(int min)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_stopBChannelMin = min;
    m_stopBChannelMinNode->setValue(min);
}

void DRS4SettingsManager::setStopChannelAMax(int max)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_stopAChannelMax = max;
    m_stopAChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setStopChannelBMax(int max)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_stopBChannelMax = max;
    m_stopBChannelMaxNode->setValue(max);
}

void DRS4SettingsManager::setMeanCableDelayInNs(double ns)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_meanNs = ns;
    m_meanNSNode->setValue(ns);
}

void DRS4SettingsManager::setFitIterations(int count)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_fitIter = count;
    m_fitIterNode->setValue(count);
}

void DRS4SettingsManager::setFitIterationsMerged(int count)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_fitIterMerged = count;
    m_fitIterMergedNode->setValue(count);
}

void DRS4SettingsManager::setFitIterationsAB(int count)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_fitIterAB = count;
    m_fitIterABNode->setValue(count);
}

void DRS4SettingsManager::setFitIterationsBA(int count)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_fitIterBA = count;
    m_fitIterBANode->setValue(count);
}

void DRS4SettingsManager::setSweepInNanoseconds(double ns)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_sweepInNanoSec = ns;
    m_sweepInNanoSecNode->setValue(ns);
}

void DRS4SettingsManager::setSampleSpeedInGHz(double ghz)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_freqInGHz = ghz;
    m_freqInGHzNode->setValue(ghz);
}

void DRS4SettingsManager::setPulseAreaFilterNormalizationA(double percentage)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseAreaNormA = percentage;
    m_pulseAreaNormANode->setValue(percentage);
}

void DRS4SettingsManager::setPulseAreaFilterNormalizationB(double percentage)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseAreaNormB = percentage;
    m_pulseAreaNormBNode->setValue(percentage);
}

void DRS4SettingsManager::setPulseAreaFilterBinningA(int binning)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseAreaBinningA = binning;
    m_pulseAreaBinningANode->setValue(binning);
}

void DRS4SettingsManager::setPulseAreaFilterBinningB(int binning)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseAreaBinningB = binning;
    m_pulseAreaBinningBNode->setValue(binning);
}

void DRS4SettingsManager::setPulseAreaFilterLimitUpperLeftA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseAreaLeftUpper_A = value;
    m_pulseAreaLeftUpper_A_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitUpperRightA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseAreaRightUpper_A = value;
    m_pulseAreaRightUpper_A_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitLowerLeftA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseAreaLeftLower_A = value;
    m_pulseAreaLeftLower_A_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitLowerRightA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseAreaRightLower_A = value;
    m_pulseAreaRightLower_A_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitUpperLeftB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseAreaLeftUpper_B = value;
    m_pulseAreaLeftUpper_B_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitUpperRightB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseAreaRightUpper_B = value;
    m_pulseAreaRightUpper_B_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitLowerLeftB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseAreaLeftLower_B = value;
    m_pulseAreaLeftLower_B_Node->setValue(value);
}

void DRS4SettingsManager::setPulseAreaFilterLimitLowerRightB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseAreaRightLower_B = value;
    m_pulseAreaRightLower_B_Node->setValue(value);
}

void DRS4SettingsManager::setRiseTimeFilterEnabled(bool enabled)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_riseTimeFilterIsActivated = enabled;
    m_riseTimeFilterIsActivated_Node->setValue(enabled);
}

void DRS4SettingsManager::setRiseTimeFilterPlotEnabled(bool enabled)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_riseTimeFilterIsPlotEnabled = enabled;
    m_riseTimeFilterIsPlotEnabled_Node->setValue(enabled);
}

void DRS4SettingsManager::setRiseTimeFilterScaleInNanosecondsOfA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_riseTimeFilter_scaleInNs_A = value;
    m_riseTimeFilter_scaleInNs_A_Node->setValue(value);
}

void DRS4SettingsManager::setRiseTimeFilterScaleInNanosecondsOfB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_riseTimeFilter_scaleInNs_B = value;
    m_riseTimeFilter_scaleInNs_B_Node->setValue(value);
}

void DRS4SettingsManager::setRiseTimeFilterBinningOfA(int value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_riseTimeFilter_scaleBinningCnt_A = value;
    m_riseTimeFilter_scaleBinningCnt_A_Node->setValue(value);
}

void DRS4SettingsManager::setRiseTimeFilterBinningOfB(int value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_riseTimeFilter_scaleBinningCnt_B = value;
    m_riseTimeFilter_scaleBinningCnt_B_Node->setValue(value);
}

void DRS4SettingsManager::setRiseTimeFilterLeftWindowOfA(int value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_riseTimeFilter_leftWindow_A = value;
    m_riseTimeFilter_leftWindow_A_Node->setValue(value);
}

void DRS4SettingsManager::setRiseTimeFilterLeftWindowOfB(int value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_riseTimeFilter_leftWindow_B = value;
    m_riseTimeFilter_leftWindow_B_Node->setValue(value);
}

void DRS4SettingsManager::setRiseTimeFilterRightWindowOfA(int value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_riseTimeFilter_rightWindow_A = value;
    m_riseTimeFilter_rightWindow_A_Node->setValue(value);
}

void DRS4SettingsManager::setRiseTimeFilterRightWindowOfB(int value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_riseTimeFilter_rightWindow_B = value;
    m_riseTimeFilter_rightWindow_B_Node->setValue(value);
}

void DRS4SettingsManager::setPersistanceEnabled(bool activated)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_persistanceEnabled = activated;
    m_persistanceEnabled_Node->setValue(activated);
}

void DRS4SettingsManager::setPersistanceLeftInNsOfA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_persistance_leftAInNs = value;
    m_persistance_leftAInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPersistanceLeftInNsOfB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_persistance_leftBInNs = value;
    m_persistance_leftBInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPersistanceRightInNsOfA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_persistance_rightAInNs = value;
    m_persistance_rightAInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPersistanceRightInNsOfB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_persistance_rightBInNs = value;
    m_persistance_rightBInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPersistanceUsingCFDBAsRefForA(bool on)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_persistanceUsingRefB_A_Node->setValue(on);
}

void DRS4SettingsManager::setPersistanceUsingCFDAAsRefForB(bool on)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_persistanceUsingRefA_B_Node->setValue(on);
}

void DRS4SettingsManager::setInterpolationType(const DRS4InterpolationType::type &type)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_cfdAlgorithmTypeNode->setValue(type);
}

void DRS4SettingsManager::setSplineInterpolationType(const DRS4SplineInterpolationType::type &type)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

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
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_splineIntraSamplingPointsNode->setValue(counts);
}

void DRS4SettingsManager::setPolynomialSamplingCounts(int counts)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_polynomialIntraSamplingPointsNode->setValue(counts);
}

void DRS4SettingsManager::setMedianFilterAEnabled(bool enabled)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_medianFilterActivated_A_Node->setValue(enabled);
}

void DRS4SettingsManager::setMedianFilterBEnabled(bool enabled)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_medianFilterActivated_B_Node->setValue(enabled);
}

void DRS4SettingsManager::setMedianFilterWindowSizeA(int size)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_medianFilterWindowSize_A_Node->setValue(size);
}

void DRS4SettingsManager::setMedianFilterWindowSizeB(int size)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_medianFilterWindowSize_B_Node->setValue(size);
}

void DRS4SettingsManager::setPulseShapeFilterNumberOfPulsesToBeRecordedA(int number)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_numberOfPulsesAcq_A_Node->setValue(number);
}

void DRS4SettingsManager::setPulseShapeFilterNumberOfPulsesToBeRecordedB(int number)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_numberOfPulsesAcq_B_Node->setValue(number);
}

void DRS4SettingsManager::setPulseShapeFilterLeftInNsOfA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_leftAInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPulseShapeFilterLeftInNsOfB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_leftBInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPulseShapeFilterRightInNsOfA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_rightAInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPulseShapeFilterRightInNsOfB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_rightBInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPulseShapeFilterROILeftInNsOfA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_ROIleftAInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPulseShapeFilterROILeftInNsOfB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_ROIleftBInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPulseShapeFilterROIRightInNsOfA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_ROIrightAInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPulseShapeFilterROIRightInNsOfB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_ROIrightBInNs_Node->setValue(value);
}

void DRS4SettingsManager::setPulseShapeFilterStdDevUpperFractionA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_StdDevUpperFractA_Node->setValue(value);
}

void DRS4SettingsManager::setPulseShapeFilterStdDevLowerFractionA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_StdDevLowerFractA_Node->setValue(value);
}

void DRS4SettingsManager::setPulseShapeFilterStdDevUpperFractionB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_StdDevUpperFractB_Node->setValue(value);
}

void DRS4SettingsManager::setPulseShapeFilterStdDevLowerFractionB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilter_StdDevLowerFractB_Node->setValue(value);
}

void DRS4SettingsManager::setPulseShapeFilterDataA(const DRS4PulseShapeFilterData &data, bool lockMutex)
{
    if (lockMutex) {
#ifndef __DISABLE_MUTEX_LOCKER
        QMutexLocker locker(&m_mutex);
#endif
    }

    m_pulseShapeFilterDataA = data;

    QString valueMean = "";
    QString valueStdDev = "";

    for (int i = 0 ; i < m_pulseShapeFilterDataA.mean().size() ; ++ i) {
        valueMean.append("{" + QVariant(m_pulseShapeFilterDataA.mean().at(i).x()).toString() + ";" + QVariant(m_pulseShapeFilterDataA.mean().at(i).y()).toString() + "}");
        valueStdDev.append("{" + QVariant(m_pulseShapeFilterDataA.stddev().at(i).x()).toString() + ";" + QVariant(m_pulseShapeFilterDataA.stddev().at(i).y()).toString() + "}");
    }

    m_pulseShapeFilter_meanDataA_Node->setValue(valueMean);
    m_pulseShapeFilter_stddevDataA_Node->setValue(valueStdDev);
}

void DRS4SettingsManager::setPulseShapeFilterDataB(const DRS4PulseShapeFilterData &data, bool lockMutex)
{
    if (lockMutex) {
#ifndef __DISABLE_MUTEX_LOCKER
        QMutexLocker locker(&m_mutex);
#endif
    }

    m_pulseShapeFilterDataB = data;

    QString valueMean = "";
    QString valueStdDev = "";

    for (int i = 0 ; i < m_pulseShapeFilterDataB.mean().size() ; ++ i) {
        valueMean.append("{" + QVariant(m_pulseShapeFilterDataB.mean().at(i).x()).toString() + ";" + QVariant(m_pulseShapeFilterDataB.mean().at(i).y()).toString() + "}");
        valueStdDev.append("{" + QVariant(m_pulseShapeFilterDataB.stddev().at(i).x()).toString() + ";" + QVariant(m_pulseShapeFilterDataB.stddev().at(i).y()).toString() + "}");
    }

    m_pulseShapeFilter_meanDataB_Node->setValue(valueMean);
    m_pulseShapeFilter_stddevDataB_Node->setValue(valueStdDev);
}

void DRS4SettingsManager::setPulseShapeFilterEnabledA(bool enabled)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilterEnabledA_Node->setValue(enabled);
}

void DRS4SettingsManager::setPulseShapeFilterEnabledB(bool enabled)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilterEnabledB_Node->setValue(enabled);
}

void DRS4SettingsManager::setPulseShapeFilterRecordScheme(const DRS4PulseShapeFilterRecordScheme::Scheme &rc)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_pulseShapeFilterRecordScheme_Node->setValue(rc);
}

void DRS4SettingsManager::setBaselineCorrectionCalculationStartCellA(int cell)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_baseLineCorrectionStartCellA_Node->setValue(cell);
}

void DRS4SettingsManager::setBaselineCorrectionCalculationRegionA(int region)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_baseLineCorrectionRegionA_Node->setValue(region);
}

void DRS4SettingsManager::setBaselineCorrectionCalculationEnabledA(bool enabled)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_baseLineCorrectionEnabledA_Node->setValue(enabled);
}

void DRS4SettingsManager::setBaselineCorrectionCalculationShiftValueInMVA(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_baseLineCorrectionShiftValueA_Node->setValue(value);
}

void DRS4SettingsManager::setBaselineCorrectionCalculationLimitInPercentageA(double limit)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_baseLineCorrectionLimitA_Node->setValue(limit);
}

void DRS4SettingsManager::setBaselineCorrectionCalculationLimitRejectLimitA(bool reject)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_baseLineCorrectionLimitExceededRejectA_Node->setValue(reject);
}

int DRS4SettingsManager::baselineCorrectionCalculationStartCellA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_baseLineCorrectionStartCellA_Node->getValue().toInt();
}

int DRS4SettingsManager::baselineCorrectionCalculationRegionA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_baseLineCorrectionRegionA_Node->getValue().toInt();
}

bool DRS4SettingsManager::baselineCorrectionCalculationEnabledA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_baseLineCorrectionEnabledA_Node->getValue().toBool();
}

double DRS4SettingsManager::baselineCorrectionCalculationShiftValueInMVA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_baseLineCorrectionShiftValueA_Node->getValue().toDouble();
}

double DRS4SettingsManager::baselineCorrectionCalculationLimitInPercentageA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_baseLineCorrectionLimitA_Node->getValue().toDouble();
}

bool DRS4SettingsManager::baselineCorrectionCalculationLimitRejectLimitA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_baseLineCorrectionLimitExceededRejectA_Node->getValue().toBool();
}

void DRS4SettingsManager::setBaselineCorrectionCalculationStartCellB(int cell)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_baseLineCorrectionStartCellB_Node->setValue(cell);
}

void DRS4SettingsManager::setBaselineCorrectionCalculationRegionB(int region)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_baseLineCorrectionRegionB_Node->setValue(region);
}

void DRS4SettingsManager::setBaselineCorrectionCalculationEnabledB(bool enabled)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_baseLineCorrectionEnabledB_Node->setValue(enabled);
}

void DRS4SettingsManager::setBaselineCorrectionCalculationShiftValueInMVB(double value)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_baseLineCorrectionShiftValueB_Node->setValue(value);
}

void DRS4SettingsManager::setBaselineCorrectionCalculationLimitInPercentageB(double limit)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_baseLineCorrectionLimitB_Node->setValue(limit);
}

void DRS4SettingsManager::setBaselineCorrectionCalculationLimitRejectLimitB(bool reject)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_baseLineCorrectionLimitExceededRejectB_Node->setValue(reject);
}

int DRS4SettingsManager::baselineCorrectionCalculationStartCellB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_baseLineCorrectionStartCellB_Node->getValue().toInt();
}

int DRS4SettingsManager::baselineCorrectionCalculationRegionB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_baseLineCorrectionRegionB_Node->getValue().toInt();
}

bool DRS4SettingsManager::baselineCorrectionCalculationEnabledB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_baseLineCorrectionEnabledB_Node->getValue().toBool();
}

double DRS4SettingsManager::baselineCorrectionCalculationShiftValueInMVB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_baseLineCorrectionShiftValueB_Node->getValue().toDouble();
}

double DRS4SettingsManager::baselineCorrectionCalculationLimitInPercentageB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_baseLineCorrectionLimitB_Node->getValue().toDouble();
}

bool DRS4SettingsManager::baselineCorrectionCalculationLimitRejectLimitB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_baseLineCorrectionLimitExceededRejectB_Node->getValue().toBool();
}

bool DRS4SettingsManager::pulseShapeFilterEnabledA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilterEnabledA_Node->getValue().toBool();
}

bool DRS4SettingsManager::pulseShapeFilterEnabledB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilterEnabledB_Node->getValue().toBool();
}

DRS4PulseShapeFilterRecordScheme::Scheme DRS4SettingsManager::pulseShapeFilterRecordScheme() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return (DRS4PulseShapeFilterRecordScheme::Scheme)(m_pulseShapeFilterRecordScheme_Node->getValue().toInt());
}

int DRS4SettingsManager::pulseShapeFilterNumberOfPulsesToBeRecordedA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_numberOfPulsesAcq_A_Node->getValue().toInt();
}

int DRS4SettingsManager::pulseShapeFilterNumberOfPulsesToBeRecordedB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_numberOfPulsesAcq_B_Node->getValue().toInt();
}

double DRS4SettingsManager::pulseShapeFilterLeftInNsOfA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_leftAInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseShapeFilterLeftInNsOfB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_leftBInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseShapeFilterRightInNsOfA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_rightAInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseShapeFilterRightInNsOfB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_rightBInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseShapeFilterROILeftInNsOfA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_ROIleftAInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseShapeFilterROILeftInNsOfB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_ROIleftBInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseShapeFilterROIRightInNsOfA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_ROIrightAInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseShapeFilterROIRightInNsOfB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_ROIrightBInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseShapeFilterStdDevUpperFractionA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_StdDevUpperFractA_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseShapeFilterStdDevLowerFractionA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_StdDevLowerFractA_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseShapeFilterStdDevUpperFractionB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_StdDevUpperFractB_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseShapeFilterStdDevLowerFractionB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilter_StdDevLowerFractB_Node->getValue().toDouble();
}

DRS4PulseShapeFilterData DRS4SettingsManager::pulseShapeFilterDataA(bool lockMutex) const
{
    if (lockMutex)
#ifndef __DISABLE_MUTEX_LOCKER
        QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilterDataA;
}

DRS4PulseShapeFilterData DRS4SettingsManager::pulseShapeFilterDataB(bool lockMutex) const
{
    if (lockMutex)
#ifndef __DISABLE_MUTEX_LOCKER
        QMutexLocker locker(&m_mutex);
#endif

    return m_pulseShapeFilterDataB;
}

DRS4PulseShapeFilterData *DRS4SettingsManager::pulseShapeFilterDataPtrA()
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return &m_pulseShapeFilterDataA;
}

DRS4PulseShapeFilterData *DRS4SettingsManager::pulseShapeFilterDataPtrB()
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return &m_pulseShapeFilterDataB;
}

bool DRS4SettingsManager::medianFilterAEnabled() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_medianFilterActivated_A_Node->getValue().toBool();
}

bool DRS4SettingsManager::medianFilterBEnabled() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_medianFilterActivated_B_Node->getValue().toBool();
}

int DRS4SettingsManager::medianFilterWindowSizeA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_medianFilterWindowSize_A_Node->getValue().toInt();
}

int DRS4SettingsManager::medianFilterWindowSizeB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_medianFilterWindowSize_B_Node->getValue().toInt();
}

DRS4InterpolationType::type DRS4SettingsManager::interpolationType() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    bool ok = false;
    DRS4InterpolationType::type type = (DRS4InterpolationType::type)m_cfdAlgorithmTypeNode->getValue().toInt(&ok);

    if (!ok)
        type = DRS4InterpolationType::type::spline;

    return type;
}

DRS4SplineInterpolationType::type DRS4SettingsManager::splineInterpolationType() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

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
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    bool ok = false;
    int points = m_splineIntraSamplingPointsNode->getValue().toInt(&ok);

    if (!ok)
        points = 10;

    return points;
}

int DRS4SettingsManager::polynomialSamplingCounts() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    bool ok = false;
    int points = m_polynomialIntraSamplingPointsNode->getValue().toInt(&ok);

    if (!ok)
        points = 10;

    return points;
}

bool DRS4SettingsManager::isPersistanceEnabled() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_persistanceEnabled_Node->getValue().toBool();
}

double DRS4SettingsManager::persistanceLeftInNsOfA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_persistance_leftAInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::persistanceLeftInNsOfB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_persistance_leftBInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::persistanceRightInNsOfA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_persistance_rightAInNs_Node->getValue().toDouble();
}

double DRS4SettingsManager::persistanceRightInNsOfB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_persistance_rightBInNs_Node->getValue().toDouble();
}

bool DRS4SettingsManager::persistanceUsingCFDBAsRefForA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_persistanceUsingRefB_A_Node->getValue().toBool();
}

bool DRS4SettingsManager::persistanceUsingCFDAAsRefForB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_persistanceUsingRefA_B_Node->getValue().toBool();
}

bool DRS4SettingsManager::isPulseAreaFilterEnabled() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaFilerEnabledNode->getValue().toBool();
}

bool DRS4SettingsManager::isPulseAreaFilterPlotEnabled() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaFilerEnabledPlotNode->getValue().toBool();
}

double DRS4SettingsManager::pulseAreaFilterNormalizationA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaNormANode->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterNormalizationB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaNormBNode->getValue().toDouble();
}

int DRS4SettingsManager::pulseAreaFilterBinningA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaBinningANode->getValue().toInt();
}

int DRS4SettingsManager::pulseAreaFilterBinningB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaBinningBNode->getValue().toInt();
}

double DRS4SettingsManager::pulseAreaFilterLimitUpperLeftA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaLeftUpper_A_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitUpperRightA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaRightUpper_A_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitLowerLeftA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaLeftLower_A_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitLowerRightA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaRightLower_A_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitUpperLeftB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaLeftUpper_B_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitUpperRightB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaRightUpper_B_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitLowerLeftB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaLeftLower_B_Node->getValue().toDouble();
}

double DRS4SettingsManager::pulseAreaFilterLimitLowerRightB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_pulseAreaRightLower_B_Node->getValue().toDouble();
}

bool DRS4SettingsManager::isRiseTimeFilterEnabled() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_riseTimeFilterIsActivated_Node->getValue().toBool();
}

bool DRS4SettingsManager::isRiseTimeFilterPlotEnabled() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_riseTimeFilterIsPlotEnabled_Node->getValue().toBool();
}

double DRS4SettingsManager::riseTimeFilterScaleInNanosecondsOfA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_riseTimeFilter_scaleInNs_A_Node->getValue().toDouble();
}

double DRS4SettingsManager::riseTimeFilterScaleInNanosecondsOfB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_riseTimeFilter_scaleInNs_B_Node->getValue().toDouble();
}

int DRS4SettingsManager::riseTimeFilterBinningOfA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_riseTimeFilter_scaleBinningCnt_A_Node->getValue().toInt();
}

int DRS4SettingsManager::riseTimeFilterBinningOfB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_riseTimeFilter_scaleBinningCnt_B_Node->getValue().toInt();
}

int DRS4SettingsManager::riseTimeFilterLeftWindowOfA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_riseTimeFilter_leftWindow_A_Node->getValue().toInt();
}

int DRS4SettingsManager::riseTimeFilterLeftWindowOfB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_riseTimeFilter_leftWindow_B_Node->getValue().toInt();
}

int DRS4SettingsManager::riseTimeFilterRightWindowOfA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_riseTimeFilter_rightWindow_A_Node->getValue().toInt();
}

int DRS4SettingsManager::riseTimeFilterRightWindowOfB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_riseTimeFilter_rightWindow_B_Node->getValue().toInt();
}

void DRS4SettingsManager::setPulseAreaFilterEnabled(bool enabled)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_bPulseAreaFilterEnabled = enabled;
    m_pulseAreaFilerEnabledNode->setValue(enabled);
}

void DRS4SettingsManager::setPulseAreaFilterPlotEnabled(bool enabled)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_bPulseAreaFilterEnabledPlot = enabled;
    m_pulseAreaFilerEnabledPlotNode->setValue(enabled);
}

void DRS4SettingsManager::setStartChannelAMin(int min)
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    m_startAChannelMin = min;
    m_startAChannelMinNode->setValue(min);
}

bool DRS4SettingsManager::isPositiveSignal() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_isPositivSignalNode->getValue().toBool();
}

bool DRS4SettingsManager::isNegativeLTAccepted() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_negativeLifetimesNode->getValue().toBool();
}

bool DRS4SettingsManager::ignoreBusyState() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_ignoreBusyNode->getValue().toBool();
}

double DRS4SettingsManager::cfdLevelA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_cfdLevelANode->getValue().toDouble();
}

double DRS4SettingsManager::cfdLevelB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_cfdLevelBNode->getValue().toDouble();
}

int DRS4SettingsManager::startCell() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_startCellNode->getValue().toInt();
}

int DRS4SettingsManager::stopCell() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_stopCellNode->getValue().toInt();
}

bool DRS4SettingsManager::isforceCoincidence() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_bForceCoincidenceNode->getValue().toBool();
}

bool DRS4SettingsManager::isBurstMode() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_burstModeNode->getValue().toBool();
}

double DRS4SettingsManager::offsetInNSAB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_offsetNsABNode->getValue().toDouble();
}

double DRS4SettingsManager::offsetInNSBA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_offsetNsBANode->getValue().toDouble();
}

double DRS4SettingsManager::offsetInNSCoincidence() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_offsetNsCoincidenceNode->getValue().toDouble();
}

double DRS4SettingsManager::offsetInNSMerged() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_offsetNsMergedNode->getValue().toDouble();
}

double DRS4SettingsManager::scalerInNSAB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_scalerNsABNode->getValue().toDouble();
}

double DRS4SettingsManager::scalerInNSBA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_scalerNsBANode->getValue().toDouble();
}

double DRS4SettingsManager::scalerInNSCoincidence() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_scalerNsCoincidenceNode->getValue().toDouble();
}

double DRS4SettingsManager::scalerInNSMerged() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_scalerNsMergedNode->getValue().toDouble();
}

int DRS4SettingsManager::channelCntAB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_channelCountABNode->getValue().toInt();
}

int DRS4SettingsManager::channelCntBA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_channelCountBANode->getValue().toInt();
}

int DRS4SettingsManager::channelCntCoincindence() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_channelCountCoincidenceNode->getValue().toInt();
}

int DRS4SettingsManager::channelCntMerged() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_channelCountMergedNode->getValue().toInt();
}

int DRS4SettingsManager::triggerSource_index() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_triggerSourceNode->getValue().toInt();
}

int DRS4SettingsManager::triggerSource() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    const int tiggerSource_id = m_triggerSourceNode->getValue().toInt();

    const int chn_A_index = DRS4SettingsManager::sharedInstance()->channelNumberA();
    const int chn_B_index = DRS4SettingsManager::sharedInstance()->channelNumberB();

    switch ( tiggerSource_id )
    {
    case 0: // A
        return (1<<chn_A_index);
    case 1: // B
        return (1<<chn_B_index);
    case 2: // A && B
        return (1<<(8+chn_A_index))|(1<<(8+chn_B_index));
    case 3: // A || B
        return ((1<<chn_A_index)|(1<<chn_B_index));
    case 4: // external
        return ((1<<4)|(1<<12));

    default:
        break;
    }

    return 0;
}

bool DRS4SettingsManager::isTriggerPolarityPositive() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_isTriggerPolarityPositiveNode->getValue().toBool();
}

double DRS4SettingsManager::triggerLevelAmV() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_triggerLevelANode->getValue().toDouble();
}

double DRS4SettingsManager::triggerLevelBmV() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_triggerLevelBNode->getValue().toDouble();
}

double DRS4SettingsManager::triggerDelayInNs() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_triggerDelayInNSNode->getValue().toDouble();
}

QString DRS4SettingsManager::comment() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_commentNode->getValue().toString();
}

QString DRS4SettingsManager::lastSaveDate() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_lastSaveDateNode->getValue().toString();
}

int DRS4SettingsManager::channelNumberA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_channelNumberANode->getValue().toInt();
}

int DRS4SettingsManager::channelNumberB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_channelNumberBNode->getValue().toInt();
}

int DRS4SettingsManager::startChanneAMin() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_startAChannelMinNode->getValue().toInt();
}

int DRS4SettingsManager::startChanneAMax() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_startAChannelMaxNode->getValue().toInt();
}

int DRS4SettingsManager::startChanneBMin() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_startBChannelMinNode->getValue().toInt();
}

int DRS4SettingsManager::startChanneBMax() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_startBChannelMaxNode->getValue().toInt();
}

int DRS4SettingsManager::stopChanneAMin() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_stopAChannelMinNode->getValue().toInt();
}

int DRS4SettingsManager::stopChanneAMax() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_stopAChannelMaxNode->getValue().toInt();
}

int DRS4SettingsManager::stopChanneBMin() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_stopBChannelMinNode->getValue().toInt();
}

int DRS4SettingsManager::stopChanneBMax() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_stopBChannelMaxNode->getValue().toInt();
}

double DRS4SettingsManager::meanCableDelay() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_meanNSNode->getValue().toDouble();
}

int DRS4SettingsManager::fitIterations() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_fitIterNode->getValue().toInt();
}

int DRS4SettingsManager::fitIterationsMerged() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_fitIterMergedNode->getValue().toInt();
}

int DRS4SettingsManager::fitIterationsAB() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_fitIterABNode->getValue().toInt();
}

int DRS4SettingsManager::fitIterationsBA() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_fitIterBANode->getValue().toInt();
}

double DRS4SettingsManager::sweepInNanoseconds() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_sweepInNanoSecNode->getValue().toDouble();
}

double DRS4SettingsManager::sampleSpeedInGHz() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_freqInGHzNode->getValue().toDouble();
}

QString DRS4SettingsManager::fileName() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    return m_fileName;
}

QString DRS4SettingsManager::xmlContent() const
{
#ifndef __DISABLE_MUTEX_LOCKER
    QMutexLocker locker(&m_mutex);
#endif

    if (m_parentNode)
        return (QString)DSimpleXMLString(m_parentNode);

    return QString("");
}
