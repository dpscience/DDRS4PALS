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

#ifndef DRS4STREAMMANAGER_H
#define DRS4STREAMMANAGER_H

#include <QFile>

#include <QMutex>
#include <QMutexLocker>

#include "DRS/drs507/DRS.h"

#include "drs4settingsmanager.h"
#include "dversion.h"

#include "DLib.h"

typedef struct
{
    quint32 version;
    double sweepInNanoseconds;
    double sampleSpeedInGHz;
    qint32 sampleDepth;
} DRS4PulseStreamHeader;

#define sz_structDRS4PulseStreamHeader sizeof(DRS4PulseStreamHeader)

class DRS4ScopeDlg;

class DRS4StreamManager
{
    DRS4StreamManager();
    virtual ~DRS4StreamManager();

    QFile *m_file;
    bool m_isArmed;

    qint64 m_contentInByte;

    DRS4ScopeDlg *m_guiAccess;

    mutable QMutex m_mutex;

public:
    static DRS4StreamManager *sharedInstance();

    void init(const QString& fileName, DRS4ScopeDlg *guiAccess);

    bool start();
    void stopAndSave();

    bool write(const char *data, qint64 length);

    bool isArmed() const;

    QString fileName() const;

    qint64 streamedContentInBytes() const;
};

class DRS4FalseTruePulseStreamManager
{
    DRS4FalseTruePulseStreamManager();
    virtual ~DRS4FalseTruePulseStreamManager();

    QFile *m_fileTrue;
    QFile *m_fileFalse;

    QString m_nameLiteral;

    bool m_isArmed;

    qint64 m_contentInByte;

    bool m_bStreamForABranch;

    DRS4ScopeDlg *m_guiAccess;

    mutable QMutex m_mutex;

public:
    static DRS4FalseTruePulseStreamManager *sharedInstance();

    void init(const QString& fileName, DRS4ScopeDlg *guiAccess);

    bool start(bool bA);
    void stopAndSave();

    bool writeTruePulse(const char *data, qint64 length);
    bool writeFalsePulse(const char *data, qint64 length);

    bool isArmed() const;
    bool isStreamingForABranch() const;

    QString fileName() const;

    qint64 streamedContentInBytes() const;
};

class DRS4TextFileStreamManager : public QObject
{
    Q_OBJECT

    DRS4TextFileStreamManager();
    virtual ~DRS4TextFileStreamManager();

    QFile *m_file;
    QString m_fileTag;
    bool m_isArmed;

    int m_counter;
    int m_counts;

    bool m_pulseA;
    bool m_pulseB;
    bool m_interpolA;
    bool m_interpolB;

    mutable QMutex m_mutex;

signals:
    void started();
    void finished();

public slots:
    static DRS4TextFileStreamManager *sharedInstance();

    void start(const QString& fileName, int n, bool pulseA, bool pulseB, bool splineA, bool splineB);
    void abort();

    void writePulses(float *timeA, float *timeB, float *waveA, float *waveB, int number);
    void writePulsesd(double *timeA, double *timeB, double *waveA, double *waveB, int number);
    void writeInterpolations(QVector<QPointF> *interpolationA, QVector<QPointF> *interpolationB, const DRS4InterpolationType::type &interpolationType, const DRS4SplineInterpolationType::type &splineInterpolationType);

    bool writeInterpolationA() const;
    bool writeInterpolationB() const;

    bool isArmed() const;
};


class DRS4TextFileStreamRangeManager : public QObject
{
    Q_OBJECT

    DRS4TextFileStreamRangeManager();
    virtual ~DRS4TextFileStreamRangeManager();


    QFile *m_file;
    QString m_fileTag;
    bool m_isArmed;

    int m_counter;
    int m_counts;

    bool m_pulseAB;
    bool m_pulseBA;

    double m_pulseABMin, m_pulseABMax;
    double m_pulseBAMin, m_pulseBAMax;

    mutable QMutex m_mutex;

signals:
    void started();
    void finished();

public slots:
    static DRS4TextFileStreamRangeManager *sharedInstance();

    void start(const QString& fileName, int n, bool pulseAB, bool pulseBA);
    void abort();

    void writePulses(QVector<QPointF> *pulseA, QVector<QPointF> *pulseB, const double &cfdValueA, const double &cfdValueB);

    void setLTRangeMinAB(double val);
    void setLTRangeMaxAB(double val);

    void setLTRangeMinBA(double val);
    void setLTRangeMaxBA(double val);

    double ltRangeMinAB() const;
    double ltRangeMaxAB() const;

    double ltRangeMinBA() const;
    double ltRangeMaxBA() const;

    bool isABEnabled() const;
    bool isBAEnabled() const;

    bool isArmed() const;
};

#endif // DRS4STREAMMANAGER_H
