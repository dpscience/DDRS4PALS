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

#ifndef DRS4SETTINGSMANAGER_H
#define DRS4SETTINGSMANAGER_H

#include <QDateTime>

#include <QMutex>
#include <QMutexLocker>

#include "DLib.h"

#include "dversion.h"

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

    mutable QMutex m_mutex;

public:
    static DRS4SettingsManager *sharedInstance();

    bool load(const QString& path);
    bool save(const QString& path, bool autosave = false);

    DSimpleXMLNode *parentNode() const;

public:
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
