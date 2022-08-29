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

#include "drs4streamdataloader.h"

DRS4StreamDataLoader *__sharedInstanceStreamDataLoader = DNULLPTR;

DRS4StreamDataLoader::DRS4StreamDataLoader() :
    m_file(DNULLPTR),
    m_guiAccess(DNULLPTR),
    m_isArmed(false),
    m_fileSize(0),
    m_loadedSize(0) {}

DRS4StreamDataLoader::~DRS4StreamDataLoader()
{
    DDELETE_SAFETY(m_file);
}

DRS4StreamDataLoader *DRS4StreamDataLoader::sharedInstance()
{
    if ( !__sharedInstanceStreamDataLoader )
        __sharedInstanceStreamDataLoader = new DRS4StreamDataLoader();

    return __sharedInstanceStreamDataLoader;
}

bool DRS4StreamDataLoader::init(const QString &fileName, DRS4ScopeDlg *guiAccess, bool accessFromScript)
{
    m_guiAccess = guiAccess;

    DDELETE_SAFETY(m_file);
    m_file = new QFile(fileName);

    if ( m_file->open(QIODevice::ReadWrite) ) {
        QFileInfo info(*m_file);
        m_fileSize = info.size(); // [Byte]
        m_loadedSize = 0;

        DRS4PulseStreamHeader header;

        if (m_file->read((char*)&header, sz_structDRS4PulseStreamHeader) == sz_structDRS4PulseStreamHeader) {
            m_loadedSize += sz_structDRS4PulseStreamHeader;

            if ( header.version <=  DATA_STREAM_VERSION ) {
                if (!accessFromScript ) {
                    if ( !qFuzzyCompare(header.sampleSpeedInGHz,  DRS4SettingsManager::sharedInstance()->sampleSpeedInGHz())
                         || !qFuzzyCompare(header.sweepInNanoseconds,  DRS4SettingsManager::sharedInstance()->sweepInNanoseconds()) ) {
                            const QString text = "Stream was recorded with the following Sample-Speed: \n" + QString::number(header.sampleSpeedInGHz, 'f', 2) + "GHz [" + QString::number(header.sweepInNanoseconds, 'f', 0) + "ns].\n"\
                                                                                                                                                                                                                          "\nSample-Speed will be adapted to this Values!";
                            const int ret = QMessageBox::warning(NULL, "Stream was recorded with different Settings!", text, QMessageBox::Ok, QMessageBox::NoButton);

                            DUNUSED_PARAM(ret);
                    }
                }

                m_guiAccess->changeSampleSpeed(m_guiAccess->getIndexForSweep(header.sweepInNanoseconds), accessFromScript);
                m_guiAccess->addSampleSpeedWarningMessage(true, accessFromScript);
            }
        }

        m_isArmed = true;
        emit started();

        return true;
    }
    else {
        m_fileSize = 0;
        m_loadedSize = 0;

        m_isArmed = false;
        emit finished();

        return false;
    }
}

bool DRS4StreamDataLoader::stop()
{
    QMutexLocker locker(&m_mutex);

    m_fileSize = 0;
    m_loadedSize = 0;

    m_isArmed = false;

    m_guiAccess->addSampleSpeedWarningMessage(false, DRS4ScriptManager::sharedInstance()->isArmed());

    emit finished();

    if ( m_file )
        m_file->close();
    else
        return false;

    DDELETE_SAFETY(m_file);

    return true;
}

bool DRS4StreamDataLoader::isArmed() const
{
    QMutexLocker locker(&m_mutex);

    return m_isArmed;
}

QString DRS4StreamDataLoader::fileName() const
{
    QMutexLocker locker(&m_mutex);

    if (m_file)
        return m_file->fileName();
    else
        return "";
}

bool DRS4StreamDataLoader::receiveGeneratedPulsePair(float *pulseATime, float *pulseAVoltage, float *pulseBTime, float *pulseBVoltage)
{ 
    QMutexLocker locker(&m_mutex);

    if ( !m_file )
    {
        locker.unlock();
        stop();
        return false;
    }

    const qint64 size = sizeof(float)*kNumberOfBins;

    if ( m_file->atEnd() )
    {
        locker.unlock();
        stop();
        return false;
    }

    if ( m_file->read((char*)pulseATime, size) != size )
    {
        locker.unlock();
        stop();
        return false;
    }

    if ( m_file->read((char*)pulseAVoltage, size) != size )
    {
        locker.unlock();
        stop();
        return false;
    }

    if ( m_file->read((char*)pulseBTime, size) != size )
    {
        locker.unlock();
        stop();
        return false;
    }

    if ( m_file->read((char*)pulseBVoltage, size) != size )
    {
        locker.unlock();
        stop();
        return false;
    }

    m_loadedSize += 4*size;


    return true;
}

