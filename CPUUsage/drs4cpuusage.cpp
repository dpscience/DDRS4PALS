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

#include "drs4cpuusage.h"

static DRS4CPUUsageManager *__sharedInstanceDRS4CPUUsageManager = DNULLPTR;

float DRS4CPUUsageManager::releaseCPULoad()
{
    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(GetCurrentProcess(), &ftime, &ftime, &fsys, &fuser);

    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));

    double percent = (sys.QuadPart - m_lastSysCPU.QuadPart) + (user.QuadPart - m_lastUserCPU.QuadPart);

    percent /= (now.QuadPart - m_lastCPU.QuadPart);
    percent /= m_sysInfo.dwNumberOfProcessors;

    m_lastCPU = now;
    m_lastUserCPU = user;
    m_lastSysCPU = sys;

    return percent * 100;
}

void DRS4CPUUsageManager::start(int intervalInMilliseconds)
{
    if (intervalInMilliseconds != -1)
        m_interval = intervalInMilliseconds;

    m_timer.setInterval(int(m_interval*0.2)); // averaging over 5 cycles
    m_timer.setSingleShot(false);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(cpuLoad()));

    m_avgCPUUsage = 0.0;
    m_avgCounter = 0;

    FILETIME ftime, fsys, fuser;

    GetSystemInfo(&m_sysInfo);

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&m_lastCPU, &ftime, sizeof(FILETIME));

    GetProcessTimes(GetCurrentProcess(), &ftime, &ftime, &fsys, &fuser);
    memcpy(&m_lastSysCPU, &fsys, sizeof(FILETIME));
    memcpy(&m_lastUserCPU, &fuser, sizeof(FILETIME));

    m_timer.start();
}

void DRS4CPUUsageManager::stop()
{
    m_timer.stop();

    m_avgCPUUsage = 0.0;
    m_avgCounter = 0;

    disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(cpuLoad()));
}

void DRS4CPUUsageManager::setInterval(int intervalInMilliseconds)
{
    m_interval = intervalInMilliseconds;
}

void DRS4CPUUsageManager::cpuLoad()
{
    m_avgCPUUsage += releaseCPULoad();
    m_avgCounter ++;

    if (m_avgCounter == 5) {
        m_avgCPUUsage /= (float)m_avgCounter;

        emit cpu((int)m_avgCPUUsage);

        m_avgCounter = 0;
        m_avgCPUUsage = 0.0;
    }
}

int DRS4CPUUsageManager::interval() const
{
    return m_interval;
}

DRS4CPUUsageManager *DRS4CPUUsageManager::sharedInstance()
{
    if (!__sharedInstanceDRS4CPUUsageManager)
        __sharedInstanceDRS4CPUUsageManager = new DRS4CPUUsageManager;

    return __sharedInstanceDRS4CPUUsageManager;
}

DRS4CPUUsageManager::DRS4CPUUsageManager(QObject *parent) :
    QObject(parent)
{
    m_avgCPUUsage = 0.0;
    m_avgCounter = 0;

    m_interval = 1000;
}

DRS4CPUUsageManager::~DRS4CPUUsageManager()
{
    m_timer.stop();

    DDELETE_SAFETY(__sharedInstanceDRS4CPUUsageManager);
}
