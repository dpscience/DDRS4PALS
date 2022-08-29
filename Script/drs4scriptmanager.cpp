/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2022 Dr. Danny Petschke
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
**  @author: Dr. Danny Petschke
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

#include "drs4scriptmanager.h"

/* ----> Script-Engine Functions <----*/

DRS4ScriptEngineCommandCollector::DRS4ScriptEngineCommandCollector(QScriptEngine *engine, DRS4ScriptString *logFile, DRS4ScriptString *logFileSucceed, DRS4ScriptString *logFileFailed, DRS4ScriptString *logFilePrintOut) :
    m_logTextEdit(logFile),
    m_logSucceedTextEdit(logFileSucceed),
    m_logFailedTextEdit(logFileFailed),
    m_logPrintOutTextEdit(logFilePrintOut)
{
    if ( !engine )
        return;

    QScriptValue appContext  = engine->newQObject(this);
    engine->globalObject().setProperty("LTSpectrumRemoteControl", appContext);
}

DRS4ScriptEngineCommandCollector::~DRS4ScriptEngineCommandCollector() {}

QStringList DRS4ScriptEngineCommandCollector::functionCollection() const
{
    QStringList list;

    /*list.append("startAcquisition()");
    list.append("stopAcquisition()");

    list.append("isAcquisitionStopped() << bool");
    list.append("isAcquisitionRunning() << bool");*/

    /*list.append("changeBurstMode(__bool_burstMode?__)");
    list.append("isBurstModeRunning() << bool");*/

    list.append("saveDataOfPromptSpectrum(\"__name_of_file__\") << bool");
    list.append("saveDataOfMergedSpectrum(\"__name_of_file__\") << bool");
    list.append("saveDataOfABSpectrum(\"__name_of_file__\") << bool");
    list.append("saveDataOfBASpectrum(\"__name_of_file__\") << bool");

    list.append("savePHSA(\"__name_of_file__\") << bool");
    list.append("savePHSB(\"__name_of_file__\") << bool");

    list.append("saveCompleteLogFile(\"__name_of_file__\") << bool");
    list.append("savePrintOutLogFile(\"__name_of_file__\") << bool");
    list.append("saveSucceedLogFile(\"__name_of_file__\") << bool");
    list.append("saveFailedLogFile(\"__name_of_file__\") << bool");

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        list.append("loadSimulationInputFile(\"__name_of_file__\") << bool");

    list.append("saveSettingsFile(\"__name_of_file__\") << bool");
    list.append("loadSettingsFile(\"__name_of_file__\") << bool");

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
        list.append("loadDataStreamFile(\"__name_of_file__\") << bool");
        list.append("stopLoadingFromDataStreamFile() << bool");

        list.append("isRunningFromDataStream() << bool");
    }

    list.append("startDataStreaming(\"__name_of_file__\") << bool");
    list.append("stopDataStreaming() << bool");

    list.append("isDataStreamArmed() << bool");

    list.append("resetPHSA()");
    list.append("resetPHSB()");

    list.append("resetAllSpectra()");
    list.append("resetABSpectrum()");
    list.append("resetBASpectrum()");
    list.append("resetPromptSpectrum()");
    list.append("resetMergedSpectrum()");

    /*list.append("resetAreaPlotA()");
    list.append("resetAreaPlotB()");*/

    list.append("changePHSStartMinA(__int_value_of_maximum_1024__)");
    list.append("changePHSStartMinB(__int_value_of_maximum_1024__)");
    list.append("changePHSStartMaxA(__int_value_of_maximum_1024__)");
    list.append("changePHSStartMaxB(__int_value_of_maximum_1024__)");

    list.append("changePHSStopMinA(__int_value_of_maximum_1024__)");
    list.append("changePHSStopMinB(__int_value_of_maximum_1024__)");
    list.append("changePHSStopMaxA(__int_value_of_maximum_1024__)");
    list.append("changePHSStopMaxB(__int_value_of_maximum_1024__)");

    list.append("changeTriggerLevelA(__int_value_in_mV__)");
    list.append("changeTriggerLevelB(__int_value_in_mV__)");

    list.append("changeStartCell(__int_cell__)");
    list.append("changeStopCell(__int_cell__)");

    list.append("changeCFDLevelA(__double_value_in_%__)");
    list.append("changeCFDLevelB(__double_value_in_%__)");

    list.append("changeTriggerDelayInNanoseconds(__int_delay_in_nanoseconds__)");

    list.append("changeATSInPicoseconds(__double_value_in_picoseconds__)");

    /*list.append("changeUsingPositiveTriggerPolarity(__bool_positive?__)");
    list.append("changeToHavingPositiveSignal(__bool_positive?__)");*/

    /*list.append("changePromptDataMaxFitIterations(__int_value__)");
    list.append("changeMergedDataMaxFitIterations(__int_value__)");
    list.append("changeABDataMaxFitIterations(__int_value__)");
    list.append("changeBADataMaxFitIterations(__int_value__)");*/

    list.append("changePulseAreaFilterEnabled(__bool_areaFilter?__)");

    list.append("waitForCountsAB(__int__counts__) << bool");
    list.append("waitForCountsBA(__int__counts__) << bool");
    list.append("waitForCountsMerged(__int__counts__) << bool");
    list.append("waitForCountsPrompt(__int__counts__) << bool");

    /*list.append("fitGaussianOnPromptData()");
    list.append("fitGaussianOnMergedData()");
    list.append("fitGaussianOnABData()");
    list.append("fitGaussianOnBAData()");*/

    list.append("getCountsOfABSpectrum() << int");
    list.append("getCountsOfBASpectrum() << int");
    list.append("getCountsOfMergedSpectrum() << int");
    list.append("getCountsOfPromptSpectrum() << int");

    /*list.append("getLastFitStateOfABSpectrum() << int");
    list.append("getLastFitStateOfBASpectrum() << int");
    list.append("getLastFitStateOfMergedSpectrum() << int");
    list.append("getLastFitStateOfPromptSpectrum() << int");

    list.append("getT0OfABSpectrumInPicoseconds() << double");
    list.append("getT0OfBASpectrumInPicoseconds() << double");
    list.append("getT0OfMergedSpectrumInPicoseconds() << double");
    list.append("getT0OfPromptSpectrumInPicoseconds() << double");

    list.append("getFWHMOfABSpectrumInPicoseconds() << double");
    list.append("getFWHMOfBASpectrumInPicoseconds() << double");
    list.append("getFWHMOfMergedSpectrumInPicoseconds() << double");
    list.append("getFWHMOfPromptSpectrumInPicoseconds() << double");*/

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        list.append("getBoardTemperatureInDegree() << double");

    /*list.append("getStartCell() << int");
    list.append("getStopCell() << int");*/

    list.append("getCurrentSettingsFile() << string");

    list.append("getTriggerLevelAInMillivolt() << int");
    list.append("getTriggerLevelBInMillivolt() << int");

    list.append("getTriggerDelayInNanoseconds() << int");

    list.append("getCFDLevelA() << double");
    list.append("getCFDLevelB() << double");

    list.append("getATSInPicoseconds() << double");

    /*list.append("getPromptFitIterations() << int");
    list.append("getABFitIterations() << int");
    list.append("getBAFitIterations() << int");
    list.append("getMergedFitIterations() << int");

    list.append("isPositiveTriggerPolarity() << bool");
    list.append("isPositiveSignal() << bool");*/

    list.append("print(__variant__)");

    return list;
}

void DRS4ScriptEngineCommandCollector::mapMsg(const QString &msg, const DRS4LogType &type)
{
    QMutexLocker locker(&m_mutex);

    if ( msg.isEmpty() )
        return;

    const QString timeHtml = "<font color=\"Gray\">";
    const QString printHtml = "<font color=\"Blue\">";
    const QString failedHtml = "<font color=\"Red\">";
    const QString warningHtml = "<font color=\"Orange\">";
    const QString succeedHtml = "<font color=\"Green\">";
    const QString endHtml = "</font>";

    const QString timeString = timeHtml + "[" + QDateTime::currentDateTime().toString() + "] " + endHtml;

    switch ( type )
    {
    case DRS4LogType::SUCCEED:
    {
        if ( m_logTextEdit )
            m_logTextEdit->append(timeString + succeedHtml + msg + endHtml);

        if ( m_logSucceedTextEdit )
            m_logSucceedTextEdit->append(timeString + succeedHtml + msg + endHtml);
    }
        break;

    case DRS4LogType::WARNING:
    {
        if ( m_logTextEdit )
            m_logTextEdit->append(timeString + warningHtml + msg + endHtml);
    }
        break;

    case DRS4LogType::FAILED:
    {
        if ( m_logTextEdit )
            m_logTextEdit->append(timeString + failedHtml + msg + endHtml);

        if ( m_logFailedTextEdit )
            m_logFailedTextEdit->append(timeString + failedHtml + msg + endHtml);
    }
        break;

    case DRS4LogType::PRINTOUT:
    {
        if ( m_logTextEdit )
            m_logTextEdit->append(timeString + "Print: " + printHtml + msg + endHtml);

        if ( m_logPrintOutTextEdit )
            m_logPrintOutTextEdit->append(timeString + printHtml + msg + endHtml);
    }
        break;

    default:
        break;
    }
}

void DRS4ScriptEngineCommandCollector::print(const QVariant &value)
{
    mapMsg(value.toString(), DRS4LogType::PRINTOUT);
}

bool DRS4ScriptEngineCommandCollector::saveDataOfPromtSpectrum(const QString& fileName)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->saveDataCoincidence(fileName);

    if ( success )
        mapMsg("Promt-Spectrum successfully saved: /" + fileName + "/", DRS4LogType::SUCCEED);
    else
        mapMsg("Error on saving Promt-Spectrum: /" + fileName + "/", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::saveDataOfMergedSpectrum(const QString& fileName)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->saveDataMerged(fileName);

    if ( success )
        mapMsg("Merged-Spectrum successfully saved: /" + fileName + "/", DRS4LogType::SUCCEED);
    else
        mapMsg("Error on saving Merged-Spectrum: /" + fileName + "/", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::saveDataOfABSpectrum(const QString& fileName)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->saveDataAB(fileName);

    if ( success )
        mapMsg("AB-Spectrum successfully saved: /" + fileName + "/", DRS4LogType::SUCCEED);
    else
        mapMsg("Error on saving AB-Spectrum: /" + fileName + "/", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::saveDataOfBASpectrum(const QString& fileName)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->saveDataBA(fileName);

    if ( success )
        mapMsg("BA-Spectrum successfully saved: /" + fileName + "/", DRS4LogType::SUCCEED);
    else
        mapMsg("Error on saving BA-Spectrum: /" + fileName + "/", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::saveCompleteLogFile(const QString &fileName)
{
    QFile file(fileName);
    QTextStream stream(&file);

    QString content = "";
    for ( QString item : m_logTextEdit->data() )
        content.append(item);

    content.remove("<font color=\"Gray\">").remove("</font>").remove("<font color=\"Red\">").remove("<font color=\"Green\">").remove("<font color=\"Blue\">").replace("<br>", "\r\n");

    if ( file.open(QIODevice::WriteOnly) )
    {
        if ( !m_logTextEdit )
        {
            file.close();
            return false;
        }

        mapMsg("Complete Log-File successfully saved: /" + fileName + "/", DRS4LogType::SUCCEED);

        stream << content;

        file.close();
    }
    else
    {
        mapMsg("Error on saving Complete Log-File: /" + fileName + "/", DRS4LogType::FAILED);

        return false;
    }

    return true;
}

bool DRS4ScriptEngineCommandCollector::savePrintOutLogFile(const QString &fileName)
{
    QFile file(fileName);
    QTextStream stream(&file);

    QString content = "";
    for ( QString item : m_logPrintOutTextEdit->data() )
        content.append(item);

    content.remove("<font color=\"Gray\">").remove("</font>").remove("<font color=\"Blue\">").replace("<br>", "\r\n");

    if ( file.open(QIODevice::WriteOnly) )
    {
        if ( !m_logPrintOutTextEdit )
        {
            file.close();
            return false;
        }

        mapMsg("Print-Out Log-File successfully saved: /" + fileName + "/", DRS4LogType::SUCCEED);

        stream << content;

        file.close();
    }
    else
    {
        mapMsg("Error on saving Print-Out Log-File: /" + fileName + "/", DRS4LogType::FAILED);

        return false;
    }

    return true;
}

bool DRS4ScriptEngineCommandCollector::saveSucceedLogFile(const QString &fileName)
{
    QFile file(fileName);
    QTextStream stream(&file);

    QString content = "";
    for ( QString item : m_logSucceedTextEdit->data() )
        content.append(item);

    content.remove("<font color=\"Gray\">").remove("</font>").remove("<font color=\"Green\">").replace("<br>", "\r\n");

    if ( file.open(QIODevice::WriteOnly) )
    {
        if ( !m_logSucceedTextEdit )
        {
            file.close();
            return false;
        }

        mapMsg("Succeed Log-File successfully saved: /" + fileName + "/", DRS4LogType::SUCCEED);

        stream << content;

        file.close();
    }
    else
    {
        mapMsg("Error on saving Succeed Log-File: /" + fileName + "/", DRS4LogType::FAILED);

        return false;
    }

    return true;
}

bool DRS4ScriptEngineCommandCollector::saveFailedLogFile(const QString &fileName)
{
    QFile file(fileName);
    QTextStream stream(&file);

    QString content = "";
    for ( QString item : m_logFailedTextEdit->data() )
        content.append(item);

    content.remove("<font color=\"Gray\">").remove("</font>").remove("<font color=\"Red\">").replace("<br>", "\r\n");

    if ( file.open(QIODevice::WriteOnly) )
    {
        if ( !m_logFailedTextEdit )
        {
            file.close();
            return false;
        }

        mapMsg("Failed Log-File successfully saved: /" + fileName + "/", DRS4LogType::SUCCEED);

        stream << content;

        file.close();
    }
    else
    {
        mapMsg("Error on saving Failed Log-File: /" + fileName + "/", DRS4LogType::FAILED);

        return false;
    }

    return true;
}

bool DRS4ScriptEngineCommandCollector::savePHSA(const QString &fileName)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->savePHSA(fileName);

    if ( success )
        mapMsg("PHS-A successfully saved: /" + fileName + "/", DRS4LogType::SUCCEED);
    else
        mapMsg("Error on saving PHS-A: /" + fileName + "/", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::savePHSB(const QString &fileName)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->savePHSB(fileName);

    if ( success )
        mapMsg("PHS-B successfully saved: /" + fileName + "/", DRS4LogType::SUCCEED);
    else
        mapMsg("Error on saving PHS-B: /" + fileName + "/", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::loadSimulationInputFile(const QString& fileName)
{
    if ( DRS4StreamDataLoader::sharedInstance()->isArmed() )
    {
        mapMsg("Simulation is currently running from Stream-Data.", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->loadSimulationInputFile(fileName);

    if ( success )
        mapMsg("Simulation-Input File successfully loaded: /" + fileName + "/", DRS4LogType::SUCCEED);
    else
        mapMsg("Error on loading Simulation-Input File: /" + fileName + "/", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::saveSettingsFile(const QString &fileName)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->saveSettingsFile(fileName);

    if ( success )
        mapMsg("Settings-File successfully saved: /" + fileName + "/", DRS4LogType::SUCCEED);
    else
        mapMsg("Error on saving Settings-File: /" + fileName + "/", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::loadSettingsFile(const QString& fileName)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->loadSettingsFile(fileName);

    if ( success )
        mapMsg("Settings-File successfully loaded: /" + fileName + "/", DRS4LogType::SUCCEED);
    else
        mapMsg("Error on loading Settings-File: /" + fileName + "/", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::loadDataStreamFile(const QString &fileName)
{
    if ( DRS4StreamDataLoader::sharedInstance()->isArmed() )
    {
        mapMsg("Access denied. Another Data-Stream is running.", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->loadDataStreamFile(fileName);

    if ( success )
        mapMsg("Data-Stream File successfully loaded: /" + fileName + "/", DRS4LogType::SUCCEED);
    else
        mapMsg("Error on loading Data-Stream File: /" + fileName + "/", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::stopLoadingFromDataStreamFile()
{
    if ( DRS4StreamDataLoader::sharedInstance()->isArmed() )
    {
        mapMsg("Function call denied. No Data-Stream available..", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->stopLoadingFromDataStreamFile();

    Sleep(5000);

    if ( success )
        mapMsg("Loading from Data-Stream stopped.", DRS4LogType::SUCCEED);
    else
        mapMsg("No Data-Stream File is running. Nothing to stop.", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::startDataStreaming(const QString &fileName)
{
    if ( DRS4StreamManager::sharedInstance()->isArmed() )
    {
        mapMsg("Function call  denied. Another Data-Stream is recording.", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->startDataStreaming(fileName);

    if ( success )
        mapMsg("Data-Stream started recording: /" + fileName + "/", DRS4LogType::SUCCEED);
    else
        mapMsg("An Error occurred while starting the Data-Stream!", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::stopDataStreaming()
{
    if ( !DRS4StreamManager::sharedInstance()->isArmed() )
    {
        mapMsg("Function call  denied. No Data-Stream is recording.", DRS4LogType::FAILED);
        return false;
    }

    const bool success = DRS4ScriptingEngineAccessManager::sharedInstance()->stopDataStreaming();

    if ( success )
        mapMsg("Data-Stream finished successfully.", DRS4LogType::SUCCEED);
    else
        mapMsg("An Error occurred while stopping the Data-Stream!", DRS4LogType::FAILED);

    return success;
}

bool DRS4ScriptEngineCommandCollector::isDataStreamArmed()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->isDataStreamArmed();
}

void DRS4ScriptEngineCommandCollector::resetPHSA()
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    mapMsg("PHS of A reseted.", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->resetPHSA();
}

void DRS4ScriptEngineCommandCollector::resetPHSB()
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    mapMsg("PHS of B reseted.", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->resetPHSB();
}

void DRS4ScriptEngineCommandCollector::resetAllSpectra()
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    mapMsg("All Spectra reseted.", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->resetAllSpectra();
}

void DRS4ScriptEngineCommandCollector::resetABSpectrum()
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    mapMsg("AB-Spectrum reseted.", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->resetABSpectra();
}

void DRS4ScriptEngineCommandCollector::resetBASpectrum()
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    mapMsg("BA-Spectrum reseted.", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->resetBASpectra();
}

void DRS4ScriptEngineCommandCollector::resetPromtSpectrum()
{
    mapMsg("Promt-Spectrum reseted.", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->resetCoincidenceSpectra();
}

void DRS4ScriptEngineCommandCollector::resetMergedSpectrum()
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    mapMsg("Merged-Spectrum reseted.", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->resetMergedSpectra();
}

void DRS4ScriptEngineCommandCollector::changePHSStartMinA(int value)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( value > 1024 )
    {
        value = 1024;

        mapMsg("PHS of A - Start Minimum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStartMinA(value);

        return;
    }

    if ( value < 0 )
    {
        value = 0;

        mapMsg("PHS of A - Start Minimum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted.", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStartMinA(value);

        return;
    }

    mapMsg("PHS of A - Start Minimum changed to " + QVariant(value).toString(), DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStartMinA(value);
}

void DRS4ScriptEngineCommandCollector::changePHSStartMaxA(int value)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( value > 1024 )
    {
        value = 1024;

        mapMsg("PHS of A - Start Maximum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStartMaxA(value);

        return;
    }

    if ( value < 0 )
    {
        value = 0;

        mapMsg("PHS of A - Start Maximum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted.", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStartMaxA(value);

        return;
    }

    mapMsg("PHS of A - Start Maximum changed to " + QVariant(value).toString(), DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStartMaxA(value);
}

void DRS4ScriptEngineCommandCollector::changePHSStopMinA(int value)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( value > 1024 )
    {
        value = 1024;

        mapMsg("PHS of A - Stop Minimum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStopMinA(value);

        return;
    }

    if ( value < 0 )
    {
        value = 0;

        mapMsg("PHS of A - Stop Minimum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted.", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStopMinA(value);

        return;
    }

    mapMsg("PHS of A - Stop Minimum changed to " + QVariant(value).toString(), DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStopMinA(value);
}

void DRS4ScriptEngineCommandCollector::changePHSStopMaxA(int value)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( value > 1024 )
    {
        value = 1024;

        mapMsg("PHS of A - Stop Maximum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStopMaxA(value);

        return;
    }

    if ( value < 0 )
    {
        value = 0;

        mapMsg("PHS of A - Stop Maximum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted.", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStopMaxA(value);

        return;
    }

    mapMsg("PHS of A - Stop Maximum changed to " + QVariant(value).toString(), DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStopMaxA(value);
}

void DRS4ScriptEngineCommandCollector::changePHSStartMinB(int value)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( value > 1024 )
    {
        value = 1024;

        mapMsg("PHS of B - Start Minimum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStartMinB(value);

        return;
    }

    if ( value < 0 )
    {
        value = 0;

        mapMsg("PHS of B - Start Minimum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted.", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStartMinB(value);

        return;
    }

    mapMsg("PHS of B - Start Minimum changed to " + QVariant(value).toString(), DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStartMinB(value);
}

void DRS4ScriptEngineCommandCollector::changePHSStartMaxB(int value)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( value > 1024 )
    {
        value = 1024;

        mapMsg("PHS of B - Start Maximum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStartMaxB(value);

        return;
    }

    if ( value < 0 )
    {
        value = 0;

        mapMsg("PHS of B - Start Maximum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted.", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStartMaxB(value);

        return;
    }

    mapMsg("PHS of B - Start Maximum changed to " + QVariant(value).toString(), DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStartMaxB(value);
}

void DRS4ScriptEngineCommandCollector::changePHSStopMinB(int value)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( value > 1024 )
    {
        value = 1024;

        mapMsg("PHS of B - Stop Minimum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStopMinB(value);

        return;
    }

    if ( value < 0 )
    {
        value = 0;

        mapMsg("PHS of B - Stop Minimum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted.", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStopMinB(value);

        return;
    }

    mapMsg("PHS of B - Stop Minimum changed to " + QVariant(value).toString(), DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStopMinB(value);
}

void DRS4ScriptEngineCommandCollector::changePHSStopMaxB(int value)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( value > 1024 )
    {
        value = 1024;

        mapMsg("PHS of B - Stop Maximum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStopMaxB(value);

        return;
    }

    if ( value < 0 )
    {
        value = 0;

        mapMsg("PHS of B - Stop Maximum changed to " + QVariant(value).toString() + " - Set value reached limit - value was adapted.", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStopMaxB(value);

        return;
    }

    mapMsg("PHS of B - Stop Maximum changed to " + QVariant(value).toString(), DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changePHSStopMaxB(value);
}

void DRS4ScriptEngineCommandCollector::changeTriggerLevelA(int levelInMilliVolt)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( levelInMilliVolt > 500.0f )
    {
        levelInMilliVolt = 500.0f;

        mapMsg("Trigger-Level of A changed to " + QVariant(levelInMilliVolt).toString() + " mV - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changeTriggerLevelA(levelInMilliVolt);

        return;
    }

    if ( levelInMilliVolt < -500.0f )
    {
        levelInMilliVolt = -500.0f;

        mapMsg("Trigger-Level of A changed to " + QVariant(levelInMilliVolt).toString() + " mV - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changeTriggerLevelA(levelInMilliVolt);

        return;
    }

    mapMsg("Trigger-Level A changed to " + QVariant(levelInMilliVolt).toString() + " mV", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changeTriggerLevelA(levelInMilliVolt);
}

void DRS4ScriptEngineCommandCollector::changeTriggerLevelB(int levelInMilliVolt)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( levelInMilliVolt > 500.0f )
    {
        levelInMilliVolt = 500.0f;

        mapMsg("Trigger-Level of B changed to " + QVariant(levelInMilliVolt).toString() + " mV - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changeTriggerLevelB(levelInMilliVolt);

        return;
    }

    if ( levelInMilliVolt < -500.0f )
    {
        levelInMilliVolt = -500.0f;

        mapMsg("Trigger-Level of B changed to " + QVariant(levelInMilliVolt).toString() + " mV - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changeTriggerLevelB(levelInMilliVolt);

        return;
    }

    mapMsg("Trigger-Level of B changed to " + QVariant(levelInMilliVolt).toString() + " mV", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changeTriggerLevelB(levelInMilliVolt);
}

void DRS4ScriptEngineCommandCollector::changeCFDLevelA(double percentage)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( percentage > 100.0f )
        percentage = 100.0f;

    if ( percentage < 0.0f )
        percentage = 0.0f;

    if ( percentage > 100.0f )
    {
        percentage = 100.0f;

        mapMsg("CFD-Level of A changed to " + QVariant(percentage).toString() + " % - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changeCFDLevelA(percentage);

        return;
    }

    if ( percentage < 0.0f )
    {
        percentage = 0.0f;

        mapMsg("CFD-Level of A changed to " + QVariant(percentage).toString() + " % - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changeCFDLevelA(percentage);

        return;
    }

    mapMsg("CFD-Level of A changed to " + QVariant(percentage).toString() + " %", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changeCFDLevelA(percentage);
}

void DRS4ScriptEngineCommandCollector::changeCFDLevelB(double percentage)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( percentage > 100.0f )
        percentage = 100.0f;

    if ( percentage < 0.0f )
        percentage = 0.0f;

    if ( percentage > 100.0f )
    {
        percentage = 100.0f;

        mapMsg("CFD-Level of B changed to " + QVariant(percentage).toString() + " % - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changeCFDLevelB(percentage);

        return;
    }

    if ( percentage < 0.0f )
    {
        percentage = 0.0f;

        mapMsg("CFD-Level of B changed to " + QVariant(percentage).toString() + " % - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changeCFDLevelB(percentage);

        return;
    }

    mapMsg("CFD-Level of B changed to " + QVariant(percentage).toString() + " %", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changeCFDLevelB(percentage);
}

void DRS4ScriptEngineCommandCollector::changeTriggerDelayInNanoseconds(int delay)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    if ( delay < 0 )
    {
        delay = 0;

        mapMsg("Trigger-Delay changed to " + QVariant(delay).toString() + " ns - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changeTriggerDelayInNanoseconds(delay);

        return;
    }

    if ( delay > DRS4SettingsManager::sharedInstance()->sweepInNanoseconds() )
    {
        delay = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();

        mapMsg("Trigger-Delay changed to " + QVariant(delay).toString() + " ns - Set value reached limit - value was adapted", DRS4LogType::FAILED);

        DRS4ScriptingEngineAccessManager::sharedInstance()->changeTriggerDelayInNanoseconds(delay);

        return;
    }

    mapMsg("Trigger-Delay changed to " + QVariant(delay).toString() + " ns", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changeTriggerDelayInNanoseconds(delay);
}

void DRS4ScriptEngineCommandCollector::changeATSInPicoseconds(double ps)
{
    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
    {
        mapMsg("Function call  denied. Burst-Mode is running.", DRS4LogType::FAILED);
        return;
    }

    mapMsg("ATS changed to: " + QVariant(ps).toString() + " ps", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->changeATSInPicoseconds(ps);
}

bool DRS4ScriptEngineCommandCollector::waitForCountsAB(int counts)
{
    if (counts <= 0) {
        mapMsg("Function call  denied. Invalid Value.", DRS4LogType::FAILED);
        return false;
    }

    return DRS4ScriptingEngineAccessManager::sharedInstance()->waitForCountsAB(counts);
}

bool DRS4ScriptEngineCommandCollector::waitForCountsBA(int counts)
{
    if (counts <= 0) {
        mapMsg("Function call  denied. Invalid Value.", DRS4LogType::FAILED);
        return false;
    }

    return DRS4ScriptingEngineAccessManager::sharedInstance()->waitForCountsBA(counts);
}

bool DRS4ScriptEngineCommandCollector::waitForCountsMerged(int counts)
{
    if (counts <= 0) {
        mapMsg("Function call  denied. Invalid Value.", DRS4LogType::FAILED);
        return false;
    }

    return DRS4ScriptingEngineAccessManager::sharedInstance()->waitForCountsMerged(counts);
}

bool DRS4ScriptEngineCommandCollector::waitForCountsPrompt(int counts)
{
    if (counts <= 0) {
        mapMsg("Function call  denied. Invalid Value.", DRS4LogType::FAILED);
        return false;
    }

    return DRS4ScriptingEngineAccessManager::sharedInstance()->waitForCountsPrompt(counts);
}

/*void DRS4ScriptEngineCommandCollector::startAcquisition()
{
    if ( DRS4ScriptingEngineAccessManager::sharedInstance()->isAcquisitionRunning() )
    {
        mapMsg("Function call denied. Acquisition is already running.", DRS4LogType::FAILED);
        return;
    }

    mapMsg("Data-Acquisition started.", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->start();
}

void DRS4ScriptEngineCommandCollector::stopAcquisition()
{
    if ( DRS4ScriptingEngineAccessManager::sharedInstance()->isAcquisitionStopped() )
    {
        mapMsg("Function call denied. No Acquisition is running.", DRS4LogType::FAILED);
        return;
    }

    mapMsg("Data-Acquisition stopped.", DRS4LogType::SUCCEED);

    DRS4ScriptingEngineAccessManager::sharedInstance()->stop();
}*/

int DRS4ScriptEngineCommandCollector::getCountsOfABSpectrum()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->integralCountsAB();
}

int DRS4ScriptEngineCommandCollector::getCountsOfBASpectrum()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->integralCountsBA();
}

int DRS4ScriptEngineCommandCollector::getCountsOfMergedSpectrum()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->integralCountsMerged();
}

int DRS4ScriptEngineCommandCollector::getCountsOfPromtSpectrum()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->integralCountsCoincidence();
}

/*bool DRS4ScriptEngineCommandCollector::isAcquisitionStopped()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->isAcquisitionStopped();
}

bool DRS4ScriptEngineCommandCollector::isAcquisitionRunning()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->isAcquisitionRunning();
}*/

double DRS4ScriptEngineCommandCollector::getBoardTemperatureInDegree()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->getBoardTemperatureInDegree();
}

QString DRS4ScriptEngineCommandCollector::getCurrentSettingsFile()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->getCurrentSettingsFile();
}

int DRS4ScriptEngineCommandCollector::getTriggerLevelAInMillivolt()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->getTriggerLevelAInMillivolt();
}

int DRS4ScriptEngineCommandCollector::getTriggerLevelBInMillivolt()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->getTriggerLevelBInMillivolt();
}

int DRS4ScriptEngineCommandCollector::getTriggerDelayInNanoseconds()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->getTriggerDelayInNanoseconds();
}

double DRS4ScriptEngineCommandCollector::getCFDLevelA()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->getCFDLevelA();
}

double DRS4ScriptEngineCommandCollector::getCFDLevelB()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->getCFDLevelB();
}

double DRS4ScriptEngineCommandCollector::getATSInPicoseconds()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->getATSInPicoseconds();
}

bool DRS4ScriptEngineCommandCollector::isRunningFromDataStream()
{
    return DRS4ScriptingEngineAccessManager::sharedInstance()->isRunningFromDataStream();
}

/* ----> /Script-Engine Functions <----*/

DRS4ScriptEngine::~DRS4ScriptEngine()
{
    DDELETE_SAFETY(m_collector);
}

DRS4ScriptEngineCommandCollector *DRS4ScriptEngine::cmdCollector() const
{
    return m_collector;
}

DRS4ScriptEngine::DRS4ScriptEngine(DRS4ScriptString *logFile, DRS4ScriptString *logFileSucceed, DRS4ScriptString *logFileFailed, DRS4ScriptString *logFilePrintOut)
{
    if ( logFile
         && logFileSucceed
         && logFileFailed
         && logFilePrintOut )
    {
       m_logTextEdit = logFile;
       m_logSucceedTextEdit = logFileSucceed;
       m_logFailedTextEdit = logFileFailed;
       m_logPrintOutTextEdit = logFilePrintOut;
    }
    else
    {
        m_logTextEdit = DNULLPTR;
        m_logSucceedTextEdit = DNULLPTR;
        m_logFailedTextEdit = DNULLPTR;
        m_logPrintOutTextEdit = DNULLPTR;
    }

    if ( m_logTextEdit
         && m_logSucceedTextEdit
         && m_logFailedTextEdit
         &&m_logPrintOutTextEdit )
        m_collector = new DRS4ScriptEngineCommandCollector(this, m_logTextEdit, m_logSucceedTextEdit, m_logFailedTextEdit, m_logPrintOutTextEdit);
    else
        m_collector = new DRS4ScriptEngineCommandCollector(this);
}


DRS4ScriptDataInterChangeManager* __sharedInstanceScriptDataInterChangeManager = DNULLPTR;

DRS4ScriptDataInterChangeManager::DRS4ScriptDataInterChangeManager() :
    m_state(SCRIP_NO_STATE),
    m_script("") {}

DRS4ScriptDataInterChangeManager::~DRS4ScriptDataInterChangeManager()
{
    DDELETE_SAFETY(__sharedInstanceScriptDataInterChangeManager);
}

DRS4ScriptDataInterChangeManager *DRS4ScriptDataInterChangeManager::sharedInstance()
{
    if ( !__sharedInstanceScriptDataInterChangeManager )
        __sharedInstanceScriptDataInterChangeManager = new DRS4ScriptDataInterChangeManager();

    return __sharedInstanceScriptDataInterChangeManager;
}

void DRS4ScriptDataInterChangeManager::setScript(const QString &script)
{
    QMutexLocker locker(&m_mutex);

    m_script = script;
}

void DRS4ScriptDataInterChangeManager::setScriptState(const ScriptState &state)
{
    QMutexLocker locker(&m_mutex);

    m_state = state;
}

QString DRS4ScriptDataInterChangeManager::getScript()
{
    QMutexLocker locker(&m_mutex);

    return m_script;
}

ScriptState DRS4ScriptDataInterChangeManager::getScriptState()
{
    QMutexLocker locker(&m_mutex);

    return m_state;
}

QStringList DRS4ScriptExecuter::functionCollection() const
{
    QMutexLocker locker(&m_mutex);

    if ( !m_engine )
        return QStringList();

    if ( !m_engine->cmdCollector() )
        return QStringList();


    return m_engine->cmdCollector()->functionCollection();
}

DRS4ScriptExecuter::DRS4ScriptExecuter(DRS4ScriptString *logFile, DRS4ScriptString *logFileSucceed, DRS4ScriptString *logFileFailed, DRS4ScriptString *logFilePrintOut)
{
     if ( logFile
          && logFileSucceed
          && logFileFailed
          && logFilePrintOut )
     {
        m_logTextEdit = logFile;
        m_logSucceedTextEdit = logFileSucceed;
        m_logFailedTextEdit = logFileFailed;
        m_logPrintOutTextEdit = logFilePrintOut;
     }
     else
     {
         m_logTextEdit = DNULLPTR;
         m_logSucceedTextEdit = DNULLPTR;
         m_logFailedTextEdit = DNULLPTR;
         m_logPrintOutTextEdit = DNULLPTR;
     }

     if ( m_logTextEdit
          && m_logSucceedTextEdit
          && m_logFailedTextEdit
          && m_logPrintOutTextEdit )
         m_engine = new DRS4ScriptEngine(m_logTextEdit, m_logSucceedTextEdit, m_logFailedTextEdit, m_logPrintOutTextEdit);
     else
         m_engine = new DRS4ScriptEngine();
}

DRS4ScriptExecuter::~DRS4ScriptExecuter()
{
    DDELETE_SAFETY(m_engine);
}

void DRS4ScriptExecuter::execute()
{
    QMutexLocker locker(&m_mutex);

    //clear the log-content:
    m_logTextEdit->clear();
    m_logSucceedTextEdit->clear();
    m_logFailedTextEdit->clear();
    m_logPrintOutTextEdit->clear();

    DRS4ScriptDataInterChangeManager::sharedInstance()->setScriptState(SCRIP_NO_STATE);

    mapMsg("Script executed.", DRS4LogType::SUCCEED);

    if ( DRS4ScriptDataInterChangeManager::sharedInstance()->getScript().trimmed().isEmpty() )
    {
        DRS4ScriptDataInterChangeManager::sharedInstance()->setScriptState(SCRIPT_SYNTAX_ERROR);
        emit finished();
        return;
    }

    const QString script = "with ( LTSpectrumRemoteControl ) {" + DRS4ScriptDataInterChangeManager::sharedInstance()->getScript() + "}";

    if ( !m_engine->canEvaluate(script) )
    {
        DRS4ScriptDataInterChangeManager::sharedInstance()->setScriptState(SCRIPT_SYNTAX_ERROR);
        mapMsg("Script aborted - Syntax-error.", DRS4LogType::FAILED);

        emit finished();
        return;
    }

    QScriptValue result = m_engine->evaluate(script);
    DUNUSED_PARAM(result);

    if ( DRS4ScriptDataInterChangeManager::sharedInstance()->getScriptState() == SCRIPT_ABORTED )
    {
        mapMsg("Script aborted by user.", DRS4LogType::SUCCEED);

        return;
    }

    if ( m_engine->hasUncaughtException() )
    {
        DRS4ScriptDataInterChangeManager::sharedInstance()->setScriptState(SCRIPT_SYNTAX_ERROR);
        mapMsg("Script aborted (Syntax-error?).", DRS4LogType::FAILED);

        emit finished();
        return;
    }

    DRS4ScriptDataInterChangeManager::sharedInstance()->setScriptState(SCRIPT_FINISHED_OK);

    mapMsg("Script finished successfully.", DRS4LogType::SUCCEED);

    emit finished();
}

void DRS4ScriptExecuter::abort()
{
    m_engine->abortEvaluation();
    DRS4ScriptDataInterChangeManager::sharedInstance()->setScriptState(SCRIPT_ABORTED);

    emit finished();
}

void DRS4ScriptExecuter::mapMsg(const QString &msg, const DRS4LogType &type)
{
    if ( msg.isEmpty() )
        return;

    const QString timeHtml = "<font color=\"Gray\">";
    const QString printHtml = "<font color=\"Blue\">";
    const QString failedHtml = "<font color=\"Red\">";
    const QString warningHtml = "<font color=\"Orange\">";
    const QString succeedHtml = "<font color=\"Green\">";
    const QString endHtml = "</font>";

    const QString timeString = timeHtml + "[" + QDateTime::currentDateTime().toString() + "] " + endHtml;

    switch ( type )
    {
    case DRS4LogType::SUCCEED:
    {
        if ( m_logTextEdit )
            m_logTextEdit->append(timeString + succeedHtml + msg + endHtml);

        if ( m_logSucceedTextEdit )
            m_logSucceedTextEdit->append(timeString + succeedHtml + msg + endHtml);
    }
        break;

    case DRS4LogType::WARNING:
    {
        if ( m_logTextEdit )
            m_logTextEdit->append(timeString + warningHtml + msg + endHtml);
    }
        break;

    case DRS4LogType::FAILED:
    {
        if ( m_logTextEdit )
            m_logTextEdit->append(timeString + failedHtml + msg + endHtml);

        if ( m_logFailedTextEdit )
            m_logFailedTextEdit->append(timeString + failedHtml + msg + endHtml);
    }
        break;

    case DRS4LogType::PRINTOUT:
    {
        if ( m_logTextEdit )
            m_logTextEdit->append(timeString + "Print: " + printHtml + msg + endHtml);

        if ( m_logPrintOutTextEdit )
            m_logPrintOutTextEdit->append(timeString + printHtml + msg + endHtml);
    }
        break;

    default:
        break;
    }
}


static DRS4ScriptManager* __sharedInstanceDRS4ScriptManager = DNULLPTR;

DRS4ScriptManager::DRS4ScriptManager() :
    m_armed(false),
    m_fileName("") {}

DRS4ScriptManager::~DRS4ScriptManager() {}

DRS4ScriptManager *DRS4ScriptManager::sharedInstance()
{
    if (!__sharedInstanceDRS4ScriptManager)
        __sharedInstanceDRS4ScriptManager = new DRS4ScriptManager;

    return __sharedInstanceDRS4ScriptManager;
}

void DRS4ScriptManager::setArmed(bool armed)
{
    QMutexLocker locker(&m_mutex);

    m_armed = armed;
}

bool DRS4ScriptManager::isArmed() const
{
    QMutexLocker locker(&m_mutex);

    return m_armed;
}

void DRS4ScriptManager::setFileName(const QString &fileName)
{
    QMutexLocker locker(&m_mutex);

    m_fileName = fileName;
}

QString DRS4ScriptManager::fileName() const
{
    QMutexLocker locker(&m_mutex);

    return m_fileName;
}
