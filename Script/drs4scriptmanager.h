/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2019 Danny Petschke
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

#ifndef DDRS4SCRIPTMANAGER_H
#define DDRS4SCRIPTMANAGER_H

#include <QtScript>

#include <QMutex>
#include <QMutexLocker>

#include "DLib.h"

#include "drs4scriptingengineaccessmanager.h"

class DRS4ScriptString;

/* ----> Script-Engine Functions <----*/

enum class DRS4LogType
{
    SUCCEED,
    FAILED,
    WARNING,
    PRINTOUT
};

class DRS4ScriptEngineCommandCollector : public QObject
{
    friend class DRS4ScriptEngine;

    DRS4ScriptString *m_logTextEdit;
    DRS4ScriptString *m_logSucceedTextEdit;
    DRS4ScriptString *m_logFailedTextEdit;
    DRS4ScriptString *m_logPrintOutTextEdit;

    mutable QMutex m_mutex;

    Q_OBJECT
public:
    QStringList functionCollection() const;

    void mapMsg(const QString& msg, const DRS4LogType& type);

public:
    DRS4ScriptEngineCommandCollector(QScriptEngine *engine, DRS4ScriptString *logFile = nullptr, DRS4ScriptString *logFileSucceed = nullptr, DRS4ScriptString *logFileFailed = nullptr, DRS4ScriptString *logFilePrintOut = nullptr);
    virtual ~DRS4ScriptEngineCommandCollector();

public slots:
    void print(const QVariant& value);

    bool saveDataOfPromtSpectrum(const QString& fileName);
    bool saveDataOfMergedSpectrum(const QString& fileName);
    bool saveDataOfABSpectrum(const QString& fileName);
    bool saveDataOfBASpectrum(const QString& fileName);

    bool saveCompleteLogFile(const QString& fileName);
    bool savePrintOutLogFile(const QString& fileName);
    bool saveSucceedLogFile(const QString& fileName);
    bool saveFailedLogFile(const QString& fileName);

    bool saveTemperatureProfile(const QString& fileName);

    bool savePHSA(const QString& fileName);
    bool savePHSB(const QString& fileName);

    bool loadSimulationInputFile(const QString& fileName);

    bool saveSettingsFile(const QString& fileName);
    bool loadSettingsFile(const QString& fileName);

    bool loadDataStreamFile(const QString& fileName);
    bool stopLoadingFromDataStreamFile();

    bool startDataStreaming(const QString& fileName);
    bool stopDataStreaming();

    bool isDataStreamArmed();

    bool isRunningFromDataStream();

    void resetPHSA();
    void resetPHSB();

    void resetTemperatureProfile();

    void resetAreaPlotA();
    void resetAreaPlotB();

    void resetAllSpectra();
    void resetABSpectrum();
    void resetBASpectrum();
    void resetPromtSpectrum();
    void resetMergedSpectrum();

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

    void changeUsingPositiveTriggerPolarity(bool positive);
    void changeToHavingPositiveSignal(bool positive);

    void changePromtDataMaxFitIterations(unsigned int iterations);
    void changeMergedDataMaxFitIterations(unsigned int iterations);
    void changeABDataMaxFitIterations(unsigned int iterations);
    void changeBADataMaxFitIterations(unsigned int iterations);

    bool waitForCountsAB(int counts);
    bool waitForCountsBA(int counts);
    bool waitForCountsMerged(int counts);
    bool waitForCountsPrompt(int counts);

    void fitGaussianOnPromtData();
    void fitGaussianOnMergedData();
    void fitGaussianOnABData();
    void fitGaussianOnBAData();

    void changeBurstMode(bool burstMode);
    bool isBurstModeRunning();

    void changePulseAreaFilterEnabled(bool enabled);

    void startAcquisition();
    void stopAcquisition();

    int getCountsOfABSpectrum();
    int getCountsOfBASpectrum();
    int getCountsOfMergedSpectrum();
    int getCountsOfPromtSpectrum();

    bool isAcquisitionStopped();
    bool isAcquisitionRunning();

    int getLastFitStateOfABSpectrum();
    int getLastFitStateOfBASpectrum();
    int getLastFitStateOfMergedSpectrum();
    int getLastFitStateOfPromtSpectrum();

    double getT0OfABSpectrumInPicoseconds();
    double getT0OfBASpectrumInPicoseconds();
    double getT0OfMergedSpectrumInPicoseconds();
    double getT0OfPromtSpectrumInPicoseconds();

    double getFWHMOfABSpectrumInPicoseconds();
    double getFWHMOfBASpectrumInPicoseconds();
    double getFWHMOfMergedSpectrumInPicoseconds();
    double getFWHMOfPromtSpectrumInPicoseconds();

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

    int getPromtFitIterations();
    int getABFitIterations();
    int getBAFitIterations();
    int getMergedFitIterations();

    bool isPositiveTriggerPolarity();
    bool isPositiveSignal();
};

/* ----> /Script-Engine Functions <----*/

class DRS4ScriptEngine : public QScriptEngine
{
    friend class DRS4ScriptExecuter;

    DRS4ScriptEngineCommandCollector *m_collector;

    DRS4ScriptString *m_logTextEdit;
    DRS4ScriptString *m_logSucceedTextEdit;
    DRS4ScriptString *m_logFailedTextEdit;
    DRS4ScriptString *m_logPrintOutTextEdit;

    Q_OBJECT
public:
    DRS4ScriptEngine(DRS4ScriptString *logFile = nullptr, DRS4ScriptString *logFileSucceed = nullptr, DRS4ScriptString *logFileFailed = nullptr, DRS4ScriptString *logFilePrintOut = nullptr);
    virtual ~DRS4ScriptEngine();

 private:
    DRS4ScriptEngineCommandCollector *cmdCollector() const;
};


typedef enum {
    SCRIP_NO_STATE,
    SCRIPT_FINISHED_OK,
    SCRIPT_SYNTAX_ERROR,
    SCRIPT_ABORTED
} ScriptState;

class DRS4ScriptDataInterChangeManager
{
    QString m_script;
    ScriptState m_state;

    DRS4ScriptDataInterChangeManager();
    virtual ~DRS4ScriptDataInterChangeManager();

    mutable QMutex m_mutex;

public:
    static DRS4ScriptDataInterChangeManager *sharedInstance();

    void setScript(const QString& script);
    void setScriptState(const ScriptState& state);

    QString getScript();
    ScriptState getScriptState();
};


class DRS4ScriptExecuter : public QObject
{
    Q_OBJECT

    DRS4ScriptEngine *m_engine;

    DRS4ScriptString *m_logTextEdit;
    DRS4ScriptString *m_logSucceedTextEdit;
    DRS4ScriptString *m_logFailedTextEdit;
    DRS4ScriptString *m_logPrintOutTextEdit;

    mutable QMutex m_mutex;

public:
    QStringList functionCollection() const;

public:
    DRS4ScriptExecuter(DRS4ScriptString *logFile, DRS4ScriptString *logFileSucceed, DRS4ScriptString *logFileFailed, DRS4ScriptString *logFilePrintOut);
    virtual ~DRS4ScriptExecuter();

signals:
    void finished();

public slots:
    void execute();
    void abort();

private:
    void mapMsg(const QString& msg, const DRS4LogType& type);
};


class DRS4ScriptString
{
    friend class DRS4ScriptEngineCommandCollector;

    mutable QMutex m_mutex;

    QStringList m_content;

    int m_lastStart;

public:
    inline void append(const QString &s)
    {
        QMutexLocker locker(&m_mutex);

        m_content.append(s + "<br>");
    }

    inline void clear()
    {
        m_lastStart = 0;
        m_content.clear();
    }

    inline QString dataBundle()
    {
        QMutexLocker locker(&m_mutex);

        if (data().isEmpty())
            return QString("");

        const QString content = dataRange(m_lastStart, data().size()-1);

        return content;
    }

private:
    inline QStringList data()
    {
        return m_content;
    }

    inline QString dataRange(int start, int stop)
    {
        if (stop < start
                || start < 0
                || stop < 0
                || start >= data().size()
                ||  stop >= data().size())
            return QString("");

        QString str = "";
        for ( int i = start ; i <= stop ; ++ i ) {
            QString line(data().at(i));

            if ( i == stop )
                line.remove("<br>");

            str.append(line);
        }

        if (!str.isEmpty())
            m_lastStart = stop+1;

        return str;
    }
};


class DRS4ScriptManager
{
    bool m_armed;
    QString m_fileName;

    DRS4ScriptManager();
    virtual ~DRS4ScriptManager();

    mutable QMutex m_mutex;

public:
    static DRS4ScriptManager* sharedInstance();

    void setArmed(bool armed);
    bool isArmed() const;

    void setFileName(const QString& fileName);
    QString fileName() const;
};

#endif // DDRS4SCRIPTMANAGER_H
