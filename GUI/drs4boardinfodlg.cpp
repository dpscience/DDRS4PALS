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

#include "drs4boardinfodlg.h"
#include "ui_drs4boardinfodlg.h"

DRS4BoardInfoDlg::DRS4BoardInfoDlg(QWidget *parent, DRS4Worker *worker) :
    QWidget(parent),
    m_worker(worker),
    ui(new Ui::DRS4BoardInfoDlg)
{
    ui->setupUi(this);

    if (DRS4BoardManager::sharedInstance()->isDemoModeEnabled()  ) {
        setWindowTitle(PROGRAM_NAME + " - SIMULATION-MODE");
        ui->pushButton_save->setDisabled(true);
    }
    else {
        setWindowTitle(PROGRAM_NAME);
        connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(save()));
    }
}

DRS4BoardInfoDlg::~DRS4BoardInfoDlg()
{
    DDELETE_SAFETY(ui);
}

void DRS4BoardInfoDlg::updateInfo()
{
    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
    {
        ui->groupBox_mainBoard->setEnabled(false);
        ui->label_mode->setText("<b>Software is running in SIMULATION-MODE.</b>");

        return;
    }

    ui->label_mode->setText("<b><font color=\"Green\">Board is connected!</font></b>");

    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRSBoard* board = DRS4BoardManager::sharedInstance()->currentBoard();

    const int SN = board->GetBoardSerialNumber();
    const int BT = board->GetBoardType();
    const int DRSType = board->GetDRSType();
    const int firmware = board->GetFirmwareVersion();

    const double caliFreq = board->GetCalibratedFrequency();
    const double caliVolt = board->GetInputRange();

    const int numberOfChannels = board->GetNumberOfChannels();
    const int numberOfChips = board->GetNumberOfChips();
    const int numberOfReadOutChannel = board->GetNumberOfReadoutChannels();
    const int numberOfInputs = board->GetNumberOfInputs();

    const double trueFreq = board->GetTrueFrequency();

    const bool validVoltCalib = board->IsVoltageCalibrationValid();
    const bool validTimingCalib = board->IsTimingCalibrationValid();

    m_worker->setBusy(false);


    ui->groupBox_mainBoard->setTitle("Serial Number: " + QVariant(SN).toString());

    ui->label_boardType->setText("<b>" + QVariant(BT).toString() + "</b>");
    ui->label_drsType->setText("<b>" + QVariant(DRSType).toString() + "</b>");
    ui->label_firmwareVersion->setText("<b>" + QVariant(firmware).toString() + "</b>");

    ui->label_numberOfChannels->setText("<b>" + QVariant(numberOfChannels).toString() + "</b>");
    ui->label_numberOfChips->setText("<b>" + QVariant(numberOfChips).toString() + "</b>");
    ui->label_numberOfReadoutChannels->setText("<b>" + QVariant(numberOfReadOutChannel).toString() + "</b>");
    ui->label_numberOfInputs->setText("<b>" + QVariant(numberOfInputs).toString() + "</b>");

    ui->label_calibratedFrequency->setText("<b>" + QVariant(caliFreq).toString() + "</b>");
    ui->label_runningFrequency->setText("<b>" + QVariant(trueFreq).toString() + "</b>");
    ui->label_calibratedVoltage->setText("<b>" + QVariant(caliVolt).toString() + "</b>");

    ui->label_validVotageCalib->setText("<b>" + QString(validVoltCalib?"<font color=\"Green\">OK</font>":"<font color=\"Red\">NO</font>") + "</b>");
    ui->label_validTimingCalib->setText("<b>" + QString(validTimingCalib?"<font color=\"Green\">OK</font>":"<font color=\"Red\">NO</font>") + "</b>");
}

void DRS4BoardInfoDlg::save()
{
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save DRS4 Info File"),
                                                          "",
                                                          tr("DRS4 Info File (*.txt)"));

    if ( fileName.isEmpty() ) {
        return;
    }


    DRSBoard* board = DRS4BoardManager::sharedInstance()->currentBoard();

    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const int SN = board->GetBoardSerialNumber();
    const int BT = board->GetBoardType();
    const int DRSType = board->GetDRSType();
    const int firmware = board->GetFirmwareVersion();

    const double caliFreq = board->GetCalibratedFrequency();
    const double caliVolt = board->GetInputRange();

    const int numberOfChannels = board->GetNumberOfChannels();
    const int numberOfChips = board->GetNumberOfChips();
    const int numberOfReadOutChannel = board->GetNumberOfReadoutChannels();
    const int numberOfInputs = board->GetNumberOfInputs();

    const double trueFreq = board->GetTrueFrequency();

    const bool validVoltCalib = board->IsVoltageCalibrationValid();
    const bool validTimingCalib = board->IsTimingCalibrationValid();

    m_worker->setBusy(false);


    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        stream << "# " << "DRS4-Info-File\n";
        stream << "# " << QDateTime::currentDateTime().toString() << "\n\n";

        stream << QString("Serial Number: " + QVariant(SN).toString());

        stream << QString("Board Type: " + QVariant(BT).toString() + "\n");
        stream << QString("DRS Type: " + QVariant(DRSType).toString() +"\n");
        stream << QString("Firmware Version: " + QVariant(firmware).toString() + "\n");;

        stream << QString("Number of Channels: " + QVariant(numberOfChannels).toString() + "\n");
        stream << QString("Number of Chips: " + QVariant(numberOfChips).toString() + "\n");
        stream << QString("Number of Readout Channels: " + QVariant(numberOfReadOutChannel).toString() + "\n");
        stream << QString("Number of Inputs: " + QVariant(numberOfInputs).toString() + "\n");

        stream << QString("Calibrated Frequency [GHz]: " + QVariant(caliFreq).toString() + "\n");
        stream << QString("Running Frequency [GHz]: " + QVariant(trueFreq).toString() + "\n");
        stream << QString("Input Voltage Range [V]: " + QVariant(caliVolt).toString() + "\n");

        stream << QString("Voltage Calib. valid?: " + QString(validVoltCalib?"OK":"NO") + "\n");
        stream << QString("Timing Calib. valid?: " + QString(validTimingCalib?"OK":"NO") + "\n");

        file.close();
    }
    else {
        MSGBOX("Error while writing DRS4 Info File!");
    }
}

void DRS4BoardInfoDlg::showEvent(QShowEvent *event)
{
    updateInfo();
    QWidget::showEvent(event);
}
