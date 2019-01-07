/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
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

#include "drs4boardmanager.h"

DRS4BoardManager *__sharedInstanceBoardManager = nullptr;

DRS4BoardManager::DRS4BoardManager() :
    m_drs(nullptr),
    m_drsBoard(nullptr),
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
        m_drsBoard = nullptr;

    if ( m_drsBoard )
        return true;
    else
        return false;
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

void DRS4BoardManager::log(const QString &logText)
{
    QMutexLocker locker(&m_mutex);

    QFile file("DRS4EvalBoardLog.log");
    QTextStream stream(&file);

    if ( file.open(QIODevice::ReadWrite|QIODevice::Append) ) {
        stream << logText << "\r\n";
        file.close();
    }
}

