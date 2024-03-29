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

#ifndef DRS4BOARDCALIBRATIONDLG_H
#define DRS4BOARDCALIBRATIONDLG_H

#include "dversion.h"
#include "drs4boardmanager.h"
#include "DRS/drs507/DRS.h"

#include "DLib.h"

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class DRS4BoardCalibrationDlg;
}

class DRS4BoardCalibrationDlg : public QMainWindow, public DRSCallback {
    Q_OBJECT

public:
    explicit DRS4BoardCalibrationDlg(QWidget *parent = DNULLPTR);
    virtual ~DRS4BoardCalibrationDlg();

    virtual void Progress(int value);

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void runVoltageCalibration();
    void runTimingCalibration();

    void changeSampleFreq(int id);
    void updateStatus(bool bConnected = true);

private:
    void enableControls(bool on);

signals:
    void progress(int value);
    void statusChanged();

private:
    Ui::DRS4BoardCalibrationDlg *ui;

    QTimer m_statusReqTimer;

    QLabel *m_infoLabel;
    QLabel *m_statusLabelV;
    QLabel *m_statusLabelT;
    QLabel *m_statusBoardTemp;

    bool m_bIsRunning;
};

#endif // DRS4BOARDCALIBRATIONDLG_H
