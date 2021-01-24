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

#include "drs4pulsesavedlg.h"
#include "ui_drs4pulsesavedlg.h"

DRS4PulseSaveDlg::DRS4PulseSaveDlg(DRS4Worker *worker, QWidget *parent) :
    m_worker(worker),
    QWidget(parent),
    ui(new Ui::DRS4PulseSaveDlg)
{
    ui->setupUi(this);

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled()  )
        setWindowTitle(PROGRAM_NAME + " - SIMULATION-MODE");
    else
        setWindowTitle(PROGRAM_NAME);

    connect(ui->checkBox_PulsesA, SIGNAL(clicked(bool)), this, SLOT(updateState(bool)));
    connect(ui->checkBox_PulsesB, SIGNAL(clicked(bool)), this, SLOT(updateState(bool)));
    connect(ui->checkBox_PulsesInterpolationA, SIGNAL(clicked(bool)), this, SLOT(updateState(bool)));
    connect(ui->checkBox_PulsesInterpolationB, SIGNAL(clicked(bool)), this, SLOT(updateState(bool)));

    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(save()));

    connect(DRS4TextFileStreamManager::sharedInstance(), SIGNAL(started()), this, SLOT(setAsRunning()));
    connect(DRS4TextFileStreamManager::sharedInstance(), SIGNAL(finished()), this, SLOT(resetAsRunning()));

    ui->checkBox_PulsesA->setChecked(true);
    ui->checkBox_PulsesB->setChecked(true);
    ui->checkBox_PulsesInterpolationA->setChecked(true);
    ui->checkBox_PulsesInterpolationB->setChecked(true);

    ui->spinBox_renderPoints->setRange(2, 100);
    ui->spinBox_renderPoints->setValue(DRS4ProgramSettingsManager::sharedInstance()->splineIntraPoints());

    connect(ui->spinBox_renderPoints, SIGNAL(valueChanged(int)), this, SLOT(changeSplineIntraPoints(int)));
}

DRS4PulseSaveDlg::~DRS4PulseSaveDlg()
{
    DDELETE_SAFETY(ui);
}

void DRS4PulseSaveDlg::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
}

void DRS4PulseSaveDlg::changeSplineIntraPoints(int val)
{
    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4ProgramSettingsManager::sharedInstance()->setSplineIntraPoints(val);

    m_worker->setBusy(false);
}

void DRS4PulseSaveDlg::updateState(bool state)
{
    DUNUSED_PARAM(state);

    if ( !ui->checkBox_PulsesA->isChecked()
         && !ui->checkBox_PulsesB->isChecked()
         && !ui->checkBox_PulsesInterpolationA->isChecked()
         && !ui->checkBox_PulsesInterpolationB->isChecked())
        ui->pushButton_save->setEnabled(false);
    else
        ui->pushButton_save->setEnabled(true);
}

void DRS4PulseSaveDlg::save()
{
    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               DRS4ProgramSettingsManager::sharedInstance()->streamTextFileInputFilePath(),
                               tr("Data-File *.txt"));

    if ( fileName.isEmpty() ) {
        m_worker->setBusy(false);
        return;
    }

    DRS4ProgramSettingsManager::sharedInstance()->setStreamTextFileInputFilePath(fileName);

    DRS4TextFileStreamManager::sharedInstance()->start(fileName,
                                                       ui->spinBox_NPulses->value(),
                                                       ui->checkBox_PulsesA->isChecked(),
                                                       ui->checkBox_PulsesB->isChecked(),
                                                       ui->checkBox_PulsesInterpolationA->isChecked(),
                                                       ui->checkBox_PulsesInterpolationB->isChecked());
}

void DRS4PulseSaveDlg::setAsRunning()
{
    ui->pushButton_save->setText("abort");
    connect(ui->pushButton_save, SIGNAL(clicked()), DRS4TextFileStreamManager::sharedInstance(), SLOT(abort()));
    disconnect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(save()));
}

void DRS4PulseSaveDlg::resetAsRunning()
{
    ui->pushButton_save->setText("save...");
    disconnect(ui->pushButton_save, SIGNAL(clicked()), DRS4TextFileStreamManager::sharedInstance(), SLOT(abort()));
    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(save()));
}
