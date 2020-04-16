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
*****************************************************************************/

#ifndef DDRS4SCRIPTINGENGINEACCESSMANAGER_H
#define DDRS4SCRIPTINGENGINEACCESSMANAGER_H

#include <QMutex>
#include <QMutexLocker>

#include "DLib.h"

#include "GUI/drs4scopedlg.h"
#include "drs4simulationsettingsmanager.h"

class DRS4ScriptingEngineAccessManager
{
    friend class DRS4ScopeDlg;

    DRS4ScopeDlg *m_dlgAccess;

    DRS4ScriptingEngineAccessManager();
    virtual ~DRS4ScriptingEngineAccessManager();

    mutable QMutex m_mutex;

public:
    static DRS4ScriptingEngineAccessManager *sharedInstance();

    void setDialogAccess(DRS4ScopeDlg *ptr);

    qint64 integralCountsAB() const;
    qint64 integralCountsBA() const;
    qint64 integralCountsMerged() const;
    qint64 integralCountsCoincidence() const;

    bool isAcquisitionStopped();
    bool isAcquisitionRunning();

    int getLastFitStateOfABSpectrum();
    int getLastFitStateOfBASpectrum();
    int getLastFitStateOfMergedSpectrum();
    int getLastFitStateOfCoincidenceSpectrum();

    double getT0OfABSpectrumInPicoseconds();
    double getT0OfBASpectrumInPicoseconds();
    double getT0OfMergedSpectrumInPicoseconds();
    double getT0OfCoincidenceSpectrumInPicoseconds();

    double getFWHMOfABSpectrumInPicoseconds();
    double getFWHMOfBASpectrumInPicoseconds();
    double getFWHMOfMergedSpectrumInPicoseconds();
    double getFWHMOfCoincidenceSpectrumInPicoseconds();

    double getBoardTemperatureInDegree();

    int getStartCell();
    int getStopCell();

    QString getCurrentSettingsFile();

    int getTriggerLevelAInMillivolt();
    int getTriggerLevelBInMillivolt();

    int getTriggerDelayInNanoseconds();

    double getCFDLevelA();
    double getCFDLevelB();

    double getATSInPicoseconds();

    int getCoincidenceFitIterations();
    int getABFitIterations();
    int getBAFitIterations();
    int getMergedFitIterations();

    bool isPositiveTriggerPolarity();
    bool isPositiveSignal();

    void stop();
    void start();

    void resetPHSA();
    void resetPHSB();

    void resetTemperatureProfile();

    void resetAreaPlotA();
    void resetAreaPlotB();

    void resetAllSpectra();

    void resetABSpectra();
    void resetBASpectra();
    void resetCoincidenceSpectra();
    void resetMergedSpectra();

    void changePHSStartMinA(int value);
    void changePHSStartMaxA(int value);
    void changePHSStopMinA(int value);
    void changePHSStopMaxA(int value);

    void changePHSStartMinB(int value);
    void changePHSStartMaxB(int value);
    void changePHSStopMinB(int value);
    void changePHSStopMaxB(int value);

    void changeTriggerLevelA(int levelInMilliVolt);
    void changeTriggerLevelB(int levelInMilliVolt);

    void changeCFDLevelA(double percentage);
    void changeCFDLevelB(double percentage);

    void changeTriggerDelayInNanoseconds(int delay);

    void changeStartCell(int cell);
    void changeStopCell(int cell);

    void changeATSInPicoseconds(double ps);

    void changeCoincidenceDataMaxFitIterations(unsigned int iterations);
    void changeMergedDataMaxFitIterations(unsigned int iterations);
    void changeABDataMaxFitIterations(unsigned int iterations);
    void changeBADataMaxFitIterations(unsigned int iterations);

    void changePulseAreaFilterEnabled(bool enabled);

    void changeBurstMode(bool burstMode);
    bool isBurstModeRunning();

    bool waitForCountsAB(int counts);
    bool waitForCountsBA(int counts);
    bool waitForCountsMerged(int counts);
    bool waitForCountsPrompt(int counts);

    void changeUsingPositiveTriggerPolarity(bool positive);
    void changeToHavingPositiveSignal(bool positive);

    void fitGaussianOnCoincidenceData();
    void fitGaussianOnMergedData();
    void fitGaussianOnABData();
    void fitGaussianOnBAData();

    bool loadSimulationInputFile(const QString& path);

    bool saveSettingsFile(const QString& path);
    bool loadSettingsFile(const QString& path);

    bool saveTemperatureProfile(const QString& fileName);

    bool loadDataStreamFile(const QString& fileName);
    bool stopLoadingFromDataStreamFile();

    bool isRunningFromDataStream();

    bool startDataStreaming(const QString& fileName);
    bool stopDataStreaming();

    bool isDataStreamArmed();

    bool saveDataAB(const QString& path);
    bool saveDataBA(const QString& path);
    bool saveDataMerged(const QString& path);
    bool saveDataCoincidence(const QString& path);

    bool savePHSA(const QString& fileName);
    bool savePHSB(const QString& fileName);
};

#endif // DDRS4SCRIPTINGENGINEACCESSMANAGER_H
