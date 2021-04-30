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

#include "drs4httpserverconfigdlg.h"
#include "ui_drs4httpserverconfigdlg.h"

DRS4HttpServerConfigDlg::DRS4HttpServerConfigDlg(DRS4Worker *worker, QWidget *parent) :
    QMainWindow(parent),
    m_worker(worker),
    ui(new Ui::DRS4HttpServerConfigDlg) {
    ui->setupUi(this);

    setWindowTitle(PROGRAM_NAME);

    connect(ui->checkBox_autoLaunch, SIGNAL(clicked(bool)), this, SLOT(changeAutostart(bool)));
    connect(ui->spinBox_port, SIGNAL(valueChanged(int)), this, SLOT(changePort(int)));
    connect(ui->spinBox_updateRate, SIGNAL(valueChanged(int)), this, SLOT(changeUpdateRate(int)));
    connect(ui->pushButton_start, SIGNAL(clicked()), this, SLOT(startStopServer()));
}

DRS4HttpServerConfigDlg::~DRS4HttpServerConfigDlg() {
    DDELETE_SAFETY(ui);
}

void DRS4HttpServerConfigDlg::showEvent(QShowEvent *event) {
    ui->checkBox_autoLaunch->setChecked(DRS4ProgramSettingsManager::sharedInstance()->httpServerAutostart());
    ui->spinBox_port->setValue(DRS4ProgramSettingsManager::sharedInstance()->httpServerPort());
    ui->spinBox_updateRate->setValue(DRS4ProgramSettingsManager::sharedInstance()->httpServerUrlUpdateRate());

    updateServer();

    event->accept();

    QMainWindow::showEvent(event);
}

void DRS4HttpServerConfigDlg::updateServer() {
    if (DRS4WebServer::sharedInstance()->isListening()) {
        ui->pushButton_start->setText("stop listening ...");

        ui->spinBox_port->setEnabled(false);
        ui->spinBox_updateRate->setEnabled(false);

        const QString info = QString(QString("your browser url should look like ... <b>http://your-ip-adress:%1/</b> ...").arg(DRS4ProgramSettingsManager::sharedInstance()->httpServerPort()));
        ui->label_urlPreview->setStyleSheet("color: green");
        ui->label_urlPreview->setText(info);
    }
    else {
        ui->pushButton_start->setText("start listening ...");

        ui->spinBox_port->setEnabled(true);
        ui->spinBox_updateRate->setEnabled(true);

        const QString info = "- server is disabled -";
        ui->label_urlPreview->setStyleSheet("color: red");
        ui->label_urlPreview->setText(info);
    }
}

void DRS4HttpServerConfigDlg::changeAutostart(bool on) {
    DRS4ProgramSettingsManager::sharedInstance()->setHttpServerAutostart(on);
}

void DRS4HttpServerConfigDlg::changePort(int port) {
    DRS4ProgramSettingsManager::sharedInstance()->setHttpServerPort(port);

    updateServer();
}

void DRS4HttpServerConfigDlg::changeUpdateRate(int rate) {
    DRS4ProgramSettingsManager::sharedInstance()->setHttpServerUrlUpdateRate(rate);

    updateServer();
}

void DRS4HttpServerConfigDlg::startStopServer() {
    if (!m_worker)
        return;

    if (DRS4WebServer::sharedInstance()->isListening()) {
        DRS4WebServer::sharedInstance()->stop();
    }
    else {
        DRS4WebServer::sharedInstance()->start(m_worker);
    }

    updateServer();
}
