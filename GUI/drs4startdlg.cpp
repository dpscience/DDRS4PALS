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

#include "drs4startdlg.h"
#include "ui_drs4startdlg.h"

DRS4StartDlg::DRS4StartDlg(ProgramStartType *startType, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DRS4StartDlg),
    m_startType(startType),
    m_drs4BoardConnected(true),
    m_simulationLibIsLoaded(true),
    m_isFinished(false) {
    ui->setupUi(this);

    ui->toolButton->hide();

    setWindowTitle(PROGRAM_NAME);

    *m_startType = ProgramStartType::Abort;
}

DRS4StartDlg::~DRS4StartDlg() {
    DDELETE_SAFETY(ui);
}

void DRS4StartDlg::startDemoMode() {
    *m_startType = ProgramStartType::Simulation;

    close();
}

void DRS4StartDlg::startDataAquistion() {
    *m_startType = ProgramStartType::DataAcquisition;

    close();
}

void DRS4StartDlg::quit() {
    *m_startType = ProgramStartType::Abort;

    close();
}

void DRS4StartDlg::showAbout() {
    SHOW_ABOUT();
}

void DRS4StartDlg::showUpdateRequestError() {
    ui->toolButton->hide();

    ui->updateLabel->setStyleSheet("color: red");
    ui->updateLabel->setText("no internet connection. cannot check for updates on GitHub.");
}

void DRS4StartDlg::showUpdateRequestUpdateAvailable(QString tag, QString url, QDateTime releaseDateTime) {
    ui->toolButton->show();

    connect(ui->toolButton, SIGNAL(clicked()), DRS4UpdateNotifier::sharedInstance(), SLOT(redirectToUpdate()));

    ui->updateLabel->setStyleSheet("color: blue");
    ui->updateLabel->setText("update to DDRS4PALS v" + tag + " (" + releaseDateTime.toString() + ") is available ... ");
}

void DRS4StartDlg::showUpdateRequestStartUp() {
    ui->toolButton->hide();

    ui->updateLabel->setStyleSheet("color: blue");
    ui->updateLabel->setText("checking for updates on GitHub ... ");
}

void DRS4StartDlg::showUpdateRequestLatestReleaseIsRunning() {
    ui->toolButton->hide();

    ui->updateLabel->setStyleSheet("color: green");
    ui->updateLabel->setText("you are running the latest release of DDRS4PALS.");
}

void DRS4StartDlg::showEvent(QShowEvent *event) {
    connect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(checkingUpdatesStarted()), this, SLOT(showUpdateRequestStartUp()));
    connect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(latestReleaseRunning()), this, SLOT(showUpdateRequestLatestReleaseIsRunning()));
    connect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(error()), this, SLOT(showUpdateRequestError()));
    connect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(updateAvailable(QString,QString,QDateTime)), this, SLOT(showUpdateRequestUpdateAvailable(QString,QString,QDateTime)));

    /* checking for updates on github */
    DRS4UpdateNotifier::sharedInstance()->checkingForUpdates();

    /* online mode */
    if ( !DRS4BoardManager::sharedInstance()->connect() ) {
        m_drs4BoardConnected = false;
    }
    else {
        DRS4BoardManager::sharedInstance()->currentBoard()->Init();
    }

    /* offline mode */
    QLibrary lib("DLTPulseGenerator.dll");
    m_simulationLibIsLoaded = lib.load();

    connect(ui->pushButton_simulation, SIGNAL(clicked()), this, SLOT(startDemoMode()));
    connect(ui->pushButton_real, SIGNAL(clicked()), this, SLOT(startDataAquistion()));
    connect(ui->pushButton_quit, SIGNAL(clicked()), this, SLOT(quit()));
    connect(ui->pushButton_info, SIGNAL(clicked()), this, SLOT(showAbout()));

    /* no board found */
    if ( !m_drs4BoardConnected ) {
        ui->label_boardConnectionState->setStyleSheet("QLabel {color: red;}");

        ui->pushButton_real->setEnabled(false);

        if ( m_simulationLibIsLoaded ) {
            const QString versionStr = QString("v%1.%2").arg(DLTPULSEGENERATOR_MAJOR_VERSION).arg(DLTPULSEGENERATOR_MINOR_VERSION);

            ui->label_simulationLibraryVersion->setText("DLTPulseGenerator library " + versionStr + " (" + DLTPULSEGENERATOR_VERSION_RELEASE_DATE + ") loaded");
            ui->label_simulationLibraryVersion->setStyleSheet("QLabel {color: green;}");

            ui->label_boardConnectionState->setText("DRS4 board is not connected");
        }
        else {
            ui->label_simulationLibraryVersion->setText("missing DLTPulseGenerator library");
            ui->label_simulationLibraryVersion->setStyleSheet("QLabel {color: red;}");

            ui->pushButton_simulation->setEnabled(false);
        }

        ui->label_boardTimingCal->setText("");
        ui->label_boardVoltageCal->setText("");
    }
    /* board found */
    else {
        ui->pushButton_real->setEnabled(true);

        ui->label_boardConnectionState->setStyleSheet("QLabel {color: green;}");

        if ( m_simulationLibIsLoaded ) {
            const QString versionStr = QString("v%1.%2").arg(DLTPULSEGENERATOR_MAJOR_VERSION).arg(DLTPULSEGENERATOR_MINOR_VERSION);

            ui->label_simulationLibraryVersion->setText("DLTPulseGenerator library " + versionStr + " (" + DLTPULSEGENERATOR_VERSION_RELEASE_DATE + ") loaded");
            ui->label_simulationLibraryVersion->setStyleSheet("QLabel {color: green;}");

            ui->label_boardConnectionState->setText("connected to DRS4 board " + QVariant(DRS4BoardManager::sharedInstance()->currentBoard()->GetBoardSerialNumber()).toString());

            const bool validVoltCalib = DRS4BoardManager::sharedInstance()->currentBoard()->IsVoltageCalibrationValid();
            const bool validTimingCalib = DRS4BoardManager::sharedInstance()->currentBoard()->IsTimingCalibrationValid();

            if (validTimingCalib) {
                ui->label_boardTimingCal->setStyleSheet("QLabel {color: green;}");

                ui->label_boardTimingCal->setText(QString("[ok] timing calibrated for frequency %1 GHz").arg(DRS4BoardManager::sharedInstance()->currentBoard()->GetCalibratedFrequency()));
            }
            else {
                ui->label_boardTimingCal->setStyleSheet("QLabel {color: red;}");

                ui->label_boardTimingCal->setText(QString("[x] missing timing calibration"));
            }

            if (validVoltCalib) {
                ui->label_boardVoltageCal->setStyleSheet("QLabel {color: green;}");

                ui->label_boardVoltageCal->setText(QString("[ok] voltage calibrated"));
            }
            else {
                ui->label_boardVoltageCal->setStyleSheet("QLabel {color: red;}");

                ui->label_boardVoltageCal->setText(QString("[x] missing voltage calibration"));
            }
        }
        else {
            ui->label_simulationLibraryVersion->setText("missing DLTPulseGenerator library");
            ui->label_simulationLibraryVersion->setStyleSheet("QLabel {color: red;}");

            ui->pushButton_simulation->setEnabled(false);
            ui->pushButton_simulation->setStyleSheet("color: white;");

            ui->label_boardConnectionState->setText("connected to DRS4 board " + QVariant(DRS4BoardManager::sharedInstance()->currentBoard()->GetBoardSerialNumber()).toString());

            const bool validVoltCalib = DRS4BoardManager::sharedInstance()->currentBoard()->IsVoltageCalibrationValid();
            const bool validTimingCalib = DRS4BoardManager::sharedInstance()->currentBoard()->IsTimingCalibrationValid();

            if (validTimingCalib) {
                ui->label_boardTimingCal->setStyleSheet("QLabel {color: green;}");

                ui->label_boardTimingCal->setText(QString("[ok] timing calibrated for %1 GHz").arg(DRS4BoardManager::sharedInstance()->currentBoard()->GetCalibratedFrequency()));
            }
            else {
                ui->label_boardTimingCal->setStyleSheet("QLabel {color: red;}");

                ui->label_boardTimingCal->setText(QString("[x] missing timing calibration"));
            }

            if (validVoltCalib) {
                ui->label_boardVoltageCal->setStyleSheet("QLabel {color: green;}");

                ui->label_boardVoltageCal->setText(QString("[ok] voltage calibrated"));
            }
            else {
                ui->label_boardVoltageCal->setStyleSheet("QLabel {color: red;}");

                ui->label_boardVoltageCal->setText(QString("[x] missing voltage calibration"));
            }
        }
    }

    QMainWindow::showEvent(event);
}

void DRS4StartDlg::closeEvent(QCloseEvent *event) {
    m_isFinished = true;

    disconnect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(checkingUpdatesStarted()), this, SLOT(showUpdateRequestStartUp()));
    disconnect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(latestReleaseRunning()), this, SLOT(showUpdateRequestLatestReleaseIsRunning()));
    disconnect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(error()), this, SLOT(showUpdateRequestError()));
    disconnect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(updateAvailable(QString,QString,QDateTime)), this, SLOT(showUpdateRequestUpdateAvailable(QString,QString,QDateTime)));
    disconnect(ui->toolButton, SIGNAL(clicked()), DRS4UpdateNotifier::sharedInstance(), SLOT(redirectToUpdate()));

    QMainWindow::closeEvent(event);
}

bool DRS4StartDlg::isFinished() const {
    return m_isFinished;
}
