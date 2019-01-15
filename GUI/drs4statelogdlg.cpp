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

#include "drs4statelogdlg.h"
#include "ui_drs4statelogdlg.h"

DRS4StateLogDlg::DRS4StateLogDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DRS4StateLogDlg),
    m_lastSimulation(""),
    m_lastSettings("")
{
    ui->setupUi(this);

    //coming soon (Masterthesis of Michael Fischer)!
    ui->checkBox_simulationElectronics->setEnabled(false);

    ui->checkBox_pulseStreamArmed->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_pulseStreamArmed->setFocusPolicy(Qt::NoFocus);

    ui->checkBox_pulseStreamPulseGeneration->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_pulseStreamPulseGeneration->setFocusPolicy(Qt::NoFocus);

    ui->checkBox_scriptIsRunning->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_scriptIsRunning->setFocusPolicy(Qt::NoFocus);

    ui->checkBox_simulationElectronics->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_simulationElectronics->setFocusPolicy(Qt::NoFocus);

    ui->checkBox_simulationLogNormal->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_simulationLogNormal->setFocusPolicy(Qt::NoFocus);

    ui->progressBar_size->setMaximum(DRS4StreamDataLoader::sharedInstance()->fileSizeInMegabyte());
    ui->progressBar_size->setValue(DRS4StreamDataLoader::sharedInstance()->loadedFileSizeInMegabyte());

    ui->treeWidget_settingsFile->setColumnCount(2);
    ui->treeWidget_settingsFile->headerItem()->setText(0, "xml-node");
    ui->treeWidget_settingsFile->headerItem()->setText(1, "value");
    ui->treeWidget_settingsFile->addTopLevelItem(new DXMLTreeWidgetItem(DRS4SettingsManager::sharedInstance()->parentNode()));
    ui->treeWidget_settingsFile->expandAll();
    ui->treeWidget_settingsFile->headerItem()->setFirstColumnSpanned(true);
    ui->treeWidget_settingsFile->headerItem()->setExpanded(true);
    QFont col1 = ui->treeWidget_settingsFile->headerItem()->font(0);
    QFont col2 = ui->treeWidget_settingsFile->headerItem()->font(1);
    col1.setBold(true);
    col2.setBold(true);
    ui->treeWidget_settingsFile->headerItem()->setFont(0, QFont(col1));
    ui->treeWidget_settingsFile->headerItem()->setFont(1, QFont(col2));

    if (ui->treeWidget_settingsFile->topLevelItem(0) )
        ui->treeWidget_settingsFile->topLevelItem(0)->setText(0, DRS4SettingsManager::sharedInstance()->fileName());

    ui->treeWidget_simulationInputFile->setColumnCount(2);
    ui->treeWidget_simulationInputFile->headerItem()->setText(0, "xml-node");
    ui->treeWidget_simulationInputFile->headerItem()->setText(1, "value");
    ui->treeWidget_simulationInputFile->addTopLevelItem(new DXMLTreeWidgetItem(DRS4SimulationSettingsManager::sharedInstance()->parentNode()));
    ui->treeWidget_simulationInputFile->expandAll();
    ui->treeWidget_simulationInputFile->headerItem()->setFirstColumnSpanned(true);
    ui->treeWidget_simulationInputFile->headerItem()->setExpanded(true);
    col1 = ui->treeWidget_simulationInputFile->headerItem()->font(0);
    col2 = ui->treeWidget_simulationInputFile->headerItem()->font(1);
    col1.setBold(true);
    col2.setBold(true);
    ui->treeWidget_settingsFile->headerItem()->setFont(0, QFont(col1));
    ui->treeWidget_settingsFile->headerItem()->setFont(1, QFont(col2));

    if (ui->treeWidget_simulationInputFile->topLevelItem(0) )
        ui->treeWidget_simulationInputFile->topLevelItem(0)->setText(0, DRS4SimulationSettingsManager::sharedInstance()->fileName());
}

DRS4StateLogDlg::~DRS4StateLogDlg()
{
    DDELETE_SAFETY(ui);
}

void DRS4StateLogDlg::init(DRS4Worker *worker)
{
    m_worker = worker;

    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateState()));

    m_updateTimer.setInterval(1200);
    m_updateTimer.setSingleShot(false);

    m_updateTimer.start();
}

void DRS4StateLogDlg::updateState()
{
    /*if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}*/

    ui->textEdit_simulationErrorCode->clear();

    //Decision-Matrix:
    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
    {
        ui->groupBox->setEnabled(false);
        ui->treeWidget_simulationInputFile->setEnabled(false);
        ui->label_fileNameSimulation2->setEnabled(false);
    }

    const bool pulseStreamWriteArmed = DRS4StreamManager::sharedInstance()->isArmed();
    const bool pulseStreamReadArmed = DRS4StreamDataLoader::sharedInstance()->isArmed();

    const bool pulseSimulationLogNormal = DRS4PulseGenerator::sharedInstance()->type() == LogNormalPulse && !(pulseStreamReadArmed || DRS4BoardManager::sharedInstance()->usingStreamDataOnDemoMode());
    const bool pulseSimulationElectronics = DRS4PulseGenerator::sharedInstance()->type() == ElectronicComponents && !(pulseStreamReadArmed || DRS4BoardManager::sharedInstance()->usingStreamDataOnDemoMode());

    const bool scriptRunning = DRS4ScriptManager::sharedInstance()->isArmed();


    const QString fileNameWriteStream = !pulseStreamWriteArmed?"...":DRS4StreamManager::sharedInstance()->fileName();
    const QString fileNameReadStream = !pulseStreamReadArmed?"...":DRS4StreamDataLoader::sharedInstance()->fileName();

    const QString fileNameSimulationFile = (!pulseSimulationLogNormal && !pulseSimulationElectronics) || DRS4BoardManager::sharedInstance()->usingStreamDataOnDemoMode() || pulseStreamReadArmed?"":DRS4SimulationSettingsManager::sharedInstance()->fileName();

    if (!fileNameSimulationFile.isEmpty()) {
        ui->textEdit_simulationErrorCode->setHtml(DRS4PulseGenerator::sharedInstance()->getErrorString());
    }

    const QString fileNameScript = !scriptRunning?"...":DRS4ScriptManager::sharedInstance()->fileName();

    const QString settingsFile = DRS4SettingsManager::sharedInstance()->fileName();

    ui->checkBox_pulseStreamArmed->setChecked(pulseStreamWriteArmed);
    ui->checkBox_pulseStreamPulseGeneration->setChecked(pulseStreamReadArmed);

    if ( pulseStreamWriteArmed ) {
        const double MB = (((double)DRS4StreamManager::sharedInstance()->streamedContentInBytes())/1024.0f)/1000.0f;
        ui->label_fileStreamByte->setText(QString::number(MB, 'f', 2) + " [MB]");
    }
    else {
        ui->label_fileStreamByte->setText("");
    }

    ui->checkBox_simulationLogNormal->setChecked(pulseSimulationLogNormal);
    ui->checkBox_simulationElectronics->setChecked(pulseSimulationElectronics);

    ui->checkBox_scriptIsRunning->setChecked(scriptRunning);

    ui->label_pulseStreamArmedFile->setText(fileNameWriteStream);
    ui->label_pulseStreamArmedFile->setToolTip(fileNameWriteStream);
    ui->label_pulseStreamPulseGenerationFile->setText(fileNameReadStream);
    ui->label_pulseStreamPulseGenerationFile->setToolTip(fileNameReadStream);

    ui->label_fileNameSimulation->setText(fileNameSimulationFile);
    ui->label_fileNameSimulation->setToolTip(fileNameSimulationFile);
    ui->label_fileNameSimulation2->setText(fileNameSimulationFile);
    ui->label_fileNameSimulation2->setToolTip(fileNameSimulationFile);

    ui->label_scriptFile->setText(fileNameScript);
    ui->label_scriptFile->setToolTip(fileNameScript);

    ui->label_settingsFile->setText(settingsFile);
    ui->label_settingsFile->setToolTip(settingsFile);
    ui->label_settingsFile2->setText(settingsFile);
    ui->label_settingsFile2->setToolTip(settingsFile);

    ui->progressBar_size->setMaximum(DRS4StreamDataLoader::sharedInstance()->fileSizeInMegabyte());
    ui->progressBar_size->setValue(DRS4StreamDataLoader::sharedInstance()->loadedFileSizeInMegabyte());

    const QString settingsContent = DRS4SettingsManager::sharedInstance()->xmlContent();

    if ( settingsContent != m_lastSettings ) {
        ui->treeWidget_settingsFile->clear();

        ui->treeWidget_settingsFile->setColumnCount(2);
        ui->treeWidget_settingsFile->headerItem()->setText(0, "xml-node");
        ui->treeWidget_settingsFile->headerItem()->setText(1, "value");
        ui->treeWidget_settingsFile->addTopLevelItem(new DXMLTreeWidgetItem(DRS4SettingsManager::sharedInstance()->parentNode()));

        ui->treeWidget_settingsFile->expandAll();
        ui->treeWidget_settingsFile->headerItem()->setFirstColumnSpanned(true);
        ui->treeWidget_settingsFile->headerItem()->setExpanded(true);
        QFont col1 = ui->treeWidget_settingsFile->headerItem()->font(0);
        QFont col2 = ui->treeWidget_settingsFile->headerItem()->font(1);
        col1.setBold(true);
        col2.setBold(true);
        ui->treeWidget_settingsFile->headerItem()->setFont(0, QFont(col1));
        ui->treeWidget_settingsFile->headerItem()->setFont(1, QFont(col2));

        if (ui->treeWidget_settingsFile->topLevelItem(0) )
            ui->treeWidget_settingsFile->topLevelItem(0)->setText(0, DRS4SettingsManager::sharedInstance()->fileName());

        m_lastSettings = settingsContent;
    }

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
        const QString simulationContent = (fileNameSimulationFile != "")?DRS4SimulationSettingsManager::sharedInstance()->xmlContent():"";

        if (simulationContent != m_lastSimulation) {
            ui->treeWidget_simulationInputFile->clear();

            ui->treeWidget_simulationInputFile->setColumnCount(2);
            ui->treeWidget_simulationInputFile->headerItem()->setText(0, "xml-node");
            ui->treeWidget_simulationInputFile->headerItem()->setText(1, "value");
            ui->treeWidget_simulationInputFile->addTopLevelItem(new DXMLTreeWidgetItem(DRS4SimulationSettingsManager::sharedInstance()->parentNode()));

            ui->treeWidget_simulationInputFile->expandAll();
            ui->treeWidget_simulationInputFile->headerItem()->setFirstColumnSpanned(true);
            ui->treeWidget_simulationInputFile->headerItem()->setExpanded(true);
            QFont col1 = ui->treeWidget_simulationInputFile->headerItem()->font(0);
            QFont col2 = ui->treeWidget_simulationInputFile->headerItem()->font(1);
            col1.setBold(true);
            col2.setBold(true);
            ui->treeWidget_simulationInputFile->headerItem()->setFont(0, QFont(col1));
            ui->treeWidget_simulationInputFile->headerItem()->setFont(1, QFont(col2));

            if (ui->treeWidget_simulationInputFile->topLevelItem(0) )
                ui->treeWidget_simulationInputFile->topLevelItem(0)->setText(0, DRS4SimulationSettingsManager::sharedInstance()->fileName());

            m_lastSimulation = simulationContent;
        }
    }

    //m_worker->setBusy(false);
}
