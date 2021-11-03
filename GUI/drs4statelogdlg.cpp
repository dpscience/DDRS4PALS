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

#include "drs4statelogdlg.h"
#include "ui_drs4statelogdlg.h"

DRS4StateLogDlg::DRS4StateLogDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DRS4StateLogDlg),
    m_lastSimulation(""),
    m_lastSettings("")
{
    ui->setupUi(this);

    ui->checkBox_pulseStreamArmed->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_pulseStreamArmed->setFocusPolicy(Qt::NoFocus);

    ui->checkBox_pulseStreamPulseGeneration->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_pulseStreamPulseGeneration->setFocusPolicy(Qt::NoFocus);

    ui->checkBox_scriptIsRunning->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_scriptIsRunning->setFocusPolicy(Qt::NoFocus);

    ui->checkBox_simulationLogNormal->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->checkBox_simulationLogNormal->setFocusPolicy(Qt::NoFocus);

    ui->progressBar_size->setMaximum(DRS4StreamDataLoader::sharedInstance()->fileSizeInMegabyte());
    ui->progressBar_size->setValue(DRS4StreamDataLoader::sharedInstance()->loadedFileSizeInMegabyte());

    m_updateTimer.setInterval(1500);
    m_updateTimer.setSingleShot(false);

    const QString text = PROGRAM_NAME + "<br><br>" + COPYRIGHT_NOTICE + "<br><br>";
    const QString contact = "contact: <a href=\"danny.petschke@uni-wuerzburg.de\">danny.petschke@uni-wuerzburg.de</a><br><br>";
    const QString publication1 = "<b>Related publications:</b><br><br><a href=\"https://www.sciencedirect.com/science/article/pii/S2352711019300676\">DDRS4PALS: A software for the acquisition and simulation of lifetime spectra using the DRS4 evaluation board</a><br><br>";
    const QString publication2 = "<a href=\"https://www.sciencedirect.com/science/article/pii/S2352340918315142?via%3Dihub\">Data on pure tin by Positron Annihilation Lifetime Spectroscopy (PALS) acquired with a semi-analog/digital setup using DDRS4PALS</a><br><br><br>";
    const QString publication3 = "<a href=\"https://www.sciencedirect.com/science/article/pii/S2352711018300530\">DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses</a><br><br><br>";
    const QString publication4 = "<a href=\"https://www.sciencedirect.com/science/article/pii/S2352711018300694\">Update (v1.1) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses</a><br><br><br>";
    const QString publication5 = "<a href=\"https://www.sciencedirect.com/science/article/pii/S2352711018301092\">Update (v1.2) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses</a><br><br><br>";
    const QString publication6 = "<a href=\"https://www.sciencedirect.com/science/article/pii/S235271101930038X\">Update (v1.3) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses</a><br><br><br>";
    const QString license = "<nobr>Fit Algorithm by: <br><a href=\"https://www.physics.wisc.edu/~craigm/idl/cmpfit.html\">MPFIT</a>: A MINPACK-1 Least Squares Fitting Library in C</nobr><br><br>";
    const QString license2 = "<nobr>CF Interpolation by:<br><a href=\"http://www.alglib.net/\">ALGLIB</a>: A Numerical Analysis and Data Processing Library</nobr><br>and<br><a href=\"http://kluge.in-chemnitz.de/opensource/spline/\">Tino Kluge</a>: Cubic Spline Interpolation in C++<br><br>";
    const QString license2_1 = "<nobr>Spike Removal by:<br><a href=\"http://www.sergejusz.com/engineering_tips/median_filter.htm/\">S. Zabinskis</a>: A Fast Median Filter in C++<br><br>";
    const QString license4 = "<nobr>Logo designed by: <br><a href=\"https://www.researchgate.net/profile/Hannah_Heil\">Hannah Heil</a></nobr><br><br><br>";
    const QString acknowlegment = "<b>Acknowledgment:</b><br><br><nobr>I thank Jakub Cizek (Charles University in Prague)<br><a href=\"https://www.researchgate.net/profile/Jakub_Cizek\">Jakub Cizek</a><br><br> and Martin Petriska (Slovak University of Technology in Bratislava)<br><a href=\"https://www.researchgate.net/profile/Martin_Petriska\">Martin Petriska</a><br><br> for fruitful discussions concerning pulse shape filtering and digital positron lifetime spectroscopy on the <a href=\"https://www.bgsu.edu/events/icpa-18.html\">ICPA-18</a>.</nobr><br><br>Furthermore, I thank <a href=\"https://www.researchgate.net/profile/Ricardo_Helm\">Ricardo Helm</a> for extensively testing this software.";
    QString versionStr; versionStr = QString("v%1.%2").arg(DLTPULSEGENERATOR_MAJOR_VERSION).arg(DLTPULSEGENERATOR_MINOR_VERSION);
    const QString license3 = "<nobr><a href=\"https://github.com/dpscience/DLTPulseGenerator\">DLTPulseGenerator</a> " + versionStr + " for Spectra-Simulation by: <br><a href=\"https://www.researchgate.net/profile/Danny_Petschke\">Danny Petschke</a></nobr><br><br>";
    QString t("<b>" + text + "</b>" + contact + license + license2 + license2_1 + license3 + license4 + publication1 + publication2 + publication3 + publication4 + publication5 + publication6 + acknowlegment);

    ui->plainTextEdit->appendHtml(t);

    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateState()));
}

DRS4StateLogDlg::~DRS4StateLogDlg()
{
    DDELETE_SAFETY(ui);
}

void DRS4StateLogDlg::init(DRS4Worker *worker)
{
    m_worker = worker;
}

void DRS4StateLogDlg::showEvent(QShowEvent *event)
{
    updateState();

    m_updateTimer.start();

    QWidget::showEvent(event);
}

void DRS4StateLogDlg::hideEvent(QHideEvent *event)
{
    m_updateTimer.stop();

    QWidget::hideEvent(event);
}

void DRS4StateLogDlg::updateState()
{
    ui->textEdit_simulationErrorCode->clear();

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

    ui->checkBox_scriptIsRunning->setChecked(scriptRunning);

    ui->label_pulseStreamArmedFile->setText(fileNameWriteStream);
    ui->label_pulseStreamArmedFile->setToolTip(fileNameWriteStream);
    ui->label_pulseStreamPulseGenerationFile->setText(fileNameReadStream);
    ui->label_pulseStreamPulseGenerationFile->setToolTip(fileNameReadStream);

    ui->label_fileNameSimulation->setText(fileNameSimulationFile);
    ui->label_fileNameSimulation->setToolTip(fileNameSimulationFile);

    ui->label_scriptFile->setText(fileNameScript);
    ui->label_scriptFile->setToolTip(fileNameScript);

    ui->label_settingsFile->setText(settingsFile);
    ui->label_settingsFile->setToolTip(settingsFile);

    ui->progressBar_size->setMaximum(DRS4StreamDataLoader::sharedInstance()->fileSizeInMegabyte());
    ui->progressBar_size->setValue(DRS4StreamDataLoader::sharedInstance()->loadedFileSizeInMegabyte());

    const QString settingsContent = DRS4SettingsManager::sharedInstance()->xmlContent();

    if ( settingsContent != m_lastSettings )
        m_lastSettings = settingsContent;

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
        const QString simulationContent = (fileNameSimulationFile != "")?DRS4SimulationSettingsManager::sharedInstance()->xmlContent():"";

        if (simulationContent != m_lastSimulation) {
            m_lastSimulation = simulationContent;
        }
    }
}
