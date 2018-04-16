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

#include "drs4programsettingsmanager.h"

DRS4ProgramSettingsManager *__sharedInstanceProgramSettingsManager = nullptr;

DRS4ProgramSettingsManager::DRS4ProgramSettingsManager()
{
    m_parentNode = new DSimpleXMLNode("DDRS4PALSProgramSettings");

    m_lastSaveDateNode = new DSimpleXMLNode("last-save-date");
    m_lastSaveDateNode->setValue(QDateTime::currentDateTime().toString());

    m_renderPointsNode = new DSimpleXMLNode("interpolation-intra-points");
    m_renderPointsNode->setValue(10);

    m_lastSimulationInputFilePathNode = new DSimpleXMLNode("simulationInputFile-path");
    m_lastSimulationInputFilePathNode->setValue("/home");

    m_lastStreamInputFilePathNode = new DSimpleXMLNode("binaryPulseStreamFile-path");
    m_lastStreamInputFilePathNode->setValue("/home");

    m_lastStreamTextFileInputFilePathNode = new DSimpleXMLNode("asciiStreamFileTextFile-path");
    m_lastStreamTextFileInputFilePathNode->setValue("/home");

    m_lastSettingsFilePathNode = new DSimpleXMLNode("settingsFile-path");
    m_lastSettingsFilePathNode->setValue("/home");

    m_lastSaveDataFilePathNode = new DSimpleXMLNode("spectraDataFile-path");
    m_lastSaveDataFilePathNode->setValue("/home");

    m_lastSaveScriptFilePathNode = new DSimpleXMLNode("scriptFile-path");
    m_lastSaveScriptFilePathNode->setValue("/home");

    m_lastSaveLogFilePathNode = new DSimpleXMLNode("scriptLogFile-path");
    m_lastSaveLogFilePathNode->setValue("/home");

    m_lastSaveTProfileFilePathNode = new DSimpleXMLNode("boardTemperatureProfileFile-path");
    m_lastSaveTProfileFilePathNode->setValue("/home");

    m_lastPHSSaveFilePathNode = new DSimpleXMLNode("PHSDataFile-path");
    m_lastPHSSaveFilePathNode->setValue("/home");


    m_multicoreThreadingParentNode = new DSimpleXMLNode("multicore-threading");

    m_enableMulticoreThreadingNode = new DSimpleXMLNode("isEnabled?");
    m_enableMulticoreThreadingNode->setValue(false);

    m_pulsePairChunkSizeNode = new DSimpleXMLNode("pulsePairChunkSize");
    m_pulsePairChunkSizeNode->setValue(1);

    (*m_parentNode) << m_lastSaveDateNode
                    << m_renderPointsNode
                    << m_lastSettingsFilePathNode
                    << m_lastStreamInputFilePathNode
                    << m_lastStreamTextFileInputFilePathNode
                    << m_lastSimulationInputFilePathNode
                    << m_lastSaveDataFilePathNode
                    << m_lastSaveScriptFilePathNode
                    << m_lastSaveLogFilePathNode
                    << m_lastSaveTProfileFilePathNode
                    << m_lastPHSSaveFilePathNode;

    (*m_multicoreThreadingParentNode) << m_enableMulticoreThreadingNode
                    << m_pulsePairChunkSizeNode;

     (*m_parentNode) << m_multicoreThreadingParentNode;
}

DRS4ProgramSettingsManager::~DRS4ProgramSettingsManager()
{
    DDELETE_SAFETY(m_multicoreThreadingParentNode);
    DDELETE_SAFETY(m_parentNode);
    DDELETE_SAFETY(__sharedInstanceProgramSettingsManager);
}

DRS4ProgramSettingsManager *DRS4ProgramSettingsManager::sharedInstance()
{
    if ( !__sharedInstanceProgramSettingsManager )
        __sharedInstanceProgramSettingsManager = new DRS4ProgramSettingsManager();

    return __sharedInstanceProgramSettingsManager;
}

void DRS4ProgramSettingsManager::setSplineIntraPoints(int points)
{
    QMutexLocker locker(&m_mutex);

    m_renderPointsNode->setValue(points);
    save();
}

bool DRS4ProgramSettingsManager::load()
{
    DSimpleXMLReader reader("settings" + EXT_PROGRAM_SETTINGS_FILE);
    DSimpleXMLTag tag;

    if ( !reader.readFromFile(&tag, true, PROGRAM_SETTINGS_ENCRYPTION_KEY) ) {
        m_lastSaveDateNode->setValue(QDateTime::currentDateTime().toString());
        m_renderPointsNode->setValue(10);
        m_lastSimulationInputFilePathNode->setValue("/home");
        m_lastSettingsFilePathNode->setValue("/home");
        m_lastSaveDataFilePathNode->setValue("/home");
        m_lastSaveScriptFilePathNode->setValue("/home");
        m_lastStreamInputFilePathNode->setValue("/home");
        m_lastStreamTextFileInputFilePathNode->setValue("/home");
        m_lastSaveLogFilePathNode->setValue("/home");
        m_lastSaveTProfileFilePathNode->setValue("/home");
        m_lastPHSSaveFilePathNode->setValue("/home");
        m_enableMulticoreThreadingNode->setValue(false);
        m_pulsePairChunkSizeNode->setValue(1);

        return true;
    }

    bool ok = false;
    const DSimpleXMLTag pTag = tag.getTag(m_parentNode, &ok);

    if ( !ok ) {
        m_lastSaveDateNode->setValue(QDateTime::currentDateTime().toString());
        m_renderPointsNode->setValue(10);
        m_lastSimulationInputFilePathNode->setValue("/home");
        m_lastSettingsFilePathNode->setValue("/home");
        m_lastSaveDataFilePathNode->setValue("/home");
        m_lastSaveScriptFilePathNode->setValue("/home");
        m_lastStreamInputFilePathNode->setValue("/home");
        m_lastStreamTextFileInputFilePathNode->setValue("/home");
        m_lastSaveLogFilePathNode->setValue("/home");
        m_lastSaveTProfileFilePathNode->setValue("/home");
        m_lastPHSSaveFilePathNode->setValue("/home");
        m_enableMulticoreThreadingNode->setValue(false);
        m_pulsePairChunkSizeNode->setValue(1);

        return true;
    }

    const QString lastDate = pTag.getValueAt(m_lastSaveDateNode, &ok).toString();
    if ( ok )
        m_lastSaveDateNode->setValue(lastDate);
    else
        m_lastSaveDateNode->setValue(QDateTime::currentDateTime().toString());

    const int intraPoints = pTag.getValueAt(m_renderPointsNode, &ok).toInt(&ok);
    if ( ok )
        m_renderPointsNode->setValue(intraPoints);
    else
        m_renderPointsNode->setValue(10);

   const QString pathSimu = pTag.getValueAt(m_lastSimulationInputFilePathNode, &ok).toString();
   if ( ok )
       m_lastSimulationInputFilePathNode->setValue(pathSimu);
   else
       m_lastSimulationInputFilePathNode->setValue("/home");

   const QString pathStream = pTag.getValueAt(m_lastStreamInputFilePathNode, &ok).toString();
   if ( ok )
       m_lastStreamInputFilePathNode->setValue(pathStream);
   else
       m_lastStreamInputFilePathNode->setValue("/home");

   const QString pathStreamTXT = pTag.getValueAt(m_lastStreamTextFileInputFilePathNode, &ok).toString();
   if ( ok )
       m_lastStreamTextFileInputFilePathNode->setValue(pathStreamTXT);
   else
       m_lastStreamTextFileInputFilePathNode->setValue("/home");

   const QString pathSett = pTag.getValueAt(m_lastSettingsFilePathNode, &ok).toString();
   if ( ok )
       m_lastSettingsFilePathNode->setValue(pathSett);
   else
       m_lastSettingsFilePathNode->setValue("/home");

   const QString pathSave = pTag.getValueAt(m_lastSaveDataFilePathNode, &ok).toString();
   if ( ok )
       m_lastSaveDataFilePathNode->setValue(pathSave);
   else
       m_lastSaveDataFilePathNode->setValue("/home");

   const QString pathScript = pTag.getValueAt(m_lastSaveScriptFilePathNode, &ok).toString();
   if ( ok )
       m_lastSaveScriptFilePathNode->setValue(pathScript);
   else
       m_lastSaveScriptFilePathNode->setValue("/home");

   const QString pathLog = pTag.getValueAt(m_lastSaveLogFilePathNode, &ok).toString();
   if ( ok )
       m_lastSaveLogFilePathNode->setValue(pathLog);
   else
       m_lastSaveLogFilePathNode->setValue("/home");

   const QString pathT = pTag.getValueAt(m_lastSaveTProfileFilePathNode, &ok).toString();
   if ( ok )
       m_lastSaveTProfileFilePathNode->setValue(pathT);
   else
       m_lastSaveTProfileFilePathNode->setValue("/home");

   const QString pathPHSSavePath = pTag.getValueAt(m_lastPHSSaveFilePathNode, &ok).toString();
   if ( ok )
       m_lastPHSSaveFilePathNode->setValue(pathPHSSavePath);
   else
       m_lastPHSSaveFilePathNode->setValue("/home");

   const DSimpleXMLTag pTagMulticoreThreading = pTag.getTag(m_multicoreThreadingParentNode, &ok);

   if (!ok)
   {
       m_enableMulticoreThreadingNode->setValue(false);
       m_pulsePairChunkSizeNode->setValue(1);

       return true;
   }

   const bool bThreading = pTagMulticoreThreading.getValueAt(m_enableMulticoreThreadingNode, &ok).toBool();
   if ( ok )
       m_enableMulticoreThreadingNode->setValue(bThreading);
   else
       m_enableMulticoreThreadingNode->setValue(false);

   const int count = pTagMulticoreThreading.getValueAt(m_pulsePairChunkSizeNode, &ok).toInt();
   if ( ok )
       m_pulsePairChunkSizeNode->setValue(count);
   else
       m_pulsePairChunkSizeNode->setValue(1);
}

bool DRS4ProgramSettingsManager::save()
{
    setLastSaveDate(QDateTime::currentDateTime());

    DSimpleXMLWriter writer("settings" + EXT_PROGRAM_SETTINGS_FILE);

    return writer.writeToFile(m_parentNode, true, PROGRAM_SETTINGS_ENCRYPTION_KEY.toStdString().c_str());
}

void DRS4ProgramSettingsManager::setSimulationInputFilePath(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    m_lastSimulationInputFilePathNode->setValue(path);
    save();
}

void DRS4ProgramSettingsManager::setStreamInputFilePath(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    m_lastStreamInputFilePathNode->setValue(path);
    save();
}

void DRS4ProgramSettingsManager::setStreamTextFileInputFilePath(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    m_lastStreamTextFileInputFilePathNode->setValue(path);
    save();
}

void DRS4ProgramSettingsManager::setSettingsFilePath(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    m_lastSettingsFilePathNode->setValue(path);
    save();
}

void DRS4ProgramSettingsManager::setSaveDataFilePath(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    m_lastSaveDataFilePathNode->setValue(path);
    save();
}

void DRS4ProgramSettingsManager::setSaveScriptFilePath(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    m_lastSaveScriptFilePathNode->setValue(path);
    save();
}

void DRS4ProgramSettingsManager::setSaveLogFilePath(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    m_lastSaveLogFilePathNode->setValue(path);
    save();
}

void DRS4ProgramSettingsManager::setSaveTProfilePath(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    m_lastSaveTProfileFilePathNode->setValue(path);
    save();
}

void DRS4ProgramSettingsManager::setSavePHSDataFilePath(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    m_lastPHSSaveFilePathNode->setValue(path);
    save();
}

void DRS4ProgramSettingsManager::setEnableMulticoreThreading(bool on)
{
    QMutexLocker locker(&m_mutex);

    m_enableMulticoreThreadingNode->setValue(on);
    save();
}

void DRS4ProgramSettingsManager::setPulsePairChunkSize(int size)
{
    QMutexLocker locker(&m_mutex);

    m_pulsePairChunkSizeNode->setValue(size);
    save();
}

int DRS4ProgramSettingsManager::splineIntraPoints()
{
    QMutexLocker locker(&m_mutex);

    load();

    bool ok = false;
    int val = m_renderPointsNode->getValue().toInt(&ok);

    if (!ok) {
        m_renderPointsNode->setValue(10);
        return 10;
    }

    return val;
}

void DRS4ProgramSettingsManager::setLastSaveDate(const QDateTime &dateTime)
{
    m_lastSaveDateNode->setValue(dateTime.toString());
}


QString DRS4ProgramSettingsManager::lastSaveDate()
{
    QMutexLocker locker(&m_mutex);

    load();
    return m_lastSaveDateNode->getValue().toString();
}

QString DRS4ProgramSettingsManager::simulationInputFilePath()
{
    QMutexLocker locker(&m_mutex);

    load();
    return m_lastSimulationInputFilePathNode->getValue().toString();
}

QString DRS4ProgramSettingsManager::streamInputFilePath()
{
    QMutexLocker locker(&m_mutex);

    load();
    return m_lastStreamInputFilePathNode->getValue().toString();
}

QString DRS4ProgramSettingsManager::streamTextFileInputFilePath()
{
    QMutexLocker locker(&m_mutex);

    load();
    return m_lastStreamTextFileInputFilePathNode->getValue().toString();
}

QString DRS4ProgramSettingsManager::settingsFilePath()
{
    QMutexLocker locker(&m_mutex);

    load();
    return m_lastSettingsFilePathNode->getValue().toString();
}

QString DRS4ProgramSettingsManager::saveDataFilePath()
{
    QMutexLocker locker(&m_mutex);

    load();
    return m_lastSaveDataFilePathNode->getValue().toString();
}

QString DRS4ProgramSettingsManager::saveScriptFilePath()
{
    QMutexLocker locker(&m_mutex);

    load();
    return m_lastSaveScriptFilePathNode->getValue().toString();
}

QString DRS4ProgramSettingsManager::saveLogFilePath()
{
    QMutexLocker locker(&m_mutex);

    load();
    return m_lastSaveLogFilePathNode->getValue().toString();
}

QString DRS4ProgramSettingsManager::saveTProfileFilePath()
{
    QMutexLocker locker(&m_mutex);

    load();
    return m_lastSaveTProfileFilePathNode->getValue().toString();
}

QString DRS4ProgramSettingsManager::savePHSDataFilePath()
{
    QMutexLocker locker(&m_mutex);

    load();
    return m_lastPHSSaveFilePathNode->getValue().toString();
}

bool DRS4ProgramSettingsManager::isMulticoreThreadingEnabled()
{
    QMutexLocker locker(&m_mutex);

    load();
    return m_enableMulticoreThreadingNode->getValue().toBool();
}

int DRS4ProgramSettingsManager::pulsePairChunkSize()
{
    QMutexLocker locker(&m_mutex);

    load();
    bool ok = false;
    int val = m_pulsePairChunkSizeNode->getValue().toInt(&ok);

    return (ok?val:1);
}

void DRS4ProgramSettingsManager::showXMLContent()
{
    m_parentNode->XMLMessageBox();
}

