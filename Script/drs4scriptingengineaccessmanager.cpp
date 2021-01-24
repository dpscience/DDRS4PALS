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

#include "drs4scriptingengineaccessmanager.h"

DRS4ScriptingEngineAccessManager *__sharedInstanceScriptingEngineAccessManager = DNULLPTR;

DRS4ScriptingEngineAccessManager *DRS4ScriptingEngineAccessManager::sharedInstance()
{
    if ( !__sharedInstanceScriptingEngineAccessManager )
        __sharedInstanceScriptingEngineAccessManager = new DRS4ScriptingEngineAccessManager();

    return __sharedInstanceScriptingEngineAccessManager;
}

void DRS4ScriptingEngineAccessManager::setDialogAccess(DRS4ScopeDlg *ptr)
{
    if ( !ptr )
        return;

    m_dlgAccess = ptr;
}

qint64 DRS4ScriptingEngineAccessManager::integralCountsAB() const
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0;

    return m_dlgAccess->countsOfABSpectrum();
}

qint64 DRS4ScriptingEngineAccessManager::integralCountsBA() const
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0;

    return m_dlgAccess->countsOfBASpectrum();
}

qint64 DRS4ScriptingEngineAccessManager::integralCountsMerged() const
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0;

    return m_dlgAccess->countsOfMergedSpectrum();
}

qint64 DRS4ScriptingEngineAccessManager::integralCountsCoincidence() const
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0;

    return m_dlgAccess->countsOfCoincidenceSpectrum();
}

bool DRS4ScriptingEngineAccessManager::isAcquisitionStopped()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->isAcquisitionStopped();
}

bool DRS4ScriptingEngineAccessManager::isAcquisitionRunning()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->isAcquisitionRunning();
}

int DRS4ScriptingEngineAccessManager::getLastFitStateOfABSpectrum()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return -1;

    return m_dlgAccess->lastFitResultAB();
}

int DRS4ScriptingEngineAccessManager::getLastFitStateOfBASpectrum()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return -1;

    return m_dlgAccess->lastFitResultBA();
}

int DRS4ScriptingEngineAccessManager::getLastFitStateOfMergedSpectrum()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return -1;

    return m_dlgAccess->lastFitResultMerged();
}

int DRS4ScriptingEngineAccessManager::getLastFitStateOfCoincidenceSpectrum()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return -1;

    return m_dlgAccess->lastFitResultCoincidence();
}

double DRS4ScriptingEngineAccessManager::getT0OfABSpectrumInPicoseconds()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0.0f;

    return m_dlgAccess->lastt0ABInPs();
}

double DRS4ScriptingEngineAccessManager::getT0OfBASpectrumInPicoseconds()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0.0f;

    return m_dlgAccess->lastt0BAInPs();
}

double DRS4ScriptingEngineAccessManager::getT0OfMergedSpectrumInPicoseconds()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0.0f;

    return m_dlgAccess->lastt0MergedInPs();
}

double DRS4ScriptingEngineAccessManager::getT0OfCoincidenceSpectrumInPicoseconds()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0.0f;

    return m_dlgAccess->lastt0CoincidenceInPs();
}

double DRS4ScriptingEngineAccessManager::getFWHMOfABSpectrumInPicoseconds()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0.0f;

    return m_dlgAccess->lastFWHMABInPs();
}

double DRS4ScriptingEngineAccessManager::getFWHMOfBASpectrumInPicoseconds()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0.0f;

    return m_dlgAccess->lastFWHMBAInPs();
}

double DRS4ScriptingEngineAccessManager::getFWHMOfMergedSpectrumInPicoseconds()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0.0f;

    return m_dlgAccess->lastFWHMMergedInPs();
}

double DRS4ScriptingEngineAccessManager::getFWHMOfCoincidenceSpectrumInPicoseconds()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0.0f;

    return m_dlgAccess->lastFWHMCoincidenceInPs();
}

double DRS4ScriptingEngineAccessManager::getBoardTemperatureInDegree()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0.0f;

    return m_dlgAccess->lastBoardTemperature();
}

int DRS4ScriptingEngineAccessManager::getStartCell()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return -1;

    return m_dlgAccess->startCell();
}

int DRS4ScriptingEngineAccessManager::getStopCell()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return -1;

    return m_dlgAccess->stopCell();
}

QString DRS4ScriptingEngineAccessManager::getCurrentSettingsFile()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return "";

    if ( m_dlgAccess->currentSettingsFile() == NO_SETTINGS_FILE_PLACEHOLDER )
        return "";

    return m_dlgAccess->currentSettingsFile();
}

int DRS4ScriptingEngineAccessManager::getTriggerLevelAInMillivolt()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0;

    return m_dlgAccess->triggerLevelAInMillivolt();
}

int DRS4ScriptingEngineAccessManager::getTriggerLevelBInMillivolt()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0;

    return m_dlgAccess->triggerLevelBInMillivolt();
}

int DRS4ScriptingEngineAccessManager::getTriggerDelayInNanoseconds()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0;

    return m_dlgAccess->triggerDelayInNanoseconds();
}

double DRS4ScriptingEngineAccessManager::getCFDLevelA()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0.0f;

    return m_dlgAccess->CFDLevelA();
}

double DRS4ScriptingEngineAccessManager::getCFDLevelB()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0.0f;

    return m_dlgAccess->CFDLevelB();
}

double DRS4ScriptingEngineAccessManager::getATSInPicoseconds()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0.0f;

    return m_dlgAccess->meanCableDelayInPicoseconds();
}

int DRS4ScriptingEngineAccessManager::getCoincidenceFitIterations()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0;

    return m_dlgAccess->coincidenceFitIterations();
}

int DRS4ScriptingEngineAccessManager::getABFitIterations()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0;

    return m_dlgAccess->ABFitIterations();
}

int DRS4ScriptingEngineAccessManager::getBAFitIterations()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0;

    return m_dlgAccess->BAFitIterations();
}

int DRS4ScriptingEngineAccessManager::getMergedFitIterations()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return 0;

    return m_dlgAccess->mergedFitIterations();
}

bool DRS4ScriptingEngineAccessManager::isPositiveTriggerPolarity()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->isPositivTriggerPolarity();
}

bool DRS4ScriptingEngineAccessManager::isPositiveSignal()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->isPositiveSignal();
}

void DRS4ScriptingEngineAccessManager::stop()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->stopAcquisition(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::start()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->startAcquisition(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::resetPHSA()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->resetPHSA(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::resetPHSB()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->resetPHSB(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::resetAreaPlotA()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->resetAreaPlotA(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::resetAreaPlotB()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->resetAreaPlotB(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::resetAllSpectra()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->resetAllLTSpectra(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::resetABSpectra()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->resetLTSpectrumAB(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::resetBASpectra()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->resetLTSpectrumBA(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::resetCoincidenceSpectra()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->resetLTSpectrumCoincidence(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::resetMergedSpectra()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->resetLTSpectrumMerged(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changePHSStartMinA(int value)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changePHSStartMinA(value, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changePHSStartMaxA(int value)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changePHSStartMaxA(value, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changePHSStopMinA(int value)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changePHSStopMinA(value, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changePHSStopMaxA(int value)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changePHSStopMaxA(value, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changePHSStartMinB(int value)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changePHSStartMinB(value, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changePHSStartMaxB(int value)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changePHSStartMaxB(value, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changePHSStopMinB(int value)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changePHSStopMinB(value, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changePHSStopMaxB(int value)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changePHSStopMaxB(value, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeTriggerLevelA(int levelInMilliVolt)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeTriggerLevelA(levelInMilliVolt, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeTriggerLevelB(int levelInMilliVolt)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeTriggerLevelB(levelInMilliVolt, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeCFDLevelA(double percentage)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeCFDLevelA(percentage, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeCFDLevelB(double percentage)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeCFDLevelB(percentage, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeTriggerDelayInNanoseconds(int delay)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeTriggerDelay(delay, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeStartCell(int cell)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeStartCell(cell, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeStopCell(int cell)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeStopCell(cell, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeATSInPicoseconds(double ps)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeMeanCableDelayPs(ps, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeCoincidenceDataMaxFitIterations(unsigned int iterations)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeCoincidenceFitIterations(iterations, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeMergedDataMaxFitIterations(unsigned int iterations)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeMergedFitIterations(iterations, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeABDataMaxFitIterations(unsigned int iterations)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeABFitIterations(iterations, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeBADataMaxFitIterations(unsigned int iterations)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeBAFitIterations(iterations, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changePulseAreaFilterEnabled(bool enabled)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changePulseAreaFilterEnabled(enabled, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeBurstMode(bool burstMode)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changeBurstModeEnabled(burstMode, FunctionSource::AccessFromScript);
}

bool DRS4ScriptingEngineAccessManager::isBurstModeRunning()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->isBurstModeRunning();
}

bool DRS4ScriptingEngineAccessManager::waitForCountsAB(int counts)
{
    if ( !m_dlgAccess )
        return false;

    while (m_dlgAccess->countsOfABSpectrum() < counts) {}

    return true;
}

bool DRS4ScriptingEngineAccessManager::waitForCountsBA(int counts)
{
    if ( !m_dlgAccess )
        return false;

    while (m_dlgAccess->countsOfBASpectrum() < counts) {}

    return true;
}

bool DRS4ScriptingEngineAccessManager::waitForCountsMerged(int counts)
{
    if ( !m_dlgAccess )
        return false;

    while (m_dlgAccess->countsOfMergedSpectrum() < counts) {}

    return true;
}

bool DRS4ScriptingEngineAccessManager::waitForCountsPrompt(int counts)
{
    if ( !m_dlgAccess )
        return false;

    while (m_dlgAccess->countsOfCoincidenceSpectrum() < counts) {}

    return true;
}

void DRS4ScriptingEngineAccessManager::changeUsingPositiveTriggerPolarity(bool positive)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changePositivTriggerPolarity(positive, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::changeToHavingPositiveSignal(bool positive)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->changePositivSignal(positive, FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::fitGaussianOnCoincidenceData()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->fitCoincidenceData(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::fitGaussianOnMergedData()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->fitMergedData(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::fitGaussianOnABData()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->fitABData(FunctionSource::AccessFromScript);
}

void DRS4ScriptingEngineAccessManager::fitGaussianOnBAData()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return;

    m_dlgAccess->fitBAData(FunctionSource::AccessFromScript);
}

bool DRS4ScriptingEngineAccessManager::loadSimulationInputFile(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->loadSimulationToolSettingsFromExtern(path);
}

bool DRS4ScriptingEngineAccessManager::saveSettingsFile(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->saveSettingsFileFromExtern(path);
}

bool DRS4ScriptingEngineAccessManager::loadSettingsFile(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->loadSettingsFileFromExtern(path);
}

bool DRS4ScriptingEngineAccessManager::loadDataStreamFile(const QString &fileName)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->loadDataStreamFromExtern(fileName);
}

bool DRS4ScriptingEngineAccessManager::stopLoadingFromDataStreamFile()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->stopDataFromDataStream();
}

bool DRS4ScriptingEngineAccessManager::isRunningFromDataStream()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->isPulseGenerationFromDataStreamRunning();
}

bool DRS4ScriptingEngineAccessManager::startDataStreaming(const QString &fileName)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->startStreamingFromExtern(fileName);
}

bool DRS4ScriptingEngineAccessManager::stopDataStreaming()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->stopStreamingFromExtern();
}

bool DRS4ScriptingEngineAccessManager::isDataStreamArmed()
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->isDataStreamArmed();
}

bool DRS4ScriptingEngineAccessManager::saveDataAB(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->saveABSpectrumFromExtern(path);
}

bool DRS4ScriptingEngineAccessManager::saveDataBA(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->saveBASpectrumFromExtern(path);
}

bool DRS4ScriptingEngineAccessManager::saveDataMerged(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->saveMergedSpectrumFromExtern(path);
}

bool DRS4ScriptingEngineAccessManager::saveDataCoincidence(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->saveCoincidenceSpectrumFromExtern(path);
}

bool DRS4ScriptingEngineAccessManager::savePHSA(const QString &fileName)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->savePHSAFromExtern(fileName);
}

bool DRS4ScriptingEngineAccessManager::savePHSB(const QString &fileName)
{
    QMutexLocker locker(&m_mutex);

    if ( !m_dlgAccess )
        return false;

    return m_dlgAccess->savePHSBFromExtern(fileName);
}

DRS4ScriptingEngineAccessManager::DRS4ScriptingEngineAccessManager() : m_dlgAccess(DNULLPTR) {}

DRS4ScriptingEngineAccessManager::~DRS4ScriptingEngineAccessManager()
{
    DDELETE_SAFETY(__sharedInstanceScriptingEngineAccessManager);
}



