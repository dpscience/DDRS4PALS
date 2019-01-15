/****************************************************************************
**
**  DDRS4PALS, a for the acquisition of lifetime spectra based on the DRS4 evaluation board
**  DRS4 Evaluation Board of PSI: https://www.psi.ch/drs/evaluation-board
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

#include "drs4streammanager.h"

#include "GUI/drs4scopedlg.h"

DRS4StreamManager *__sharedInstanceStreamManager = nullptr;

DRS4StreamManager::DRS4StreamManager()
{
    m_file = nullptr;
    m_guiAccess = nullptr;
    m_isArmed = false;
    m_contentInByte = 0;
}

DRS4StreamManager::~DRS4StreamManager()
{
    DDELETE_SAFETY(m_file);
}

DRS4StreamManager *DRS4StreamManager::sharedInstance()
{
    if ( !__sharedInstanceStreamManager )
        __sharedInstanceStreamManager = new DRS4StreamManager();

    return __sharedInstanceStreamManager;
}

void DRS4StreamManager::init(const QString &fileName, DRS4ScopeDlg *guiAccess)
{
    QMutexLocker locker(&m_mutex);

    m_guiAccess = guiAccess;

    DDELETE_SAFETY(m_file);

    m_file = new QFile(fileName);

    m_contentInByte = 0;
    m_isArmed = false;
}

bool DRS4StreamManager::start()
{
    QMutexLocker locker(&m_mutex);

    if ( m_file->open(QIODevice::ReadWrite) )
    {
        m_isArmed = true;

        DRS4PulseStreamHeader header;

        header.version = DATA_STREAM_VERSION;
        header.sweepInNanoseconds = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();
        header.sampleSpeedInGHz = DRS4SettingsManager::sharedInstance()->sampleSpeedInGHz();
        header.sampleDepth = kNumberOfBins;

        if (m_file->write((const char*)&header, sz_structDRS4PulseStreamHeader) != sz_structDRS4PulseStreamHeader)
        {
            m_file->close();

            m_isArmed = false;
            m_contentInByte = 0;

            return false;
        }

        m_contentInByte += sz_structDRS4PulseStreamHeader;

        m_guiAccess->addSampleSpeedWarningMessage(true, DRS4ScriptManager::sharedInstance()->isArmed());

        return true;
    }
    else
    {
        m_contentInByte = 0;
        m_isArmed = false;

        return false;
    }
}

void DRS4StreamManager::stopAndSave()
{
    QMutexLocker locker(&m_mutex);

    m_isArmed = false;
    m_contentInByte = 0;

    if ( m_file )
        m_file->close();

    m_guiAccess->addSampleSpeedWarningMessage(false, DRS4ScriptManager::sharedInstance()->isArmed());

    DDELETE_SAFETY(m_file);
}

bool DRS4StreamManager::write(const char* data, qint64 length)
{
    QMutexLocker locker(&m_mutex);

    m_contentInByte += length;

    return (m_file->write(data, length) == length);
}

bool DRS4StreamManager::isArmed() const
{
    QMutexLocker locker(&m_mutex);

    return m_isArmed;
}

QString DRS4StreamManager::fileName() const
{
    QMutexLocker locker(&m_mutex);

    if (m_file)
        return m_file->fileName();
    else
        return "";
}

qint64 DRS4StreamManager::streamedContentInBytes() const
{
    return m_contentInByte;
}


DRS4TextFileStreamManager *__sharedInstanceTextFileStreamManager = nullptr;

DRS4TextFileStreamManager::DRS4TextFileStreamManager()
{
    m_counts = 0;
    m_fileTag = "";
    m_file = nullptr;
    m_isArmed = false;
    m_counter = 0;

    m_pulseA = true;
    m_pulseB = true;
    m_interpolA = true;
    m_interpolB = true;
}

DRS4TextFileStreamManager::~DRS4TextFileStreamManager()
{
    DDELETE_SAFETY(m_file);
}

DRS4TextFileStreamManager *DRS4TextFileStreamManager::sharedInstance()
{
    if ( !__sharedInstanceTextFileStreamManager )
        __sharedInstanceTextFileStreamManager = new DRS4TextFileStreamManager();

    return __sharedInstanceTextFileStreamManager;
}

void DRS4TextFileStreamManager::start(const QString &fileName, int n, bool pulseA, bool pulseB, bool splineA, bool splineB)
{
    QMutexLocker locker(&m_mutex);

    m_counts = n;

    m_pulseA = pulseA;
    m_pulseB = pulseB;
    m_interpolA = splineA;
    m_interpolB = splineB;

    m_counter = 0;
    m_isArmed = false;
    m_fileTag = "";

    DDELETE_SAFETY(m_file);

    m_fileTag = fileName.split(".txt").first();

    m_isArmed = true;

    emit started();
}

void DRS4TextFileStreamManager::abort()
{
    QMutexLocker locker(&m_mutex);

    m_counter = m_counts;
    m_isArmed = false;

    emit finished();
}

void DRS4TextFileStreamManager::writePulses(float *timeA, float *timeB, float *waveA, float *waveB, int number)
{
    if ( !isArmed() )
        return;

    QMutexLocker locker(&m_mutex);

    m_counter ++;

    if ( !m_pulseA && !m_pulseB )
        return;

    if ( number < 1 )
        return;

    if ( !timeA || !timeB || !waveA || !waveB )
        return;

    DDELETE_SAFETY(m_file);

    m_file = new QFile(m_fileTag + "_pulse_" + QVariant(m_counter).toString() + ".txt");

    QTextStream stream(m_file);

    if ( m_file->open(QIODevice::WriteOnly) )
    {
        if ( m_pulseA )
            stream << "time [ns] (Pulse A)\tvoltage [mV] (Pulse A)\t";

        if ( m_pulseB )
            stream << "time [ns] (Pulse B)\tvoltage [mV] (Pulse B)";

        stream << "\n";

        for ( int i = 0 ; i < number ; ++ i )
        {
            const QString ext = m_pulseB?"\t":"";

            if ( m_pulseA )
                stream << QString::number(timeA[i], 'f', 6) + "\t" + QString::number(waveA[i], 'f', 6) << ext;

            if ( m_pulseB )
                 stream << QString::number(timeB[i], 'f', 6) + "\t" + QString::number(waveB[i], 'f', 6);

            stream << "\n";
        }

        m_file->close();
    }

    DDELETE_SAFETY(m_file);

    if ( m_counter > m_counts )
    {
        if ( !m_interpolA && !m_interpolB )
        {
            m_isArmed = false;

            emit finished();
        }
    }
}

void DRS4TextFileStreamManager::writePulsesd(double *timeA, double *timeB, double *waveA, double *waveB, int number)
{
    if ( !isArmed() )
        return;

    QMutexLocker locker(&m_mutex);

    m_counter ++;

    if ( !m_pulseA && !m_pulseB )
        return;

    if ( number < 1 )
        return;

    if ( !timeA || !timeB || !waveA || !waveB )
        return;

    DDELETE_SAFETY(m_file);

    m_file = new QFile(m_fileTag + "_pulse_" + QVariant(m_counter).toString() + ".txt");

    QTextStream stream(m_file);

    if ( m_file->open(QIODevice::WriteOnly) )
    {
        if ( m_pulseA )
            stream << "time [ns] (Pulse A)\tvoltage [mV] (Pulse A)\t";

        if ( m_pulseB )
            stream << "time [ns] (Pulse B)\tvoltage [mV] (Pulse B)";

        stream << "\n";

        for ( int i = 0 ; i < number ; ++ i )
        {
            const QString ext = m_pulseB?"\t":"";

            if ( m_pulseA )
                stream << QString::number(timeA[i], 'f', 6) + "\t" + QString::number(waveA[i], 'f', 6) << ext;

            if ( m_pulseB )
                 stream << QString::number(timeB[i], 'f', 6) + "\t" + QString::number(waveB[i], 'f', 6);

            stream << "\n";
        }

        m_file->close();
    }

    DDELETE_SAFETY(m_file);

    if ( m_counter > m_counts )
    {
        if ( !m_interpolA && !m_interpolB )
        {
            m_isArmed = false;

            emit finished();
        }
    }
}

void DRS4TextFileStreamManager::writeInterpolations(QVector<QPointF> *interpolationA, QVector<QPointF> *interpolationB, const DRS4InterpolationType::type& interpolationType, const DRS4SplineInterpolationType::type& splineInterpolationType)
{
    if ( !isArmed() )
        return;

    QMutexLocker locker(&m_mutex);

    if ( !m_interpolA && !m_interpolB )
        return;

     if ( !interpolationA || !interpolationB )
         return;

     if ( interpolationA->size() != interpolationB->size() )
         return;

     DDELETE_SAFETY(m_file);

     m_file = new QFile(m_fileTag + "_interpolation_" + QVariant(m_counter).toString() + ".txt");

     QTextStream stream(m_file);

     if ( m_file->open(QIODevice::WriteOnly) ) {
         stream << "#interpolation-type:\t" << ((interpolationType == DRS4InterpolationType::type::polynomial)?QString("polynomial"):QString("spline")) << "\n";

         if (interpolationType == DRS4InterpolationType::type::spline) {
             QString splineType;
             switch (splineInterpolationType) {
             case DRS4SplineInterpolationType::type::linear:
                 splineType = "linear";
                 break;
             case DRS4SplineInterpolationType::type::cubic:
                 splineType = "cubic - ALGLIB";
                 break;
             case DRS4SplineInterpolationType::type::akima:
                 splineType = "akima - ALGLIB";
                 break;
             case DRS4SplineInterpolationType::type::catmullRom:
                 splineType = "catmull-rom - ALGLIB";
                 break;
             case DRS4SplineInterpolationType::type::monotone:
                 splineType = "monotone - ALGLIB";
                 break;
             case DRS4SplineInterpolationType::type::tk_cubic:
                 splineType = "cubic - Tino Kluge";
                 break;
             default:
                 splineType = "cubic - ALGLIB";
                 break;
             }

             stream << "#spline-type:\t" << splineType << "\n\n";
         }

         if ( m_pulseA )
             stream << "time [ns] (Interpolation A)\tvoltage [mV] (Interpolation A)\t";

         if ( m_pulseB )
             stream << "time [ns] (Interpolation B)\tvoltage [mV] (Interpolation B)";

         stream << "\n";

         for ( int i = 0 ; i < interpolationA->size() ; ++ i )
         {
             const QString ext = m_pulseB?"\t":"";

             if ( m_interpolA )
                 stream << QString::number(interpolationA->at(i).x(), 'f', 6) + "\t" + QString::number(interpolationA->at(i).y(), 'f', 6) << ext;

             if ( m_interpolB )
                  stream << QString::number(interpolationB->at(i).x(), 'f', 6) + "\t" + QString::number(interpolationB->at(i).y(), 'f', 6);

             stream << "\n";
         }

         m_file->close();
     }

     DDELETE_SAFETY(m_file);

     if ( m_counter > m_counts )
     {
             m_isArmed = false;
             emit finished();
     }
}

bool DRS4TextFileStreamManager::writeInterpolationA() const
{
    return m_interpolA;
}

bool DRS4TextFileStreamManager::writeInterpolationB() const
{
    return m_interpolB;
}

bool DRS4TextFileStreamManager::isArmed() const
{
    QMutexLocker locker(&m_mutex);

    return m_isArmed;
}


DRS4TextFileStreamRangeManager *__sharedInstanceTextFileStreamRangeManager = nullptr;

DRS4TextFileStreamRangeManager::DRS4TextFileStreamRangeManager()
{
    m_counts = 0;
    m_fileTag = "";
    m_file = nullptr;
    m_isArmed = false;
    m_counter = 0;

    m_pulseAB = true;
    m_pulseBA = true;

    m_pulseABMin = 0.0f;
    m_pulseABMax = 0.0f;

    m_pulseBAMin = 0.0f;
    m_pulseBAMax = 0.0f;
}

DRS4TextFileStreamRangeManager::~DRS4TextFileStreamRangeManager()
{
    DDELETE_SAFETY(m_file);
}

DRS4TextFileStreamRangeManager *DRS4TextFileStreamRangeManager::sharedInstance()
{
    if ( !__sharedInstanceTextFileStreamRangeManager )
        __sharedInstanceTextFileStreamRangeManager = new DRS4TextFileStreamRangeManager();

    return __sharedInstanceTextFileStreamRangeManager;
}

void DRS4TextFileStreamRangeManager::start(const QString &fileName, int n, bool pulseAB, bool pulseBA)
{
    QMutexLocker locker(&m_mutex);

    m_counts = n;

    m_pulseAB = pulseAB;
    m_pulseBA = pulseBA;

    m_counter = 0;
    m_isArmed = false;
    m_fileTag = "";

    DDELETE_SAFETY(m_file);

    m_fileTag = fileName.split(".txt").first();

    m_isArmed = true;

    emit started();
}

void DRS4TextFileStreamRangeManager::abort()
{
    QMutexLocker locker(&m_mutex);

    m_counter = m_counts;
    m_isArmed = false;

    emit finished();
}

void DRS4TextFileStreamRangeManager::writePulses(QVector<QPointF> *pulseA, QVector<QPointF> *pulseB, const double& cfdValueA, const double& cfdValueB)
{
    if ( !isArmed() )
        return;

    QMutexLocker locker(&m_mutex);

    m_counter ++;

    if ( !m_pulseAB && !m_pulseBA )
        return;

    if ( !pulseA || !pulseB )
        return;

    DDELETE_SAFETY(m_file);

    m_file = new QFile(m_fileTag + "_pulse_" + QVariant(m_counter).toString() + ".txt");

    QTextStream stream(m_file);

    if ( m_file->open(QIODevice::WriteOnly) )
    {
        stream << "time [ns] (Pulse A)\tvoltage [mV] (Pulse A)\t";
        stream << "time [ns] (Pulse B)\tvoltage [mV] (Pulse B)\t";
        stream << "cfd-Value A [ns]\tmax. amplitude A [mV]\t";
        stream << "cfd-Value B [ns]\tmax. amplitude B [mV]\n";

        for ( int i = 0 ; i < pulseA->size() ; ++ i )
        {
            if ( i == 0 ) {
                stream << QString::number(pulseA->at(i).x(), 'f', 6) + "\t" + QString::number(pulseA->at(i).y(), 'f', 6) << "\t";
                stream << QString::number(pulseB->at(i).x(), 'f', 6) + "\t" + QString::number(pulseB->at(i).y(), 'f', 6) << "\t";
                stream << QString::number(cfdValueA, 'f', 6) << "\t" << "-500.0" << "\t";
                stream << QString::number(cfdValueB, 'f', 6) << "\t" << "-500.0";
            }
            else if ( i == 1 ) {
                stream << QString::number(pulseA->at(i).x(), 'f', 6) + "\t" + QString::number(pulseA->at(i).y(), 'f', 6) << "\t";
                stream << QString::number(pulseB->at(i).x(), 'f', 6) + "\t" + QString::number(pulseB->at(i).y(), 'f', 6) << "\t";
                stream << QString::number(cfdValueA, 'f', 6) << "\t" << "500.0" << "\t";
                stream << QString::number(cfdValueB, 'f', 6) << "\t" << "500.0";
            }
            else {
                stream << QString::number(pulseA->at(i).x(), 'f', 6) + "\t" + QString::number(pulseA->at(i).y(), 'f', 6) << "\t";
                stream << QString::number(pulseB->at(i).x(), 'f', 6) + "\t" + QString::number(pulseB->at(i).y(), 'f', 6);
            }

            stream << "\n";
        }

        m_file->close();
    }

    DDELETE_SAFETY(m_file);

    if ( m_counter > m_counts )
    {
         m_isArmed = false;

         emit finished();
    }
}

void DRS4TextFileStreamRangeManager::setLTRangeMinAB(double val)
{
    QMutexLocker locker(&m_mutex);

    m_pulseABMin = val;
}

void DRS4TextFileStreamRangeManager::setLTRangeMaxAB(double val)
{
    QMutexLocker locker(&m_mutex);

    m_pulseABMax = val;
}

void DRS4TextFileStreamRangeManager::setLTRangeMinBA(double val)
{
    QMutexLocker locker(&m_mutex);

    m_pulseBAMin = val;
}

void DRS4TextFileStreamRangeManager::setLTRangeMaxBA(double val)
{
    QMutexLocker locker(&m_mutex);

    m_pulseBAMax = val;
}

double DRS4TextFileStreamRangeManager::ltRangeMinAB() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseABMin;
}

double DRS4TextFileStreamRangeManager::ltRangeMaxAB() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseABMax;
}

double DRS4TextFileStreamRangeManager::ltRangeMinBA() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseBAMin;
}

double DRS4TextFileStreamRangeManager::ltRangeMaxBA() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseBAMax;
}

bool DRS4TextFileStreamRangeManager::isABEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseAB;
}

bool DRS4TextFileStreamRangeManager::isBAEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_pulseBA;
}

bool DRS4TextFileStreamRangeManager::isArmed() const
{
    QMutexLocker locker(&m_mutex);

    return m_isArmed;
}
