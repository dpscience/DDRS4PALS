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

#include "drs4worker.h"

#include "Stream/drs4streamdataloader.h"
#include "DLib/DMath/dspline.h"

DRS4Worker::DRS4Worker(DRS4WorkerDataExchange *dataExchange, QObject *parent) :
    m_dataExchange(dataExchange),
    QObject(parent),
    m_nextSignal(true),
    m_isBlocking(false),
    m_isRunning(false)
{
    m_workerConcurrentManager = new DRS4WorkerConcurrentManager(this);

    resetPHSA();
    resetPHSB();

    resetAreaFilterA();
    resetAreaFilterB();

    resetMergedSpectrum();

    resetLifetimeEfficiencyCounter();
}

DRS4Worker::~DRS4Worker()
{
    DDELETE_SAFETY(m_workerConcurrentManager);
}

void DRS4Worker::initDRS4Worker() {}

void DRS4Worker::start()
{
    m_copyData.clear();

    if ( !DRS4BoardManager::sharedInstance()->currentBoard() ) {
        DRS4BoardManager::sharedInstance()->setDemoMode(true);
    }
    else
        DRS4BoardManager::sharedInstance()->setDemoMode(false);

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
#ifdef __DEPRECATED_WORKER
    m_pListChannelASpline.clear();
#endif
}

void DRS4Worker::resetPulseB()
{
    m_pListChannelB.clear();
#ifdef __DEPRECATED_WORKER
    m_pListChannelBSpline.clear();
#endif
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
}

void DRS4Worker::resetAreaFilterB()
{
    QMutexLocker locker(&m_mutex);

    m_areaFilterBCounter = 0;
    m_areaFilterDataB.fill(QPointF(-1, -1), 5000);
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

#ifdef __DEPRECATED_WORKER
void DRS4Worker::runSingleThreaded()
{
    const int sizeOfWave = sizeof(float)*kNumberOfBins;
    const int sizeOfFloat = sizeof(float);

    m_isBlocking = false;
    m_isRunning = true;

    DRS4LifetimeData ltData;
    DRS4FilterData dataF;

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
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

        if ( !DRS4SettingsManager::sharedInstance()->ignoreBusyState() ) {
            if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
                while ( !DRS4BoardManager::sharedInstance()->currentBoard()->IsEventAvailable() ) {
                    while ( !nextSignal() ) {
                        m_isBlocking = true;
                    }
                }
            }
        }

        while ( !nextSignal() ) {
            m_isBlocking = true;
        }

        m_isBlocking = false;

        if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
            try {
                 // just 2 waves are necessary! Place them next to each other like (Chn1 & Chn2) to reduce sampling!
                DRS4BoardManager::sharedInstance()->currentBoard()->TransferWaves(0, 2);
            }
            catch (...) {
                continue;
            }
        }

        float tChannel0[kNumberOfBins] = {0};
        float tChannel1[kNumberOfBins] = {0};

        float waveChannel0[kNumberOfBins] = {0};
        float waveChannel1[kNumberOfBins] = {0};

        std::fill(tChannel0, tChannel0 + sizeof(tChannel0)/sizeOfFloat, 0);
        std::fill(tChannel1, tChannel1 + sizeof(tChannel1)/sizeOfFloat, 0);

        std::fill(waveChannel0, waveChannel0 + sizeof(waveChannel0)/sizeOfFloat, 0);
        std::fill(waveChannel1, waveChannel1 + sizeof(waveChannel1)/sizeOfFloat, 0);

        if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
            int retState = 1;
            int retStateT = 1;
            int retStateV = kSuccess;

            try {
                retStateT = DRS4BoardManager::sharedInstance()->currentBoard()->GetTime(0, 0, DRS4BoardManager::sharedInstance()->currentBoard()->GetTriggerCell(0), tChannel0);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\ttime(0) try-catch"));
                continue;
            }

            if ( retStateT != 1 ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState time(0): " + QVariant(retStateT).toString()));
                continue;
            }

            try {
                retStateT = DRS4BoardManager::sharedInstance()->currentBoard()->GetTime(0, 2 ,DRS4BoardManager::sharedInstance()->currentBoard()->GetTriggerCell(0), tChannel1);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\ttime(1) try-catch"));
                continue;
            }

            if ( retStateT != 1 ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState time(1): " + QVariant(retStateT).toString()));
                continue;
            }

            try {
                retStateV = DRS4BoardManager::sharedInstance()->currentBoard()->GetWave(0, 0, waveChannel0);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\tvolt(0) try-catch"));
                continue;
            }

            if ( retStateV != kSuccess ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState volt(0): " + QVariant(retStateV).toString()));
                continue;
            }

            try {
                retStateV = DRS4BoardManager::sharedInstance()->currentBoard()->GetWave(0, 2, waveChannel1);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\tvolt(1) try-catch"));
                continue;
            }

            if ( retStateV != kSuccess ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState volt(1): " + QVariant(retStateV).toString()));
                continue;
            }

            try {
                retState = DRS4BoardManager::sharedInstance()->currentBoard()->StartDomino(); // returns always 1.
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\tstart Domino-Wave: try-catch"));
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
                    DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treadStream: error"));
                    continue;
                }
            }
        }

        //clear pulse-data:
        if ( !DRS4SettingsManager::sharedInstance()->isBurstMode() ) {
            resetPulseA();
            resetPulseB();
        }

        //statistics:
        time(&stop);
        const double diffTime = difftime(stop, start);
        if ( diffTime >= 1.0f ) {
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

        float xMinA = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();
        float xMaxA = 0.0f;

        float xMinB = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();
        float xMaxB = 0.0f;

        float yMinA = 500.0f;
        float yMaxA = -500.0f;

        float yMinB = 500.0f;
        float yMaxB = -500.0f;

        float yMinASort = 500.0f;
        float yMaxASort = -500.0f;

        float yMinBSort = 500.0f;
        float yMaxBSort = -500.0f;

        const int startCell = DRS4SettingsManager::sharedInstance()->startCell();
        const int endRange = DRS4SettingsManager::sharedInstance()->stopCell();
        const int stopCellWidth = kNumberOfBins-DRS4SettingsManager::sharedInstance()->stopCell();
        const int cellWidth = kNumberOfBins-startCell-stopCellWidth;

        std::vector<double> dataVecXA(cellWidth);
        std::vector<double> dataVecYA(cellWidth);

        std::vector<double> dataVecXB(cellWidth);
        std::vector<double> dataVecYB(cellWidth);

        //default: linear!
        DSpline _splineA, _splineB;

        //cubic spline?
        if ( (cellWidth > DRS4SettingsManager::sharedInstance()->numberOfCubicSplinePoints() || cellWidth < DRS4SettingsManager::sharedInstance()->numberOfCubicSplinePoints())
             && !DRS4SettingsManager::sharedInstance()->usingLinearSpline() ) {
            _splineA.set_type(SplineType::Cubic);
            _splineB.set_type(SplineType::Cubic);
        }

        int cellYAMax = -1;
        int cellYAMin = -1;
        int cellYBMax = -1;
        int cellYBMin = -1;

        for ( int a = startCell ; a < endRange ; ++ a ) {
            xMinA = qMin(xMinA, tChannel0[a]);
            xMaxA = qMax(xMaxA, tChannel0[a]);

            xMinB = qMin(xMinB, tChannel1[a]);
            xMaxB = qMax(xMaxB, tChannel1[a]);

            if ( waveChannel0[a] >= yMaxA ) {
                yMaxA = waveChannel0[a];
                cellYAMax = a;
            }

            if ( waveChannel1[a] >= yMaxB ) {
                yMaxB = waveChannel1[a];
                cellYBMax = a;
            }

            if ( waveChannel0[a] <= yMinA ) {
                yMinA = waveChannel0[a];
                cellYAMin = a;
            }

            if ( waveChannel1[a] <= yMinB ) {
                yMinB = waveChannel1[a];
                cellYBMin = a;
            }

            if ( waveChannel0[a] >= yMaxASort )
                yMaxASort = waveChannel0[a];

            if ( waveChannel1[a] >= yMaxBSort )
                yMaxBSort = waveChannel1[a];

            if ( waveChannel0[a] <= yMinASort )
                yMinASort = waveChannel0[a];

            if ( waveChannel1[a] <= yMinBSort )
                yMinBSort = waveChannel1[a];

            //append pulse-point:
            if ( !DRS4SettingsManager::sharedInstance()->isBurstMode() ) {
                m_pListChannelA.append(QPointF(tChannel0[a], waveChannel0[a]));
                m_pListChannelB.append(QPointF(tChannel1[a], waveChannel1[a]));
            }

            const int it = (a-startCell);

            dataVecXA[it] = tChannel0[a];
            dataVecYA[it] = waveChannel0[a];

            dataVecXB[it] = tChannel1[a];
            dataVecYB[it] = waveChannel1[a];
        }

        if (cellYAMax == -1 || cellYAMin == -1 || cellYBMax == -1 || cellYBMin == -1)
            continue;

        if (qFuzzyCompare(yMinA, yMaxA) || qFuzzyCompare(yMinB, yMaxB))
            continue;

        if ( DRS4StreamManager::sharedInstance()->isArmed() ) {

            if (!DRS4StreamManager::sharedInstance()->write((const char*)tChannel0, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream time(0): size(" + QVariant(sizeOfWave).toString() + ")"));
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)waveChannel0, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream volt(0): size(" + QVariant(sizeOfWave).toString() + ")"));
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)tChannel1, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream time(1): size(" + QVariant(sizeOfWave).toString() + ")"));
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)waveChannel1, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream volt(1): size(" + QVariant(sizeOfWave).toString() + ")"));
            }
        }

        if ( DRS4TextFileStreamManager::sharedInstance()->isArmed() )
            DRS4TextFileStreamManager::sharedInstance()->writePulses(tChannel0, tChannel1, waveChannel0, waveChannel1, kNumberOfBins);


        //cubic spline?
        if ( (cellWidth > DRS4SettingsManager::sharedInstance()->numberOfCubicSplinePoints() || cellWidth < DRS4SettingsManager::sharedInstance()->numberOfCubicSplinePoints())
             && !DRS4SettingsManager::sharedInstance()->usingLinearSpline() ) {
            _splineA.set_points(dataVecXA, dataVecYA);
            _splineB.set_points(dataVecXB, dataVecYB);
        }

        //cubic spline?
        const int cubSplinePointsCnt = DRS4SettingsManager::sharedInstance()->numberOfCubicSplinePoints();
        const double fcubSplinePointsCntRatio = 1.0f/((double)cubSplinePointsCnt);
        const double incrA = (xMaxA-xMinA)*fcubSplinePointsCntRatio;
        const double incrB = (xMaxB-xMinB)*fcubSplinePointsCntRatio;

        if ( DRS4TextFileStreamManager::sharedInstance()->isArmed() ) {
            QList<QPointF> interpolA, interpolB;
            //cubic spline?
            if ( (cellWidth > DRS4SettingsManager::sharedInstance()->numberOfCubicSplinePoints() || cellWidth < DRS4SettingsManager::sharedInstance()->numberOfCubicSplinePoints())
                 && !DRS4SettingsManager::sharedInstance()->usingLinearSpline() ) {
                for ( int a = 0 ; a <= cubSplinePointsCnt ; ++ a ) {
                    const double splineXValueA = ((double)a)*incrA+xMinA;
                    const double splineXValueB = ((double)a)*incrB+xMinB;

                    const double valueAY = _splineA(splineXValueA);
                    const double valueBY = _splineB(splineXValueB);

                    interpolA.append(QPointF(splineXValueA, valueAY));
                    interpolB.append(QPointF(splineXValueB, valueBY));
                }
            }
            else { //linear spline?
                for ( int a = 0 ; a < cellWidth ; ++ a ) {
                    const double splineXValueA = dataVecXA.at(a);
                    const double splineXValueB = dataVecXB.at(a);

                    const double valueAY = dataVecYA.at(a);
                    const double valueBY = dataVecYB.at(a);

                    interpolA.append(QPointF(splineXValueA, valueAY));
                    interpolB.append(QPointF(splineXValueB, valueBY));
                }
            }

            DRS4TextFileStreamManager::sharedInstance()->writeInterpolations(&interpolA, &interpolB);
        }

        double areaA = 0;
        double areaB = 0;

        float t0_YMaxA = 0.0f;
        float t0_YMaxB = 0.0f;

        float t0_YMinA = 0.0f;
        float t0_YMinB = 0.0f;


        yMinA = 500.0f;
        yMinB = 500.0f;
        yMaxA = -500.0f;
        yMaxB = -500.0f;

        //cubic spline?
        if ( (cellWidth > DRS4SettingsManager::sharedInstance()->numberOfCubicSplinePoints() || cellWidth < DRS4SettingsManager::sharedInstance()->numberOfCubicSplinePoints())
             && !DRS4SettingsManager::sharedInstance()->usingLinearSpline() ) {
            //subsequent area determination for area-filter:
            for ( int a = 0 ; a < cubSplinePointsCnt ; ++ a ) {
                const double splineXValueA = ((double)a)*incrA+xMinA;
                const double splineXValueB = ((double)a)*incrB+xMinB;

                float valueAY = _splineA(splineXValueA);
                float valueBY = _splineB(splineXValueB);

                if ( valueAY < yMinA ) {
                    yMinA = valueAY;
                    t0_YMinA = splineXValueA;
                }

                if ( valueBY < yMinB ) {
                    yMinB = valueBY;
                    t0_YMinB = splineXValueB;
                }

                if ( valueAY > yMaxA ) {
                    yMaxA = valueAY;
                    t0_YMaxA = splineXValueA;
                }

                if ( valueBY > yMaxB ) {
                    yMaxB = valueBY;
                    t0_YMaxB = splineXValueB;
                }

                if ( a < cubSplinePointsCnt - 1 ) {
                    const double splineXValueANext = ((double)(a+1))*incrA+xMinA;
                    const double splineXValueBNext = ((double)(a+1))*incrB+xMinB;

                    float valueAYNext = _splineA(splineXValueANext);
                    float valueBYNext = _splineB(splineXValueBNext);

                    areaA += valueAYNext*0.5f;
                    areaB += valueBYNext*0.5f;
                }
            }
        }
        else { //linear spline?
            for ( int a = 0 ; a < cellWidth ; ++ a ) {
                const double splineXValueA = dataVecXA.at(a);
                const double splineXValueB = dataVecXB.at(a);

                const double valueAY = dataVecYA.at(a);
                const double valueBY = dataVecYB.at(a);

                if ( valueAY < yMinA ) {
                    yMinA = valueAY;
                    t0_YMinA = splineXValueA;
                }

                if ( valueBY < yMinB ) {
                    yMinB = valueBY;
                    t0_YMinB = splineXValueB;
                }

                if ( valueAY > yMaxA ) {
                    yMaxA = valueAY;
                    t0_YMaxA = splineXValueA;
                }

                if ( valueBY > yMaxB ) {
                    yMaxB = valueBY;
                    t0_YMaxB = splineXValueB;
                }

                if ( a < cellWidth - 1 ) {
                    const double splineXValueANext = dataVecXA.at(a+1);
                    const double splineXValueBNext = dataVecXB.at(a+1);

                    const double valueAYNext = dataVecYA.at(a+1);
                    const double valueBYNext = dataVecYB.at(a+1);

                    areaA += valueAYNext*(splineXValueANext - splineXValueA)*0.5f;
                    areaB += valueBYNext*(splineXValueBNext - splineXValueB)*0.5f;
                }
            }
        }

        const float rat = 5120*((float)cellWidth/((float)kNumberOfBins));

        areaA = abs(areaA)/(DRS4SettingsManager::sharedInstance()->pulseAreaFilterNormalizationA()*rat);
        areaB = abs(areaB)/(DRS4SettingsManager::sharedInstance()->pulseAreaFilterNormalizationB()*rat);

        if ( (int)yMinA == (int)yMaxA || (int)yMinB == (int)yMaxB  )
            continue;

        //filter wrong pulses:
        if ( DRS4SettingsManager::sharedInstance()->isPositiveSignal() ) {
            if ( fabs(yMinASort) > fabs(yMaxASort) )
                continue;

            if ( fabs(yMinBSort) > fabs(yMaxBSort) )
                continue;

            if ( abs(cellYAMax - startCell) <= 45 )
                continue;

            if ( abs(cellYBMax - startCell) <= 45 )
                continue;

            if ( qFuzzyCompare(dataVecYA.at(cellWidth-1), (double)yMaxASort) )
                continue;

            if ( qFuzzyCompare(dataVecYB.at(cellWidth-1), (double)yMaxBSort) )
                continue;

            if ( qFuzzyCompare(dataVecYA.at(0), (double)yMaxASort) )
                continue;

            if ( qFuzzyCompare(dataVecYB.at(0), (double)yMaxBSort) )
                continue;
        }
        else {
            if ( fabs(yMinASort) < fabs(yMaxASort) )
                continue;

            if ( fabs(yMinBSort) < fabs(yMaxBSort) )
                continue;

            if ( abs(cellYAMin - startCell) <= 45 )
                continue;

            if ( abs(cellYBMin - startCell) <= 45 )
                continue;

            if ( qFuzzyCompare(dataVecYA.at(cellWidth-1), (double)yMinASort) )
                continue;

            if ( qFuzzyCompare(dataVecYB.at(cellWidth-1), (double)yMinBSort) )
                continue;

            if ( qFuzzyCompare(dataVecYA.at(0), (double)yMinASort) )
                continue;

            if ( qFuzzyCompare(dataVecYB.at(0), (double)yMinBSort) )
                continue;
        }

        int channelYMaxA = 0;
        int channelYMaxB = 0;

        double cfdValueA = 0;
        double cfdValueB = 0;

        if ( DRS4SettingsManager::sharedInstance()->isPositiveSignal() ) {
            channelYMaxA = ((int)((yMaxA/(500.0f))*(double)kNumberOfBins))-1;
            channelYMaxB = ((int)((yMaxB/(500.0f))*(double)kNumberOfBins))-1;

            cfdValueA = DRS4SettingsManager::sharedInstance()->cfdLevelA()*yMaxA;
            cfdValueB = DRS4SettingsManager::sharedInstance()->cfdLevelB()*yMaxB;

            if ( cfdValueA > 500.0f || qFuzzyCompare(cfdValueA, 0.0) || cfdValueA < 0.0f )
                continue;

            if ( cfdValueB > 500.0f || qFuzzyCompare(cfdValueB, 0.0) || cfdValueB < 0.0f )
                continue;

            if ( (int)cfdValueA == (int)yMaxA )
                continue;

            if ( (int)cfdValueB == (int)yMaxB )
                continue;
        }
        else {
            channelYMaxA = ((int)((yMinA/(-500.0f))*(double)kNumberOfBins))-1;
            channelYMaxB = ((int)((yMinB/(-500.0f))*(double)kNumberOfBins))-1;

            cfdValueA = DRS4SettingsManager::sharedInstance()->cfdLevelA()*yMinA;
            cfdValueB = DRS4SettingsManager::sharedInstance()->cfdLevelB()*yMinB;

            if ( cfdValueA < -500.0f || qFuzzyCompare(cfdValueA, 0.0) || cfdValueA > 0.0f )
                continue;

            if ( cfdValueB < -500.0f || qFuzzyCompare(cfdValueB, 0.0) || cfdValueB > 0.0f )
                continue;

            if ( (int)cfdValueA == (int)yMinA )
                continue;

            if ( (int)cfdValueB == (int)yMinB )
                continue;
        }

        //PHS:
        if ( channelYMaxA < kNumberOfBins && channelYMaxA >= 0 ) {
            m_phsA[channelYMaxA] ++;
            m_phsACounts ++;
        }

        if ( channelYMaxB < kNumberOfBins && channelYMaxB >= 0 ) {
            m_phsB[channelYMaxB] ++;
            m_phsBCounts ++;
        }

        /* Area-Filter */
        dataF.areaARatio = areaA;
        dataF.areaBRatio = areaB;
        dataF.amplitudeAInMV = DRS4SettingsManager::sharedInstance()->isPositiveSignal()?yMaxA:abs(yMinA);
        dataF.amplitudeBInMV = DRS4SettingsManager::sharedInstance()->isPositiveSignal()?yMaxB:abs(yMinB);

        double timeStampA = -1;
        double timeStampB = -1;

        bool btimeStampA = false;
        bool btimeStampB = false;

        bool bReject = false;
        int cntTimeStampA = 0;
        int cntTimeStampB = 0;

        //cubic spline?
        if ( (cellWidth > DRS4SettingsManager::sharedInstance()->numberOfCubicSplinePoints() || cellWidth < DRS4SettingsManager::sharedInstance()->numberOfCubicSplinePoints())
             && !DRS4SettingsManager::sharedInstance()->usingLinearSpline() ) {
            for ( int a = 0 ; a <= cubSplinePointsCnt-1 ; ++ a ) {
                const double splineXValueA_1 = ((double)a)*incrA+xMinA;
                const double splineXValueB_1 = ((double)a)*incrB+xMinB;

                const double splineXValueA_2 = ((double)(a+1))*incrA+xMinA;
                const double splineXValueB_2 = ((double)(a+1))*incrB+xMinB;

                const double valueAY_1 = _splineA(splineXValueA_1);
                const double valueBY_1 = _splineB(splineXValueB_1);

                const double valueAY_2 = _splineA(splineXValueA_2);
                const double valueBY_2 = _splineB(splineXValueB_2);

                if ( DRS4SettingsManager::sharedInstance()->isPositiveSignal() ) {
                    if ( ((cfdValueA > valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA < valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2)))
                         || ((cfdValueA < valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA > valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2))) ) {

                        if ( qFuzzyCompare(cfdValueA, valueAY_2) )
                            continue;

                        if ( cntTimeStampA == 0 ) {
                            const double slopeA = (valueAY_2 - valueAY_1)/(splineXValueA_2 - splineXValueA_1);
                            const double intersectA = valueAY_2 - slopeA*splineXValueA_2;

                            timeStampA = (cfdValueA - intersectA)/slopeA;

                            btimeStampA = true;
                        }

                        cntTimeStampA ++;
                    }

                    if ( ((cfdValueB > valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB < valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2)))
                         || ((cfdValueB < valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB > valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2))) ) {

                        if ( qFuzzyCompare(cfdValueB, valueBY_2) )
                            continue;

                        if ( cntTimeStampB == 0 ) {
                            const double slopeB = (valueBY_2 - valueBY_1)/(splineXValueB_2 - splineXValueB_1);
                            const double intersectB = valueBY_2 - slopeB*splineXValueB_2;

                            timeStampB = (cfdValueB - intersectB)/slopeB;

                            btimeStampB = true;
                        }

                        cntTimeStampB ++;
                    }

                    if ( cntTimeStampA > 2 || cntTimeStampB > 2 ) {
                        bReject = true;
                    }
                }
                else {
                    if ( ((cfdValueA < valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA > valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2)))
                         || ((cfdValueA > valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA < valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2))) ) {

                        if ( qFuzzyCompare(cfdValueA, valueAY_2) )
                            continue;

                        if ( cntTimeStampA == 0 ) {
                            const double slopeA = (valueAY_2 - valueAY_1)/(splineXValueA_2 - splineXValueA_1);
                            const double intersectA = valueAY_2 - slopeA*splineXValueA_2;

                            timeStampA = (cfdValueA - intersectA)/slopeA;

                            btimeStampA = true;
                        }

                        cntTimeStampA ++;
                    }

                    if ( ((cfdValueB < valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB > valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2)))
                         || ((cfdValueB > valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB < valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2))) ) {

                        if ( qFuzzyCompare(cfdValueB, valueBY_2) )
                            continue;

                        if ( cntTimeStampB == 0 ) {
                            const double slopeB = (valueBY_2 - valueBY_1)/(splineXValueB_2 - splineXValueB_1);
                            const double intersectB = valueBY_2 - slopeB*splineXValueB_2;

                            timeStampB = (cfdValueB - intersectB)/slopeB;

                            btimeStampB = true;
                        }

                        cntTimeStampB ++;
                    }

                    if ( cntTimeStampA > 2 || cntTimeStampB > 2 ) {
                        bReject = true;
                    }
                }

                if (!DRS4SettingsManager::sharedInstance()->isBurstMode()) {
                    if ( !btimeStampA )
                        m_pListChannelASpline.append(QPointF(splineXValueA_2, valueAY_2));

                    if ( !btimeStampB )
                        m_pListChannelBSpline.append(QPointF(splineXValueB_2, valueBY_2));
                }

                if (bReject) {
                    if (!DRS4SettingsManager::sharedInstance()->isBurstMode())
                        resetPulseSplines();

                    break;
                }
            }

            if ( cntTimeStampA <= 1 || cntTimeStampB <= 1 )
                bReject = true;

            if (bReject) {
                if (!DRS4SettingsManager::sharedInstance()->isBurstMode())
                    resetPulseSplines();

                continue;
            }
        }
        else { //linear spline?
            for ( int a = 0 ; a < cellWidth - 1 ; ++ a ) {
                const double splineXValueA_1 = dataVecXA.at(a);
                const double splineXValueB_1 = dataVecXB.at(a);

                const double splineXValueA_2 = dataVecXA.at(a+1);
                const double splineXValueB_2 = dataVecXB.at(a+1);

                const double valueAY_1 = dataVecYA.at(a);
                const double valueBY_1 = dataVecYB.at(a);

                const double valueAY_2 = dataVecYA.at(a+1);
                const double valueBY_2 = dataVecYB.at(a+1);

                if ( DRS4SettingsManager::sharedInstance()->isPositiveSignal() ) {
                    if ( ((cfdValueA > valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA < valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2)))
                         || ((cfdValueA < valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA > valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2))) ) {

                        if ( qFuzzyCompare(cfdValueA, valueAY_2) )
                            continue;

                        if ( cntTimeStampA == 0 ) {
                            const double slopeA = (valueAY_2 - valueAY_1)/(splineXValueA_2 - splineXValueA_1);
                            const double intersectA = valueAY_2 - slopeA*splineXValueA_2;

                            timeStampA = (cfdValueA - intersectA)/slopeA;

                            btimeStampA = true;
                        }

                        cntTimeStampA ++;
                    }

                    if ( ((cfdValueB > valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB < valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2)))
                         || ((cfdValueB < valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB > valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2))) ) {

                        if ( qFuzzyCompare(cfdValueB, valueBY_2) )
                            continue;

                        if ( cntTimeStampB == 0 ) {
                            const double slopeB = (valueBY_2 - valueBY_1)/(splineXValueB_2 - splineXValueB_1);
                            const double intersectB = valueBY_2 - slopeB*splineXValueB_2;

                            timeStampB = (cfdValueB - intersectB)/slopeB;

                            btimeStampB = true;
                        }

                        cntTimeStampB ++;
                    }

                    if ( cntTimeStampA > 2 || cntTimeStampB > 2) {
                        bReject = true;
                    }
                }
                else {
                    if ( ((cfdValueA < valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA > valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2)))
                         || ((cfdValueA > valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA < valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2))) ) {

                        if ( qFuzzyCompare(cfdValueA, valueAY_2) )
                            continue;

                        if ( cntTimeStampA == 0 ) {
                            const double slopeA = (valueAY_2 - valueAY_1)/(splineXValueA_2 - splineXValueA_1);
                            const double intersectA = valueAY_2 - slopeA*splineXValueA_2;

                            timeStampA = (cfdValueA - intersectA)/slopeA;

                            btimeStampA = true;
                        }

                        cntTimeStampA ++;
                    }

                    if ( ((cfdValueB < valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB > valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2)))
                         || ((cfdValueB > valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB < valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2))) ) {

                        if ( qFuzzyCompare(cfdValueB, valueBY_2) )
                            continue;

                        if ( cntTimeStampB == 0 ) {
                            const double slopeB = (valueBY_2 - valueBY_1)/(splineXValueB_2 - splineXValueB_1);
                            const double intersectB = valueBY_2 - slopeB*splineXValueB_2;

                            timeStampB = (cfdValueB - intersectB)/slopeB;

                            btimeStampB = true;
                        }

                        cntTimeStampB ++;
                    }

                    if ( cntTimeStampA > 2 || cntTimeStampB > 2 ) {
                        bReject = true;
                    }
                }

                if (!DRS4SettingsManager::sharedInstance()->isBurstMode()) {
                    if ( !btimeStampA )
                        m_pListChannelASpline.append(QPointF(splineXValueA_2, valueAY_2));

                    if ( !btimeStampB )
                        m_pListChannelBSpline.append(QPointF(splineXValueB_2, valueBY_2));
                }

                if (bReject) {
                    if (!DRS4SettingsManager::sharedInstance()->isBurstMode())
                        resetPulseSplines();

                    break;
                }
            }

            if ( cntTimeStampA <= 1 || cntTimeStampB <= 1 )
                bReject = true;

            if (bReject) {
                if (!DRS4SettingsManager::sharedInstance()->isBurstMode())
                    resetPulseSplines();

                continue;
            }
        }

        if (!DRS4SettingsManager::sharedInstance()->isBurstMode()) {
            if ( m_pListChannelASpline.isEmpty() || m_pListChannelBSpline.isEmpty() ) {
                continue;
            }
        }

        if ( bReject || !btimeStampA || !btimeStampB ) {
            continue;
        }

        /* lifetime-data */
        if ( DRS4SettingsManager::sharedInstance()->isPositiveSignal() ) {
            ltData.amplitudeAInMV = yMaxA;
            ltData.amplitudeBInMV = yMaxB;
        }
        else {
            ltData.amplitudeAInMV = yMinA;
            ltData.amplitudeBInMV = yMinB;
        }

        ltData.tAInNS = timeStampA;
        ltData.tBInNS = timeStampB;

        ltData.areaARatio = areaA;
        ltData.areaBRatio = areaB;

        if ( DRS4TextFileStreamRangeManager::sharedInstance()->isArmed() ) {
            ltData.rawDataA.append(m_pListChannelA);
            ltData.rawDataB.append(m_pListChannelB);
        }

        /* persistance - data */
        QVector<QPointF> persistanceAMeta;
        QVector<QPointF> persistanceBMeta;

        if ( btimeStampA && btimeStampB && !bReject ) {
            //normalized persistance data:
            if (DRS4SettingsManager::sharedInstance()->isPersistanceEnabled()
                    && !DRS4SettingsManager::sharedInstance()->isBurstMode() ) {

                m_persistanceDataA.clear();
                m_persistanceDataB.clear();

                for ( int j = 0 ; j < m_pListChannelA.size() ; ++ j ) {
                    const float yA = DRS4SettingsManager::sharedInstance()->isPositiveSignal()?(m_pListChannelA.at(j).y()/yMaxA):(m_pListChannelA.at(j).y()/yMinA);
                    const float yB = DRS4SettingsManager::sharedInstance()->isPositiveSignal()?(m_pListChannelB.at(j).y()/yMaxB):(m_pListChannelB.at(j).y()/yMinB);

                    const float t0A = timeStampA; //DRS4SettingsManager::sharedInstance()->isPositiveSignal()?t0_YMaxA:t0_YMinA;
                    const float t0B = timeStampB; //DRS4SettingsManager::sharedInstance()->isPositiveSignal()?t0_YMaxB:t0_YMinB;

                    persistanceAMeta.append(QPointF(m_pListChannelA.at(j).x()-t0B, yA)); // >> shift channel A relative to CFD-%(t0) of B
                    persistanceBMeta.append(QPointF(m_pListChannelB.at(j).x()-t0A, yB)); // >> shift channel B relative to CFD-%(t0) of A
                }
            }
        }

        /* area-Filter */
        if (!DRS4SettingsManager::sharedInstance()->isBurstMode()) {
            if (DRS4SettingsManager::sharedInstance()->isPulseAreaFilterPlotEnabled()) {
                const int indexPHSA = (dataF.amplitudeAInMV/500.0f)*kNumberOfBins;
                const int indexPHSB = (dataF.amplitudeBInMV/500.0f)*kNumberOfBins;

                const int binA = dataF.areaARatio*(double)DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA();
                const int binB = dataF.areaBRatio*(double)DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB();

                if (!(indexPHSA>(kNumberOfBins-1)
                      || indexPHSB>(kNumberOfBins-1)
                      || indexPHSA<0
                      || indexPHSB<0
                      || binA < 0
                      || binB < 0)) {
                    if (m_areaFilterACounter >= 5000 )
                        m_areaFilterACounter = 0;

                    if (m_areaFilterBCounter >= 5000 )
                        m_areaFilterBCounter = 0;

                    m_areaFilterDataA[m_areaFilterACounter] = QPointF(indexPHSA, binA);
                    m_areaFilterDataB[m_areaFilterBCounter] = QPointF(indexPHSB, binB);

                    m_areaFilterACounter ++;
                    m_areaFilterBCounter ++;
                }
            }
        }

        calcLifetimesInBurstMode(&ltData, &persistanceAMeta, &persistanceBMeta);
    }
}
#endif

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
                while ( !DRS4BoardManager::sharedInstance()->currentBoard()->IsEventAvailable() ) {
                    while ( !nextSignal() ) {
                        m_isBlocking = true;
                    }
                }
            }
        }

        while ( !nextSignal() ) {
            m_isBlocking = true;
        }

        m_isBlocking = false;

        if (!bDemoMode) {
            try {
                 // just 2 waves are necessary! Place them next to each other like (Chn1 & Chn2) to reduce sampling!
                DRS4BoardManager::sharedInstance()->currentBoard()->TransferWaves(0, 2);
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


        if (!bDemoMode) {
            int retState = 1;
            int retStateT = 1;
            int retStateV = kSuccess;

            try {
                retStateT = DRS4BoardManager::sharedInstance()->currentBoard()->GetTime(0, 0, DRS4BoardManager::sharedInstance()->currentBoard()->GetTriggerCell(0), tChannel0);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\ttime(0) try-catch"));
                continue;
            }

            if ( retStateT != 1 ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState time(0): " + QVariant(retStateT).toString()));
                continue;
            }

            try {
                retStateT = DRS4BoardManager::sharedInstance()->currentBoard()->GetTime(0, 2 ,DRS4BoardManager::sharedInstance()->currentBoard()->GetTriggerCell(0), tChannel1);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\ttime(1) try-catch"));
                continue;
            }

            if ( retStateT != 1 ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState time(1): " + QVariant(retStateT).toString()));
                continue;
            }

            try {
                retStateV = DRS4BoardManager::sharedInstance()->currentBoard()->GetWave(0, 0, waveChannel0);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\tvolt(0) try-catch"));
                continue;
            }

            if ( retStateV != kSuccess ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState volt(0): " + QVariant(retStateV).toString()));
                continue;
            }

            try {
                retStateV = DRS4BoardManager::sharedInstance()->currentBoard()->GetWave(0, 2, waveChannel1);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\tvolt(1) try-catch"));
                continue;
            }

            if ( retStateV != kSuccess ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState volt(1): " + QVariant(retStateV).toString()));
                continue;
            }

            try {
                retState = DRS4BoardManager::sharedInstance()->currentBoard()->StartDomino(); // returns always 1.
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\tstart Domino-Wave: try-catch"));
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
                    DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treadStream: error"));
                    continue;
                }
            }
        }

        /* statistics: */
        time(&stop);
        const double diffTime = difftime(stop, start);
        if ( diffTime >= 1.0f ) {
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

        const DRS4InterpolationType::type interpolationType = DRS4SettingsManager::sharedInstance()->interpolationType();
        const DRS4SplineInterpolationType::type splineInterpolationType = DRS4SettingsManager::sharedInstance()->splineInterpolationType();
        const bool bUsingALGLIB = (splineInterpolationType < 6 && splineInterpolationType > 1);
        const bool bUsingTinoKluge = ((interpolationType == DRS4InterpolationType::type::spline) && (splineInterpolationType == 6));
        const bool bUsingLinearInterpol = ((interpolationType == DRS4InterpolationType::type::spline) && (splineInterpolationType == 1));
        const int intraRenderPoints = (DRS4InterpolationType::type::spline == interpolationType)?(DRS4SettingsManager::sharedInstance()->splineIntraSamplingCounts()):(DRS4SettingsManager::sharedInstance()->polynomialSamplingCounts());
        const int streamIntraRenderPoints = DRS4ProgramSettingsManager::sharedInstance()->splineIntraPoints();

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

        /* inline CFD value estimation */
        float cfdValueA = 0.0f;
        float cfdValueB = 0.0f;

        /* inline CFD cell estimation */
        int estimCFDACellStart = -1;
        int estimCFDBCellStart = -1;

        int estimCFDACellStop = -1;
        int estimCFDBCellStop = -1;

        int cfdACounter = 0;
        int cfdBCounter = 0;

        float areaA = 0.0f;
        float areaB = 0.0f;

        /* reduce mathematical operations within in the loop */
        const int reducedEndRange = (endRange - 1);
        const int reducedCellWidth = (cellWidth - 1);
        const int extendedStartCell = (startCell + 1);

        /* determine min/max and proceed a CFD estimation in ROI */
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

                    cfdACounter = 0;
                }
            }

            if ( waveChannel1[a] >= yMaxB ) {
                yMaxB = waveChannel1[a];
                cellYBMax = a;

                if (positiveSignal) {
                    cfdValueB = cfdB*yMaxB;

                    cfdBCounter = 0;
                }
            }

            if ( waveChannel0[a] <= yMinA ) {
                yMinA = waveChannel0[a];
                cellYAMin = a;

                if (!positiveSignal) {
                    cfdValueA = cfdA*yMinA;

                    cfdACounter = 0;
                }
            }

            if ( waveChannel1[a] <= yMinB ) {
                yMinB = waveChannel1[a];
                cellYBMin = a;

                if (!positiveSignal) {
                    cfdValueB = cfdB*yMinB;

                    cfdBCounter = 0;
                }
            }

            /* CFD cell estimation/determination */
            if ( a >= extendedStartCell ) {
                const int aDecr = (a - 1);

                /* calculate the pulse area of ROI */
                if (bPulseAreaPlot
                        || bPulseAreaFilter) {
                    areaA += waveChannel0[aDecr]*(tChannel0[a] - tChannel0[aDecr])*0.5f;
                    areaB += waveChannel1[aDecr]*(tChannel1[a] - tChannel1[aDecr])*0.5f;
                }

                const double slopeA = (waveChannel0[a] - waveChannel0[aDecr])/(tChannel0[a] - tChannel0[aDecr]);
                const double slopeB = (waveChannel1[a] - waveChannel1[aDecr])/(tChannel1[a] - tChannel1[aDecr]);

                /* CFD on rising edge = positive slope? or falling edge = negative slope?*/
                const bool bInRangeA = positiveSignal?(slopeA > 1E-6):(slopeA < 1E-6);
                const bool bInRangeB = positiveSignal?(slopeB > 1E-6):(slopeB < 1E-6);

                if ( bInRangeA ) {
                    const bool cfdLevelInRangeA = positiveSignal?(waveChannel0[a] > cfdValueA && waveChannel0[aDecr] < cfdValueA):(waveChannel0[a] < cfdValueA && waveChannel0[aDecr] > cfdValueA);

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
                }

                if ( bInRangeB ) {
                    const bool cfdLevelInRangeB = positiveSignal?(waveChannel1[a] > cfdValueB && waveChannel1[aDecr] < cfdValueB):(waveChannel1[a] < cfdValueB && waveChannel1[aDecr] > cfdValueB);

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
                }
            }

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

        if ( DRS4StreamManager::sharedInstance()->isArmed() ) {
            if (!DRS4StreamManager::sharedInstance()->write((const char*)tChannel0, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream time(0): size(" + QVariant(sizeOfWave).toString() + ")"));
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)waveChannel0, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream volt(0): size(" + QVariant(sizeOfWave).toString() + ")"));
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)tChannel1, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream time(1): size(" + QVariant(sizeOfWave).toString() + ")"));
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)waveChannel1, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream volt(1): size(" + QVariant(sizeOfWave).toString() + ")"));
            }
        }

        /* light-weight filtering of wrong pulses: */
        if ( positiveSignal ) {
            if ( (abs(yMinA) > abs(yMaxA))
                 || (abs(yMinB) > abs(yMaxB)) )
                continue;

            if ( abs(cellYAMax - startCell) <= 45
                 || abs(cellYBMax - startCell) <= 45 )
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

            if ( abs(cellYAMin - startCell) <= 45
                 || abs(cellYBMin - startCell) <= 45 )
                continue;

            if ( qFuzzyCompare(waveChannel0[reducedEndRange], yMinA)
                 || qFuzzyCompare(waveChannel1[reducedEndRange], yMinB)
                 || qFuzzyCompare(waveChannel0[startCell], yMinA)
                 || qFuzzyCompare(waveChannel1[startCell], yMinB) )
                continue;
        }

        /* reject artefacts */
        if ( cfdBCounter > 1
             || cfdACounter > 1 )
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
            } else if (bUsingTinoKluge) {
                tkSplineA.setType(SplineType::Cubic);
                tkSplineB.setType(SplineType::Cubic);

                tkSplineA.setPoints(m_arrayDataTKX_A, m_arrayDataTKY_A);
                tkSplineB.setPoints(m_arrayDataTKX_B, m_arrayDataTKY_B);
            } else if (bUsingLinearInterpol) {
                /* nothing yet */
            }
        }
        else {
            alglib::polynomialbuild(m_arrayDataX_A, m_arrayDataY_A, m_baryCentricInterpolantA);
            alglib::polynomialbuild(m_arrayDataX_B, m_arrayDataY_B, m_baryCentricInterpolantB);
        }

        /* calculate and normalize pulse area for subsequent filtering */
        if (bPulseAreaPlot
                || bPulseAreaFilter) {
            const float rat = 5120*((float)cellWidth/((float)kNumberOfBins));

            areaA = abs(areaA)/(pulseAreaFilterNormA*rat);
            areaB = abs(areaB)/(pulseAreaFilterNormB*rat);
        }

        /* store/write pulses and interpolations to ASCII file */
        if (!bBurstMode) {
            if (bStreamWithoutRangeArmed) {
                DRS4TextFileStreamManager::sharedInstance()->writePulses(tChannel0, tChannel1, waveChannel0, waveChannel1, kNumberOfBins);

                if ( (DRS4TextFileStreamManager::sharedInstance()->writeInterpolationA() || DRS4TextFileStreamManager::sharedInstance()->writeInterpolationB())
                     && (m_pListChannelA.size() > 0 && m_pListChannelB.size() > 0) ) {
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
                        const float valYA = (interpolationType == DRS4InterpolationType::type::spline)?(alglib::spline1dcalc(m_interpolantA, valXA)):(alglib::barycentriccalc(m_baryCentricInterpolantA, valXA));
                        const float valYB = (interpolationType == DRS4InterpolationType::type::spline)?(alglib::spline1dcalc(m_interpolantA, valXB)):(alglib::barycentriccalc(m_baryCentricInterpolantB, valXB));

                        splineA[i] = QPointF(valXA, valYA);
                        splineB[i] = QPointF(valXB, valYB);
                    }

                    DRS4TextFileStreamManager::sharedInstance()->writeInterpolations(&splineA, &splineB, interpolationType, splineInterpolationType);
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
                const float valYA = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantA, tChannel0[cell_interpolRangeA_start] + (float)i*renderIncrementA)):(tkSplineA(tChannel0[cell_interpolRangeA_start] + (float)i*renderIncrementA))):(alglib::barycentriccalc(m_baryCentricInterpolantA, tChannel0[cell_interpolRangeA_start] + (float)i*renderIncrementA));
                const float valYB = (interpolationType == DRS4InterpolationType::type::spline)?(bUsingALGLIB?(alglib::spline1dcalc(m_interpolantB, tChannel1[cell_interpolRangeB_start] + (float)i*renderIncrementB)):(tkSplineB(tChannel1[cell_interpolRangeB_start] + (float)i*renderIncrementB))):(alglib::barycentriccalc(m_baryCentricInterpolantB, tChannel1[cell_interpolRangeB_start] + (float)i*renderIncrementB));

                /* modify min/max to calculate the CFD in  high accuracy, subsequently */
                yMaxA = qMax(yMaxA, valYA);
                yMaxB = qMax(yMaxB, valYB);

                yMinA = qMin(yMinA, valYA);
                yMinB = qMin(yMinB, valYB);
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

        /* CFD levels valid? */
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
            /* finding correct CFD timestamp within the estimated CFD bracket index region */
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
                    } else if ( qFuzzyCompare(cfdValueA, valY1) ) {
                        timeStampA = timeStamp1;

                        break;
                    } else if ( qFuzzyCompare(cfdValueA, valY2) ) {
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
                    } else if ( qFuzzyCompare(cfdValueB, valY1) ) {
                        timeStampB = timeStamp1;

                        break;
                    } else if ( qFuzzyCompare(cfdValueB, valY2) ) {
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
                } else if ( qFuzzyCompare(cfdValueA, valY1) ) {
                    timeStampA = timeStamp1;
                } else if ( qFuzzyCompare(cfdValueA, valY2) ) {
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
                } else if ( qFuzzyCompare(cfdValueB, valY1) ) {
                    timeStampB = timeStamp1;
                } else if ( qFuzzyCompare(cfdValueB, valY2) ) {
                    timeStampB = timeStamp2;
                }
            }
        }

        if ((int)timeStampA == -1
           || (int)timeStampB == -1)
            continue;

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

            if ( !y_AInside || !y_BInside )
                continue;
        }

        /* lifetime: A-B - master */
        if ( bIsStart_A
             && bIsStop_B && !bForcePrompt  )
        {
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
                }
                else if ( ltdiff >= 0 ) {
                    m_lifeTimeDataAB[binAB] ++;
                    m_abCounts ++;

                    m_maxY_ABSpectrum = qMax(m_maxY_ABSpectrum, m_lifeTimeDataAB[binAB]);
                }

                m_specABCounterCnt ++;

                if ( bStreamInRangeArmed ) {
                    if ( binAB >= DRS4TextFileStreamRangeManager::sharedInstance()->ltRangeMinAB()
                         && binAB <= DRS4TextFileStreamRangeManager::sharedInstance()->ltRangeMaxAB() ) {
                        if ( DRS4TextFileStreamRangeManager::sharedInstance()->isABEnabled() )
                            DRS4TextFileStreamRangeManager::sharedInstance()->writePulses(pulseDataA(), pulseDataB());
                    }
                }
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
                }
                else if ( ltdiff >= 0 ) {
                    m_lifeTimeDataBA[binBA] ++;
                    m_baCounts ++;

                    m_maxY_BASpectrum = qMax(m_maxY_BASpectrum, m_lifeTimeDataBA[binBA]);
                }

                m_specBACounterCnt ++;

                if ( bStreamInRangeArmed ) {
                    if ( binBA >= DRS4TextFileStreamRangeManager::sharedInstance()->ltRangeMinBA()
                         && binBA <= DRS4TextFileStreamRangeManager::sharedInstance()->ltRangeMaxBA() ) {
                        if ( DRS4TextFileStreamRangeManager::sharedInstance()->isBAEnabled() )
                            DRS4TextFileStreamRangeManager::sharedInstance()->writePulses(pulseDataA(), pulseDataB());
                    }
                }
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

                m_specCoincidenceCounterCnt ++;
            }

            /* calculate normalized persistance data */
            if (bPersistance
                    && !bBurstMode ) {
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

                    m_persistanceDataA[j] = QPointF(m_pListChannelA.at(j).x()-timeStampB, yA); // >> shift channel A relative to CFD-%(t0) of B
                    m_persistanceDataB[j] = QPointF(m_pListChannelB.at(j).x()-timeStampA, yB); // >> shift channel B relative to CFD-%(t0) of A
                }
            }
        }
    }
}

#ifdef __DEPRECATED_WORKER
void DRS4Worker::calcLifetimesInBurstMode(DRS4LifetimeData *ltData, QVector<QPointF> *persistanceA, QVector<QPointF> *persistanceB)
{
    if ( !ltData
         || !m_dataExchange
         || !persistanceA
         || !persistanceB)
        return;

    int convertToBinA = -1;
    int convertToBinB = -1;

    if ( DRS4SettingsManager::sharedInstance()->isPositiveSignal() ) { //rising edge:
        convertToBinA = ((int)((ltData->amplitudeAInMV/(500.0f))*kNumberOfBins))-1;
        convertToBinB = ((int)((ltData->amplitudeBInMV/(500.0f))*kNumberOfBins))-1;
    }
    else { //falling edge:
        convertToBinA = ((int)((ltData->amplitudeAInMV/(-500.0f))*kNumberOfBins))-1;
        convertToBinB = ((int)((ltData->amplitudeBInMV/(-500.0f))*kNumberOfBins))-1;
    }

    if ( convertToBinA == -1
         || convertToBinB == -1 )
        return;

    if ( convertToBinA >= kNumberOfBins || convertToBinA < 0 )
        return;

    if ( convertToBinB >= kNumberOfBins || convertToBinB < 0 )
        return;

    bool bIsStart_A = false;
    bool bIsStop_A = false;

    bool bIsStart_B = false;
    bool bIsStop_B = false;

    const int startMinA = DRS4SettingsManager::sharedInstance()->startChanneAMin();
    const int startMaxA = DRS4SettingsManager::sharedInstance()->startChanneAMax();
    const int stopMinA = DRS4SettingsManager::sharedInstance()->stopChanneAMin();
    const int stopMaxA = DRS4SettingsManager::sharedInstance()->stopChanneAMax();

    const int startMinB = DRS4SettingsManager::sharedInstance()->startChanneBMin();
    const int startMaxB = DRS4SettingsManager::sharedInstance()->startChanneBMax();
    const int stopMinB = DRS4SettingsManager::sharedInstance()->stopChanneBMin();
    const int stopMaxB = DRS4SettingsManager::sharedInstance()->stopChanneBMax();

    if ( convertToBinA >= startMinA && convertToBinA <= startMaxA ) bIsStart_A = true;
    if ( convertToBinA >= stopMinA && convertToBinA <= stopMaxA )  bIsStop_A = true;
    if ( convertToBinB >= startMinB && convertToBinB <= startMaxB ) bIsStart_B = true;
    if ( convertToBinB >= stopMinB && convertToBinB <= stopMaxB ) bIsStop_B = true;

    //apply area-filter:
    if ( DRS4SettingsManager::sharedInstance()->isPulseAreaFilterEnabled() ) {
        const double indexPHSA = (abs(ltData->amplitudeAInMV)/500.0f)*1024.0f;
        const double indexPHSB = (abs(ltData->amplitudeBInMV)/500.0f)*1024.0f;

        const double yLowerA = *m_dataExchange->m_areaFilterASlopeLower*indexPHSA+*m_dataExchange->m_areaFilterAInterceptLower;
        const double yUpperA = *m_dataExchange->m_areaFilterASlopeUpper*indexPHSA+*m_dataExchange->m_areaFilterAInterceptUpper;

        const bool y_AInside = (ltData->areaARatio*DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA() >= yLowerA
                                && ltData->areaARatio*DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA() <=yUpperA);

        const double yLowerB = *m_dataExchange->m_areaFilterBSlopeLower*indexPHSB+*m_dataExchange->m_areaFilterBInterceptLower;
        const double yUpperB = *m_dataExchange->m_areaFilterBSlopeUpper*indexPHSB+*m_dataExchange->m_areaFilterBInterceptUpper;

        const bool y_BInside = (ltData->areaBRatio*DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB() >= yLowerB
                                && ltData->areaBRatio*DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB() <=yUpperB);

        if ( !y_AInside
             || !y_BInside )
            return;
    }

    if ( bIsStart_A
         && bIsStop_B && !DRS4SettingsManager::sharedInstance()->isforceCoincidence()  ) //A-B (Master)
    {
        const double ltdiff = ltData->tBInNS - ltData->tAInNS;
        const int binAB = ((int)round(((((ltdiff)+DRS4SettingsManager::sharedInstance()->offsetInNSAB())/DRS4SettingsManager::sharedInstance()->scalerInNSAB()))*((double)DRS4SettingsManager::sharedInstance()->channelCntAB())))-1;

        const int binMerged = ((int)round(((((ltdiff+DRS4SettingsManager::sharedInstance()->meanCableDelay())+DRS4SettingsManager::sharedInstance()->offsetInNSMerged())/DRS4SettingsManager::sharedInstance()->scalerInNSMerged()))*((double)DRS4SettingsManager::sharedInstance()->channelCntMerged())))-1;

        if ( binAB < 0 || binAB >= m_lifeTimeDataAB.size() )
            return;

        if ( binAB >= 0
             && binAB < DRS4SettingsManager::sharedInstance()->channelCntAB() )
        {
            if ( DRS4SettingsManager::sharedInstance()->isNegativeLTAccepted() && ltdiff < 0  )
            {
                m_lifeTimeDataAB[binAB] ++;
                m_abCounts ++;

                m_maxY_ABSpectrum = qMax(m_maxY_ABSpectrum, m_lifeTimeDataAB[binAB]);
            }
            else if ( ltdiff >= 0 )
            {
                m_lifeTimeDataAB[binAB] ++;
                m_abCounts ++;

                m_maxY_ABSpectrum = qMax(m_maxY_ABSpectrum, m_lifeTimeDataAB[binAB]);
            }

            m_specABCounterCnt ++;

            if ( DRS4TextFileStreamRangeManager::sharedInstance()->isArmed() ) {
                if ( binAB >= DRS4TextFileStreamRangeManager::sharedInstance()->ltRangeMinAB()
                     && binAB <= DRS4TextFileStreamRangeManager::sharedInstance()->ltRangeMaxAB() ) {
                    if ( DRS4TextFileStreamRangeManager::sharedInstance()->isABEnabled() )
                        DRS4TextFileStreamRangeManager::sharedInstance()->writePulses(&ltData->rawDataA, &ltData->rawDataB);
                }
            }
        }

        if ( binMerged < 0 || binMerged >= m_lifeTimeDataMerged.size() )
            return;

        if ( binMerged >= 0
             && binMerged < DRS4SettingsManager::sharedInstance()->channelCntMerged() )
        {
            if ( DRS4SettingsManager::sharedInstance()->isNegativeLTAccepted() && ltdiff < 0  )
            {
                m_lifeTimeDataMerged[binMerged] ++;
                m_mergedCounts ++;

                m_maxY_MergedSpectrum = qMax(m_maxY_MergedSpectrum, m_lifeTimeDataMerged[binMerged]);
            }
            else if ( ltdiff >= 0 )
            {
                m_lifeTimeDataMerged[binMerged] ++;
                m_mergedCounts ++;

                m_maxY_MergedSpectrum = qMax(m_maxY_MergedSpectrum, m_lifeTimeDataMerged[binMerged]);
            }

            m_specMergedCounterCnt ++;
        }
    }
    else if (  bIsStart_B
               && bIsStop_A && !DRS4SettingsManager::sharedInstance()->isforceCoincidence() ) // B-A (Master)
    {
        const double ltdiff = ltData->tAInNS - ltData->tBInNS;
        const int binBA = ((int)round(((((ltdiff)+DRS4SettingsManager::sharedInstance()->offsetInNSBA())/DRS4SettingsManager::sharedInstance()->scalerInNSBA()))*((double)DRS4SettingsManager::sharedInstance()->channelCntBA())))-1;

        const int binMerged = ((int)round(((((ltdiff-DRS4SettingsManager::sharedInstance()->meanCableDelay())+DRS4SettingsManager::sharedInstance()->offsetInNSMerged())/DRS4SettingsManager::sharedInstance()->scalerInNSMerged()))*((double)DRS4SettingsManager::sharedInstance()->channelCntMerged())))-1;

        if ( binBA < 0 || binBA >= m_lifeTimeDataBA.size() )
            return;

        if ( binBA >= 0
             && binBA < DRS4SettingsManager::sharedInstance()->channelCntBA() )
        {
            if ( DRS4SettingsManager::sharedInstance()->isNegativeLTAccepted() && ltdiff < 0  )
            {
                m_lifeTimeDataBA[binBA] ++;
                m_baCounts ++;

                m_maxY_BASpectrum = qMax(m_maxY_BASpectrum, m_lifeTimeDataBA[binBA]);
            }
            else if ( ltdiff >= 0 )
            {
                m_lifeTimeDataBA[binBA] ++;
                m_baCounts ++;

                m_maxY_BASpectrum = qMax(m_maxY_BASpectrum, m_lifeTimeDataBA[binBA]);
            }

            m_specBACounterCnt ++;

            if ( DRS4TextFileStreamRangeManager::sharedInstance()->isArmed() ) {
                if ( binBA >= DRS4TextFileStreamRangeManager::sharedInstance()->ltRangeMinBA()
                     && binBA <= DRS4TextFileStreamRangeManager::sharedInstance()->ltRangeMaxBA() ) {
                    if ( DRS4TextFileStreamRangeManager::sharedInstance()->isBAEnabled() )
                        DRS4TextFileStreamRangeManager::sharedInstance()->writePulses(&ltData->rawDataA, &ltData->rawDataB);
                }
            }
        }

        if ( binMerged < 0 || binMerged >= m_lifeTimeDataMerged.size() )
            return;

        if ( binMerged >= 0
             && binMerged < DRS4SettingsManager::sharedInstance()->channelCntMerged() )
        {
            if ( DRS4SettingsManager::sharedInstance()->isNegativeLTAccepted() && ltdiff < 0  )
            {
                m_lifeTimeDataMerged[binMerged] ++;
                m_mergedCounts ++;

                m_maxY_MergedSpectrum = qMax(m_maxY_MergedSpectrum, m_lifeTimeDataMerged[binMerged]);
            }
            else if ( ltdiff >= 0 )
            {
                m_lifeTimeDataMerged[binMerged] ++;
                m_mergedCounts ++;

                m_maxY_MergedSpectrum = qMax(m_maxY_MergedSpectrum, m_lifeTimeDataMerged[binMerged]);
            }

            m_specMergedCounterCnt ++;
        }
    }
    else if (  bIsStop_B && bIsStop_A ) //Prompt (Stop A - Stop B) (Slave)
    {
        const double ltdiff = ltData->tAInNS - ltData->tBInNS;
        const int binBA = ((int)round(((((ltdiff)+DRS4SettingsManager::sharedInstance()->offsetInNSCoincidence())/DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence()))*((double)DRS4SettingsManager::sharedInstance()->channelCntCoincindence())))-1;

        if ( binBA < 0 || binBA >= m_lifeTimeDataCoincidence.size() )
            return;

        if ( binBA >= 0
             && binBA < DRS4SettingsManager::sharedInstance()->channelCntCoincindence() )
        {
            m_lifeTimeDataCoincidence[binBA] ++;
            m_coincidenceCounts ++;

            m_maxY_CoincidenceSpectrum = qMax(m_maxY_CoincidenceSpectrum, m_lifeTimeDataCoincidence[binBA]);

            m_specCoincidenceCounterCnt ++;
        }

        /* swap persistance data to member */
        if (DRS4SettingsManager::sharedInstance()->isPersistanceEnabled()
                && !DRS4SettingsManager::sharedInstance()->isBurstMode()
                && !persistanceA->isEmpty()
                && !persistanceB->isEmpty() ) {
            m_persistanceDataA.append(*persistanceA);
            m_persistanceDataB.append(*persistanceB);
        }
    }
}
#endif

#ifdef __DEPRECATED_WORKER
void DRS4Worker::runMultiThreaded()
{
    const int sizeOfWave = sizeof(float)*kNumberOfBins;
    const int sizeOfFloat = sizeof(float);

    m_isBlocking = false;
    m_isRunning = true;

    const int pulsePairChunkSize = DRS4ProgramSettingsManager::sharedInstance()->pulsePairChunkSize();

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4BoardManager::sharedInstance()->currentBoard()->StartDomino();

    time_t start;
    time_t stop;

    time(&start);
    time(&stop);

    forever {
        if ( !m_isRunning ) {
            m_copyData.clear();
            m_workerConcurrentManager->cancel();

            m_isBlocking = false;
            return;
        }

        if ( !DRS4SettingsManager::sharedInstance()->ignoreBusyState() ) {
            if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
                while ( !DRS4BoardManager::sharedInstance()->currentBoard()->IsEventAvailable() ) {
                    while ( !nextSignal() ) {
                        m_copyData.clear();
                        m_workerConcurrentManager->cancel();

                        m_isBlocking = true;
                    }
                }
            }
        }

        while ( !nextSignal() ) {
            m_copyData.clear();
            m_workerConcurrentManager->cancel();

            m_isBlocking = true;
        }

        m_isBlocking = false;

        if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
            try {
                DRS4BoardManager::sharedInstance()->currentBoard()->TransferWaves(0, 2); // just 2 waves are necessary! Place them next to each other like (Chn1 & Chn2) to reduce sampling-counts!
            }
            catch (...) {
                continue;
            }
        }

        float tChannel0[kNumberOfBins] = {0};
        float tChannel1[kNumberOfBins] = {0};

        float waveChannel0[kNumberOfBins] = {0};
        float waveChannel1[kNumberOfBins] = {0};

        std::fill(tChannel0, tChannel0 + sizeof(tChannel0)/sizeOfFloat, 0);
        std::fill(tChannel1, tChannel1 + sizeof(tChannel1)/sizeOfFloat, 0);

        std::fill(waveChannel0, waveChannel0 + sizeof(waveChannel0)/sizeOfFloat, 0);
        std::fill(waveChannel1, waveChannel1 + sizeof(waveChannel1)/sizeOfFloat, 0);


        if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
            int retState = 1;
            int retStateT = 1;
            int retStateV = kSuccess;

            try {
                retStateT = DRS4BoardManager::sharedInstance()->currentBoard()->GetTime(0, 0, DRS4BoardManager::sharedInstance()->currentBoard()->GetTriggerCell(0), tChannel0);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\ttime(0) try-catch"));
                continue;
            }

            if ( retStateT != 1 ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState time(0): " + QVariant(retStateT).toString()));
                continue;
            }

            try {
                retStateT = DRS4BoardManager::sharedInstance()->currentBoard()->GetTime(0, 2 ,DRS4BoardManager::sharedInstance()->currentBoard()->GetTriggerCell(0), tChannel1);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\ttime(1) try-catch"));
                continue;
            }

            if ( retStateT != 1 ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState time(1): " + QVariant(retStateT).toString()));
                continue;
            }

            try {
                retStateV = DRS4BoardManager::sharedInstance()->currentBoard()->GetWave(0, 0, waveChannel0);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\tvolt(0) try-catch"));
                continue;
            }

            if ( retStateV != kSuccess ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState volt(0): " + QVariant(retStateV).toString()));
                continue;
            }

            try {
                retStateV = DRS4BoardManager::sharedInstance()->currentBoard()->GetWave(0, 2, waveChannel1);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\tvolt(1) try-catch"));
                continue;
            }

            if ( retStateV != kSuccess ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState volt(1): " + QVariant(retStateV).toString()));
                continue;
            }

            try {
                retState = DRS4BoardManager::sharedInstance()->currentBoard()->StartDomino(); // returns always 1.
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\tstart Domino-Wave: try-catch"));
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
                    DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treadStream: error"));
                    continue;
                }
            }
        }

        //clear pulse-data:
        if ( !DRS4SettingsManager::sharedInstance()->isBurstMode() ) {
            resetPulseA();
            resetPulseB();
        }

        //statistics:
        time(&stop);
        const double diffTime = difftime(stop, start);
        if ( diffTime >= 1.0f ) {
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


        DRS4ConcurrentCopyInputData copyData;

        copyData.m_xMinA = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();
        copyData.m_xMaxA = 0.0f;

        copyData.m_xMinB = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();
        copyData.m_xMaxB = 0.0f;

        copyData.m_yMinA = 500.0f;
        copyData.m_yMaxA = -500.0f;

        copyData.m_yMinB = 500.0f;
        copyData.m_yMaxB = -500.0f;

        copyData.m_yMinASort = 500.0f;
        copyData.m_yMaxASort = -500.0f;

        copyData.m_yMinBSort = 500.0f;
        copyData.m_yMaxBSort = -500.0f;

        copyData.m_cellYAMax = -1;
        copyData.m_cellYAMin = -1;
        copyData.m_cellYBMax = -1;
        copyData.m_cellYBMin = -1;

        copyData.m_numberOfCubicSplinePoints = /*DRS4SettingsManager::sharedInstance()->numberOfCubicSplinePoints()*/1024;
        copyData.m_isPositiveSignalPolarity = DRS4SettingsManager::sharedInstance()->isPositiveSignal();
        copyData.m_isLinearInterpolation = /*DRS4SettingsManager::sharedInstance()->usingLinearSpline()*/true;

        copyData.m_cfdLevelA = DRS4SettingsManager::sharedInstance()->cfdLevelA();
        copyData.m_cfdLevelB = DRS4SettingsManager::sharedInstance()->cfdLevelB();

        copyData.m_usingBurstMode = DRS4SettingsManager::sharedInstance()->isBurstMode();

        copyData.m_startMinA = DRS4SettingsManager::sharedInstance()->startChanneAMin();
        copyData.m_startMaxA = DRS4SettingsManager::sharedInstance()->startChanneAMax();
        copyData.m_stopMinA = DRS4SettingsManager::sharedInstance()->stopChanneAMin();
        copyData.m_stopMaxA = DRS4SettingsManager::sharedInstance()->stopChanneAMax();

        copyData.m_startMinB = DRS4SettingsManager::sharedInstance()->startChanneBMin();
        copyData.m_startMaxB = DRS4SettingsManager::sharedInstance()->startChanneBMax();
        copyData.m_stopMinB = DRS4SettingsManager::sharedInstance()->stopChanneBMin();
        copyData.m_stopMaxB = DRS4SettingsManager::sharedInstance()->stopChanneBMax();

        copyData.m_isPulseAreaFilterEnabled = DRS4SettingsManager::sharedInstance()->isPulseAreaFilterEnabled();
        copyData.m_isPulseAreaFilterPlotEnabled = DRS4SettingsManager::sharedInstance()->isPulseAreaFilterPlotEnabled();

        copyData.m_pulseAreaFilterBinningA = DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA();
        copyData.m_pulseAreaFilterBinningB = DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB();

        copyData.m_pulseAreaFilterNormalizationA = DRS4SettingsManager::sharedInstance()->pulseAreaFilterNormalizationA();
        copyData.m_pulseAreaFilterNormalizationB = DRS4SettingsManager::sharedInstance()->pulseAreaFilterNormalizationB();

        copyData.m_channelCntCoincindence = DRS4SettingsManager::sharedInstance()->channelCntCoincindence();
        copyData.m_channelCntAB = DRS4SettingsManager::sharedInstance()->channelCntAB();
        copyData.m_channelCntBA = DRS4SettingsManager::sharedInstance()->channelCntBA();
        copyData.m_channelCntMerged = DRS4SettingsManager::sharedInstance()->channelCntMerged();

        copyData.m_offsetInNSCoincidence = DRS4SettingsManager::sharedInstance()->offsetInNSCoincidence();
        copyData.m_offsetInNSAB = DRS4SettingsManager::sharedInstance()->offsetInNSAB();
        copyData.m_offsetInNSBA = DRS4SettingsManager::sharedInstance()->offsetInNSBA();
        copyData.m_offsetInNSMerged = DRS4SettingsManager::sharedInstance()->offsetInNSMerged();

        copyData.m_scalerInNSCoincidence = DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence();
        copyData.m_scalerInNSAB = DRS4SettingsManager::sharedInstance()->scalerInNSAB();
        copyData.m_scalerInNSBA = DRS4SettingsManager::sharedInstance()->scalerInNSBA();
        copyData.m_scalerInNSMerged = DRS4SettingsManager::sharedInstance()->scalerInNSMerged();

        copyData.m_meanCableDelay = DRS4SettingsManager::sharedInstance()->meanCableDelay();

        copyData.m_isforceCoincidence = DRS4SettingsManager::sharedInstance()->isforceCoincidence();
        copyData.m_isNegativeLTAccepted = DRS4SettingsManager::sharedInstance()->isNegativeLTAccepted();

        copyData.m_areaFilterASlopeUpper = *m_dataExchange->m_areaFilterASlopeUpper;
        copyData.m_areaFilterAInterceptUpper = *m_dataExchange->m_areaFilterAInterceptUpper;

        copyData.m_areaFilterASlopeLower = *m_dataExchange->m_areaFilterASlopeLower;
        copyData.m_areaFilterAInterceptLower = *m_dataExchange->m_areaFilterAInterceptLower;

        copyData.m_areaFilterBSlopeUpper = *m_dataExchange->m_areaFilterBSlopeUpper;
        copyData.m_areaFilterBInterceptUpper = *m_dataExchange->m_areaFilterBInterceptUpper;

        copyData.m_areaFilterBSlopeLower = *m_dataExchange->m_areaFilterBSlopeLower;
        copyData.m_areaFilterBInterceptLower = *m_dataExchange->m_areaFilterBInterceptLower;

        copyData.m_startCell = DRS4SettingsManager::sharedInstance()->startCell();
        copyData.m_endRange = DRS4SettingsManager::sharedInstance()->stopCell();
        copyData.m_stopCellWidth = (kNumberOfBins - DRS4SettingsManager::sharedInstance()->stopCell());
        copyData.m_cellWidth = (kNumberOfBins - copyData.m_startCell - copyData.m_stopCellWidth);

        copyData.m_dataVecXA.resize(copyData.m_cellWidth);
        copyData.m_dataVecYA.resize(copyData.m_cellWidth);

        copyData.m_dataVecXB.resize(copyData.m_cellWidth);
        copyData.m_dataVecYB.resize(copyData.m_cellWidth);

        //prepare pulse data:
        for ( int a = copyData.m_startCell ; a < copyData.m_endRange ; ++ a ) {
            copyData.m_xMinA = qMin(copyData.m_xMinA, tChannel0[a]);
            copyData.m_xMaxA = qMax(copyData.m_xMaxA, tChannel0[a]);

            copyData.m_xMinB = qMin(copyData.m_xMinB, tChannel1[a]);
            copyData.m_xMaxB = qMax(copyData.m_xMaxB, tChannel1[a]);

            if ( waveChannel0[a] >= copyData.m_yMaxA ) {
                copyData.m_yMaxA = waveChannel0[a];
                copyData.m_cellYAMax = a;
            }

            if ( waveChannel1[a] >= copyData.m_yMaxB ) {
                copyData.m_yMaxB = waveChannel1[a];
                copyData.m_cellYBMax = a;
            }

            if ( waveChannel0[a] <= copyData.m_yMinA ) {
                copyData.m_yMinA = waveChannel0[a];
                copyData.m_cellYAMin = a;
            }

            if ( waveChannel1[a] <= copyData.m_yMinB ) {
                copyData.m_yMinB = waveChannel1[a];
                copyData.m_cellYBMin = a;
            }

            if ( !DRS4SettingsManager::sharedInstance()->isBurstMode() ) {
                /* visualization */
                m_pListChannelA.append(QPointF(tChannel0[a], waveChannel0[a]));
                m_pListChannelB.append(QPointF(tChannel1[a], waveChannel1[a]));
            }

            const int it = (a - copyData.m_startCell);

            /* pulse meta data */
            copyData.m_dataVecXA[it] = tChannel0[a];
            copyData.m_dataVecYA[it] = waveChannel0[a];

            copyData.m_dataVecXB[it] = tChannel1[a];
            copyData.m_dataVecYB[it] = waveChannel1[a];
        }

        copyData.m_yMaxASort = copyData.m_yMaxA;
        copyData.m_yMinASort = copyData.m_yMinA;

        copyData.m_yMaxBSort = copyData.m_yMaxB;
        copyData.m_yMinBSort = copyData.m_yMinB;

        if (copyData.m_cellYAMax == -1
                || copyData.m_cellYAMin == -1
                || copyData.m_cellYBMax == -1
                || copyData.m_cellYBMin == -1
                || qFuzzyCompare(copyData.m_yMinA, copyData.m_yMaxA)
                || qFuzzyCompare(copyData.m_yMinB, copyData.m_yMaxB)
                || copyData.m_dataVecXA.empty()
                || copyData.m_dataVecYA.empty()
                || copyData.m_dataVecXB.empty()
                || copyData.m_dataVecYB.empty())
            continue;

        if ( DRS4StreamManager::sharedInstance()->isArmed() ) {

            if (!DRS4StreamManager::sharedInstance()->write((const char*)tChannel0, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream time(0): size(" + QVariant(sizeOfWave).toString() + ")"));
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)waveChannel0, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream volt(0): size(" + QVariant(sizeOfWave).toString() + ")"));
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)tChannel1, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream time(1): size(" + QVariant(sizeOfWave).toString() + ")"));
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)waveChannel1, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream volt(1): size(" + QVariant(sizeOfWave).toString() + ")"));
            }
        }

        m_copyData.append(copyData);

        if ( m_copyData.size() == pulsePairChunkSize ) {
            m_workerConcurrentManager->add(m_copyData);
            m_copyData.clear();
        }
    } //end forever
}
#endif

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
                }
            }
        }

        while ( !nextSignal() ) {
            m_isBlocking = true;
        }

        m_isBlocking = false;

        if (!bDemoMode) {
            try {
                 // just 2 waves are necessary! Place them next to each other like (Chn1 & Chn2) to reduce sampling!
                DRS4BoardManager::sharedInstance()->currentBoard()->TransferWaves(0, 2);
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


        if (!bDemoMode) {
            int retState = 1;
            int retStateT = 1;
            int retStateV = kSuccess;

            try {
                retStateT = DRS4BoardManager::sharedInstance()->currentBoard()->GetTime(0, 0, DRS4BoardManager::sharedInstance()->currentBoard()->GetTriggerCell(0), inputData.m_tChannel0);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\ttime(0) try-catch"));
                continue;
            }

            if ( retStateT != 1 ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState time(0): " + QVariant(retStateT).toString()));
                continue;
            }

            try {
                retStateT = DRS4BoardManager::sharedInstance()->currentBoard()->GetTime(0, 2 ,DRS4BoardManager::sharedInstance()->currentBoard()->GetTriggerCell(0), inputData.m_tChannel1);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\ttime(1) try-catch"));
                continue;
            }

            if ( retStateT != 1 ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState time(1): " + QVariant(retStateT).toString()));
                continue;
            }

            try {
                retStateV = DRS4BoardManager::sharedInstance()->currentBoard()->GetWave(0, 0, inputData.m_waveChannel0);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\tvolt(0) try-catch"));
                continue;
            }

            if ( retStateV != kSuccess ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState volt(0): " + QVariant(retStateV).toString()));
                continue;
            }

            try {
                retStateV = DRS4BoardManager::sharedInstance()->currentBoard()->GetWave(0, 2, inputData.m_waveChannel1);
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\tvolt(1) try-catch"));
                continue;
            }

            if ( retStateV != kSuccess ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treturnState volt(1): " + QVariant(retStateV).toString()));
                continue;
            }

            try {
                retState = DRS4BoardManager::sharedInstance()->currentBoard()->StartDomino(); // returns always 1.
            }
            catch ( ... ) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\tstart Domino-Wave: try-catch"));
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
                    DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\treadStream: error"));
                    continue;
                }
            }
        }

        /* statistics: */
        time(&stop);
        const double diffTime = difftime(stop, start);
        if ( diffTime >= 1.0f ) {
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

        /* prevent mutex locking: call these functions only once within the loop */


        if ( DRS4StreamManager::sharedInstance()->isArmed() ) {
            if (!DRS4StreamManager::sharedInstance()->write((const char*)inputData.m_tChannel0, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream time(0): size(" + QVariant(sizeOfWave).toString() + ")"));
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)inputData.m_waveChannel0, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream volt(0): size(" + QVariant(sizeOfWave).toString() + ")"));
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)inputData.m_tChannel1, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream time(1): size(" + QVariant(sizeOfWave).toString() + ")"));
            }

            if (!DRS4StreamManager::sharedInstance()->write((const char*)inputData.m_waveChannel1, sizeOfWave)) {
                DRS4BoardManager::sharedInstance()->log(QString(QDateTime::currentDateTime().toString() + "\twriteStream volt(1): size(" + QVariant(sizeOfWave).toString() + ")"));
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

        /* inline CFD cell estimation */
        int estimCFDACellStart = -1;
        int estimCFDBCellStart = -1;

        int estimCFDACellStop = -1;
        int estimCFDBCellStop = -1;

        int cfdACounter = 0;
        int cfdBCounter = 0;

        float areaA = 0.0f;
        float areaB = 0.0f;

        /* reduce mathematical operations within in the loop */
        const int reducedEndRange = (inputData.m_endRange - 1);
        const int reducedCellWidth = (inputData.m_cellWidth - 1);
        const int extendedStartCell = (inputData.m_startCell + 1);

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

                    cfdACounter = 0;
                }
            }

            if ( inputData.m_waveChannel1[a] >= yMaxB ) {
                yMaxB = inputData.m_waveChannel1[a];
                cellYBMax = a;

                if (inputData.m_positiveSignal) {
                    cfdValueB = inputData.m_cfdB*yMaxB;

                    cfdBCounter = 0;
                }
            }

            if ( inputData.m_waveChannel0[a] <= yMinA ) {
                yMinA = inputData.m_waveChannel0[a];
                cellYAMin = a;

                if (!inputData.m_positiveSignal) {
                    cfdValueA = inputData.m_cfdA*yMinA;

                    cfdACounter = 0;
                }
            }

            if ( inputData.m_waveChannel1[a] <= yMinB ) {
                yMinB = inputData.m_waveChannel1[a];
                cellYBMin = a;

                if (!inputData.m_positiveSignal) {
                    cfdValueB = inputData.m_cfdB*yMinB;

                    cfdBCounter = 0;
                }
            }

            /* CFD cell estimation/determination */
            if ( a >= extendedStartCell ) {
                const int aDecr = (a - 1);

                /* calculate the pulse area of ROI */
                if (inputData.m_bPulseAreaPlot
                        || inputData.m_bPulseAreaFilter) {
                    areaA += inputData.m_waveChannel0[aDecr]*(inputData.m_tChannel0[a] - inputData.m_tChannel0[aDecr])*0.5f;
                    areaB += inputData.m_waveChannel1[aDecr]*(inputData.m_tChannel1[a] - inputData.m_tChannel1[aDecr])*0.5f;
                }

                const double slopeA = (inputData.m_waveChannel0[a] - inputData.m_waveChannel0[aDecr])/(inputData.m_tChannel0[a] - inputData.m_tChannel0[aDecr]);
                const double slopeB = (inputData.m_waveChannel1[a] - inputData.m_waveChannel1[aDecr])/(inputData.m_tChannel1[a] - inputData.m_tChannel1[aDecr]);

                /* CFD on rising edge = positive slope? or falling edge = negative slope?*/
                const bool bInRangeA = inputData.m_positiveSignal?(slopeA > 1E-6):(slopeA < 1E-6);
                const bool bInRangeB = inputData.m_positiveSignal?(slopeB > 1E-6):(slopeB < 1E-6);

                if ( bInRangeA ) {
                    const bool cfdLevelInRangeA = inputData.m_positiveSignal?(inputData.m_waveChannel0[a] > cfdValueA && inputData.m_waveChannel0[aDecr] < cfdValueA):(inputData.m_waveChannel0[a] < cfdValueA && inputData.m_waveChannel0[aDecr] > cfdValueA);

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
                }

                if ( bInRangeB ) {
                    const bool cfdLevelInRangeB = inputData.m_positiveSignal?(inputData.m_waveChannel1[a] > cfdValueB && inputData.m_waveChannel1[aDecr] < cfdValueB):(inputData.m_waveChannel1[a] < cfdValueB && inputData.m_waveChannel1[aDecr] > cfdValueB);

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
                }
            }

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

        /* light-weight filtering of wrong pulses: */
        if ( inputData.m_positiveSignal ) {
            if ( (abs(yMinA) > abs(yMaxA))
                 || (abs(yMinB) > abs(yMaxB)) )
                continue;

            if ( abs(cellYAMax - inputData.m_startCell) <= 45
                 || abs(cellYBMax - inputData.m_startCell) <= 45 )
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

            if ( abs(cellYAMin - inputData.m_startCell) <= 45
                 || abs(cellYBMin - inputData.m_startCell) <= 45 )
                continue;

            if ( qFuzzyCompare(inputData.m_waveChannel0[reducedEndRange], yMinA)
                 || qFuzzyCompare(inputData.m_waveChannel1[reducedEndRange], yMinB)
                 || qFuzzyCompare(inputData.m_waveChannel0[inputData.m_startCell], yMinA)
                 || qFuzzyCompare(inputData.m_waveChannel1[inputData.m_startCell], yMinB) )
                continue;
        }

        /* reject artefacts */
        if ( cfdBCounter > 1
             || cfdACounter > 1 )
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
            } else if (inputData.m_bUsingTinoKluge) {
                tkSplineA.setType(SplineType::Cubic);
                tkSplineB.setType(SplineType::Cubic);

                tkSplineA.setPoints(arrayDataTKX_A, arrayDataTKY_A);
                tkSplineB.setPoints(arrayDataTKX_B, arrayDataTKY_B);
            } else if (inputData.m_bUsingLinearInterpol) {
                /* nothing yet */
            }
        }
        else {
            alglib::polynomialbuild(arrayDataX_A, arrayDataY_A, baryCentricInterpolantA);
            alglib::polynomialbuild(arrayDataX_B, arrayDataY_B, baryCentricInterpolantB);
        }

        /* calculate and normalize pulse area for subsequent filtering */
        if (inputData.m_bPulseAreaPlot
                || inputData.m_bPulseAreaFilter) {
            const float rat = 5120*((float)inputData.m_cellWidth/((float)kNumberOfBins));

            areaA = abs(areaA)/(inputData.m_pulseAreaFilterNormA*rat);
            areaB = abs(areaB)/(inputData.m_pulseAreaFilterNormB*rat);
        }

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
                const float valYA = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantA, inputData.m_tChannel0[cell_interpolRangeA_start] + (float)i*renderIncrementA)):(tkSplineA(inputData.m_tChannel0[cell_interpolRangeA_start] + (float)i*renderIncrementA))):(alglib::barycentriccalc(baryCentricInterpolantA, inputData.m_tChannel0[cell_interpolRangeA_start] + (float)i*renderIncrementA));
                const float valYB = (inputData.m_interpolationType == DRS4InterpolationType::type::spline)?(inputData.m_bUsingALGLIB?(alglib::spline1dcalc(interpolantB, inputData.m_tChannel1[cell_interpolRangeB_start] + (float)i*renderIncrementB)):(tkSplineB(inputData.m_tChannel1[cell_interpolRangeB_start] + (float)i*renderIncrementB))):(alglib::barycentriccalc(baryCentricInterpolantB, inputData.m_tChannel1[cell_interpolRangeB_start] + (float)i*renderIncrementB));

                /* modify min/max to calculate the CFD in  high accuracy, subsequently */
                yMaxA = qMax(yMaxA, valYA);
                yMaxB = qMax(yMaxB, valYB);

                yMinA = qMin(yMinA, valYA);
                yMinB = qMin(yMinB, valYB);
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

        /* CFD levels valid? */
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
            /* finding correct CFD timestamp within the estimated CFD bracket index region */
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
                    } else if ( qFuzzyCompare(cfdValueA, valY1) ) {
                        timeStampA = timeStamp1;

                        break;
                    } else if ( qFuzzyCompare(cfdValueA, valY2) ) {
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
                    } else if ( qFuzzyCompare(cfdValueB, valY1) ) {
                        timeStampB = timeStamp1;

                        break;
                    } else if ( qFuzzyCompare(cfdValueB, valY2) ) {
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
                } else if ( qFuzzyCompare(cfdValueA, valY1) ) {
                    timeStampA = timeStamp1;
                } else if ( qFuzzyCompare(cfdValueA, valY2) ) {
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
                } else if ( qFuzzyCompare(cfdValueB, valY1) ) {
                    timeStampB = timeStamp1;
                } else if ( qFuzzyCompare(cfdValueB, valY2) ) {
                    timeStampB = timeStamp2;
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

            const bool y_BInside = (multB >= yLowerB && multB <=yUpperB);

            if ( !y_AInside || !y_BInside )
                continue;
        }

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
                 && binBA < inputData.m_channelCntPrompt )
                outputData.m_lifeTimeDataCoincidence.append(binBA);
        }
    }

    return outputData;
}

#ifdef __DEPRECATED_WORKER
DRS4ConcurrentCopyOutputData runCalculation(const QVector<DRS4ConcurrentCopyInputData> &copyDataVec)
{
    if ( copyDataVec.size() == 0 )
        return DRS4ConcurrentCopyOutputData(0, 0, 0, 0);

    const int channelCntCoincindence = copyDataVec.first().m_channelCntCoincindence;
    const int channelCntAB = copyDataVec.first().m_channelCntAB;
    const int channelCntBA = copyDataVec.first().m_channelCntBA;
    const int channelCntMerged = copyDataVec.first().m_channelCntMerged;

    for ( DRS4ConcurrentCopyInputData copyData : copyDataVec ) {
        if ( channelCntCoincindence != copyData.m_channelCntCoincindence )
            return DRS4ConcurrentCopyOutputData(0, 0, 0, 0);

        if ( channelCntAB != copyData.m_channelCntAB )
            return DRS4ConcurrentCopyOutputData(0, 0, 0, 0);

        if ( channelCntBA != copyData.m_channelCntBA )
            return DRS4ConcurrentCopyOutputData(0, 0, 0, 0);

        if ( channelCntMerged != copyData.m_channelCntMerged )
            return DRS4ConcurrentCopyOutputData(0, 0, 0, 0);
    }

    DRS4ConcurrentCopyOutputData outputData(channelCntCoincindence, channelCntAB, channelCntBA, channelCntMerged, false);

    for ( DRS4ConcurrentCopyInputData copyData : copyDataVec ) {
        const bool bSpline = (copyData.m_cellWidth > copyData.m_numberOfCubicSplinePoints || copyData.m_cellWidth < copyData.m_numberOfCubicSplinePoints)
                && !copyData.m_isLinearInterpolation;

        //default: linear!
        DSpline _splineA, _splineB;

        //cubic spline?
        if ( bSpline ) {
            _splineA.setType(SplineType::Cubic);
            _splineA.setPoints(copyData.m_dataVecXA, copyData.m_dataVecYA);

            _splineB.setType(SplineType::Cubic);
            _splineB.setPoints(copyData.m_dataVecXB, copyData.m_dataVecYB);
        }

        //cubic spline?
        const int cubSplinePointsCnt = copyData.m_numberOfCubicSplinePoints;
        const double fcubSplinePointsCntRatio = 1.0f/((double)cubSplinePointsCnt);
        const double incrA = (copyData.m_xMaxA - copyData.m_xMinA)*fcubSplinePointsCntRatio;
        const double incrB = (copyData.m_xMaxB - copyData.m_xMinB)*fcubSplinePointsCntRatio;

        double areaA = 0;
        double areaB = 0;

        float t0_YMaxA = 0.0f;
        float t0_YMaxB = 0.0f;

        float t0_YMinA = 0.0f;
        float t0_YMinB = 0.0f;

        copyData.m_yMinA = 500.0f;
        copyData.m_yMinB = 500.0f;
        copyData.m_yMaxA = -500.0f;
        copyData.m_yMaxB = -500.0f;

        //cubic spline?
        if ( bSpline ) {
            //subsequent area determination for area-filter:
            for ( int a = 0 ; a < cubSplinePointsCnt ; ++ a ) {
                const double splineXValueA = ((double)a)*incrA+copyData.m_xMinA;
                const double splineXValueB = ((double)a)*incrB+copyData.m_xMinB;

                float valueAY = _splineA(splineXValueA);
                float valueBY = _splineB(splineXValueB);

                if ( valueAY < copyData.m_yMinA ) {
                    copyData.m_yMinA = valueAY;
                    t0_YMinA = splineXValueA;
                }

                if ( valueBY < copyData.m_yMinB ) {
                    copyData.m_yMinB = valueBY;
                    t0_YMinB = splineXValueB;
                }

                if ( valueAY > copyData.m_yMaxA ) {
                    copyData.m_yMaxA = valueAY;
                    t0_YMaxA = splineXValueA;
                }

                if ( valueBY > copyData.m_yMaxB ) {
                    copyData.m_yMaxB = valueBY;
                    t0_YMaxB = splineXValueB;
                }

                if ( a < cubSplinePointsCnt - 1 ) {
                    const double splineXValueANext = ((double)(a+1))*incrA+copyData.m_xMinA;
                    const double splineXValueBNext = ((double)(a+1))*incrB+copyData.m_xMinB;

                    float valueAYNext = _splineA(splineXValueANext);
                    float valueBYNext = _splineB(splineXValueBNext);

                    areaA += valueAYNext*0.5f;
                    areaB += valueBYNext*0.5f;
                }
            }
        }
        else { //linear spline?
            for ( int a = 0 ; a < copyData.m_cellWidth ; ++ a ) {
                const double splineXValueA = copyData.m_dataVecXA.at(a);
                const double splineXValueB = copyData.m_dataVecXB.at(a);

                const double valueAY = copyData.m_dataVecYA.at(a);
                const double valueBY = copyData.m_dataVecYB.at(a);

                if ( valueAY < copyData.m_yMinA ) {
                    copyData.m_yMinA = valueAY;
                    t0_YMinA = splineXValueA;
                }

                if ( valueBY < copyData.m_yMinB ) {
                    copyData.m_yMinB = valueBY;
                    t0_YMinB = splineXValueB;
                }

                if ( valueAY > copyData.m_yMaxA ) {
                    copyData.m_yMaxA = valueAY;
                    t0_YMaxA = splineXValueA;
                }

                if ( valueBY > copyData.m_yMaxB ) {
                    copyData.m_yMaxB = valueBY;
                    t0_YMaxB = splineXValueB;
                }

                const int reducedCellWidth = (copyData.m_cellWidth - 1);
                if ( a < reducedCellWidth ) {
                    const double splineXValueANext = copyData.m_dataVecXA.at(a+1);
                    const double splineXValueBNext = copyData.m_dataVecXB.at(a+1);

                    const double valueAYNext = copyData.m_dataVecYA.at(a+1);
                    const double valueBYNext = copyData.m_dataVecYB.at(a+1);

                    areaA += valueAYNext*(splineXValueANext - splineXValueA)*0.5f;
                    areaB += valueBYNext*(splineXValueBNext - splineXValueB)*0.5f;
                }
            }
        }

        const float rat = 5120*((float)copyData.m_cellWidth/((float)kNumberOfBins));

        areaA = abs(areaA)/(copyData.m_pulseAreaFilterNormalizationA*rat);
        areaB = abs(areaB)/(copyData.m_pulseAreaFilterNormalizationB*rat);

        if ( (int)copyData.m_yMinA == (int)copyData.m_yMaxA
             || (int)copyData.m_yMinB == (int)copyData.m_yMaxB  )
            /*return*/continue;

        //filter wrong pulses:
        if ( copyData.m_isPositiveSignalPolarity ) {
            if ( fabs(copyData.m_yMinASort) > fabs(copyData.m_yMaxASort) )
                /*return*/continue;

            if ( fabs(copyData.m_yMinBSort) > fabs(copyData.m_yMaxBSort) )
                /*return*/continue;

            if ( abs(copyData.m_cellYAMax - copyData.m_startCell) <= 45 )
                /*return*/continue;

            if ( abs(copyData.m_cellYBMax - copyData.m_startCell) <= 45 )
                /*return*/continue;

            if ( qFuzzyCompare(copyData.m_dataVecYA.at(copyData.m_cellWidth-1), (double)copyData.m_yMaxASort) )
                /*return*/continue;

            if ( qFuzzyCompare(copyData.m_dataVecYB.at(copyData.m_cellWidth-1), (double)copyData.m_yMaxBSort) )
                /*return*/continue;

            if ( qFuzzyCompare(copyData.m_dataVecYA.at(0), (double)copyData.m_yMaxASort) )
                /*return*/continue;

            if ( qFuzzyCompare(copyData.m_dataVecYB.at(0), (double)copyData.m_yMaxBSort) )
                /*return*/continue;
        }
        else {
            if ( fabs(copyData.m_yMinASort) < fabs(copyData.m_yMaxASort) )
                /*return*/continue;

            if ( fabs(copyData.m_yMinBSort) < fabs(copyData.m_yMaxBSort) )
                /*return*/continue;

            if ( abs(copyData.m_cellYAMin - copyData.m_startCell) <= 45 )
                /*return*/continue;

            if ( abs(copyData.m_cellYBMin - copyData.m_startCell) <= 45 )
                /*return*/continue;

            if ( qFuzzyCompare(copyData.m_dataVecYA.at(copyData.m_cellWidth-1), (double)copyData.m_yMinASort) )
                /*return*/continue;

            if ( qFuzzyCompare(copyData.m_dataVecYB.at(copyData.m_cellWidth-1), (double)copyData.m_yMinBSort) )
                /*return*/continue;

            if ( qFuzzyCompare(copyData.m_dataVecYA.at(0), (double)copyData.m_yMinASort) )
                /*return*/continue;

            if ( qFuzzyCompare(copyData.m_dataVecYB.at(0), (double)copyData.m_yMinBSort) )
                /*return*/continue;
        }

        int channelYMaxA = 0;
        int channelYMaxB = 0;

        double cfdValueA = 0;
        double cfdValueB = 0;

        if ( copyData.m_isPositiveSignalPolarity ) {
            channelYMaxA = ((int)((copyData.m_yMaxA/(500.0f))*(double)kNumberOfBins))-1;
            channelYMaxB = ((int)((copyData.m_yMaxB/(500.0f))*(double)kNumberOfBins))-1;

            cfdValueA = copyData.m_cfdLevelA*copyData.m_yMaxA;
            cfdValueB = copyData.m_cfdLevelB*copyData.m_yMaxB;

            if ( cfdValueA > 500.0f || qFuzzyCompare(cfdValueA, 0.0) || cfdValueA < 0.0f )
                /*return*/continue;

            if ( cfdValueB > 500.0f || qFuzzyCompare(cfdValueB, 0.0) || cfdValueB < 0.0f )
                /*return*/continue;

            if ( (int)cfdValueA == (int)copyData.m_yMaxA )
                /*return*/continue;

            if ( (int)cfdValueB == (int)copyData.m_yMaxB )
                /*return*/continue;
        }
        else {
            channelYMaxA = ((int)((copyData.m_yMinA/(-500.0f))*(double)kNumberOfBins))-1;
            channelYMaxB = ((int)((copyData.m_yMinB/(-500.0f))*(double)kNumberOfBins))-1;

            cfdValueA = copyData.m_cfdLevelA*copyData.m_yMinA;
            cfdValueB = copyData.m_cfdLevelB*copyData.m_yMinB;

            if ( cfdValueA < -500.0f || qFuzzyCompare(cfdValueA, 0.0) || cfdValueA > 0.0f )
                /*return*/continue;

            if ( cfdValueB < -500.0f || qFuzzyCompare(cfdValueB, 0.0) || cfdValueB > 0.0f )
                /*return*/continue;

            if ( (int)cfdValueA == (int)copyData.m_yMinA )
                /*return*/continue;

            if ( (int)cfdValueB == (int)copyData.m_yMinB )
                /*return*/continue;
        }

        //PHS:
        if ( channelYMaxA < kNumberOfBins && channelYMaxA >= 0 ) {
            outputData.m_phsA.append(channelYMaxA); /* outputData.m_phsA[channelYMaxA] ++; */
            /* outputData.m_phsACounts ++; */
        }

        if ( channelYMaxB < kNumberOfBins && channelYMaxB >= 0 ) {
            outputData.m_phsB.append(channelYMaxB); /* outputData.m_phsB[channelYMaxB] ++; */
            /* outputData.m_phsBCounts ++; */
        }

        DRS4FilterData dataF;

        /* Area-Filter */
        dataF.areaARatio = areaA;
        dataF.areaBRatio = areaB;
        dataF.amplitudeAInMV = copyData.m_isPositiveSignalPolarity?copyData.m_yMaxA:abs(copyData.m_yMinA);
        dataF.amplitudeBInMV = copyData.m_isPositiveSignalPolarity?copyData.m_yMaxB:abs(copyData.m_yMinB);

        double timeStampA = -1;
        double timeStampB = -1;

        bool btimeStampA = false;
        bool btimeStampB = false;

        bool bReject = false;
        int cntTimeStampA = 0;
        int cntTimeStampB = 0;

        //cubic spline?
        if ( bSpline ) {
            const int reducedSplinePoints = (cubSplinePointsCnt - 1);
            for ( int a = 0 ; a <= reducedSplinePoints ; ++ a ) {
                const double splineXValueA_1 = ((double)a)*incrA+copyData.m_xMinA;
                const double splineXValueB_1 = ((double)a)*incrB+copyData.m_xMinB;

                const double splineXValueA_2 = ((double)(a+1))*incrA+copyData.m_xMinA;
                const double splineXValueB_2 = ((double)(a+1))*incrB+copyData.m_xMinB;

                const double valueAY_1 = _splineA(splineXValueA_1);
                const double valueBY_1 = _splineB(splineXValueB_1);

                const double valueAY_2 = _splineA(splineXValueA_2);
                const double valueBY_2 = _splineB(splineXValueB_2);

                if ( copyData.m_isPositiveSignalPolarity ) {
                    if ( ((cfdValueA > valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA < valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2)))
                         || ((cfdValueA < valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA > valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2))) ) {

                        if ( qFuzzyCompare(cfdValueA, valueAY_2) )
                            continue;

                        if ( cntTimeStampA == 0 ) {
                            const double slopeA = (valueAY_2 - valueAY_1)/(splineXValueA_2 - splineXValueA_1);
                            const double intersectA = valueAY_2 - slopeA*splineXValueA_2;

                            timeStampA = (cfdValueA - intersectA)/slopeA;

                            btimeStampA = true;
                        }

                        cntTimeStampA ++;
                    }

                    if ( ((cfdValueB > valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB < valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2)))
                         || ((cfdValueB < valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB > valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2))) ) {

                        if ( qFuzzyCompare(cfdValueB, valueBY_2) )
                            continue;

                        if ( cntTimeStampB == 0 ) {
                            const double slopeB = (valueBY_2 - valueBY_1)/(splineXValueB_2 - splineXValueB_1);
                            const double intersectB = valueBY_2 - slopeB*splineXValueB_2;

                            timeStampB = (cfdValueB - intersectB)/slopeB;

                            btimeStampB = true;
                        }

                        cntTimeStampB ++;
                    }

                    if ( cntTimeStampA > 2 || cntTimeStampB > 2 ) {
                        bReject = true;
                    }
                }
                else {
                    if ( ((cfdValueA < valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA > valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2)))
                         || ((cfdValueA > valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA < valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2))) ) {

                        if ( qFuzzyCompare(cfdValueA, valueAY_2) )
                            continue;

                        if ( cntTimeStampA == 0 ) {
                            const double slopeA = (valueAY_2 - valueAY_1)/(splineXValueA_2 - splineXValueA_1);
                            const double intersectA = valueAY_2 - slopeA*splineXValueA_2;

                            timeStampA = (cfdValueA - intersectA)/slopeA;

                            btimeStampA = true;
                        }

                        cntTimeStampA ++;
                    }

                    if ( ((cfdValueB < valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB > valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2)))
                         || ((cfdValueB > valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB < valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2))) ) {

                        if ( qFuzzyCompare(cfdValueB, valueBY_2) )
                            continue;

                        if ( cntTimeStampB == 0 ) {
                            const double slopeB = (valueBY_2 - valueBY_1)/(splineXValueB_2 - splineXValueB_1);
                            const double intersectB = valueBY_2 - slopeB*splineXValueB_2;

                            timeStampB = (cfdValueB - intersectB)/slopeB;

                            btimeStampB = true;
                        }

                        cntTimeStampB ++;
                    }

                    if ( cntTimeStampA > 2 || cntTimeStampB > 2 ) {
                        bReject = true;
                    }
                }

                if (bReject) {
                    break;
                }
            }

            if ( cntTimeStampA <= 1 || cntTimeStampB <= 1 ) {
                /* bReject = true; */
                /*return*/continue;
            }

            if (bReject) {
                /*return*/continue;
            }
        }
        else { //linear spline?
            const int reducedCellWidth = (copyData.m_cellWidth - 1);
            for ( int a = 0 ; a < reducedCellWidth ; ++ a ) {
                const double splineXValueA_1 = copyData.m_dataVecXA.at(a);
                const double splineXValueB_1 = copyData.m_dataVecXB.at(a);

                const double splineXValueA_2 = copyData.m_dataVecXA.at(a+1);
                const double splineXValueB_2 = copyData.m_dataVecXB.at(a+1);

                const double valueAY_1 = copyData.m_dataVecYA.at(a);
                const double valueBY_1 = copyData.m_dataVecYB.at(a);

                const double valueAY_2 = copyData.m_dataVecYA.at(a+1);
                const double valueBY_2 = copyData.m_dataVecYB.at(a+1);

                if ( copyData.m_isPositiveSignalPolarity ) {
                    if ( ((cfdValueA > valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA < valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2)))
                         || ((cfdValueA < valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA > valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2))) ) {

                        if ( qFuzzyCompare(cfdValueA, valueAY_2) )
                            continue;

                        if ( cntTimeStampA == 0 ) {
                            const double slopeA = (valueAY_2 - valueAY_1)/(splineXValueA_2 - splineXValueA_1);
                            const double intersectA = valueAY_2 - slopeA*splineXValueA_2;

                            timeStampA = (cfdValueA - intersectA)/slopeA;

                            btimeStampA = true;
                        }

                        cntTimeStampA ++;
                    }

                    if ( ((cfdValueB > valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB < valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2)))
                         || ((cfdValueB < valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB > valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2))) ) {

                        if ( qFuzzyCompare(cfdValueB, valueBY_2) )
                            continue;

                        if ( cntTimeStampB == 0 ) {
                            const double slopeB = (valueBY_2 - valueBY_1)/(splineXValueB_2 - splineXValueB_1);
                            const double intersectB = valueBY_2 - slopeB*splineXValueB_2;

                            timeStampB = (cfdValueB - intersectB)/slopeB;

                            btimeStampB = true;
                        }

                        cntTimeStampB ++;
                    }

                    if ( cntTimeStampA > 2 || cntTimeStampB > 2) {
                        bReject = true;
                    }
                }
                else {
                    if ( ((cfdValueA < valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA > valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2)))
                         || ((cfdValueA > valueAY_1 || qFuzzyCompare(cfdValueA, valueAY_1)) && (cfdValueA < valueAY_2 || qFuzzyCompare(cfdValueA, valueAY_2))) ) {

                        if ( qFuzzyCompare(cfdValueA, valueAY_2) )
                            continue;

                        if ( cntTimeStampA == 0 ) {
                            const double slopeA = (valueAY_2 - valueAY_1)/(splineXValueA_2 - splineXValueA_1);
                            const double intersectA = valueAY_2 - slopeA*splineXValueA_2;

                            timeStampA = (cfdValueA - intersectA)/slopeA;

                            btimeStampA = true;
                        }

                        cntTimeStampA ++;
                    }

                    if ( ((cfdValueB < valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB > valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2)))
                         || ((cfdValueB > valueBY_1 || qFuzzyCompare(cfdValueB, valueBY_1)) && (cfdValueB < valueBY_2 || qFuzzyCompare(cfdValueB, valueBY_2))) ) {

                        if ( qFuzzyCompare(cfdValueB, valueBY_2) )
                            continue;

                        if ( cntTimeStampB == 0 ) {
                            const double slopeB = (valueBY_2 - valueBY_1)/(splineXValueB_2 - splineXValueB_1);
                            const double intersectB = valueBY_2 - slopeB*splineXValueB_2;

                            timeStampB = (cfdValueB - intersectB)/slopeB;

                            btimeStampB = true;
                        }

                        cntTimeStampB ++;
                    }

                    if ( cntTimeStampA > 2 || cntTimeStampB > 2 ) {
                        bReject = true;
                    }
                }

                if (bReject) {
                    break;
                }
            }

            if ( cntTimeStampA <= 1 || cntTimeStampB <= 1 ) {
                /* bReject = true; */
                /*return*/continue;
            }

            if (bReject) {
                /*return*/continue;
            }
        }

        if ( bReject || !btimeStampA || !btimeStampB ) {
            /*return*/continue;
        }

        DRS4LifetimeData ltData;

        /* lifetime-data */
        if ( copyData.m_isPositiveSignalPolarity ) {
            ltData.amplitudeAInMV = copyData.m_yMaxA;
            ltData.amplitudeBInMV = copyData.m_yMaxB;
        }
        else {
            ltData.amplitudeAInMV = copyData.m_yMinA;
            ltData.amplitudeBInMV = copyData.m_yMinB;
        }

        ltData.tAInNS = timeStampA;
        ltData.tBInNS = timeStampB;

        ltData.areaARatio = areaA;
        ltData.areaBRatio = areaB;

        /* area-Filter */
        if (!copyData.m_usingBurstMode) {
            if (copyData.m_isPulseAreaFilterPlotEnabled) {
                const int indexPHSA = (dataF.amplitudeAInMV/500.0f)*kNumberOfBins;
                const int indexPHSB = (dataF.amplitudeBInMV/500.0f)*kNumberOfBins;

                const int binA = dataF.areaARatio*(double)copyData.m_pulseAreaFilterBinningA;
                const int binB = dataF.areaBRatio*(double)copyData.m_pulseAreaFilterBinningB;

                if (!(indexPHSA>(kNumberOfBins-1)
                      || indexPHSB>(kNumberOfBins-1)
                      || indexPHSA<0
                      || indexPHSB<0
                      || binA < 0
                      || binB < 0)) {
                    outputData.m_areaFilterDataA.append(QPointF(indexPHSA, binA));
                    outputData.m_areaFilterDataB.append(QPointF(indexPHSB, binB));
                }
            }
        }

        int convertToBinA = -1;
        int convertToBinB = -1;

        //rising edge:
        if ( copyData.m_isPositiveSignalPolarity ) {
            convertToBinA = ((int)((ltData.amplitudeAInMV/(500.0f))*kNumberOfBins))-1;
            convertToBinB = ((int)((ltData.amplitudeBInMV/(500.0f))*kNumberOfBins))-1;
        }
        //falling edge:
        else {
            convertToBinA = ((int)((ltData.amplitudeAInMV/(-500.0f))*kNumberOfBins))-1;
            convertToBinB = ((int)((ltData.amplitudeBInMV/(-500.0f))*kNumberOfBins))-1;
        }

        if ( convertToBinA == -1
             || convertToBinB == -1 )
            /*return*/continue;

        if ( convertToBinA >= kNumberOfBins || convertToBinA < 0 )
            /*return*/continue;

        if ( convertToBinB >= kNumberOfBins || convertToBinB < 0 )
            /*return*/continue;

        bool bIsStart_A = false;
        bool bIsStop_A = false;

        bool bIsStart_B = false;
        bool bIsStop_B = false;

        if ( convertToBinA >= copyData.m_startMinA
             && convertToBinA <= copyData.m_startMaxA )
            bIsStart_A = true;

        if ( convertToBinA >= copyData.m_stopMinA
             && convertToBinA <= copyData.m_stopMaxA )
            bIsStop_A = true;

        if ( convertToBinB >= copyData.m_startMinB
             && convertToBinB <= copyData.m_startMaxB )
            bIsStart_B = true;

        if ( convertToBinB >= copyData.m_stopMinB
             && convertToBinB <= copyData.m_stopMaxB )
            bIsStop_B = true;

        //apply area-filter:
        if ( copyData.m_isPulseAreaFilterEnabled ) {
            const double indexPHSA = (abs(ltData.amplitudeAInMV)/500.0f)*1024.0f;
            const double indexPHSB = (abs(ltData.amplitudeBInMV)/500.0f)*1024.0f;

            const double yLowerA = copyData.m_areaFilterASlopeLower*indexPHSA+copyData.m_areaFilterAInterceptLower;
            const double yUpperA = copyData.m_areaFilterASlopeUpper*indexPHSA+copyData.m_areaFilterAInterceptUpper;

            const bool y_AInside = (ltData.areaARatio*copyData.m_pulseAreaFilterBinningA >= yLowerA
                                    && ltData.areaARatio*copyData.m_pulseAreaFilterBinningA <=yUpperA);

            const double yLowerB = copyData.m_areaFilterBSlopeLower*indexPHSB+copyData.m_areaFilterBInterceptLower;
            const double yUpperB = copyData.m_areaFilterBSlopeUpper*indexPHSB+copyData.m_areaFilterBInterceptUpper;

            const bool y_BInside = (ltData.areaBRatio*copyData.m_pulseAreaFilterBinningB >= yLowerB
                                    && ltData.areaBRatio*copyData.m_pulseAreaFilterBinningB <=yUpperB);

            if ( !y_AInside
                 || !y_BInside )
                /*return*/continue;
        }

        if ( bIsStart_A
             && bIsStop_B && !copyData.m_isforceCoincidence  ) //A-B (Master)
        {
            const double ltdiff = (ltData.tBInNS - ltData.tAInNS);
            const int binAB = ((int)round(((((ltdiff)+copyData.m_offsetInNSAB)/copyData.m_scalerInNSAB))*((double)copyData.m_channelCntAB)))-1;

            const int binMerged = ((int)round(((((ltdiff+copyData.m_meanCableDelay)+copyData.m_offsetInNSMerged)/copyData.m_scalerInNSMerged))*((double)copyData.m_channelCntMerged)))-1;

            if ( binAB < 0 || binAB >= copyData.m_channelCntAB )
                /*return*/continue;

            if ( binAB >= 0
                 && binAB < copyData.m_channelCntAB )
            {
                if ( copyData.m_isNegativeLTAccepted && ltdiff < 0  )
                {
                    outputData.m_lifeTimeDataAB.append(binAB); /* outputData.m_lifeTimeDataAB[binAB] ++; */
                    /* outputData.m_abCounts ++; */

                    /* outputData.m_maxY_ABSpectrum = qMax(outputData.m_maxY_ABSpectrum, outputData.m_lifeTimeDataAB[binAB]); */
                }
                else if ( ltdiff >= 0 )
                {
                    outputData.m_lifeTimeDataAB.append(binAB); /* outputData.m_lifeTimeDataAB[binAB] ++; */
                    /* outputData.m_abCounts ++; */

                    /* outputData.m_maxY_ABSpectrum = qMax(outputData.m_maxY_ABSpectrum, outputData.m_lifeTimeDataAB[binAB]); */
                }

                /* outputData.m_specABCounterCnt ++; */
            }

            if ( binMerged < 0 || binMerged >= outputData.m_channelCntMerged )
                /*return*/ continue;

            if ( binMerged >= 0
                 && binMerged < copyData.m_channelCntMerged )
            {
                if ( copyData.m_isNegativeLTAccepted && ltdiff < 0  )
                {
                    outputData.m_lifeTimeDataMerged.append(binMerged); /* outputData.m_lifeTimeDataMerged[binMerged] ++; */
                    /* outputData.m_mergedCounts ++; */

                    /* outputData.m_maxY_MergedSpectrum = qMax(outputData.m_maxY_MergedSpectrum, outputData.m_lifeTimeDataMerged[binMerged]); */
                }
                else if ( ltdiff >= 0 )
                {
                    outputData.m_lifeTimeDataMerged.append(binMerged); /* outputData.m_lifeTimeDataMerged[binMerged] ++; */
                    /* outputData.m_mergedCounts ++; */

                    /* outputData.m_maxY_MergedSpectrum = qMax(outputData.m_maxY_MergedSpectrum, outputData.m_lifeTimeDataMerged[binMerged]); */
                }

                /* outputData.m_specMergedCounterCnt ++; */
            }
        }
        else if (  bIsStart_B
                   && bIsStop_A && !copyData.m_isforceCoincidence ) // B-A (Master)
        {
            const double ltdiff = (ltData.tAInNS - ltData.tBInNS);
            const int binBA = ((int)round(((((ltdiff)+copyData.m_offsetInNSBA)/copyData.m_scalerInNSBA))*((double)copyData.m_channelCntBA)))-1;

            const int binMerged = ((int)round(((((ltdiff-copyData.m_meanCableDelay)+copyData.m_offsetInNSMerged)/copyData.m_scalerInNSMerged))*((double)copyData.m_channelCntMerged)))-1;

            if ( binBA < 0 || binBA >= copyData.m_channelCntBA )
                /*return*/continue;

            if ( binBA >= 0
                 && binBA < copyData.m_channelCntBA )
            {
                if ( copyData.m_isNegativeLTAccepted && ltdiff < 0  )
                {
                    outputData.m_lifeTimeDataBA.append(binBA); /* outputData.m_lifeTimeDataBA[binBA] ++; */
                    /* outputData.m_baCounts ++; */

                    /* outputData.m_maxY_BASpectrum = qMax(outputData.m_maxY_BASpectrum, outputData.m_lifeTimeDataBA[binBA]); */
                }
                else if ( ltdiff >= 0 )
                {
                    outputData.m_lifeTimeDataBA.append(binBA); /* outputData.m_lifeTimeDataBA[binBA] ++; */
                    /* outputData.m_baCounts ++; */

                    /* outputData.m_maxY_BASpectrum = qMax(outputData.m_maxY_BASpectrum, outputData.m_lifeTimeDataBA[binBA]); */
                }

                /* outputData.m_specBACounterCnt ++; */
            }

            if ( binMerged < 0 || binMerged >= copyData.m_channelCntMerged )
                /*return*/ continue;

            if ( binMerged >= 0
                 && binMerged < copyData.m_channelCntMerged )
            {
                if ( copyData.m_isNegativeLTAccepted && ltdiff < 0  )
                {
                    outputData.m_lifeTimeDataMerged.append(binMerged); /* outputData.m_lifeTimeDataMerged[binMerged] ++; */
                    /* outputData.m_mergedCounts ++; */

                    /* outputData.m_maxY_MergedSpectrum = qMax(outputData.m_maxY_MergedSpectrum, outputData.m_lifeTimeDataMerged[binMerged]); */
                }
                else if ( ltdiff >= 0 )
                {
                    outputData.m_lifeTimeDataMerged.append(binMerged); /* outputData.m_lifeTimeDataMerged[binMerged] ++; */
                    /* outputData.m_mergedCounts ++; */

                    /* outputData.m_maxY_MergedSpectrum = qMax(outputData.m_maxY_MergedSpectrum, outputData.m_lifeTimeDataMerged[binMerged]); */
                }

                /* outputData.m_specMergedCounterCnt ++; */
            }
        }
        else if (  bIsStop_B && bIsStop_A ) //Prompt (Stop A - Stop B) (Slave)
        {
            const double ltdiff = (ltData.tAInNS - ltData.tBInNS);
            const int binBA = ((int)round(((((ltdiff)+copyData.m_offsetInNSCoincidence)/copyData.m_scalerInNSCoincidence))*((double)copyData.m_channelCntCoincindence)))-1;

            if ( binBA < 0 || binBA >= copyData.m_channelCntCoincindence )
                /*return*/ continue;

            if ( binBA >= 0
                 && binBA < copyData.m_channelCntCoincindence )
            {
                outputData.m_lifeTimeDataCoincidence.append(binBA); /* outputData.m_lifeTimeDataCoincidence[binBA] ++; */
                /* outputData.m_coincidenceCounts ++; */

                /* outputData.m_maxY_CoincidenceSpectrum = qMax(outputData.m_maxY_CoincidenceSpectrum, outputData.m_lifeTimeDataCoincidence[binBA]); */

                /* outputData.m_specCoincidenceCounterCnt ++; */
            }
        }
    }

    return outputData;
}
#endif

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
