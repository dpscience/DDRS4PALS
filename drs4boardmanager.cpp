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

#include "drs4boardmanager.h"

static DRS4BoardManager *__sharedInstanceBoardManager = DNULLPTR;

DRS4BoardManager::DRS4BoardManager() :
    m_drs(DNULLPTR),
    m_drsBoard(DNULLPTR),
    m_demoMode(false),
    m_demoFromStreamData(false) {}

DRS4BoardManager::~DRS4BoardManager()
{
    DDELETE_SAFETY(m_drs);
    DDELETE_SAFETY(m_drsBoard);
    DDELETE_SAFETY(__sharedInstanceBoardManager);
}

DRS4BoardManager *DRS4BoardManager::sharedInstance()
{
    if ( !__sharedInstanceBoardManager )
        __sharedInstanceBoardManager = new DRS4BoardManager();

    return __sharedInstanceBoardManager;
}

bool DRS4BoardManager::connect()
{
    QMutexLocker locker(&m_mutex);

    m_drs = new DRS;

    if ( !m_drs )
        return false;

    if ( m_drs->GetNumberOfBoards() == 1 )
        m_drsBoard = m_drs->GetBoard(m_drs->GetNumberOfBoards()-1);
    else
        m_drsBoard = DNULLPTR;

    if ( m_drsBoard )
        return true;
    else
        return false;
}

bool DRS4BoardManager::isConnected() const
{
    return m_drsBoard != DNULLPTR;
}

DRSBoard *DRS4BoardManager::currentBoard() const
{
    QMutexLocker locker(&m_mutex);

    return m_drsBoard;
}

void DRS4BoardManager::setDemoMode(bool demoMode)
{
    QMutexLocker locker(&m_mutex);

    m_demoMode = demoMode;
}

void DRS4BoardManager::setDemoFromStreamData(bool usingStreamData)
{
    QMutexLocker locker(&m_mutex);

    m_demoFromStreamData = usingStreamData;
}

bool DRS4BoardManager::isDemoModeEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return m_demoMode;
}

bool DRS4BoardManager::usingStreamDataOnDemoMode() const
{
    QMutexLocker locker(&m_mutex);

    return m_demoFromStreamData;
}

QJsonDocument DRS4BoardManager::hardwareInfo() const
{
    QJsonObject main;

    /* specifications */
    QJsonObject specs;

    specs["no. of inputs"] = currentBoard()->GetNumberOfInputs();
    specs["no. of channels"] = currentBoard()->GetNumberOfChannels();
    specs["no. of chips"] = currentBoard()->GetNumberOfChips();
    specs["no. of readout-channels"] = currentBoard()->GetNumberOfReadoutChannels();
    specs["firmware-version"] = currentBoard()->GetFirmwareVersion();
    specs["chip-type"] = currentBoard()->GetDRSType();
    specs["board-type"] = currentBoard()->GetBoardType();
    specs["serial-number"] = currentBoard()->GetBoardSerialNumber();

    /* calibration */
    QJsonObject calib;

    calib["calibrated temperature"] = QString::number(currentBoard()->GetCalibratedTemperature(), 'f', 2) + QString(" °C");
    calib["calibrated voltage input-range"] = currentBoard()->GetCalibratedInputRange();
    calib["is voltage-calibration valid?"] = currentBoard()->IsVoltageCalibrationValid() ? "true" : "false";
    calib["calibrated frequency"] = QString::number(currentBoard()->GetCalibratedFrequency(), 'f', 2) + QString(" GHz");
    calib["is timing-calibration valid?"] = currentBoard()->IsTimingCalibrationValid() ? "true" : "false";

    /* trigger */
    QJsonObject trigger;

    trigger["trigger-delay"] = QString::number(currentBoard()->GetTriggerDelayNs(), 'f', 2) + QString(" ns");
    trigger["trigger-source"] = currentBoard()->GetTriggerSource();

    main["trigger-configuration"] = trigger;
    main["calibration"] = calib;
    main["board-temperature"] = QString::number(currentBoard()->GetTemperature(), 'f', 2) + QString(" °C");
    main["true-frequency"] = QString::number(currentBoard()->GetTrueFrequency(), 'f', 2) + QString(" GHz");
    main["nominal-frequency"] = QString::number(currentBoard()->GetNominalFrequency(), 'f', 2) + QString(" GHz");
    main["voltage input-range"] = currentBoard()->GetInputRange();
    main["specifications"] = specs;

    return QJsonDocument(main);
}
