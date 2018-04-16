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

#ifndef DRS4STREAMDATALOADER_H
#define DRS4STREAMDATALOADER_H

#include "DLib.h"

#include "GUI/drs4scopedlg.h"

#include "drs4boardmanager.h"
#include "drs4streammanager.h"

#include <QFile>

#include <QMutex>
#include <QMutexLocker>

class DRS4StreamDataLoader : public QObject
{
    Q_OBJECT

    DRS4StreamDataLoader();
    virtual ~DRS4StreamDataLoader();

    QFile *m_file;
    bool m_isArmed;

    DRS4ScopeDlg *m_guiAccess;

    qint64 m_fileSize;
    qint64 m_loadedSize;

    mutable QMutex m_mutex;

public:
    static DRS4StreamDataLoader *sharedInstance();

    bool init(const QString& fileName, DRS4ScopeDlg *guiAccess, bool accessFromScript = false);
    bool stop();

    bool isArmed() const;
    QString fileName() const;

    inline qint64 fileSizeInMegabyte() const { QMutexLocker locker(&m_mutex); return ((qint64)((float)m_fileSize/1024.0f)*0.001); }
    inline qint64 loadedFileSizeInMegabyte() const { QMutexLocker locker(&m_mutex); return ((qint64)((float)m_loadedSize/1024.0f)*0.001); }

    bool receiveGeneratedPulsePair(float *pulseATime, float *pulseAVoltage, float *pulseBTime, float *pulseBVoltage);

signals:
    void started();
    void finished();
};

#endif // DRS4STREAMDATALOADER_H
