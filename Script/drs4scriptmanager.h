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
    DRS4ScriptEngineCommandCollector(QScriptEngine *engine,
                                     DRS4ScriptString *logFile = DNULLPTR,
                                     DRS4ScriptString *logFileSucceed = DNULLPTR,
                                     DRS4ScriptString *logFileFailed = DNULLPTR,
                                     DRS4ScriptString *logFilePrintOut = DNULLPTR);
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

    void changeATSInPicoseconds(double ps);

    bool waitForCountsAB(int counts);
    bool waitForCountsBA(int counts);
    bool waitForCountsMerged(int counts);
    bool waitForCountsPrompt(int counts);

    /*void startAcquisition();
    void stopAcquisition();*/

    int getCountsOfABSpectrum();
    int getCountsOfBASpectrum();
    int getCountsOfMergedSpectrum();
    int getCountsOfPromtSpectrum();

    /*bool isAcquisitionStopped();
    bool isAcquisitionRunning();*/

    double getBoardTemperatureInDegree();

    QString getCurrentSettingsFile();

    int getTriggerLevelAInMillivolt();
    int getTriggerLevelBInMillivolt();

    int getTriggerDelayInNanoseconds();

    double getCFDLevelA();
    double getCFDLevelB();

    double getATSInPicoseconds();
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
    DRS4ScriptEngine(DRS4ScriptString *logFile = DNULLPTR, DRS4ScriptString *logFileSucceed = DNULLPTR, DRS4ScriptString *logFileFailed = DNULLPTR, DRS4ScriptString *logFilePrintOut = DNULLPTR);
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
