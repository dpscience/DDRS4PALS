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

#include "drs4boardcalibrationdlg.h"
#include "ui_drs4boardcalibrationdlg.h"

DRS4BoardCalibrationDlg::DRS4BoardCalibrationDlg(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DRS4BoardCalibrationDlg),
    m_bIsRunning(false) {
    ui->setupUi(this);

    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::WindowCloseButtonHint);

    setWindowTitle(PROGRAM_NAME);

    m_statusLabelV = new QLabel;
    m_statusLabelT = new QLabel;
    m_infoLabel = new QLabel;
    m_statusBoardTemp = new QLabel;

    QFont font = m_statusLabelV->font();
    font.setBold(true);

    m_statusLabelT->setFont(font);
    m_statusLabelV->setFont(font);
    m_infoLabel->setFont(font);
    m_statusBoardTemp->setFont(font);

    ui->statusbar->addWidget(m_infoLabel);
    ui->statusbar->addWidget(m_statusLabelV);
    ui->statusbar->addWidget(m_statusLabelT);
    ui->statusbar->addWidget(m_statusBoardTemp);

    ui->comboBox_frequ->addItem("~5.12GHz (~200ns)");
    ui->comboBox_frequ->addItem("~2.00GHz (~500ns)");
    ui->comboBox_frequ->addItem("~1.00GHz (~1000ns)");
    ui->comboBox_frequ->addItem("~0.50GHz (~2000ns)");

    connect(ui->comboBox_frequ, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSampleFreq(int)));
    connect(ui->pushButton_freqCalib, SIGNAL(clicked()), this, SLOT(runTimingCalibration()));
    connect(ui->pushButton_voltCalib, SIGNAL(clicked()), this, SLOT(runVoltageCalibration()));

    ui->progressBar_freq->setValue(0);
    ui->progressBar_volt->setValue(0);

    if ( !DRS4BoardManager::sharedInstance()->isConnected() ) {
        enableControls(false);
        updateStatus(false);
    }
    else {
        DRS4BoardManager::sharedInstance()->currentBoard()->Init();

        emit ui->comboBox_frequ->currentIndexChanged(0);

        enableControls(true);

        updateStatus();

        connect(&m_statusReqTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));
        m_statusReqTimer.start(1500);
    }
}

DRS4BoardCalibrationDlg::~DRS4BoardCalibrationDlg() {
    DDELETE_SAFETY(m_infoLabel);
    DDELETE_SAFETY(m_statusLabelV);
    DDELETE_SAFETY(m_statusLabelT);
    DDELETE_SAFETY(m_statusBoardTemp);
    DDELETE_SAFETY(ui);
}

void DRS4BoardCalibrationDlg::runVoltageCalibration() {
    const QMessageBox::StandardButton reply = QMessageBox::question(this, "disconnect analog inputs ...", "Please disconnect the analog inputs before you proceed!", QMessageBox::Ok|QMessageBox::Cancel);

    if (reply != QMessageBox::Ok)
        return;

    const double temp = DRS4BoardManager::sharedInstance()->currentBoard()->GetTemperature();

    if (temp < 42.0) {
        const QMessageBox::StandardButton reply = QMessageBox::question(this, "temperature too low ...", "The current temperature is below 42.0 °C.\n\nWould you like to still proceed ?", QMessageBox::Yes|QMessageBox::No);

        if (reply != QMessageBox::Yes)
            return;
    }

    m_bIsRunning = true;

    connect(this, SIGNAL(progress(int)), ui->progressBar_volt, SLOT(setValue(int)));

    enableControls(false);
    ui->progressBar_volt->setEnabled(true);

    DRS4BoardManager::sharedInstance()->currentBoard()->CalibrateVolt(this);
}

void DRS4BoardCalibrationDlg::runTimingCalibration() {
    const double temp = DRS4BoardManager::sharedInstance()->currentBoard()->GetTemperature();

    if (temp < 42.0) {
        const QMessageBox::StandardButton reply = QMessageBox::question(this, "temperature too low ...", "The current temperature is below 42.0 °C.\n\nWould you like to still proceed ?", QMessageBox::Yes|QMessageBox::No);

        if (reply != QMessageBox::Yes)
            return;
    }

    m_bIsRunning = true;

    connect(this, SIGNAL(progress(int)), ui->progressBar_freq, SLOT(setValue(int)));

    enableControls(false);
    ui->progressBar_freq->setEnabled(true);

    DRS4BoardManager::sharedInstance()->currentBoard()->CalibrateTiming(this);
}

void DRS4BoardCalibrationDlg::changeSampleFreq(int id)
{
    switch(id) {
    case 0:
        DRS4BoardManager::sharedInstance()->currentBoard()->SetFrequency(5.12, true);
        break;

    case 1:
        DRS4BoardManager::sharedInstance()->currentBoard()->SetFrequency(2., true);
        break;

    case 2:
        DRS4BoardManager::sharedInstance()->currentBoard()->SetFrequency(1., true);
        break;

    case 3:
        DRS4BoardManager::sharedInstance()->currentBoard()->SetFrequency(0.5, true);
        break;
    }

    updateStatus();
}

void DRS4BoardCalibrationDlg::updateStatus(bool bConnected) {
    if (bConnected) {
        const QString serialNumber = QVariant(DRS4BoardManager::sharedInstance()->currentBoard()->GetBoardSerialNumber()).toString();
        const QString firmware = QVariant(DRS4BoardManager::sharedInstance()->currentBoard()->GetFirmwareVersion()).toString();

        QString infoText = QString("connected to DRS4 <%1> | firmware <%2>").arg(serialNumber).arg(firmware);

        m_infoLabel->setText(infoText);
        m_infoLabel->setStyleSheet("color: green");

        const QString sTimingCalib = DRS4BoardManager::sharedInstance()->currentBoard()->IsTimingCalibrationValid() ? "ok" : "missing";
        const QString sVoltCalib = DRS4BoardManager::sharedInstance()->currentBoard()->IsVoltageCalibrationValid() ? "ok" : "missing";

        const QString msg = QString("actual freq. = %1 GHz (calibrated freq. = %2 GHz)").arg(DRS4BoardManager::sharedInstance()->currentBoard()->GetTrueFrequency()).arg(DRS4BoardManager::sharedInstance()->currentBoard()->GetCalibratedFrequency());

        setWindowTitle(msg);

        m_statusLabelT->setText(QString("timing <%1>").arg(sTimingCalib));
        m_statusLabelV->setText(QString("voltage <%1>").arg(sVoltCalib));

        if (DRS4BoardManager::sharedInstance()->currentBoard()->IsTimingCalibrationValid())
            m_statusLabelT->setStyleSheet("color: green");
        else
            m_statusLabelT->setStyleSheet("color: red");

        if (DRS4BoardManager::sharedInstance()->currentBoard()->IsVoltageCalibrationValid())
            m_statusLabelV->setStyleSheet("color: green");
        else
            m_statusLabelV->setStyleSheet("color: red");

        emit statusChanged();

        const double temp = DRS4BoardManager::sharedInstance()->currentBoard()->GetTemperature();
        const QString sTemp = QString::number(temp,'f',1) + QString(" °C");

        m_statusBoardTemp->setText(sTemp);

        if (temp > 42.)
            m_statusBoardTemp->setStyleSheet("color: green");
        else if (temp < 38.)
            m_statusBoardTemp->setStyleSheet("color: red");
        else
            m_statusBoardTemp->setStyleSheet("color: rgb(204, 145, 27)");
    }
    else {
        QString infoText = QString("no connection");

        m_infoLabel->setText(infoText);
        m_infoLabel->setStyleSheet("color: red");
    }
}

void DRS4BoardCalibrationDlg::enableControls(bool on) {
    ui->groupBox_time->setDisabled(!on);
    ui->groupBox_volt->setDisabled(!on);
}

void DRS4BoardCalibrationDlg::Progress(int value) {
    emit progress(value);

    if (value >= 100) {
        disconnect(this, SIGNAL(progress(int)), ui->progressBar_volt, SLOT(setValue(int)));
        disconnect(this, SIGNAL(progress(int)), ui->progressBar_freq, SLOT(setValue(int)));

        enableControls(true);

        ui->progressBar_freq->setValue(0);
        ui->progressBar_volt->setValue(0);

        m_bIsRunning = false;
    }
}

void DRS4BoardCalibrationDlg::closeEvent(QCloseEvent *event) {
    if (m_bIsRunning) {
        event->ignore();

        return;
    }

    QMainWindow::closeEvent(event);
}
