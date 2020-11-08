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

#ifndef DRS4CPUUSAGE_H
#define DRS4CPUUSAGE_H

#include <QObject>
#include <QTimer>

#include "dversion.h"
#include "DLib.h"

#include <Windows.h>

class DRS4CPUUsageManager : public QObject
{
    Q_OBJECT
public slots:
    void start(int intervalInMilliseconds = -1);
    void stop();

    void setInterval(int intervalInMilliseconds);

public:
    static DRS4CPUUsageManager *sharedInstance();

    int interval() const;

signals:
    void cpu(int);

private slots:
    void cpuLoad();

private:
    DRS4CPUUsageManager(QObject *parent = DNULLPTR);
    virtual ~DRS4CPUUsageManager();

    float releaseCPULoad();

    QTimer m_timer;
    int m_interval;

    float m_avgCPUUsage;
    int m_avgCounter;

    SYSTEM_INFO m_sysInfo;

    ULARGE_INTEGER m_lastCPU;
    ULARGE_INTEGER m_lastUserCPU;
    ULARGE_INTEGER m_lastSysCPU;
};

#endif // DRS4CPUUSAGE_H
