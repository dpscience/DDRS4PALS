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

#ifndef DRS4STREAMMANAGER_H
#define DRS4STREAMMANAGER_H

#include <QFile>

#include <QMutex>
#include <QMutexLocker>

#include "DRS/DRS.h"

#include "drs4settingsmanager.h"
#include "dversion.h"

#include "DLib.h"

typedef struct
{
    unsigned int version;
    double sweepInNanoseconds;
    double sampleSpeedInGHz;
    int sampleDepth;
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
