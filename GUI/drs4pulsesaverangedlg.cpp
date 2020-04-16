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
*****************************************************************************/

#include "drs4pulsesaverangedlg.h"
#include "ui_drs4pulsesaverangedlg.h"

DRS4PulseSaveRangeDlg::DRS4PulseSaveRangeDlg(DRS4Worker *worker, QWidget *parent) :
    QWidget(parent),
    m_worker(worker),
    ui(new Ui::DRS4PulseSaveRangeDlg)
{
    ui->setupUi(this);

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled()  )
        setWindowTitle(PROGRAM_NAME + " - SIMULATION-MODE");
    else
        setWindowTitle(PROGRAM_NAME);

    connect(ui->checkBox_AB, SIGNAL(clicked(bool)), this, SLOT(updateState(bool)));
    connect(ui->checkBox_BA, SIGNAL(clicked(bool)), this, SLOT(updateState(bool)));

    connect(ui->spinBox_AB_left, SIGNAL(valueChanged(int)), this, SLOT(updateState2(int)));
    connect(ui->spinBox_AB_right, SIGNAL(valueChanged(int)), this, SLOT(updateState2(int)));
    connect(ui->spinBox_BA_left, SIGNAL(valueChanged(int)), this, SLOT(updateState2(int)));
    connect(ui->spinBox_BA_right, SIGNAL(valueChanged(int)), this, SLOT(updateState2(int)));

    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(save()));

    connect(DRS4TextFileStreamRangeManager::sharedInstance(), SIGNAL(started()), this, SLOT(setAsRunning()));
    connect(DRS4TextFileStreamRangeManager::sharedInstance(), SIGNAL(finished()), this, SLOT(resetAsRunning()));

    ui->spinBox_AB_left->setMinimum(0);
    ui->spinBox_AB_left->setMaximum(DRS4SettingsManager::sharedInstance()->channelCntAB());

    ui->spinBox_AB_right->setMinimum(0);
    ui->spinBox_AB_right->setMaximum(DRS4SettingsManager::sharedInstance()->channelCntAB());

    ui->spinBox_BA_left->setMinimum(0);
    ui->spinBox_BA_left->setMaximum(DRS4SettingsManager::sharedInstance()->channelCntBA());

    ui->spinBox_BA_right->setMinimum(0);
    ui->spinBox_BA_right->setMaximum(DRS4SettingsManager::sharedInstance()->channelCntBA());

    if ( ui->spinBox_AB_left->value() > DRS4SettingsManager::sharedInstance()->channelCntAB() )
        ui->spinBox_AB_left->setValue(0);

    if ( ui->spinBox_AB_right->value() > DRS4SettingsManager::sharedInstance()->channelCntAB() )
        ui->spinBox_AB_right->setValue(DRS4SettingsManager::sharedInstance()->channelCntAB());

    if ( ui->spinBox_BA_left->value() > DRS4SettingsManager::sharedInstance()->channelCntBA() )
        ui->spinBox_BA_left->setValue(0);

    if ( ui->spinBox_BA_right->value() > DRS4SettingsManager::sharedInstance()->channelCntBA() )
        ui->spinBox_BA_right->setValue(DRS4SettingsManager::sharedInstance()->channelCntBA());

    ui->checkBox_AB->setChecked(true);
    ui->checkBox_BA->setChecked(true);

    updateState(true);
}

DRS4PulseSaveRangeDlg::~DRS4PulseSaveRangeDlg()
{
    DDELETE_SAFETY(ui);
}

void DRS4PulseSaveRangeDlg::updateState(bool state)
{
    DUNUSED_PARAM(state);

    if ( !ui->checkBox_AB->isChecked()
         && !ui->checkBox_BA->isChecked() )
        ui->pushButton_save->setEnabled(false);
    else
        ui->pushButton_save->setEnabled(true);

    ui->spinBox_AB_left->setMinimum(0);
    ui->spinBox_AB_left->setMaximum(DRS4SettingsManager::sharedInstance()->channelCntAB());

    ui->spinBox_AB_right->setMinimum(0);
    ui->spinBox_AB_right->setMaximum(DRS4SettingsManager::sharedInstance()->channelCntAB());

    ui->spinBox_BA_left->setMinimum(0);
    ui->spinBox_BA_left->setMaximum(DRS4SettingsManager::sharedInstance()->channelCntBA());

    ui->spinBox_BA_right->setMinimum(0);
    ui->spinBox_BA_right->setMaximum(DRS4SettingsManager::sharedInstance()->channelCntBA());

    bool bValid = true;

    if ( ui->checkBox_AB->isChecked() )
    {
        if ( ui->spinBox_AB_left->value() >= ui->spinBox_AB_right->value() )
            bValid = false;
    }

    if ( ui->checkBox_BA->isChecked() )
    {
        if ( ui->spinBox_BA_left->value() >= ui->spinBox_BA_right->value() )
            bValid = false;
    }

    ui->pushButton_save->setEnabled(bValid);

    DRS4TextFileStreamRangeManager::sharedInstance()->setLTRangeMinAB(ui->spinBox_AB_left->value());
    DRS4TextFileStreamRangeManager::sharedInstance()->setLTRangeMaxAB(ui->spinBox_AB_right->value());

    DRS4TextFileStreamRangeManager::sharedInstance()->setLTRangeMinBA(ui->spinBox_BA_left->value());
    DRS4TextFileStreamRangeManager::sharedInstance()->setLTRangeMaxBA(ui->spinBox_BA_right->value());
}

void DRS4PulseSaveRangeDlg::updateState2(int state)
{
    updateState(state);
}

void DRS4PulseSaveRangeDlg::save()
{
    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               DRS4ProgramSettingsManager::sharedInstance()->streamTextFileInputFilePath(),
                               tr("Data-File *.txt"));

    if ( fileName.isEmpty() )
    {
        m_worker->setBusy(false);
        return;
    }

    DRS4ProgramSettingsManager::sharedInstance()->setStreamTextFileInputFilePath(fileName);

    DRS4TextFileStreamRangeManager::sharedInstance()->start(fileName, ui->spinBox_NPulses->value(), ui->checkBox_AB->isChecked(), ui->checkBox_BA->isChecked());
}

void DRS4PulseSaveRangeDlg::setAsRunning()
{
    ui->pushButton_save->setText("abort");
    connect(ui->pushButton_save, SIGNAL(clicked()), DRS4TextFileStreamRangeManager::sharedInstance(), SLOT(abort()));
    disconnect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(save()));
}

void DRS4PulseSaveRangeDlg::resetAsRunning()
{
    ui->pushButton_save->setText("save...");
    disconnect(ui->pushButton_save, SIGNAL(clicked()), DRS4TextFileStreamRangeManager::sharedInstance(), SLOT(abort()));
    connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(save()));
}

void DRS4PulseSaveRangeDlg::showEvent(QShowEvent *event)
{
    ui->spinBox_AB_left->setMinimum(0);
    ui->spinBox_AB_left->setMaximum(DRS4SettingsManager::sharedInstance()->channelCntAB());

    ui->spinBox_AB_right->setMinimum(0);
    ui->spinBox_AB_right->setMaximum(DRS4SettingsManager::sharedInstance()->channelCntAB());

    ui->spinBox_BA_left->setMinimum(0);
    ui->spinBox_BA_left->setMaximum(DRS4SettingsManager::sharedInstance()->channelCntBA());

    ui->spinBox_BA_right->setMinimum(0);
    ui->spinBox_BA_right->setMaximum(DRS4SettingsManager::sharedInstance()->channelCntBA());

    if ( ui->spinBox_AB_left->value() > DRS4SettingsManager::sharedInstance()->channelCntAB() )
        ui->spinBox_AB_left->setValue(0);

    if ( ui->spinBox_AB_right->value() > DRS4SettingsManager::sharedInstance()->channelCntAB() )
        ui->spinBox_AB_right->setValue(DRS4SettingsManager::sharedInstance()->channelCntAB());

    if ( ui->spinBox_BA_left->value() > DRS4SettingsManager::sharedInstance()->channelCntBA() )
        ui->spinBox_BA_left->setValue(0);

    if ( ui->spinBox_BA_right->value() > DRS4SettingsManager::sharedInstance()->channelCntBA() )
        ui->spinBox_BA_right->setValue(DRS4SettingsManager::sharedInstance()->channelCntBA());

    updateState(true);

    QWidget::showEvent(event);
}
