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

#ifndef DRS4WORKER_H
#define DRS4WORKER_H

#include <QObject>
#include <QPointF>
#include <QList>
#include <QVector>
#include <QMetaType>
#include <QApplication>

#include <QRunnable>
#include <QThread>
#include <QThreadPool>
#include <QtConcurrent>
#include <QMutex>
#include <QMutexLocker>

#include <random>
#include <stdio.h>
#include <time.h>
#include <sys/utime.h>
#include <chrono>
#include <ctime>

#include "alglib.h"

#include "drs4boardmanager.h"
#include "drs4settingsmanager.h"
#include "drs4pulsegenerator.h"

#include "DQuickLTFit/projectmanager.h"

#include "Stream/drs4streammanager.h"

#include "Fit/dspline.h"

#define __STATISTIC_AVG_TIME 4.0f // [s]

using namespace QtConcurrent;

class DSpline;

class DRS4WorkerConcurrentManager;
class DRS4ConcurrentCopyInputData;
class DRS4ConcurrentCopyOutputData;
class DRS4WorkerDataExchange;
class DRS4Worker;

class DRS4ConcurrentCopyInputData final {
public:
    int m_startCell;
    int m_endRange;
    int m_stopCellWidth;
    int m_cellWidth;

    bool m_positiveSignal;
    double m_cfdA;
    double m_cfdB;
    bool m_bBurstMode;
    double m_sweep;
    bool m_bPulseAreaPlot;
    bool m_bPulseAreaFilter;
    bool m_bPulseRiseTimeFilter;

    DRS4InterpolationType::type m_interpolationType;
    DRS4SplineInterpolationType::type m_splineInterpolationType;
    bool m_bUsingALGLIB;
    bool m_bUsingTinoKluge;
    bool m_bUsingLinearInterpol;
    int m_intraRenderPoints;

    bool m_bPersistance;

    int m_pulseAreaFilterBinningA;
    int m_pulseAreaFilterBinningB;
    double m_pulseAreaFilterNormA;
    double m_pulseAreaFilterNormB;

    double m_areaFilterASlopeUpper;
    double m_areaFilterAInterceptUpper;

    double m_areaFilterASlopeLower;
    double m_areaFilterAInterceptLower;

    double m_areaFilterBSlopeUpper;
    double m_areaFilterBInterceptUpper;

    double m_areaFilterBSlopeLower;
    double m_areaFilterBInterceptLower;

    double m_riseTimeFilterARangeInNanoseconds;
    double m_riseTimeFilterBRangeInNanoseconds;

    int m_riseTimeFilterBinningA;
    int m_riseTimeFilterBinningB;

    int m_riseTimeFilterLeftWindowA;
    int m_riseTimeFilterLeftWindowB;

    int m_riseTimeFilterRightWindowA;
    int m_riseTimeFilterRightWindowB;

    bool m_pulseShapeFilterEnabledA;
    bool m_pulseShapeFilterEnabledB;

    double m_pulseShapeFilterLeftInNsROIA;
    double m_pulseShapeFilterRightInNsROIA;

    double m_pulseShapeFilterLeftInNsROIB;
    double m_pulseShapeFilterRightInNsROIB;

    double m_pulseShapeFilterFractOfStdDevUpperA;
    double m_pulseShapeFilterFractOfStdDevLowerA;

    double m_pulseShapeFilterFractOfStdDevUpperB;
    double m_pulseShapeFilterFractOfStdDevLowerB;

    bool m_pulseShapeFilterAIsRecording;
    bool m_pulseShapeFilterBIsRecording;

    DRS4PulseShapeFilterRecordScheme::Scheme m_rcScheme;

    float m_pulseShapeFilterDataMeanTraceA_X[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];
    float m_pulseShapeFilterDataMeanTraceA_Y[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];

    float m_pulseShapeFilterDataMeanTraceB_X[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];
    float m_pulseShapeFilterDataMeanTraceB_Y[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];

    float m_pulseShapeFilterDataStdDevTraceA_X[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];
    float m_pulseShapeFilterDataStdDevTraceA_Y[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];

    float m_pulseShapeFilterDataStdDevTraceB_X[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];
    float m_pulseShapeFilterDataStdDevTraceB_Y[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER];

    bool m_bMedianFilterA;
    bool m_bMedianFilterB;

    int m_medianFilterWindowSizeA;
    int m_medianFilterWindowSizeB;

    int m_channelCntAB;
    int m_channelCntBA;
    int m_channelCntPrompt;
    int m_channelCntMerged;

    double m_offsetAB;
    double m_offsetBA;
    double m_offsetPrompt;
    double m_offsetMerged;

    double m_scalerAB;
    double m_scalerBA;
    double m_scalerPrompt;
    double m_scalerMerged;

    int m_startAMinPHS, m_startAMaxPHS;
    int m_startBMinPHS, m_startBMaxPHS;

    int m_stopAMinPHS, m_stopAMaxPHS;
    int m_stopBMinPHS, m_stopBMaxPHS;

    double m_ATS;

    bool m_bNegativeLT;
    bool m_bForcePrompt;

    float m_tChannel0[kNumberOfBins];
    float m_tChannel1[kNumberOfBins];

    float m_waveChannel0[kNumberOfBins];
    float m_waveChannel1[kNumberOfBins];
};

class DRS4WorkerDataExchange final
{
public:
    double *m_areaFilterASlopeUpper;
    double *m_areaFilterAInterceptUpper;

    double *m_areaFilterASlopeLower;
    double *m_areaFilterAInterceptLower;

    double *m_areaFilterBSlopeUpper;
    double *m_areaFilterBInterceptUpper;

    double *m_areaFilterBSlopeLower;
    double *m_areaFilterBInterceptLower;


    DRS4WorkerDataExchange::DRS4WorkerDataExchange(double *areaFilterASlopeUpper = DNULLPTR,
                            double *areaFilterAInterceptUpper = DNULLPTR,
                            double *areaFilterASlopeLower = DNULLPTR,
                            double *areaFilterAInterceptLower = DNULLPTR,
                            double *areaFilterBSlopeUpper = DNULLPTR,
                            double *areaFilterBInterceptUpper = DNULLPTR,
                            double *areaFilterBSlopeLower = DNULLPTR,
                            double *areaFilterBInterceptLower = DNULLPTR)
    {
        m_areaFilterASlopeUpper = areaFilterASlopeUpper;
        m_areaFilterAInterceptUpper = areaFilterAInterceptUpper;
        m_areaFilterASlopeLower = areaFilterASlopeLower;
        m_areaFilterAInterceptLower = areaFilterAInterceptLower;
        m_areaFilterBSlopeUpper = areaFilterBSlopeUpper;
        m_areaFilterBInterceptUpper = areaFilterBInterceptUpper;
        m_areaFilterBSlopeLower = areaFilterBSlopeLower;
        m_areaFilterBInterceptLower = areaFilterBInterceptLower;
    }

    DRS4WorkerDataExchange::~DRS4WorkerDataExchange() {}
};

class DRS4Worker : public QObject
{
    Q_OBJECT

    friend class DRS4WorkerConcurrentManager;


    bool m_nextSignal;
    bool m_isBlocking;
    bool m_isRunning;

    DRS4WorkerDataExchange *m_dataExchange;

    mutable QMutex m_mutex;

    DRS4WorkerConcurrentManager *m_workerConcurrentManager;
    QVector<DRS4ConcurrentCopyInputData> m_copyData;

    /* Pulse-Scope */
    QVector<QPointF> m_pListChannelA, m_pListChannelB;
    QVector<QPointF> m_pListChannelASpline, m_pListChannelBSpline;

    /* ALGLIB */
    alglib::real_1d_array m_arrayDataX_A, m_arrayDataY_A;
    alglib::real_1d_array m_arrayDataX_B, m_arrayDataY_B;

    alglib::spline1dinterpolant m_interpolantA;
    alglib::spline1dinterpolant m_interpolantB;

    alglib::barycentricinterpolant m_baryCentricInterpolantA;
    alglib::barycentricinterpolant m_baryCentricInterpolantB;

    /* Tino Kluge */
    std::vector<double> m_arrayDataTKX_A, m_arrayDataTKY_A;
    std::vector<double> m_arrayDataTKX_B, m_arrayDataTKY_B;

public:
    /* PHS */
    QVector<int> m_phsA, m_phsB;

    int m_phsACounts, m_phsBCounts;

private:
    double m_summedPulseCountRateInSeconds;
    double m_currentPulseCountRateInSeconds;
    double m_avgPulseCountRateInSeconds;

    int m_pulseCounterCnt, m_pulseCounterCntAvg;

public:
    /* Area-Filter */
    QVector<QPointF> m_areaFilterDataA;
    QVector<QPointF> m_areaFilterDataB;

    QVector<QPointF> m_areaFilterCollectedDataA;
    QVector<QPointF> m_areaFilterCollectedDataB;

    QVector<int> m_areaFilterCollectedDataCounterA;
    QVector<int> m_areaFilterCollectedDataCounterB;

    int m_areaFilterACounter;
    int m_areaFilterBCounter;

    int m_areaFilterCollectedACounter;
    int m_areaFilterCollectedBCounter;

    /* Rise-Time Filter */
    QVector<int> m_riseTimeFilterDataA;
    QVector<int> m_riseTimeFilterDataB;

    int m_riseTimeFilterACounter;
    int m_riseTimeFilterBCounter;

    int m_maxY_RiseTimeSpectrumA, m_maxY_RiseTimeSpectrumB;

    /* Lifetime-Spectra */
    QVector<int> m_lifeTimeDataAB, m_lifeTimeDataBA, m_lifeTimeDataCoincidence, m_lifeTimeDataMerged;
    int m_abCounts, m_baCounts, m_mergedCounts, m_coincidenceCounts;
    int m_maxY_ABSpectrum, m_maxY_BASpectrum, m_maxY_CoincidenceSpectrum, m_maxY_MergedSpectrum;

    QDateTime m_startAqAB;
    QDateTime m_startAqBA;
    QDateTime m_startAqPrompt;
    QDateTime m_startAqMerged;

private:
    double m_summedABSpecCountRateInSeconds, m_summedBASpecCountRateInSeconds, m_summedMergedSpecCountRateInSeconds,m_summedCoincidenceSpecCountRateInSeconds;
    double m_currentABSpecCountRateInSeconds, m_currentBASpecCountRateInSeconds, m_currentMergedSpecCountRateInSeconds, m_currentCoincidenceSpecCountRateInSeconds;
    double m_avgABSpecCountRateInSeconds, m_avgBASpecCountRateInSeconds, m_avgMergedSpecCountRateInSeconds, m_avgCoincidenceSpecCountRateInSeconds;

public:
    int m_specABCounterCnt, m_specABCounterCntAvg;
    int m_specBACounterCnt, m_specBACounterCntAvg;
    int m_specMergedCounterCnt, m_specMergedCounterCntAvg;
    int m_specCoincidenceCounterCnt, m_specCoincidencCounterCntAvg;

    /* Persistance - Data */
    QVector<QPointF> m_persistanceDataA;
    QVector<QPointF> m_persistanceDataB;

    /* Pulse Shape - Data */
    QVector<QPointF> m_pulseShapeDataA;
    QVector<QPointF> m_pulseShapeDataB;

    QVector<DSpline> m_pulseShapeDataSplineA;
    QVector<DSpline> m_pulseShapeDataSplineB;

    int m_pulseShapeDataACounter;
    int m_pulseShapeDataBCounter;

    int m_pulseShapeDataAmountA;
    int m_pulseShapeDataAmountB;

    bool m_isRecordingForShapeFilterA;
    bool m_isRecordingForShapeFilterB;

public:
    explicit DRS4Worker(DRS4WorkerDataExchange *dataExchange, QObject *parent = 0);
    virtual ~DRS4Worker();

    /* handshaking between threads */
    bool nextSignal() const;
    bool isBlocking() const;

    bool isRunning() const;

signals:
    void started();
    void stopped();

    void startedRecordingForPulseShapeFilterA();
    void startedRecordingForPulseShapeFilterB();

    void stoppedRecordingForPulseShapeFilterA();
    void stoppedRecordingForPulseShapeFilterB();

public slots:
    void initDRS4Worker();
    void start();
    void stop();

    /* Pulse - Shape Filter */
    void startRecordingForShapeFilterA(int numberOfPulses);
    void startRecordingForShapeFilterB(int numberOfPulses);

    void recordPulseShapeData(bool positiveSignal, double timeStampA, double timeStampB, double yMinA, double yMaxA, double yMinB, double yMaxB);

    QVector<QPointF> calculateMeanTraceA() const;
    QVector<QPointF> calculateMeanTraceB() const;

    QVector<QPointF> calculateStdDevTraceA(const QVector<QPointF>& meanTrace) const;
    QVector<QPointF> calculateStdDevTraceB(const QVector<QPointF>& meanTrace) const;

    void stopRecordingForShapeFilterA();
    void stopRecordingForShapeFilterB();

    bool isRecordingForPulseShapeFilterA();
    bool isRecordingForPulseShapeFilterB();

    int pulseShapeFilterRecordingProgressA();
    int pulseShapeFilterRecordingProgressB();


    void setBusy(bool busy);

    void run();

private:
    void runSingleThreaded();
    void runMultiThreaded();

#ifdef __DEPRECATED_WORKER
    void calcLifetimesInBurstMode(DRS4LifetimeData *ltData, QVector<QPointF> *persistanceA, QVector<QPointF> *persistanceB);
#endif

    /* Pulse-Scope */
    void resetPulseSplines();

    void resetPulseA();
    void resetPulseB();

    /* Lifetime-Spectra */
    void resetLifetimeEfficiencyCounter();

public:
    /* Pulse-Scope */
    QVector<QPointF>* pulseDataA();
    QVector<QPointF>* pulseDataB();

    QVector<QPointF>* pulseSplineDataA();
    QVector<QPointF>* pulseSplineDataB();

    /* PHS */
    void resetPHSA();
    void resetPHSB();

    QVector<int>* phsA();
    QVector<int>* phsB();

    int phsACounts() const;
    int phsBCounts() const;

    double avgPulseCountRateInHz() const;
    double currentPulseCountRateInHz() const;

    /* Area-Filter */
    void resetAreaFilterA();
    void resetAreaFilterB();

    QVector<QPointF>* areaFilterAData();
    QVector<QPointF>* areaFilterBData();

    QVector<QPointF>* areaFilterACollectedData();
    QVector<int>* cntsAreaFilterACollectedData();
    QVector<QPointF>* areaFilterBCollectedData();
    QVector<int>* cntsAreaFilterBCollectedData();

    int countsCollectedInAreaFilterA();
    int countsCollectedInAreaFilterB();

    /* Rise-Time Filter */
    void resetRiseTimeFilterA();
    void resetRiseTimeFilterB();

    QVector<int>* riseTimeFilterAData();
    QVector<int> *riseTimeFilterBData();

    int riseTimeFilterADataMax();
    int riseTimeFilterBDataMax();

    /* Lifetime-Spectra */
    void resetABSpectrum();
    void resetBASpectrum();
    void resetMergedSpectrum();
    void resetCoincidenceSpectrum();

    QVector<int>* spectrumAB();
    QVector<int>* spectrumBA();
    QVector<int>* spectrumMerged();
    QVector<int>* spectrumCoincidence();

    int countsSpectrumAB() const;
    int countsSpectrumBA() const;
    int countsSpectrumMerged() const;
    int countsSpectrumCoincidence() const;

    int maxYValueABSpectrum() const;
    int maxYValueBASpectrum() const;
    int maxYValueMergedSpectrum() const;
    int maxYValueCoincidenceSpectrum() const;

    double avgLifetimeABCountRateInHz() const;
    double currentLifetimeABCountRateInHz() const;

    double avgLifetimeBACountRateInHz() const;
    double currentLifetimeBACountRateInHz() const;

    double avgLifetimeMergedCountRateInHz() const;
    double currentLifetimeMergedCountRateInHz() const;

    double avgLifetimeCoincidenceCountRateInHz() const;
    double currentLifetimeCoincidenceCountRateInHz() const;

    /* Persistance - Data */
    QVector<QPointF> *persistanceDataA();
    QVector<QPointF> *persistanceDataB();

    /* Pulse Shape - Data */
    void resetPulseShapeFilterA();
    void resetPulseShapeFilterB();

    QVector<QPointF> *pulseShapeDataA();
    QVector<QPointF> *pulseShapeDataB();

    DRS4PulseShapeFilterData pulseShapeFilterSplineDataA() const;
    DRS4PulseShapeFilterData pulseShapeFilterSplineDataB() const;

    int activeThreads() const;
    int maxThreads() const;
};

class DRS4ConcurrentCopyOutputData final {
public:
    /* data to be manipulated */

    /* PHS */
    QVector<int> m_phsA, m_phsB; /* stores the index */

    /* Lifetime-Spectra */
    QVector<int> m_lifeTimeDataAB, m_lifeTimeDataBA, m_lifeTimeDataCoincidence, m_lifeTimeDataMerged; /* stores the index */

    /* Area-Filter */
    QVector<QPointF> m_areaFilterDataA, m_areaFilterDataB;
    QVector<QPointF> m_areaFilterCollectionDataA, m_areaFilterCollectionDataB; /*mean and stddev calculation */

    /* Rise-Time Filter */
    QVector<int> m_riseTimeFilterDataA, m_riseTimeFilterDataB;

    /* Pulse Shape - Data */
    QVector<QVector<QPointF> > m_pulseShapeDataA;
    QVector<QVector<QPointF> > m_pulseShapeDataB;

    QVector<DSpline> m_pulseShapeDataSplineA;
    QVector<DSpline> m_pulseShapeDataSplineB;

    /* input data */
    int m_channelCntCoincindence;
    int m_channelCntAB;
    int m_channelCntBA;
    int m_channelCntMerged;

    bool m_bReject;

    DRS4ConcurrentCopyOutputData() : m_bReject(true) {}
    DRS4ConcurrentCopyOutputData(int channelCntCoincindence, int channelCntAB, int channelCntBA, int channelCntMerged, bool bReject = true) :
        m_channelCntCoincindence(channelCntCoincindence),
        m_channelCntAB(channelCntAB),
        m_channelCntBA(channelCntBA),
        m_channelCntMerged(channelCntMerged),
        m_bReject(bReject) {}

    virtual ~DRS4ConcurrentCopyOutputData() {}

    inline bool rejectData() const {
        return m_bReject;
    }
};

DRS4ConcurrentCopyOutputData runCalculation(const QVector<DRS4ConcurrentCopyInputData>& copyDataVec);

class DRS4WorkerConcurrentManager : public QObject
{
    Q_OBJECT

    friend class DRS4Worker;
    friend class DRS4WorkerDataExchange;

    DRS4Worker *m_worker;

    QVector<QVector<DRS4ConcurrentCopyInputData> > m_copyData;
    QVector<QVector<DRS4ConcurrentCopyInputData> > m_copyMetaData;

    QFuture<DRS4ConcurrentCopyOutputData> m_future;

    int m_recommendedThreads;

    DRS4WorkerConcurrentManager(DRS4Worker *worker) :
        m_worker(worker),
        m_recommendedThreads(QThread::idealThreadCount()) {
        QThreadPool::globalInstance()->setMaxThreadCount(m_recommendedThreads);
    }

    virtual ~DRS4WorkerConcurrentManager() {}

    bool add(const QVector<DRS4ConcurrentCopyInputData>& copyData);

    void start();
    void cancel();
    void merge();

    int activeThreads() const;
    int maxThreads() const;
};

#endif // DRS4WORKER_H
