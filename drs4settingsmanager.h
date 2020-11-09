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

#ifndef DRS4SETTINGSMANAGER_H
#define DRS4SETTINGSMANAGER_H

#include <QDateTime>

#include <QMutex>
#include <QMutexLocker>

#include "DLib.h"
#include "Fit/dspline.h"
#include "dversion.h"

#include "alglib.h"

#define __PULSESHAPEFILTER_LEFT_MAX -200.0 /* [ns] */
#define __PULSESHAPEFILTER_RIGHT_MAX 200.0  /* [ns] */

#define __PULSESHAPEFILTER_REGION 400.0 /* [ns] */

#define __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER 4381

struct DRS4PulseShapeFilterRecordScheme {
    enum Scheme : int {
        RC_AB = 0,
        RC_BA = 1,
        RC_Prompt = 2
    };
};

class DRS4PulseShapeFilterData
{
    QVector<QPointF> m_meanTrace;
    QVector<QPointF> m_stdDevTrace;

    std::vector<double> m_meanTraceDataX, m_meanTraceDataY;
    std::vector<double> m_stdDevTraceDataX, m_stdDevTraceDataY;

    float *m_meanTraceDataXArray;
    float *m_meanTraceDataYArray;

    float *m_stdDevTraceDataXArray;
    float *m_stdDevTraceDataYArray;

    int m_sizeOfFloat;

    DSpline m_meanTraceSpline;
    DSpline m_stddevTraceSpline;

public:
    DRS4PulseShapeFilterData() {
        m_sizeOfFloat = 1/sizeof(float);

        m_meanTraceDataXArray = DNULLPTR;
        m_meanTraceDataYArray = DNULLPTR;
        m_stdDevTraceDataXArray = DNULLPTR;
        m_stdDevTraceDataYArray = DNULLPTR;

        m_meanTrace.clear();
        m_stdDevTrace.clear();

        m_meanTraceDataX.clear();
        m_stdDevTraceDataX.clear();

        m_meanTraceDataY.clear();
        m_stdDevTraceDataY.clear();
    }

    virtual ~DRS4PulseShapeFilterData() {
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
    }

    DRS4PulseShapeFilterData& operator=(const DRS4PulseShapeFilterData& copy);

public:
    void setData(const QVector<QPointF>& mean, const QVector<QPointF>& stddev);
    bool isInsideBounding(const double& x, const double& y, const double& lowerFraction, const double &upperFraction) const;

    double getMeanAt(unsigned int index) const {
        if (index >= m_meanTrace.size())
            return 0.0;

       return m_meanTrace.at(index).y();
    }

    double getStdDevAt(unsigned int index) const {
        if (index >= m_stdDevTrace.size())
            return 0.0;

        return m_stdDevTrace.at(index).y();
    }

    double getFractUpperStdDevValueAt(unsigned int index, const double& fraction = 1.0f) const {
        const double mean = getMeanAt(index);
        const double stddev = getStdDevAt(index);

        return (mean + fraction*stddev);
    }

    double getFractLowerStdDevValueAt(unsigned int index, const double& fraction = 1.0f) const {
        const double mean = getMeanAt(index);
        const double stddev = getStdDevAt(index);

        return (mean - fraction*stddev);
    }

    QVector<QPointF> mean() const {
        return m_meanTrace;
    }

    QVector<QPointF> stddev() const {
        return m_stdDevTrace;
    }

    void meanCpy(float x[], float y[]) {
        if (!x || !y)
            return;

        const int sizeOfFloat = sizeof(float);

        if (m_meanTraceDataXArray)
            memcpy(x, m_meanTraceDataXArray, __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER*sizeOfFloat);

        if (m_meanTraceDataYArray)
            memcpy(y, m_meanTraceDataYArray, __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER*sizeOfFloat);
    }

    void stddevCpy(float x[], float y[]) {
        if (!x || !y)
            return;

        const int sizeOfFloat = sizeof(float);

        if (m_stdDevTraceDataXArray)
            memcpy(x, m_stdDevTraceDataXArray, __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER*sizeOfFloat);

        if (m_stdDevTraceDataYArray)
            memcpy(y, m_stdDevTraceDataYArray, __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER*sizeOfFloat);
    }

    QVector<QPointF> stddevUpper(const double& fraction = 1.0) const {
        QVector<QPointF> vec;
        for (int i = 0 ; i < m_meanTrace.size() ; ++ i) {
            vec.append(QPointF(m_meanTrace.at(i).x(), getFractUpperStdDevValueAt(i, fraction)));
        }

        return vec;
    }

    QVector<QPointF> stddevLower(const double& fraction = 1.0) const {
        QVector<QPointF> vec;
        for (int i = 0 ; i < m_meanTrace.size() ; ++ i) {
            vec.append(QPointF(m_meanTrace.at(i).x(), getFractLowerStdDevValueAt(i, fraction)));
        }

        return vec;
    }

private:
    void setMeanTrace(const QVector<QPointF>& vec) {
        m_meanTrace.clear();
        m_meanTrace.append(vec);
    }

    void setStddevTrace(const QVector<QPointF>& vec) {
        m_stdDevTrace.clear();
        m_stdDevTrace.append(vec);
    }
};

typedef struct {
public:
    static QStringList typeList() {
        QStringList list;
        list.append("Polynomial - ALGLIB");
        list.append("Spline & Fitting");

        return list;
    }

    enum type : int {
        unknown = 0,
        polynomial = 1,
        spline = 2
    };
} DRS4InterpolationType;

typedef struct {
public:
    static QStringList typeList() {
        QStringList type;
        type.append("Linear");
        type.append("Cubic - ALGLIB");
        type.append("Akima - ALGLIB");
        type.append("Catmull-Rom - ALGLIB");
        type.append("Monotone - ALGLIB");
        type.append("Cubic - Tino Kluge");

        return type;
    }

    enum type : int {
        unknown = 0,
        linear = 1,
        /* ALGLIB */
        cubic = 2,
        akima = 3,
        catmullRom = 4,
        monotone = 5,
        /* Tino Kluge */
        tk_cubic = 6
    };
} DRS4SplineInterpolationType;

class DRS4SettingsManager
{
    DRS4SettingsManager();
    virtual ~DRS4SettingsManager();

    QString m_fileName;
    QString m_comment;
    QString m_lastSaveDate;

    bool m_burstMode;
    bool m_negativeLifetimes;

    bool m_ignoreBusy;

    double m_cfdLevelA;
    double m_cfdLevelB;

    int m_startCell;
    int m_stopCell;

    double m_offsetNsAB, m_offsetNsBA, m_offsetNsCoincidence, m_offsetNsMerged;
    double m_scalerNsAB, m_scalerNsBA, m_scalerNsCoincidence, m_scalerNsMerged;
    int m_channelCountAB, m_channelCountBA, m_channelCountCoincidence, m_channelCountMerged;

    double m_meanNs;

    int m_triggerSource;

    bool m_isPositivSignal;
    bool m_isTriggerPolarityPositive;

    int m_channelNumberA;
    int m_channelNumberB;

    double m_triggerLevelA;
    double m_triggerLevelB;

    double m_triggerDelayInNS;

    int m_startAChannelMin, m_startAChannelMax;
    int m_stopAChannelMin, m_stopAChannelMax;

    int m_startBChannelMin, m_startBChannelMax;
    int m_stopBChannelMin, m_stopBChannelMax;

    bool m_bForceCoincidence;

    int m_fitIter;
    int m_fitIterMerged;
    int m_fitIterAB;
    int m_fitIterBA;

    double m_sweepInNanoSec;
    double m_freqInGHz;

    bool m_bPulseAreaFilterEnabled;
    bool m_bPulseAreaFilterEnabledPlot;

    double m_pulseAreaNormA;
    double m_pulseAreaNormB;

    int m_pulseAreaBinningA;
    int m_pulseAreaBinningB;

    double m_pulseAreaLeftUpper_A, m_pulseAreaRightUpper_A;
    double m_pulseAreaLeftLower_A, m_pulseAreaRightLower_A;

    double m_pulseAreaLeftUpper_B, m_pulseAreaRightUpper_B;
    double m_pulseAreaLeftLower_B, m_pulseAreaRightLower_B;

    double m_persistance_leftAInNs, m_persistance_rightAInNs;
    double m_persistance_leftBInNs, m_persistance_rightBInNs;

    bool m_persistanceEnabled;

    bool m_riseTimeFilterIsPlotEnabled;
    bool m_riseTimeFilterIsActivated;

    double m_riseTimeFilter_scaleInNs_A;
    double m_riseTimeFilter_scaleInNs_B;

    int m_riseTimeFilter_scaleBinningCnt_A;
    int m_riseTimeFilter_scaleBinningCnt_B;

    int m_riseTimeFilter_leftWindow_A;
    int m_riseTimeFilter_leftWindow_B;

    int m_riseTimeFilter_rightWindow_A;
    int m_riseTimeFilter_rightWindow_B;

    int m_pulseShapeFilter_numberOfPulsesAcq_A;
    int m_pulseShapeFilter_numberOfPulsesAcq_B;

    double m_pulseShapeFilter_leftAInNs, m_pulseShapeFilter_rightAInNs;
    double m_pulseShapeFilter_leftBInNs, m_pulseShapeFilter_rightBInNs;

    double m_pulseShapeFilter_ROIleftAInNs, m_pulseShapeFilter_ROIrightAInNs;
    double m_pulseShapeFilter_ROIleftBInNs, m_pulseShapeFilter_ROIrightBInNs;

    double m_pulseShapeFilter_StdDevUpperFractA;
    double m_pulseShapeFilter_StdDevLowerFractA;

    double m_pulseShapeFilter_StdDevUpperFractB;
    double m_pulseShapeFilter_StdDevLowerFractB;

    DRS4PulseShapeFilterData m_pulseShapeFilterDataA, m_pulseShapeFilterDataB;

    bool m_pulseShapeFilterEnabledA, m_pulseShapeFilterEnabledB;

    DRS4PulseShapeFilterRecordScheme::Scheme m_pulseShapeFilterRecordScheme;

    int m_baseLineCorrectionStartCellA;
    int m_baseLineCorrectionRegionA;
    double m_baseLineCorrectionShiftValueA;
    bool m_baseLineCorrectionEnabledA;
    double m_baseLineCorrectionLimitA;
    bool m_baseLineCorrectionLimitExceededRejectA;

    int m_baseLineCorrectionStartCellB;
    int m_baseLineCorrectionRegionB;
    double m_baseLineCorrectionShiftValueB;
    bool m_baseLineCorrectionEnabledB;
    double m_baseLineCorrectionLimitB;
    bool m_baseLineCorrectionLimitExceededRejectB;

    DSimpleXMLNode *m_parentNode;
    DSimpleXMLNode *m_versionNode;
    DSimpleXMLNode *m_generalSettingsNode;
    DSimpleXMLNode *m_cfdDeterminationSettingsNode;
    DSimpleXMLNode *m_cfdAlgorithmTypeNode;
    DSimpleXMLNode *m_cfdAlgorithmType_polynomialInterpolationNode;
    DSimpleXMLNode *m_polynomialIntraSamplingPointsNode;
    DSimpleXMLNode *m_cfdAlgorithmType_splineAndFittingInterpolationNode;
    DSimpleXMLNode *m_splineTypeNode;
    DSimpleXMLNode *m_splineTypeLinearNode;
    DSimpleXMLNode *m_splineTypeLinearEnabledNode;
    DSimpleXMLNode *m_splineTypeCubicNode;
    DSimpleXMLNode *m_splineTypeCubicEnabledNode;
    DSimpleXMLNode *m_splineTypeAkimaNode;
    DSimpleXMLNode *m_splineTypeAkimaEnabledNode;
    DSimpleXMLNode *m_splineTypeCatmullRomNode;
    DSimpleXMLNode *m_splineTypeCatmullRomEnabledNode;
    DSimpleXMLNode *m_splineTypeMonotoneNode;
    DSimpleXMLNode *m_splineTypeMonotoneEnabledNode;
    DSimpleXMLNode *m_splineTypeTKCubicNode;
    DSimpleXMLNode *m_splineTypeTKCubicEnabledNode;
    DSimpleXMLNode *m_splineIntraSamplingPointsNode;
    DSimpleXMLNode *m_phsSettingsNode;
    DSimpleXMLNode *m_spectrumSettingsNode;
    DSimpleXMLNode *m_abSpecNode, *m_baSpecNode, *m_mergedSpecNode, *m_promptSpecNode;
    DSimpleXMLNode *m_areaFilterSettingsNode;
    DSimpleXMLNode *m_medianFilterSettingsNode;
    DSimpleXMLNode *m_riseTimeFilterSettingsNode;
    DSimpleXMLNode *m_pulseShapeFilterSettingsNode;
    DSimpleXMLNode *m_persistancePlotSettingsNode;
    DSimpleXMLNode *m_startAChannelMinNode, *m_startAChannelMaxNode;
    DSimpleXMLNode *m_startBChannelMinNode, *m_startBChannelMaxNode;
    DSimpleXMLNode *m_stopAChannelMinNode, *m_stopAChannelMaxNode;
    DSimpleXMLNode *m_stopBChannelMinNode, *m_stopBChannelMaxNode;
    DSimpleXMLNode *m_burstModeNode;
    DSimpleXMLNode *m_negativeLifetimesNode;
    DSimpleXMLNode *m_ignoreBusyNode;
    DSimpleXMLNode *m_cfdLevelANode;
    DSimpleXMLNode *m_cfdLevelBNode;
    DSimpleXMLNode *m_startCellNode;
    DSimpleXMLNode *m_stopCellNode;
    DSimpleXMLNode *m_offsetNsABNode;
    DSimpleXMLNode *m_offsetNsBANode;
    DSimpleXMLNode *m_offsetNsCoincidenceNode;
    DSimpleXMLNode *m_offsetNsMergedNode;
    DSimpleXMLNode *m_scalerNsABNode;
    DSimpleXMLNode *m_scalerNsBANode;
    DSimpleXMLNode *m_scalerNsCoincidenceNode;
    DSimpleXMLNode *m_scalerNsMergedNode;
    DSimpleXMLNode *m_channelCountABNode;
    DSimpleXMLNode *m_channelCountBANode;
    DSimpleXMLNode *m_channelCountCoincidenceNode;
    DSimpleXMLNode *m_channelCountMergedNode;
    DSimpleXMLNode *m_triggerSourceNode;
    DSimpleXMLNode *m_isPositivSignalNode;
    DSimpleXMLNode *m_sweepInNanoSecNode;
    DSimpleXMLNode *m_freqInGHzNode;
    DSimpleXMLNode *m_meanNSNode;
    DSimpleXMLNode *m_isTriggerPolarityPositiveNode;
    DSimpleXMLNode *m_triggerLevelANode;
    DSimpleXMLNode *m_triggerLevelBNode;
    DSimpleXMLNode *m_triggerDelayInNSNode;
    DSimpleXMLNode *m_commentNode;
    DSimpleXMLNode *m_lastSaveDateNode;
    DSimpleXMLNode *m_channelNumberANode;
    DSimpleXMLNode *m_channelNumberBNode;
    DSimpleXMLNode *m_bForceCoincidenceNode;
    DSimpleXMLNode *m_fitIterNode;
    DSimpleXMLNode *m_fitIterMergedNode;
    DSimpleXMLNode *m_fitIterABNode;
    DSimpleXMLNode *m_fitIterBANode;
    DSimpleXMLNode *m_pulseAreaFilerEnabledNode;
    DSimpleXMLNode *m_pulseAreaFilerEnabledPlotNode;
    DSimpleXMLNode *m_pulseAreaBinningANode;
    DSimpleXMLNode *m_pulseAreaBinningBNode;
    DSimpleXMLNode *m_pulseAreaNormANode;
    DSimpleXMLNode *m_pulseAreaNormBNode;
    DSimpleXMLNode *m_pulseAreaLeftUpper_A_Node;
    DSimpleXMLNode *m_pulseAreaRightUpper_A_Node;
    DSimpleXMLNode *m_pulseAreaLeftLower_A_Node;
    DSimpleXMLNode *m_pulseAreaRightLower_A_Node;
    DSimpleXMLNode *m_pulseAreaLeftUpper_B_Node;
    DSimpleXMLNode *m_pulseAreaRightUpper_B_Node;
    DSimpleXMLNode *m_pulseAreaLeftLower_B_Node;
    DSimpleXMLNode *m_pulseAreaRightLower_B_Node;
    DSimpleXMLNode *m_medianFilterActivated_A_Node;
    DSimpleXMLNode *m_medianFilterWindowSize_A_Node;
    DSimpleXMLNode *m_medianFilterActivated_B_Node;
    DSimpleXMLNode *m_medianFilterWindowSize_B_Node;
    DSimpleXMLNode *m_persistanceUsingRefB_A_Node;
    DSimpleXMLNode *m_persistanceUsingRefA_B_Node;
    DSimpleXMLNode *m_persistance_leftAInNs_Node;
    DSimpleXMLNode *m_persistance_leftBInNs_Node;
    DSimpleXMLNode *m_persistance_rightAInNs_Node;
    DSimpleXMLNode *m_persistance_rightBInNs_Node;
    DSimpleXMLNode *m_persistanceEnabled_Node;
    DSimpleXMLNode *m_riseTimeFilterIsPlotEnabled_Node;
    DSimpleXMLNode *m_riseTimeFilterIsActivated_Node;
    DSimpleXMLNode *m_riseTimeFilter_scaleInNs_A_Node;
    DSimpleXMLNode *m_riseTimeFilter_scaleInNs_B_Node;
    DSimpleXMLNode *m_riseTimeFilter_scaleBinningCnt_A_Node;
    DSimpleXMLNode *m_riseTimeFilter_scaleBinningCnt_B_Node;
    DSimpleXMLNode *m_riseTimeFilter_leftWindow_A_Node;
    DSimpleXMLNode *m_riseTimeFilter_leftWindow_B_Node;
    DSimpleXMLNode *m_riseTimeFilter_rightWindow_A_Node;
    DSimpleXMLNode *m_riseTimeFilter_rightWindow_B_Node;

    DSimpleXMLNode *m_pulseShapeFilter_numberOfPulsesAcq_A_Node;
    DSimpleXMLNode *m_pulseShapeFilter_numberOfPulsesAcq_B_Node;

    DSimpleXMLNode *m_pulseShapeFilter_leftAInNs_Node;
    DSimpleXMLNode *m_pulseShapeFilter_leftBInNs_Node;
    DSimpleXMLNode *m_pulseShapeFilter_rightAInNs_Node;
    DSimpleXMLNode *m_pulseShapeFilter_rightBInNs_Node;

    DSimpleXMLNode *m_pulseShapeFilter_ROIleftAInNs_Node;
    DSimpleXMLNode *m_pulseShapeFilter_ROIleftBInNs_Node;
    DSimpleXMLNode *m_pulseShapeFilter_ROIrightAInNs_Node;
    DSimpleXMLNode *m_pulseShapeFilter_ROIrightBInNs_Node;

    DSimpleXMLNode *m_pulseShapeFilter_StdDevUpperFractA_Node;
    DSimpleXMLNode *m_pulseShapeFilter_StdDevLowerFractA_Node;

    DSimpleXMLNode *m_pulseShapeFilter_StdDevUpperFractB_Node;
    DSimpleXMLNode *m_pulseShapeFilter_StdDevLowerFractB_Node;

    DSimpleXMLNode *m_pulseShapeFilter_contentDataA_Node;
    DSimpleXMLNode *m_pulseShapeFilter_contentDataB_Node;

    DSimpleXMLNode *m_pulseShapeFilter_meanDataA_Node;
    DSimpleXMLNode *m_pulseShapeFilter_meanDataB_Node;

    DSimpleXMLNode *m_pulseShapeFilter_stddevDataA_Node;
    DSimpleXMLNode *m_pulseShapeFilter_stddevDataB_Node;

    DSimpleXMLNode *m_pulseShapeFilterEnabledA_Node;
    DSimpleXMLNode *m_pulseShapeFilterEnabledB_Node;

    DSimpleXMLNode *m_pulseShapeFilterRecordScheme_Node;

    DSimpleXMLNode *m_baseLineFilterSettingsNode;

    DSimpleXMLNode *m_baseLineCorrectionStartCellA_Node;
    DSimpleXMLNode *m_baseLineCorrectionRegionA_Node;
    DSimpleXMLNode *m_baseLineCorrectionShiftValueA_Node;
    DSimpleXMLNode *m_baseLineCorrectionEnabledA_Node;
    DSimpleXMLNode *m_baseLineCorrectionLimitA_Node;
    DSimpleXMLNode *m_baseLineCorrectionLimitExceededRejectA_Node;

    DSimpleXMLNode *m_baseLineCorrectionStartCellB_Node;
    DSimpleXMLNode *m_baseLineCorrectionRegionB_Node;
    DSimpleXMLNode *m_baseLineCorrectionShiftValueB_Node;
    DSimpleXMLNode *m_baseLineCorrectionEnabledB_Node;
    DSimpleXMLNode *m_baseLineCorrectionLimitB_Node;
    DSimpleXMLNode *m_baseLineCorrectionLimitExceededRejectB_Node;

    mutable QMutex m_mutex;

public:
    static DRS4SettingsManager *sharedInstance();

    bool load(const QString& path);
    bool save(const QString& path, bool autosave = false);

    DSimpleXMLNode *parentNode() const;

    QMutex *mutex();

public:
    void parsePulseShapeData(DSimpleXMLNode *node, QVector<QPointF> *filterData);

    void setForceCoincidence(bool force);
    void setBurstMode(bool on);

    void setStartCell(int startCell);
    void setStopCell(int stopCell);

    void setPositivSignal(bool positiv);

    void setNegativeLifetimeAccepted(bool accepted);

    void setIgnoreBusyState(bool ignore);

    void setCFDLevelA(double level);
    void setCFDLevelB(double level);

    void setOffsetInNSAB(double offset);
    void setOffsetInNSBA(double offset);
    void setOffsetInNSCoincidence(double offset);
    void setOffsetInNSMerged(double offset);

    void setScalerInNSAB(double scaler);
    void setScalerInNSBA(double scaler);
    void setScalerInNSCoincidence(double scaler);
    void setScalerInNSMerged(double scaler);

    void setChannelCntAB(int count);
    void setChannelCntBA(int count);
    void setChannelCntCoincindence(int count);
    void setChannelCntMerged(int count);

    void setTriggerSource(int triggerSource);
    void setTriggerPolarityPositive(bool positive);

    void setTriggerLevelAmV(double triggerLevelInmV);
    void setTriggerLevelBmV(double triggerLevelInmV);

    void setTriggerDelayInNs(double delayInNs);

    void setComment(const QString& comment);
    void setLastSaveDate(const QString& lastSaveDate);

    void setChannelNumberA(int channelNumber);
    void setChannelNumberB(int channelNumber);

    void setStartChannelA(int min, int max);
    void setStartChannelB(int min, int max);

    void setStopChannelA(int min, int max);
    void setStopChannelB(int min, int max);

    void setStartChannelAMin(int min);
    void setStartChannelBMin(int min);
    void setStartChannelAMax(int max);
    void setStartChannelBMax(int max);

    void setStopChannelAMin(int min);
    void setStopChannelBMin(int min);
    void setStopChannelAMax(int max);
    void setStopChannelBMax(int max);

    void setMeanCableDelayInNs(double ns);

    void setFitIterations(int count);
    void setFitIterationsMerged(int count);
    void setFitIterationsAB(int count);
    void setFitIterationsBA(int count);

    void setSweepInNanoseconds(double ns);
    void setSampleSpeedInGHz(double ghz);

    void setPulseAreaFilterEnabled(bool enabled);
    void setPulseAreaFilterPlotEnabled(bool enabled);

    void setPulseAreaFilterNormalizationA(double percentage);
    void setPulseAreaFilterNormalizationB(double percentage);

    void setPulseAreaFilterBinningA(int binning);
    void setPulseAreaFilterBinningB(int binning);

    void setPulseAreaFilterLimitUpperLeftA(double value);
    void setPulseAreaFilterLimitUpperRightA(double value);
    void setPulseAreaFilterLimitLowerLeftA(double value);
    void setPulseAreaFilterLimitLowerRightA(double value);

    void setPulseAreaFilterLimitUpperLeftB(double value);
    void setPulseAreaFilterLimitUpperRightB(double value);
    void setPulseAreaFilterLimitLowerLeftB(double value);
    void setPulseAreaFilterLimitLowerRightB(double value);

    void setRiseTimeFilterEnabled(bool enabled);
    void setRiseTimeFilterPlotEnabled(bool enabled);

    void setRiseTimeFilterScaleInNanosecondsOfA(double value);
    void setRiseTimeFilterScaleInNanosecondsOfB(double value);
    void setRiseTimeFilterBinningOfA(int value);
    void setRiseTimeFilterBinningOfB(int value);

    void setRiseTimeFilterLeftWindowOfA(int value);
    void setRiseTimeFilterLeftWindowOfB(int value);
    void setRiseTimeFilterRightWindowOfA(int value);
    void setRiseTimeFilterRightWindowOfB(int value);

    void setPersistanceEnabled(bool activated);
    void setPersistanceLeftInNsOfA(double value);
    void setPersistanceLeftInNsOfB(double value);
    void setPersistanceRightInNsOfA(double value);
    void setPersistanceRightInNsOfB(double value);

    void setPersistanceUsingCFDBAsRefForA(bool on);
    void setPersistanceUsingCFDAAsRefForB(bool on);

    void setInterpolationType(const DRS4InterpolationType::type &type);
    void setSplineInterpolationType(const DRS4SplineInterpolationType::type& type);
    void setSplineIntraSamplingCounts(int counts);
    void setPolynomialSamplingCounts(int counts);

    void setMedianFilterAEnabled(bool enabled);
    void setMedianFilterBEnabled(bool enabled);

    void setMedianFilterWindowSizeA(int size);
    void setMedianFilterWindowSizeB(int size);

    void setPulseShapeFilterNumberOfPulsesToBeRecordedA(int number);
    void setPulseShapeFilterNumberOfPulsesToBeRecordedB(int number);

    void setPulseShapeFilterLeftInNsOfA(double value);
    void setPulseShapeFilterLeftInNsOfB(double value);
    void setPulseShapeFilterRightInNsOfA(double value);
    void setPulseShapeFilterRightInNsOfB(double value);

    void setPulseShapeFilterROILeftInNsOfA(double value);
    void setPulseShapeFilterROILeftInNsOfB(double value);
    void setPulseShapeFilterROIRightInNsOfA(double value);
    void setPulseShapeFilterROIRightInNsOfB(double value);

    void setPulseShapeFilterStdDevUpperFractionA(double value);
    void setPulseShapeFilterStdDevLowerFractionA(double value);

    void setPulseShapeFilterStdDevUpperFractionB(double value);
    void setPulseShapeFilterStdDevLowerFractionB(double value);

    void setPulseShapeFilterDataA(const DRS4PulseShapeFilterData& data, bool lockMutex = true);
    void setPulseShapeFilterDataB(const DRS4PulseShapeFilterData& data, bool lockMutex = true);

    void setPulseShapeFilterEnabledA(bool enabled);
    void setPulseShapeFilterEnabledB(bool enabled);

    void setPulseShapeFilterRecordScheme(const DRS4PulseShapeFilterRecordScheme::Scheme& rc);

    void setBaselineCorrectionCalculationStartCellA(int cell);
    void setBaselineCorrectionCalculationRegionA(int region);
    void setBaselineCorrectionCalculationEnabledA(bool enabled);
    void setBaselineCorrectionCalculationShiftValueInMVA(double value);
    void setBaselineCorrectionCalculationLimitInPercentageA(double limit);
    void setBaselineCorrectionCalculationLimitRejectLimitA(bool reject);

    int baselineCorrectionCalculationStartCellA() const;
    int baselineCorrectionCalculationRegionA() const;
    bool baselineCorrectionCalculationEnabledA() const;
    double baselineCorrectionCalculationShiftValueInMVA() const;
    double baselineCorrectionCalculationLimitInPercentageA() const;
    bool baselineCorrectionCalculationLimitRejectLimitA() const;

    void setBaselineCorrectionCalculationStartCellB(int cell);
    void setBaselineCorrectionCalculationRegionB(int region);
    void setBaselineCorrectionCalculationEnabledB(bool enabled);
    void setBaselineCorrectionCalculationShiftValueInMVB(double value);
    void setBaselineCorrectionCalculationLimitInPercentageB(double limit);
    void setBaselineCorrectionCalculationLimitRejectLimitB(bool reject);

    int baselineCorrectionCalculationStartCellB() const;
    int baselineCorrectionCalculationRegionB() const;
    bool baselineCorrectionCalculationEnabledB() const;
    double baselineCorrectionCalculationShiftValueInMVB() const;
    double baselineCorrectionCalculationLimitInPercentageB() const;
    bool baselineCorrectionCalculationLimitRejectLimitB() const;

    bool pulseShapeFilterEnabledA() const;
    bool pulseShapeFilterEnabledB() const;

    DRS4PulseShapeFilterRecordScheme::Scheme pulseShapeFilterRecordScheme() const;

    int pulseShapeFilterNumberOfPulsesToBeRecordedA() const;
    int pulseShapeFilterNumberOfPulsesToBeRecordedB() const;

    double pulseShapeFilterLeftInNsOfA() const;
    double pulseShapeFilterLeftInNsOfB() const;
    double pulseShapeFilterRightInNsOfA() const;
    double pulseShapeFilterRightInNsOfB() const;

    double pulseShapeFilterROILeftInNsOfA() const;
    double pulseShapeFilterROILeftInNsOfB() const;
    double pulseShapeFilterROIRightInNsOfA() const;
    double pulseShapeFilterROIRightInNsOfB() const;

    double pulseShapeFilterStdDevUpperFractionA() const;
    double pulseShapeFilterStdDevLowerFractionA() const;

    double pulseShapeFilterStdDevUpperFractionB() const;
    double pulseShapeFilterStdDevLowerFractionB() const;

    DRS4PulseShapeFilterData pulseShapeFilterDataA(bool lockMutex = true) const;
    DRS4PulseShapeFilterData pulseShapeFilterDataB(bool lockMutex = true) const;

    DRS4PulseShapeFilterData* pulseShapeFilterDataPtrA();
    DRS4PulseShapeFilterData* pulseShapeFilterDataPtrB();

    bool medianFilterAEnabled() const;
    bool medianFilterBEnabled() const;

    int medianFilterWindowSizeA() const;
    int medianFilterWindowSizeB() const;

    DRS4InterpolationType::type interpolationType() const;
    DRS4SplineInterpolationType::type splineInterpolationType() const;
    int splineIntraSamplingCounts() const;
    int polynomialSamplingCounts() const;

    bool isPersistanceEnabled() const;
    double persistanceLeftInNsOfA() const;
    double persistanceLeftInNsOfB() const;
    double persistanceRightInNsOfA() const;
    double persistanceRightInNsOfB() const;

    bool persistanceUsingCFDBAsRefForA() const;
    bool persistanceUsingCFDAAsRefForB() const;

    bool isPulseAreaFilterEnabled() const;
    bool isPulseAreaFilterPlotEnabled() const;

    double pulseAreaFilterNormalizationA() const;
    double pulseAreaFilterNormalizationB() const;

    int pulseAreaFilterBinningA() const;
    int pulseAreaFilterBinningB() const;

    double pulseAreaFilterLimitUpperLeftA() const;
    double pulseAreaFilterLimitUpperRightA() const;
    double pulseAreaFilterLimitLowerLeftA() const;
    double pulseAreaFilterLimitLowerRightA() const;

    double pulseAreaFilterLimitUpperLeftB() const;
    double pulseAreaFilterLimitUpperRightB() const;
    double pulseAreaFilterLimitLowerLeftB() const;
    double pulseAreaFilterLimitLowerRightB() const;

    bool isRiseTimeFilterEnabled() const;
    bool isRiseTimeFilterPlotEnabled() const;

    double riseTimeFilterScaleInNanosecondsOfA() const;
    double riseTimeFilterScaleInNanosecondsOfB() const;
    int riseTimeFilterBinningOfA() const;
    int riseTimeFilterBinningOfB() const;

    int riseTimeFilterLeftWindowOfA() const;
    int riseTimeFilterLeftWindowOfB() const;
    int riseTimeFilterRightWindowOfA() const;
    int riseTimeFilterRightWindowOfB() const;

    bool isPositiveSignal() const;
    bool isNegativeLTAccepted() const;
    bool ignoreBusyState() const;

    double cfdLevelA() const;
    double cfdLevelB() const;

    int startCell() const;
    int stopCell() const;

    bool isforceCoincidence() const;

    bool isBurstMode() const;

    double offsetInNSAB() const;
    double offsetInNSBA() const;
    double offsetInNSCoincidence() const;
    double offsetInNSMerged() const;

    double scalerInNSAB() const;
    double scalerInNSBA() const;
    double scalerInNSCoincidence() const;
    double scalerInNSMerged() const;

    int channelCntAB() const;
    int channelCntBA() const;
    int channelCntCoincindence() const;
    int channelCntMerged() const;

    int triggerSource() const;
    bool isTriggerPolarityPositive() const;

    double triggerLevelAmV() const;
    double triggerLevelBmV() const;

    double triggerDelayInNs() const;

    QString comment() const;
    QString lastSaveDate() const;

    int channelNumberA() const;
    int channelNumberB() const;

    int startChanneAMin() const;
    int startChanneAMax() const;

    int startChanneBMin() const;
    int startChanneBMax() const;

    int stopChanneAMin() const;
    int stopChanneAMax() const;

    int stopChanneBMin() const;
    int stopChanneBMax() const;

    double meanCableDelay() const;

    int fitIterations() const;
    int fitIterationsMerged() const;
    int fitIterationsAB() const;
    int fitIterationsBA() const;

    double sweepInNanoseconds() const;
    double sampleSpeedInGHz() const;

    QString fileName() const;

    QString xmlContent() const;
};

#endif // DRS4SETTINGSMANAGER_H
