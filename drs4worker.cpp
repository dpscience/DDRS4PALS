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

#include "drs4worker.h"

#include "Stream/drs4streamdataloader.h"
#include "DLib/DMath/dmedianfilter.h"

DRS4Worker::DRS4Worker(DRS4WorkerDataExchange *dataExchange, QObject *parent) :
    m_dataExchange(dataExchange),
    QObject(parent),
    m_nextSignal(true),
    m_isBlocking(false),
    m_isRunning(false),
    m_isRecordingForShapeFilterA(false),
    m_isRecordingForShapeFilterB(false),
    m_pulseShapeDataAmountA(0),
    m_pulseShapeDataAmountB(0) {
    m_workerConcurrentManager = new DRS4WorkerConcurrentManager(this);

    resetPHSA();
    resetPHSB();

    resetAreaFilterA();
    resetAreaFilterB();

    resetRiseTimeFilterA();
    resetRiseTimeFilterB();

    resetPulseShapeFilterA();
    resetPulseShapeFilterB();

    resetMergedSpectrum();

    resetLifetimeEfficiencyCounter();
}

DRS4Worker::~DRS4Worker() {
    DDELETE_SAFETY(m_workerConcurrentManager);
}

void DRS4Worker::initDRS4Worker() {}

void DRS4Worker::start() {
    m_copyData.clear();

    if ( !DRS4BoardManager::sharedInstance()->currentBoard() ) {
        DRS4BoardManager::sharedInstance()->setDemoMode(true);
    }

    resetLifetimeEfficiencyCounter();

    emit started();

    run();
}

void DRS4Worker::stop()
{
    m_copyData.clear();
    m_isRunning = false;

    emit stopped();
}

void DRS4Worker::startRecordingForShapeFilterA(int numberOfPulses)
{
    resetPulseShapeFilterA();

    m_pulseShapeDataAmountA = numberOfPulses;
    m_isRecordingForShapeFilterA = true;

    emit startedRecordingForPulseShapeFilterA();
}

void DRS4Worker::startRecordingForShapeFilterB(int numberOfPulses)
{
    resetPulseShapeFilterB();

    m_pulseShapeDataAmountB = numberOfPulses;
    m_isRecordingForShapeFilterB = true;

    emit startedRecordingForPulseShapeFilterB();
}

void DRS4Worker::recordPulseShapeData(bool positiveSignal, double timeStampA, double timeStampB, double yMinA, double yMaxA, double yMinB, double yMaxB)
{
    if (m_isRecordingForShapeFilterA
            || m_isRecordingForShapeFilterB) {
        const float fractYMaxA = 1.0f/yMaxA;
        const float fractYMaxB = 1.0f/yMaxB;

        const float fractYMinA = 1.0f/yMinA;
        const float fractYMinB = 1.0f/yMinB;

        const int size = m_pListChannelA.size();

        bool bdirA = true;
        bool bdirB = true;

        std::vector<double> xSplineA, ySplineA;
        std::vector<double> xSplineB, ySplineB;

        if (m_isRecordingForShapeFilterA) {
            if (!(m_pulseShapeDataACounter%2))
                bdirA = true;
            else
                bdirA = false;
        }
        else {
            xSplineA.resize(0);
            ySplineA.resize(0);
        }

        if (m_isRecordingForShapeFilterB) {
            if (!(m_pulseShapeDataBCounter%2))
                bdirB = true;
            else
                bdirB = false;
        }
        else {
            xSplineB.resize(0);
            ySplineB.resize(0);
        }

        if (bdirA) {
            if (m_isRecordingForShapeFilterA) {
                for ( int j = 0 ; j < size ; ++ j ) {
                    const double tA = (m_pListChannelA.at(j).x() - timeStampA);
                    const bool bROIA = (tA >= __PULSESHAPEFILTER_LEFT_MAX) && (tA <= __PULSESHAPEFILTER_RIGHT_MAX);

                    if (bROIA) {
                        const float yA = positiveSignal?(m_pListChannelA.at(j).y()*fractYMaxA):(m_pListChannelA.at(j).y()*fractYMinA);

                        m_pulseShapeDataA.append(QPointF(tA, yA));

                        xSplineA.push_back(tA);
                        ySplineA.push_back(yA);
                    }
                }
            }
        }
        else {
            if (m_isRecordingForShapeFilterA) {
                for ( int j = size-1 ; j >= 0 ; -- j ) {
                    const double tA = (m_pListChannelA.at(j).x() - timeStampA);
                    const bool bROIA = (tA >= __PULSESHAPEFILTER_LEFT_MAX) && (tA <= __PULSESHAPEFILTER_RIGHT_MAX);

                    if (bROIA) {
                        const float yA = positiveSignal?(m_pListChannelA.at(j).y()*fractYMaxA):(m_pListChannelA.at(j).y()*fractYMinA);

                        m_pulseShapeDataA.append(QPointF(tA, yA));

                        xSplineA.push_back(tA);
                        ySplineA.push_back(yA);
                    }
                }

                if (!xSplineA.empty()) {
                    std::reverse(std::begin(xSplineA), std::end(xSplineA));
                    std::reverse(std::begin(ySplineA), std::end(ySplineA));
                }
            }
        }

        if (bdirB) {
            if (m_isRecordingForShapeFilterB) {
                for ( int j = 0 ; j < size ; ++ j ) {
                    const double tB = (m_pListChannelB.at(j).x() - timeStampB);
                    const bool bROIB = (tB >= __PULSESHAPEFILTER_LEFT_MAX) && (tB <= __PULSESHAPEFILTER_RIGHT_MAX);

                    if (bROIB) {
                        const float yB = positiveSignal?(m_pListChannelB.at(j).y()*fractYMaxB):(m_pListChannelB.at(j).y()*fractYMinB);

                        m_pulseShapeDataB.append(QPointF(tB, yB));

                        xSplineB.push_back(tB);
                        ySplineB.push_back(yB);
                    }
                }
            }
        }
        else {
            if (m_isRecordingForShapeFilterB) {
                for ( int j = size-1 ; j >= 0 ; -- j ) {
                    const double tB = (m_pListChannelB.at(j).x() - timeStampB);
                    const bool bROIB = (tB >= __PULSESHAPEFILTER_LEFT_MAX) && (tB <= __PULSESHAPEFILTER_RIGHT_MAX);

                    if (bROIB) {
                        const float yB = positiveSignal?(m_pListChannelB.at(j).y()*fractYMaxB):(m_pListChannelB.at(j).y()*fractYMinB);

                        m_pulseShapeDataB.append(QPointF(tB, yB));

                        xSplineB.push_back(tB);
                        ySplineB.push_back(yB);
                    }
                }

                if (!xSplineB.empty()) {
                    std::reverse(std::begin(xSplineB), std::end(xSplineB));
                    std::reverse(std::begin(ySplineB), std::end(ySplineB));
                }
            }
        }

        if (m_isRecordingForShapeFilterA && !xSplineA.empty() && !ySplineA.empty()) {
            DSpline spline(SplineType::Cubic);
            spline.setPoints(xSplineA, ySplineA);

            m_pulseShapeDataSplineA.append(spline);

            m_pulseShapeDataACounter ++;

            if (m_pulseShapeDataACounter == (m_pulseShapeDataAmountA + 1)) {
                m_isRecordingForShapeFilterA = false;
            }
        }

        if (m_isRecordingForShapeFilterB && !xSplineB.empty() && !ySplineB.empty()) {
            DSpline spline(SplineType::Cubic);
            spline.setPoints(xSplineB, ySplineB);

            m_pulseShapeDataSplineB.append(spline);

            m_pulseShapeDataBCounter ++;

            if (m_pulseShapeDataBCounter == (m_pulseShapeDataAmountB + 1)) {
                m_isRecordingForShapeFilterB = false;
            }
        }
    }
}

QVector<QPointF> DRS4Worker::calculateMeanTraceA() const
{
    const double timeIncr = __PULSESHAPEFILTER_REGION/__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER;
    const int size = m_pulseShapeDataSplineA.size();

    QVector<QPointF> meanVec;
    meanVec.fill(QPointF(0, 0), __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER);

    for (DSpline spline : m_pulseShapeDataSplineA) {
        for (int i = 0 ; i < __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER ; ++ i) {
            const double t = __PULSESHAPEFILTER_LEFT_MAX + (double)i*timeIncr;
            meanVec[i] += QPointF(t, spline(t));
        }
    }

    for (int i = 0 ; i < __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER ; ++ i) {
        meanVec[i] /= (double)size;
    }

    return meanVec;
}

QVector<QPointF> DRS4Worker::calculateMeanTraceB() const
{
    const double timeIncr = __PULSESHAPEFILTER_REGION/__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER;
    const int size = m_pulseShapeDataSplineB.size();

    QVector<QPointF> meanVec;
    meanVec.fill(QPointF(0, 0), __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER);

    for (DSpline spline : m_pulseShapeDataSplineB) {
        for (int i = 0 ; i < __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER ; ++ i) {
            const double t = __PULSESHAPEFILTER_LEFT_MAX + ((double)i)*timeIncr;
            meanVec[i] += QPointF(t, spline(t));
        }
    }

    for (int i = 0 ; i < __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER ; ++ i) {
        meanVec[i] /= (double)size;
    }

    return meanVec;
}

QVector<QPointF> DRS4Worker::calculateStdDevTraceA(const QVector<QPointF>& meanTrace) const
{
    const int size = m_pulseShapeDataSplineA.size();

    QVector<QPointF> stdDevVec;
    stdDevVec.fill(QPointF(0, 0), __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER);

    for (DSpline spline : m_pulseShapeDataSplineA) {
        for (int i = 0 ; i < __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER ; ++ i) {
            const double t = meanTrace.at(i).x();

            const double splineT = spline(t);

            const double x = (t - meanTrace.at(i).x())*(t - meanTrace.at(i).x()); // should be zero
            const double y = (splineT - meanTrace.at(i).y())*(splineT - meanTrace.at(i).y());

            stdDevVec[i].setX(stdDevVec[i].x() + x);
            stdDevVec[i].setY(stdDevVec[i].y() + y);
        }
    }

    for (int i = 0 ; i < __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER ; ++ i) {
        stdDevVec[i] /= (double)size;

        if (stdDevVec[i].x() > 0 )
            stdDevVec[i].setX(sqrt(stdDevVec[i].x()));
        else
             stdDevVec[i].setX(0.0f);

        if (stdDevVec[i].y() > 0 )
            stdDevVec[i].setY(sqrt(stdDevVec[i].y()));
        else
             stdDevVec[i].setY(0.0f);
    }

    return stdDevVec;
}

QVector<QPointF> DRS4Worker::calculateStdDevTraceB(const QVector<QPointF> &meanTrace) const
{
    const int size = m_pulseShapeDataSplineB.size();

    QVector<QPointF> stdDevVec;
    stdDevVec.fill(QPointF(0, 0), __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER);

    for (DSpline spline : m_pulseShapeDataSplineB) {
        for (int i = 0 ; i < __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER ; ++ i) {
            const double t = meanTrace.at(i).x();

            const double splineT = spline(t);

            const double x = (t - meanTrace.at(i).x())*(t - meanTrace.at(i).x());
            const double y = (splineT - meanTrace.at(i).y())*(splineT - meanTrace.at(i).y());

            stdDevVec[i].setX(stdDevVec[i].x() + x);
            stdDevVec[i].setY(stdDevVec[i].y() + y);
        }
    }

    for (int i = 0 ; i < __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER ; ++ i) {
        stdDevVec[i] /= (double)size;

        if (stdDevVec[i].x() > 0 )
            stdDevVec[i].setX(sqrt(stdDevVec[i].x()));
        else
             stdDevVec[i].setX(0.0f);

        if (stdDevVec[i].y() > 0 )
            stdDevVec[i].setY(sqrt(stdDevVec[i].y()));
        else
             stdDevVec[i].setY(0.0f);
    }

    return stdDevVec;
}

void DRS4Worker::stopRecordingForShapeFilterA()
{
    QMutexLocker locker(&m_mutex);

    m_isRecordingForShapeFilterA = false;

    emit stoppedRecordingForPulseShapeFilterA();
}

void DRS4Worker::stopRecordingForShapeFilterB()
{
    QMutexLocker locker(&m_mutex);

    m_isRecordingForShapeFilterB = false;

    emit stoppedRecordingForPulseShapeFilterB();
}

bool DRS4Worker::isRecordingForPulseShapeFilterA()
{
    QMutexLocker locker(&m_mutex);

    return m_isRecordingForShapeFilterA;
}

bool DRS4Worker::isRecordingForPulseShapeFilterB()
{
    QMutexLocker locker(&m_mutex);

    return m_isRecordingForShapeFilterB;
}

int DRS4Worker::pulseShapeFilterRecordingProgressA()
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeDataACounter;
}

int DRS4Worker::pulseShapeFilterRecordingProgressB()
{
    QMutexLocker locker(&m_mutex);

    return m_pulseShapeDataBCounter;
}

void DRS4Worker::setBusy(bool busy)
{
    QMutexLocker locker(&m_mutex);

    if (!m_isRunning)
        return;

    m_nextSignal = !busy;
}

void DRS4Worker::resetPHSA()
{
    QMutexLocker locker(&m_mutex);

    m_phsACounts = 0;

    m_phsA.fill(0, kNumberOfBins);

    m_currentPulseCountRateInSeconds = 0.0f;
    m_avgPulseCountRateInSeconds = 0.0f;
    m_summedPulseCountRateInSeconds = 0.0f;

    m_pulseCounterCnt = 0;
    m_pulseCounterCntAvg = 0;
}

void DRS4Worker::resetPHSB()
{
    QMutexLocker locker(&m_mutex);

    m_phsBCounts = 0;

    m_phsB.fill(0, kNumberOfBins);

    m_currentPulseCountRateInSeconds = 0.0f;
    m_avgPulseCountRateInSeconds = 0.0f;
    m_summedPulseCountRateInSeconds = 0.0f;

    m_pulseCounterCnt = 0;
    m_pulseCounterCntAvg = 0;
}

void DRS4Worker::resetPulseA()
{
    m_pListChannelA.clear();
}

void DRS4Worker::resetPulseB()
{
    m_pListChannelB.clear();
}

void DRS4Worker::resetPulseSplines()
{
    m_pListChannelASpline.clear();
    m_pListChannelBSpline.clear();
}

QVector<QPointF>* DRS4Worker::pulseDataA()
{
    QMutexLocker locker(&m_mutex);

    return &m_pListChannelA;
}

QVector<QPointF>* DRS4Worker::pulseDataB()
{
    QMutexLocker locker(&m_mutex);

    return &m_pListChannelB;
}

QVector<QPointF>* DRS4Worker::pulseSplineDataA()
{
    QMutexLocker locker(&m_mutex);

    return &m_pListChannelASpline;
}

QVector<QPointF>* DRS4Worker::pulseSplineDataB()
{
    QMutexLocker locker(&m_mutex);

    return &m_pListChannelBSpline;
}

QVector<int> *DRS4Worker::phsA()
{
    QMutexLocker locker(&m_mutex);

    return &m_phsA;
}

QVector<int> *DRS4Worker::phsB()
{
    QMutexLocker locker(&m_mutex);

    return &m_phsB;
}

int DRS4Worker::phsACounts() const
{
    QMutexLocker locker(&m_mutex);

    return m_phsACounts;
}

int DRS4Worker::phsBCounts() const
{
    QMutexLocker locker(&m_mutex);

    return m_phsBCounts;
}

double DRS4Worker::avgPulseCountRateInHz() const
{
    QMutexLocker locker(&m_mutex);

    return m_avgPulseCountRateInSeconds;
}

double DRS4Worker::currentPulseCountRateInHz() const
{
    QMutexLocker locker(&m_mutex);

    return m_currentPulseCountRateInSeconds;
}

void DRS4Worker::resetAreaFilterA()
{
    QMutexLocker locker(&m_mutex);

    m_areaFilterACounter = 0;
    m_areaFilterDataA.fill(QPointF(-1, -1), 5000);

    m_areaFilterCollectedACounter = 0;
    m_areaFilterCollectedDataA.fill(QPointF(0.0, 0.0), kNumberOfBins);
    m_areaFilterCollectedDataCounterA.fill(0, kNumberOfBins);
}

void DRS4Worker::resetAreaFilterB()
{
    QMutexLocker locker(&m_mutex);

    m_areaFilterBCounter = 0;
    m_areaFilterDataB.fill(QPointF(-1, -1), 5000);

    m_areaFilterCollectedBCounter = 0;
    m_areaFilterCollectedDataB.fill(QPointF(0.0, 0.0), kNumberOfBins);
    m_areaFilterCollectedDataCounterB.fill(0, kNumberOfBins);
}

QVector<QPointF> *DRS4Worker::areaFilterAData()
{
    QMutexLocker locker(&m_mutex);

    return &m_areaFilterDataA;
}

QVector<QPointF> *DRS4Worker::areaFilterBData()
{
    QMutexLocker locker(&m_mutex);

    return &m_areaFilterDataB;
}

QVector<QPointF> *DRS4Worker::areaFilterACollectedData()
{
    QMutexLocker locker(&m_mutex);

    return &m_areaFilterCollectedDataA;
}

QVector<int> *DRS4Worker::cntsAreaFilterACollectedData()
{
    QMutexLocker locker(&m_mutex);

    return &m_areaFilterCollectedDataCounterA;
}

QVector<QPointF> *DRS4Worker::areaFilterBCollectedData()
{
    QMutexLocker locker(&m_mutex);

    return &m_areaFilterCollectedDataB;
}

QVector<int> *DRS4Worker::cntsAreaFilterBCollectedData()
{
    QMutexLocker locker(&m_mutex);

    return &m_areaFilterCollectedDataCounterB;
}

int DRS4Worker::countsCollectedInAreaFilterA()
{
    QMutexLocker locker(&m_mutex);

    return m_areaFilterCollectedACounter;
}

int DRS4Worker::countsCollectedInAreaFilterB()
{
    QMutexLocker locker(&m_mutex);

    return m_areaFilterCollectedBCounter;
}

void DRS4Worker::resetRiseTimeFilterA()
{
    QMutexLocker locker(&m_mutex);

    m_riseTimeFilterACounter = 0;
    m_maxY_RiseTimeSpectrumA = 0;
    m_riseTimeFilterDataA.fill(-1, DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfA());
}

void DRS4Worker::resetRiseTimeFilterB()
{
    QMutexLocker locker(&m_mutex);

    m_riseTimeFilterBCounter = 0;
    m_maxY_RiseTimeSpectrumB = 0;
    m_riseTimeFilterDataB.fill(-1, DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfB());
}

QVector<int> *DRS4Worker::riseTimeFilterAData()
{
    QMutexLocker locker(&m_mutex);

    return &m_riseTimeFilterDataA;
}

QVector<int> *DRS4Worker::riseTimeFilterBData()
{
    QMutexLocker locker(&m_mutex);

    return &m_riseTimeFilterDataB;
}

int DRS4Worker::riseTimeFilterADataMax()
{
    QMutexLocker locker(&m_mutex);

    return m_maxY_RiseTimeSpectrumA;
}

int DRS4Worker::riseTimeFilterBDataMax()
{
    QMutexLocker locker(&m_mutex);

    return m_maxY_RiseTimeSpectrumB;
}

void DRS4Worker::resetABSpectrum()
{
    QMutexLocker locker(&m_mutex);

    m_abCounts = 0;
    m_lifeTimeDataAB.fill(0, DRS4SettingsManager::sharedInstance()->channelCntAB());

    m_maxY_ABSpectrum = 0;
}

void DRS4Worker::resetBASpectrum()
{
    QMutexLocker locker(&m_mutex);

    m_baCounts = 0;
    m_lifeTimeDataBA.fill(0, DRS4SettingsManager::sharedInstance()->channelCntBA());

    m_maxY_BASpectrum = 0;
}

void DRS4Worker::resetMergedSpectrum()
{
    QMutexLocker locker(&m_mutex);

    m_abCounts = 0;
    m_baCounts = 0;
    m_coincidenceCounts = 0;
    m_mergedCounts = 0;

    m_lifeTimeDataAB.fill(0, DRS4SettingsManager::sharedInstance()->channelCntAB());
    m_lifeTimeDataBA.fill(0, DRS4SettingsManager::sharedInstance()->channelCntBA());
    m_lifeTimeDataCoincidence.fill(0, DRS4SettingsManager::sharedInstance()->channelCntCoincindence());
    m_lifeTimeDataMerged.fill(0, DRS4SettingsManager::sharedInstance()->channelCntMerged());

    m_maxY_ABSpectrum = 0;
    m_maxY_BASpectrum = 0;
    m_maxY_MergedSpectrum = 0;
    m_maxY_CoincidenceSpectrum = 0;
}

void DRS4Worker::resetCoincidenceSpectrum()
{
    QMutexLocker locker(&m_mutex);

    m_coincidenceCounts = 0;
    m_lifeTimeDataCoincidence.fill(0, DRS4SettingsManager::sharedInstance()->channelCntCoincindence());

    m_maxY_CoincidenceSpectrum = 0;
}

QVector<int> *DRS4Worker::spectrumAB()
{
    QMutexLocker locker(&m_mutex);

    return &m_lifeTimeDataAB;
}

QVector<int> *DRS4Worker::spectrumBA()
{
    QMutexLocker locker(&m_mutex);

    return &m_lifeTimeDataBA;
}

QVector<int> *DRS4Worker::spectrumCoincidence()
{
    QMutexLocker locker(&m_mutex);

    return &m_lifeTimeDataCoincidence;
}

int DRS4Worker::countsSpectrumAB() const
{
    QMutexLocker locker(&m_mutex);

    return m_abCounts;
}

int DRS4Worker::countsSpectrumBA() const
{
    QMutexLocker locker(&m_mutex);

    return m_baCounts;
}

int DRS4Worker::countsSpectrumCoincidence() const
{
    QMutexLocker locker(&m_mutex);

    return m_coincidenceCounts;
}

int DRS4Worker::countsSpectrumMerged() const
{
    QMutexLocker locker(&m_mutex);

    return m_mergedCounts;
}

int DRS4Worker::maxYValueABSpectrum() const
{
    QMutexLocker locker(&m_mutex);

    return m_maxY_ABSpectrum;
}

int DRS4Worker::maxYValueBASpectrum() const
{
    QMutexLocker locker(&m_mutex);

    return m_maxY_BASpectrum;
}

int DRS4Worker::maxYValueMergedSpectrum() const
{
    QMutexLocker locker(&m_mutex);

    return m_maxY_MergedSpectrum;
}

int DRS4Worker::maxYValueCoincidenceSpectrum() const
{
    QMutexLocker locker(&m_mutex);

    return m_maxY_CoincidenceSpectrum;
}

void DRS4Worker::resetLifetimeEfficiencyCounter()
{
    QMutexLocker locker(&m_mutex);

    m_summedABSpecCountRateInSeconds = 0;
    m_summedBASpecCountRateInSeconds = 0;
    m_summedMergedSpecCountRateInSeconds = 0;
    m_summedCoincidenceSpecCountRateInSeconds = 0;

    m_currentABSpecCountRateInSeconds = 0;
    m_currentBASpecCountRateInSeconds = 0;
    m_currentMergedSpecCountRateInSeconds = 0;
    m_currentCoincidenceSpecCountRateInSeconds = 0;

    m_avgABSpecCountRateInSeconds = 0;
    m_avgBASpecCountRateInSeconds = 0;
    m_avgMergedSpecCountRateInSeconds = 0;
    m_avgCoincidenceSpecCountRateInSeconds = 0;

    m_specABCounterCnt = 0;
    m_specABCounterCntAvg = 0;
    m_specBACounterCnt = 0;
    m_specBACounterCntAvg = 0;
    m_specMergedCounterCnt = 0;
    m_specMergedCounterCntAvg = 0;
    m_specCoincidenceCounterCnt = 0;
    m_specCoincidencCounterCntAvg = 0;
}

double DRS4Worker::avgLifetimeABCountRateInHz() const
{
    QMutexLocker locker(&m_mutex);

    return m_avgABSpecCountRateInSeconds;
}

double DRS4Worker::currentLifetimeABCountRateInHz() const
{
    QMutexLocker locker(&m_mutex);

    return m_currentABSpecCountRateInSeconds;
}

double DRS4Worker::avgLifetimeBACountRateInHz() const
{
    QMutexLocker locker(&m_mutex);

    return m_avgBASpecCountRateInSeconds;
}

double DRS4Worker::currentLifetimeBACountRateInHz() const
{
    QMutexLocker locker(&m_mutex);

    return m_currentBASpecCountRateInSeconds;
}

double DRS4Worker::avgLifetimeMergedCountRateInHz() const
{
    QMutexLocker locker(&m_mutex);

    return m_avgMergedSpecCountRateInSeconds;
}

double DRS4Worker::currentLifetimeMergedCountRateInHz() const
{
    QMutexLocker locker(&m_mutex);

    return m_currentMergedSpecCountRateInSeconds;
}

double DRS4Worker::avgLifetimeCoincidenceCountRateInHz() const
{
    QMutexLocker locker(&m_mutex);

    return m_avgCoincidenceSpecCountRateInSeconds;
}

double DRS4Worker::currentLifetimeCoincidenceCountRateInHz() const
{
    QMutexLocker locker(&m_mutex);

    return m_currentCoincidenceSpecCountRateInSeconds;
}

QVector<QPointF> *DRS4Worker::persistanceDataA()
{
    QMutexLocker locker(&m_mutex);

    return &m_persistanceDataA;
}

QVector<QPointF> *DRS4Worker::persistanceDataB()
{
    QMutexLocker locker(&m_mutex);

    return &m_persistanceDataB;
}

void DRS4Worker::resetPulseShapeFilterA()
{
    QMutexLocker locker(&m_mutex);

    m_pulseShapeDataA.clear();
    m_pulseShapeDataSplineA.clear();

    m_pulseShapeDataACounter = 0;
}

void DRS4Worker::resetPulseShapeFilterB()
{
    QMutexLocker locker(&m_mutex);

    m_pulseShapeDataB.clear();
    m_pulseShapeDataSplineB.clear();

    m_pulseShapeDataBCounter = 0;
}

QVector<QPointF> *DRS4Worker::pulseShapeDataA()
{
    QMutexLocker locker(&m_mutex);

    return &m_pulseShapeDataA;
}

QVector<QPointF> *DRS4Worker::pulseShapeDataB()
{
    QMutexLocker locker(&m_mutex);

    return &m_pulseShapeDataB;
}

DRS4PulseShapeFilterData DRS4Worker::pulseShapeFilterSplineDataA() const
{
    QMutexLocker locker(&m_mutex);

    const QVector<QPointF> meanVec = calculateMeanTraceA();
    const QVector<QPointF> stddDevVec = calculateStdDevTraceA(meanVec);

    DRS4PulseShapeFilterData data;
    data.setData(meanVec, stddDevVec);

    return data;
}

DRS4PulseShapeFilterData DRS4Worker::pulseShapeFilterSplineDataB() const
{
    QMutexLocker locker(&m_mutex);

    const QVector<QPointF> meanVec = calculateMeanTraceB();
    const QVector<QPointF> stddDevVec = calculateStdDevTraceB(meanVec);

    DRS4PulseShapeFilterData data;
    data.setData(meanVec, stddDevVec);

    return data;
}

int DRS4Worker::activeThreads() const
{
    QMutexLocker locker(&m_mutex);

    return m_workerConcurrentManager->activeThreads();
}

int DRS4Worker::maxThreads() const
{
    QMutexLocker locker(&m_mutex);

    return m_workerConcurrentManager->maxThreads();
}

QVector<int> *DRS4Worker::spectrumMerged()
{
    QMutexLocker locker(&m_mutex);

    return &m_lifeTimeDataMerged;
}

bool DRS4Worker::nextSignal() const
{
    QMutexLocker locker(&m_mutex);

    return m_nextSignal;
}

bool DRS4Worker::isBlocking() const
{
    QMutexLocker locker(&m_mutex);

    if (!m_isRunning)
        return true;

    return m_isBlocking;
}

bool DRS4Worker::isRunning() const
{
    QMutexLocker locker(&m_mutex);

    return m_isRunning;
}

void DRS4Worker::run()
{
    if (DRS4ProgramSettingsManager::sharedInstance()->isMulticoreThreadingEnabled())
        runMultiThreaded();
    else
        runSingleThreaded();
}

void DRS4Worker::runSingleThreaded()
{
    DSpline tkSplineA, tkSplineB;

    const int sizeOfWave = sizeof(float)*kNumberOfBins;
    const int sizeOfFloat = 1/sizeof(float);

    m_isBlocking = false;
    m_isRunning = true;

    const bool bDemoMode = DRS4BoardManager::sharedInstance()->isDemoModeEnabled();
    const bool bIgnoreBusyState = DRS4SettingsManager::sharedInstance()->ignoreBusyState(); /* this value is deprecated and for test purposes only */

    if ( !bDemoMode )
        DRS4BoardManager::sharedInstance()->currentBoard()->StartDomino();

    time_t start;
    time_t stop;

    time(&start);
    time(&stop);

    forever {
        if ( !m_isRunning ) {
            m_isBlocking = false;

            return;
        }

        if (!bIgnoreBusyState) {
            if (!bDemoMode) {
                while ( !DRS4BoardManager::sharedInstance()->currentBoard()->IsEventAvailable()) {
                    while ( !nextSignal() ) {
                        m_isBlocking = true;
                    }

                    if ( !m_isRunning ) {
                        m_isBlocking = false;

                        return;
                    }
                }
            }
        }

        while ( !nextSignal() ) {
            m_isBlocking = true;
        }

        m_isBlocking = false;

        const int chnA = DRS4SettingsManager::sharedInstance()->channelNumberA();
        const int chnB = DRS4SettingsManager::sharedInstance()->channelNumberB();

        if (!bDemoMode) {
            try {
                const int minChn = qMin(chnA, chnB);
                const int maxChn = qMax(chnA, chnB);

                DRS4BoardManager::sharedInstance()->currentBoard()->TransferWaves(minChn, maxChn);
            }
            catch (...) {
                continue;
            }
        }

        float tChannel0[kNumberOfBins] = {0};
        float tChannel1[kNumberOfBins] = {0};

        float waveChannel0[kNumberOfBins] = {0};
        float waveChannel1[kNumberOfBins] = {0};

        std::fill(tChannel0, tChannel0 + sizeof(tChannel0)*sizeOfFloat, 0);
        std::fill(tChannel1, tChannel1 + sizeof(tChannel1)*sizeOfFloat, 0);

        std::fill(waveChannel0, waveChannel0 + sizeof(waveChannel0)*sizeOfFloat, 0);
        std::fill(waveChannel1, waveChannel1 + sizeof(waveChannel1)*sizeOfFloat, 0);

        /* the 'S' extension represents the holding as source (original) data set */
        float waveChannel0S[kNumberOfBins] = {0};
        float waveChannel1S[kNumberOfBins] = {0};

        std::fill(waveChannel0S, waveChannel0S + sizeof(waveChannel0S)*sizeOfFloat, 0);
        std::fill(waveChannel1S, waveChannel1S + sizeof(waveChannel1S)*sizeOfFloat, 0);

        if (!bDemoMode) {
            int retState = 1;
            int retStateT = 1;
            int retStateV = kSuccess;

            try {
                retStateT = DRS4BoardManager::sharedInstance()->currentBoard()->GetTime(0, 2*chnA, DRS4BoardManager::sharedInstance()->currentBoard()->GetTriggerCell(0), tChannel0);
            }
            catch ( ... ) {               
                continue;
            }

            if ( retStateT != 1 ) {
                continue;
            }

            try {
                retStateT = DRS4BoardManager::sharedInstance()->currentBoard()->GetTime(0, 2*chnB ,DRS4BoardManager::sharedInstance()->currentBoard()->GetTriggerCell(0), tChannel1);
            }
            catch ( ... ) {
                continue;
            }

            if ( retStateT != 1 ) {
                continue;
            }

            try {
                retStateV = DRS4BoardManager::sharedInstance()->currentBoard()->GetWave(0, 2*chnA, waveChannel0);
            }
            catch ( ... ) {
                continue;
            }

            if ( retStateV != kSuccess ) {
                continue;
            }

            try {
                retStateV = DRS4BoardManager::sharedInstance()->currentBoard()->GetWave(0, 2*chnB, waveChannel1);
            }
            catch ( ... ) {
                continue;
            }

            if ( retStateV != kSuccess ) {
                continue;
            }

            try {
                retState = DRS4BoardManager::sharedInstance()->currentBoard()->StartDomino(); // returns always 1.
            }
            catch ( ... ) {
                /* nothing here */
            }
        }
        else {
            if ( !DRS4BoardManager::sharedInstance()->usingStreamDataOnDemoMode() ) {
                if ( !DRS4PulseGenerator::sharedInstance()->receiveGeneratedPulsePair(tChannel0, waveChannel0, tChannel1, waveChannel1) )
                    continue;
            }
            else {
                if ( !DRS4StreamDataLoader::sharedInstance()->isArmed() )
                    continue;

                if ( !DRS4StreamDataLoader::sharedInstance()->receiveGeneratedPulsePair(tChannel0, waveChannel0, tChannel1, waveChannel1) ) {
                    continue;
                }
            }
        }

        /* statistics: */
        time(&stop);
        const double diffTime = difftime(stop, start);

        if ( diffTime >= __STATISTIC_AVG_TIME ) {
            m_currentPulseCountRateInSeconds = ((double)m_pulseCounterCnt)/diffTime;
            m_summedPulseCountRateInSeconds += m_currentPulseCountRateInSeconds;
            m_avgPulseCountRateInSeconds = (m_pulseCounterCntAvg==0)?m_currentPulseCountRateInSeconds:(m_summedPulseCountRateInSeconds/(double)m_pulseCounterCntAvg);

            m_currentABSpecCountRateInSeconds = ((m_specABCounterCnt==0)?0:((double)m_specABCounterCnt)/diffTime);
            m_summedABSpecCountRateInSeconds += m_currentABSpecCountRateInSeconds;
            m_avgABSpecCountRateInSeconds = (m_specABCounterCntAvg==0)?m_currentABSpecCountRateInSeconds:(m_summedABSpecCountRateInSeconds/(double)m_specABCounterCntAvg);

            m_currentBASpecCountRateInSeconds = ((m_specBACounterCnt==0)?0:((double)m_specBACounterCnt)/diffTime);
            m_summedBASpecCountRateInSeconds += m_currentBASpecCountRateInSeconds;
            m_avgBASpecCountRateInSeconds = (m_specBACounterCntAvg==0)?m_currentBASpecCountRateInSeconds:(m_summedBASpecCountRateInSeconds/(double)m_specBACounterCntAvg);

            m_currentMergedSpecCountRateInSeconds = ((m_specMergedCounterCnt==0)?0:((double)m_specMergedCounterCnt)/diffTime);
            m_summedMergedSpecCountRateInSeconds += m_currentMergedSpecCountRateInSeconds;
            m_avgMergedSpecCountRateInSeconds = (m_specMergedCounterCntAvg==0)?m_currentMergedSpecCountRateInSeconds:(m_summedMergedSpecCountRateInSeconds/(double)m_specMergedCounterCntAvg);

            m_currentCoincidenceSpecCountRateInSeconds = ((m_specCoincidenceCounterCnt==0)?0:((double)m_specCoincidenceCounterCnt)/diffTime);
            m_summedCoincidenceSpecCountRateInSeconds += m_currentCoincidenceSpecCountRateInSeconds;
            m_avgCoincidenceSpecCountRateInSeconds = (m_specCoincidencCounterCntAvg==0)?m_currentCoincidenceSpecCountRateInSeconds:(m_summedCoincidenceSpecCountRateInSeconds/(double)m_specCoincidencCounterCntAvg);

            time(&start);

            m_pulseCounterCnt = 0;
            m_pulseCounterCntAvg ++;

            m_specABCounterCnt = 0;
            m_specBACounterCnt = 0;
            m_specMergedCounterCnt = 0;
            m_specCoincidenceCounterCnt = 0;

            m_specABCounterCntAvg ++;
            m_specBACounterCntAvg ++;
            m_specMergedCounterCntAvg ++;
            m_specCoincidencCounterCntAvg ++;
        }

        m_pulseCounterCnt ++;


        /* ROI variables */
        const int startCell = DRS4SettingsManager::sharedInstance()->startCell();
        const int endRange = DRS4SettingsManager::sharedInstance()->stopCell();
        const int stopCellWidth = (kNumberOfBins - DRS4SettingsManager::sharedInstance()->stopCell());
        const int cellWidth = (kNumberOfBins - startCell - stopCellWidth);

        /* prevent mutex locking: call these functions only once within the loop */
        const bool positiveSignal = DRS4SettingsManager::sharedInstance()->isPositiveSignal();
        const double cfdA = DRS4SettingsManager::sharedInstance()->cfdLevelA();
        const double cfdB = DRS4SettingsManager::sharedInstance()->cfdLevelB();
        const bool bBurstMode = DRS4SettingsManager::sharedInstance()->isBurstMode();
        const double sweep = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();

        const bool bPulseAreaPlot = DRS4SettingsManager::sharedInstance()->isPulseAreaFilterPlotEnabled();
        const bool bPulseAreaFilter = DRS4SettingsManager::sharedInstance()->isPulseAreaFilterEnabled();
        const bool bPulseRiseTimeFilter = DRS4SettingsManager::sharedInstance()->isRiseTimeFilterEnabled();

        const DRS4PulseShapeFilterRecordScheme::Scheme rcScheme = DRS4SettingsManager::sharedInstance()->pulseShapeFilterRecordScheme();

        const DRS4InterpolationType::type interpolationType = DRS4SettingsManager::sharedInstance()->interpolationType();
        const DRS4SplineInterpolationType::type splineInterpolationType = DRS4SettingsManager::sharedInstance()->splineInterpolationType();
        const bool bUsingALGLIB = (splineInterpolationType < 6 && splineInterpolationType > 1);
        const bool bUsingTinoKluge = ((interpolationType == DRS4InterpolationType::type::spline) && (splineInterpolationType == 6));
        const bool bUsingLinearInterpol = ((interpolationType == DRS4InterpolationType::type::spline) && (splineInterpolationType == 1));
        const int intraRenderPoints = (DRS4InterpolationType::type::spline == interpolationType)?(DRS4SettingsManager::sharedInstance()->splineIntraSamplingCounts()):(DRS4SettingsManager::sharedInstance()->polynomialSamplingCounts());
        const int streamIntraRenderPoints =  DRS4ProgramSettingsManager::sharedInstance()->splineIntraPoints();

        const bool bMedianFilterA = DRS4SettingsManager::sharedInstance()->medianFilterAEnabled();
        const bool bMedianFilterB = DRS4SettingsManager::sharedInstance()->medianFilterBEnabled();
        const int medianFilterWindowSizeA = DRS4SettingsManager::sharedInstance()->medianFilterWindowSizeA();
        const int medianFilterWindowSizeB = DRS4SettingsManager::sharedInstance()->medianFilterWindowSizeB();

        /* Baseline - Jitter Corrections */
        bool bUseBaseLineCorrectionA = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationEnabledA();
        int bl_startCellA = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationStartCellA();
        int bl_cellRegionA = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationRegionA();
        double bl_valueA = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationShiftValueInMVA();
        bool bUseBaseLineCorrectionRejectionA = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitRejectLimitA();
        double bl_rejectionLimitA = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitInPercentageA();

        bool bUseBaseLineCorrectionB = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationEnabledB();
        int bl_startCellB = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationStartCellB();
        int bl_cellRegionB = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationRegionB();
        double bl_valueB = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationShiftValueInMVB();
        bool bUseBaseLineCorrectionRejectionB = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitRejectLimitB();
        double bl_rejectionLimitB = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitInPercentageB();

        /* Shape Filter */
        const bool bPulseShapeFilterIsEnabledA = DRS4SettingsManager::sharedInstance()->pulseShapeFilterEnabledA();
        const bool bPulseShapeFilterIsEnabledB = DRS4SettingsManager::sharedInstance()->pulseShapeFilterEnabledB();

        const bool bIntrinsicFilterA = (bMedianFilterA || bUseBaseLineCorrectionA);
        const bool bIntrinsicFilterB = (bMedianFilterB || bUseBaseLineCorrectionB);

        if (bIntrinsicFilterA) {
            copy(waveChannel0, waveChannel0 + kNumberOfBins, waveChannel0S);
        }

        if (bIntrinsicFilterB) {
            copy(waveChannel1, waveChannel1 + kNumberOfBins, waveChannel1S);
        }

        //apply median filter to remove spikes:
        if (bMedianFilterA) {
            if (!DMedianFilter::apply(waveChannel0, kNumberOfBins, medianFilterWindowSizeA))
                continue;
        }

        if (bMedianFilterB) {
            if (!DMedianFilter::apply(waveChannel1, kNumberOfBins, medianFilterWindowSizeB))
                continue;
        }

        /* baseline - jitter corrections */
        if (bUseBaseLineCorrectionA) {
            const int endRegionA = (bl_startCellA + bl_cellRegionA - 1);

            double blA = 0.0f;

            for (int i = bl_startCellA ; i  < endRegionA ; ++ i )
                blA += waveChannel0[i];

            blA /= bl_cellRegionA;

            const bool limitExceededA = (abs(blA - bl_valueA)/500.0) > bl_rejectionLimitA*0.01;

            if (bUseBaseLineCorrectionRejectionA && limitExceededA)
                continue;

            for (int i = 0 ; i < kNumberOfBins ; ++ i)
                waveChannel0[i] -= blA;
        }

        if (bUseBaseLineCorrectionB) {
            const int endRegionB = (bl_startCellB + bl_cellRegionB - 1);

            double blB = 0.0f;

            for (int i = bl_startCellB ; i  < endRegionB ; ++ i )
                blB += waveChannel1[i];

            blB /= bl_cellRegionB;

            const bool limitExceededB = (abs(blB - bl_valueB)/500.0) > bl_rejectionLimitB*0.01;

            if (bUseBaseLineCorrectionRejectionB && limitExceededB)
                continue;

            for (int i = 0 ; i < kNumberOfBins ; ++ i)
                waveChannel1[i] -= blB;
        }

        /* clear pulse-data for new visualization */
        if (!bBurstMode) {
            resetPulseA();
            resetPulseB();

            m_pListChannelA.resize(cellWidth);
            m_pListChannelB.resize(cellWidth);
        }

        if (bUsingALGLIB) {
            /* ALGLIB array allocation */
            m_arrayDataX_A.setlength(cellWidth);
            m_arrayDataY_A.setlength(cellWidth);
            m_arrayDataX_B.setlength(cellWidth);
            m_arrayDataY_B.setlength(cellWidth);
        }
        else if (bUsingTinoKluge) {
            m_arrayDataTKX_A.resize(cellWidth);
            m_arrayDataTKX_B.resize(cellWidth);
            m_arrayDataTKY_A.resize(cellWidth);
            m_arrayDataTKY_B.resize(cellWidth);
        }
        else if (bUsingLinearInterpol) {
            /* nothing to do yet */
        }

        /* inline time and voltage extrema value estimation */
        float xMinA = sweep;
        float xMaxA = 0.0f;

        float xMinB = sweep;
        float xMaxB = 0.0f;

        float yMinA = 500.0f;
        float yMaxA = -500.0f;

        float yMinB = 500.0f;
        float yMaxB = -500.0f;

        /* inline voltage extrema value estimation */
        int cellYAMin = -1;
        int cellYAMax = -1;

        int cellYBMin = -1;
        int cellYBMax = -1;

        /* inline CF value estimation */
        float cfdValueA = 0.0f;
        float cfdValueB = 0.0f;

        float cfdValueA_10perc = 0.0f;
        float cfdValueB_10perc = 0.0f;

        float cfdValueA_90perc = 0.0f;
        float cfdValueB_90perc = 0.0f;

        /* inline CF cell estimation */
        int estimCFDACellStart = -1;
        int estimCFDBCellStart = -1;

        int estimCFDACellStop = -1;
        int estimCFDBCellStop = -1;

        /* inline CF (10%, 90%) cell estimation >> rise-time estimation (delta illumination signal) */
        int estimCFDACellStart_10perc = -1;
        int estimCFDBCellStart_10perc = -1;

        int estimCFDACellStart_90perc = -1;
        int estimCFDBCellStart_90perc = -1;

        int estimCFDACellStop_10perc = -1;
        int estimCFDBCellStop_10perc = -1;

        int estimCFDACellStop_90perc = -1;
        int estimCFDBCellStop_90perc = -1;

        int cfdACounter = 0;
        int cfdBCounter = 0;

        float areaA = 0.0f;
        float areaB = 0.0f;

        /* reduce mathematical operations within in the loop */
        const int reducedEndRange = (endRange - 1);
        const int reducedCellWidth = (cellWidth - 1);
        const int extendedStartCell = (startCell + 1);


        bool bForceReject = false;

        /* determine min/max and proceed a CF estimation in ROI */
        for ( int a = reducedEndRange, it = reducedCellWidth ; a >= startCell ; -- a, -- it ) {
            /* time domain bracket values */
            xMinA = qMin(xMinA, tChannel0[a]);
            xMaxA = qMax(xMaxA, tChannel0[a]);

            xMinB = qMin(xMinB, tChannel1[a]);
            xMaxB = qMax(xMaxB, tChannel1[a]);

            if ( waveChannel0[a] >= yMaxA ) {
                yMaxA = waveChannel0[a];
                cellYAMax = a;

                if (positiveSignal) {
                    cfdValueA = cfdA*yMaxA;

                    /* t - rise according to the spec definition of a delta illumination signal */
                    cfdValueA_10perc = 0.1*yMaxA;
                    cfdValueA_90perc = 0.9*yMaxA;

                    cfdACounter = 0;
                }              
            }

            if ( waveChannel1[a] >= yMaxB ) {
                yMaxB = waveChannel1[a];
                cellYBMax = a;

                if (positiveSignal) {
                    cfdValueB = cfdB*yMaxB;

                    /* t - rise according to the spec definition of a delta illumination signal */
                    cfdValueB_10perc = 0.1*yMaxB;
                    cfdValueB_90perc = 0.9*yMaxB;

                    cfdBCounter = 0;
                }                
            }

            if ( waveChannel0[a] <= yMinA ) {
                yMinA = waveChannel0[a];
                cellYAMin = a;

                if (!positiveSignal) {
                    cfdValueA = cfdA*yMinA;

                    /* t - rise according to the spec definition of a delta illumination signal */
                    cfdValueA_10perc = 0.1*yMinA;
                    cfdValueA_90perc = 0.9*yMinA;

                    cfdACounter = 0;
                }
            }

            if ( waveChannel1[a] <= yMinB ) {
                yMinB = waveChannel1[a];
                cellYBMin = a;

                if (!positiveSignal) {
                    cfdValueB = cfdB*yMinB;

                    /* t - rise according to the spec definition of a delta illumination signal */
                    cfdValueB_10perc = 0.1*yMinB;
                    cfdValueB_90perc = 0.9*yMinB;

                    cfdBCounter = 0;
                }
            }

            /* CFD cell estimation/determination */
            if ( a >= extendedStartCell ) {
                const int aDecr = (a - 1);

                /* calculate the pulse area of ROI */
                if (bPulseAreaPlot
                        || bPulseAreaFilter) {
                    areaA += abs((waveChannel0[aDecr] + 0.5*(waveChannel0[a] - waveChannel0[aDecr]))*(tChannel0[a] - tChannel0[aDecr]));
                    areaB += abs((waveChannel1[aDecr] + 0.5*(waveChannel1[a] - waveChannel1[aDecr]))*(tChannel1[a] - tChannel1[aDecr]));
                }

                const double slopeA = (waveChannel0[a] - waveChannel0[aDecr])/(tChannel0[a] - tChannel0[aDecr]);
                const double slopeB = (waveChannel1[a] - waveChannel1[aDecr])/(tChannel1[a] - tChannel1[aDecr]);

                if (!qIsFinite(slopeA) || !qIsFinite(slopeB)) {
                    bForceReject = true;
                }

                /* CFD on rising edge = positive slope? or falling edge = negative slope?*/
                const bool bInRangeA = positiveSignal?(slopeA > 1E-6):(slopeA < 1E-6);
                const bool bInRangeB = positiveSignal?(slopeB > 1E-6):(slopeB < 1E-6);

                if ( bInRangeA ) {
                    const bool cfdLevelInRangeA = positiveSignal?(waveChannel0[a] > cfdValueA && waveChannel0[aDecr] < cfdValueA):(waveChannel0[a] < cfdValueA && waveChannel0[aDecr] > cfdValueA);

                    /* t - rise according to the spec definition of a delta illumination signal */
                    const bool cfdLevelInRangeA_10perc = positiveSignal?(waveChannel0[a] > cfdValueA_10perc && waveChannel0[aDecr] < cfdValueA_10perc):(waveChannel0[a] < cfdValueA_10perc && waveChannel0[aDecr] > cfdValueA_10perc);
                    const bool cfdLevelInRangeA_90perc = positiveSignal?(waveChannel0[a] > cfdValueA_90perc && waveChannel0[aDecr] < cfdValueA_90perc):(waveChannel0[a] < cfdValueA_90perc && waveChannel0[aDecr] > cfdValueA_90perc);

                    if ( cfdLevelInRangeA ) {
                        estimCFDACellStart = aDecr;
                        estimCFDACellStop = a;

                        cfdACounter ++;
                    }
                    else if ( qFuzzyCompare(waveChannel0[a], cfdValueA) ) {
                        estimCFDACellStart = a;
                        estimCFDACellStop = a;

                        cfdACounter ++;
                    }
                    else if ( qFuzzyCompare(waveChannel0[aDecr], cfdValueA) ) {
                        estimCFDACellStart = aDecr;
                        estimCFDACellStop = aDecr;

                        cfdACounter ++;
                    }

                    /* 10% CF Level */
                    if ( cfdLevelInRangeA_10perc ) {
                        estimCFDACellStart_10perc = aDecr;
                        estimCFDACellStop_10perc = a;
                    }
                    else if ( qFuzzyCompare(waveChannel0[a], cfdValueA_10perc) ) {
                        estimCFDACellStart_10perc = a;
                        estimCFDACellStop_10perc = a;
                    }
                    else if ( qFuzzyCompare(waveChannel0[aDecr], cfdValueA_10perc) ) {
                        estimCFDACellStart_10perc = aDecr;
                        estimCFDACellStop_10perc = aDecr;
                    }

                    /* 90% CF Level */
                    if ( cfdLevelInRangeA_90perc ) {
                        estimCFDACellStart_90perc = aDecr;
                        estimCFDACellStop_90perc = a;
                    }
                    else if ( qFuzzyCompare(waveChannel0[a], cfdValueA_90perc) ) {
                        estimCFDACellStart_90perc = a;
                        estimCFDACellStop_90perc = a;
                    }
                    else if ( qFuzzyCompare(waveChannel0[aDecr], cfdValueA_90perc) ) {
                        estimCFDACellStart_90perc = aDecr;
                        estimCFDACellStop_90perc = aDecr;
                    }
                }

                if ( bInRangeB ) {
                    const bool cfdLevelInRangeB = positiveSignal?(waveChannel1[a] > cfdValueB && waveChannel1[aDecr] < cfdValueB):(waveChannel1[a] < cfdValueB && waveChannel1[aDecr] > cfdValueB);

                    /* t - rise according to the spec definition of a delta illumination signal */
                    const bool cfdLevelInRangeB_10perc = positiveSignal?(waveChannel1[a] > cfdValueB_10perc && waveChannel1[aDecr] < cfdValueB_10perc):(waveChannel1[a] < cfdValueB_10perc && waveChannel1[aDecr] > cfdValueB_10perc);
                    const bool cfdLevelInRangeB_90perc = positiveSignal?(waveChannel1[a] > cfdValueB_90perc && waveChannel1[aDecr] < cfdValueB_90perc):(waveChannel1[a] < cfdValueB_90perc && waveChannel1[aDecr] > cfdValueB_90perc);

                    if ( cfdLevelInRangeB  ) {
                        estimCFDBCellStart = aDecr;
                        estimCFDBCellStop = a;

                        cfdBCounter ++;
                    }
                    else if ( qFuzzyCompare(waveChannel1[a], cfdValueB) ) {
                        estimCFDBCellStart = a;
                        estimCFDBCellStop = a;

                        cfdBCounter ++;
                    }
                    else if ( qFuzzyCompare(waveChannel1[aDecr], cfdValueB) ) {
                        estimCFDBCellStart = aDecr;
                        estimCFDBCellStop = aDecr;

                        cfdBCounter ++;
                    }

                    /* 10% CF Level */
                    if ( cfdLevelInRangeB_10perc ) {
                        estimCFDBCellStart_10perc = aDecr;
                        estimCFDBCellStop_10perc = a;
                    }
                    else if ( qFuzzyCompare(waveChannel1[a], cfdValueB_10perc) ) {
                        estimCFDBCellStart_10perc = a;
                        estimCFDBCellStop_10perc = a;
                    }
                    else if ( qFuzzyCompare(waveChannel1[aDecr], cfdValueB_10perc) ) {
                        estimCFDBCellStart_10perc = aDecr;
                        estimCFDBCellStop_10perc = aDecr;
                    }

                    /* 90% CF Level */
                    if ( cfdLevelInRangeB_90perc ) {
                        estimCFDBCellStart_90perc = aDecr;
                        estimCFDBCellStop_90perc = a;
                    }
                    else if ( qFuzzyCompare(waveChannel1[a], cfdValueB_90perc) ) {
                        estimCFDBCellStart_90perc = a;
                        estimCFDBCellStop_90perc = a;
                    }
                    else if ( qFuzzyCompare(waveChannel1[aDecr], cfdValueB_90perc) ) {
                        estimCFDBCellStart_90perc = aDecr;
                        estimCFDBCellStop_90perc = aDecr;
                    }
                }
            }

            if (bForceReject)
                continue;

            /* insert pulse-point for visualization*/
            if (!bBurstMode) {
                m_pListChannelA[it] = QPointF(tChannel0[a], waveChannel0[a]);
                m_pListChannelB[it] = QPointF(tChannel1[a], waveChannel1[a]);
            }

            if (bUsingALGLIB) {
                /* ALGLIB array */
                m_arrayDataX_A.ptr->ptr.p_double[it] = tChannel0[a];
                m_arrayDataX_B.ptr->ptr.p_double[it] = tChannel1[a];

                m_arrayDataY_A.ptr->ptr.p_double[it] = waveChannel0[a];
                m_arrayDataY_B.ptr->ptr.p_double[it] = waveChannel1[a];
            }
            else if (bUsingTinoKluge) {
                m_arrayDataTKX_A[it] = tChannel0[a];
                m_arrayDataTKX_B[it] = tChannel1[a];

                m_arrayDataTKY_A[it] = waveChannel0[a];
                m_arrayDataTKY_B[it] = waveChannel1[a];
            } else if (bUsingLinearInterpol) {
                /* nothing yet */
            }
        }

        /* min/max was not able to be determined */
        if (cellYAMax == -1
                || cellYAMin == -1
                || cellYBMax == -1
                || cellYBMin == -1
                || qFuzzyCompare(yMinA, yMaxA)
                || qFuzzyCompare(yMinB, yMaxB)
                || (((int)yMinA == (int)yMaxA) || ((int)yMinB == (int)yMaxB)))
            continue;

        /* prevent mutex locking: call these functions only once within the loop */
        const bool bPersistance = DRS4SettingsManager::sharedInstance()->isPersistanceEnabled();
        const int pulseAreaFilterBinningA = DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA();
        const int pulseAreaFilterBinningB = DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB();
        const double pulseAreaFilterNormA = DRS4SettingsManager::sharedInstance()->pulseAreaFilterNormalizationA();
        const double pulseAreaFilterNormB = DRS4SettingsManager::sharedInstance()->pulseAreaFilterNormalizationB();
        const double riseTimeFilterAScale = DRS4SettingsManager::sharedInstance()->riseTimeFilterScaleInNanosecondsOfA();
        const double riseTimeFilterBScale = DRS4SettingsManager::sharedInstance()->riseTimeFilterScaleInNanosecondsOfB();
        const int riseTimeFilterABinning = DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfA();
        const int riseTimeFilterBBinning = DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfB();
        const int riseTimeFilterWindowLeftA = DRS4SettingsManager::sharedInstance()->riseTimeFilterLeftWindowOfA();
        const int riseTimeFilterWindowLeftB = DRS4SettingsManager::sharedInstance()->riseTimeFilterLeftWindowOfB();
        const int riseTimeFilterWindowRightA = DRS4SettingsManager::sharedInstance()->riseTimeFilterRightWindowOfA();
        const int riseTimeFilterWindowRightB = DRS4SettingsManager::sharedInstance()->riseTimeFilterRightWindowOfB();
        const int channelCntAB = DRS4SettingsManager::sharedInstance()->channelCntAB();
        const int channelCntBA = DRS4SettingsManager::sharedInstance()->channelCntBA();
        const int channelCntPrompt = DRS4SettingsManager::sharedInstance()->channelCntCoincindence();
        const int channelCntMerged = DRS4SettingsManager::sharedInstance()->channelCntMerged();
        const bool bNegativeLT = DRS4SettingsManager::sharedInstance()->isNegativeLTAccepted();
        const bool bForcePrompt = DRS4SettingsManager::sharedInstance()->isforceCoincidence();
        const double offsetAB = DRS4SettingsManager::sharedInstance()->offsetInNSAB();
        const double offsetBA = DRS4SettingsManager::sharedInstance()->offsetInNSBA();
        const double offsetPrompt = DRS4SettingsManager::sharedInstance()->offsetInNSCoincidence();
        const double offsetMerged = DRS4SettingsManager::sharedInstance()->offsetInNSMerged();
        const double scalerAB = DRS4SettingsManager::sharedInstance()->scalerInNSAB();
        const double scalerBA = DRS4SettingsManager::sharedInstance()->scalerInNSBA();
        const double scalerPrompt = DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence();
        const double scalerMerged = DRS4SettingsManager::sharedInstance()->scalerInNSMerged();
        const double ATS = DRS4SettingsManager::sharedInstance()->meanCableDelay();
        const bool bStreamInRangeArmed = DRS4TextFileStreamRangeManager::sharedInstance()->isArmed();
        const bool bStreamWithoutRangeArmed = DRS4TextFileStreamManager::sharedInstance()->isArmed();
        const bool bOppositePersistanceA = DRS4SettingsManager::sharedInstance()->persistanceUsingCFDBAsRefForA();
        const bool bOppositePersistanceB = DRS4SettingsManager::sharedInstance()->persistanceUsingCFDAAsRefForB();

        if ( DRS4StreamManager::sharedInstance()->isArmed() ) {
            if (!DRS4StreamManager::sharedInstance()->write((const char*)tChannel0, sizeOfWave)) {
                /* nothing yet */
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)(bMedianFilterA?waveChannel0S:waveChannel0), sizeOfWave)) {
                /* nothing yet */
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)tChannel1, sizeOfWave)) {
                /* nothing yet */
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)(bMedianFilterA?waveChannel1S:waveChannel1), sizeOfWave)) {
                /* nothing yet */
            }
        }

        /* light-weight filtering of wrong events: */
        if ( positiveSignal ) {
            if ( (abs(yMinA) > abs(yMaxA))
                 || (abs(yMinB) > abs(yMaxB)) )
                continue;

            if ( abs(cellYAMax - startCell) <= 15
                 || abs(cellYBMax - startCell) <= 15 )
                continue;

            if ( qFuzzyCompare(waveChannel0[reducedEndRange], yMaxA)
                 || qFuzzyCompare(waveChannel1[reducedEndRange], yMaxB)
                 || qFuzzyCompare(waveChannel0[startCell], yMaxA)
                 || qFuzzyCompare(waveChannel1[startCell], yMaxB) )
                continue;
        }
        else {
            if ( (abs(yMinA) < abs(yMaxA))
                 || (abs(yMinB) < abs(yMaxB)) )
                continue;

            if ( abs(cellYAMin - startCell) <= 15
                 || abs(cellYBMin - startCell) <= 15 )
                continue;

            if ( qFuzzyCompare(waveChannel0[reducedEndRange], yMinA)
                 || qFuzzyCompare(waveChannel1[reducedEndRange], yMinB)
                 || qFuzzyCompare(waveChannel0[startCell], yMinA)
                 || qFuzzyCompare(waveChannel1[startCell], yMinB) )
                continue;
        }

        /* reject artifacts */
        if ( cfdBCounter > 1 || cfdBCounter == 0
             || cfdACounter > 1 || cfdACounter == 0 )
            continue;

        /* select interpolation type - obtain interpolant */
        if (interpolationType == DRS4InterpolationType::type::spline) {
            if (bUsingALGLIB) {
                switch (splineInterpolationType) {
                case DRS4SplineInterpolationType::type::cubic: {
                    alglib::spline1dbuildcubic(m_arrayDataX_A, m_arrayDataY_A, m_interpolantA);
                    alglib::spline1dbuildcubic(m_arrayDataX_B, m_arrayDataY_B, m_interpolantB);
                }
                    break;
                case DRS4SplineInterpolationType::type::akima: {
                    alglib::spline1dbuildakima(m_arrayDataX_A, m_arrayDataY_A, m_interpolantA);
                    alglib::spline1dbuildakima(m_arrayDataX_B, m_arrayDataY_B, m_interpolantB);
                }
                    break;
                case DRS4SplineInterpolationType::type::catmullRom: {
                    alglib::spline1dbuildcatmullrom(m_arrayDataX_A, m_arrayDataY_A, m_interpolantA);
                    alglib::spline1dbuildcatmullrom(m_arrayDataX_B, m_arrayDataY_B, m_interpolantB);
                }
                    break;
                case DRS4SplineInterpolationType::type::monotone: {
                    alglib::spline1dbuildmonotone(m_arrayDataX_A, m_arrayDataY_A, m_interpolantA);
                    alglib::spline1dbuildmonotone(m_arrayDataX_B, m_arrayDataY_B, m_interpolantB);
                }
                    break;
                default: {
                    alglib::spline1dbuildcubic(m_arrayDataX_A, m_arrayDataY_A, m_interpolantA);
                    alglib::spline1dbuildcubic(m_arrayDataX_B, m_arrayDataY_B, m_interpolantB);
                }
                    break;
                }
            }
            else if (bUsingTinoKluge) {
                tkSplineA.setType(SplineType::Cubic);
                tkSplineB.setType(SplineType::Cubic);

                tkSplineA.setPoints(m_arrayDataTKX_A, m_arrayDataTKY_A);
                tkSplineB.setPoints(m_arrayDataTKX_B, m_arrayDataTKY_B);
            }
            else if (bUsingLinearInterpol) {
                /* nothing yet */
            }
        }
        else {
            alglib::polynomialbuild(m_arrayDataX_A, m_arrayDataY_A, m_baryCentricInterpolantA);
            alglib::polynomialbuild(m_arrayDataX_B, m_arrayDataY_B, m_baryCentricInterpolantB);
        }

        /* calculate and normalize pulse area for subsequent area filtering */
        if (bPulseAreaPlot
                || bPulseAreaFilter) {
            const float rat = 5120*((float)cellWidth/((float)kNumberOfBins));

            areaA = areaA/(pulseAreaFilterNormA*rat);
            areaB = areaB/(pulseAreaFilterNormB*rat);
        }

        double timeAForYMax = -1;
        double timeBForYMax = -1;

        if (positiveSignal) {
            timeAForYMax = tChannel0[cellYAMax];
            timeBForYMax = tChannel1[cellYBMax];
        }
        else {
            timeAForYMax = tChannel0[cellYAMin];
            timeBForYMax = tChannel1[cellYBMin];
        }

        if (qFuzzyCompare(timeAForYMax, -1)
                || qFuzzyCompare(timeBForYMax, -1))
            continue;

        /* store/write pulses and interpolations to ASCII file */
        if (!bBurstMode) {
            if (bStreamWithoutRangeArmed) {
                DRS4TextFileStreamManager::sharedInstance()->writePulses(tChannel0, tChannel1, waveChannel0, waveChannel1, kNumberOfBins);

                if ( (DRS4TextFileStreamManager::sharedInstance()->writeInterpolationA() || DRS4TextFileStreamManager::sharedInstance()->writeInterpolationB())
                     && (m_pListChannelA.size() > 0 && m_pListChannelB.size() > 0) ) {
                    if (!bUsingLinearInterpol) {
                        const int points = m_pListChannelA.size()*streamIntraRenderPoints;

                        QVector<QPointF> splineA(points);
                        QVector<QPointF> splineB(points);

                        const float incrA = (m_pListChannelA.at(m_pListChannelA.size()-1).x() - m_pListChannelA.at(0).x())/(float)points;
                        const float incrB = (m_pListChannelB.at(m_pListChannelB.size()-1).x() - m_pListChannelB.at(0).x())/(float)points;
                        const float startXA = m_pListChannelA.at(0).x();
                        const float startXB = m_pListChannelB.at(0).x();

                        for ( int i = 0 ; i < points ; ++ i ) {
                            const float valXA = startXA + (float)i*incrA;
                            const float valXB = startXB + (float)i*incrB;

                            const float valYA = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingTinoKluge?(tkSplineA(valXA)):(alglib::spline1dcalc(m_interpolantA, valXA))):(alglib::barycentriccalc(m_baryCentricInterpolantA, valXA));
                            const float valYB = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingTinoKluge?(tkSplineB(valXB)):(alglib::spline1dcalc(m_interpolantB, valXB))):(alglib::barycentriccalc(m_baryCentricInterpolantB, valXB));

                            splineA[i] = QPointF(valXA, valYA);
                            splineB[i] = QPointF(valXB, valYB);
                        }

                        DRS4TextFileStreamManager::sharedInstance()->writeInterpolations(&splineA, &splineB, interpolationType, splineInterpolationType);
                    }
                    else {
                        DRS4TextFileStreamManager::sharedInstance()->writeInterpolations(&m_pListChannelA, &m_pListChannelB, interpolationType, splineInterpolationType);
                    }
                }
            }
        }

        /* determine max/min more precisely */
        if (!bUsingLinearInterpol) {
            const int cell_interpolRangeA_start = positiveSignal?(cellYAMax-1):(cellYAMin-1);
            const int cell_interpolRangeA_stop = positiveSignal?(cellYAMax+1):(cellYAMin+1);

            const int cell_interpolRangeB_start = positiveSignal?(cellYBMax-1):(cellYBMin-1);
            const int cell_interpolRangeB_stop = positiveSignal?(cellYBMax+1):(cellYBMin+1);

            if ( !(cell_interpolRangeA_start >= 0 && cell_interpolRangeA_start < kNumberOfBins)
                 || !(cell_interpolRangeA_stop >= 0 && cell_interpolRangeA_stop < kNumberOfBins)
                 || !(cell_interpolRangeB_start >= 0 && cell_interpolRangeB_start < kNumberOfBins)
                 || !(cell_interpolRangeB_stop >= 0 && cell_interpolRangeB_stop < kNumberOfBins) )
                continue;

            const double renderIncrementA = (tChannel0[cell_interpolRangeA_stop] - tChannel0[cell_interpolRangeA_start])/((float)intraRenderPoints);
            const double renderIncrementB = (tChannel1[cell_interpolRangeB_stop] - tChannel1[cell_interpolRangeB_start])/((float)intraRenderPoints);

            for ( int i = 0 ; i <= intraRenderPoints ; ++ i ) {
                const double t_A = tChannel0[cell_interpolRangeA_start] + (float)i*renderIncrementA;
                const double t_B = tChannel1[cell_interpolRangeB_start] + (float)i*renderIncrementB;

                const float valYA = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantA, t_A)):(tkSplineA(t_A))):(alglib::barycentriccalc(m_baryCentricInterpolantA, t_A));
                const float valYB = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantB, t_B)):(tkSplineB(t_B))):(alglib::barycentriccalc(m_baryCentricInterpolantB, t_B));

                /* modify min/max to calculate the CF level in high accuracy, subsequently */
                if (valYA > yMaxA) {
                    yMaxA = valYA;

                    if (positiveSignal) {
                        timeAForYMax = t_A;
                    }
                }

                if (valYB > yMaxB) {
                    yMaxB = valYB;

                    if (positiveSignal) {
                        timeBForYMax = t_B;
                    }
                }

                if (valYA < yMinA) {
                    yMinA = valYA;

                    if (!positiveSignal) {
                        timeAForYMax = t_A;
                    }
                }

                if (valYB < yMinB) {
                    yMinB = valYB;

                    if (!positiveSignal) {
                        timeBForYMax = t_B;
                    }
                }
            }
        }

        /* add modified (interpolated/fitted) pulse height to PHS */
        float fractPHSA = 0.0f;
        float fractPHSB = 0.0f;

        const double fkNumberOfBins = (double)kNumberOfBins;

        if (positiveSignal) {
            fractPHSA = yMaxA*0.002;
            fractPHSB = yMaxB*0.002;
        }
        else {
            fractPHSA = abs(yMinA)*0.002;
            fractPHSB = abs(yMinB)*0.002;
        }

        /* add pulse heights to PHS */
        const int cellPHSA = ((int)(fractPHSA*fkNumberOfBins))-1;
        const int cellPHSB = ((int)(fractPHSB*fkNumberOfBins))-1;

        if ( cellPHSA < kNumberOfBins && cellPHSA >= 0 ) {
            m_phsA[cellPHSA] ++;
            m_phsACounts ++;
        }

        if ( cellPHSB < kNumberOfBins && cellPHSB >= 0 ) {
            m_phsB[cellPHSB] ++;
            m_phsBCounts ++;
        }

        /* CF levels valid? */
        if (estimCFDACellStart == -1
            || estimCFDACellStop == -1
            || estimCFDBCellStart == -1
            || estimCFDBCellStop == -1)
            continue;

        if (positiveSignal) {
            if ( cfdValueA > 500.0f
                 || cfdValueB > 500.0f
                 || qFuzzyCompare(cfdValueA, 0.0f)
                 || qFuzzyCompare(cfdValueB, 0.0f)
                 || cfdValueA < 0.0f
                 || cfdValueB < 0.0f
                 || ((int)cfdValueA == (int)yMaxA)
                 || ((int)cfdValueB == (int)yMaxB))
                continue;
        }
        else {
            if ( cfdValueA < -500.0f
                 || cfdValueB < -500.0f
                 || qFuzzyCompare(cfdValueA, 0.0f)
                 || qFuzzyCompare(cfdValueB, 0.0f)
                 || cfdValueA > 0.0f
                 || cfdValueB > 0.0f
                 || ((int)cfdValueA == (int)yMinA)
                 || ((int)cfdValueB == (int)yMinB) )
                continue;
        }

        double timeStampA = -1.0f;
        double timeStampB = -1.0f;

        if (!bUsingLinearInterpol) {
            /* find correct CF level timestamp within the estimated CF bracketed index region */
            if ( estimCFDACellStart == estimCFDACellStop ) {
                timeStampA = tChannel0[estimCFDACellStart];
            }
            else {
                const double timeIncr = (tChannel0[estimCFDACellStop] - tChannel0[estimCFDACellStart])/((double)intraRenderPoints);
                for ( int i = 0 ; i < intraRenderPoints ; ++ i ) {
                    const double timeStamp1 = tChannel0[estimCFDACellStart] + (double)i*timeIncr;
                    const double timeStamp2 = tChannel0[estimCFDACellStart] + (double)(i+1)*timeIncr;

                    const float valY1 = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantA, timeStamp1)):(tkSplineA(timeStamp1))):(alglib::barycentriccalc(m_baryCentricInterpolantA, timeStamp1));
                    const float valY2 = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantA, timeStamp2)):(tkSplineA(timeStamp2))):(alglib::barycentriccalc(m_baryCentricInterpolantA, timeStamp2));

                    if ( (cfdValueA < valY1 && cfdValueA > valY2)
                         || (cfdValueA > valY1 && cfdValueA < valY2) ) {
                        const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                        const double intersect = valY1 - slope*timeStamp1;

                        timeStampA = (cfdValueA - intersect)/slope;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueA, valY1) ) {
                        timeStampA = timeStamp1;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueA, valY2) ) {
                        timeStampA = timeStamp2;

                        break;
                    }
                }
            }

            if ( estimCFDBCellStart == estimCFDBCellStop ) {
                timeStampB = tChannel1[estimCFDBCellStart];
            }
            else {
                const double timeIncr = (tChannel1[estimCFDBCellStop] - tChannel1[estimCFDBCellStart])/((double)intraRenderPoints);
                for ( int i = 0 ; i < intraRenderPoints ; ++ i ) {
                    const double timeStamp1 = tChannel1[estimCFDBCellStart] + (double)i*timeIncr;
                    const double timeStamp2 = tChannel1[estimCFDBCellStart] + (double)(i+1)*timeIncr;

                    const float valY1 = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantB, timeStamp1)):(tkSplineB(timeStamp1))):(alglib::barycentriccalc(m_baryCentricInterpolantB, timeStamp1));
                    const float valY2 = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantB, timeStamp2)):(tkSplineB(timeStamp2))):(alglib::barycentriccalc(m_baryCentricInterpolantB, timeStamp2));

                    if ( (cfdValueB < valY1 && cfdValueB > valY2)
                         || (cfdValueB > valY1 && cfdValueB < valY2) ) {
                        const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                        const double intersect = valY1 - slope*timeStamp1;

                        timeStampB = (cfdValueB - intersect)/slope;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueB, valY1) ) {
                        timeStampB = timeStamp1;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueB, valY2) ) {
                        timeStampB = timeStamp2;

                        break;
                    }
                }
            }
        }
        /* linear interpolation */
        else {
            if ( estimCFDACellStart == estimCFDACellStop ) {
                timeStampA = tChannel0[estimCFDACellStart];
            }
            else {
                const float timeStamp1 = tChannel0[estimCFDACellStart];
                const float timeStamp2 = tChannel0[estimCFDACellStop];

                const float valY1 = waveChannel0[estimCFDACellStart];
                const float valY2 = waveChannel0[estimCFDACellStop];

                if ( (cfdValueA < valY1 && cfdValueA > valY2)
                     || (cfdValueA > valY1 && cfdValueA < valY2) ) {
                    const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                    const double intersect = valY1 - slope*timeStamp1;

                    timeStampA = (cfdValueA - intersect)/slope;
                }
                else if ( qFuzzyCompare(cfdValueA, valY1) ) {
                    timeStampA = timeStamp1;
                }
                else if ( qFuzzyCompare(cfdValueA, valY2) ) {
                    timeStampA = timeStamp2;
                }
            }

            if ( estimCFDBCellStart == estimCFDBCellStop ) {
                timeStampB = tChannel1[estimCFDBCellStart];
            }
            else {
                const float timeStamp1 = tChannel1[estimCFDBCellStart];
                const float timeStamp2 = tChannel1[estimCFDBCellStop];

                const float valY1 = waveChannel1[estimCFDBCellStart];
                const float valY2 = waveChannel1[estimCFDBCellStop];

                if ( (cfdValueB < valY1 && cfdValueB > valY2)
                     || (cfdValueB > valY1 && cfdValueB < valY2) ) {
                    const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                    const double intersect = valY1 - slope*timeStamp1;

                    timeStampB = (cfdValueB - intersect)/slope;
                }
                else if ( qFuzzyCompare(cfdValueB, valY1) ) {
                    timeStampB = timeStamp1;
                }
                else if ( qFuzzyCompare(cfdValueB, valY2) ) {
                    timeStampB = timeStamp2;
                }
            }
        }

        double timeStampA_10perc = -1.0f;
        double timeStampB_10perc = -1.0f;

        if (!bUsingLinearInterpol) {
            /* finding correct CF (10%) timestamp within the estimated CF (10%) bracketed index region */
            if ( estimCFDACellStart_10perc == estimCFDACellStop_10perc ) {
                timeStampA_10perc = tChannel0[estimCFDACellStart_10perc];
            }
            else {
                const double timeIncr = (tChannel0[estimCFDACellStop_10perc] - tChannel0[estimCFDACellStart_10perc])/((double)intraRenderPoints);
                for ( int i = 0 ; i < intraRenderPoints ; ++ i ) {
                    const double timeStamp1 = tChannel0[estimCFDACellStart_10perc] + (double)i*timeIncr;
                    const double timeStamp2 = tChannel0[estimCFDACellStart_10perc] + (double)(i+1)*timeIncr;

                    const float valY1 = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantA, timeStamp1)):(tkSplineA(timeStamp1))):(alglib::barycentriccalc(m_baryCentricInterpolantA, timeStamp1));
                    const float valY2 = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantA, timeStamp2)):(tkSplineA(timeStamp2))):(alglib::barycentriccalc(m_baryCentricInterpolantA, timeStamp2));

                    if ( (cfdValueA_10perc < valY1 && cfdValueA_10perc > valY2)
                         || (cfdValueA_10perc > valY1 && cfdValueA_10perc < valY2) ) {
                        const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                        const double intersect = valY1 - slope*timeStamp1;

                        timeStampA_10perc = (cfdValueA_10perc - intersect)/slope;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueA_10perc, valY1) ) {
                        timeStampA_10perc = timeStamp1;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueA_10perc, valY2) ) {
                        timeStampA_10perc = timeStamp2;

                        break;
                    }
                }
            }

            if ( estimCFDBCellStart_10perc == estimCFDBCellStop_10perc ) {
                timeStampB_10perc = tChannel1[estimCFDBCellStart_10perc];
            }
            else {
                const double timeIncr = (tChannel1[estimCFDBCellStop_10perc] - tChannel1[estimCFDBCellStart_10perc])/((double)intraRenderPoints);
                for ( int i = 0 ; i < intraRenderPoints ; ++ i ) {
                    const double timeStamp1 = tChannel1[estimCFDBCellStart_10perc] + (double)i*timeIncr;
                    const double timeStamp2 = tChannel1[estimCFDBCellStart_10perc] + (double)(i+1)*timeIncr;

                    const float valY1 = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantB, timeStamp1)):(tkSplineB(timeStamp1))):(alglib::barycentriccalc(m_baryCentricInterpolantB, timeStamp1));
                    const float valY2 = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantB, timeStamp2)):(tkSplineB(timeStamp2))):(alglib::barycentriccalc(m_baryCentricInterpolantB, timeStamp2));

                    if ( (cfdValueB_10perc < valY1 && cfdValueB_10perc > valY2)
                         || (cfdValueB_10perc > valY1 && cfdValueB_10perc < valY2) ) {
                        const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                        const double intersect = valY1 - slope*timeStamp1;

                        timeStampB_10perc = (cfdValueB_10perc - intersect)/slope;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueB_10perc, valY1) ) {
                        timeStampB_10perc = timeStamp1;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueB_10perc, valY2) ) {
                        timeStampB_10perc = timeStamp2;

                        break;
                    }
                }
            }
        }
        /* linear interpolation */
        else {
            if ( estimCFDACellStart_10perc == estimCFDACellStop_10perc ) {
                timeStampA_10perc = tChannel0[estimCFDACellStart_10perc];
            }
            else {
                const float timeStamp1 = tChannel0[estimCFDACellStart_10perc];
                const float timeStamp2 = tChannel0[estimCFDACellStop_10perc];

                const float valY1 = waveChannel0[estimCFDACellStart_10perc];
                const float valY2 = waveChannel0[estimCFDACellStop_10perc];

                if ( (cfdValueA_10perc < valY1 && cfdValueA_10perc > valY2)
                     || (cfdValueA_10perc > valY1 && cfdValueA_10perc < valY2) ) {
                    const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                    const double intersect = valY1 - slope*timeStamp1;

                    timeStampA_10perc = (cfdValueA_10perc - intersect)/slope;
                }
                else if ( qFuzzyCompare(cfdValueA_10perc, valY1) ) {
                    timeStampA_10perc = timeStamp1;
                }
                else if ( qFuzzyCompare(cfdValueA_10perc, valY2) ) {
                    timeStampA_10perc = timeStamp2;
                }
            }

            if ( estimCFDBCellStart_10perc == estimCFDBCellStop_10perc ) {
                timeStampB_10perc = tChannel1[estimCFDBCellStart_10perc];
            }
            else {
                const float timeStamp1 = tChannel1[estimCFDBCellStart_10perc];
                const float timeStamp2 = tChannel1[estimCFDBCellStop_10perc];

                const float valY1 = waveChannel1[estimCFDBCellStart_10perc];
                const float valY2 = waveChannel1[estimCFDBCellStop_10perc];

                if ( (cfdValueB_10perc < valY1 && cfdValueB_10perc > valY2)
                     || (cfdValueB_10perc > valY1 && cfdValueB_10perc < valY2) ) {
                    const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                    const double intersect = valY1 - slope*timeStamp1;

                    timeStampB_10perc = (cfdValueB_10perc - intersect)/slope;
                } else if ( qFuzzyCompare(cfdValueB_10perc, valY1) ) {
                    timeStampB_10perc = timeStamp1;
                } else if ( qFuzzyCompare(cfdValueB_10perc, valY2) ) {
                    timeStampB_10perc = timeStamp2;
                }
            }
        }

        double timeStampA_90perc = -1.0f;
        double timeStampB_90perc = -1.0f;

        if (!bUsingLinearInterpol) {
            /* find correct CF level (90%) timestamp within the estimated CF (90%) bracketed index region */
            if ( estimCFDACellStart_90perc == estimCFDACellStop_90perc ) {
                timeStampA_90perc = tChannel0[estimCFDACellStart_90perc];
            }
            else {
                const double timeIncr = (tChannel0[estimCFDACellStop_90perc] - tChannel0[estimCFDACellStart_90perc])/((double)intraRenderPoints);
                for ( int i = 0 ; i < intraRenderPoints ; ++ i ) {
                    const double timeStamp1 = tChannel0[estimCFDACellStart_90perc] + (double)i*timeIncr;
                    const double timeStamp2 = tChannel0[estimCFDACellStart_90perc] + (double)(i+1)*timeIncr;

                    const float valY1 = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantA, timeStamp1)):(tkSplineA(timeStamp1))):(alglib::barycentriccalc(m_baryCentricInterpolantA, timeStamp1));
                    const float valY2 = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantA, timeStamp2)):(tkSplineA(timeStamp2))):(alglib::barycentriccalc(m_baryCentricInterpolantA, timeStamp2));

                    if ( (cfdValueA_90perc < valY1 && cfdValueA_90perc > valY2)
                         || (cfdValueA_90perc > valY1 && cfdValueA_90perc < valY2) ) {
                        const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                        const double intersect = valY1 - slope*timeStamp1;

                        timeStampA_90perc = (cfdValueA_90perc - intersect)/slope;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueA_90perc, valY1) ) {
                        timeStampA_90perc = timeStamp1;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueA_90perc, valY2) ) {
                        timeStampA_90perc = timeStamp2;

                        break;
                    }
                }
            }

            if ( estimCFDBCellStart_90perc == estimCFDBCellStop_90perc ) {
                timeStampB_90perc = tChannel1[estimCFDBCellStart_90perc];
            }
            else {
                const double timeIncr = (tChannel1[estimCFDBCellStop_90perc] - tChannel1[estimCFDBCellStart_90perc])/((double)intraRenderPoints);
                for ( int i = 0 ; i < intraRenderPoints ; ++ i ) {
                    const double timeStamp1 = tChannel1[estimCFDBCellStart_90perc] + (double)i*timeIncr;
                    const double timeStamp2 = tChannel1[estimCFDBCellStart_90perc] + (double)(i+1)*timeIncr;

                    const float valY1 = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantB, timeStamp1)):(tkSplineB(timeStamp1))):(alglib::barycentriccalc(m_baryCentricInterpolantB, timeStamp1));
                    const float valY2 = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantB, timeStamp2)):(tkSplineB(timeStamp2))):(alglib::barycentriccalc(m_baryCentricInterpolantB, timeStamp2));

                    if ( (cfdValueB_90perc < valY1 && cfdValueB_90perc > valY2)
                         || (cfdValueB_90perc > valY1 && cfdValueB_90perc < valY2) ) {
                        const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                        const double intersect = valY1 - slope*timeStamp1;

                        timeStampB_90perc = (cfdValueB_90perc - intersect)/slope;

                        break;
                    } else if ( qFuzzyCompare(cfdValueB_90perc, valY1) ) {
                        timeStampB_90perc = timeStamp1;

                        break;
                    } else if ( qFuzzyCompare(cfdValueB_90perc, valY2) ) {
                        timeStampB_90perc = timeStamp2;

                        break;
                    }
                }
            }
        }
        /* linear interpolation */
        else {
            if ( estimCFDACellStart_90perc == estimCFDACellStop_90perc ) {
                timeStampA_90perc = tChannel0[estimCFDACellStart_90perc];
            }
            else {
                const float timeStamp1 = tChannel0[estimCFDACellStart_90perc];
                const float timeStamp2 = tChannel0[estimCFDACellStop_90perc];

                const float valY1 = waveChannel0[estimCFDACellStart_90perc];
                const float valY2 = waveChannel0[estimCFDACellStop_90perc];

                if ( (cfdValueA_90perc < valY1 && cfdValueA_90perc > valY2)
                     || (cfdValueA_90perc > valY1 && cfdValueA_90perc < valY2) ) {
                    const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                    const double intersect = valY1 - slope*timeStamp1;

                    timeStampA_90perc = (cfdValueA_90perc - intersect)/slope;
                }
                else if ( qFuzzyCompare(cfdValueA_90perc, valY1) ) {
                    timeStampA_90perc = timeStamp1;
                }
                else if ( qFuzzyCompare(cfdValueA_90perc, valY2) ) {
                    timeStampA_90perc = timeStamp2;
                }
            }

            if ( estimCFDBCellStart_90perc == estimCFDBCellStop_90perc ) {
                timeStampB_90perc = tChannel1[estimCFDBCellStart_90perc];
            }
            else {
                const float timeStamp1 = tChannel1[estimCFDBCellStart_90perc];
                const float timeStamp2 = tChannel1[estimCFDBCellStop_90perc];

                const float valY1 = waveChannel1[estimCFDBCellStart_90perc];
                const float valY2 = waveChannel1[estimCFDBCellStop_90perc];

                if ( (cfdValueB_90perc < valY1 && cfdValueB_90perc > valY2)
                     || (cfdValueB_90perc > valY1 && cfdValueB_90perc < valY2) ) {
                    const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                    const double intersect = valY1 - slope*timeStamp1;

                    timeStampB_90perc = (cfdValueB_90perc - intersect)/slope;
                }
                else if ( qFuzzyCompare(cfdValueB_90perc, valY1) ) {
                    timeStampB_90perc = timeStamp1;
                }
                else if ( qFuzzyCompare(cfdValueB_90perc, valY2) ) {
                    timeStampB_90perc = timeStamp2;
                }
            }
        }

        if ((int)timeStampA == -1
           || (int)timeStampB == -1) {
            if ((int)timeStampA == -1) {
                /* stream as 'false' pulse */
                if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed()
                     && DRS4FalseTruePulseStreamManager::sharedInstance()->isStreamingForABranch()) {
                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)tChannel0, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream time(0): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }

                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)waveChannel0S, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream volt(0): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }
                }
            }

            if ((int)timeStampB == -1) {
                /* stream as 'false' pulse */
                if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed()
                     && !DRS4FalseTruePulseStreamManager::sharedInstance()->isStreamingForABranch()) {
                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)tChannel1, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream time(1): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }

                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)waveChannel1S, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream volt(1): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }
                }
            }

            continue;
        }

        /* area-Filter */
        if (!bBurstMode
                && bPulseAreaPlot) {
            const int binA = areaA*(double)pulseAreaFilterBinningA;
            const int binB = areaB*(double)pulseAreaFilterBinningB;

            const int reducedNumberOfBins = (kNumberOfBins-1);

            if (!(cellPHSA>reducedNumberOfBins
                  || cellPHSB>reducedNumberOfBins
                  || cellPHSA<0
                  || cellPHSB<0
                  || binA<0
                  || binB<0)) {
                if (m_areaFilterACounter >= 5000 )
                    m_areaFilterACounter = 0;

                if (m_areaFilterBCounter >= 5000 )
                    m_areaFilterBCounter = 0;

                m_areaFilterDataA[m_areaFilterACounter] = QPointF(cellPHSA, binA);
                m_areaFilterDataB[m_areaFilterBCounter] = QPointF(cellPHSB, binB);

                m_areaFilterACounter ++;
                m_areaFilterBCounter ++;
            }
        }

        /* determine start and stop branches */
        bool bIsStart_A = false;
        bool bIsStop_A = false;

        bool bIsStart_B = false;
        bool bIsStop_B = false;

        if ( cellPHSA >= DRS4SettingsManager::sharedInstance()->startChanneAMin()
             && cellPHSA <= DRS4SettingsManager::sharedInstance()->startChanneAMax() )
            bIsStart_A = true;

        if ( cellPHSA >= DRS4SettingsManager::sharedInstance()->stopChanneAMin()
             && cellPHSA <= DRS4SettingsManager::sharedInstance()->stopChanneAMax() )
            bIsStop_A = true;

        if ( cellPHSB >= DRS4SettingsManager::sharedInstance()->startChanneBMin()
             && cellPHSB <= DRS4SettingsManager::sharedInstance()->startChanneBMax() )
            bIsStart_B = true;

        if ( cellPHSB >= DRS4SettingsManager::sharedInstance()->stopChanneBMin()
             && cellPHSB <= DRS4SettingsManager::sharedInstance()->stopChanneBMax() )
            bIsStop_B = true;

        /* rise-time Filter */
        if ((int)timeStampA_10perc != -1
                || (int)timeStampA_90perc != -1) {
            const int binA = (int)((double)riseTimeFilterABinning*(timeStampA_90perc-timeStampA_10perc)/riseTimeFilterAScale);

            if ( !(binA < 0 || binA >= riseTimeFilterABinning) ) {
                if (bIsStart_A || bIsStop_A) {
                    m_riseTimeFilterDataA[binA] ++;
                    m_riseTimeFilterACounter ++;
                    m_maxY_RiseTimeSpectrumA = qMax(m_maxY_RiseTimeSpectrumA, m_riseTimeFilterDataA[binA]);
                }
            }
        }

        if ((int)timeStampB_10perc != -1
                || (int)timeStampB_90perc != -1) {
            const int binB = (int)((double)riseTimeFilterBBinning*(timeStampB_90perc-timeStampB_10perc)/riseTimeFilterBScale);

            if ( !(binB < 0 || binB >= riseTimeFilterBBinning) ) {
                if (bIsStart_B || bIsStop_B) {
                    m_riseTimeFilterDataB[binB] ++;
                    m_riseTimeFilterBCounter ++;
                    m_maxY_RiseTimeSpectrumB = qMax(m_maxY_RiseTimeSpectrumB, m_riseTimeFilterDataB[binB]);
                }
            }
        }

        /* apply area-filter and reject pulses if one of both appears outside the windows */
        if (bPulseAreaFilter) {
            const double indexPHSA = cellPHSA;
            const double indexPHSB = cellPHSB;

            const double yLowerA = (*m_dataExchange->m_areaFilterASlopeLower*indexPHSA+*m_dataExchange->m_areaFilterAInterceptLower);
            const double yUpperA = (*m_dataExchange->m_areaFilterASlopeUpper*indexPHSA+*m_dataExchange->m_areaFilterAInterceptUpper);

            const double multA = areaA*pulseAreaFilterBinningA;
            const double multB = areaB*pulseAreaFilterBinningB;

            const bool y_AInside = (multA >= yLowerA && multA <=yUpperA);

            const double yLowerB = (*m_dataExchange->m_areaFilterBSlopeLower*indexPHSB+*m_dataExchange->m_areaFilterBInterceptLower);
            const double yUpperB = (*m_dataExchange->m_areaFilterBSlopeUpper*indexPHSB+*m_dataExchange->m_areaFilterBInterceptUpper);

            const bool y_BInside = (multB >= yLowerB && multB <=yUpperB);

            if (!y_AInside) {
                /* stream as 'false' pulse */
                if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed()
                     && DRS4FalseTruePulseStreamManager::sharedInstance()->isStreamingForABranch()) {
                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)tChannel0, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream time(0): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }

                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)waveChannel0S, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream volt(0): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }
                }
            }

            if (!y_BInside) {
                /* stream as 'false' pulse */
                if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed()
                     && !DRS4FalseTruePulseStreamManager::sharedInstance()->isStreamingForABranch()) {
                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)tChannel1, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream time(1): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }

                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)waveChannel1S, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream volt(1): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }
                }
            }

            if (y_AInside) {
                /* incremental (mean ; stddev) */
                double meanA  = m_areaFilterCollectedDataA[indexPHSA].x();
                double stddevA = m_areaFilterCollectedDataA[indexPHSA].y();

                m_areaFilterCollectedDataCounterA[indexPHSA] ++;

                if (m_areaFilterCollectedDataCounterA[indexPHSA] >= 2) {
                    stddevA = ((m_areaFilterCollectedDataCounterA[indexPHSA]-2)/(m_areaFilterCollectedDataCounterA[indexPHSA]-1))*stddevA*stddevA + (1.0/m_areaFilterCollectedDataCounterA[indexPHSA])*(areaA-meanA)*(areaA-meanA);
                    stddevA = sqrt(stddevA);
                }
                else
                    stddevA = 0.0;

                meanA = (1/(float)m_areaFilterCollectedDataCounterA[indexPHSA])*(areaA + float(m_areaFilterCollectedDataCounterA[indexPHSA] - 1)*meanA);


                m_areaFilterCollectedDataA[indexPHSA].setX(meanA);
                m_areaFilterCollectedDataA[indexPHSA].setY(stddevA);

                m_areaFilterCollectedACounter ++;
            }

            if (y_BInside) {
                /* incremental (mean ; stddev) */
                double meanB  = m_areaFilterCollectedDataB[indexPHSB].x();
                double stddevB = m_areaFilterCollectedDataB[indexPHSB].y();

                m_areaFilterCollectedDataCounterB[indexPHSB] ++;

                if (m_areaFilterCollectedDataCounterB[indexPHSB] >= 2) {
                    stddevB = ((m_areaFilterCollectedDataCounterB[indexPHSB]-2)/(m_areaFilterCollectedDataCounterB[indexPHSB]-1))*stddevB*stddevB + (1.0/m_areaFilterCollectedDataCounterB[indexPHSB])*(areaB-meanB)*(areaB-meanB);
                    stddevB = sqrt(stddevB);
                }
                else
                    stddevB = 0.0;

                meanB = (1/(float)m_areaFilterCollectedDataCounterB[indexPHSB])*(areaB + float(m_areaFilterCollectedDataCounterB[indexPHSB] - 1)*meanB);

                m_areaFilterCollectedDataB[indexPHSB].setX(meanB);
                m_areaFilterCollectedDataB[indexPHSB].setY(stddevB);

                m_areaFilterCollectedBCounter ++;
            }

            if ( !y_AInside || !y_BInside ) {
                continue;
            }
        }

        /* apply rise time-filter and reject pulses if one of both appears outside the windows */
        if (bPulseRiseTimeFilter) {
            const int binA = (int)((double)riseTimeFilterABinning*(timeStampA_90perc-timeStampA_10perc)/riseTimeFilterAScale);
            const int binB = (int)((double)riseTimeFilterBBinning*(timeStampB_90perc-timeStampB_10perc)/riseTimeFilterBScale);

            bool bAcceptedA = false;
            bool bAcceptedB = false;

            if (binA >= riseTimeFilterWindowLeftA && binA <= riseTimeFilterWindowRightA)
                bAcceptedA = true;

            if (binB >= riseTimeFilterWindowLeftB && binB <= riseTimeFilterWindowRightB)
                bAcceptedB = true;

            if (!bAcceptedA) {
                /* stream as 'false' pulse */
                if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed()
                     && DRS4FalseTruePulseStreamManager::sharedInstance()->isStreamingForABranch()) {
                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)tChannel0, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream time(0): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }

                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)waveChannel0S, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream volt(0): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }
                }
            }

            if (!bAcceptedB) {
                /* stream as 'false' pulse */
                if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed()
                     && !DRS4FalseTruePulseStreamManager::sharedInstance()->isStreamingForABranch()) {
                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)tChannel1, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream time(1): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }

                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)waveChannel1S, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream volt(1): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }
                }
            }

            if (!bAcceptedA || !bAcceptedB) {
                continue;
            }
        }

        /* apply pulse-shape filter */
        if (bPulseShapeFilterIsEnabledA
             || bPulseShapeFilterIsEnabledB) {
            bool bRejectA = false;
            bool bRejectB = false;

            const float fractYMaxA = 1.0f/yMaxA;
            const float fractYMaxB = 1.0f/yMaxB;

            const float fractYMinA = 1.0f/yMinA;
            const float fractYMinB = 1.0f/yMinB;

            const int size = kNumberOfBins;

            const double leftOfRefA = -DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfA();
            const double rightOfRefA = DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfA();

            const double leftOfRefB = -DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfB();
            const double rightOfRefB = DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfB();

            DRS4PulseShapeFilterData *filterDataA = DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrA();
            DRS4PulseShapeFilterData *filterDataB = DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrB();

            const double lowerStddevFractionA = DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevLowerFractionA();
            const double lowerStddevFractionB = DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevLowerFractionB();

            const double upperStddevFractionA = DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevUpperFractionA();
            const double upperStddevFractionB = DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevUpperFractionB();

            for ( int j = 0 ; j < size ; ++ j ) {
                const double tA =  (tChannel0[j] - timeAForYMax);
                const double tB =  (tChannel1[j] - timeBForYMax);

                const bool bROIA = (tA >= leftOfRefA) && (tA <= rightOfRefA);
                const bool bROIB = (tB >= leftOfRefB) && (tB <= rightOfRefB);

                if (bROIA && bPulseShapeFilterIsEnabledA) {
                    const float yA = positiveSignal?(waveChannel0[j]*fractYMaxA):(waveChannel0[j]*fractYMinA);

                    if (filterDataA) {
                        if (!filterDataA->isInsideBounding(tA, yA, lowerStddevFractionA, upperStddevFractionA))
                            bRejectA = true;
                    }
                }

                if (bPulseShapeFilterIsEnabledA && bRejectA)
                    break;

                if (bROIB && bPulseShapeFilterIsEnabledB) {
                    const float yB = positiveSignal?(waveChannel1[j]*fractYMaxB):(waveChannel1[j]*fractYMinB);

                    if (filterDataB) {
                        if (!filterDataB->isInsideBounding(tB, yB, lowerStddevFractionB, upperStddevFractionB))
                            bRejectB = true;
                    }
                }

                if (bPulseShapeFilterIsEnabledB && bRejectB)
                    break;
            }

            if (bRejectA) {
                /* stream as 'false' pulse */
                if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed()
                     && DRS4FalseTruePulseStreamManager::sharedInstance()->isStreamingForABranch()) {
                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)tChannel0, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream time(0): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }

                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)waveChannel0S, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream volt(0): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }
                }
            }

            if (bRejectB) {
                /* stream as 'false' pulse */
                if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed()
                     && !DRS4FalseTruePulseStreamManager::sharedInstance()->isStreamingForABranch()) {
                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)tChannel1, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream time(1): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }

                    if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeFalsePulse((const char*)waveChannel1S, sizeOfWave)) {
                        // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteFalsePulseStream volt(1): size(" + QVariant(sizeOfWave).toString() + ")"));
                    }
                }
            }

            if (bRejectA || bRejectB) {
                continue;
            }
        }

        bool bValidLifetime = false;
        bool bValidLifetime2 = false;

        /* lifetime: A-B - master */
        if ( bIsStart_A
             && bIsStop_B && !bForcePrompt  ) {
            const double ltdiff = (timeStampB - timeStampA);
            const int binAB = ((int)round(((((ltdiff)+offsetAB)/scalerAB))*((double)channelCntAB)))-1;

            const int binMerged = ((int)round(((((ltdiff+ATS)+offsetMerged)/scalerMerged))*((double)channelCntMerged)))-1;

            if ( binAB < 0 || binAB >= channelCntAB )
                continue;

            if ( binAB >= 0
                 && binAB < channelCntAB ) {
                if ( bNegativeLT && ltdiff < 0  ) {
                    m_lifeTimeDataAB[binAB] ++;
                    m_abCounts ++;

                    m_maxY_ABSpectrum = qMax(m_maxY_ABSpectrum, m_lifeTimeDataAB[binAB]);

                    if (rcScheme == DRS4PulseShapeFilterRecordScheme::Scheme::RC_AB)
                        bValidLifetime = true;

                    bValidLifetime2 = true;
                }
                else if ( ltdiff >= 0 ) {
                    m_lifeTimeDataAB[binAB] ++;
                    m_abCounts ++;

                    m_maxY_ABSpectrum = qMax(m_maxY_ABSpectrum, m_lifeTimeDataAB[binAB]);

                    if (rcScheme == DRS4PulseShapeFilterRecordScheme::Scheme::RC_AB)
                        bValidLifetime = true;

                    bValidLifetime2 = true;
                }

                m_specABCounterCnt ++;

                if ( bStreamInRangeArmed && bValidLifetime2) {
                    if ( binAB >= DRS4TextFileStreamRangeManager::sharedInstance()->ltRangeMinAB()
                         && binAB <= DRS4TextFileStreamRangeManager::sharedInstance()->ltRangeMaxAB() ) {
                        if ( DRS4TextFileStreamRangeManager::sharedInstance()->isABEnabled() )
                            DRS4TextFileStreamRangeManager::sharedInstance()->writePulses(pulseDataA(), pulseDataB(), timeStampA, timeStampB);
                    }
                }

                /******* count here for phs created of valid lifetimes ********************/
                //if ( cellPHSA < kNumberOfBins && cellPHSA >= 0 ) {
                    //m_phsA[cellPHSA] ++;
                    //m_phsACounts ++;
                //}

                //if ( cellPHSB < kNumberOfBins && cellPHSB >= 0 ) {
                    //m_phsB[cellPHSB] ++;
                    //m_phsBCounts ++;
                //}
                /******* count here for phs created of valid lifetimes ********************/

                /* calculate normalized persistance data */
                if (bPersistance && bValidLifetime2 && !bBurstMode) {
                    m_persistanceDataA.clear();
                    m_persistanceDataB.clear();

                    m_persistanceDataA.resize(m_pListChannelA.size());
                    m_persistanceDataB.resize(m_pListChannelB.size());

                    const float fractYMaxA = 1.0f/yMaxA;
                    const float fractYMaxB = 1.0f/yMaxB;

                    const float fractYMinA = 1.0f/yMinA;
                    const float fractYMinB = 1.0f/yMinB;

                    const int size = m_persistanceDataA.size();
                    for ( int j = 0 ; j < size ; ++ j ) {
                        const float yA = positiveSignal?(m_pListChannelA.at(j).y()*fractYMaxA):(m_pListChannelA.at(j).y()*fractYMinA);
                        const float yB = positiveSignal?(m_pListChannelB.at(j).y()*fractYMaxB):(m_pListChannelB.at(j).y()*fractYMinB);

                        m_persistanceDataA[j] = QPointF(m_pListChannelA.at(j).x()-((!bOppositePersistanceA)?timeStampA:timeStampB), yA); // >> shift channel A relative to CFD-%(t0) of A (of B)
                        m_persistanceDataB[j] = QPointF(m_pListChannelB.at(j).x()-((!bOppositePersistanceB)?timeStampB:timeStampA), yB); // >> shift channel B relative to CFD-%(t0) of B (of A)
                    }
                }

                if (bValidLifetime2) {
                    /* stream as 'false' pulse */
                    if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed()) {
                        if (DRS4FalseTruePulseStreamManager::sharedInstance()->isStreamingForABranch() ) {
                            if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeTruePulse((const char*)tChannel0, sizeOfWave)) {
                                // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteTruePulseStream time(0): size(" + QVariant(sizeOfWave).toString() + ")"));
                            }

                            if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeTruePulse((const char*)waveChannel0S, sizeOfWave)) {
                                // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteTruePulseStream volt(0): size(" + QVariant(sizeOfWave).toString() + ")"));
                            }
                        }
                        else {
                            if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeTruePulse((const char*)tChannel1, sizeOfWave)) {
                                // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteTruePulseStream time(1): size(" + QVariant(sizeOfWave).toString() + ")"));
                            }

                            if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeTruePulse((const char*)waveChannel1S, sizeOfWave)) {
                                // DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteTruePulseStream volt(1): size(" + QVariant(sizeOfWave).toString() + ")"));
                            }
                        }
                    }
                }

                /* recording pulse shape data if required */
                if (!bBurstMode && bValidLifetime)
                    recordPulseShapeData(positiveSignal, timeAForYMax, timeBForYMax, yMinA, yMaxA, yMinB, yMaxB);
            }

            if ( binMerged < 0 || binMerged >= channelCntMerged )
                continue;

            if ( binMerged >= 0
                 && binMerged < channelCntMerged ) {
                if ( bNegativeLT && ltdiff < 0  ) {
                    m_lifeTimeDataMerged[binMerged] ++;
                    m_mergedCounts ++;

                    m_maxY_MergedSpectrum = qMax(m_maxY_MergedSpectrum, m_lifeTimeDataMerged[binMerged]);
                }
                else if ( ltdiff >= 0 ) {
                    m_lifeTimeDataMerged[binMerged] ++;
                    m_mergedCounts ++;

                    m_maxY_MergedSpectrum = qMax(m_maxY_MergedSpectrum, m_lifeTimeDataMerged[binMerged]);
                }

                m_specMergedCounterCnt ++;
            }
        }
        /* lifetime: B-A - master */
        else if ( bIsStart_B
                   && bIsStop_A && !bForcePrompt ) {
            const double ltdiff = (timeStampA - timeStampB);
            const int binBA = ((int)round(((((ltdiff)+offsetBA)/scalerBA))*((double)channelCntBA)))-1;

            const int binMerged = ((int)round(((((ltdiff-ATS)+offsetMerged)/scalerMerged))*((double)channelCntMerged)))-1;

            if ( binBA < 0 || binBA >= channelCntBA )
                continue;

            if ( binBA >= 0
                 && binBA < channelCntBA ) {
                if ( bNegativeLT && ltdiff < 0 ) {
                    m_lifeTimeDataBA[binBA] ++;
                    m_baCounts ++;

                    m_maxY_BASpectrum = qMax(m_maxY_BASpectrum, m_lifeTimeDataBA[binBA]);

                    if (rcScheme == DRS4PulseShapeFilterRecordScheme::Scheme::RC_BA)
                        bValidLifetime = true;

                    bValidLifetime2 = true;
                }
                else if ( ltdiff >= 0 ) {
                    m_lifeTimeDataBA[binBA] ++;
                    m_baCounts ++;

                    m_maxY_BASpectrum = qMax(m_maxY_BASpectrum, m_lifeTimeDataBA[binBA]);

                    if (rcScheme == DRS4PulseShapeFilterRecordScheme::Scheme::RC_BA)
                        bValidLifetime = true;

                    bValidLifetime2 = true;
                }

                m_specBACounterCnt ++;

                if ( bStreamInRangeArmed && bValidLifetime2) {
                    if ( binBA >= DRS4TextFileStreamRangeManager::sharedInstance()->ltRangeMinBA()
                         && binBA <= DRS4TextFileStreamRangeManager::sharedInstance()->ltRangeMaxBA() ) {
                        if ( DRS4TextFileStreamRangeManager::sharedInstance()->isBAEnabled() )
                            DRS4TextFileStreamRangeManager::sharedInstance()->writePulses(pulseDataA(), pulseDataB(), timeStampA, timeStampB);
                    }
                }

                /* calculate normalized persistance data */
                if (bPersistance && bValidLifetime2 && !bBurstMode) {
                    m_persistanceDataA.clear();
                    m_persistanceDataB.clear();

                    m_persistanceDataA.resize(m_pListChannelA.size());
                    m_persistanceDataB.resize(m_pListChannelB.size());

                    const float fractYMaxA = 1.0f/yMaxA;
                    const float fractYMaxB = 1.0f/yMaxB;

                    const float fractYMinA = 1.0f/yMinA;
                    const float fractYMinB = 1.0f/yMinB;

                    const int size = m_persistanceDataA.size();
                    for ( int j = 0 ; j < size ; ++ j ) {
                        const float yA = positiveSignal?(m_pListChannelA.at(j).y()*fractYMaxA):(m_pListChannelA.at(j).y()*fractYMinA);
                        const float yB = positiveSignal?(m_pListChannelB.at(j).y()*fractYMaxB):(m_pListChannelB.at(j).y()*fractYMinB);

                        m_persistanceDataA[j] = QPointF(m_pListChannelA.at(j).x()-((!bOppositePersistanceA)?timeStampA:timeStampB), yA); // >> shift channel A relative to CFD-%(t0) of A (of B)
                        m_persistanceDataB[j] = QPointF(m_pListChannelB.at(j).x()-((!bOppositePersistanceB)?timeStampB:timeStampA), yB); // >> shift channel B relative to CFD-%(t0) of B (of A)
                    }
                }

                /*******count here for phs created of valid lifetimes********************/
                /*if ( cellPHSA < kNumberOfBins && cellPHSA >= 0 ) {
                    m_phsA[cellPHSA] ++;
                    m_phsACounts ++;
                }

                if ( cellPHSB < kNumberOfBins && cellPHSB >= 0 ) {
                    m_phsB[cellPHSB] ++;
                    m_phsBCounts ++;
                }*/

                 /*******count here for phs created of valid lifetimes********************/

                if (bValidLifetime2) {
                    if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed()) {
                        if (DRS4FalseTruePulseStreamManager::sharedInstance()->isStreamingForABranch() ) {
                            if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeTruePulse((const char*)tChannel0, sizeOfWave)) {
                                /* nothing yet */
                            }

                            if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeTruePulse((const char*)waveChannel0S, sizeOfWave)) {
                                /* nothing yet */
                            }
                        }
                        else {
                            if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeTruePulse((const char*)tChannel1, sizeOfWave)) {
                                /* nothing yet */
                            }

                            if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeTruePulse((const char*)waveChannel1S, sizeOfWave)) {
                                /* nothing yet */
                            }
                        }
                    }
                }

                /* recording pulse shape data if required */
                if (!bBurstMode && bValidLifetime)
                    recordPulseShapeData(positiveSignal, timeAForYMax, timeBForYMax, yMinA, yMaxA, yMinB, yMaxB);
            }

            if ( binMerged < 0 || binMerged >= channelCntMerged )
                continue;

            if ( binMerged >= 0
                 && binMerged < channelCntMerged ) {
                if ( bNegativeLT && ltdiff < 0  ) {
                    m_lifeTimeDataMerged[binMerged] ++;
                    m_mergedCounts ++;

                    m_maxY_MergedSpectrum = qMax(m_maxY_MergedSpectrum, m_lifeTimeDataMerged[binMerged]);
                }
                else if ( ltdiff >= 0 ) {
                    m_lifeTimeDataMerged[binMerged] ++;
                    m_mergedCounts ++;

                    m_maxY_MergedSpectrum = qMax(m_maxY_MergedSpectrum, m_lifeTimeDataMerged[binMerged]);
                }

                m_specMergedCounterCnt ++;
            }
        }
        /* prompt spectrum: A-B of stop - slave */
        else if (  bIsStop_B && bIsStop_A ) {
            const double ltdiff = (timeStampA - timeStampB);
            const int binBA = ((int)round(((((ltdiff)+offsetPrompt)/scalerPrompt))*((double)channelCntPrompt)))-1;

            if ( binBA < 0 || binBA >= channelCntPrompt )
                continue;

            if ( binBA >= 0
                 && binBA < channelCntPrompt ) {
                m_lifeTimeDataCoincidence[binBA] ++;
                m_coincidenceCounts ++;

                m_maxY_CoincidenceSpectrum = qMax(m_maxY_CoincidenceSpectrum, m_lifeTimeDataCoincidence[binBA]);

                if (rcScheme == DRS4PulseShapeFilterRecordScheme::Scheme::RC_Prompt)
                    bValidLifetime = true;

                bValidLifetime2 = true;

                m_specCoincidenceCounterCnt ++;

                /* recording pulse shape data if required */
                if (!bBurstMode && bValidLifetime)
                    recordPulseShapeData(positiveSignal, timeAForYMax, timeBForYMax, yMinA, yMaxA, yMinB, yMaxB);
            }

            if (bValidLifetime2) {
                if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed()) {
                    if (DRS4FalseTruePulseStreamManager::sharedInstance()->isStreamingForABranch() ) {
                        if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeTruePulse((const char*)tChannel0, sizeOfWave)) {
                        }

                        if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeTruePulse((const char*)waveChannel0S, sizeOfWave)) {
                            /* nothing yet */
                        }
                    }
                    else {
                        if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeTruePulse((const char*)tChannel1, sizeOfWave)) {
                            /* nothing yet */
                        }

                        if (!DRS4FalseTruePulseStreamManager::sharedInstance()->writeTruePulse((const char*)waveChannel1S, sizeOfWave)) {
                            /* nothing yet */
                        }
                    }
                }
            }
        } //end prompt
    }
}

void DRS4Worker::runMultiThreaded()
{
    const int sizeOfWave = sizeof(float)*kNumberOfBins;
    const int sizeOfFloat = 1/sizeof(float);

    m_isBlocking = false;
    m_isRunning = true;

    const bool bDemoMode = DRS4BoardManager::sharedInstance()->isDemoModeEnabled();
    const bool bIgnoreBusyState = DRS4SettingsManager::sharedInstance()->ignoreBusyState(); /* this value is deprecated and for test purposes only */
    const int pulsePairChunkSize = DRS4ProgramSettingsManager::sharedInstance()->pulsePairChunkSize();

    if (!bDemoMode)
        DRS4BoardManager::sharedInstance()->currentBoard()->StartDomino();

    time_t start;
    time_t stop;

    time(&start);
    time(&stop);

    forever {
        if ( !m_isRunning ) {
            m_isBlocking = false;

            return;
        }

        if (!bIgnoreBusyState) {
            if ( !bDemoMode ) {
                while ( !DRS4BoardManager::sharedInstance()->currentBoard()->IsEventAvailable() ) {
                    while ( !nextSignal() ) {
                        m_isBlocking = true;
                    }

                    if ( !m_isRunning ) {
                        m_isBlocking = false;

                        return;
                    }
                }
            }
        }

        while ( !nextSignal() ) {
            m_isBlocking = true;
        }

        m_isBlocking = false;

        const int chnA = DRS4SettingsManager::sharedInstance()->channelNumberA();
        const int chnB = DRS4SettingsManager::sharedInstance()->channelNumberB();

        if (!bDemoMode) {
            try {
                const int minChn = qMin(chnA, chnB);
                const int maxChn = qMax(chnA, chnB);

                DRS4BoardManager::sharedInstance()->currentBoard()->TransferWaves(minChn, maxChn);
            }
            catch (...) {
                continue;
            }
        }

        /* define concurrent input data */
        DRS4ConcurrentCopyInputData inputData;

        inputData.m_tChannel0[kNumberOfBins] = {0};
        inputData.m_tChannel1[kNumberOfBins] = {0};

        inputData.m_waveChannel0[kNumberOfBins] = {0};
        inputData.m_waveChannel1[kNumberOfBins] = {0};

        std::fill(inputData.m_tChannel0, inputData.m_tChannel0 + sizeof(inputData.m_tChannel0)*sizeOfFloat, 0);
        std::fill(inputData.m_tChannel1, inputData.m_tChannel1 + sizeof(inputData.m_tChannel1)*sizeOfFloat, 0);

        std::fill(inputData.m_waveChannel0, inputData.m_waveChannel0 + sizeof(inputData.m_waveChannel0)*sizeOfFloat, 0);
        std::fill(inputData.m_waveChannel1, inputData.m_waveChannel1 + sizeof(inputData.m_waveChannel1)*sizeOfFloat, 0);

        /* the 'S' extension represents the holding as source (original) data set */
        float waveChannel0S[kNumberOfBins] = {0};
        float waveChannel1S[kNumberOfBins] = {0};

        std::fill(waveChannel0S, waveChannel0S + sizeof(waveChannel0S)*sizeOfFloat, 0);
        std::fill(waveChannel1S, waveChannel1S + sizeof(waveChannel1S)*sizeOfFloat, 0);

        if (!bDemoMode) {
            int retState = 1;
            int retStateT = 1;
            int retStateV = kSuccess;

            try {
                retStateT = DRS4BoardManager::sharedInstance()->currentBoard()->GetTime(0, 2*chnA, DRS4BoardManager::sharedInstance()->currentBoard()->GetTriggerCell(0), inputData.m_tChannel0);
            }
            catch ( ... ) {
                continue;
            }

            if ( retStateT != 1 ) {
                continue;
            }

            try {
                retStateT = DRS4BoardManager::sharedInstance()->currentBoard()->GetTime(0, 2*chnB ,DRS4BoardManager::sharedInstance()->currentBoard()->GetTriggerCell(0), inputData.m_tChannel1);
            }
            catch ( ... ) {
                continue;
            }

            if ( retStateT != 1 ) {
                continue;
            }

            try {
                retStateV = DRS4BoardManager::sharedInstance()->currentBoard()->GetWave(0, 2*chnA, inputData.m_waveChannel0);
            }
            catch ( ... ) {
                continue;
            }

            if ( retStateV != kSuccess ) {
                continue;
            }

            try {
                retStateV = DRS4BoardManager::sharedInstance()->currentBoard()->GetWave(0, 2*chnB, inputData.m_waveChannel1);
            }
            catch ( ... ) {
                continue;
            }

            if ( retStateV != kSuccess ) {
                continue;
            }

            try {
                retState = DRS4BoardManager::sharedInstance()->currentBoard()->StartDomino(); // returns always 1.
            }
            catch ( ... ) {
            }
        }
        else {
            if ( !DRS4BoardManager::sharedInstance()->usingStreamDataOnDemoMode() ) {
                if ( !DRS4PulseGenerator::sharedInstance()->receiveGeneratedPulsePair(inputData.m_tChannel0, inputData.m_waveChannel0, inputData.m_tChannel1, inputData.m_waveChannel1) )
                    continue;
            }
            else {
                if ( !DRS4StreamDataLoader::sharedInstance()->isArmed() )
                    continue;

                if ( !DRS4StreamDataLoader::sharedInstance()->receiveGeneratedPulsePair(inputData.m_tChannel0, inputData.m_waveChannel0, inputData.m_tChannel1, inputData.m_waveChannel1) ) {
                    continue;
                }
            }
        }

        /* statistics: */
        time(&stop);
        const double diffTime = difftime(stop, start);
        if ( diffTime >= __STATISTIC_AVG_TIME ) {
            m_currentPulseCountRateInSeconds = ((double)m_pulseCounterCnt)/diffTime;
            m_summedPulseCountRateInSeconds += m_currentPulseCountRateInSeconds;
            m_avgPulseCountRateInSeconds = (m_pulseCounterCntAvg==0)?m_currentPulseCountRateInSeconds:(m_summedPulseCountRateInSeconds/(double)m_pulseCounterCntAvg);

            m_currentABSpecCountRateInSeconds = ((m_specABCounterCnt==0)?0:((double)m_specABCounterCnt)/diffTime);
            m_summedABSpecCountRateInSeconds += m_currentABSpecCountRateInSeconds;
            m_avgABSpecCountRateInSeconds = (m_specABCounterCntAvg==0)?m_currentABSpecCountRateInSeconds:(m_summedABSpecCountRateInSeconds/(double)m_specABCounterCntAvg);

            m_currentBASpecCountRateInSeconds = ((m_specBACounterCnt==0)?0:((double)m_specBACounterCnt)/diffTime);
            m_summedBASpecCountRateInSeconds += m_currentBASpecCountRateInSeconds;
            m_avgBASpecCountRateInSeconds = (m_specBACounterCntAvg==0)?m_currentBASpecCountRateInSeconds:(m_summedBASpecCountRateInSeconds/(double)m_specBACounterCntAvg);

            m_currentMergedSpecCountRateInSeconds = ((m_specMergedCounterCnt==0)?0:((double)m_specMergedCounterCnt)/diffTime);
            m_summedMergedSpecCountRateInSeconds += m_currentMergedSpecCountRateInSeconds;
            m_avgMergedSpecCountRateInSeconds = (m_specMergedCounterCntAvg==0)?m_currentMergedSpecCountRateInSeconds:(m_summedMergedSpecCountRateInSeconds/(double)m_specMergedCounterCntAvg);

            m_currentCoincidenceSpecCountRateInSeconds = ((m_specCoincidenceCounterCnt==0)?0:((double)m_specCoincidenceCounterCnt)/diffTime);
            m_summedCoincidenceSpecCountRateInSeconds += m_currentCoincidenceSpecCountRateInSeconds;
            m_avgCoincidenceSpecCountRateInSeconds = (m_specCoincidencCounterCntAvg==0)?m_currentCoincidenceSpecCountRateInSeconds:(m_summedCoincidenceSpecCountRateInSeconds/(double)m_specCoincidencCounterCntAvg);

            time(&start);

            m_pulseCounterCnt = 0;
            m_pulseCounterCntAvg ++;

            m_specABCounterCnt = 0;
            m_specBACounterCnt = 0;
            m_specMergedCounterCnt = 0;
            m_specCoincidenceCounterCnt = 0;

            m_specABCounterCntAvg ++;
            m_specBACounterCntAvg ++;
            m_specMergedCounterCntAvg ++;
            m_specCoincidencCounterCntAvg ++;
        }

        m_pulseCounterCnt ++;

        /* ROI variables */
        inputData.m_startCell = DRS4SettingsManager::sharedInstance()->startCell();
        inputData.m_endRange = DRS4SettingsManager::sharedInstance()->stopCell();
        inputData.m_stopCellWidth = (kNumberOfBins - DRS4SettingsManager::sharedInstance()->stopCell());
        inputData.m_cellWidth = (kNumberOfBins - inputData.m_startCell - inputData.m_stopCellWidth);

        /* prevent mutex locking: call these functions only once within the loop */
        inputData.m_positiveSignal = DRS4SettingsManager::sharedInstance()->isPositiveSignal();
        inputData.m_cfdA = DRS4SettingsManager::sharedInstance()->cfdLevelA();
        inputData.m_cfdB = DRS4SettingsManager::sharedInstance()->cfdLevelB();
        inputData.m_bBurstMode = DRS4SettingsManager::sharedInstance()->isBurstMode();
        inputData.m_sweep = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();
        inputData.m_bPulseAreaPlot = DRS4SettingsManager::sharedInstance()->isPulseAreaFilterPlotEnabled();
        inputData.m_bPulseAreaFilter = DRS4SettingsManager::sharedInstance()->isPulseAreaFilterEnabled();
        inputData.m_bPulseRiseTimeFilter = DRS4SettingsManager::sharedInstance()->isRiseTimeFilterEnabled();

        inputData.m_interpolationType = DRS4SettingsManager::sharedInstance()->interpolationType();
        inputData.m_splineInterpolationType = DRS4SettingsManager::sharedInstance()->splineInterpolationType();
        inputData.m_bUsingALGLIB = (inputData.m_splineInterpolationType < 6 && inputData.m_splineInterpolationType > 1);
        inputData.m_bUsingTinoKluge = ((inputData.m_interpolationType == DRS4InterpolationType::type::spline) && (inputData.m_splineInterpolationType == 6));
        inputData.m_bUsingLinearInterpol = ((inputData.m_interpolationType == DRS4InterpolationType::type::spline) && (inputData.m_splineInterpolationType == 1));
        inputData.m_intraRenderPoints = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(DRS4SettingsManager::sharedInstance()->splineIntraSamplingCounts()):(DRS4SettingsManager::sharedInstance()->polynomialSamplingCounts());

        inputData.m_bPersistance = DRS4SettingsManager::sharedInstance()->isPersistanceEnabled();

        inputData.m_pulseAreaFilterBinningA = DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA();
        inputData.m_pulseAreaFilterBinningB = DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB();
        inputData.m_pulseAreaFilterNormA = DRS4SettingsManager::sharedInstance()->pulseAreaFilterNormalizationA();
        inputData.m_pulseAreaFilterNormB = DRS4SettingsManager::sharedInstance()->pulseAreaFilterNormalizationB();

        inputData.m_areaFilterASlopeUpper = *m_dataExchange->m_areaFilterASlopeUpper;
        inputData.m_areaFilterAInterceptUpper = *m_dataExchange->m_areaFilterAInterceptUpper;

        inputData.m_areaFilterASlopeLower = *m_dataExchange->m_areaFilterASlopeLower;
        inputData.m_areaFilterAInterceptLower = *m_dataExchange->m_areaFilterAInterceptLower;

        inputData.m_areaFilterBSlopeUpper = *m_dataExchange->m_areaFilterBSlopeUpper;
        inputData.m_areaFilterBInterceptUpper = *m_dataExchange->m_areaFilterBInterceptUpper;

        inputData.m_areaFilterBSlopeLower = *m_dataExchange->m_areaFilterBSlopeLower;
        inputData.m_areaFilterBInterceptLower = *m_dataExchange->m_areaFilterBInterceptLower;

        inputData.m_riseTimeFilterARangeInNanoseconds = DRS4SettingsManager::sharedInstance()->riseTimeFilterScaleInNanosecondsOfA();
        inputData.m_riseTimeFilterBRangeInNanoseconds = DRS4SettingsManager::sharedInstance()->riseTimeFilterScaleInNanosecondsOfB();

        inputData.m_riseTimeFilterBinningA = DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfA();
        inputData.m_riseTimeFilterBinningB = DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfB();

        inputData.m_riseTimeFilterLeftWindowA = DRS4SettingsManager::sharedInstance()->riseTimeFilterLeftWindowOfA();
        inputData.m_riseTimeFilterLeftWindowB = DRS4SettingsManager::sharedInstance()->riseTimeFilterLeftWindowOfB();

        inputData.m_riseTimeFilterRightWindowA = DRS4SettingsManager::sharedInstance()->riseTimeFilterRightWindowOfA();
        inputData.m_riseTimeFilterRightWindowB = DRS4SettingsManager::sharedInstance()->riseTimeFilterRightWindowOfB();

        inputData.m_pulseShapeFilterAIsRecording = m_isRecordingForShapeFilterA;
        inputData.m_pulseShapeFilterBIsRecording = m_isRecordingForShapeFilterB;

        inputData.m_pulseShapeFilterEnabledA = DRS4SettingsManager::sharedInstance()->pulseShapeFilterEnabledA();
        inputData.m_pulseShapeFilterEnabledB = DRS4SettingsManager::sharedInstance()->pulseShapeFilterEnabledB();

        inputData.m_pulseShapeFilterLeftInNsROIA = DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfA();
        inputData.m_pulseShapeFilterRightInNsROIA = DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfA();

        inputData.m_pulseShapeFilterLeftInNsROIB = DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfB();
        inputData.m_pulseShapeFilterRightInNsROIB = DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfB();

        inputData.m_pulseShapeFilterFractOfStdDevLowerA = DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevLowerFractionA();
        inputData.m_pulseShapeFilterFractOfStdDevUpperA = DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevUpperFractionA();

        inputData.m_pulseShapeFilterFractOfStdDevLowerB = DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevLowerFractionB();
        inputData.m_pulseShapeFilterFractOfStdDevUpperB = DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevUpperFractionB();

        inputData.m_rcScheme = DRS4SettingsManager::sharedInstance()->pulseShapeFilterRecordScheme();

        if (inputData.m_pulseShapeFilterEnabledA) {
            inputData.m_pulseShapeFilterDataMeanTraceA_X[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};
            inputData.m_pulseShapeFilterDataMeanTraceA_Y[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};

            inputData.m_pulseShapeFilterDataStdDevTraceA_X[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};
            inputData.m_pulseShapeFilterDataStdDevTraceA_Y[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};

            std::fill(inputData.m_pulseShapeFilterDataMeanTraceA_X, inputData.m_pulseShapeFilterDataMeanTraceA_X + sizeof(inputData.m_pulseShapeFilterDataMeanTraceA_X)*sizeOfFloat, 0);
            std::fill(inputData.m_pulseShapeFilterDataMeanTraceA_Y, inputData.m_pulseShapeFilterDataMeanTraceA_Y + sizeof(inputData.m_pulseShapeFilterDataMeanTraceA_Y)*sizeOfFloat, 0);

            std::fill(inputData.m_pulseShapeFilterDataStdDevTraceA_X, inputData.m_pulseShapeFilterDataStdDevTraceA_X + sizeof(inputData.m_pulseShapeFilterDataStdDevTraceA_X)*sizeOfFloat, 0);
            std::fill(inputData.m_pulseShapeFilterDataStdDevTraceA_Y, inputData.m_pulseShapeFilterDataStdDevTraceA_Y + sizeof(inputData.m_pulseShapeFilterDataStdDevTraceA_Y)*sizeOfFloat, 0);

            DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrA()->meanCpy(inputData.m_pulseShapeFilterDataMeanTraceA_X, inputData.m_pulseShapeFilterDataMeanTraceA_Y);
            DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrA()->stddevCpy(inputData.m_pulseShapeFilterDataStdDevTraceA_X, inputData.m_pulseShapeFilterDataStdDevTraceA_Y);
        }
        else {
            inputData.m_pulseShapeFilterDataMeanTraceA_X[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};
            inputData.m_pulseShapeFilterDataMeanTraceA_Y[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};

            inputData.m_pulseShapeFilterDataStdDevTraceA_X[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};
            inputData.m_pulseShapeFilterDataStdDevTraceA_Y[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};

            std::fill(inputData.m_pulseShapeFilterDataMeanTraceA_X, inputData.m_pulseShapeFilterDataMeanTraceA_X + sizeof(inputData.m_pulseShapeFilterDataMeanTraceA_X)*sizeOfFloat, 0);
            std::fill(inputData.m_pulseShapeFilterDataMeanTraceA_Y, inputData.m_pulseShapeFilterDataMeanTraceA_Y + sizeof(inputData.m_pulseShapeFilterDataMeanTraceA_Y)*sizeOfFloat, 0);

            std::fill(inputData.m_pulseShapeFilterDataStdDevTraceA_X, inputData.m_pulseShapeFilterDataStdDevTraceA_X + sizeof(inputData.m_pulseShapeFilterDataStdDevTraceA_X)*sizeOfFloat, 0);
            std::fill(inputData.m_pulseShapeFilterDataStdDevTraceA_Y, inputData.m_pulseShapeFilterDataStdDevTraceA_Y + sizeof(inputData.m_pulseShapeFilterDataStdDevTraceA_Y)*sizeOfFloat, 0);
        }

        if (inputData.m_pulseShapeFilterEnabledB) {
            inputData.m_pulseShapeFilterDataMeanTraceB_X[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};
            inputData.m_pulseShapeFilterDataMeanTraceB_Y[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};

            inputData.m_pulseShapeFilterDataStdDevTraceB_X[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};
            inputData.m_pulseShapeFilterDataStdDevTraceB_Y[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};

            std::fill(inputData.m_pulseShapeFilterDataMeanTraceB_X, inputData.m_pulseShapeFilterDataMeanTraceB_X + sizeof(inputData.m_pulseShapeFilterDataMeanTraceB_X)*sizeOfFloat, 0);
            std::fill(inputData.m_pulseShapeFilterDataMeanTraceB_Y, inputData.m_pulseShapeFilterDataMeanTraceB_Y + sizeof(inputData.m_pulseShapeFilterDataMeanTraceB_Y)*sizeOfFloat, 0);

            std::fill(inputData.m_pulseShapeFilterDataStdDevTraceB_X, inputData.m_pulseShapeFilterDataStdDevTraceB_X + sizeof(inputData.m_pulseShapeFilterDataStdDevTraceB_X)*sizeOfFloat, 0);
            std::fill(inputData.m_pulseShapeFilterDataStdDevTraceB_Y, inputData.m_pulseShapeFilterDataStdDevTraceB_Y + sizeof(inputData.m_pulseShapeFilterDataStdDevTraceB_Y)*sizeOfFloat, 0);

            DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrB()->meanCpy(inputData.m_pulseShapeFilterDataMeanTraceB_X, inputData.m_pulseShapeFilterDataMeanTraceB_Y);
            DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrB()->stddevCpy(inputData.m_pulseShapeFilterDataStdDevTraceB_X, inputData.m_pulseShapeFilterDataStdDevTraceB_Y);
        }
        else {
            inputData.m_pulseShapeFilterDataMeanTraceB_X[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};
            inputData.m_pulseShapeFilterDataMeanTraceB_Y[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};

            inputData.m_pulseShapeFilterDataStdDevTraceB_X[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};
            inputData.m_pulseShapeFilterDataStdDevTraceB_Y[__PULSESHAPEFILTER_SPLINE_TRACE_NUMBER] = {0};

            std::fill(inputData.m_pulseShapeFilterDataMeanTraceB_X, inputData.m_pulseShapeFilterDataMeanTraceB_X + sizeof(inputData.m_pulseShapeFilterDataMeanTraceB_X)*sizeOfFloat, 0);
            std::fill(inputData.m_pulseShapeFilterDataMeanTraceB_Y, inputData.m_pulseShapeFilterDataMeanTraceB_Y + sizeof(inputData.m_pulseShapeFilterDataMeanTraceB_Y)*sizeOfFloat, 0);

            std::fill(inputData.m_pulseShapeFilterDataStdDevTraceB_X, inputData.m_pulseShapeFilterDataStdDevTraceB_X + sizeof(inputData.m_pulseShapeFilterDataStdDevTraceB_X)*sizeOfFloat, 0);
            std::fill(inputData.m_pulseShapeFilterDataStdDevTraceB_Y, inputData.m_pulseShapeFilterDataStdDevTraceB_Y + sizeof(inputData.m_pulseShapeFilterDataStdDevTraceB_Y)*sizeOfFloat, 0);
        }

        inputData.m_channelCntAB = DRS4SettingsManager::sharedInstance()->channelCntAB();
        inputData.m_channelCntBA = DRS4SettingsManager::sharedInstance()->channelCntBA();
        inputData.m_channelCntPrompt = DRS4SettingsManager::sharedInstance()->channelCntCoincindence();
        inputData.m_channelCntMerged = DRS4SettingsManager::sharedInstance()->channelCntMerged();

        inputData.m_offsetAB = DRS4SettingsManager::sharedInstance()->offsetInNSAB();
        inputData.m_offsetBA = DRS4SettingsManager::sharedInstance()->offsetInNSBA();
        inputData.m_offsetPrompt = DRS4SettingsManager::sharedInstance()->offsetInNSCoincidence();
        inputData.m_offsetMerged = DRS4SettingsManager::sharedInstance()->offsetInNSMerged();

        inputData.m_scalerAB = DRS4SettingsManager::sharedInstance()->scalerInNSAB();
        inputData.m_scalerBA = DRS4SettingsManager::sharedInstance()->scalerInNSBA();
        inputData.m_scalerPrompt = DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence();
        inputData.m_scalerMerged = DRS4SettingsManager::sharedInstance()->scalerInNSMerged();

        inputData.m_ATS = DRS4SettingsManager::sharedInstance()->meanCableDelay();

        inputData.m_bNegativeLT = DRS4SettingsManager::sharedInstance()->isNegativeLTAccepted();
        inputData.m_bForcePrompt = DRS4SettingsManager::sharedInstance()->isforceCoincidence();

        inputData.m_startAMinPHS = DRS4SettingsManager::sharedInstance()->startChanneAMin();
        inputData.m_startAMaxPHS = DRS4SettingsManager::sharedInstance()->startChanneAMax();
        inputData.m_startBMinPHS = DRS4SettingsManager::sharedInstance()->startChanneBMin();
        inputData.m_startBMaxPHS = DRS4SettingsManager::sharedInstance()->startChanneBMax();

        inputData.m_stopAMinPHS = DRS4SettingsManager::sharedInstance()->stopChanneAMin();
        inputData.m_stopAMaxPHS = DRS4SettingsManager::sharedInstance()->stopChanneAMax();
        inputData.m_stopBMinPHS = DRS4SettingsManager::sharedInstance()->stopChanneBMin();
        inputData.m_stopBMaxPHS = DRS4SettingsManager::sharedInstance()->stopChanneBMax();

        inputData.m_bMedianFilterA = DRS4SettingsManager::sharedInstance()->medianFilterAEnabled();
        inputData.m_bMedianFilterB = DRS4SettingsManager::sharedInstance()->medianFilterBEnabled();
        inputData.m_medianFilterWindowSizeA = DRS4SettingsManager::sharedInstance()->medianFilterWindowSizeA();
        inputData.m_medianFilterWindowSizeB = DRS4SettingsManager::sharedInstance()->medianFilterWindowSizeB();

        /* Baseline - Jitter Corrections */
        bool bUseBaseLineCorrectionA = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationEnabledA();
        int bl_startCellA = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationStartCellA();
        int bl_cellRegionA = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationRegionA();
        double bl_valueA = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationShiftValueInMVA();
        bool bUseBaseLineCorrectionRejectionA = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitRejectLimitA();
        double bl_rejectionLimitA = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitInPercentageA();

        bool bUseBaseLineCorrectionB = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationEnabledB();
        int bl_startCellB = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationStartCellB();
        int bl_cellRegionB = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationRegionB();
        double bl_valueB = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationShiftValueInMVB();
        bool bUseBaseLineCorrectionRejectionB = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitRejectLimitB();
        double bl_rejectionLimitB = DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitInPercentageB();

        const bool bIntrinsicFilterA = (inputData.m_bMedianFilterA || bUseBaseLineCorrectionA);
        const bool bIntrinsicFilterB = (inputData.m_bMedianFilterB || bUseBaseLineCorrectionB);

        if (bIntrinsicFilterA) {
            copy(inputData.m_waveChannel0, inputData.m_waveChannel0 + kNumberOfBins, waveChannel0S);
        }

        if (bIntrinsicFilterB) {
            copy(inputData.m_waveChannel1, inputData.m_waveChannel1 + kNumberOfBins, waveChannel1S);
        }

        /* apply median filter to remove spikes */
        if (inputData.m_bMedianFilterA) {
            if (!DMedianFilter::apply(inputData.m_waveChannel0, kNumberOfBins, inputData.m_medianFilterWindowSizeA))
                continue;
        }

        if (inputData.m_bMedianFilterB) {
            if (!DMedianFilter::apply(inputData.m_waveChannel1, kNumberOfBins, inputData.m_medianFilterWindowSizeB))
                continue;
        }

        /* baseline - jitter corrections */
        if (bUseBaseLineCorrectionA) {
            const int endRegionA = (bl_startCellA + bl_cellRegionA - 1);

            double blA = 0.0f;

            for (int i = bl_startCellA ; i  < endRegionA ; ++ i )
                blA += inputData.m_waveChannel0[i];

            blA /= bl_cellRegionA;

            const bool limitExceededA = (abs(blA - bl_valueA)/500.0) > bl_rejectionLimitA*0.01;

            if (bUseBaseLineCorrectionRejectionA && limitExceededA)
                continue;

            for (int i = 0 ; i < kNumberOfBins ; ++ i)
                inputData.m_waveChannel0[i] -= blA;
        }

        if (bUseBaseLineCorrectionB) {
            const int endRegionB = (bl_startCellB + bl_cellRegionB - 1);

            double blB = 0.0f;

            for (int i = bl_startCellB ; i  < endRegionB ; ++ i )
                blB += inputData.m_waveChannel1[i];

            blB /= bl_cellRegionB;

            const bool limitExceededB = (abs(blB - bl_valueB)/500.0) > bl_rejectionLimitB*0.01;

            if (bUseBaseLineCorrectionRejectionB && limitExceededB)
                continue;

            for (int i = 0 ; i < kNumberOfBins ; ++ i)
                inputData.m_waveChannel1[i] -= blB;
        }

        /* clear pulse-data for new visualization */
        if (!inputData.m_bBurstMode) {
            resetPulseA();
            resetPulseB();

            m_pListChannelA.resize(inputData.m_cellWidth);
            m_pListChannelB.resize(inputData.m_cellWidth);

            /* reduce mathematical operations within in the loop */
            const int reducedEndRange = (inputData.m_endRange - 1);
            const int reducedCellWidth = (inputData.m_cellWidth - 1);

            /* determine min/max and proceed a CFD estimation in ROI */
            for ( int a = reducedEndRange, it = reducedCellWidth ; a >= inputData.m_startCell ; -- a, -- it ) {
                m_pListChannelA[it] = QPointF(inputData.m_tChannel0[a], inputData.m_waveChannel0[a]);
                m_pListChannelB[it] = QPointF(inputData.m_tChannel1[a], inputData.m_waveChannel1[a]);
            }
        }

        if ( DRS4StreamManager::sharedInstance()->isArmed() ) {
            if (!DRS4StreamManager::sharedInstance()->write((const char*)inputData.m_tChannel0, sizeOfWave)) {
                /* nothing yet */
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)(!bIntrinsicFilterA?inputData.m_waveChannel0:waveChannel0S), sizeOfWave)) {
                /* nothing yet */
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)inputData.m_tChannel1, sizeOfWave)) {
                /* nothing yet */
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)(!bIntrinsicFilterB?inputData.m_waveChannel1:waveChannel1S), sizeOfWave)) {
                /* nothing yet */
            }
        }

        m_copyData.append(inputData);

        if ( m_copyData.size() == pulsePairChunkSize ) {
            m_workerConcurrentManager->add(m_copyData);
            m_copyData.clear();
        }
    } // end forever
}

DRS4ConcurrentCopyOutputData runCalculation(const QVector<DRS4ConcurrentCopyInputData> &copyDataVec)
{
    if ( copyDataVec.size() == 0 )
        return DRS4ConcurrentCopyOutputData(0, 0, 0, 0);

    const int channelCntCoincindence = copyDataVec.first().m_channelCntPrompt;
    const int channelCntAB = copyDataVec.first().m_channelCntAB;
    const int channelCntBA = copyDataVec.first().m_channelCntBA;
    const int channelCntMerged = copyDataVec.first().m_channelCntMerged;

    for ( DRS4ConcurrentCopyInputData copyData : copyDataVec ) {
        if ( channelCntCoincindence != copyData.m_channelCntPrompt )
            return DRS4ConcurrentCopyOutputData(0, 0, 0, 0);

        if ( channelCntAB != copyData.m_channelCntAB )
            return DRS4ConcurrentCopyOutputData(0, 0, 0, 0);

        if ( channelCntBA != copyData.m_channelCntBA )
            return DRS4ConcurrentCopyOutputData(0, 0, 0, 0);

        if ( channelCntMerged != copyData.m_channelCntMerged )
            return DRS4ConcurrentCopyOutputData(0, 0, 0, 0);
    }

    /* ALGLIB */
    alglib::real_1d_array arrayDataX_A, arrayDataY_A;
    alglib::real_1d_array arrayDataX_B, arrayDataY_B;

    alglib::spline1dinterpolant interpolantA;
    alglib::spline1dinterpolant interpolantB;

    alglib::barycentricinterpolant baryCentricInterpolantA;
    alglib::barycentricinterpolant baryCentricInterpolantB;

    /* Tino Kluge */
    std::vector<double> arrayDataTKX_A, arrayDataTKY_A;
    std::vector<double> arrayDataTKX_B, arrayDataTKY_B;

    DSpline tkSplineA, tkSplineB;

    DRS4ConcurrentCopyOutputData outputData(channelCntCoincindence, channelCntAB, channelCntBA, channelCntMerged, false);

    for ( DRS4ConcurrentCopyInputData inputData : copyDataVec ) {
        if (inputData.m_bUsingALGLIB) {
            /* ALGLIB array allocation */
            arrayDataX_A.setlength(inputData.m_cellWidth);
            arrayDataY_A.setlength(inputData.m_cellWidth);
            arrayDataX_B.setlength(inputData.m_cellWidth);
            arrayDataY_B.setlength(inputData.m_cellWidth);
        }
        else if (inputData.m_bUsingTinoKluge) {
            arrayDataTKX_A.resize(inputData.m_cellWidth);
            arrayDataTKX_B.resize(inputData.m_cellWidth);
            arrayDataTKY_A.resize(inputData.m_cellWidth);
            arrayDataTKY_B.resize(inputData.m_cellWidth);
        }
        else if (inputData.m_bUsingLinearInterpol) {
            /* nothing to do yet */
        }

        /* inline time and voltage extrema value estimation */
        float xMinA = inputData.m_sweep;
        float xMaxA = 0.0f;

        float xMinB = inputData.m_sweep;
        float xMaxB = 0.0f;

        float yMinA = 500.0f;
        float yMaxA = -500.0f;

        float yMinB = 500.0f;
        float yMaxB = -500.0f;

        /* inline voltage extrema value estimation */
        int cellYAMin = -1;
        int cellYAMax = -1;

        int cellYBMin = -1;
        int cellYBMax = -1;

        /* inline CFD value estimation */
        float cfdValueA = 0.0f;
        float cfdValueB = 0.0f;

        float cfdValueA_10perc = 0.0f;
        float cfdValueB_10perc = 0.0f;

        float cfdValueA_90perc = 0.0f;
        float cfdValueB_90perc = 0.0f;

        /* inline CFD cell estimation */
        int estimCFDACellStart = -1;
        int estimCFDBCellStart = -1;

        int estimCFDACellStop = -1;
        int estimCFDBCellStop = -1;

        /* inline CF (10%, 90%) cell estimation >> rise-time estimation (delta illumination signal) */
        int estimCFDACellStart_10perc = -1;
        int estimCFDBCellStart_10perc = -1;

        int estimCFDACellStart_90perc = -1;
        int estimCFDBCellStart_90perc = -1;

        int estimCFDACellStop_10perc = -1;
        int estimCFDBCellStop_10perc = -1;

        int estimCFDACellStop_90perc = -1;
        int estimCFDBCellStop_90perc = -1;

        int cfdACounter = 0;
        int cfdBCounter = 0;

        float areaA = 0.0f;
        float areaB = 0.0f;

        /* reduce mathematical operations within in the loop */
        const int reducedEndRange = (inputData.m_endRange - 1);
        const int reducedCellWidth = (inputData.m_cellWidth - 1);
        const int extendedStartCell = (inputData.m_startCell + 1);


        bool bForceReject = false;

        /* determine min/max and proceed a CFD estimation in ROI */
        for ( int a = reducedEndRange, it = reducedCellWidth ; a >= inputData.m_startCell ; -- a, -- it ) {
            /* time domain bracket values */
            xMinA = qMin(xMinA, inputData.m_tChannel0[a]);
            xMaxA = qMax(xMaxA, inputData.m_tChannel0[a]);

            xMinB = qMin(xMinB, inputData.m_tChannel1[a]);
            xMaxB = qMax(xMaxB, inputData.m_tChannel1[a]);

            if ( inputData.m_waveChannel0[a] >= yMaxA ) {
                yMaxA = inputData.m_waveChannel0[a];
                cellYAMax = a;

                if (inputData.m_positiveSignal) {
                    cfdValueA = inputData.m_cfdA*yMaxA;

                    /* t - rise according to the spec definition of a delta illumination signal */
                    cfdValueA_10perc = 0.1*yMaxA;
                    cfdValueA_90perc = 0.9*yMaxA;

                    cfdACounter = 0;
                }
            }

            if ( inputData.m_waveChannel1[a] >= yMaxB ) {
                yMaxB = inputData.m_waveChannel1[a];
                cellYBMax = a;

                if (inputData.m_positiveSignal) {
                    cfdValueB = inputData.m_cfdB*yMaxB;

                    /* t - rise according to the spec definition of a delta illumination signal */
                    cfdValueB_10perc = 0.1*yMaxB;
                    cfdValueB_90perc = 0.9*yMaxB;

                    cfdBCounter = 0;
                }
            }

            if ( inputData.m_waveChannel0[a] <= yMinA ) {
                yMinA = inputData.m_waveChannel0[a];
                cellYAMin = a;

                if (!inputData.m_positiveSignal) {
                    cfdValueA = inputData.m_cfdA*yMinA;

                    /* t - rise according to the spec definition of a delta illumination signal */
                    cfdValueA_10perc = 0.1*yMinA;
                    cfdValueA_90perc = 0.9*yMinA;

                    cfdACounter = 0;
                }
            }

            if ( inputData.m_waveChannel1[a] <= yMinB ) {
                yMinB = inputData.m_waveChannel1[a];
                cellYBMin = a;

                if (!inputData.m_positiveSignal) {
                    cfdValueB = inputData.m_cfdB*yMinB;

                    /* t - rise according to the spec definition of a delta illumination signal */
                    cfdValueB_10perc = 0.1*yMinB;
                    cfdValueB_90perc = 0.9*yMinB;

                    cfdBCounter = 0;
                }
            }

            /* CFD cell estimation/determination */
            if ( a >= extendedStartCell ) {
                const int aDecr = (a - 1);

                /* calculate the pulse area of ROI */
                if (inputData.m_bPulseAreaPlot
                        || inputData.m_bPulseAreaFilter) {
                    areaA += abs((inputData.m_waveChannel0[aDecr] + 0.5*(inputData.m_waveChannel0[a] - inputData.m_waveChannel0[aDecr]))*(inputData.m_tChannel0[a] - inputData.m_tChannel0[aDecr]));
                    areaB += abs((inputData.m_waveChannel1[aDecr] + 0.5*(inputData.m_waveChannel1[a] - inputData.m_waveChannel1[aDecr]))*(inputData.m_tChannel1[a] - inputData.m_tChannel1[aDecr]));
                }

                const double slopeA = (inputData.m_waveChannel0[a] - inputData.m_waveChannel0[aDecr])/(inputData.m_tChannel0[a] - inputData.m_tChannel0[aDecr]);
                const double slopeB = (inputData.m_waveChannel1[a] - inputData.m_waveChannel1[aDecr])/(inputData.m_tChannel1[a] - inputData.m_tChannel1[aDecr]);

                if (!qIsFinite(slopeA) || !qIsFinite(slopeB)) {
                    bForceReject = true;
                }

                /* CFD on rising edge = positive slope? or falling edge = negative slope?*/
                const bool bInRangeA = inputData.m_positiveSignal?(slopeA > 1E-6):(slopeA < 1E-6);
                const bool bInRangeB = inputData.m_positiveSignal?(slopeB > 1E-6):(slopeB < 1E-6);

                if ( bInRangeA ) {
                    const bool cfdLevelInRangeA = inputData.m_positiveSignal?(inputData.m_waveChannel0[a] > cfdValueA && inputData.m_waveChannel0[aDecr] < cfdValueA):(inputData.m_waveChannel0[a] < cfdValueA && inputData.m_waveChannel0[aDecr] > cfdValueA);

                    /* t - rise according to the spec definition of a delta illumination signal */
                    const bool cfdLevelInRangeA_10perc = inputData.m_positiveSignal?(inputData.m_waveChannel0[a] > cfdValueA_10perc && inputData.m_waveChannel0[aDecr] < cfdValueA_10perc):(inputData.m_waveChannel0[a] < cfdValueA_10perc && inputData.m_waveChannel0[aDecr] > cfdValueA_10perc);
                    const bool cfdLevelInRangeA_90perc = inputData.m_positiveSignal?(inputData.m_waveChannel0[a] > cfdValueA_90perc && inputData.m_waveChannel0[aDecr] < cfdValueA_90perc):(inputData.m_waveChannel0[a] < cfdValueA_90perc && inputData.m_waveChannel0[aDecr] > cfdValueA_90perc);

                    if ( cfdLevelInRangeA ) {
                        estimCFDACellStart = aDecr;
                        estimCFDACellStop = a;

                        cfdACounter ++;
                    }
                    else if ( qFuzzyCompare(inputData.m_waveChannel0[a], cfdValueA) ) {
                        estimCFDACellStart = a;
                        estimCFDACellStop = a;

                        cfdACounter ++;
                    }
                    else if ( qFuzzyCompare(inputData.m_waveChannel0[aDecr], cfdValueA) ) {
                        estimCFDACellStart = aDecr;
                        estimCFDACellStop = aDecr;

                        cfdACounter ++;
                    }

                    /* 10% CF Level */
                    if ( cfdLevelInRangeA_10perc ) {
                        estimCFDACellStart_10perc = aDecr;
                        estimCFDACellStop_10perc = a;
                    }
                    else if ( qFuzzyCompare(inputData.m_waveChannel0[a], cfdValueA_10perc) ) {
                        estimCFDACellStart_10perc = a;
                        estimCFDACellStop_10perc = a;
                    }
                    else if ( qFuzzyCompare(inputData.m_waveChannel0[aDecr], cfdValueA_10perc) ) {
                        estimCFDACellStart_10perc = aDecr;
                        estimCFDACellStop_10perc = aDecr;
                    }

                    /* 90% CF Level */
                    if ( cfdLevelInRangeA_90perc ) {
                        estimCFDACellStart_90perc = aDecr;
                        estimCFDACellStop_90perc = a;
                    }
                    else if ( qFuzzyCompare(inputData.m_waveChannel0[a], cfdValueA_90perc) ) {
                        estimCFDACellStart_90perc = a;
                        estimCFDACellStop_90perc = a;
                    }
                    else if ( qFuzzyCompare(inputData.m_waveChannel0[aDecr], cfdValueA_90perc) ) {
                        estimCFDACellStart_90perc = aDecr;
                        estimCFDACellStop_90perc = aDecr;
                    }
                }

                if ( bInRangeB ) {
                    const bool cfdLevelInRangeB = inputData.m_positiveSignal?(inputData.m_waveChannel1[a] > cfdValueB && inputData.m_waveChannel1[aDecr] < cfdValueB):(inputData.m_waveChannel1[a] < cfdValueB && inputData.m_waveChannel1[aDecr] > cfdValueB);

                    /* t - rise according to the spec definition of a delta illumination signal */
                    const bool cfdLevelInRangeB_10perc = inputData.m_positiveSignal?(inputData.m_waveChannel1[a] > cfdValueB_10perc && inputData.m_waveChannel1[aDecr] < cfdValueB_10perc):(inputData.m_waveChannel1[a] < cfdValueB_10perc && inputData.m_waveChannel1[aDecr] > cfdValueB_10perc);
                    const bool cfdLevelInRangeB_90perc = inputData.m_positiveSignal?(inputData.m_waveChannel1[a] > cfdValueB_90perc && inputData.m_waveChannel1[aDecr] < cfdValueB_90perc):(inputData.m_waveChannel1[a] < cfdValueB_90perc && inputData.m_waveChannel1[aDecr] > cfdValueB_90perc);

                    if ( cfdLevelInRangeB  ) {
                        estimCFDBCellStart = aDecr;
                        estimCFDBCellStop = a;

                        cfdBCounter ++;
                    }
                    else if ( qFuzzyCompare(inputData.m_waveChannel1[a], cfdValueB) ) {
                        estimCFDBCellStart = a;
                        estimCFDBCellStop = a;

                        cfdBCounter ++;
                    }
                    else if ( qFuzzyCompare(inputData.m_waveChannel1[aDecr], cfdValueB) ) {
                        estimCFDBCellStart = aDecr;
                        estimCFDBCellStop = aDecr;

                        cfdBCounter ++;
                    }

                    /* 10% CF Level */
                    if ( cfdLevelInRangeB_10perc ) {
                        estimCFDBCellStart_10perc = aDecr;
                        estimCFDBCellStop_10perc = a;
                    }
                    else if ( qFuzzyCompare(inputData.m_waveChannel1[a], cfdValueB_10perc) ) {
                        estimCFDBCellStart_10perc = a;
                        estimCFDBCellStop_10perc = a;
                    }
                    else if ( qFuzzyCompare(inputData.m_waveChannel1[aDecr], cfdValueB_10perc) ) {
                        estimCFDBCellStart_10perc = aDecr;
                        estimCFDBCellStop_10perc = aDecr;
                    }

                    /* 90% CF Level */
                    if ( cfdLevelInRangeB_90perc ) {
                        estimCFDBCellStart_90perc = aDecr;
                        estimCFDBCellStop_90perc = a;
                    }
                    else if ( qFuzzyCompare(inputData.m_waveChannel1[a], cfdValueB_90perc) ) {
                        estimCFDBCellStart_90perc = a;
                        estimCFDBCellStop_90perc = a;
                    }
                    else if ( qFuzzyCompare(inputData.m_waveChannel1[aDecr], cfdValueB_90perc) ) {
                        estimCFDBCellStart_90perc = aDecr;
                        estimCFDBCellStop_90perc = aDecr;
                    }
                }
            }

            if (bForceReject)
                continue;

            if (inputData.m_bUsingALGLIB) {
                /* ALGLIB array */
                arrayDataX_A.ptr->ptr.p_double[it] = inputData.m_tChannel0[a];
                arrayDataX_B.ptr->ptr.p_double[it] = inputData.m_tChannel1[a];

                arrayDataY_A.ptr->ptr.p_double[it] = inputData.m_waveChannel0[a];
                arrayDataY_B.ptr->ptr.p_double[it] = inputData.m_waveChannel1[a];
            }
            else if (inputData.m_bUsingTinoKluge) {
                arrayDataTKX_A[it] = inputData.m_tChannel0[a];
                arrayDataTKX_B[it] = inputData.m_tChannel1[a];

                arrayDataTKY_A[it] = inputData.m_waveChannel0[a];
                arrayDataTKY_B[it] = inputData.m_waveChannel1[a];
            } else if (inputData.m_bUsingLinearInterpol) {
                /* nothing yet */
            }
        }

        /* min/max was not able to be determined */
        if (cellYAMax == -1
                || cellYAMin == -1
                || cellYBMax == -1
                || cellYBMin == -1
                || qFuzzyCompare(yMinA, yMaxA)
                || qFuzzyCompare(yMinB, yMaxB)
                || (((int)yMinA == (int)yMaxA) || ((int)yMinB == (int)yMaxB)))
            continue;

        /* light-weight filtering of wrong events: */
        if ( inputData.m_positiveSignal ) {
            if ( (abs(yMinA) > abs(yMaxA))
                 || (abs(yMinB) > abs(yMaxB)) )
                continue;

            if ( abs(cellYAMax - inputData.m_startCell) <= 15
                 || abs(cellYBMax - inputData.m_startCell) <= 15 )
                continue;

            if ( qFuzzyCompare(inputData.m_waveChannel0[reducedEndRange], yMaxA)
                 || qFuzzyCompare(inputData.m_waveChannel1[reducedEndRange], yMaxB)
                 || qFuzzyCompare(inputData.m_waveChannel0[inputData.m_startCell], yMaxA)
                 || qFuzzyCompare(inputData.m_waveChannel1[inputData.m_startCell], yMaxB) )
                continue;
        }
        else {
            if ( (abs(yMinA) < abs(yMaxA))
                 || (abs(yMinB) < abs(yMaxB)) )
                continue;

            if ( abs(cellYAMin - inputData.m_startCell) <= 15
                 || abs(cellYBMin - inputData.m_startCell) <= 15 )
                continue;

            if ( qFuzzyCompare(inputData.m_waveChannel0[reducedEndRange], yMinA)
                 || qFuzzyCompare(inputData.m_waveChannel1[reducedEndRange], yMinB)
                 || qFuzzyCompare(inputData.m_waveChannel0[inputData.m_startCell], yMinA)
                 || qFuzzyCompare(inputData.m_waveChannel1[inputData.m_startCell], yMinB) )
                continue;
        }

        /* reject artifacts */
        if ( cfdBCounter > 1 || cfdBCounter == 0
             || cfdACounter > 1 || cfdACounter == 0 )
            continue;

        /* select interpolation type - obtain interpolant */
        if (inputData.m_interpolationType == DRS4InterpolationType::type::spline) {
            if (inputData.m_bUsingALGLIB) {
                switch (inputData.m_splineInterpolationType) {
                case DRS4SplineInterpolationType::type::cubic: {
                    alglib::spline1dbuildcubic(arrayDataX_A, arrayDataY_A, interpolantA);
                    alglib::spline1dbuildcubic(arrayDataX_B, arrayDataY_B, interpolantB);
                }
                    break;
                case DRS4SplineInterpolationType::type::akima: {
                    alglib::spline1dbuildakima(arrayDataX_A, arrayDataY_A, interpolantA);
                    alglib::spline1dbuildakima(arrayDataX_B, arrayDataY_B, interpolantB);
                }
                    break;
                case DRS4SplineInterpolationType::type::catmullRom: {
                    alglib::spline1dbuildcatmullrom(arrayDataX_A, arrayDataY_A, interpolantA);
                    alglib::spline1dbuildcatmullrom(arrayDataX_B, arrayDataY_B, interpolantB);
                }
                    break;
                case DRS4SplineInterpolationType::type::monotone: {
                    alglib::spline1dbuildmonotone(arrayDataX_A, arrayDataY_A, interpolantA);
                    alglib::spline1dbuildmonotone(arrayDataX_B, arrayDataY_B, interpolantB);
                }
                    break;
                default: {
                    alglib::spline1dbuildcubic(arrayDataX_A, arrayDataY_A, interpolantA);
                    alglib::spline1dbuildcubic(arrayDataX_B, arrayDataY_B, interpolantB);
                }
                    break;
                }
            }
            else if (inputData.m_bUsingTinoKluge) {
                tkSplineA.setType(SplineType::Cubic);
                tkSplineB.setType(SplineType::Cubic);

                tkSplineA.setPoints(arrayDataTKX_A, arrayDataTKY_A);
                tkSplineB.setPoints(arrayDataTKX_B, arrayDataTKY_B);
            }
            else if (inputData.m_bUsingLinearInterpol) {
                /* nothing yet */
            }
        }
        else {
            alglib::polynomialbuild(arrayDataX_A, arrayDataY_A, baryCentricInterpolantA);
            alglib::polynomialbuild(arrayDataX_B, arrayDataY_B, baryCentricInterpolantB);
        }

        /* calculate and normalize pulse area for subsequent area filtering */
        if (inputData.m_bPulseAreaPlot
                || inputData.m_bPulseAreaFilter) {
            const float rat = 5120*((float)inputData.m_cellWidth/((float)kNumberOfBins));

            areaA = areaA/(inputData.m_pulseAreaFilterNormA*rat);
            areaB = areaB/(inputData.m_pulseAreaFilterNormB*rat);
        }

        double timeAForYMax = -1;
        double timeBForYMax = -1;

        if (inputData.m_positiveSignal) {
            timeAForYMax = inputData.m_tChannel0[cellYAMax];
            timeBForYMax = inputData.m_tChannel1[cellYBMax];
        }
        else {
            timeAForYMax = inputData.m_tChannel0[cellYAMin];
            timeBForYMax = inputData.m_tChannel1[cellYBMin];
        }

        if (qFuzzyCompare(timeAForYMax, -1)
                || qFuzzyCompare(timeBForYMax, -1))
            continue;

        /* determine max/min more precisely */
        if (!inputData.m_bUsingLinearInterpol) {
            const int cell_interpolRangeA_start = inputData.m_positiveSignal?(cellYAMax-1):(cellYAMin-1);
            const int cell_interpolRangeA_stop = inputData.m_positiveSignal?(cellYAMax+1):(cellYAMin+1);

            const int cell_interpolRangeB_start = inputData.m_positiveSignal?(cellYBMax-1):(cellYBMin-1);
            const int cell_interpolRangeB_stop = inputData.m_positiveSignal?(cellYBMax+1):(cellYBMin+1);

            if ( !(cell_interpolRangeA_start >= 0 && cell_interpolRangeA_start < kNumberOfBins)
                 || !(cell_interpolRangeA_stop >= 0 && cell_interpolRangeA_stop < kNumberOfBins)
                 || !(cell_interpolRangeB_start >= 0 && cell_interpolRangeB_start < kNumberOfBins)
                 || !(cell_interpolRangeB_stop >= 0 && cell_interpolRangeB_stop < kNumberOfBins) )
                continue;

            const double renderIncrementA = (inputData.m_tChannel0[cell_interpolRangeA_stop] - inputData.m_tChannel0[cell_interpolRangeA_start])/((float)inputData.m_intraRenderPoints);
            const double renderIncrementB = (inputData.m_tChannel1[cell_interpolRangeB_stop] - inputData.m_tChannel1[cell_interpolRangeB_start])/((float)inputData.m_intraRenderPoints);

            for ( int i = 0 ; i <= inputData.m_intraRenderPoints ; ++ i ) {
                const double t_A = inputData.m_tChannel0[cell_interpolRangeA_start] + (float)i*renderIncrementA;
                const double t_B = inputData.m_tChannel1[cell_interpolRangeB_start] + (float)i*renderIncrementB;

                const float valYA = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantA, t_A)):(tkSplineA(t_A))):(alglib::barycentriccalc(baryCentricInterpolantA, t_A));
                const float valYB = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantB, t_B)):(tkSplineB(t_B))):(alglib::barycentriccalc(baryCentricInterpolantB, t_B));

                /* modify min/max to calculate the CF level in high accuracy, subsequently */
                if (valYA > yMaxA) {
                    yMaxA = valYA;

                    if (inputData.m_positiveSignal) {
                        timeAForYMax = t_A;
                    }
                }

                if (valYB > yMaxB) {
                    yMaxB = valYB;

                    if (inputData.m_positiveSignal) {
                        timeBForYMax = t_B;
                    }
                }

                if (valYA < yMinA) {
                    yMinA = valYA;

                    if (!inputData.m_positiveSignal) {
                        timeAForYMax = t_A;
                    }
                }

                if (valYB < yMinB) {
                    yMinB = valYB;

                    if (!inputData.m_positiveSignal) {
                        timeBForYMax = t_B;
                    }
                }
            }
        }

        /* add modified (interpolated/fitted) pulse height to PHS */
        float fractPHSA = 0.0f;
        float fractPHSB = 0.0f;

        const double fkNumberOfBins = (double)kNumberOfBins;

        if (inputData.m_positiveSignal) {
            fractPHSA = yMaxA*0.002;
            fractPHSB = yMaxB*0.002;
        }
        else {
            fractPHSA = abs(yMinA)*0.002;
            fractPHSB = abs(yMinB)*0.002;
        }

        /* add pulse heights to PHS */
        const int cellPHSA = ((int)(fractPHSA*fkNumberOfBins))-1;
        const int cellPHSB = ((int)(fractPHSB*fkNumberOfBins))-1;

        if ( cellPHSA < kNumberOfBins && cellPHSA >= 0 ) {
            outputData.m_phsA.append(cellPHSA);
        }

        if ( cellPHSB < kNumberOfBins && cellPHSB >= 0 ) {
            outputData.m_phsB.append(cellPHSB);
        }

        /* CF levels valid? */
        if (estimCFDACellStart == -1
                || estimCFDACellStop == -1
                || estimCFDBCellStart == -1
                || estimCFDBCellStop == -1)
            continue;

        if (inputData.m_positiveSignal) {
            if ( cfdValueA > 500.0f
                 || cfdValueB > 500.0f
                 || qFuzzyCompare(cfdValueA, 0.0f)
                 || qFuzzyCompare(cfdValueB, 0.0f)
                 || cfdValueA < 0.0f
                 || cfdValueB < 0.0f
                 || ((int)cfdValueA == (int)yMaxA)
                 || ((int)cfdValueB == (int)yMaxB))
                continue;
        }
        else {
            if ( cfdValueA < -500.0f
                 || cfdValueB < -500.0f
                 || qFuzzyCompare(cfdValueA, 0.0f)
                 || qFuzzyCompare(cfdValueB, 0.0f)
                 || cfdValueA > 0.0f
                 || cfdValueB > 0.0f
                 || ((int)cfdValueA == (int)yMinA)
                 || ((int)cfdValueB == (int)yMinB) )
                continue;
        }

        double timeStampA = -1.0f;
        double timeStampB = -1.0f;

        if (!inputData.m_bUsingLinearInterpol) {
            /* find correct CF level timestamp within the estimated CFD bracket index region */
            if ( estimCFDACellStart == estimCFDACellStop ) {
                timeStampA = inputData.m_tChannel0[estimCFDACellStart];
            }
            else {
                const double timeIncr = (inputData.m_tChannel0[estimCFDACellStop] - inputData.m_tChannel0[estimCFDACellStart])/((double)inputData.m_intraRenderPoints);
                for ( int i = 0 ; i < inputData.m_intraRenderPoints ; ++ i ) {
                    const double timeStamp1 = inputData.m_tChannel0[estimCFDACellStart] + (double)i*timeIncr;
                    const double timeStamp2 = inputData.m_tChannel0[estimCFDACellStart] + (double)(i+1)*timeIncr;

                    const float valY1 = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantA, timeStamp1)):(tkSplineA(timeStamp1))):(alglib::barycentriccalc(baryCentricInterpolantA, timeStamp1));
                    const float valY2 = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantA, timeStamp2)):(tkSplineA(timeStamp2))):(alglib::barycentriccalc(baryCentricInterpolantA, timeStamp2));

                    if ( (cfdValueA < valY1 && cfdValueA > valY2)
                         || (cfdValueA > valY1 && cfdValueA < valY2) ) {
                        const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                        const double intersect = valY1 - slope*timeStamp1;

                        timeStampA = (cfdValueA - intersect)/slope;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueA, valY1) ) {
                        timeStampA = timeStamp1;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueA, valY2) ) {
                        timeStampA = timeStamp2;

                        break;
                    }
                }
            }

            if ( estimCFDBCellStart == estimCFDBCellStop ) {
                timeStampB = inputData.m_tChannel1[estimCFDBCellStart];
            }
            else {
                const double timeIncr = (inputData.m_tChannel1[estimCFDBCellStop] - inputData.m_tChannel1[estimCFDBCellStart])/((double)inputData.m_intraRenderPoints);
                for ( int i = 0 ; i < inputData.m_intraRenderPoints ; ++ i ) {
                    const double timeStamp1 = inputData.m_tChannel1[estimCFDBCellStart] + (double)i*timeIncr;
                    const double timeStamp2 = inputData.m_tChannel1[estimCFDBCellStart] + (double)(i+1)*timeIncr;

                    const float valY1 = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantB, timeStamp1)):(tkSplineB(timeStamp1))):(alglib::barycentriccalc(baryCentricInterpolantB, timeStamp1));
                    const float valY2 = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantB, timeStamp2)):(tkSplineB(timeStamp2))):(alglib::barycentriccalc(baryCentricInterpolantB, timeStamp2));

                    if ( (cfdValueB < valY1 && cfdValueB > valY2)
                         || (cfdValueB > valY1 && cfdValueB < valY2) ) {
                        const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                        const double intersect = valY1 - slope*timeStamp1;

                        timeStampB = (cfdValueB - intersect)/slope;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueB, valY1) ) {
                        timeStampB = timeStamp1;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueB, valY2) ) {
                        timeStampB = timeStamp2;

                        break;
                    }
                }
            }
        }
        /* linear interpolation */
        else {
            if ( estimCFDACellStart == estimCFDACellStop ) {
                timeStampA = inputData.m_tChannel0[estimCFDACellStart];
            }
            else {
                const float timeStamp1 = inputData.m_tChannel0[estimCFDACellStart];
                const float timeStamp2 = inputData.m_tChannel0[estimCFDACellStop];

                const float valY1 = inputData.m_waveChannel0[estimCFDACellStart];
                const float valY2 = inputData.m_waveChannel0[estimCFDACellStop];

                if ( (cfdValueA < valY1 && cfdValueA > valY2)
                     || (cfdValueA > valY1 && cfdValueA < valY2) ) {
                    const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                    const double intersect = valY1 - slope*timeStamp1;

                    timeStampA = (cfdValueA - intersect)/slope;
                }
                else if ( qFuzzyCompare(cfdValueA, valY1) ) {
                    timeStampA = timeStamp1;
                }
                else if ( qFuzzyCompare(cfdValueA, valY2) ) {
                    timeStampA = timeStamp2;
                }
            }

            if ( estimCFDBCellStart == estimCFDBCellStop ) {
                timeStampB = inputData.m_tChannel1[estimCFDBCellStart];
            }
            else {
                const float timeStamp1 = inputData.m_tChannel1[estimCFDBCellStart];
                const float timeStamp2 = inputData.m_tChannel1[estimCFDBCellStop];

                const float valY1 = inputData.m_waveChannel1[estimCFDBCellStart];
                const float valY2 = inputData.m_waveChannel1[estimCFDBCellStop];

                if ( (cfdValueB < valY1 && cfdValueB > valY2)
                     || (cfdValueB > valY1 && cfdValueB < valY2) ) {
                    const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                    const double intersect = valY1 - slope*timeStamp1;

                    timeStampB = (cfdValueB - intersect)/slope;
                }
                else if ( qFuzzyCompare(cfdValueB, valY1) ) {
                    timeStampB = timeStamp1;
                }
                else if ( qFuzzyCompare(cfdValueB, valY2) ) {
                    timeStampB = timeStamp2;
                }
            }
        }

        double timeStampA_10perc = -1.0f;
        double timeStampB_10perc = -1.0f;

        if (!inputData.m_bUsingLinearInterpol) {
            /* finding correct CF (10%) timestamp within the estimated CF (10%) bracketed index region */
            if ( estimCFDACellStart_10perc == estimCFDACellStop_10perc ) {
                timeStampA_10perc = inputData.m_tChannel0[estimCFDACellStart_10perc];
            }
            else {
                const double timeIncr = (inputData.m_tChannel0[estimCFDACellStop_10perc] - inputData.m_tChannel0[estimCFDACellStart_10perc])/((double)inputData.m_intraRenderPoints);
                for ( int i = 0 ; i < inputData.m_intraRenderPoints ; ++ i ) {
                    const double timeStamp1 = inputData.m_tChannel0[estimCFDACellStart_10perc] + (double)i*timeIncr;
                    const double timeStamp2 = inputData.m_tChannel0[estimCFDACellStart_10perc] + (double)(i+1)*timeIncr;

                    const float valY1 = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantA, timeStamp1)):(tkSplineA(timeStamp1))):(alglib::barycentriccalc(baryCentricInterpolantA, timeStamp1));
                    const float valY2 = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantA, timeStamp2)):(tkSplineA(timeStamp2))):(alglib::barycentriccalc(baryCentricInterpolantA, timeStamp2));

                    if ( (cfdValueA_10perc < valY1 && cfdValueA_10perc > valY2)
                         || (cfdValueA_10perc > valY1 && cfdValueA_10perc < valY2) ) {
                        const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                        const double intersect = valY1 - slope*timeStamp1;

                        timeStampA_10perc = (cfdValueA_10perc - intersect)/slope;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueA_10perc, valY1) ) {
                        timeStampA_10perc = timeStamp1;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueA_10perc, valY2) ) {
                        timeStampA_10perc = timeStamp2;

                        break;
                    }
                }
            }

            if ( estimCFDBCellStart_10perc == estimCFDBCellStop_10perc ) {
                timeStampB_10perc = inputData.m_tChannel1[estimCFDBCellStart_10perc];
            }
            else {
                const double timeIncr = (inputData.m_tChannel1[estimCFDBCellStop_10perc] - inputData.m_tChannel1[estimCFDBCellStart_10perc])/((double)inputData.m_intraRenderPoints);
                for ( int i = 0 ; i < inputData.m_intraRenderPoints ; ++ i ) {
                    const double timeStamp1 = inputData.m_tChannel1[estimCFDBCellStart_10perc] + (double)i*timeIncr;
                    const double timeStamp2 = inputData.m_tChannel1[estimCFDBCellStart_10perc] + (double)(i+1)*timeIncr;

                    const float valY1 = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantB, timeStamp1)):(tkSplineB(timeStamp1))):(alglib::barycentriccalc(baryCentricInterpolantB, timeStamp1));
                    const float valY2 = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantB, timeStamp2)):(tkSplineB(timeStamp2))):(alglib::barycentriccalc(baryCentricInterpolantB, timeStamp2));

                    if ( (cfdValueB_10perc < valY1 && cfdValueB_10perc > valY2)
                         || (cfdValueB_10perc > valY1 && cfdValueB_10perc < valY2) ) {
                        const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                        const double intersect = valY1 - slope*timeStamp1;

                        timeStampB_10perc = (cfdValueB_10perc - intersect)/slope;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueB_10perc, valY1) ) {
                        timeStampB_10perc = timeStamp1;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueB_10perc, valY2) ) {
                        timeStampB_10perc = timeStamp2;

                        break;
                    }
                }
            }
        }
        /* linear interpolation */
        else {
            if ( estimCFDACellStart_10perc == estimCFDACellStop_10perc ) {
                timeStampA_10perc = inputData.m_tChannel0[estimCFDACellStart_10perc];
            }
            else {
                const float timeStamp1 = inputData.m_tChannel0[estimCFDACellStart_10perc];
                const float timeStamp2 = inputData.m_tChannel0[estimCFDACellStop_10perc];

                const float valY1 = inputData.m_waveChannel0[estimCFDACellStart_10perc];
                const float valY2 = inputData.m_waveChannel0[estimCFDACellStop_10perc];

                if ( (cfdValueA_10perc < valY1 && cfdValueA_10perc > valY2)
                     || (cfdValueA_10perc > valY1 && cfdValueA_10perc < valY2) ) {
                    const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                    const double intersect = valY1 - slope*timeStamp1;

                    timeStampA_10perc = (cfdValueA_10perc - intersect)/slope;
                }
                else if ( qFuzzyCompare(cfdValueA_10perc, valY1) ) {
                    timeStampA_10perc = timeStamp1;
                }
                else if ( qFuzzyCompare(cfdValueA_10perc, valY2) ) {
                    timeStampA_10perc = timeStamp2;
                }
            }

            if ( estimCFDBCellStart_10perc == estimCFDBCellStop_10perc ) {
                timeStampB_10perc = inputData.m_tChannel1[estimCFDBCellStart_10perc];
            }
            else {
                const float timeStamp1 = inputData.m_tChannel1[estimCFDBCellStart_10perc];
                const float timeStamp2 = inputData.m_tChannel1[estimCFDBCellStop_10perc];

                const float valY1 = inputData.m_waveChannel1[estimCFDBCellStart_10perc];
                const float valY2 = inputData.m_waveChannel1[estimCFDBCellStop_10perc];

                if ( (cfdValueB_10perc < valY1 && cfdValueB_10perc > valY2)
                     || (cfdValueB_10perc > valY1 && cfdValueB_10perc < valY2) ) {
                    const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                    const double intersect = valY1 - slope*timeStamp1;

                    timeStampB_10perc = (cfdValueB_10perc - intersect)/slope;
                }
                else if ( qFuzzyCompare(cfdValueB_10perc, valY1) ) {
                    timeStampB_10perc = timeStamp1;
                }
                else if ( qFuzzyCompare(cfdValueB_10perc, valY2) ) {
                    timeStampB_10perc = timeStamp2;
                }
            }
        }

        double timeStampA_90perc = -1.0f;
        double timeStampB_90perc = -1.0f;

        if (!inputData.m_bUsingLinearInterpol) {
            /* find correct CF level (90%) timestamp within the estimated CF (90%) bracketed index region */
            if ( estimCFDACellStart_90perc == estimCFDACellStop_90perc ) {
                timeStampA_90perc = inputData.m_tChannel0[estimCFDACellStart_90perc];
            }
            else {
                const double timeIncr = (inputData.m_tChannel0[estimCFDACellStop_90perc] - inputData.m_tChannel0[estimCFDACellStart_90perc])/((double)inputData.m_intraRenderPoints);
                for ( int i = 0 ; i < inputData.m_intraRenderPoints ; ++ i ) {
                    const double timeStamp1 = inputData.m_tChannel0[estimCFDACellStart_90perc] + (double)i*timeIncr;
                    const double timeStamp2 = inputData.m_tChannel0[estimCFDACellStart_90perc] + (double)(i+1)*timeIncr;

                    const float valY1 = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantA, timeStamp1)):(tkSplineA(timeStamp1))):(alglib::barycentriccalc(baryCentricInterpolantA, timeStamp1));
                    const float valY2 = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantA, timeStamp2)):(tkSplineA(timeStamp2))):(alglib::barycentriccalc(baryCentricInterpolantA, timeStamp2));

                    if ( (cfdValueA_90perc < valY1 && cfdValueA_90perc > valY2)
                         || (cfdValueA_90perc > valY1 && cfdValueA_90perc < valY2) ) {
                        const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                        const double intersect = valY1 - slope*timeStamp1;

                        timeStampA_90perc = (cfdValueA_90perc - intersect)/slope;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueA_90perc, valY1) ) {
                        timeStampA_90perc = timeStamp1;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueA_90perc, valY2) ) {
                        timeStampA_90perc = timeStamp2;

                        break;
                    }
                }
            }

            if ( estimCFDBCellStart_90perc == estimCFDBCellStop_90perc ) {
                timeStampB_90perc = inputData.m_tChannel1[estimCFDBCellStart_90perc];
            }
            else {
                const double timeIncr = (inputData.m_tChannel1[estimCFDBCellStop_90perc] - inputData.m_tChannel1[estimCFDBCellStart_90perc])/((double)inputData.m_intraRenderPoints);
                for ( int i = 0 ; i < inputData.m_intraRenderPoints ; ++ i ) {
                    const double timeStamp1 = inputData.m_tChannel1[estimCFDBCellStart_90perc] + (double)i*timeIncr;
                    const double timeStamp2 = inputData.m_tChannel1[estimCFDBCellStart_90perc] + (double)(i+1)*timeIncr;

                    const float valY1 = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantB, timeStamp1)):(tkSplineB(timeStamp1))):(alglib::barycentriccalc(baryCentricInterpolantB, timeStamp1));
                    const float valY2 = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantB, timeStamp2)):(tkSplineB(timeStamp2))):(alglib::barycentriccalc(baryCentricInterpolantB, timeStamp2));

                    if ( (cfdValueB_90perc < valY1 && cfdValueB_90perc > valY2)
                         || (cfdValueB_90perc > valY1 && cfdValueB_90perc < valY2) ) {
                        const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                        const double intersect = valY1 - slope*timeStamp1;

                        timeStampB_90perc = (cfdValueB_90perc - intersect)/slope;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueB_90perc, valY1) ) {
                        timeStampB_90perc = timeStamp1;

                        break;
                    }
                    else if ( qFuzzyCompare(cfdValueB_90perc, valY2) ) {
                        timeStampB_90perc = timeStamp2;

                        break;
                    }
                }
            }
        }
        /* linear interpolation */
        else {
            if ( estimCFDACellStart_90perc == estimCFDACellStop_90perc ) {
                timeStampA_90perc = inputData.m_tChannel0[estimCFDACellStart_90perc];
            }
            else {
                const float timeStamp1 = inputData.m_tChannel0[estimCFDACellStart_90perc];
                const float timeStamp2 = inputData.m_tChannel0[estimCFDACellStop_90perc];

                const float valY1 = inputData.m_waveChannel0[estimCFDACellStart_90perc];
                const float valY2 = inputData.m_waveChannel0[estimCFDACellStop_90perc];

                if ( (cfdValueA_90perc < valY1 && cfdValueA_90perc > valY2)
                     || (cfdValueA_90perc > valY1 && cfdValueA_90perc < valY2) ) {
                    const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                    const double intersect = valY1 - slope*timeStamp1;

                    timeStampA_90perc = (cfdValueA_90perc - intersect)/slope;
                }
                else if ( qFuzzyCompare(cfdValueA_90perc, valY1) ) {
                    timeStampA_90perc = timeStamp1;
                }
                else if ( qFuzzyCompare(cfdValueA_90perc, valY2) ) {
                    timeStampA_90perc = timeStamp2;
                }
            }

            if ( estimCFDBCellStart_90perc == estimCFDBCellStop_90perc ) {
                timeStampB_90perc = inputData.m_tChannel1[estimCFDBCellStart_90perc];
            }
            else {
                const float timeStamp1 = inputData.m_tChannel1[estimCFDBCellStart_90perc];
                const float timeStamp2 = inputData.m_tChannel1[estimCFDBCellStop_90perc];

                const float valY1 = inputData.m_waveChannel1[estimCFDBCellStart_90perc];
                const float valY2 = inputData.m_waveChannel1[estimCFDBCellStop_90perc];

                if ( (cfdValueB_90perc < valY1 && cfdValueB_90perc > valY2)
                     || (cfdValueB_90perc > valY1 && cfdValueB_90perc < valY2) ) {
                    const double slope = (valY2 - valY1)/(timeStamp2 - timeStamp1);
                    const double intersect = valY1 - slope*timeStamp1;

                    timeStampB_90perc = (cfdValueB_90perc - intersect)/slope;
                }
                else if ( qFuzzyCompare(cfdValueB_90perc, valY1) ) {
                    timeStampB_90perc = timeStamp1;
                }
                else if ( qFuzzyCompare(cfdValueB_90perc, valY2) ) {
                    timeStampB_90perc = timeStamp2;
                }
            }
        }

        if ((int)timeStampA == -1
                || (int)timeStampB == -1)
            continue;

        /* area-Filter */
        if (!inputData.m_bBurstMode
                && inputData.m_bPulseAreaPlot) {
            const int binA = areaA*(double)inputData.m_pulseAreaFilterBinningA;
            const int binB = areaB*(double)inputData.m_pulseAreaFilterBinningB;

            const int reducedNumberOfBins = (kNumberOfBins-1);

            if (!(cellPHSA>reducedNumberOfBins
                  || cellPHSB>reducedNumberOfBins
                  || cellPHSA<0
                  || cellPHSB<0
                  || binA<0
                  || binB<0)) {
                outputData.m_areaFilterDataA.append(QPointF(cellPHSA, binA));
                outputData.m_areaFilterDataB.append(QPointF(cellPHSB, binB));
            }
        }

        /* determine start and stop branches */
        bool bIsStart_A = false;
        bool bIsStop_A = false;

        bool bIsStart_B = false;
        bool bIsStop_B = false;

        if ( cellPHSA >= inputData.m_startAMinPHS
             && cellPHSA <= inputData.m_startAMaxPHS )
            bIsStart_A = true;

        if ( cellPHSA >= inputData.m_stopAMinPHS
             && cellPHSA <= inputData.m_stopAMaxPHS )
            bIsStop_A = true;

        if ( cellPHSB >= inputData.m_startBMinPHS
             && cellPHSB <= inputData.m_startBMaxPHS )
            bIsStart_B = true;

        if ( cellPHSB >= inputData.m_stopBMinPHS
             && cellPHSB <= inputData.m_stopBMaxPHS )
            bIsStop_B = true;

        /* rise-time Filter */
        if ((int)timeStampA_10perc != -1
                || (int)timeStampA_90perc != -1) {
            const int binA = (int)((double)inputData.m_riseTimeFilterBinningA*(timeStampA_90perc-timeStampA_10perc)/inputData.m_riseTimeFilterARangeInNanoseconds);

            if ( !(binA < 0 || binA >= inputData.m_riseTimeFilterBinningA) ) {
                if (bIsStart_A || bIsStop_A) {
                    outputData.m_riseTimeFilterDataA.append(binA);
                }
            }
        }

        if ((int)timeStampB_10perc != -1
                || (int)timeStampB_90perc != -1) {
            const int binB = (int)((double)inputData.m_riseTimeFilterBinningB*(timeStampB_90perc-timeStampB_10perc)/inputData.m_riseTimeFilterBRangeInNanoseconds);

            if ( !(binB < 0 || binB >= inputData.m_riseTimeFilterBinningB) ) {
                if (bIsStart_B || bIsStop_B) {
                    outputData.m_riseTimeFilterDataB.append(binB);
                }
            }
        }

        /* apply area-filter and reject pulses if one of both appears outside the windows */
        if (inputData.m_bPulseAreaFilter) {
            const double indexPHSA = cellPHSA;
            const double indexPHSB = cellPHSB;

            const double yLowerA = (inputData.m_areaFilterASlopeLower*indexPHSA+inputData.m_areaFilterAInterceptLower);
            const double yUpperA = (inputData.m_areaFilterASlopeUpper*indexPHSA+inputData.m_areaFilterAInterceptUpper);

            const double multA = areaA*inputData.m_pulseAreaFilterBinningA;
            const double multB = areaB*inputData.m_pulseAreaFilterBinningB;

            const bool y_AInside = (multA >= yLowerA && multA <=yUpperA);

            const double yLowerB = (inputData.m_areaFilterBSlopeLower*indexPHSB+inputData.m_areaFilterBInterceptLower);
            const double yUpperB = (inputData.m_areaFilterBSlopeUpper*indexPHSB+inputData.m_areaFilterBInterceptUpper);

            const bool y_BInside = (multB >= yLowerB && multB <= yUpperB);

            if (y_AInside) {
                /* incremental (mean ; stddev) */
                outputData.m_areaFilterCollectionDataA.append(QPointF(indexPHSA, areaA));
            }

            if (y_BInside) {
                /* incremental (mean ; stddev) */
                outputData.m_areaFilterCollectionDataB.append(QPointF(indexPHSB, areaB));
            }

            if ( !y_AInside || !y_BInside )
                continue;
        }

        /* apply rise time-filter and reject pulses if one of both appears outside the windows */
        if (inputData.m_bPulseRiseTimeFilter) {
            const int binA = (int)((double)inputData.m_riseTimeFilterBinningA*(timeStampA_90perc-timeStampA_10perc)/inputData.m_riseTimeFilterARangeInNanoseconds);
            const int binB = (int)((double)inputData.m_riseTimeFilterBinningB*(timeStampB_90perc-timeStampB_10perc)/inputData.m_riseTimeFilterBRangeInNanoseconds);

            bool bAcceptedA = false;
            bool bAcceptedB = false;

            if (binA >= inputData.m_riseTimeFilterLeftWindowA && binA <= inputData.m_riseTimeFilterRightWindowA)
                bAcceptedA = true;

            if (binB >= inputData.m_riseTimeFilterLeftWindowB && binB <= inputData.m_riseTimeFilterRightWindowB)
                bAcceptedB = true;

            if (!bAcceptedA || !bAcceptedB)
                continue;
        }

        /* apply pulse-shape filter */
        if (inputData.m_pulseShapeFilterEnabledA
             || inputData.m_pulseShapeFilterEnabledB) {
            bool bRejectA = false;
            bool bRejectB = false;

            const float fractYMaxA = 1.0f/yMaxA;
            const float fractYMaxB = 1.0f/yMaxB;

            const float fractYMinA = 1.0f/yMinA;
            const float fractYMinB = 1.0f/yMinB;

            const int size = kNumberOfBins;

            DSpline splineMeanA, splineMeanB;
            DSpline splineStdDevA, splineStdDevB;

            if (inputData.m_pulseShapeFilterEnabledA) {
                splineMeanA.setType(SplineType::Cubic);
                splineMeanA.setPointsArray(inputData.m_pulseShapeFilterDataMeanTraceA_X, inputData.m_pulseShapeFilterDataMeanTraceA_Y, __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER);

                splineStdDevA.setType(SplineType::Cubic);
                splineStdDevA.setPointsArray(inputData.m_pulseShapeFilterDataStdDevTraceA_X, inputData.m_pulseShapeFilterDataStdDevTraceA_Y, __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER);
            }

            if (inputData.m_pulseShapeFilterEnabledB) {
                splineMeanB.setType(SplineType::Cubic);
                splineMeanB.setPointsArray(inputData.m_pulseShapeFilterDataMeanTraceB_X, inputData.m_pulseShapeFilterDataMeanTraceB_Y, __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER);

                splineStdDevB.setType(SplineType::Cubic);
                splineStdDevB.setPointsArray(inputData.m_pulseShapeFilterDataStdDevTraceB_X, inputData.m_pulseShapeFilterDataStdDevTraceB_Y, __PULSESHAPEFILTER_SPLINE_TRACE_NUMBER);
            }

            for ( int j = 0 ; j < size ; ++ j ) {
                const double tA = (inputData.m_tChannel0[j] - timeAForYMax);
                const double tB = (inputData.m_tChannel1[j] - timeBForYMax);

                const bool bROIA = (tA >= -inputData.m_pulseShapeFilterLeftInNsROIA) && (tA <= inputData.m_pulseShapeFilterRightInNsROIA);
                const bool bROIB = (tB >= -inputData.m_pulseShapeFilterLeftInNsROIB) && (tB <= inputData.m_pulseShapeFilterRightInNsROIB);

                if (bROIA && inputData.m_pulseShapeFilterEnabledA) {
                    const float yA = inputData.m_positiveSignal?(inputData.m_waveChannel0[j]*fractYMaxA):(inputData.m_waveChannel0[j]*fractYMinA);

                    const double mean = splineMeanA(tA);
                    const double val = splineStdDevA(tA);

                    const double stddevUpper = inputData.m_pulseShapeFilterFractOfStdDevUpperA*val;
                    const double stddevLower = inputData.m_pulseShapeFilterFractOfStdDevLowerA*val;

                    const double upperLimit = (mean + stddevUpper);
                    const double lowerLimit = (mean - stddevLower);

                    const bool inside = (yA <= upperLimit) && (yA >= lowerLimit);

                    if (!inside)
                        bRejectA = true;
                }

                if (inputData.m_pulseShapeFilterEnabledA && bRejectA)
                    break;

                if (bROIB && inputData.m_pulseShapeFilterEnabledB) {
                    const float yB = inputData.m_positiveSignal?(inputData.m_waveChannel1[j]*fractYMaxB):(inputData.m_waveChannel1[j]*fractYMinB);

                    const double mean = splineMeanB(tB);
                    const double val = splineStdDevB(tB);

                    const double stddevUpper = inputData.m_pulseShapeFilterFractOfStdDevUpperB*val;
                    const double stddevLower = inputData.m_pulseShapeFilterFractOfStdDevLowerB*val;

                    const double upperLimit = (mean + stddevUpper);
                    const double lowerLimit = (mean - stddevLower);

                    const bool inside = (yB <= upperLimit) && (yB >= lowerLimit);

                    if (!inside)
                        bRejectB = true;
                }

                if (inputData.m_pulseShapeFilterEnabledB && bRejectB)
                    break;
            }

            if (bRejectA || bRejectB)
                continue;
        }

        bool bValidLifetime = false;
        bool bValidLifetime2 = false;

        /* lifetime: A-B - master */
        if ( bIsStart_A
             && bIsStop_B && !inputData.m_bForcePrompt  ) {
            const double ltdiff = (timeStampB - timeStampA);
            const int binAB = ((int)round(((((ltdiff)+inputData.m_offsetAB)/inputData.m_scalerAB))*((double)inputData.m_channelCntAB)))-1;

            const int binMerged = ((int)round(((((ltdiff+inputData.m_ATS)+inputData.m_offsetMerged)/inputData.m_scalerMerged))*((double)inputData.m_channelCntMerged)))-1;

            if ( binAB < 0 || binAB >= inputData.m_channelCntAB )
                continue;

            if ( binAB >= 0
                 && binAB < inputData.m_channelCntAB ) {
                if ( inputData.m_bNegativeLT && ltdiff < 0  )
                    outputData.m_lifeTimeDataAB.append(binAB);
                else if ( ltdiff >= 0 )
                    outputData.m_lifeTimeDataAB.append(binAB);

                if (inputData.m_rcScheme == DRS4PulseShapeFilterRecordScheme::Scheme::RC_AB)
                    bValidLifetime = true;

                bValidLifetime2 = true;
            }

            /* pulse-shape filter: record */
            if (inputData.m_pulseShapeFilterAIsRecording && bValidLifetime) {
                const float fractYMaxA = 1.0f/yMaxA;
                const float fractYMinA = 1.0f/yMinA;

                DSpline splineA;
                splineA.setType(SplineType::Cubic);
                std::vector<double> xSplineA, ySplineA;

                const int size = kNumberOfBins;

                QVector<QPointF> pVecA;

                for ( int j = 0 ; j < size ; ++ j ) {
                    const double tA = (inputData.m_tChannel0[j] - timeAForYMax);
                    const bool bROIA = (tA >= __PULSESHAPEFILTER_LEFT_MAX) && (tA <= __PULSESHAPEFILTER_RIGHT_MAX);

                    if (bROIA) {
                        const float yA = inputData.m_positiveSignal?(inputData.m_waveChannel0[j]*fractYMaxA):(inputData.m_waveChannel0[j]*fractYMinA);

                        pVecA.append(QPointF(tA, yA));

                        xSplineA.push_back(tA);
                        ySplineA.push_back(yA);
                    }
                }

                if (!pVecA.isEmpty()) {
                    outputData.m_pulseShapeDataA.append(pVecA);

                    splineA.setPoints(xSplineA, ySplineA);
                    outputData.m_pulseShapeDataSplineA.append(splineA);
                }
            }

            if (inputData.m_pulseShapeFilterBIsRecording && bValidLifetime) {
                const float fractYMinB = 1.0f/yMinB;
                const float fractYMaxB = 1.0f/yMaxB;

                DSpline splineB;
                splineB.setType(SplineType::Cubic);
                std::vector<double> xSplineB, ySplineB;

                const int size = kNumberOfBins;

                QVector<QPointF> pVecB;

                for ( int j = 0 ; j < size ; ++ j ) {
                    const double tB = (inputData.m_tChannel1[j] - timeBForYMax);
                    const bool bROIB = (tB >= __PULSESHAPEFILTER_LEFT_MAX) && (tB <= __PULSESHAPEFILTER_RIGHT_MAX);

                    if (bROIB) {
                        const float yB = inputData.m_positiveSignal?(inputData.m_waveChannel1[j]*fractYMaxB):(inputData.m_waveChannel1[j]*fractYMinB);

                        pVecB.append(QPointF(tB, yB));

                        xSplineB.push_back(tB);
                        ySplineB.push_back(yB);
                    }
                }

                if (!pVecB.isEmpty()) {
                    outputData.m_pulseShapeDataB.append(pVecB);

                    splineB.setPoints(xSplineB, ySplineB);
                    outputData.m_pulseShapeDataSplineB.append(splineB);
                }
            }

            if ( binMerged < 0 || binMerged >= inputData.m_channelCntMerged )
                continue;

            if ( binMerged >= 0
                 && binMerged < inputData.m_channelCntMerged ) {
                if ( inputData.m_bNegativeLT && ltdiff < 0  )
                    outputData.m_lifeTimeDataMerged.append(binMerged);
                else if ( ltdiff >= 0 )
                    outputData.m_lifeTimeDataMerged.append(binMerged);
            }
        }
        /* lifetime: B-A - master */
        else if ( bIsStart_B
                  && bIsStop_A && !inputData.m_bForcePrompt ) {
            const double ltdiff = (timeStampA - timeStampB);
            const int binBA = ((int)round(((((ltdiff)+inputData.m_offsetBA)/inputData.m_scalerBA))*((double)inputData.m_channelCntBA)))-1;

            const int binMerged = ((int)round(((((ltdiff-inputData.m_ATS)+inputData.m_offsetMerged)/inputData.m_scalerMerged))*((double)inputData.m_channelCntMerged)))-1;

            if ( binBA < 0 || binBA >= inputData.m_channelCntBA )
                continue;

            if ( binBA >= 0
                 && binBA < inputData.m_channelCntBA ) {
                if ( inputData.m_bNegativeLT && ltdiff < 0 )
                    outputData.m_lifeTimeDataBA.append(binBA);
                else if ( ltdiff >= 0 )
                    outputData.m_lifeTimeDataBA.append(binBA);

                if (inputData.m_rcScheme == DRS4PulseShapeFilterRecordScheme::Scheme::RC_BA)
                    bValidLifetime = true;

                bValidLifetime2 = true;
            }

            /* pulse-shape filter: record */
            if (inputData.m_pulseShapeFilterAIsRecording && bValidLifetime) {
                const float fractYMaxA = 1.0f/yMaxA;
                const float fractYMinA = 1.0f/yMinA;

                DSpline splineA;
                splineA.setType(SplineType::Cubic);
                std::vector<double> xSplineA, ySplineA;

                const int size = kNumberOfBins;

                QVector<QPointF> pVecA;

                for ( int j = 0 ; j < size ; ++ j ) {
                    const double tA = (inputData.m_tChannel0[j] - timeAForYMax);
                    const bool bROIA = (tA >= __PULSESHAPEFILTER_LEFT_MAX) && (tA <= __PULSESHAPEFILTER_RIGHT_MAX);

                    if (bROIA) {
                        const float yA = inputData.m_positiveSignal?(inputData.m_waveChannel0[j]*fractYMaxA):(inputData.m_waveChannel0[j]*fractYMinA);

                        pVecA.append(QPointF(tA, yA));

                        xSplineA.push_back(tA);
                        ySplineA.push_back(yA);
                    }
                }

                if (!pVecA.isEmpty()) {
                    outputData.m_pulseShapeDataA.append(pVecA);

                    splineA.setPoints(xSplineA, ySplineA);
                    outputData.m_pulseShapeDataSplineA.append(splineA);
                }
            }

            if (inputData.m_pulseShapeFilterBIsRecording && bValidLifetime) {
                const float fractYMinB = 1.0f/yMinB;
                const float fractYMaxB = 1.0f/yMaxB;

                DSpline splineB;
                splineB.setType(SplineType::Cubic);
                std::vector<double> xSplineB, ySplineB;

                const int size = kNumberOfBins;

                QVector<QPointF> pVecB;

                for ( int j = 0 ; j < size ; ++ j ) {
                    const double tB = (inputData.m_tChannel1[j] - timeBForYMax);
                    const bool bROIB = (tB >= __PULSESHAPEFILTER_LEFT_MAX) && (tB <= __PULSESHAPEFILTER_RIGHT_MAX);

                    if (bROIB) {
                        const float yB = inputData.m_positiveSignal?(inputData.m_waveChannel1[j]*fractYMaxB):(inputData.m_waveChannel1[j]*fractYMinB);

                        pVecB.append(QPointF(tB, yB));

                        xSplineB.push_back(tB);
                        ySplineB.push_back(yB);
                    }
                }

                if (!pVecB.isEmpty()) {
                    outputData.m_pulseShapeDataB.append(pVecB);

                    splineB.setPoints(xSplineB, ySplineB);
                    outputData.m_pulseShapeDataSplineB.append(splineB);
                }
            }

            if ( binMerged < 0 || binMerged >= inputData.m_channelCntMerged )
                continue;

            if ( binMerged >= 0
                 && binMerged < inputData.m_channelCntMerged ) {
                if ( inputData.m_bNegativeLT && ltdiff < 0  )
                    outputData.m_lifeTimeDataMerged.append(binMerged);
                else if ( ltdiff >= 0 )
                    outputData.m_lifeTimeDataMerged.append(binMerged);
            }
        }
        /* prompt spectrum: A-B of stop - slave */
        else if (  bIsStop_B && bIsStop_A ) {
            const double ltdiff = (timeStampA - timeStampB);
            const int binBA = ((int)round(((((ltdiff)+inputData.m_offsetPrompt)/inputData.m_scalerPrompt))*((double)inputData.m_channelCntPrompt)))-1;

            if ( binBA < 0 || binBA >= inputData.m_channelCntPrompt )
                continue;

            if ( binBA >= 0
                 && binBA < inputData.m_channelCntPrompt ) {
                outputData.m_lifeTimeDataCoincidence.append(binBA);

                if (inputData.m_rcScheme == DRS4PulseShapeFilterRecordScheme::Scheme::RC_Prompt)
                    bValidLifetime = true;

                bValidLifetime2 = true;
            }

            /* pulse-shape filter: record */
            if (inputData.m_pulseShapeFilterAIsRecording && bValidLifetime) {
                const float fractYMaxA = 1.0f/yMaxA;
                const float fractYMinA = 1.0f/yMinA;

                DSpline splineA;
                splineA.setType(SplineType::Cubic);
                std::vector<double> xSplineA, ySplineA;

                const int size = kNumberOfBins;

                QVector<QPointF> pVecA;

                for ( int j = 0 ; j < size ; ++ j ) {
                    const double tA = (inputData.m_tChannel0[j] - timeAForYMax);
                    const bool bROIA = (tA >= __PULSESHAPEFILTER_LEFT_MAX) && (tA <= __PULSESHAPEFILTER_RIGHT_MAX);

                    if (bROIA) {
                        const float yA = inputData.m_positiveSignal?(inputData.m_waveChannel0[j]*fractYMaxA):(inputData.m_waveChannel0[j]*fractYMinA);

                        pVecA.append(QPointF(tA, yA));

                        xSplineA.push_back(tA);
                        ySplineA.push_back(yA);
                    }
                }

                if (!pVecA.isEmpty()) {
                    outputData.m_pulseShapeDataA.append(pVecA);

                    splineA.setPoints(xSplineA, ySplineA);
                    outputData.m_pulseShapeDataSplineA.append(splineA);
                }
            }

            if (inputData.m_pulseShapeFilterBIsRecording && bValidLifetime) {
                const float fractYMinB = 1.0f/yMinB;
                const float fractYMaxB = 1.0f/yMaxB;

                DSpline splineB;
                splineB.setType(SplineType::Cubic);
                std::vector<double> xSplineB, ySplineB;

                const int size = kNumberOfBins;

                QVector<QPointF> pVecB;

                for ( int j = 0 ; j < size ; ++ j ) {
                    const double tB = (inputData.m_tChannel1[j] - timeBForYMax);
                    const bool bROIB = (tB >= __PULSESHAPEFILTER_LEFT_MAX) && (tB <= __PULSESHAPEFILTER_RIGHT_MAX);

                    if (bROIB) {
                        const float yB = inputData.m_positiveSignal?(inputData.m_waveChannel1[j]*fractYMaxB):(inputData.m_waveChannel1[j]*fractYMinB);

                        pVecB.append(QPointF(tB, yB));

                        xSplineB.push_back(tB);
                        ySplineB.push_back(yB);
                    }
                }

                if (!pVecB.isEmpty()) {
                    outputData.m_pulseShapeDataB.append(pVecB);

                    splineB.setPoints(xSplineB, ySplineB);
                    outputData.m_pulseShapeDataSplineB.append(splineB);
                }
            }
        }
    }

    return outputData;
}

bool DRS4WorkerConcurrentManager::add(const QVector<DRS4ConcurrentCopyInputData> &copyData)
{
    if (copyData.isEmpty())
        return false;

    m_copyData.append(copyData);

    if (!m_future.isRunning() && m_future.isFinished()) {
        merge();
        start();
    }

    return true;
}

void DRS4WorkerConcurrentManager::start()
{
    m_copyMetaData.clear();
    m_copyMetaData.swap(m_copyData);

    m_future = QtConcurrent::mapped(m_copyMetaData, runCalculation);
}

void DRS4WorkerConcurrentManager::cancel()
{
    if (m_future.isRunning() ) {
        m_future.cancel();
        m_future.waitForFinished();
    }
}

void DRS4WorkerConcurrentManager::merge()
{
    if (m_future.isRunning()
            && !m_future.isFinished())
        return;

    for ( DRS4ConcurrentCopyOutputData outputData : m_future.results()  ) {
        if (outputData.rejectData())
            continue;

        /* PHS */
        for ( int index : outputData.m_phsA )
            m_worker->m_phsA[index] ++;

        m_worker->m_phsACounts += outputData.m_phsA.size();

        for ( int index : outputData.m_phsB )
            m_worker->m_phsB[index] ++;

        m_worker->m_phsBCounts += outputData.m_phsB.size();

        /* Lifetime-Spectrum */
        for ( int index : outputData.m_lifeTimeDataAB ) {
            m_worker->m_lifeTimeDataAB[index] ++;
            m_worker->m_maxY_ABSpectrum = qMax(m_worker->m_maxY_ABSpectrum, m_worker->m_lifeTimeDataAB[index]);
        }

        m_worker->m_abCounts += outputData.m_lifeTimeDataAB.size();

        for ( int index : outputData.m_lifeTimeDataBA ) {
            m_worker->m_lifeTimeDataBA[index] ++;
            m_worker->m_maxY_BASpectrum = qMax(m_worker->m_maxY_BASpectrum, m_worker->m_lifeTimeDataBA[index]);
        }

        m_worker->m_baCounts += outputData.m_lifeTimeDataBA.size();

        for ( int index : outputData.m_lifeTimeDataCoincidence ) {
            m_worker->m_lifeTimeDataCoincidence[index] ++;
            m_worker->m_maxY_CoincidenceSpectrum = qMax(m_worker->m_maxY_CoincidenceSpectrum, m_worker->m_lifeTimeDataCoincidence[index]);
        }

        m_worker->m_coincidenceCounts += outputData.m_lifeTimeDataCoincidence.size();

        for ( int index : outputData.m_lifeTimeDataMerged ) {
            m_worker->m_lifeTimeDataMerged[index] ++;
            m_worker->m_maxY_MergedSpectrum = qMax(m_worker->m_maxY_MergedSpectrum, m_worker->m_lifeTimeDataMerged[index]);
        }

        m_worker->m_mergedCounts += outputData.m_lifeTimeDataMerged.size();

        /* Statistics */
        m_worker->m_specABCounterCnt += outputData.m_lifeTimeDataAB.size();
        m_worker->m_specBACounterCnt += outputData.m_lifeTimeDataBA.size();
        m_worker->m_specCoincidenceCounterCnt += outputData.m_lifeTimeDataCoincidence.size();
        m_worker->m_specMergedCounterCnt += outputData.m_lifeTimeDataMerged.size();

        /* Area - Filter */
        int index = 0;
        for ( QPointF p : outputData.m_areaFilterDataA ) {
            if (m_worker->m_areaFilterACounter >= 5000 )
                m_worker->m_areaFilterACounter = 0;

            if (m_worker->m_areaFilterBCounter >= 5000 )
                m_worker->m_areaFilterBCounter = 0;

            m_worker->m_areaFilterDataA[m_worker->m_areaFilterACounter] = p;
            m_worker->m_areaFilterDataB[m_worker->m_areaFilterBCounter] = outputData.m_areaFilterDataB.at(index);

            m_worker->m_areaFilterACounter ++;
            m_worker->m_areaFilterBCounter ++;

            index ++;
        }

        for ( int i = 0 ; i < outputData.m_areaFilterCollectionDataA.size() ; ++ i ) {
            const QPointF p = outputData.m_areaFilterCollectionDataA[i];

            const int indexA = (int)p.x();
            const double areaA = p.y();

            /* incremental (mean ; stddev) */
            double meanA  = m_worker->m_areaFilterCollectedDataA[indexA].x();
            double stddevA = m_worker->m_areaFilterCollectedDataA[indexA].y();

            m_worker->m_areaFilterCollectedDataCounterA[indexA] ++;

            if (m_worker->m_areaFilterCollectedDataCounterA[indexA] >= 2) {
                stddevA = ((m_worker->m_areaFilterCollectedDataCounterA[indexA]-2)/(m_worker->m_areaFilterCollectedDataCounterA[indexA]-1))*stddevA*stddevA + (1.0/m_worker->m_areaFilterCollectedDataCounterA[indexA])*(areaA-meanA)*(areaA-meanA);
                stddevA = sqrt(stddevA);
            }
            else
                stddevA = 0.0;

            meanA = (1/(float)m_worker->m_areaFilterCollectedDataCounterA[indexA])*(areaA + float(m_worker->m_areaFilterCollectedDataCounterA[indexA] - 1)*meanA);


            m_worker->m_areaFilterCollectedDataA[indexA].setX(meanA);
            m_worker->m_areaFilterCollectedDataA[indexA].setY(stddevA);

            m_worker->m_areaFilterCollectedACounter ++;
        }

        for ( int i = 0 ; i < outputData.m_areaFilterCollectionDataB.size() ; ++ i ) {
            const QPointF p = outputData.m_areaFilterCollectionDataB[i];

            const int indexB = (int)p.x();
            const double areaB = p.y();

            /* incremental (mean ; stddev) */
            double meanB  = m_worker->m_areaFilterCollectedDataB[indexB].x();
            double stddevB = m_worker->m_areaFilterCollectedDataB[indexB].y();

            m_worker->m_areaFilterCollectedDataCounterB[indexB] ++;

            if (m_worker->m_areaFilterCollectedDataCounterB[indexB] >= 2) {
                stddevB = ((m_worker->m_areaFilterCollectedDataCounterB[indexB]-2)/(m_worker->m_areaFilterCollectedDataCounterB[indexB]-1))*stddevB*stddevB + (1.0/m_worker->m_areaFilterCollectedDataCounterB[indexB])*(areaB-meanB)*(areaB-meanB);
                stddevB = sqrt(stddevB);
            }
            else
                stddevB = 0.0;

            meanB = (1/(float)m_worker->m_areaFilterCollectedDataCounterB[indexB])*(areaB + float(m_worker->m_areaFilterCollectedDataCounterB[indexB] - 1)*meanB);


            m_worker->m_areaFilterCollectedDataB[indexB].setX(meanB);
            m_worker->m_areaFilterCollectedDataB[indexB].setY(stddevB);

            m_worker->m_areaFilterCollectedBCounter ++;
        }

        /* Rise - Time Filter */
        for ( int i = 0 ; i < outputData.m_riseTimeFilterDataA.size() ; ++ i ) {
            m_worker->m_riseTimeFilterDataA[outputData.m_riseTimeFilterDataA[i]] ++;

            m_worker->m_maxY_RiseTimeSpectrumA = qMax(m_worker->m_maxY_RiseTimeSpectrumA, m_worker->m_riseTimeFilterDataA[outputData.m_riseTimeFilterDataA[i]]);
        }

        m_worker->m_riseTimeFilterACounter += outputData.m_riseTimeFilterDataA.size();

        for ( int i = 0 ; i < outputData.m_riseTimeFilterDataB.size() ; ++ i ) {
            m_worker->m_riseTimeFilterDataB[outputData.m_riseTimeFilterDataB[i]] ++;

            m_worker->m_maxY_RiseTimeSpectrumB = qMax(m_worker->m_maxY_RiseTimeSpectrumB, m_worker->m_riseTimeFilterDataB[outputData.m_riseTimeFilterDataB[i]]);
        }

        m_worker->m_riseTimeFilterBCounter += outputData.m_riseTimeFilterDataB.size();

        /* Pulse - Shape Filter (during record) */
        if (m_worker->m_isRecordingForShapeFilterA
                || m_worker->m_isRecordingForShapeFilterB) {
            const int sizeA = outputData.m_pulseShapeDataA.size();
            const int sizeB = outputData.m_pulseShapeDataB.size();

            if (m_worker->m_isRecordingForShapeFilterA && sizeA > 0) {
                for (int i = 0 ; i < sizeA ; ++ i) {
                    if (m_worker->m_isRecordingForShapeFilterA) {
                        m_worker->m_pulseShapeDataSplineA.append(outputData.m_pulseShapeDataSplineA.at(i));

                        const int size = outputData.m_pulseShapeDataA.at(i).size();

                        bool bdirA = true;

                        if (!(m_worker->m_pulseShapeDataACounter%2))
                            bdirA = true;
                        else
                            bdirA = false;

                        if (bdirA) {
                            for ( int j = 0 ; j < size ; ++ j ) {
                                m_worker->m_pulseShapeDataA.append(outputData.m_pulseShapeDataA.at(i).at(j));
                            }
                        }
                        else {
                            for ( int j = size-1 ; j >= 0 ; -- j ) {
                                m_worker->m_pulseShapeDataA.append(outputData.m_pulseShapeDataA.at(i).at(j));
                            }
                        }

                        m_worker->m_pulseShapeDataACounter ++;


                        if (m_worker->m_pulseShapeDataACounter == (m_worker->m_pulseShapeDataAmountA + 1))
                            m_worker->m_isRecordingForShapeFilterA = false;
                    }
                    else {
                        break;
                    }
                }
            }

            if (m_worker->m_isRecordingForShapeFilterB && sizeB > 0) {
                for (int i = 0 ; i < sizeB ; ++ i) {
                    if (m_worker->m_isRecordingForShapeFilterB) {
                        m_worker->m_pulseShapeDataSplineB.append(outputData.m_pulseShapeDataSplineB.at(i));

                        const int size = outputData.m_pulseShapeDataB.at(i).size();

                        bool bdirB = true;

                        if (!(m_worker->m_pulseShapeDataBCounter%2))
                            bdirB = true;
                        else
                            bdirB = false;

                        if (bdirB) {
                            for ( int j = 0 ; j < size ; ++ j ) {
                                m_worker->m_pulseShapeDataB.append(outputData.m_pulseShapeDataB.at(i).at(j));
                            }
                        }
                        else {
                            for ( int j = size-1 ; j >= 0 ; -- j ) {
                                m_worker->m_pulseShapeDataB.append(outputData.m_pulseShapeDataB.at(i).at(j));
                            }
                        }

                        m_worker->m_pulseShapeDataBCounter ++;


                        if (m_worker->m_pulseShapeDataBCounter == (m_worker->m_pulseShapeDataAmountB + 1))
                            m_worker->m_isRecordingForShapeFilterB = false;
                    }
                    else {
                        break;
                    }
                }
            }
        }
    }
}

int DRS4WorkerConcurrentManager::activeThreads() const
{
    const int threadCount = QThreadPool::globalInstance()->activeThreadCount();

    return (threadCount==0?1:threadCount);
}

int DRS4WorkerConcurrentManager::maxThreads() const
{
    return m_recommendedThreads;
}
