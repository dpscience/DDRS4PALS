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

#ifndef DRS4STARTDLG_H
#define DRS4STARTDLG_H

#include "dversion.h"
#include "drs4boardmanager.h"
#include "UpdateNotifier/drs4updatenotifier.h"

#include "drs4boardcalibrationdlg.h"

#include "DLib.h"

#include <QMainWindow>
#include <QLibrary>

namespace Ui {
class DRS4StartDlg;
}

class DRS4StartDlg : public QMainWindow
{
    Q_OBJECT
public:
    explicit DRS4StartDlg(ProgramStartType *startType, QWidget *parent = DNULLPTR);
    virtual ~DRS4StartDlg();

public slots:
    void startDemoMode();
    void startDataAquistion();
    void openCalibration();

    void quit();
    void showAbout();

private slots:
    void showUpdateRequestStartUp();
    void showUpdateRequestLatestReleaseIsRunning();
    void showUpdateRequestError();
    void showUpdateRequestUpdateAvailable(QString tag, QString url, QDateTime releaseDateTime);
    void adaptCalibStatus();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent *event);

public:
    bool isFinished() const;

private:
    Ui::DRS4StartDlg *ui;

    bool m_simulationLibIsLoaded;
    bool m_drs4BoardConnected;
    bool m_isFinished;

    ProgramStartType *m_startType;

    DRS4BoardCalibrationDlg *m_calibDlg;
};

#endif // DRS4STARTDLG_H
