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

#include "drs4scopedlg.h"
#include "ui_drs4scopedlg.h"

#include <QGraphicsEffect>
#include <QDesktopWidget>
#include <QSplashScreen>

class DRS4ConnectionLostEffect : public QGraphicsEffect {
public:
    void draw( QPainter* painter ) override {
        QPixmap pixmap;
        QPoint offset;

        if (sourceIsPixmap())
            pixmap = sourcePixmap(Qt::LogicalCoordinates, &offset);
        else {
            pixmap = sourcePixmap(Qt::DeviceCoordinates, &offset);
            painter->setWorldTransform(QTransform());
        }

        painter->setBrush(QColor(0, 0, 0, 255));
        painter->drawRect(pixmap.rect());
        painter->setOpacity(0.5);
        painter->drawPixmap(offset, pixmap);
    }
};

class DRS4ConnectionLostSplashScreen : public QSplashScreen {
protected:
    virtual void mousePressEvent(QMouseEvent *event) override {
        event->ignore();

        QSplashScreen::mousePressEvent(event);
    }
};

DRS4ScopeDlg::DRS4ScopeDlg(const ProgramStartType &startType, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DRS4ScopeDlg),
    m_worker(DNULLPTR),
    m_dataExchange(DNULLPTR),
    m_workerThread(DNULLPTR),
    m_currentSettingsFileLabel(DNULLPTR),
    m_pulseRequestTimer(DNULLPTR),
    m_phsRequestTimer(DNULLPTR),
    m_areaRequestTimer(DNULLPTR),
    m_riseTimeRequestTimer(DNULLPTR),
    m_pulseShapeFilterTimerA(DNULLPTR),
    m_pulseShapeFilterTimerB(DNULLPTR),
    m_lifetimeRequestTimer(DNULLPTR),
    m_temperatureTimer(DNULLPTR),
    m_autoSaveTimer(DNULLPTR),
    m_addInfoDlg(DNULLPTR),
    m_gplDialog(DNULLPTR),
    m_serverDlg(DNULLPTR),
    m_rcServerDlg(DNULLPTR),
    m_lgplDialog(DNULLPTR),
    m_usedgplDialog(DNULLPTR),
    m_boardInfoDlg(DNULLPTR),
    m_scriptDlg(DNULLPTR),
    m_pulseSaveDlg(DNULLPTR),
    m_pulseSaveRangeDlg(DNULLPTR),
    m_burstModeTimer(DNULLPTR),
    m_autoSaveSpectraTimer(DNULLPTR),
    m_connectionTimer(DNULLPTR),
    m_bSwapDirection(true),
    m_bConnectionLost(false) {
    ui->setupUi(this);

    /* Check if DQuickLTFit Software is available? */
    QFile file("DQuickLTFit.exe");
    const bool bDQuickExists = file.exists();

    if ( bDQuickExists ) {
        connect(ui->pushButton_openABDQuick, SIGNAL(clicked()), this, SLOT(openDQuickLTFitABSpectrum()));
        connect(ui->pushButton_openBADQuick, SIGNAL(clicked()), this, SLOT(openDQuickLTFitBASpectrum()));
        connect(ui->pushButton_openPromptDQuick, SIGNAL(clicked()), this, SLOT(openDQuickLTFitCoincidenceSpectrum()));
        connect(ui->pushButton_openMergedDQuickLT, SIGNAL(clicked()), this, SLOT(openDQuickLTFitMergedSpectrum()));
    }
    else {
        ui->pushButton_openABDQuick->setEnabled(false);
        ui->pushButton_openBADQuick->setEnabled(false);
        ui->pushButton_openPromptDQuick->setEnabled(false);
        ui->pushButton_openMergedDQuickLT->setEnabled(false);
    }

    arrangeIcons();

    m_lastTemperatureInDegree = 0.0f;

    m_lastFWHMABInPs = 0.0f;
    m_lastFWHMBAInPs = 0.0f;
    m_lastFWHMMergedInPs = 0.0f;
    m_lastFWHMCoincidenceInPs = 0.0f;

    m_lastt0ABInPs = 0.0f;
    m_lastt0BAInPs = 0.0f;
    m_lastt0MergedInPs = 0.0f;
    m_lastt0CoincidenceInPs = 0.0f;

    m_lastFitResultAB = -1;
    m_lastFitResultBA = -1;
    m_lastFitResultCoincidence = -1;
    m_lastFitResultMerged = -1;

    m_currentSettingsFileLabel = new QLabel;
    m_currentSettingsFileLabel->setFont(QFont("Arial", 10));
    ui->statusbar->addPermanentWidget(m_currentSettingsFileLabel);

    m_areaFilterASlopeUpper = 0;
    m_areaFilterAInterceptUpper = 250;

    m_areaFilterASlopeLower = 0;
    m_areaFilterAInterceptLower = 250;

    m_areaFilterBSlopeUpper = 0;
    m_areaFilterBInterceptUpper = 250;

    m_areaFilterBSlopeLower = 0;
    m_areaFilterBInterceptLower = 250;

    setWindowTitle(PROGRAM_NAME);

    //scripting access:
    DRS4ScriptingEngineAccessManager::sharedInstance()->setDialogAccess(this);

    if ( startType == ProgramStartType::Simulation ) {
        DRS4BoardManager::sharedInstance()->setDemoMode(true);
        DRS4BoardManager::sharedInstance()->setDemoFromStreamData(false);

        DRS4SimulationSettingsManager::sharedInstance()->save("__generalSimulationSettings" + EXT_SIMULATION_INPUT_FILE);

        ui->actionLoad_Simulation_Input->setEnabled(true);
        connect(ui->actionLoad_Simulation_Input, SIGNAL(triggered()), this, SLOT(loadSimulationToolSettings()));

        ui->actionLoad_Streaming_Data_File->setEnabled(true);
        connect(ui->actionLoad_Streaming_Data_File, SIGNAL(triggered()), this, SLOT(loadStreamingData()));

        connect(DRS4StreamDataLoader::sharedInstance(), SIGNAL(started()), this, SLOT(runningFromDataStreamStarted()));
        connect(DRS4StreamDataLoader::sharedInstance(), SIGNAL(finished()), this, SLOT(runningFromDataStreamFinished()));

        setWindowTitle(PROGRAM_NAME + " - SIMULATION-MODE");

        ui->comboBox_triggerLogic->setEnabled(false);
    }
    else {
        DRS4BoardManager::sharedInstance()->setDemoMode(false);
        DRS4BoardManager::sharedInstance()->setDemoFromStreamData(false);

        ui->actionLoad_Simulation_Input->setEnabled(false);
        ui->actionLoad_Streaming_Data_File->setEnabled(false);
    }

    m_addInfoDlg = new DRS4AddInfoDlg;
    m_addInfoDlg->hide();

    m_scriptDlg = new DRS4ScriptDlg(this);
    m_scriptDlg->hide();

    m_calculatorDlg = new DRS4CalculatorDlg;
    m_calculatorDlg->hide();

    m_gplDialog = new DRS4LicenseTextBox;
    m_gplDialog->addLicense(":/license/GPL", "License - GPLv3");
    m_gplDialog->hide();

    m_lgplDialog = new DRS4LicenseTextBox;
    m_lgplDialog->addLicense(":/license/LGPL", "License - LGPLv3");
    m_lgplDialog->hide();

    m_usedgplDialog = new DRS4LicenseTextBox;
    m_usedgplDialog->addLicense(":/license/used-license", "Used License - GPLv3");
    m_usedgplDialog->hide();

    /* Pulse -Scope */
    m_pulseRequestTimer = new QTimer;
    m_pulseRequestTimer->setInterval(120);

    connect(m_pulseRequestTimer, SIGNAL(timeout()), this, SLOT(plotPulseScope()));

    /* PHS-Plot */
    m_phsRequestTimer = new QTimer;
    m_phsRequestTimer->setInterval(200);

    connect(m_phsRequestTimer, SIGNAL(timeout()), this, SLOT(plotPHS()));

    /* Area-Filter */
    m_areaRequestTimer = new QTimer;
    m_areaRequestTimer->setInterval(200);

    connect(m_areaRequestTimer, SIGNAL(timeout()), this, SLOT(plotPulseAreaFilterData()));

    /* Rise-Time Filter */
    m_riseTimeRequestTimer = new QTimer;
    m_riseTimeRequestTimer->setInterval(200);

    connect(m_riseTimeRequestTimer, SIGNAL(timeout()), this, SLOT(plotRiseTimeFilterData()));

    /* Lifetime-Spectra */
    m_lifetimeRequestTimer = new QTimer;
    m_lifetimeRequestTimer->setInterval(200);

    connect(m_lifetimeRequestTimer, SIGNAL(timeout()), this, SLOT(plotLifetimeSpectra()));

    /* Persistance - Data */
    m_persistanceRequestTimer = new QTimer;
    m_persistanceRequestTimer->setInterval(200);

    connect(m_persistanceRequestTimer, SIGNAL(timeout()), this, SLOT(plotPersistance()));

    /* Burst - Mode - Timer */
    m_burstModeTimer = new QTimer;
    m_burstModeTimer->setInterval(500);

    connect(m_burstModeTimer, SIGNAL(timeout()), this, SLOT(updateInBurstMode()));

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
        ui->tabWidget->removeTab(6); // initially T-Plot

        ui->label_temperatureState->setStyleSheet("QLabel {color: white}");
        ui->label_temperatureState->setText("");

        m_temperatureTimer = new QTimer;
        m_temperatureTimer->setInterval(5000);
        m_temperatureTimer->setSingleShot(false);
    }
    else {
        ui->menuDRS4_Board_Information->menuAction()->setVisible(false);
        ui->tabWidget->removeTab(6); // initially T-Plot

        ui->progressBar_boardTemperature->setEnabled(false);
        ui->label_149->setEnabled(false);
        ui->label_temperatureState->setEnabled(false);
        ui->label_temperatureState->setVisible(false);
        ui->label_149->setVisible(false);
        ui->progressBar_boardTemperature->setVisible(false);
    }

    m_autoSaveTimer = new QTimer();
    m_autoSaveTimer->setInterval(30000);
    m_autoSaveTimer->setSingleShot(false);

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
        DRS4BoardManager::sharedInstance()->currentBoard()->Init();
        DRS4BoardManager::sharedInstance()->currentBoard()->SetFrequency(DRS4SettingsManager::sharedInstance()->sampleSpeedInGHz(), true);
    }

    initScopePlots();
    initPulseHeightSpectraPlots();
    initABLTSpectrum();
    initBALTSpectrum();
    initCoincidenceLTSpectrum();
    initMergedLTSpectrum();
    initBoardTPlot();
    initPulseAreaFilterA();
    initPulseAreaFilterB();
    initPulseRiseTimeFilterA();
    initPulseRiseTimeFilterB();
    initPersistancePlots();
    initPulseShapeFilterPlots();

    ui->verticalSlider_triggerLevelA->setRange(-500, 500);
    ui->verticalSlider_triggerLevelA->setValue(-15);
    ui->lineEdit_triggerA->setText("-15");

    ui->verticalSlider_triggerLevelB->setRange(-500, 500);
    ui->verticalSlider_triggerLevelB->setValue(-15);
    ui->lineEdit_triggerB->setText("-15");

    ui->action_StartDAQStreaming->setEnabled(true);
    ui->actionStop_Running->setEnabled(false);

    connect(ui->action_StartDAQStreaming, SIGNAL(triggered()), this, SLOT(startStreaming()));
    connect(ui->actionStop_Running, SIGNAL(triggered()), this, SLOT(stopStreaming()));

    ui->actionStart_True_False_Pulse_Streaming->setEnabled(true);
    ui->actionStop_True_False_Pulse_Streaming->setEnabled(false);

    connect(ui->actionStart_True_False_Pulse_Streaming, SIGNAL(triggered()), this, SLOT(startTrueFalsePulseStreaming()));
    connect(ui->actionStop_True_False_Pulse_Streaming, SIGNAL(triggered()), this, SLOT(stopTrueFalsePulseStreaming()));

    connect(ui->actionInfo, SIGNAL(triggered()), this, SLOT(showAboutBox()));

    connect(this, SIGNAL(signalUpdateCurrentFileLabelFromScript(QString)), this, SLOT(updateCurrentFileLabelFromScript(QString)));
    connect(this, SIGNAL(signalUpdateInfoDlgFromScript(QString)), this, SLOT(updateInfoDlgFromScript(QString)));
    connect(this, SIGNAL(signalUpdateThreadRunning(QString,QString)), this, SLOT(updateThreadRunning(QString,QString)));
    connect(this, SIGNAL(signalAddSampleSpeedWarningMessage(bool,bool)), this, SLOT(addSampleSpeedWarningMessage(bool,bool)));
    connect(this, SIGNAL(signalChangeSampleSpeed(int,bool)), this, SLOT(changeSampleSpeed(int,bool)));

    m_dataExchange = new DRS4WorkerDataExchange(&m_areaFilterASlopeUpper,
                                           &m_areaFilterAInterceptUpper,
                                           &m_areaFilterASlopeLower,
                                           &m_areaFilterAInterceptLower,
                                           &m_areaFilterBSlopeUpper,
                                           &m_areaFilterBInterceptUpper,
                                           &m_areaFilterBSlopeLower,
                                           &m_areaFilterBInterceptLower);

    m_worker = new DRS4Worker(m_dataExchange);
    m_workerThread = new QThread;

    connect(DRS4WebServer::sharedInstance(), SIGNAL(stateChanged(bool)), this, SLOT(changeServerState(bool)));
    connect(DRS4RemoteControlServer::sharedInstance(), SIGNAL(stateChanged(bool)), this, SLOT(changeServerState(bool)));

    connect(DRS4RemoteControlServer::sharedInstance(), SIGNAL(startAcquisition()), this, SLOT(startStopThread()));
    connect(DRS4RemoteControlServer::sharedInstance(), SIGNAL(stopAcquisition()), this, SLOT(startStopThread()));

    /* start web server ... */
    if (DRS4ProgramSettingsManager::sharedInstance()->httpServerAutostart())
        DRS4WebServer::sharedInstance()->start(m_worker);
    else
        emit DRS4WebServer::sharedInstance()->stateChanged(false);

    connect(ui->widget_serverState, SIGNAL(clicked()), this, SLOT(showServerConfig()));
    connect(ui->widget_rcServerState, SIGNAL(clicked()), this, SLOT(showRCServerConfig()));

    /* start remote control server ... */
    if (DRS4ProgramSettingsManager::sharedInstance()->remoteControlServerAutostart())
        DRS4RemoteControlServer::sharedInstance()->start(m_worker);
    else
        emit DRS4RemoteControlServer::sharedInstance()->stateChanged(false);

    m_worker->moveToThread(m_workerThread);

    ui->label_cores->setNum((m_worker->maxThreads()==0)?1:m_worker->maxThreads());

    if ( m_worker->maxThreads() <= 1 ) {
        DRS4ProgramSettingsManager::sharedInstance()->setEnableMulticoreThreading(false);
        ui->checkBox_hyperthreading->setEnabled(false);
        ui->spinBox_parallelChunkSize->setEnabled(false);
        ui->label_87->setEnabled(false);
        ui->label_88->setEnabled(false);
        ui->label_89->setEnabled(false);
        ui->label_cores->setEnabled(false);

        ui->actionStart_True_False_Pulse_Streaming->setEnabled(true);
        ui->actionStop_True_False_Pulse_Streaming->setEnabled(false);
    }

    ui->widget_CFDAlgorithm->init(m_worker);

    m_boardInfoDlg = new DRS4BoardInfoDlg(DNULLPTR, m_worker);
    m_boardInfoDlg->hide();

    ui->widget_info->init(m_worker);

    m_pulseSaveRangeDlg = new DRS4PulseSaveRangeDlg(m_worker);
    m_pulseSaveRangeDlg->hide();

    m_serverDlg = new DRS4HttpServerConfigDlg(m_worker);
    m_serverDlg->hide();

    m_rcServerDlg = new DRS4RemoteControlServerConfigDlg(m_worker);
    m_rcServerDlg->hide();

    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveSettings()));
    connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(saveAsSettings()));
    connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(loadSettings()));
    connect(ui->actionLoad_Autosave, SIGNAL(triggered()), this, SLOT(loadAutoSave()));

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(showAddInfoBox()));    
    connect(ui->pushButton_writeAComment, SIGNAL(clicked()), this, SLOT(showAddInfoBox()));
    connect(ui->actionOPen, SIGNAL(triggered()), this, SLOT(showBoardInfoBox()));
    connect(ui->actionOpen_script, SIGNAL(triggered()), this, SLOT(showScriptBox()));
    connect(ui->actionSave_next_N_Pulses, SIGNAL(triggered()), this, SLOT(showSavePulses()));
    connect(ui->actionSave_next_N_Pulses_in_Range, SIGNAL(triggered()), this, SLOT(showSavePulsesRange()));
    connect(ui->actionOpen_calculator, SIGNAL(triggered()), this, SLOT(showCalculator()));
    connect(ui->actionLicense_GPLv3, SIGNAL(triggered()), this, SLOT(showGPL()));
    connect(ui->actionLicense_LGPLv3, SIGNAL(triggered()), this, SLOT(showLGPL()));
    connect(ui->actionUsed_License_GPLv3, SIGNAL(triggered()), this, SLOT(showUsedGPL()));
    connect(ui->actionOpen_serverConfig, SIGNAL(triggered()), this, SLOT(showServerConfig()));
    connect(ui->actionRemote_Control_server, SIGNAL(triggered()), this, SLOT(showRCServerConfig()));

    connect(ui->spinBox_fitIterations, SIGNAL(valueChanged(int)), this, SLOT(changeCoincidenceFitIterations(int)));
    connect(ui->spinBox_fitIterations_Merged, SIGNAL(valueChanged(int)), this, SLOT(changeMergedFitIterations(int)));
    connect(ui->spinBox_fitIterations_AB, SIGNAL(valueChanged(int)), this, SLOT(changeABFitIterations(int)));
    connect(ui->spinBox_fitIterations_BA, SIGNAL(valueChanged(int)), this, SLOT(changeBAFitIterations(int)));

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
        DRS4BoardManager::sharedInstance()->currentBoard()->SetFrequency(DRS4SettingsManager::sharedInstance()->sampleSpeedInGHz(), true);

        DRS4BoardManager::sharedInstance()->currentBoard()->SetTranspMode(1);
        DRS4BoardManager::sharedInstance()->currentBoard()->SetDominoMode(1);
        DRS4BoardManager::sharedInstance()->currentBoard()->SetDominoActive(1);

        DRS4BoardManager::sharedInstance()->currentBoard()->SetInputRange(0.0); // +/-0.5V

        DRS4BoardManager::sharedInstance()->currentBoard()->SetTriggerPolarity(true); //false=positive, true=negative

        DRS4BoardManager::sharedInstance()->currentBoard()->EnableTrigger(1, 0);
        DRS4BoardManager::sharedInstance()->currentBoard()->SetTriggerDelayNs(0);
    }

    ui->spinBox_startMinA->setRange(0, kNumberOfBins);
    ui->spinBox_startMaxA->setRange(0, kNumberOfBins);
    ui->spinBox_stopMinA->setRange(0, kNumberOfBins);
    ui->spinBox_stopMaxA->setRange(0, kNumberOfBins);

    ui->spinBox_startMinB->setRange(0, kNumberOfBins);
    ui->spinBox_startMaxB->setRange(0, kNumberOfBins);
    ui->spinBox_stopMinB->setRange(0, kNumberOfBins);
    ui->spinBox_stopMaxB->setRange(0, kNumberOfBins);

    ui->verticalSlider_triggerLevelA->setRange(-500, 500);
    ui->verticalSlider_triggerLevelB->setRange(-500, 500);

    ui->horizontalSlider_triggerDelay->setRange(0, DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());

    ui->verticalSlider_triggerLevelA->setValue(-15);
    ui->verticalSlider_triggerLevelB->setValue(-15);

    ui->horizontalSlider_triggerDelay->setValue(0);

    ui->checkBox_activateAreaFilter->setChecked(false);
    ui->checkBox_enableAreaFilter->setChecked(true);

    ui->checkBox_activateRiseTimeFilter->setChecked(false);
    ui->checkBox_enableRiseTimeFilter->setChecked(false);

    ui->doubleSpinBox_riseTimeFilterTimeScaleA->setRange(0, 50000.0);
    ui->doubleSpinBox_riseTimeFilterTimeScaleA->setValue(5.0);

    ui->doubleSpinBox_riseTimeFilterTimeScaleB->setRange(0, 50000.0);
    ui->doubleSpinBox_riseTimeFilterTimeScaleB->setValue(5.0);

    ui->spinBox_riseTimeFilter_binningA->setRange(100, 1000000);
    ui->spinBox_riseTimeFilter_binningA->setValue(1000);

    ui->spinBox_riseTimeFilter_binningB->setRange(100, 1000000);
    ui->spinBox_riseTimeFilter_binningB->setValue(1000);

    ui->spinBox_riseTimerFilterLeftA->setRange(0, 1000);
    ui->spinBox_riseTimerFilterLeftA->setValue(20);
    ui->spinBox_riseTimeFilterRightA->setRange(0, 1000);
    ui->spinBox_riseTimeFilterRightA->setValue(980);

    ui->spinBox_riseTimerFilterLeftB->setRange(0, 1000);
    ui->spinBox_riseTimerFilterLeftB->setValue(20);
    ui->spinBox_riseTimeFilterRightB->setRange(0, 1000);
    ui->spinBox_riseTimeFilterRightB->setValue(980);

    ui->checkBox_medianFilterAActivate->setChecked(false);
    ui->checkBox_medianFilterBActivate->setChecked(false);

    ui->checkBox_usingCFDB_A->setChecked(false);
    ui->checkBox_usingCFDA_B->setChecked(false);

    ui->spinBox_medianFilterWindowSizeA->setRange(3, kNumberOfBins-1);
    ui->spinBox_medianFilterWindowSizeB->setRange(3, kNumberOfBins-1);

    ui->spinBox_medianFilterWindowSizeA->setValue(3);
    ui->spinBox_medianFilterWindowSizeB->setValue(3);

    ui->spinBox_parallelChunkSize->setRange(1, 10000);
    ui->spinBox_parallelChunkSize->setValue(DRS4ProgramSettingsManager::sharedInstance()->pulsePairChunkSize());

    ui->checkBox_hyperthreading->setChecked(DRS4ProgramSettingsManager::sharedInstance()->isMulticoreThreadingEnabled());

    if (ui->checkBox_hyperthreading->isChecked()) {
        ui->actionStart_True_False_Pulse_Streaming->setEnabled(false);
        ui->actionStop_True_False_Pulse_Streaming->setEnabled(false);
    }
    else {
        ui->actionStart_True_False_Pulse_Streaming->setEnabled(true);
        ui->actionStop_True_False_Pulse_Streaming->setEnabled(false);
    }

    if (DRS4ProgramSettingsManager::sharedInstance()->isMulticoreThreadingEnabled()) {
        ui->actionSave_next_N_Pulses->setEnabled(false);
        ui->actionSave_next_N_Pulses_in_Range->setEnabled(false);
        ui->tabWidget_persistanceB->setEnabled(false);
        ui->checkBox_enablePersistance->setEnabled(false);
    }

    m_pulseSaveDlg = new DRS4PulseSaveDlg(m_worker);
    m_pulseSaveDlg->hide();

    connect(ui->spinBox_startMinA, SIGNAL(valueChanged(int)), this, SLOT(changePHSStartMinA(int)));
    connect(ui->spinBox_startMaxA, SIGNAL(valueChanged(int)), this, SLOT(changePHSStartMaxA(int)));
    connect(ui->spinBox_stopMinA, SIGNAL(valueChanged(int)), this, SLOT(changePHSStopMinA(int)));
    connect(ui->spinBox_stopMaxA, SIGNAL(valueChanged(int)), this, SLOT(changePHSStopMaxA(int)));

    connect(ui->spinBox_startMinB, SIGNAL(valueChanged(int)), this, SLOT(changePHSStartMinB(int)));
    connect(ui->spinBox_startMaxB, SIGNAL(valueChanged(int)), this, SLOT(changePHSStartMaxB(int)));
    connect(ui->spinBox_stopMinB, SIGNAL(valueChanged(int)), this, SLOT(changePHSStopMinB(int)));
    connect(ui->spinBox_stopMaxB, SIGNAL(valueChanged(int)), this, SLOT(changePHSStopMaxB(int)));

    connect(ui->pushButton_resetA, SIGNAL(clicked()), this, SLOT(resetPHSA()));
    connect(ui->pushButton_resetB, SIGNAL(clicked()), this, SLOT(resetPHSB()));

    connect(ui->pushButton_resetABLTSpectrum, SIGNAL(clicked()), this, SLOT(resetLTSpectrumABByPushButton()));
    connect(ui->pushButton_resetBALTSpectrum, SIGNAL(clicked()), this, SLOT(resetLTSpectrumBAByPushButton()));
    connect(ui->pushButton_resetCoincidenceLTSpectrum, SIGNAL(clicked()), this, SLOT(resetLTSpectrumCoincidenceByPushButton()));
    connect(ui->pushButton_resetMergedLTSpectrum, SIGNAL(clicked()), this, SLOT(resetLTSpectrumMergedByPushButton()));
    connect(ui->pushButton_resetAll, SIGNAL(clicked()), this, SLOT(resetAllLTSpectraByPushButton()));

    connect(ui->pushButton_resetPersistanceA, SIGNAL(clicked()), this, SLOT(resetPersistancePlotA()));
    connect(ui->pushButton_resetPersistanceB, SIGNAL(clicked()), this, SLOT(resetPersistancePlotB()));

    connect(ui->pushButton_resetAreaFilterA, SIGNAL(clicked()), this, SLOT(resetAreaPlotAByPushButton()));
    connect(ui->pushButton_resetAreaFilterB_2, SIGNAL(clicked()), this, SLOT(resetAreaPlotBByPushButton()));

    connect(ui->pushButton_resetRiseTimeFilterA, SIGNAL(clicked()), this, SLOT(resetRiseTimePlotAByPushButton()));
    connect(ui->pushButton_resetRiseTimeFilterB, SIGNAL(clicked()), this, SLOT(resetRiseTimePlotBByPushButton()));

    connect(ui->verticalSlider_triggerLevelA, SIGNAL(valueChanged(int)), this, SLOT(changeTriggerLevelA(int)));
    connect(ui->verticalSlider_triggerLevelB, SIGNAL(valueChanged(int)), this, SLOT(changeTriggerLevelB(int)));

    connect(ui->horizontalSlider_triggerDelay, SIGNAL(valueChanged(int)), this, SLOT(changeTriggerDelay(int)));

    connect(ui->pushButton_saveALTSpectrum, SIGNAL(clicked()), this, SLOT(saveABSpectrum()));
    connect(ui->pushButton_saveDQuickAB, SIGNAL(clicked()), this, SLOT(saveABSpectrumDQuickLTFit()));
    connect(ui->pushButton_saveBALTSpectrum, SIGNAL(clicked()), this, SLOT(saveBASpectrum()));
    connect(ui->pushButton_BADQuickLT, SIGNAL(clicked()), this, SLOT(saveBASpectrumDQuickLTFit()));
    connect(ui->pushButton_saveCoincidenceLTSpectrum, SIGNAL(clicked()), this, SLOT(saveCoincidenceSpectrum()));
    connect(ui->pushButton_coincDQuickLT, SIGNAL(clicked()), this, SLOT(saveCoincidenceSpectrumDQuickLTFit()));
    connect(ui->pushButton_saveMergedLTSpectrum, SIGNAL(clicked()), this, SLOT(saveMergedSpectrum()));
    connect(ui->pushButton_mergedDQuickLT, SIGNAL(clicked()), this, SLOT(saveMergedSpectrumDQuickLTFit()));

    connect(ui->pushButton_savePHSA, SIGNAL(clicked()), this, SLOT(savePHSA()));
    connect(ui->pushButton_savePHSB, SIGNAL(clicked()), this, SLOT(savePHSB()));

    connect(ui->pushButton_saveRiseTimeA, SIGNAL(clicked()), this, SLOT(saveRiseTimeDistributionA()));
    connect(ui->pushButton_saveRiseTimeB, SIGNAL(clicked()), this, SLOT(saveRiseTimeDistributionB()));

    connect(ui->pushButton_saveAreaCollectionA, SIGNAL(clicked()), this, SLOT(saveAreaDistributionA()));
    connect(ui->pushButton_saveAreaCollectionB, SIGNAL(clicked()), this, SLOT(saveAreaDistributionB()));

    connect(ui->checkBox_positiveTriggerPol, SIGNAL(clicked(bool)), this, SLOT(changePositivTriggerPolarity(bool)));
    connect(ui->checkBox_positivSignal, SIGNAL(clicked(bool)), this, SLOT(changePositivSignal(bool)));

    connect(ui->doubleSpinBox_cfdA, SIGNAL(valueChanged(double)), this, SLOT(changeCFDLevelA(double)));
    connect(ui->doubleSpinBox_cfdB, SIGNAL(valueChanged(double)), this, SLOT(changeCFDLevelB(double)));

    connect(ui->checkBox_activateAreaFilter, SIGNAL(clicked(bool)), this, SLOT(changePulseAreaFilterEnabled(bool)));
    connect(ui->checkBox_medianFilterAActivate, SIGNAL(clicked(bool)), this, SLOT(changeMedianFilterAEnabled(bool)));
    connect(ui->checkBox_medianFilterBActivate, SIGNAL(clicked(bool)), this, SLOT(changeMedianFilterBEnabled(bool)));
    connect(ui->checkBox_activateRiseTimeFilter, SIGNAL(clicked(bool)), this, SLOT(changeRiseTimeFilterEnabled(bool)));

    connect(ui->checkBox_enableAreaFilter, SIGNAL(clicked(bool)), this, SLOT(changePulseAreaFilterPlotEnabled(bool)));
    connect(ui->checkBox_enableRiseTimeFilter, SIGNAL(clicked(bool)), this, SLOT(changeRiseTimeFilterPlotEnabled(bool)));
    connect(ui->checkBox_enablePersistance, SIGNAL(clicked(bool)), this, SLOT(changePersistancePlotEnabled(bool)));
    connect(ui->checkBox_usingCFDB_A, SIGNAL(clicked(bool)), this, SLOT(changePersistancePlotUsingCFDB_For_A(bool)));
    connect(ui->checkBox_usingCFDA_B, SIGNAL(clicked(bool)), this, SLOT(changePersistancePlotUsingCFDA_For_B(bool)));
    connect(ui->checkBox_forceStopStopCoincindence, SIGNAL(clicked(bool)), this, SLOT(changeForceCoincidence(bool)));
    connect(ui->checkBox_burstMode, SIGNAL(clicked(bool)), this, SLOT(changeBurstModeEnabled(bool)));
    connect(ui->checkBox_hyperthreading, SIGNAL(clicked(bool)), this, SLOT(changeMulticoreThreadingEnabled(bool)));

    connect(ui->spinBox_startCell, SIGNAL(editingFinished()), this, SLOT(changeStartCell()));
    connect(ui->spinBox_stopCell, SIGNAL(editingFinished()), this, SLOT(changeStopCell()));

    connect(ui->lineEdit_triggerA, SIGNAL(editingFinished()), this, SLOT(changeTriggerLevelA2()));
    connect(ui->lineEdit_triggerB, SIGNAL(editingFinished()), this, SLOT(changeTriggerLevelB2()));

    connect(ui->doubleSpinBox_offsetAB, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsAB(double)));
    connect(ui->doubleSpinBox_offsetBA, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsBA(double)));
    connect(ui->doubleSpinBox_offsetCoincidence, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsCoincidence(double)));
    connect(ui->doubleSpinBox_offsetMerged, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsMerged(double)));

    connect(ui->doubleSpinBox_scalerAB, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsAB(double)));
    connect(ui->doubleSpinBox_scalerBA, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsBA(double)));
    connect(ui->doubleSpinBox_scalerCoincidence, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsCoincidence(double)));
    connect(ui->doubleSpinBox_scalerMerged, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsMerged(double)));

    ui->doubleSpinBox_persistanceLeftA->setRange(0, 2000);
    ui->doubleSpinBox_persistanceLeftB->setRange(0, 2000);
    ui->doubleSpinBox_persistanceRightA->setRange(0, 2000);
    ui->doubleSpinBox_persistanceRightB->setRange(0, 2000);

    connect(ui->doubleSpinBox_persistanceLeftA, SIGNAL(valueChanged(double)), this, SLOT(changeLeftAPersistance(double)));
    connect(ui->doubleSpinBox_persistanceLeftB, SIGNAL(valueChanged(double)), this, SLOT(changeLeftBPersistance(double)));
    connect(ui->doubleSpinBox_persistanceRightA, SIGNAL(valueChanged(double)), this, SLOT(changeRightAPersistance(double)));
    connect(ui->doubleSpinBox_persistanceRightB, SIGNAL(valueChanged(double)), this, SLOT(changeRightBPersistance(double)));

    connect(ui->spinBox_parallelChunkSize, SIGNAL(valueChanged(int)), this, SLOT(changePulsePairChunkSize(int)));

    connect(ui->spinBox_chnCountAB, SIGNAL(valueChanged(int)), this, SLOT(changeChannelSettingsAB2(int)));
    connect(ui->spinBox_chnCountBA, SIGNAL(valueChanged(int)), this, SLOT(changeChannelSettingsBA2(int)));
    connect(ui->spinBox_chnCountCoincidence, SIGNAL(valueChanged(int)), this, SLOT(changeChannelSettingsCoincidence2(int)));
    connect(ui->spinBox_chnCountMerged, SIGNAL(valueChanged(int)), this, SLOT(changeChannelSettingsMerged2(int)));

    connect(ui->doubleSpinBox_meanCoincidence, SIGNAL(valueChanged(double)), this, SLOT(changeMeanCableDelayPs(double)));

    connect(ui->doubleSpinBox_areaANorm, SIGNAL(valueChanged(double)), this, SLOT(changeNormalizationForPulseAreaFilterA(double)));
    connect(ui->doubleSpinBox_areaBNorm_2, SIGNAL(valueChanged(double)), this, SLOT(changeNormalizationForPulseAreaFilterB(double)));

    connect(ui->spinBox_areaBinningCntA, SIGNAL(valueChanged(int)), this, SLOT(changePulseAreaFilterA(int)));
    connect(ui->spinBox_areaBinningCntB_2, SIGNAL(valueChanged(int)), this, SLOT(changePulseAreaFilterB(int)));

    connect(ui->doubleSpinBox_areaFilter_leftA_lower, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsLowerLeftA(double)));
    connect(ui->doubleSpinBox_areaFilter_rightA_lower, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsLowerRightA(double)));
    connect(ui->doubleSpinBox_areaFilter_leftA_upper, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsUpperLeftA(double)));
    connect(ui->doubleSpinBox_areaFilter_rightA_upper, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsUpperRightA(double)));

    connect(ui->doubleSpinBox_areaFilter_leftB_lower_2, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsLowerLeftB(double)));
    connect(ui->doubleSpinBox_areaFilter_rightB_lower_2, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsLowerRightB(double)));
    connect(ui->doubleSpinBox_areaFilter_leftB_upper_2, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsUpperLeftB(double)));
    connect(ui->doubleSpinBox_areaFilter_rightB_upper_2, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsUpperRightB(double)));

    connect(ui->doubleSpinBox_riseTimeFilterTimeScaleA, SIGNAL(valueChanged(double)), this, SLOT(changeRiseTimeFilterScaleInNanosecondsA(double)));
    connect(ui->doubleSpinBox_riseTimeFilterTimeScaleB, SIGNAL(valueChanged(double)), this, SLOT(changeRiseTimeFilterScaleInNanosecondsB(double)));

    connect(ui->spinBox_riseTimeFilter_binningA, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterBinningCountA(int)));
    connect(ui->spinBox_riseTimeFilter_binningB, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterBinningCountB(int)));

    connect(ui->spinBox_riseTimerFilterLeftA, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterLeftWindowA(int)));
    connect(ui->spinBox_riseTimeFilterRightA, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterRightWindowA(int)));

    connect(ui->spinBox_riseTimerFilterLeftB, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterLeftWindowB(int)));
    connect(ui->spinBox_riseTimeFilterRightB, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterRightWindowB(int)));

    connect(ui->spinBox_medianFilterWindowSizeA, SIGNAL(valueChanged(int)), this, SLOT(changeMedianFilterWindowSizeA(int)));
    connect(ui->spinBox_medianFilterWindowSizeB, SIGNAL(valueChanged(int)), this, SLOT(changeMedianFilterWindowSizeB(int)));

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        connect(m_temperatureTimer, SIGNAL(timeout()), this, SLOT(updateTemperature()));

    connect(ui->pushButton_scaleRange, SIGNAL(clicked()), this, SLOT(scaleRange()));
    connect(ui->pushButton_fullRange, SIGNAL(clicked()), this, SLOT(fullRange()));

    connect(ui->pushButton_clearCoincidenceFitData, SIGNAL(clicked()), this, SLOT(clearCoincidenceFitData()));
    connect(ui->pushButton_CoincidenceFit, SIGNAL(clicked()), this, SLOT(fitCoincidenceData()));

    connect(ui->pushButton_MergedFit, SIGNAL(clicked()), this, SLOT(fitMergedData()));
    connect(ui->pushButton_clearMergedFitData, SIGNAL(clicked()), this, SLOT(clearMergedFitData()));

    connect(ui->pushButton_ABFit, SIGNAL(clicked()), this, SLOT(fitABData()));
    connect(ui->pushButton_clearABFitData, SIGNAL(clicked()), this, SLOT(clearABFitData()));

    connect(ui->pushButton_BAFit, SIGNAL(clicked()), this, SLOT(fitBAData()));
    connect(ui->pushButton_clearBAFitData, SIGNAL(clicked()), this, SLOT(clearBAFitData()));

    connect(m_autoSaveTimer, SIGNAL(timeout()), this, SLOT(autoSave()));

    ui->comboBox_triggerLogic->addItem("A");
    ui->comboBox_triggerLogic->addItem("B");
    ui->comboBox_triggerLogic->addItem("A && B");
    ui->comboBox_triggerLogic->addItem("A || B");
    ui->comboBox_triggerLogic->addItem("External");

    ui->comboBox_triggerLogic->setCurrentIndex(2);

    /* board-specs (see manual) */
    ui->comboBox_sampleSpeed->addItem("~5.12GHz (~200ns)");
    ui->comboBox_sampleSpeed->addItem("~2.00GHz (~500ns)");
    ui->comboBox_sampleSpeed->addItem("~1.00GHz (~1000ns)");
    ui->comboBox_sampleSpeed->addItem("~0.50GHz (~2000ns)");

    ui->comboBox_sampleSpeed->setCurrentIndex(0);

    connect(ui->comboBox_triggerLogic, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTriggerLogic(int)));
    connect(ui->comboBox_sampleSpeed, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSampleSpeed(int)));

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
        DRS4BoardManager::sharedInstance()->currentBoard()->SetTriggerSource(DRS4SettingsManager::sharedInstance()->triggerSource());
        DRS4BoardManager::sharedInstance()->currentBoard()->SetIndividualTriggerLevel(DRS4SettingsManager::sharedInstance()->channelNumberA(), -0.015);
        DRS4BoardManager::sharedInstance()->currentBoard()->SetIndividualTriggerLevel(DRS4SettingsManager::sharedInstance()->channelNumberB(), -0.015);
    }

    /* set analog channels to default */
    connect(ui->checkBox_chn1_A, SIGNAL(clicked(bool)), this, SLOT(changeChannel1_A(bool)));
    connect(ui->checkBox_chn1_B, SIGNAL(clicked(bool)), this, SLOT(changeChannel1_B(bool)));

    connect(ui->checkBox_chn2_A, SIGNAL(clicked(bool)), this, SLOT(changeChannel2_A(bool)));
    connect(ui->checkBox_chn2_B, SIGNAL(clicked(bool)), this, SLOT(changeChannel2_B(bool)));

    connect(ui->checkBox_chn3_A, SIGNAL(clicked(bool)), this, SLOT(changeChannel3_A(bool)));
    connect(ui->checkBox_chn3_B, SIGNAL(clicked(bool)), this, SLOT(changeChannel3_B(bool)));

    connect(ui->checkBox_chn4_A, SIGNAL(clicked(bool)), this, SLOT(changeChannel4_A(bool)));
    connect(ui->checkBox_chn4_B, SIGNAL(clicked(bool)), this, SLOT(changeChannel4_B(bool)));

    emit ui->checkBox_chn1_A->clicked(true);
    emit ui->checkBox_chn2_B->clicked(true);

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
        ui->checkBox_chn1_A->setDisabled(true);
        ui->checkBox_chn2_A->setDisabled(true);
        ui->checkBox_chn3_A->setDisabled(true);
        ui->checkBox_chn4_A->setDisabled(true);

        ui->checkBox_chn1_B->setDisabled(true);
        ui->checkBox_chn2_B->setDisabled(true);
        ui->checkBox_chn3_B->setDisabled(true);
        ui->checkBox_chn4_B->setDisabled(true);

        disconnect(ui->checkBox_chn1_A, SIGNAL(clicked(bool)), this, SLOT(changeChannel1_A(bool)));
        disconnect(ui->checkBox_chn1_B, SIGNAL(clicked(bool)), this, SLOT(changeChannel1_B(bool)));

        disconnect(ui->checkBox_chn2_A, SIGNAL(clicked(bool)), this, SLOT(changeChannel2_A(bool)));
        disconnect(ui->checkBox_chn2_B, SIGNAL(clicked(bool)), this, SLOT(changeChannel2_B(bool)));

        disconnect(ui->checkBox_chn3_A, SIGNAL(clicked(bool)), this, SLOT(changeChannel3_A(bool)));
        disconnect(ui->checkBox_chn3_B, SIGNAL(clicked(bool)), this, SLOT(changeChannel3_B(bool)));

        disconnect(ui->checkBox_chn4_A, SIGNAL(clicked(bool)), this, SLOT(changeChannel4_A(bool)));
        disconnect(ui->checkBox_chn4_B, SIGNAL(clicked(bool)), this, SLOT(changeChannel4_B(bool)));
    }

    /* pulse shape filter record scheme */
    ui->comboBox_pulseShapeFilterRecordScheme->addItem("AB: A (start) - B (stop)");
    ui->comboBox_pulseShapeFilterRecordScheme->addItem("BA: B (start) - A (stop)");
    ui->comboBox_pulseShapeFilterRecordScheme->addItem("prompt: A (stop) - B (stop)");

    ui->comboBox_pulseShapeFilterRecordScheme->setCurrentIndex(DRS4PulseShapeFilterRecordScheme::Scheme::RC_AB);

    connect(ui->comboBox_pulseShapeFilterRecordScheme, SIGNAL(currentIndexChanged(int)), this, SLOT(changePulseShapeFilterRecordScheme(int)));
    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterRecordScheme(DRS4PulseShapeFilterRecordScheme::Scheme::RC_AB);

    ui->doubleSpinBox_cfdA->setValue(25);
    ui->doubleSpinBox_cfdB->setValue(25);

    ui->spinBox_fitIterations->setValue(40);
    ui->spinBox_fitIterations_Merged->setValue(40);
    ui->spinBox_fitIterations_AB->setValue(40);
    ui->spinBox_fitIterations_BA->setValue(40);


    DRS4SettingsManager::sharedInstance()->setNegativeLifetimeAccepted(true);
    DRS4SettingsManager::sharedInstance()->setPositivSignal(ui->checkBox_positivSignal->isChecked());
    DRS4SettingsManager::sharedInstance()->setIgnoreBusyState(false);

    DRS4SettingsManager::sharedInstance()->setBurstMode(false);
    DRS4SettingsManager::sharedInstance()->setStartCell(0);
    DRS4SettingsManager::sharedInstance()->setForceCoincidence(false);
    DRS4SettingsManager::sharedInstance()->setStopCell(kNumberOfBins-1);

    DRS4SettingsManager::sharedInstance()->setMeanCableDelayInNs(ui->doubleSpinBox_meanCoincidence->value()/1000.0f);

    ui->spinBox_startCell->setValue(0);
    ui->spinBox_stopCell->setValue(kNumberOfBins-1);

    ui->spinBox_startMinA->setValue(0);
    ui->spinBox_startMaxA->setValue(1000);
    ui->spinBox_stopMinA->setValue(0);
    ui->spinBox_stopMaxA->setValue(1000);

    ui->spinBox_startMinB->setValue(0);
    ui->spinBox_startMaxB->setValue(1000);
    ui->spinBox_stopMinB->setValue(0);
    ui->spinBox_stopMaxB->setValue(1000);

    ui->spinBox_chnCountAB->setRange(10, 1000000);
    ui->spinBox_chnCountBA->setRange(10, 1000000);
    ui->spinBox_chnCountCoincidence->setRange(10, 1000000);
    ui->spinBox_chnCountMerged->setRange(10, 1000000);

    ui->doubleSpinBox_offsetAB->setRange(-1000000, 1000000);
    ui->doubleSpinBox_offsetBA->setRange(-1000000, 1000000);
    ui->doubleSpinBox_offsetCoincidence->setRange(-1000000, 1000000);
    ui->doubleSpinBox_offsetMerged->setRange(-1000000, 1000000);

    ui->doubleSpinBox_scalerAB->setRange(0.010, 1000000);
    ui->doubleSpinBox_scalerBA->setRange(0.010, 1000000);
    ui->doubleSpinBox_scalerCoincidence->setRange(0.010, 1000000);
    ui->doubleSpinBox_scalerMerged->setRange(0.010, 1000000);

    ui->spinBox_chnCountAB->setValue(4096);
    ui->spinBox_chnCountBA->setValue(4096);
    ui->spinBox_chnCountCoincidence->setValue(4096);
    ui->spinBox_chnCountMerged->setValue(4096);

    ui->doubleSpinBox_scalerAB->setValue(30);
    ui->doubleSpinBox_scalerBA->setValue(30);
    ui->doubleSpinBox_scalerCoincidence->setValue(30);
    ui->doubleSpinBox_scalerMerged->setValue(30);

    ui->doubleSpinBox_offsetAB->setValue(5);
    ui->doubleSpinBox_offsetBA->setValue(5);
    ui->doubleSpinBox_offsetCoincidence->setValue(5);
    ui->doubleSpinBox_offsetMerged->setValue(5);

    ui->doubleSpinBox_meanCoincidence->setValue(0.0f);

    /* Autosave - Spectra */
    m_autoSaveSpectraTimer = new QTimer;
    m_autoSaveSpectraTimer->setInterval(300000);

    connect(m_autoSaveSpectraTimer, SIGNAL(timeout()), this, SLOT(autosaveAllSpectra()));

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        m_temperatureTimer->start();

    m_autoSaveTimer->start();
    m_autoSaveSpectraTimer->start();

    /* Connection Check Timer */
    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
        m_connectionTimer = new QTimer;
        m_connectionTimer->setInterval(1000);

        connect(m_connectionTimer, SIGNAL(timeout()), this, SLOT(checkForConnection()));

        m_connectionTimer->start();
    }

    /* CPU usage */
    connect(DRS4CPUUsageManager::sharedInstance(), SIGNAL(cpu(int)), ui->progressBar_cpuUsage, SLOT(setValue(int)));

    DRS4CPUUsageManager::sharedInstance()->start(500);

    ui->widget_play->setLiteralSVG(":/images/images/play");
    ui->widget_stop->setLiteralSVG(":/images/images/stop");

    ui->widget_play->setToolTip("Start Acquisition.");
    ui->widget_stop->setToolTip("Stop Acquisition.");

    ui->label_threadRunning->setText("State: Idle");
    ui->label_threadRunning->setStyleSheet("color: blue");

    ui->widget_stop->enableWidget(false);

    /* Pulse-Shape Filter */
    m_pulseShapeFilterTimerA = new QTimer;
    m_pulseShapeFilterTimerA->setInterval(200);

    m_pulseShapeFilterTimerB = new QTimer;
    m_pulseShapeFilterTimerB->setInterval(200);

    connect(m_pulseShapeFilterTimerA, SIGNAL(timeout()), this, SLOT(incrementPulseShapeFilterProgressA()));//;, Qt::QueuedConnection);
    connect(m_pulseShapeFilterTimerB, SIGNAL(timeout()), this, SLOT(incrementPulseShapeFilterProgressB()));//;, Qt::QueuedConnection);

    ui->widget_playPulseShapeFilterA->setLiteralSVG(":/images/images/play");
    ui->widget_playPulseShapeFilterB->setLiteralSVG(":/images/images/play");

    ui->widget_stopPulseShapeFilterA->setLiteralSVG(":/images/images/stop");
    ui->widget_stopPulseShapeFilterB->setLiteralSVG(":/images/images/stop");

    ui->widget_stopPulseShapeFilterA->enableWidget(false);
    ui->widget_stopPulseShapeFilterB->enableWidget(false);

    connect(ui->widget_playPulseShapeFilterA, SIGNAL(clicked()), this, SLOT(startAcquisitionOfPulseShapeFilterDataA()));
    connect(ui->widget_playPulseShapeFilterB, SIGNAL(clicked()), this, SLOT(startAcquisitionOfPulseShapeFilterDataB()));

    connect(ui->widget_stopPulseShapeFilterA, SIGNAL(clicked()), this, SLOT(stopAcquisitionOfPulseShapeFilterDataA()));
    connect(ui->widget_stopPulseShapeFilterB, SIGNAL(clicked()), this, SLOT(stopAcquisitionOfPulseShapeFilterDataB()));

    ui->progressBar_daqShapeFilterA->setValue(0);
    ui->progressBar_daqShapeFilterB->setValue(0);

    ui->progressBar_daqShapeFilterA->setDisabled(true);
    ui->progressBar_daqShapeFilterB->setDisabled(true);

    ui->spinBox_NPulsesShapeFilterA->setRange(50, 95000); // << that limit is given by the plot view widget!!!
    ui->spinBox_NPulsesShapeFilterA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedA());

    ui->spinBox_NPulsesShapeFilterB->setRange(50, 95000); // << that limit is given by the plot view widget!!!
    ui->spinBox_NPulsesShapeFilterB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedB());

    ui->checkBox_activatePulseShapeFilterA->setChecked(DRS4SettingsManager::sharedInstance()->pulseShapeFilterEnabledA());
    ui->checkBox_activatePulseShapeFilterB->setChecked(DRS4SettingsManager::sharedInstance()->pulseShapeFilterEnabledB());

    connect(ui->checkBox_activatePulseShapeFilterA, SIGNAL(clicked(bool)), this, SLOT(changePulseShapeFilterEnabledA(bool)));
    connect(ui->checkBox_activatePulseShapeFilterB, SIGNAL(clicked(bool)), this, SLOT(changePulseShapeFilterEnabledB(bool)));

    connect(ui->spinBox_NPulsesShapeFilterA, SIGNAL(valueChanged(int)), this, SLOT(changePulseShapeFilterPulseRecordNumberA(int)));
    connect(ui->spinBox_NPulsesShapeFilterB, SIGNAL(valueChanged(int)), this, SLOT(changePulseShapeFilterPulseRecordNumberB(int)));

    changePulseShapeFilterPulseRecordNumberA(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedA());
    changePulseShapeFilterPulseRecordNumberB(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedB());

    ui->doubleSpinBox_shapeFilterLeftCFDA->setRange(0.0, -__PULSESHAPEFILTER_LEFT_MAX);
    ui->doubleSpinBox_shapeFilterRightCFDA->setRange(0.0, __PULSESHAPEFILTER_RIGHT_MAX);

    ui->doubleSpinBox_shapeFilterLeftCFDA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterLeftInNsOfA());
    ui->doubleSpinBox_shapeFilterRightCFDA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterRightInNsOfA());

    connect(ui->doubleSpinBox_shapeFilterLeftCFDA, SIGNAL(valueChanged(double)), this, SLOT(changeLeftAPulseShapeFilter(double)));
    connect(ui->doubleSpinBox_shapeFilterRightCFDA, SIGNAL(valueChanged(double)), this, SLOT(changeRightAPulseShapeFilter(double)));

    changeLeftAPulseShapeFilter(DRS4SettingsManager::sharedInstance()->pulseShapeFilterLeftInNsOfA());
    changeRightAPulseShapeFilter(DRS4SettingsManager::sharedInstance()->pulseShapeFilterRightInNsOfA());

    ui->doubleSpinBox_shapeFilterLeftCFDB->setRange(0.0, -__PULSESHAPEFILTER_LEFT_MAX);
    ui->doubleSpinBox_shapeFilterRightCFDB->setRange(0.0, __PULSESHAPEFILTER_RIGHT_MAX);

    ui->doubleSpinBox_shapeFilterLeftCFDB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterLeftInNsOfB());
    ui->doubleSpinBox_shapeFilterRightCFDB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterRightInNsOfB());

    connect(ui->doubleSpinBox_shapeFilterLeftCFDB, SIGNAL(valueChanged(double)), this, SLOT(changeLeftBPulseShapeFilter(double)));
    connect(ui->doubleSpinBox_shapeFilterRightCFDB, SIGNAL(valueChanged(double)), this, SLOT(changeRightBPulseShapeFilter(double)));

    changeLeftBPulseShapeFilter(DRS4SettingsManager::sharedInstance()->pulseShapeFilterLeftInNsOfB());
    changeRightBPulseShapeFilter(DRS4SettingsManager::sharedInstance()->pulseShapeFilterRightInNsOfB());

    ui->doubleSpinBox_shapeFilterLeftCFDROIA->setRange(0.0, -__PULSESHAPEFILTER_LEFT_MAX);
    ui->doubleSpinBox_shapeFilterRightCFDROIA->setRange(0.0, __PULSESHAPEFILTER_RIGHT_MAX);

    ui->doubleSpinBox_shapeFilterLeftCFDROIA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfA());
    ui->doubleSpinBox_shapeFilterRightCFDROIA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfA());

    connect(ui->doubleSpinBox_shapeFilterLeftCFDROIA, SIGNAL(valueChanged(double)), this, SLOT(changeROILeftAPulseShapeFilter(double)));
    connect(ui->doubleSpinBox_shapeFilterRightCFDROIA, SIGNAL(valueChanged(double)), this, SLOT(changeROIRightAPulseShapeFilter(double)));

    changeROILeftAPulseShapeFilter(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfA());
    changeROIRightAPulseShapeFilter(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfA());

    ui->doubleSpinBox_shapeFilterLeftCFDROIB->setRange(0.0, -__PULSESHAPEFILTER_LEFT_MAX);
    ui->doubleSpinBox_shapeFilterRightCFDROIB->setRange(0.0, __PULSESHAPEFILTER_RIGHT_MAX);

    ui->doubleSpinBox_shapeFilterLeftCFDROIB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfB());
    ui->doubleSpinBox_shapeFilterRightCFDROIB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfB());

    connect(ui->doubleSpinBox_shapeFilterLeftCFDROIB, SIGNAL(valueChanged(double)), this, SLOT(changeROILeftBPulseShapeFilter(double)));
    connect(ui->doubleSpinBox_shapeFilterRightCFDROIB, SIGNAL(valueChanged(double)), this, SLOT(changeROIRightBPulseShapeFilter(double)));

    changeROILeftBPulseShapeFilter(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfB());
    changeROIRightBPulseShapeFilter(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfB());

    connect(ui->pushButton_shapeFilterCalcA, SIGNAL(clicked()), this, SLOT(calculatePulseShapeFilterSplineA()));
    connect(ui->pushButton_shapeFilterCalcB, SIGNAL(clicked()), this, SLOT(calculatePulseShapeFilterSplineB()));

    ui->doubleSpinBox_shapeFilterLLStddevA->setRange(0.1, 15.0);
    ui->doubleSpinBox_shapeFilterULStddevA->setRange(0.1, 15.0);

    ui->doubleSpinBox_shapeFilterLLStddevA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevLowerFractionA());
    ui->doubleSpinBox_shapeFilterULStddevA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevUpperFractionA());

    connect(ui->doubleSpinBox_shapeFilterLLStddevA,SIGNAL(valueChanged(double)), this, SLOT(changeLowerStdDevFractionAPulseShapeFilter(double)));
    connect(ui->doubleSpinBox_shapeFilterULStddevA,SIGNAL(valueChanged(double)), this, SLOT(changeUpperStdDevFractionAPulseShapeFilter(double)));

    changeLowerStdDevFractionAPulseShapeFilter(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevLowerFractionA());
    changeUpperStdDevFractionAPulseShapeFilter(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevUpperFractionA());

    ui->doubleSpinBox_shapeFilterLLStddevB->setRange(0.1, 15.0);
    ui->doubleSpinBox_shapeFilterULStddevB->setRange(0.1, 15.0);

    ui->doubleSpinBox_shapeFilterLLStddevB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevLowerFractionB());
    ui->doubleSpinBox_shapeFilterULStddevB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevUpperFractionB());

    connect(ui->doubleSpinBox_shapeFilterLLStddevB,SIGNAL(valueChanged(double)), this, SLOT(changeLowerStdDevFractionBPulseShapeFilter(double)));
    connect(ui->doubleSpinBox_shapeFilterULStddevB,SIGNAL(valueChanged(double)), this, SLOT(changeUpperStdDevFractionBPulseShapeFilter(double)));

    changeLowerStdDevFractionBPulseShapeFilter(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevLowerFractionB());
    changeUpperStdDevFractionBPulseShapeFilter(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevUpperFractionB());

    /* Baseline Jitter Correction/Filter */
    ui->spinBox_baseLineStartCell_A->setRange(0, 700);
    ui->spinBox_baseLineStartCell_A->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationStartCellA());

    connect(ui->spinBox_baseLineStartCell_A, SIGNAL(valueChanged(int)), this, SLOT(changeBaselineStartCellA(int)));

    changeBaselineStartCellA(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationStartCellA());

    ui->spinBox_baseLine_region_A->setRange(2, 324);
    ui->spinBox_baseLine_region_A->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationRegionA());

    connect(ui->spinBox_baseLine_region_A, SIGNAL(valueChanged(int)), this, SLOT(changeBaselineRegionA(int)));

    changeBaselineRegionA(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationRegionA());

    ui->doubleSpinBox_baseLineValue_A->setRange(-499.0, 499.0);
    ui->doubleSpinBox_baseLineValue_A->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationShiftValueInMVA());

    connect(ui->doubleSpinBox_baseLineValue_A, SIGNAL(valueChanged(double)), this, SLOT(changeBaselineShiftValueA(double)));

    changeBaselineShiftValueA(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationShiftValueInMVA());

    ui->doubleSpinBox_baseLineLimit_A->setRange(0.0, 100.0);
    ui->doubleSpinBox_baseLineLimit_A->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitInPercentageA());

    connect(ui->doubleSpinBox_baseLineLimit_A, SIGNAL(valueChanged(double)), this, SLOT(changeBaselineFilterRejectionLimitA(double)));

    changeBaselineFilterRejectionLimitA(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitInPercentageA());

    ui->checkBox_activateBaselineCorrection_A->setChecked(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationEnabledA());

    connect(ui->checkBox_activateBaselineCorrection_A, SIGNAL(clicked(bool)), this, SLOT(changeBaselineCorrectionEnabledA(bool)));

    changeBaselineCorrectionEnabledA(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationEnabledA());

    ui->checkBox_baseLineReject_A->setChecked(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitRejectLimitA());

    connect(ui->checkBox_baseLineReject_A, SIGNAL(clicked(bool)), this, SLOT(changeBaselineFilterEnabledA(bool)));

    changeBaselineFilterEnabledA(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitRejectLimitA());

    ui->spinBox_baseLineStartCell_B->setRange(0, 700);
    ui->spinBox_baseLineStartCell_B->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationStartCellB());

    connect(ui->spinBox_baseLineStartCell_B, SIGNAL(valueChanged(int)), this, SLOT(changeBaselineStartCellB(int)));

    changeBaselineStartCellB(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationStartCellB());

    ui->spinBox_baseLine_region_B->setRange(2, 324);
    ui->spinBox_baseLine_region_B->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationRegionB());

    connect(ui->spinBox_baseLine_region_B, SIGNAL(valueChanged(int)), this, SLOT(changeBaselineRegionB(int)));

    changeBaselineRegionB(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationRegionB());

    ui->doubleSpinBox_baseLineValue_B->setRange(-499.0, 499.0);
    ui->doubleSpinBox_baseLineValue_B->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationShiftValueInMVB());

    connect(ui->doubleSpinBox_baseLineValue_B, SIGNAL(valueChanged(double)), this, SLOT(changeBaselineShiftValueB(double)));

    changeBaselineShiftValueB(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationShiftValueInMVB());

    ui->doubleSpinBox_baseLineLimit_B->setRange(0.1, 100.0);
    ui->doubleSpinBox_baseLineLimit_B->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitInPercentageB());

    connect(ui->doubleSpinBox_baseLineLimit_B, SIGNAL(valueChanged(double)), this, SLOT(changeBaselineFilterRejectionLimitB(double)));

    changeBaselineFilterRejectionLimitB(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitInPercentageB());

    ui->checkBox_activateBaselineCorrection_B->setChecked(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationEnabledB());

    connect(ui->checkBox_activateBaselineCorrection_B, SIGNAL(clicked(bool)), this, SLOT(changeBaselineCorrectionEnabledB(bool)));

    changeBaselineCorrectionEnabledB(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationEnabledB());

    ui->checkBox_baseLineReject_B->setChecked(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitRejectLimitB());

    connect(ui->checkBox_baseLineReject_B, SIGNAL(clicked(bool)), this, SLOT(changeBaselineFilterEnabledB(bool)));

    changeBaselineFilterEnabledB(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitRejectLimitB());

    /* update - Timer */
    m_pulseRequestTimer->start();
    m_phsRequestTimer->start();
    m_areaRequestTimer->start();
    m_riseTimeRequestTimer->start();
    m_lifetimeRequestTimer->start();
    m_persistanceRequestTimer->start();

    connect(ui->widget_play, SIGNAL(clicked()), this, SLOT(startStopThread()));
    connect(ui->widget_stop, SIGNAL(clicked()), this, SLOT(startStopThread()));

    DRS4SettingsManager::sharedInstance()->load(":/settings/defaultSimulationSettings");
    ui->horizontalSlider_triggerDelay->setValue(DRS4SettingsManager::sharedInstance()->triggerDelayInNs());

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
        if ( loadSettingsFileFromExtern(":/settings/defaultSimulationSettings", false) )
            m_currentSettingsPath = NO_SETTINGS_FILE_PLACEHOLDER;
    }
    else {
        if ( loadSettingsFileFromExtern(":/settings/defaultSimulationSettings", false) )
            m_currentSettingsPath = NO_SETTINGS_FILE_PLACEHOLDER;
    }

    updateCurrentFileLabel();

    ui->tabWidget->tabBar()->setTabTextColor(0, Qt::blue); // signal scope
    ui->tabWidget->tabBar()->setTabTextColor(1, Qt::red); // phs
    ui->tabWidget->tabBar()->setTabTextColor(2, Qt::darkGreen); // spec A-B
    ui->tabWidget->tabBar()->setTabTextColor(3, Qt::darkGreen); // spec B-A
    ui->tabWidget->tabBar()->setTabTextColor(4, Qt::darkGreen); // spec prompt
    ui->tabWidget->tabBar()->setTabTextColor(5, Qt::darkGreen); // spec merged
}

DRS4ScopeDlg::~DRS4ScopeDlg()
{
    DDELETE_SAFETY(m_pulseSaveDlg);
    DDELETE_SAFETY(m_pulseSaveRangeDlg);
    DDELETE_SAFETY(m_addInfoDlg);
    DDELETE_SAFETY(m_boardInfoDlg);
    DDELETE_SAFETY(m_scriptDlg);
    DDELETE_SAFETY(m_calculatorDlg);
    DDELETE_SAFETY(m_gplDialog);
    DDELETE_SAFETY(m_lgplDialog);
    DDELETE_SAFETY(m_usedgplDialog);
    DDELETE_SAFETY(m_serverDlg);
    DDELETE_SAFETY(m_rcServerDlg);

    if ( m_worker->isRecordingForPulseShapeFilterA() )
        stopAcquisitionOfPulseShapeFilterDataA();

    if ( m_worker->isRecordingForPulseShapeFilterB() )
        stopAcquisitionOfPulseShapeFilterDataB();

    if ( m_workerThread ) {
        if ( m_workerThread->isRunning() )
            m_workerThread->exit(0);

        while ( m_workerThread->isRunning() ) {}
    }

    DDELETE_SAFETY(m_worker);
    DDELETE_SAFETY(m_dataExchange);

    DDELETE_SAFETY(m_temperatureTimer);
    DDELETE_SAFETY(m_autoSaveTimer);
    DDELETE_SAFETY(m_currentSettingsFileLabel);

    DDELETE_SAFETY(m_workerThread);

    DDELETE_SAFETY(m_pulseRequestTimer);
    DDELETE_SAFETY(m_phsRequestTimer);
    DDELETE_SAFETY(m_lifetimeRequestTimer);
    DDELETE_SAFETY(m_areaRequestTimer);
    DDELETE_SAFETY(m_riseTimeRequestTimer);
    DDELETE_SAFETY(m_persistanceRequestTimer);
    DDELETE_SAFETY(m_burstModeTimer);
    DDELETE_SAFETY(m_connectionTimer);
    DDELETE_SAFETY(m_autoSaveSpectraTimer);
    DDELETE_SAFETY(m_pulseShapeFilterTimerA);
    DDELETE_SAFETY(m_pulseShapeFilterTimerB);

    DDELETE_SAFETY(ui);
}

void DRS4ScopeDlg::initScopePlots()
{
    ui->widget_plotA->showXBottomGrid(false);
    ui->widget_plotA->showXTopGrid(false);
    ui->widget_plotA->showYLeftGrid(false);
    ui->widget_plotA->showYRightGrid(false);

    ui->widget_plotB->showXBottomGrid(false);
    ui->widget_plotB->showXTopGrid(false);
    ui->widget_plotB->showYLeftGrid(false);
    ui->widget_plotB->showYRightGrid(false);

    ui->widget_plotA->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_plotA->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_plotB->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_plotB->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_plotA->yLeft()->setAxisRange(-500, 500);
    ui->widget_plotB->yLeft()->setAxisRange(-500, 500);

    ui->widget_plotA->yLeft()->setAxisDistribution(2);
    ui->widget_plotB->yLeft()->setAxisDistribution(2);

    ui->widget_plotA->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());
    ui->widget_plotB->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());


    ui->widget_plotA->curve().at(0)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotA->curve().at(0)->setCurveWidth(2);
    ui->widget_plotA->curve().at(0)->setCurveColor(Qt::red);

    ui->widget_plotA->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotA->curve().at(1)->setCurveWidth(2);
    ui->widget_plotA->curve().at(1)->setCurveColor(Qt::blue);

    ui->widget_plotB->curve().at(0)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotB->curve().at(0)->setCurveWidth(2);
    ui->widget_plotB->curve().at(0)->setCurveColor(Qt::red);

    ui->widget_plotB->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotB->curve().at(1)->setCurveWidth(2);
    ui->widget_plotB->curve().at(1)->setCurveColor(Qt::blue);


    ui->widget_plotA->xBottom()->setAxisLabelText("Sweep [ns]");
    ui->widget_plotA->yLeft()->setAxisLabelText("U [mV]");

    ui->widget_plotB->xBottom()->setAxisLabelText("Sweep [ns]");
    ui->widget_plotB->yLeft()->setAxisLabelText("U [mV]");
}

void DRS4ScopeDlg::initPulseHeightSpectraPlots()
{
    ui->widget_phs_A->showXBottomGrid(false);
    ui->widget_phs_A->showXTopGrid(false);
    ui->widget_phs_A->showYLeftGrid(false);
    ui->widget_phs_A->showYRightGrid(false);

    ui->widget_phs_B->showXBottomGrid(false);
    ui->widget_phs_B->showXTopGrid(false);
    ui->widget_phs_B->showYLeftGrid(false);
    ui->widget_phs_B->showYRightGrid(false);

    ui->widget_phs_A->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_phs_A->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_phs_B->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_phs_B->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_phs_A->yLeft()->setAxisRange(0, 500);
    ui->widget_phs_B->yLeft()->setAxisRange(0, 500);

    ui->widget_phs_A->xBottom()->setAxisRange(0, kNumberOfBins);
    ui->widget_phs_B->xBottom()->setAxisRange(0, kNumberOfBins);

    ui->widget_phs_A->yLeft()->setAxisDistribution(1);
    ui->widget_phs_B->yLeft()->setAxisDistribution(1);

    ui->widget_phs_A->xBottom()->setAxisDistribution(8);
    ui->widget_phs_B->xBottom()->setAxisDistribution(8);

    ui->widget_phs_A->yLeft()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_phs_A->yLeft()->setNumberPrecision(0);

    ui->widget_phs_A->xBottom()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_phs_A->xBottom()->setNumberPrecision(0);

    ui->widget_phs_B->yLeft()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_phs_B->yLeft()->setNumberPrecision(0);

    ui->widget_phs_B->xBottom()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_phs_B->xBottom()->setNumberPrecision(0);

    ui->widget_phs_A->xBottom()->showHelpIncrements(false);
    ui->widget_phs_B->xBottom()->showHelpIncrements(false);

    ui->widget_phs_A->yLeft()->showHelpIncrements(false);
    ui->widget_phs_B->yLeft()->showHelpIncrements(false);


    ui->widget_phs_A->curve().at(0)->setCurveStyle(plot2DXCurve::cross);
    ui->widget_phs_A->curve().at(0)->setCurveWidth(4);
    ui->widget_phs_A->curve().at(0)->setCurveColor(Qt::green);

    ui->widget_phs_A->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_A->curve().at(1)->setCurveWidth(1);
    ui->widget_phs_A->curve().at(1)->setCurveColor(Qt::gray);

    ui->widget_phs_B->curve().at(0)->setCurveStyle(plot2DXCurve::cross);
    ui->widget_phs_B->curve().at(0)->setCurveWidth(4);
    ui->widget_phs_B->curve().at(0)->setCurveColor(Qt::green);

    ui->widget_phs_B->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_B->curve().at(1)->setCurveWidth(1);
    ui->widget_phs_B->curve().at(1)->setCurveColor(Qt::gray);


    //phs windows A:
    ui->widget_phs_A->curve().at(2)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_A->curve().at(2)->setCurveWidth(1);
    ui->widget_phs_A->curve().at(2)->setCurveColor(Qt::red);

    ui->widget_phs_A->curve().at(3)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_A->curve().at(3)->setCurveWidth(1);
    ui->widget_phs_A->curve().at(3)->setCurveColor(Qt::red);

    ui->widget_phs_A->curve().at(4)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_A->curve().at(4)->setCurveWidth(1);
    ui->widget_phs_A->curve().at(4)->setCurveColor(Qt::blue);

    ui->widget_phs_A->curve().at(5)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_A->curve().at(5)->setCurveWidth(1);
    ui->widget_phs_A->curve().at(5)->setCurveColor(Qt::blue);

    ui->widget_phs_A->curve().at(6)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_A->curve().at(6)->setCurveWidth(1);
    ui->widget_phs_A->curve().at(6)->setCurveColor(Qt::red);

    ui->widget_phs_A->curve().at(7)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_A->curve().at(7)->setCurveWidth(1);
    ui->widget_phs_A->curve().at(7)->setCurveColor(Qt::blue);

    //phs windows B:
    ui->widget_phs_B->curve().at(2)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_B->curve().at(2)->setCurveWidth(1);
    ui->widget_phs_B->curve().at(2)->setCurveColor(Qt::red);

    ui->widget_phs_B->curve().at(3)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_B->curve().at(3)->setCurveWidth(1);
    ui->widget_phs_B->curve().at(3)->setCurveColor(Qt::red);

    ui->widget_phs_B->curve().at(4)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_B->curve().at(4)->setCurveWidth(1);
    ui->widget_phs_B->curve().at(4)->setCurveColor(Qt::blue);

    ui->widget_phs_B->curve().at(5)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_B->curve().at(5)->setCurveWidth(1);
    ui->widget_phs_B->curve().at(5)->setCurveColor(Qt::blue);

    ui->widget_phs_B->curve().at(6)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_B->curve().at(6)->setCurveWidth(1);
    ui->widget_phs_B->curve().at(6)->setCurveColor(Qt::red);

    ui->widget_phs_B->curve().at(7)->setCurveStyle(plot2DXCurve::line);
    ui->widget_phs_B->curve().at(7)->setCurveWidth(1);
    ui->widget_phs_B->curve().at(7)->setCurveColor(Qt::blue);


    ui->widget_phs_B->xBottom()->setAxisLabelText("Channels [#]");
    ui->widget_phs_B->yLeft()->setAxisLabelText("Counts [#]");

    ui->widget_phs_A->xBottom()->setAxisLabelText("Channels [#]");
    ui->widget_phs_A->yLeft()->setAxisLabelText("Counts [#]");
}

void DRS4ScopeDlg::initABLTSpectrum()
{
    ui->widget_ltAB->showXBottomGrid(false);
    ui->widget_ltAB->showXTopGrid(false);
    ui->widget_ltAB->showYLeftGrid(false);
    ui->widget_ltAB->showYRightGrid(false);

    ui->widget_ltAB->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_ltAB->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_ltAB->yLeft()->setAxisRange(1, 500);
    ui->widget_ltAB->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->channelCntAB());

    ui->widget_ltAB->yLeft()->setAxisScaling(plot2DXAxis::logarithmic);

    ui->widget_ltAB->xBottom()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_ltAB->xBottom()->setNumberPrecision(0);

    ui->widget_ltAB->yLeft()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_ltAB->yLeft()->setNumberPrecision(0);

    ui->widget_ltAB->yLeft()->setAxisDistribution(1);
    ui->widget_ltAB->yLeft()->showHelpIncrements(false);
    ui->widget_ltAB->xBottom()->showHelpIncrements(false);


    ui->widget_ltAB->curve().at(0)->setCurveStyle(plot2DXCurve::circle);
    ui->widget_ltAB->curve().at(0)->setCurveWidth(6);
    ui->widget_ltAB->curve().at(0)->setCurveColor(Qt::blue);

    //fitting curve:
    ui->widget_ltAB->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ltAB->curve().at(1)->setCurveWidth(2);
    ui->widget_ltAB->curve().at(1)->setCurveColor(Qt::red);

    ui->widget_ltAB->xBottom()->setAxisLabelText("Channels [#]");
    ui->widget_ltAB->yLeft()->setAxisLabelText("Counts [#]");
}

void DRS4ScopeDlg::initBALTSpectrum()
{
    ui->widget_ltBA->showXBottomGrid(false);
    ui->widget_ltBA->showXTopGrid(false);
    ui->widget_ltBA->showYLeftGrid(false);
    ui->widget_ltBA->showYRightGrid(false);

    ui->widget_ltBA->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_ltBA->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_ltBA->yLeft()->setAxisRange(1, 500);
    ui->widget_ltBA->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->channelCntBA());

    ui->widget_ltBA->yLeft()->setAxisScaling(plot2DXAxis::logarithmic);

    ui->widget_ltBA->xBottom()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_ltBA->xBottom()->setNumberPrecision(0);

    ui->widget_ltBA->yLeft()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_ltBA->yLeft()->setNumberPrecision(0);

    ui->widget_ltBA->yLeft()->setAxisDistribution(1);
    ui->widget_ltBA->yLeft()->showHelpIncrements(false);
    ui->widget_ltBA->xBottom()->showHelpIncrements(false);

    ui->widget_ltBA->curve().at(0)->setCurveStyle(plot2DXCurve::circle);
    ui->widget_ltBA->curve().at(0)->setCurveWidth(6);
    ui->widget_ltBA->curve().at(0)->setCurveColor(Qt::blue);

    //fitting curve:
    ui->widget_ltBA->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ltBA->curve().at(1)->setCurveWidth(2);
    ui->widget_ltBA->curve().at(1)->setCurveColor(Qt::red);

    ui->widget_ltBA->xBottom()->setAxisLabelText("Channels [#]");
    ui->widget_ltBA->yLeft()->setAxisLabelText("Counts [#]");
}

void DRS4ScopeDlg::initCoincidenceLTSpectrum()
{
    ui->widget_ltConicidence->showXBottomGrid(false);
    ui->widget_ltConicidence->showXTopGrid(false);
    ui->widget_ltConicidence->showYLeftGrid(false);
    ui->widget_ltConicidence->showYRightGrid(false);

    ui->widget_ltConicidence ->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_ltConicidence->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_ltConicidence->yLeft()->setAxisRange(1, 500);
    ui->widget_ltConicidence->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->channelCntCoincindence());

    ui->widget_ltConicidence->yLeft()->setAxisScaling(plot2DXAxis::logarithmic);

    ui->widget_ltConicidence->xBottom()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_ltConicidence->xBottom()->setNumberPrecision(0);

    ui->widget_ltConicidence->yLeft()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_ltConicidence->yLeft()->setNumberPrecision(0);

    ui->widget_ltConicidence->yLeft()->setAxisDistribution(1);
    ui->widget_ltConicidence->yLeft()->showHelpIncrements(false);
    ui->widget_ltConicidence->xBottom()->showHelpIncrements(false);

    ui->widget_ltConicidence->curve().at(0)->setCurveStyle(plot2DXCurve::circle);
    ui->widget_ltConicidence->curve().at(0)->setCurveWidth(6);
    ui->widget_ltConicidence->curve().at(0)->setCurveColor(Qt::blue);

    //fitting curve:
    ui->widget_ltConicidence->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ltConicidence->curve().at(1)->setCurveWidth(2);
    ui->widget_ltConicidence->curve().at(1)->setCurveColor(Qt::red);

    ui->widget_ltConicidence->xBottom()->setAxisLabelText("Channels [#]");
    ui->widget_ltConicidence->yLeft()->setAxisLabelText("Counts [#]");

    //doppler coincidence range:
    ui->widget_ltConicidence->curve().at(2)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ltConicidence->curve().at(2)->setCurveWidth(1);
    ui->widget_ltConicidence->curve().at(2)->setCurveColor(Qt::red);

    ui->widget_ltConicidence->curve().at(3)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ltConicidence->curve().at(3)->setCurveWidth(1);
    ui->widget_ltConicidence->curve().at(3)->setCurveColor(Qt::red);
}

void DRS4ScopeDlg::initMergedLTSpectrum()
{
    ui->widget_ltMerged->showXBottomGrid(false);
    ui->widget_ltMerged->showXTopGrid(false);
    ui->widget_ltMerged->showYLeftGrid(false);
    ui->widget_ltMerged->showYRightGrid(false);

    ui->widget_ltMerged->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_ltMerged->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_ltMerged->yLeft()->setAxisRange(1, 500);
    ui->widget_ltMerged->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->channelCntMerged());

    ui->widget_ltMerged->yLeft()->setAxisScaling(plot2DXAxis::logarithmic);

    ui->widget_ltMerged->xBottom()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_ltMerged->xBottom()->setNumberPrecision(0);

    ui->widget_ltMerged->yLeft()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_ltMerged->yLeft()->setNumberPrecision(0);

    ui->widget_ltMerged->yLeft()->setAxisDistribution(1);
    ui->widget_ltMerged->yLeft()->showHelpIncrements(false);
    ui->widget_ltMerged->xBottom()->showHelpIncrements(false);

    ui->widget_ltMerged->curve().at(0)->setCurveStyle(plot2DXCurve::circle);
    ui->widget_ltMerged->curve().at(0)->setCurveWidth(6);
    ui->widget_ltMerged->curve().at(0)->setCurveColor(Qt::blue);

    //fitting curve:
    ui->widget_ltMerged->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ltMerged->curve().at(1)->setCurveWidth(2);
    ui->widget_ltMerged->curve().at(1)->setCurveColor(Qt::red);

    ui->widget_ltMerged->xBottom()->setAxisLabelText("Channels [#]");
    ui->widget_ltMerged->yLeft()->setAxisLabelText("Counts [#]");
}

void DRS4ScopeDlg::initBoardTPlot()
{
    ui->widget_boardT->showXBottomGrid(false);
    ui->widget_boardT->showXTopGrid(false);
    ui->widget_boardT->showYLeftGrid(false);
    ui->widget_boardT->showYRightGrid(false);

    ui->widget_boardT->xBottom()->setAxisPlotType(plot2DXAxis::timePlot);
    ui->widget_boardT->xTop()->setAxisPlotType(plot2DXAxis::timePlot);

    ui->widget_boardT->yLeft()->setAxisRange(20, 70); //20-70°C
    ui->widget_boardT->xBottom()->setAxisRange(0, 300000); //5min
    ui->widget_boardT->xTop()->setAxisRange(0, 300000); //5min

    ui->widget_boardT->xTop()->setValueDisplay(plot2DXAxis::hh_mm_ss);
    ui->widget_boardT->xBottom()->setValueDisplay(plot2DXAxis::hh_mm_ss);

    ui->widget_boardT->yLeft()->setAxisDistribution(6);

    ui->widget_boardT->xBottom()->setAxisDistribution(5);
    ui->widget_boardT->xTop()->setAxisDistribution(5);

    ui->widget_boardT->curve().at(0)->setCurveStyle(plot2DXCurve::rect);
    ui->widget_boardT->curve().at(0)->setCurveWidth(4);
    ui->widget_boardT->curve().at(0)->setCurveColor(Qt::red);

    ui->widget_boardT->xBottom()->setAxisLabelText("");
    ui->widget_boardT->yLeft()->setAxisLabelText("T [°C]");
}

void DRS4ScopeDlg::initPersistancePlots()
{
    //A:
    ui->widget_persistanceA->showXBottomGrid(false);
    ui->widget_persistanceA->showXTopGrid(false);
    ui->widget_persistanceA->showYLeftGrid(false);
    ui->widget_persistanceA->showYRightGrid(false);

    ui->widget_persistanceA->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_persistanceA->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_persistanceA->yLeft()->setAxisDistribution(7);
    ui->widget_persistanceA->xBottom()->setAxisDistribution(9);

    ui->widget_persistanceA->yLeft()->setAxisRange(-0.2f, 1.2f);
    ui->widget_persistanceA->xBottom()->setAxisRange(-DRS4SettingsManager::sharedInstance()->persistanceLeftInNsOfA(), DRS4SettingsManager::sharedInstance()->persistanceRightInNsOfA());

    ui->widget_persistanceA->yLeft()->setAxisScaling(plot2DXAxis::linear);

    ui->widget_persistanceA->curve().at(0)->setCurveStyle(plot2DXCurve::line);
    ui->widget_persistanceA->curve().at(0)->setCurveWidth(1);
    ui->widget_persistanceA->curve().at(0)->setCurveColor(Qt::blue);

    ui->widget_persistanceA->xBottom()->setAxisLabelText("Time [ns]");
    ui->widget_persistanceA->yLeft()->setAxisLabelText("CFD");

    //B:
    ui->widget_persistanceB->showXBottomGrid(false);
    ui->widget_persistanceB->showXTopGrid(false);
    ui->widget_persistanceB->showYLeftGrid(false);
    ui->widget_persistanceB->showYRightGrid(false);

    ui->widget_persistanceB->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_persistanceB->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_persistanceB->yLeft()->setAxisDistribution(7);
    ui->widget_persistanceB->xBottom()->setAxisDistribution(9);

    ui->widget_persistanceB->yLeft()->setAxisRange(-0.2f, 1.2f);
    ui->widget_persistanceB->xBottom()->setAxisRange(-DRS4SettingsManager::sharedInstance()->persistanceLeftInNsOfB(), DRS4SettingsManager::sharedInstance()->persistanceRightInNsOfB());

    ui->widget_persistanceB->yLeft()->setAxisScaling(plot2DXAxis::linear);

    ui->widget_persistanceB->curve().at(0)->setCurveStyle(plot2DXCurve::line);
    ui->widget_persistanceB->curve().at(0)->setCurveWidth(1);
    ui->widget_persistanceB->curve().at(0)->setCurveColor(Qt::blue);

    ui->widget_persistanceB->xBottom()->setAxisLabelText("Time [ns]");
    ui->widget_persistanceB->yLeft()->setAxisLabelText("CFD");
}

void DRS4ScopeDlg::initPulseShapeFilterPlots()
{
    //A:
    ui->widget_ShapeFilterPlotA->showXBottomGrid(false);
    ui->widget_ShapeFilterPlotA->showXTopGrid(false);
    ui->widget_ShapeFilterPlotA->showYLeftGrid(false);
    ui->widget_ShapeFilterPlotA->showYRightGrid(false);

    ui->widget_ShapeFilterPlotA->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_ShapeFilterPlotA->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_ShapeFilterPlotA->yLeft()->setAxisDistribution(5);
    ui->widget_ShapeFilterPlotA->xBottom()->setAxisDistribution(9);

    ui->widget_ShapeFilterPlotA->yLeft()->setAxisRange(-0.3f, 1.2f);

    ui->widget_ShapeFilterPlotA->yLeft()->setAxisScaling(plot2DXAxis::linear);

    /* pulses */
    ui->widget_ShapeFilterPlotA->curve().at(0)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ShapeFilterPlotA->curve().at(0)->setCurveWidth(1);
    ui->widget_ShapeFilterPlotA->curve().at(0)->setCurveColor(Qt::gray);

    /* left - ROI */
    ui->widget_ShapeFilterPlotA->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ShapeFilterPlotA->curve().at(1)->setCurveWidth(2);
    ui->widget_ShapeFilterPlotA->curve().at(1)->setCurveColor(Qt::red);

    /* right - ROI */
    ui->widget_ShapeFilterPlotA->curve().at(2)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ShapeFilterPlotA->curve().at(2)->setCurveWidth(2);
    ui->widget_ShapeFilterPlotA->curve().at(2)->setCurveColor(Qt::green);

    /* spline - mean */
    ui->widget_ShapeFilterPlotA->curve().at(3)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ShapeFilterPlotA->curve().at(3)->setCurveWidth(2);
    ui->widget_ShapeFilterPlotA->curve().at(3)->setCurveColor(Qt::red);

    ui->widget_ShapeFilterPlotA->xBottom()->setAxisLabelText("Time [ns]");
    ui->widget_ShapeFilterPlotA->yLeft()->setAxisLabelText("Norm. Pulse");

    /* spline - upper stddev */
    ui->widget_ShapeFilterPlotA->curve().at(4)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ShapeFilterPlotA->curve().at(4)->setCurveWidth(2);
    ui->widget_ShapeFilterPlotA->curve().at(4)->setCurveColor(Qt::blue);

    /* spline - lower stddev */
    ui->widget_ShapeFilterPlotA->curve().at(5)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ShapeFilterPlotA->curve().at(5)->setCurveWidth(2);
    ui->widget_ShapeFilterPlotA->curve().at(5)->setCurveColor(Qt::blue);

    ui->widget_ShapeFilterPlotA->xBottom()->setAxisLabelText("Time [ns]");
    ui->widget_ShapeFilterPlotA->yLeft()->setAxisLabelText("Norm. Pulse");

    //B:
    ui->widget_ShapeFilterPlotB->showXBottomGrid(false);
    ui->widget_ShapeFilterPlotB->showXTopGrid(false);
    ui->widget_ShapeFilterPlotB->showYLeftGrid(false);
    ui->widget_ShapeFilterPlotB->showYRightGrid(false);

    ui->widget_ShapeFilterPlotB->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_ShapeFilterPlotB->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_ShapeFilterPlotB->yLeft()->setAxisDistribution(5);
    ui->widget_ShapeFilterPlotB->xBottom()->setAxisDistribution(9);

    ui->widget_ShapeFilterPlotB->yLeft()->setAxisRange(-0.3f, 1.2f);

    ui->widget_ShapeFilterPlotB->yLeft()->setAxisScaling(plot2DXAxis::linear);

    /* pulses */
    ui->widget_ShapeFilterPlotB->curve().at(0)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ShapeFilterPlotB->curve().at(0)->setCurveWidth(1);
    ui->widget_ShapeFilterPlotB->curve().at(0)->setCurveColor(Qt::gray);

    /* left - ROI */
    ui->widget_ShapeFilterPlotB->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ShapeFilterPlotB->curve().at(1)->setCurveWidth(2);
    ui->widget_ShapeFilterPlotB->curve().at(1)->setCurveColor(Qt::red);

    /* right - ROI */
    ui->widget_ShapeFilterPlotB->curve().at(2)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ShapeFilterPlotB->curve().at(2)->setCurveWidth(2);
    ui->widget_ShapeFilterPlotB->curve().at(2)->setCurveColor(Qt::green);

    /* spline - mean */
    ui->widget_ShapeFilterPlotB->curve().at(3)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ShapeFilterPlotB->curve().at(3)->setCurveWidth(2);
    ui->widget_ShapeFilterPlotB->curve().at(3)->setCurveColor(Qt::red);

    ui->widget_ShapeFilterPlotB->xBottom()->setAxisLabelText("Time [ns]");
    ui->widget_ShapeFilterPlotB->yLeft()->setAxisLabelText("Norm. Pulse");

    /* spline - upper stddev */
    ui->widget_ShapeFilterPlotB->curve().at(4)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ShapeFilterPlotB->curve().at(4)->setCurveWidth(2);
    ui->widget_ShapeFilterPlotB->curve().at(4)->setCurveColor(Qt::blue);

    /* spline - lower stddev */
    ui->widget_ShapeFilterPlotB->curve().at(5)->setCurveStyle(plot2DXCurve::line);
    ui->widget_ShapeFilterPlotB->curve().at(5)->setCurveWidth(2);
    ui->widget_ShapeFilterPlotB->curve().at(5)->setCurveColor(Qt::blue);

    ui->widget_ShapeFilterPlotB->xBottom()->setAxisLabelText("Time [ns]");
    ui->widget_ShapeFilterPlotB->yLeft()->setAxisLabelText("Norm. Pulse");
}

void DRS4ScopeDlg::adaptPersistancePlotA()
{
    ui->widget_persistanceA->yLeft()->setAxisRange(-0.2f, 1.2f);
    ui->widget_persistanceA->xBottom()->setAxisRange(-DRS4SettingsManager::sharedInstance()->persistanceLeftInNsOfA(), DRS4SettingsManager::sharedInstance()->persistanceRightInNsOfA());
    ui->widget_persistanceA->updatePlotView();
}

void DRS4ScopeDlg::adaptPersistancePlotB()
{
    ui->widget_persistanceB->yLeft()->setAxisRange(-0.2f, 1.2f);
    ui->widget_persistanceB->xBottom()->setAxisRange(-DRS4SettingsManager::sharedInstance()->persistanceLeftInNsOfB(), DRS4SettingsManager::sharedInstance()->persistanceRightInNsOfB());
    ui->widget_persistanceB->updatePlotView();
}

void DRS4ScopeDlg::initPulseAreaFilterA()
{
    //A:
    ui->widget_plotAreaFilterA->showXBottomGrid(false);
    ui->widget_plotAreaFilterA->showXTopGrid(false);
    ui->widget_plotAreaFilterA->showYLeftGrid(false);
    ui->widget_plotAreaFilterA->showYRightGrid(false);

    ui->widget_plotAreaFilterA->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_plotAreaFilterA->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_plotAreaFilterA->yLeft()->setAxisDistribution(1);
    ui->widget_plotAreaFilterA->xBottom()->setAxisDistribution(1);

    ui->widget_plotAreaFilterA->yLeft()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA());
    ui->widget_plotAreaFilterA->xBottom()->setAxisRange(0, kNumberOfBins);
    ui->widget_plotAreaFilterA->xBottom()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_plotAreaFilterA->xBottom()->setNumberPrecision(0);

    ui->widget_plotAreaFilterA->yLeft()->setAxisScaling(plot2DXAxis::linear);
    ui->widget_plotAreaFilterA->yLeft()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_plotAreaFilterA->yLeft()->setNumberPrecision(0);

    ui->widget_plotAreaFilterA->yLeft()->showHelpIncrements(false);
    ui->widget_plotAreaFilterA->xBottom()->showHelpIncrements(false);

    //data:
    ui->widget_plotAreaFilterA->curve().at(0)->setCurveStyle(plot2DXCurve::cross);
    ui->widget_plotAreaFilterA->curve().at(0)->setCurveWidth(4);
    ui->widget_plotAreaFilterA->curve().at(0)->setCurveColor(Qt::blue);

    //upper limit:
    ui->widget_plotAreaFilterA->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotAreaFilterA->curve().at(1)->setCurveWidth(4);
    ui->widget_plotAreaFilterA->curve().at(1)->setCurveColor(Qt::red);

    //lower limit:
    ui->widget_plotAreaFilterA->curve().at(2)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotAreaFilterA->curve().at(2)->setCurveWidth(4);
    ui->widget_plotAreaFilterA->curve().at(2)->setCurveColor(Qt::green);

    //phs lower limit min:
    ui->widget_plotAreaFilterA->curve().at(3)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotAreaFilterA->curve().at(3)->setCurveWidth(2);
    ui->widget_plotAreaFilterA->curve().at(3)->setCurveColor(Qt::red);

    //phs lower limit max:
    ui->widget_plotAreaFilterA->curve().at(4)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotAreaFilterA->curve().at(4)->setCurveWidth(2);
    ui->widget_plotAreaFilterA->curve().at(4)->setCurveColor(Qt::red);

    //phs upper limit min:
    ui->widget_plotAreaFilterA->curve().at(5)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotAreaFilterA->curve().at(5)->setCurveWidth(2);
    ui->widget_plotAreaFilterA->curve().at(5)->setCurveColor(Qt::blue);

    //phs upper limit max:
    ui->widget_plotAreaFilterA->curve().at(6)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotAreaFilterA->curve().at(6)->setCurveWidth(2);
    ui->widget_plotAreaFilterA->curve().at(6)->setCurveColor(Qt::blue);

    ui->widget_plotAreaFilterA->xBottom()->setAxisLabelText("Pulse-Height - Channel A [0.488mV]");
    ui->widget_plotAreaFilterA->yLeft()->setAxisLabelText("Norm. A");

    updatePulseAreaFilterALimits();
}

void DRS4ScopeDlg::initPulseAreaFilterB()
{
    //B:
    ui->widget_plotAreaFilterB_2->showXBottomGrid(false);
    ui->widget_plotAreaFilterB_2->showXTopGrid(false);
    ui->widget_plotAreaFilterB_2->showYLeftGrid(false);
    ui->widget_plotAreaFilterB_2->showYRightGrid(false);

    ui->widget_plotAreaFilterB_2->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_plotAreaFilterB_2->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_plotAreaFilterB_2->yLeft()->setAxisDistribution(1);
    ui->widget_plotAreaFilterB_2->xBottom()->setAxisDistribution(1);

    ui->widget_plotAreaFilterB_2->yLeft()->showHelpIncrements(false);
    ui->widget_plotAreaFilterB_2->xBottom()->showHelpIncrements(false);

    ui->widget_plotAreaFilterB_2->yLeft()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB());
    ui->widget_plotAreaFilterB_2->xBottom()->setAxisRange(0, kNumberOfBins);

    ui->widget_plotAreaFilterB_2->xBottom()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_plotAreaFilterB_2->xBottom()->setNumberPrecision(0);

    ui->widget_plotAreaFilterB_2->yLeft()->setAxisScaling(plot2DXAxis::linear);
    ui->widget_plotAreaFilterB_2->yLeft()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_plotAreaFilterB_2->yLeft()->setNumberPrecision(0);

    //data:
    ui->widget_plotAreaFilterB_2->curve().at(0)->setCurveStyle(plot2DXCurve::cross);
    ui->widget_plotAreaFilterB_2->curve().at(0)->setCurveWidth(4);
    ui->widget_plotAreaFilterB_2->curve().at(0)->setCurveColor(Qt::blue);

    //upper limit:
    ui->widget_plotAreaFilterB_2->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotAreaFilterB_2->curve().at(1)->setCurveWidth(4);
    ui->widget_plotAreaFilterB_2->curve().at(1)->setCurveColor(Qt::red);

    //lower limit:
    ui->widget_plotAreaFilterB_2->curve().at(2)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotAreaFilterB_2->curve().at(2)->setCurveWidth(4);
    ui->widget_plotAreaFilterB_2->curve().at(2)->setCurveColor(Qt::green);

    //phs lower limit min:
    ui->widget_plotAreaFilterB_2->curve().at(3)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotAreaFilterB_2->curve().at(3)->setCurveWidth(2);
    ui->widget_plotAreaFilterB_2->curve().at(3)->setCurveColor(Qt::red);

    //phs lower limit max:
    ui->widget_plotAreaFilterB_2->curve().at(4)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotAreaFilterB_2->curve().at(4)->setCurveWidth(2);
    ui->widget_plotAreaFilterB_2->curve().at(4)->setCurveColor(Qt::red);

    //phs upper limit min:
    ui->widget_plotAreaFilterB_2->curve().at(5)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotAreaFilterB_2->curve().at(5)->setCurveWidth(2);
    ui->widget_plotAreaFilterB_2->curve().at(5)->setCurveColor(Qt::blue);

    //phs upper limit max:
    ui->widget_plotAreaFilterB_2->curve().at(6)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotAreaFilterB_2->curve().at(6)->setCurveWidth(2);
    ui->widget_plotAreaFilterB_2->curve().at(6)->setCurveColor(Qt::blue);

    ui->widget_plotAreaFilterB_2->xBottom()->setAxisLabelText("Pulse-Height - Channel B [0.488mV]");
    ui->widget_plotAreaFilterB_2->yLeft()->setAxisLabelText("Norm. A");

    updatePulseAreaFilterBLimits();
}

void DRS4ScopeDlg::initPulseRiseTimeFilterA()
{
    //A:
    ui->widget_plotrRiseTimeFilterA->showXBottomGrid(false);
    ui->widget_plotrRiseTimeFilterA->showXTopGrid(false);
    ui->widget_plotrRiseTimeFilterA->showYLeftGrid(false);
    ui->widget_plotrRiseTimeFilterA->showYRightGrid(false);

    ui->widget_plotrRiseTimeFilterA->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_plotrRiseTimeFilterA->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_plotrRiseTimeFilterA->yLeft()->setAxisDistribution(1);
    ui->widget_plotrRiseTimeFilterA->xBottom()->setAxisDistribution(11);

    ui->widget_plotrRiseTimeFilterA->yLeft()->setAxisRange(0, 1000);
    ui->widget_plotrRiseTimeFilterA->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfA());
    ui->widget_plotrRiseTimeFilterA->xBottom()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_plotrRiseTimeFilterA->xBottom()->setNumberPrecision(0);

    ui->widget_plotrRiseTimeFilterA->yLeft()->setAxisScaling(plot2DXAxis::logarithmic);
    ui->widget_plotrRiseTimeFilterA->yLeft()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_plotrRiseTimeFilterA->yLeft()->setNumberPrecision(0);

    ui->widget_plotrRiseTimeFilterA->yLeft()->showHelpIncrements(false);
    ui->widget_plotrRiseTimeFilterA->xBottom()->showHelpIncrements(false);

    //data:
    ui->widget_plotrRiseTimeFilterA->curve().at(0)->setCurveStyle(plot2DXCurve::circle);
    ui->widget_plotrRiseTimeFilterA->curve().at(0)->setCurveWidth(4);
    ui->widget_plotrRiseTimeFilterA->curve().at(0)->setCurveColor(Qt::blue);

    //left window:
    ui->widget_plotrRiseTimeFilterA->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotrRiseTimeFilterA->curve().at(1)->setCurveWidth(4);
    ui->widget_plotrRiseTimeFilterA->curve().at(1)->setCurveColor(Qt::red);

    //right window:
    ui->widget_plotrRiseTimeFilterA->curve().at(2)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotrRiseTimeFilterA->curve().at(2)->setCurveWidth(4);
    ui->widget_plotrRiseTimeFilterA->curve().at(2)->setCurveColor(Qt::green);

    ui->widget_plotrRiseTimeFilterA->xBottom()->setAxisLabelText("rise-time binning [#]");
    ui->widget_plotrRiseTimeFilterA->yLeft()->setAxisLabelText("counts [#]");

    updateRiseTimeFilterALimits();
}

void DRS4ScopeDlg::initPulseRiseTimeFilterB()
{
    //B:
    ui->widget_plotrRiseTimeFilterB->showXBottomGrid(false);
    ui->widget_plotrRiseTimeFilterB->showXTopGrid(false);
    ui->widget_plotrRiseTimeFilterB->showYLeftGrid(false);
    ui->widget_plotrRiseTimeFilterB->showYRightGrid(false);

    ui->widget_plotrRiseTimeFilterB->xBottom()->setAxisPlotType(plot2DXAxis::valuePlot);
    ui->widget_plotrRiseTimeFilterB->xTop()->setAxisPlotType(plot2DXAxis::valuePlot);

    ui->widget_plotrRiseTimeFilterB->yLeft()->setAxisDistribution(1);
    ui->widget_plotrRiseTimeFilterB->xBottom()->setAxisDistribution(11);

    ui->widget_plotrRiseTimeFilterB->yLeft()->setAxisRange(0, 1000);
    ui->widget_plotrRiseTimeFilterB->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfB());
    ui->widget_plotrRiseTimeFilterB->xBottom()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_plotrRiseTimeFilterB->xBottom()->setNumberPrecision(0);

    ui->widget_plotrRiseTimeFilterB->yLeft()->setAxisScaling(plot2DXAxis::logarithmic);
    ui->widget_plotrRiseTimeFilterB->yLeft()->setNumberFormat(plot2DXAxis::floating);
    ui->widget_plotrRiseTimeFilterB->yLeft()->setNumberPrecision(0);

    ui->widget_plotrRiseTimeFilterB->yLeft()->showHelpIncrements(false);
    ui->widget_plotrRiseTimeFilterB->xBottom()->showHelpIncrements(false);

    //data:
    ui->widget_plotrRiseTimeFilterB->curve().at(0)->setCurveStyle(plot2DXCurve::circle);
    ui->widget_plotrRiseTimeFilterB->curve().at(0)->setCurveWidth(4);
    ui->widget_plotrRiseTimeFilterB->curve().at(0)->setCurveColor(Qt::blue);

    //left window:
    ui->widget_plotrRiseTimeFilterB->curve().at(1)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotrRiseTimeFilterB->curve().at(1)->setCurveWidth(4);
    ui->widget_plotrRiseTimeFilterB->curve().at(1)->setCurveColor(Qt::red);

    //right window:
    ui->widget_plotrRiseTimeFilterB->curve().at(2)->setCurveStyle(plot2DXCurve::line);
    ui->widget_plotrRiseTimeFilterB->curve().at(2)->setCurveWidth(4);
    ui->widget_plotrRiseTimeFilterB->curve().at(2)->setCurveColor(Qt::green);

    ui->widget_plotrRiseTimeFilterB->xBottom()->setAxisLabelText("rise-time binning [#]");
    ui->widget_plotrRiseTimeFilterB->yLeft()->setAxisLabelText("counts [#]");

    updateRiseTimeFilterBLimits();
}

bool DRS4ScopeDlg::loadSimulationToolSettingsFromExtern(const QString &path, bool checkForExtension)
{
    DUNUSED_PARAM(checkForExtension);

    QMutexLocker locker(&m_mutex);

    if ( DRS4StreamDataLoader::sharedInstance()->isArmed() )
        return false;

    QString filePath = path;

     if ( filePath.split(".").last() != EXT_SIMULATION_INPUT_FILE.remove(QChar('.')) )
        filePath = filePath + EXT_SIMULATION_INPUT_FILE;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( !DRS4SimulationSettingsManager::sharedInstance()->load(filePath) ) {
            m_worker->setBusy(false);

            return false;
    }

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4PulseGenerator::sharedInstance()->update();

    m_worker->setBusy(false);

    return true;
}

bool DRS4ScopeDlg::loadDataStreamFromExtern(const QString &path, bool checkForExtension)
{
    QMutexLocker locker(&m_mutex);

    if ( DRS4StreamDataLoader::sharedInstance()->isArmed() )
        return false;

    QString filePath = path;

    if ( checkForExtension )
    {
        if ( filePath.split(".").last() != EXT_PULSE_STREAM_FILE.remove(QChar('.')) )
            filePath = filePath + EXT_PULSE_STREAM_FILE;
    }

    if ( path.isEmpty() || filePath.isEmpty() )
        return false;


    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( !DRS4StreamDataLoader::sharedInstance()->init(filePath, this, true) ) {
        m_worker->setBusy(false);

        return false;
    }

    DRS4BoardManager::sharedInstance()->setDemoFromStreamData(true);

    m_worker->setBusy(false);

    return true;
}

bool DRS4ScopeDlg::stopDataFromDataStream()
{
    QMutexLocker locker(&m_mutex);

    return DRS4StreamDataLoader::sharedInstance()->stop();
}

bool DRS4ScopeDlg::loadSettingsFileFromExtern(const QString &path, bool checkForExtension)
{
    QMutexLocker locker(&m_mutex);

    QString filePath = path;

    if ( checkForExtension )
    {
        if ( filePath.split(".").last() != EXT_LT_SETTINGS_FILE.remove(QChar('.')) )
            filePath = filePath + EXT_LT_SETTINGS_FILE;
    }

    const int oldValue = ui->horizontalSlider_triggerDelay->value();
    const double oldSweep = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();
    const double ratio = (double)oldValue/oldSweep;

    if ( !DRS4SettingsManager::sharedInstance()->load(filePath) )
        return false;
    else
        m_currentSettingsPath = path;

    locker.unlock();
    setup(oldValue, oldSweep, ratio);

    return true;
}

bool DRS4ScopeDlg::saveSettingsFileFromExtern(const QString &path, bool checkForExtension)
{
    QMutexLocker locker(&m_mutex);

    if ( path.isEmpty() )
        return false;

    if ( path == NO_SETTINGS_FILE_PLACEHOLDER )
        return false;


    QString filePath = path;

    if ( checkForExtension )
    {
        if ( filePath.split(".").last() != EXT_LT_SETTINGS_FILE.remove(QChar('.')) )
            filePath = filePath + EXT_LT_SETTINGS_FILE;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( m_currentSettingsPath == NO_SETTINGS_FILE_PLACEHOLDER )
    {
        if ( DRS4SettingsManager::sharedInstance()->save(filePath) )
        {
            m_currentSettingsPath = filePath;

            if ( DRS4ScriptManager::sharedInstance()->isArmed() ) {
                const QString currentFileName = updateCurrentFileLabel(true);

                emit signalUpdateCurrentFileLabelFromScript(currentFileName);
            }
            else
                updateCurrentFileLabel();

            m_worker->setBusy(false);

            return true;
        }
        else {
            m_worker->setBusy(false);

            return false;
        }
    }

    if ( !DRS4SettingsManager::sharedInstance()->save(filePath) ) {
        m_worker->setBusy(false);

        return false;
    }

    m_currentSettingsPath = filePath;

    if ( DRS4ScriptManager::sharedInstance()->isArmed() ) {
        const QString currentFileName = updateCurrentFileLabel(true);

        emit signalUpdateCurrentFileLabelFromScript(currentFileName);
    }
    else
        updateCurrentFileLabel();

    m_worker->setBusy(false);

    return true;
}

QString DRS4ScopeDlg::currentSettingsFile() const
{
    QMutexLocker locker(&m_mutex);

    return m_currentSettingsPath;
}

void DRS4ScopeDlg::changePulsePairChunkSize(int value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_parallelChunkSize->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4ProgramSettingsManager::sharedInstance()->setPulsePairChunkSize(value);

    m_worker->setBusy(false);
}

int DRS4ScopeDlg::pulsePairChunkSize() const
{
    QMutexLocker locker(&m_mutex);

    return DRS4ProgramSettingsManager::sharedInstance()->pulsePairChunkSize();
}

bool DRS4ScopeDlg::saveABSpectrumFromExtern(const QString &fileName)
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return false;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( fileName.isEmpty() ) {
        m_worker->setBusy(false);
        return false;
    }

    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) )
    {
        const QString res = QString::number(1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSAB()/(double)DRS4SettingsManager::sharedInstance()->channelCntAB(), 'f', 4);

        stream << "#" << "Lifetime: [Channel2 - Channel1]\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Channel-Resolution: " << res << "ps\n";
        stream << "# Total Counts: " << QString::number(QVariant(m_worker->countsSpectrumAB()).toDouble(), 'f', 0) << "[#]\n";
        stream << "channel\tcounts\n";

        for ( int i = 0 ; i < DRS4SettingsManager::sharedInstance()->channelCntAB() ; ++ i ) {
            stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->spectrumAB()->at(i)).toString() << "\n";
        }

        file.close();
        m_worker->setBusy(false);

        return true;
    }
    else {
        m_worker->setBusy(false);

        return false;
    }

    Q_UNREACHABLE();
}

bool DRS4ScopeDlg::saveBASpectrumFromExtern(const QString &fileName)
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
         return false;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( fileName.isEmpty() )
    {
        m_worker->setBusy(false);
        return false;
    }

    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) )
    {
        const QString res = QString::number(1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSBA()/(double)DRS4SettingsManager::sharedInstance()->channelCntBA(), 'f', 4);

        stream << "#" << "Lifetime: [Channel1 - Channel2]\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Channel-Resolution: " << res << "ps\n";
        stream << "# Total Counts: " << QString::number(QVariant(m_worker->countsSpectrumBA()).toDouble(), 'f', 0) << "[#]\n";
        stream << "channel\tcounts\n";

        for ( int i = 0 ; i < DRS4SettingsManager::sharedInstance()->channelCntBA() ; ++ i ) {
            stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->spectrumBA()->at(i)).toString() << "\n";
        }

        file.close();
        m_worker->setBusy(false);

        return true;
    }
    else {
        m_worker->setBusy(false);

        return false;
    }

    Q_UNREACHABLE();
}

bool DRS4ScopeDlg::saveMergedSpectrumFromExtern(const QString &fileName)
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
         return false;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( fileName.isEmpty() )
    {
        m_worker->setBusy(false);
        return false;
    }

    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const QString res = QString::number(1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSMerged()/(double)DRS4SettingsManager::sharedInstance()->channelCntMerged(), 'f', 4);

        stream << "#" << "Lifetime: [Merged]\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Channel-Resolution: " << res << "ps\n";
        stream << "# Total Counts: " << QString::number(QVariant(m_worker->countsSpectrumMerged()).toDouble(), 'f', 0) << "[#]\n";
        stream << "channel\tcounts\n";

        for ( int i = 0 ; i < DRS4SettingsManager::sharedInstance()->channelCntMerged() ; ++ i ) {
            stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->spectrumMerged()->at(i)).toString() << "\n";
        }

        file.close();
        m_worker->setBusy(false);

        return true;
    }
    else {
        m_worker->setBusy(false);

        return false;
    }

    Q_UNREACHABLE();
}

bool DRS4ScopeDlg::saveCoincidenceSpectrumFromExtern(const QString &fileName)
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
         return false;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( fileName.isEmpty() )
    {
        m_worker->setBusy(false);
        return false;
    }

    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const QString res = QString::number(1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence()/(double)DRS4SettingsManager::sharedInstance()->channelCntCoincindence(), 'f', 4);

        stream << "#" << "Lifetime: Prompt [Channel2 (Stop)- Channel1 (Stop)]\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Channel-Resolution: " << res << "ps\n";
        stream << "# Total Counts: " << QString::number(QVariant(m_worker->countsSpectrumCoincidence()).toDouble(), 'f', 0) << "[#]\n";
        stream << "channel\tcounts\n";

        for ( int i = 0 ; i < DRS4SettingsManager::sharedInstance()->channelCntCoincindence() ; ++ i ) {
            stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->spectrumCoincidence()->at(i)).toString() << "\n";
        }

        file.close();

        m_worker->setBusy(false);

        return true;
    }
    else {
        m_worker->setBusy(false);

        return false;
    }

    Q_UNREACHABLE();
}

bool DRS4ScopeDlg::savePHSAFromExtern(const QString &fileName)
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return false;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( fileName.isEmpty() )
    {
        m_worker->setBusy(false);
        return false;
    }

    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const QString res = QString::number(500.0f/(float)(kNumberOfBins), 'f', 3);

        stream << "#" << "PHS - A\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Channel-Resolution: " << res << "mV\n";
        stream << "# Total Counts: " << QString::number((double)m_worker->phsACounts(), 'f', 0) << "[#]\n";
        stream << "channel\tcounts\n";

        for ( int i = 0 ; i < m_worker->phsA()->size() ; ++ i ) {
            stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->phsA()->at(i)).toString() << "\n";
        }

        file.close();

        m_worker->setBusy(false);

        return true;
    }
    else
    {
        m_worker->setBusy(false);

        return false;
    }

    Q_UNREACHABLE();
}

bool DRS4ScopeDlg::savePHSBFromExtern(const QString &fileName)
{
    QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( fileName.isEmpty() )
    {
        m_worker->setBusy(false);
        return false;
    }

    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const QString res = QString::number(500.0f/((float)kNumberOfBins), 'f', 3);

        stream << "#" << "PHS - B\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Channel-Resolution: " << res << "mV\n";
        stream << "# Total Counts: " << QString::number((double)m_worker->phsBCounts(), 'f', 0) << "[#]\n";
        stream << "channel\tcounts\n";

        for ( int i = 0 ; i < m_worker->phsB()->size() ; ++ i ) {
            stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->phsB()->at(i)).toString() << "\n";
        }

        file.close();

        m_worker->setBusy(false);

        return true;
    }
    else
    {
        m_worker->setBusy(false);

        return false;
    }

    Q_UNREACHABLE();
}

bool DRS4ScopeDlg::startStreamingFromExtern(const QString &fileName, bool checkForExtension)
{
    QMutexLocker locker(&m_mutex);

    if ( DRS4StreamManager::sharedInstance()->isArmed() )
        return false;

    QString filePath = fileName;

    if ( checkForExtension )
    {
        if ( filePath.split(".").last() != EXT_PULSE_STREAM_FILE.remove(QChar('.')) )
            filePath = filePath + EXT_PULSE_STREAM_FILE;
    }

    if ( fileName.isEmpty() || filePath.isEmpty() )
        return false;

    DRS4StreamManager::sharedInstance()->init(filePath, this);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( DRS4StreamManager::sharedInstance()->start() )
    {
        m_worker->setBusy(false);

        ui->action_StartDAQStreaming->setEnabled(false);
        ui->actionStop_Running->setEnabled(true);

        return true;
    }
    else
    {
        m_worker->setBusy(false);

        return false;
    }

    Q_UNREACHABLE();
}

bool DRS4ScopeDlg::stopStreamingFromExtern()
{
    QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4StreamManager::sharedInstance()->stopAndSave();

    ui->action_StartDAQStreaming->setEnabled(true);
    ui->actionStop_Running->setEnabled(false);

    m_worker->setBusy(false);

    return true;
}

void DRS4ScopeDlg::runDQuickLTFit(const QString& projectPath)
{
    QSharedMemory sharedMemory;
    sharedMemory.setKey("DQuickLTFit0123456789qwetzuioasdfghjklerfgbnpokjn,.-234567890weuhcq8934cn43q8DQuickLTFit");

    if (!sharedMemory.create(1)) {
        MSGBOX("An instancen of DQuickLTFit is already running. Please save your currect working project, close DQuickLTFit and try again.");
        return;
    }

    sharedMemory.detach();

    QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    HINSTANCE retVal = ShellExecuteA(NULL,
                                     "open",
                                     (LPCSTR)(QString(QFileInfo(QCoreApplication::applicationFilePath()).absolutePath() + "/DQuickLTFit.exe").toStdString().c_str()),
                                     (LPCSTR)(projectPath.toStdString().c_str()),
                                     NULL,
                                     SW_SHOWDEFAULT);

    if ( ((int)retVal) < 32 ) {
        m_worker->setBusy(false);
        MSGBOX("Cannot run DQuickLTFit.");

        return;
    }

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::openDQuickLTFitABSpectrum()
{
    saveABSpectrumDQuickLTFit(true);

    runDQuickLTFit(QString("__autosaveSpecAB.dquicklt"));
}

void DRS4ScopeDlg::openDQuickLTFitBASpectrum()
{
    saveBASpectrumDQuickLTFit(true);

    runDQuickLTFit(QString("__autosaveSpecBA.dquicklt"));
}

void DRS4ScopeDlg::openDQuickLTFitCoincidenceSpectrum()
{
    saveCoincidenceSpectrumDQuickLTFit(true);

    runDQuickLTFit(QString("__autosaveSpecPrompt.dquicklt"));
}

void DRS4ScopeDlg::openDQuickLTFitMergedSpectrum()
{
    saveMergedSpectrumDQuickLTFit(true);

    runDQuickLTFit(QString("__autosaveSpecMerged.dquicklt"));
}

void DRS4ScopeDlg::updateCurrentFileLabelFromScript(const QString &currentFile)
{
    m_currentSettingsFileLabel->setText(currentFile);
}

void DRS4ScopeDlg::updateInfoDlgFromScript(const QString &comment)
{
    m_addInfoDlg->setAddInfo(comment);

    ui->pushButton_shapeFilterCalcA->setText(QString("calculate (0)"));
    ui->pushButton_shapeFilterCalcB->setText(QString("calculate (0)"));
}

void DRS4ScopeDlg::updateThreadRunning(const QString &state, const QString &styleSheet)
{
    ui->label_threadRunning->setText(state);
    ui->label_threadRunning->setStyleSheet(styleSheet);
}

void DRS4ScopeDlg::startStopThread()
{
    if ( m_workerThread->isRunning() )
        stopThread();
    else
        startThread();
}

void DRS4ScopeDlg::startThread()
{
    if ( !m_workerThread->isRunning() ) {
        connect(m_workerThread, SIGNAL(started()), m_worker, SLOT(start()), Qt::DirectConnection);

        m_workerThread->start(QThread::TimeCriticalPriority);

        while ( !m_workerThread->isRunning() ) { QApplication::processEvents(); }
    }
    else
        return;

    if ( m_workerThread->isRunning() ) {
        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}
    }

    ui->widget_play->enableWidget(false);
    ui->widget_stop->enableWidget(true);

    ui->spinBox_parallelChunkSize->setEnabled(false);
    ui->checkBox_hyperthreading->setEnabled(false);

    if ( !DRS4ScriptManager::sharedInstance()->isArmed() ) {
        ui->label_threadRunning->setText("State: Running");
        ui->label_threadRunning->setStyleSheet("color: green");
    }
    else {
        emit signalUpdateThreadRunning(QString("State: Running"), QString("color: green"));
    }

    if ( !DRS4SettingsManager::sharedInstance()->isBurstMode() ) {
        if (!DRS4ProgramSettingsManager::sharedInstance()->isMulticoreThreadingEnabled()) {
            ui->actionSave_next_N_Pulses->setEnabled(true);
            ui->actionSave_next_N_Pulses_in_Range->setEnabled(true);
        }
        else {
            ui->actionSave_next_N_Pulses->setEnabled(false);
            ui->actionSave_next_N_Pulses_in_Range->setEnabled(false);
        }

        ui->actionLoad->setEnabled(true);
        ui->actionLoad_Autosave->setEnabled(true);
        ui->actionSave->setEnabled(true);
        ui->actionSave_as->setEnabled(true);
    }
    else {
        ui->actionSave_next_N_Pulses->setEnabled(false);
        ui->actionSave_next_N_Pulses_in_Range->setEnabled(false);

        ui->actionLoad->setEnabled(false);
        ui->actionLoad_Autosave->setEnabled(false);
        ui->actionSave->setEnabled(false);
        ui->actionSave_as->setEnabled(false);
    }

    if ( m_workerThread->isRunning() )
        m_worker->setBusy(false);
}

void DRS4ScopeDlg::stopThread()
{
    if ( m_workerThread->isRunning() ) {
        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}
    }

    if (DRS4TextFileStreamRangeManager::sharedInstance()->isArmed())
        DRS4TextFileStreamRangeManager::sharedInstance()->abort();

    if (DRS4TextFileStreamManager::sharedInstance()->isArmed())
        DRS4TextFileStreamManager::sharedInstance()->abort();

    if (m_worker->isRecordingForPulseShapeFilterA())
        m_worker->stopRecordingForShapeFilterA();

    if (m_worker->isRecordingForPulseShapeFilterB())
        m_worker->stopRecordingForShapeFilterB();

    if ( m_pulseSaveDlg->isVisible() ) {
        m_pulseSaveDlg->close();
        DDELETE_SAFETY(m_pulseSaveDlg);
        m_pulseSaveDlg = new DRS4PulseSaveDlg(m_worker);
    }

    if ( m_workerThread->isRunning() )
        m_worker->setBusy(false);

    if ( m_pulseSaveRangeDlg->isVisible() ) {
        m_pulseSaveRangeDlg->close();
        DDELETE_SAFETY(m_pulseSaveRangeDlg);
        m_pulseSaveRangeDlg = new DRS4PulseSaveRangeDlg(m_worker);
    }

    if ( !DRS4SettingsManager::sharedInstance()->isBurstMode() ) {
        if (!DRS4ProgramSettingsManager::sharedInstance()->isMulticoreThreadingEnabled()) {
            ui->actionSave_next_N_Pulses->setEnabled(true);
            ui->actionSave_next_N_Pulses_in_Range->setEnabled(true);
        }
        else {
            ui->actionSave_next_N_Pulses->setEnabled(false);
            ui->actionSave_next_N_Pulses_in_Range->setEnabled(false);
        }

        ui->actionLoad->setEnabled(true);
        ui->actionLoad_Autosave->setEnabled(true);
        ui->actionSave->setEnabled(true);
        ui->actionSave_as->setEnabled(true);
    }
    else {
        ui->actionSave_next_N_Pulses->setEnabled(false);
        ui->actionSave_next_N_Pulses_in_Range->setEnabled(false);

        ui->actionLoad->setEnabled(false);
        ui->actionLoad_Autosave->setEnabled(false);
        ui->actionSave->setEnabled(false);
        ui->actionSave_as->setEnabled(false);
    }

    if ( m_workerThread->isRunning() ) {
        m_worker->stop();

        while ( m_worker->isRunning() ) { QApplication::processEvents(); }
        m_workerThread->exit(0);
        while ( m_workerThread->isRunning() ) { QApplication::processEvents(); }

        disconnect(m_workerThread, SIGNAL(started()), m_worker, SLOT(start()));
    }
    else
        return;

    ui->widget_play->enableWidget(true);
    ui->widget_stop->enableWidget(false);

    ui->spinBox_parallelChunkSize->setEnabled(true);
    ui->checkBox_hyperthreading->setEnabled(true);

    if ( !DRS4ScriptManager::sharedInstance()->isArmed() ) {
        ui->label_threadRunning->setText("State: Idle");
        ui->label_threadRunning->setStyleSheet("color: blue");
    }
    else {
        emit signalUpdateThreadRunning(QString("State: Idle"), QString("color: blue"));
    }
}

void DRS4ScopeDlg::startAcquisitionOfPulseShapeFilterDataA()
{
    if (!isAcquisitionRunning())
        return;

    resetPulseShapeFilterProgressA();

    ui->progressBar_daqShapeFilterA->setDisabled(false);

    ui->spinBox_NPulsesShapeFilterA->setDisabled(true);    
    ui->pushButton_shapeFilterCalcA->setDisabled(true);
    ui->pushButton_shapeFilterCalcB->setDisabled(true);

    ui->widget_playPulseShapeFilterA->enableWidget(false);
    ui->widget_stopPulseShapeFilterA->enableWidget(true);

    ui->tabWidget_3->widget(0)->setDisabled(true);

    ui->label_pulseShapeFilterDaqStateA->setText("Running");
    ui->label_pulseShapeFilterDaqStateA->setStyleSheet("color: green");


    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->startRecordingForShapeFilterA(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedA());

    m_worker->setBusy(false);

    m_pulseShapeFilterTimerA->start();
}

void DRS4ScopeDlg::stopAcquisitionOfPulseShapeFilterDataA()
{
    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->stopRecordingForShapeFilterA();

    m_worker->setBusy(false);


    m_pulseShapeFilterTimerA->stop();

    resetPulseShapeFilterProgressA();

    ui->spinBox_NPulsesShapeFilterA->setDisabled(false);

    if (!m_worker->isRecordingForPulseShapeFilterB()) {
        ui->pushButton_shapeFilterCalcA->setDisabled(false);
        ui->pushButton_shapeFilterCalcB->setDisabled(false);
    }

    if (!DRS4SettingsManager::sharedInstance()->isBurstMode())
        ui->widget_playPulseShapeFilterA->enableWidget(true);

    ui->widget_stopPulseShapeFilterA->enableWidget(false);

    ui->tabWidget_3->widget(0)->setDisabled(false);

    ui->label_pulseShapeFilterDaqStateA->setText("Idle");
    ui->label_pulseShapeFilterDaqStateA->setStyleSheet("color: blue");

    swapPulseShapeFilterDataA();
}

void DRS4ScopeDlg::resetPulseShapeFilterProgressA()
{
    ui->progressBar_daqShapeFilterA->setDisabled(true);
    ui->progressBar_daqShapeFilterA->setRange(0, DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedA());
    ui->progressBar_daqShapeFilterA->setValue(0);

    ui->label_remainingPulsesA->setText(QString(QVariant(0).toString() % "/" % QVariant(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedA()).toString()));
}

void DRS4ScopeDlg::incrementPulseShapeFilterProgressA()
{
    QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    ui->progressBar_daqShapeFilterA->setValue(m_worker->pulseShapeFilterRecordingProgressA());
    ui->label_remainingPulsesA->setText(QString(QVariant(m_worker->pulseShapeFilterRecordingProgressA() - 1).toString() % "/" % QVariant(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedA()).toString()));

    if (!m_worker->isRecordingForPulseShapeFilterA())
        stopAcquisitionOfPulseShapeFilterDataA();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::swapPulseShapeFilterDataA()
{
    m_pulseShapeDataA.clear();

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_pulseShapeDataA.append(*m_worker->pulseShapeDataA());

     m_worker->setBusy(false);

    ui->pushButton_shapeFilterCalcA->setText(QString(QString("calculate (") % QVariant(m_worker->pulseShapeFilterRecordingProgressA() - 1).toString() % QString(")")));

    adaptPulseShapeFilterPlotA();
}

void DRS4ScopeDlg::changePulseShapeFilterPulseRecordNumberA(int value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_NPulsesShapeFilterA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterNumberOfPulsesToBeRecordedA(value);

    m_worker->setBusy(false);

    ui->label_remainingPulsesA->setText(QString(QVariant(0).toString() % "/" % QVariant(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedA()).toString()));
}

void DRS4ScopeDlg::startAcquisitionOfPulseShapeFilterDataB()
{
    if (!isAcquisitionRunning())
        return;

    resetPulseShapeFilterProgressB();

    ui->progressBar_daqShapeFilterB->setDisabled(false);

    ui->spinBox_NPulsesShapeFilterB->setDisabled(true);
    ui->pushButton_shapeFilterCalcA->setDisabled(true);
    ui->pushButton_shapeFilterCalcB->setDisabled(true);

    ui->widget_playPulseShapeFilterB->enableWidget(false);
    ui->widget_stopPulseShapeFilterB->enableWidget(true);

    ui->tabWidget_3->widget(0)->setDisabled(true);

    ui->label_pulseShapeFilterDaqStateB->setText("Running");
    ui->label_pulseShapeFilterDaqStateB->setStyleSheet("color: green");


    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->startRecordingForShapeFilterB(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedB());

    m_worker->setBusy(false);

    m_pulseShapeFilterTimerB->start();
}

void DRS4ScopeDlg::stopAcquisitionOfPulseShapeFilterDataB()
{
    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->stopRecordingForShapeFilterB();

    m_worker->setBusy(false);


    m_pulseShapeFilterTimerB->stop();

    resetPulseShapeFilterProgressB();

    ui->spinBox_NPulsesShapeFilterB->setDisabled(false);

    if (!m_worker->isRecordingForPulseShapeFilterA()) {
        ui->pushButton_shapeFilterCalcA->setDisabled(false);
        ui->pushButton_shapeFilterCalcB->setDisabled(false);
    }

    if (!DRS4SettingsManager::sharedInstance()->isBurstMode())
        ui->widget_playPulseShapeFilterB->enableWidget(true);

    ui->widget_stopPulseShapeFilterB->enableWidget(false);

    ui->tabWidget_3->widget(0)->setDisabled(false);

    ui->label_pulseShapeFilterDaqStateB->setText("Idle");
    ui->label_pulseShapeFilterDaqStateB->setStyleSheet("color: blue");

    swapPulseShapeFilterDataB();
}

void DRS4ScopeDlg::resetPulseShapeFilterProgressB()
{
    ui->progressBar_daqShapeFilterB->setDisabled(true);
    ui->progressBar_daqShapeFilterB->setRange(0, DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedB());
    ui->progressBar_daqShapeFilterB->setValue(0);

    ui->label_remainingPulsesB->setText(QString(QVariant(0).toString() % "/" % QVariant(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedB()).toString()));
}

void DRS4ScopeDlg::incrementPulseShapeFilterProgressB()
{
    QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    ui->progressBar_daqShapeFilterB->setValue(m_worker->pulseShapeFilterRecordingProgressB());
    ui->label_remainingPulsesB->setText(QString(QVariant(m_worker->pulseShapeFilterRecordingProgressB() - 1).toString() % "/" % QVariant(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedB()).toString()));

    if (!m_worker->isRecordingForPulseShapeFilterB())
        stopAcquisitionOfPulseShapeFilterDataB();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::swapPulseShapeFilterDataB()
{
    m_pulseShapeDataB.clear();

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_pulseShapeDataB.append(*m_worker->pulseShapeDataB());

     m_worker->setBusy(false);

    ui->pushButton_shapeFilterCalcB->setText(QString(QString("calculate (") % QVariant(m_worker->pulseShapeFilterRecordingProgressB() - 1).toString() % QString(")")));

    adaptPulseShapeFilterPlotB();
}

void DRS4ScopeDlg::changePulseShapeFilterPulseRecordNumberB(int value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_NPulsesShapeFilterB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterNumberOfPulsesToBeRecordedB(value);

    m_worker->setBusy(false);

    ui->label_remainingPulsesB->setText(QString(QVariant(0).toString() % "/" % QVariant(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedB()).toString()));
}

void DRS4ScopeDlg::changeLeftAPulseShapeFilter(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_shapeFilterLeftCFDA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterLeftInNsOfA(value);

    adaptPulseShapeFilterPlotA();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeLeftBPulseShapeFilter(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_shapeFilterLeftCFDB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterLeftInNsOfB(value);

    adaptPulseShapeFilterPlotB();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRightAPulseShapeFilter(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_shapeFilterRightCFDA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterRightInNsOfA(value);

    adaptPulseShapeFilterPlotA();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRightBPulseShapeFilter(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_shapeFilterRightCFDB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterRightInNsOfB(value);

    adaptPulseShapeFilterPlotB();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeROILeftAPulseShapeFilter(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_shapeFilterLeftCFDROIA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterROILeftInNsOfA(value);

    adaptPulseShapeFilterPlotA();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeROILeftBPulseShapeFilter(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_shapeFilterLeftCFDROIB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterROILeftInNsOfB(value);

    adaptPulseShapeFilterPlotB();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeROIRightAPulseShapeFilter(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_shapeFilterRightCFDROIA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterROIRightInNsOfA(value);

    adaptPulseShapeFilterPlotA();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeROIRightBPulseShapeFilter(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_shapeFilterRightCFDROIB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterROIRightInNsOfB(value);

    adaptPulseShapeFilterPlotB();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeLowerStdDevFractionAPulseShapeFilter(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_shapeFilterLLStddevA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterStdDevLowerFractionA(value);

    adaptPulseShapeFilterPlotA();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeUpperStdDevFractionAPulseShapeFilter(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_shapeFilterULStddevA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterStdDevUpperFractionA(value);

    adaptPulseShapeFilterPlotA();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeLowerStdDevFractionBPulseShapeFilter(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_shapeFilterLLStddevB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterStdDevLowerFractionB(value);

    adaptPulseShapeFilterPlotB();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeUpperStdDevFractionBPulseShapeFilter(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_shapeFilterULStddevB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterStdDevUpperFractionB(value);

    adaptPulseShapeFilterPlotB();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::adaptPulseShapeFilterPlotA()
{
    ui->widget_ShapeFilterPlotA->yLeft()->setAxisRange(-0.3f, 1.2f);
    ui->widget_ShapeFilterPlotA->xBottom()->setAxisRange(-DRS4SettingsManager::sharedInstance()->pulseShapeFilterLeftInNsOfA(), DRS4SettingsManager::sharedInstance()->pulseShapeFilterRightInNsOfA());
    ui->widget_ShapeFilterPlotA->updatePlotView();

    ui->widget_ShapeFilterPlotA->curve().at(0)->clearCurveCache();
    ui->widget_ShapeFilterPlotA->curve().at(0)->clearCurveContent();

     ui->widget_ShapeFilterPlotA->curve().at(1)->clearCurveCache();
     ui->widget_ShapeFilterPlotA->curve().at(1)->clearCurveContent();

     ui->widget_ShapeFilterPlotA->curve().at(2)->clearCurveCache();
     ui->widget_ShapeFilterPlotA->curve().at(2)->clearCurveContent();

     QPointF p1L(-DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfA(), -0.1);
     QPointF p2L(-DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfA(), 1.1);

     QPointF p1R(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfA(), -0.1);
     QPointF p2R(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfA(), 1.1);

     ui->widget_ShapeFilterPlotA->curve().at(0)->addData(m_pulseShapeDataA);

     ui->widget_ShapeFilterPlotA->curve().at(1)->addData(p1L);
     ui->widget_ShapeFilterPlotA->curve().at(1)->addData(p2L);

     ui->widget_ShapeFilterPlotA->curve().at(2)->addData(p1R);
     ui->widget_ShapeFilterPlotA->curve().at(2)->addData(p2R);

     if ( !m_pulseShapeSplineDataA.mean().isEmpty() ) {
         ui->widget_ShapeFilterPlotA->curve().at(3)->clearCurveCache();
         ui->widget_ShapeFilterPlotA->curve().at(3)->clearCurveContent();

         ui->widget_ShapeFilterPlotA->curve().at(4)->clearCurveCache();
         ui->widget_ShapeFilterPlotA->curve().at(4)->clearCurveContent();

         ui->widget_ShapeFilterPlotA->curve().at(5)->clearCurveCache();
         ui->widget_ShapeFilterPlotA->curve().at(5)->clearCurveContent();

         ui->widget_ShapeFilterPlotA->curve().at(3)->addData(DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrA()->mean());
         ui->widget_ShapeFilterPlotA->curve().at(4)->addData(DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrA()->stddevUpper(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevUpperFractionA()));
         ui->widget_ShapeFilterPlotA->curve().at(5)->addData(DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrA()->stddevLower(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevLowerFractionA()));
     }

    ui->widget_ShapeFilterPlotA->replot();
}

void DRS4ScopeDlg::adaptPulseShapeFilterPlotB()
{
    ui->widget_ShapeFilterPlotB->yLeft()->setAxisRange(-0.3f, 1.2f);
    ui->widget_ShapeFilterPlotB->xBottom()->setAxisRange(-DRS4SettingsManager::sharedInstance()->pulseShapeFilterLeftInNsOfB(), DRS4SettingsManager::sharedInstance()->pulseShapeFilterRightInNsOfB());
    ui->widget_ShapeFilterPlotB->updatePlotView();

    ui->widget_ShapeFilterPlotB->curve().at(0)->clearCurveCache();
    ui->widget_ShapeFilterPlotB->curve().at(0)->clearCurveContent();

     ui->widget_ShapeFilterPlotB->curve().at(1)->clearCurveCache();
     ui->widget_ShapeFilterPlotB->curve().at(1)->clearCurveContent();

     ui->widget_ShapeFilterPlotB->curve().at(2)->clearCurveCache();
     ui->widget_ShapeFilterPlotB->curve().at(2)->clearCurveContent();

     QPointF p1L(-DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfB(), -0.1);
     QPointF p2L(-DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfB(), 1.1);

     QPointF p1R(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfB(), -0.1);
     QPointF p2R(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfB(), 1.1);

     ui->widget_ShapeFilterPlotB->curve().at(0)->addData(m_pulseShapeDataB);

     ui->widget_ShapeFilterPlotB->curve().at(1)->addData(p1L);
     ui->widget_ShapeFilterPlotB->curve().at(1)->addData(p2L);

     ui->widget_ShapeFilterPlotB->curve().at(2)->addData(p1R);
     ui->widget_ShapeFilterPlotB->curve().at(2)->addData(p2R);

     if ( !m_pulseShapeSplineDataB.mean().isEmpty() ) {
         ui->widget_ShapeFilterPlotB->curve().at(3)->clearCurveCache();
         ui->widget_ShapeFilterPlotB->curve().at(3)->clearCurveContent();

         ui->widget_ShapeFilterPlotB->curve().at(4)->clearCurveCache();
         ui->widget_ShapeFilterPlotB->curve().at(4)->clearCurveContent();

         ui->widget_ShapeFilterPlotB->curve().at(5)->clearCurveCache();
         ui->widget_ShapeFilterPlotB->curve().at(5)->clearCurveContent();

         ui->widget_ShapeFilterPlotB->curve().at(3)->addData(DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrB()->mean());
         ui->widget_ShapeFilterPlotB->curve().at(4)->addData(DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrB()->stddevUpper(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevUpperFractionB()));
         ui->widget_ShapeFilterPlotB->curve().at(5)->addData(DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrB()->stddevLower(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevLowerFractionB()));
     }

    ui->widget_ShapeFilterPlotB->replot();
}

void DRS4ScopeDlg::calculatePulseShapeFilterSplineA()
{
    QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_pulseShapeDataA = *m_worker->pulseShapeDataA();

    if (m_pulseShapeDataA.isEmpty()) {
        m_worker->setBusy(false);

        return;
    }

    m_pulseShapeSplineDataA = m_worker->pulseShapeFilterSplineDataA();

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterDataA(m_pulseShapeSplineDataA);

    m_worker->setBusy(false);

    adaptPulseShapeFilterPlotA();
}

void DRS4ScopeDlg::calculatePulseShapeFilterSplineB()
{
    QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_pulseShapeDataB = *m_worker->pulseShapeDataB();

    if (m_pulseShapeDataB.isEmpty()) {
        m_worker->setBusy(false);

        return;
    }

    m_pulseShapeSplineDataB = m_worker->pulseShapeFilterSplineDataB();

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterDataB(m_pulseShapeSplineDataB);

    m_worker->setBusy(false);

    adaptPulseShapeFilterPlotB();
}

void DRS4ScopeDlg::changePulseShapeFilterEnabledA(bool activate, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if (m_pulseShapeSplineDataA.mean().isEmpty()) {
        if (ui->checkBox_activatePulseShapeFilterA->isChecked())
            ui->checkBox_activatePulseShapeFilterA->setChecked(false);
        else
            ui->checkBox_activatePulseShapeFilterA->setChecked(true);

        return;
    }

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_activatePulseShapeFilterA->clicked(activate);
        ui->checkBox_activatePulseShapeFilterA->setChecked(activate);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterEnabledA(activate);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseShapeFilterEnabledB(bool activate, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if (m_pulseShapeSplineDataB.mean().isEmpty()) {
        if (ui->checkBox_activatePulseShapeFilterB->isChecked())
            ui->checkBox_activatePulseShapeFilterB->setChecked(false);
        else
            ui->checkBox_activatePulseShapeFilterB->setChecked(true);

        return;
    }

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_activatePulseShapeFilterB->clicked(activate);
        ui->checkBox_activatePulseShapeFilterB->setChecked(activate);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterEnabledB(activate);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseShapeFilterRecordScheme(int index)
{
    QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseShapeFilterRecordScheme((DRS4PulseShapeFilterRecordScheme::Scheme)index);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBaselineStartCellA(int startCell, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_baseLineStartCell_A->setValue(startCell);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setBaselineCorrectionCalculationStartCellA(startCell);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBaselineRegionA(int region, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_baseLine_region_A->setValue(region);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setBaselineCorrectionCalculationRegionA(region);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBaselineShiftValueA(double baseline, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_baseLineValue_A->setValue(baseline);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setBaselineCorrectionCalculationShiftValueInMVA(baseline);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBaselineFilterRejectionLimitA(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_baseLineLimit_A->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setBaselineCorrectionCalculationLimitInPercentageA(value);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBaselineCorrectionEnabledA(bool enabled, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_activateBaselineCorrection_A->clicked(enabled);
        ui->checkBox_activateBaselineCorrection_A->setChecked(enabled);
        return;
    }


    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setBaselineCorrectionCalculationEnabledA(enabled);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBaselineFilterEnabledA(bool enabled, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_baseLineReject_A->clicked(enabled);
        ui->checkBox_baseLineReject_A->setChecked(enabled);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setBaselineCorrectionCalculationLimitRejectLimitA(enabled);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBaselineStartCellB(int startCell, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_baseLineStartCell_B->setValue(startCell);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setBaselineCorrectionCalculationStartCellB(startCell);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBaselineRegionB(int region, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_baseLine_region_B->setValue(region);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setBaselineCorrectionCalculationRegionB(region);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBaselineShiftValueB(double baseline, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_baseLineValue_B->setValue(baseline);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setBaselineCorrectionCalculationShiftValueInMVB(baseline);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBaselineFilterRejectionLimitB(double value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_baseLineLimit_B->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setBaselineCorrectionCalculationLimitInPercentageB(value);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBaselineCorrectionEnabledB(bool enabled, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_activateBaselineCorrection_B->clicked(enabled);
        ui->checkBox_activateBaselineCorrection_B->setChecked(enabled);
        return;
    }


    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setBaselineCorrectionCalculationEnabledB(enabled);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBaselineFilterEnabledB(bool enabled, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_baseLineReject_B->clicked(enabled);
        ui->checkBox_baseLineReject_B->setChecked(enabled);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setBaselineCorrectionCalculationLimitRejectLimitB(enabled);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePHSStartMinA(int value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_startMinA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setStartChannelAMin(value);

    m_worker->setBusy(false);

    plotPHSWindows();
}

void DRS4ScopeDlg::changePHSStartMaxA(int value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_startMaxA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setStartChannelAMax(value);

    m_worker->setBusy(false);

    plotPHSWindows();
}

void DRS4ScopeDlg::changePHSStopMinA(int value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_stopMinA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setStopChannelAMin(value);

    m_worker->setBusy(false);

    plotPHSWindows();
}

void DRS4ScopeDlg::changePHSStopMaxA(int value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_stopMaxA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setStopChannelAMax(value);

    m_worker->setBusy(false);

    plotPHSWindows();
}

void DRS4ScopeDlg::changePHSStartMinB(int value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_startMinB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setStartChannelBMin(value);

    m_worker->setBusy(true);

    plotPHSWindows();
}

void DRS4ScopeDlg::changePHSStartMaxB(int value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_startMaxB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setStartChannelBMax(value);

    m_worker->setBusy(false);

    plotPHSWindows();
}

void DRS4ScopeDlg::changePHSStopMinB(int value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_stopMinB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setStopChannelBMin(value);

    m_worker->setBusy(false);

    plotPHSWindows();
}

void DRS4ScopeDlg::changePHSStopMaxB(int value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_stopMaxB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setStopChannelBMax(value);

    m_worker->setBusy(false);

    plotPHSWindows();
}

int DRS4ScopeDlg::startCell() const
{
    QMutexLocker locker(&m_mutex);

    return ui->spinBox_startCell->value();
}

int DRS4ScopeDlg::stopCell() const
{
    QMutexLocker locker(&m_mutex);

    return ui->spinBox_stopCell->value();
}

void DRS4ScopeDlg::changeStartCell(int value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_startCell->setValue(value);
        emit ui->spinBox_startCell->editingFinished();
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setStartCell(ui->spinBox_startCell->value());
    ui->label_ReadoutRangeCell->setNum(DRS4SettingsManager::sharedInstance()->stopCell()-ui->spinBox_startCell->value());

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeStopCell(int value, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_stopCell->setValue(value);
        emit ui->spinBox_stopCell->editingFinished();
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setStopCell(ui->spinBox_stopCell->value());
    ui->label_ReadoutRangeCell->setNum(ui->spinBox_stopCell->value()-DRS4SettingsManager::sharedInstance()->startCell());

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeNormalizationForPulseAreaFilterA(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_areaANorm->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterNormalizationA(value);

    resetAreaPlotA(source);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeNormalizationForPulseAreaFilterB(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_areaBNorm_2->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterNormalizationB(value);

    resetAreaPlotB(source);

    m_worker->setBusy(false);    
}

void DRS4ScopeDlg::changePulseAreaFilterLimitsUpperLeftA(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_areaFilter_leftA_upper->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterLimitUpperLeftA(value);

    updatePulseAreaFilterALimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseAreaFilterLimitsUpperRightA(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_areaFilter_rightA_upper->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterLimitUpperRightA(value);

    updatePulseAreaFilterALimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseAreaFilterLimitsLowerLeftA(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_areaFilter_leftA_lower->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterLimitLowerLeftA(value);

    updatePulseAreaFilterALimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseAreaFilterLimitsLowerRightA(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_areaFilter_rightA_lower->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterLimitLowerRightA(value);

    updatePulseAreaFilterALimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseAreaFilterLimitsUpperLeftB(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_areaFilter_leftB_upper_2->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterLimitUpperLeftB(value);

    updatePulseAreaFilterBLimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseAreaFilterLimitsLowerLeftB(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_areaFilter_leftB_lower_2->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterLimitLowerLeftB(value);

    updatePulseAreaFilterBLimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseAreaFilterLimitsUpperRightB(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_areaFilter_rightB_upper_2->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterLimitUpperRightB(value);

    updatePulseAreaFilterBLimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseAreaFilterLimitsLowerRightB(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_areaFilter_rightB_lower_2->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterLimitLowerRightB(value);

    updatePulseAreaFilterBLimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseAreaFilterA(int value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_areaBinningCntA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterBinningA(value);

    resetAreaPlotA(source);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseAreaFilterB(int value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_areaBinningCntB_2->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterBinningB(value);

    resetAreaPlotB(source);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::plotPHSWindows()
{
    const double yMaxValA = ui->widget_phs_A->yLeft()->getAxisMaxValue();

    ui->widget_phs_A->curve().at(2)->clearCurveContent();
    ui->widget_phs_A->curve().at(3)->clearCurveContent();
    ui->widget_phs_A->curve().at(4)->clearCurveContent();
    ui->widget_phs_A->curve().at(5)->clearCurveContent();

    const QPointF p1A(ui->spinBox_startMinA->value(), 0);
    const QPointF p2A(ui->spinBox_startMinA->value(), yMaxValA);

    const QPointF p3A(ui->spinBox_startMaxA->value(), 0);
    const QPointF p4A(ui->spinBox_startMaxA->value(), yMaxValA);

    const QPointF p5A(ui->spinBox_stopMinA->value(), 0);
    const QPointF p6A(ui->spinBox_stopMinA->value(), yMaxValA);

    const QPointF p7A(ui->spinBox_stopMaxA->value(), 0);
    const QPointF p8A(ui->spinBox_stopMaxA->value(), yMaxValA);

    QVector<QPointF> pList1A; pList1A << p1A <<p2A;
    QVector<QPointF> pList2A; pList2A << p3A <<p4A;
    QVector<QPointF> pList3A; pList3A << p5A <<p6A;
    QVector<QPointF> pList4A; pList4A << p7A <<p8A;

    ui->widget_phs_A->curve().at(2)->addData(pList1A);
    ui->widget_phs_A->curve().at(3)->addData(pList2A);
    ui->widget_phs_A->curve().at(4)->addData(pList3A);
    ui->widget_phs_A->curve().at(5)->addData(pList4A);

    ui->widget_phs_A->replot();

    const double yMaxValB = ui->widget_phs_B->yLeft()->getAxisMaxValue();

    ui->widget_phs_B->curve().at(2)->clearCurveContent();
    ui->widget_phs_B->curve().at(3)->clearCurveContent();
    ui->widget_phs_B->curve().at(4)->clearCurveContent();
    ui->widget_phs_B->curve().at(5)->clearCurveContent();

    const QPointF p1B(ui->spinBox_startMinB->value(), 0);
    const QPointF p2B(ui->spinBox_startMinB->value(), yMaxValB);

    const QPointF p3B(ui->spinBox_startMaxB->value(), 0);
    const QPointF p4B(ui->spinBox_startMaxB->value(), yMaxValB);

    const QPointF p5B(ui->spinBox_stopMinB->value(), 0);
    const QPointF p6B(ui->spinBox_stopMinB->value(), yMaxValB);

    const QPointF p7B(ui->spinBox_stopMaxB->value(), 0);
    const QPointF p8B(ui->spinBox_stopMaxB->value(), yMaxValB);

    QVector<QPointF> pList1B; pList1B << p1B <<p2B;
    QVector<QPointF> pList2B; pList2B << p3B <<p4B;
    QVector<QPointF> pList3B; pList3B << p5B <<p6B;
    QVector<QPointF> pList4B; pList4B << p7B <<p8B;

    ui->widget_phs_B->curve().at(2)->addData(pList1B);
    ui->widget_phs_B->curve().at(3)->addData(pList2B);
    ui->widget_phs_B->curve().at(4)->addData(pList3B);
    ui->widget_phs_B->curve().at(5)->addData(pList4B);

    ui->widget_phs_B->replot();
}

void DRS4ScopeDlg::plotPulseAreaFilterData()
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return;

    if (!ui->tab_12->isVisible()
            && !ui->tab_36->isVisible())
        return;

    if ( !DRS4SettingsManager::sharedInstance()->isPulseAreaFilterPlotEnabled()
         || DRS4SettingsManager::sharedInstance()->isBurstMode() ) {
        m_areaRequestTimer->stop();

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        ui->label_areaCollectedCountsA->setNum(m_worker->countsCollectedInAreaFilterA());
        ui->label_areaCollectedCountsB->setNum(m_worker->countsCollectedInAreaFilterB());

        m_worker->setBusy(false);

        m_areaRequestTimer->start();

        return;
    }

    m_areaRequestTimer->stop();

    ui->widget_plotAreaFilterA->curve().at(0)->clearCurveContent();
    ui->widget_plotAreaFilterB_2->curve().at(0)->clearCurveContent();

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    ui->widget_plotAreaFilterA->curve().at(0)->addData(*m_worker->areaFilterAData());
    ui->widget_plotAreaFilterB_2->curve().at(0)->addData(*m_worker->areaFilterBData());

    ui->label_areaCollectedCountsA->setNum(m_worker->countsCollectedInAreaFilterA());
    ui->label_areaCollectedCountsB->setNum(m_worker->countsCollectedInAreaFilterB());

    m_worker->setBusy(false);

    ui->widget_plotAreaFilterA->yLeft()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA());
    ui->widget_plotAreaFilterB_2->yLeft()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB());

    updatePulseAreaFilterALimits();
    updatePulseAreaFilterBLimits();

    m_areaRequestTimer->start();
}

void DRS4ScopeDlg::plotRiseTimeFilterData()
{
    QMutexLocker locker(&m_mutex);

    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
        return;

    if ( !DRS4SettingsManager::sharedInstance()->isRiseTimeFilterPlotEnabled() )
        return;

    if (!m_worker)
        return;

    if (!ui->tab_17->isVisible()
            && !ui->tab_18->isVisible())
        return;

    m_riseTimeRequestTimer->stop();

    ui->widget_plotrRiseTimeFilterA->curve().at(0)->clearCurveContent();
    ui->widget_plotrRiseTimeFilterB->curve().at(0)->clearCurveContent();

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    ui->widget_plotrRiseTimeFilterA->curve().at(0)->addDataVec(*m_worker->riseTimeFilterAData());
    ui->widget_plotrRiseTimeFilterB->curve().at(0)->addDataVec(*m_worker->riseTimeFilterBData());

    m_worker->setBusy(false);

    const int valA = m_worker->riseTimeFilterADataMax();
    const int valB = m_worker->riseTimeFilterBDataMax();

    ui->widget_plotrRiseTimeFilterA->yLeft()->setAxisRange(1, valA<=/*1000?1000*/10?10:valA);
    ui->widget_plotrRiseTimeFilterB->yLeft()->setAxisRange(1, valB<=/*1000?1000*/10?10:valB);

    updateRiseTimeFilterALimits();
    updateRiseTimeFilterBLimits();

    const double resA = DRS4SettingsManager::sharedInstance()->riseTimeFilterScaleInNanosecondsOfA()/(double)DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfA();
    const double resB = DRS4SettingsManager::sharedInstance()->riseTimeFilterScaleInNanosecondsOfB()/(double)DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfB();

    ui->label_riseTimeChannelWidthA->setNum(resA);
    ui->label_riseTimeWindowA->setNum(((double)DRS4SettingsManager::sharedInstance()->riseTimeFilterRightWindowOfA()-(double)DRS4SettingsManager::sharedInstance()->riseTimeFilterLeftWindowOfA())*resA);

    ui->label_riseTimeChannelWidthB->setNum(resB);
    ui->label_riseTimeWindowB->setNum(((double)DRS4SettingsManager::sharedInstance()->riseTimeFilterRightWindowOfB()-(double)DRS4SettingsManager::sharedInstance()->riseTimeFilterLeftWindowOfB())*resB);

    m_riseTimeRequestTimer->start();
}

void DRS4ScopeDlg::plotLifetimeSpectra()
{
    QMutexLocker locker(&m_mutex);

    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
        return;

    if (!m_worker)
        return;

    m_lifetimeRequestTimer->stop();

    /* A-B */
    if (ui->tab_3->isVisible()) {
        /* AB-Spectrum */
        ui->widget_ltAB->curve().at(0)->clearCurveContent();
        ui->widget_ltAB->curve().at(1)->clearCurveContent();
    }

    /* B-A */
    if (ui->tab_4->isVisible()) {
        /* BA-Spectrum */
        ui->widget_ltBA->curve().at(0)->clearCurveContent();
        ui->widget_ltBA->curve().at(1)->clearCurveContent();
    }

    /* Prompt */
    if (ui->tab_5->isVisible()) {
        /* Prompt-Spectrum */
        ui->widget_ltConicidence->curve().at(0)->clearCurveContent();
        ui->widget_ltConicidence->curve().at(1)->clearCurveContent();
    }

    /* Merged */
    if (ui->tab_7->isVisible()) {
        /* Merged-Spectrum */
        ui->widget_ltMerged->curve().at(0)->clearCurveContent();
        ui->widget_ltMerged->curve().at(1)->clearCurveContent();
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const double countABHz = m_worker->currentLifetimeABCountRateInHz();
    const double avgCountABHz = m_worker->avgLifetimeABCountRateInHz();

    const double countBAHz = m_worker->currentLifetimeBACountRateInHz();
    const double avgCountBAHz = m_worker->avgLifetimeBACountRateInHz();

    const double countMergedHz = m_worker->currentLifetimeMergedCountRateInHz();
    const double avgCountMergedHz = m_worker->avgLifetimeMergedCountRateInHz();

    const double countCoincidenceHz = m_worker->currentLifetimeCoincidenceCountRateInHz();
    const double avgCountCoincidenceHz = m_worker->avgLifetimeCoincidenceCountRateInHz();

    const int yABMax = m_worker->maxYValueABSpectrum();
    const int yBAMax = m_worker->maxYValueBASpectrum();
    const int yMergedMax = m_worker->maxYValueMergedSpectrum();
    const int yCoincidenceMax = m_worker->maxYValueCoincidenceSpectrum();

    const int abCounts = m_worker->countsSpectrumAB();
    const int baCounts = m_worker->countsSpectrumBA();
    const int mergedCounts = m_worker->countsSpectrumMerged();
    const int coincidenceCounts = m_worker->countsSpectrumCoincidence();

    /* A-B */
    if (ui->tab_3->isVisible()) {
        ui->widget_ltAB->curve().at(0)->addDataVec(*m_worker->spectrumAB());
    }

    /* B-A */
    if (ui->tab_4->isVisible()) {
        ui->widget_ltBA->curve().at(0)->addDataVec(*m_worker->spectrumBA());
    }

    /* Merged */
    if (ui->tab_7->isVisible()) {
        ui->widget_ltMerged->curve().at(0)->addDataVec(*m_worker->spectrumMerged());
    }

    /* Prompt */
    if (ui->tab_5->isVisible()) {
        ui->widget_ltConicidence->curve().at(0)->addDataVec(*m_worker->spectrumCoincidence());
    }

    m_worker->setBusy(false);

    /* A-B */
    if (ui->tab_3->isVisible()) {
        ui->widget_ltAB->yLeft()->setAxisRange(1, qMax(yABMax, 2));
        ui->widget_ltAB->curve().at(1)->addData(m_fitPointsAB_Single);
        ui->widget_ltAB->yLeft()->setAxisScaling(plot2DXAxis::logarithmic);
        ui->widget_ltAB->replot();

        ui->label_countsIntergralAB->setNum(abCounts);

        if ( abCounts < 3000 )
            ui->pushButton_ABFit->setEnabled(false);
        else
            ui->pushButton_ABFit->setEnabled(true);
    }

    /* B-A */
    if (ui->tab_4->isVisible()) {
        ui->widget_ltBA->yLeft()->setAxisRange(1, qMax(yBAMax, 2));
        ui->widget_ltBA->curve().at(1)->addData(m_fitPointsBA_Single);
        ui->widget_ltBA->yLeft()->setAxisScaling(plot2DXAxis::logarithmic);
        ui->widget_ltBA->replot();

        ui->label_countsIntergralBA->setNum(baCounts);

        if ( baCounts < 3000 )
            ui->pushButton_BAFit->setEnabled(false);
        else
            ui->pushButton_BAFit->setEnabled(true);
    }

    /* Merged */
    if (ui->tab_7->isVisible()) {
        ui->widget_ltMerged->yLeft()->setAxisRange(1, qMax(yMergedMax, 2));
        ui->widget_ltMerged->curve().at(1)->addData(m_fitPointsMerged_Single);
        ui->widget_ltMerged->yLeft()->setAxisScaling(plot2DXAxis::logarithmic);
        ui->widget_ltMerged->replot();

        ui->label_countsIntergralMerged->setNum(mergedCounts);

        if ( mergedCounts < 3000 )
            ui->pushButton_MergedFit->setEnabled(false);
        else
            ui->pushButton_MergedFit->setEnabled(true);
    }

    /* Prompt */
    if (ui->tab_5->isVisible()) {
        ui->widget_ltConicidence->yLeft()->setAxisRange(1, qMax(yCoincidenceMax, 2));
        ui->widget_ltConicidence->curve().at(1)->addData(m_fitPoints);
        ui->widget_ltConicidence->yLeft()->setAxisScaling(plot2DXAxis::logarithmic);
        ui->widget_ltConicidence->replot();

        ui->label_countsIntergralCoincidence->setNum(coincidenceCounts);

        if ( coincidenceCounts < 3000 )
            ui->pushButton_CoincidenceFit->setEnabled(false);
        else
            ui->pushButton_CoincidenceFit->setEnabled(true);
    }

    ui->label_valiLTPerSec->setText("Lifetime Efficiency\t[Hz]:\t[A-B] " + QString::number(avgCountABHz, 'f', 2) + " (" + QString::number(countABHz, 'f', 2) + ") [B-A] " + QString::number(avgCountBAHz, 'f', 2) + " (" + QString::number(countBAHz, 'f', 2) + ") [Merged] " + QString::number(avgCountMergedHz, 'f', 2) + " (" + QString::number(countMergedHz, 'f', 2) + ")");
    ui->label_validCoincidencePerSec->setText("Prompt Efficiency\t[Hz]:\t" + QString::number(avgCountCoincidenceHz, 'f', 2) + " (" + QString::number(countCoincidenceHz, 'f', 2) + ")" );

    m_lifetimeRequestTimer->start();
}

void DRS4ScopeDlg::plotPersistance()
{
    QMutexLocker locker(&m_mutex);

    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
        return;

    if ( !DRS4SettingsManager::sharedInstance()->isPersistanceEnabled() )
        return;

    if (!m_worker)
        return;

    if (!ui->tab_persistance->isVisible())
        return;

    m_persistanceRequestTimer->stop();

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if (!m_worker->persistanceDataA()->isEmpty()
            && !m_worker->persistanceDataB()->isEmpty()) {
        if (ui->widget_persistanceA->curve().at(0)->getDataSize() >= 360000) {
            ui->widget_persistanceA->curve().at(0)->clearCurveContent();
            ui->widget_persistanceA->curve().at(0)->clearCurveCache();
        }

        if (ui->widget_persistanceB->curve().at(0)->getDataSize() >= 360000) {
            ui->widget_persistanceB->curve().at(0)->clearCurveContent();
            ui->widget_persistanceB->curve().at(0)->clearCurveCache();
        }

        ui->widget_persistanceA->curve().at(0)->addData(*m_worker->persistanceDataA(), m_bSwapDirection);
        ui->widget_persistanceB->curve().at(0)->addData(*m_worker->persistanceDataB(), m_bSwapDirection);

        m_bSwapDirection = m_bSwapDirection?false:true;
    }

    m_worker->setBusy(false);

    ui->widget_persistanceA->replot();
    ui->widget_persistanceB->replot();

    m_persistanceRequestTimer->start();
}

void DRS4ScopeDlg::changeLeftAPersistance(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_persistanceLeftA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPersistanceLeftInNsOfA(value);

    adaptPersistancePlotA();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeLeftBPersistance(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_persistanceLeftB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPersistanceLeftInNsOfB(value);

    adaptPersistancePlotB();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRightAPersistance(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_persistanceRightA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPersistanceRightInNsOfA(value);

    adaptPersistancePlotA();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRightBPersistance(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_persistanceRightB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPersistanceRightInNsOfB(value);

    adaptPersistancePlotB();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetPHSA(const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    DUNUSED_PARAM(source);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->resetPHSA();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetPHSB(const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    DUNUSED_PARAM(source);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->resetPHSB();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetAllLTSpectraByPushButton(const FunctionSource &source)
{
    const QString text = "This action will reset the following spectra. Are you sure ?<br><b><lu><li>Spec (A-B)</li><li>Spec (B-A)</li><li>Spec (Merged)</li><li>Spec(Prompt/IRF)</li><li>Persistence</li><li>All Filters</li></lu></b><br>The following spectra will not be resetted:<br><b><lu><li>PHS</li></lu></b>";

    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Reset all Spectra?", text, QMessageBox::Yes|QMessageBox::No);

    if ( reply == QMessageBox::Yes )
        resetAllLTSpectra(source);
}

void DRS4ScopeDlg::resetLTSpectrumABByPushButton(const FunctionSource &source)
{
    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Resetting Spectrum A-B. Are you sure?", "Reset AB Spectrum (Spec (A-B))?", QMessageBox::Yes|QMessageBox::No);

    if ( reply == QMessageBox::Yes )
        resetLTSpectrumAB(source);
}

void DRS4ScopeDlg::resetLTSpectrumBAByPushButton(const FunctionSource &source)
{
    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Resetting Spectrum B-A. Are you sure?", "Reset BA Spectrum (Spec (B-A)?", QMessageBox::Yes|QMessageBox::No);

    if ( reply == QMessageBox::Yes )
        resetLTSpectrumBA(source);
}

void DRS4ScopeDlg::resetLTSpectrumCoincidenceByPushButton(const FunctionSource &source)
{
    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Resetting Spectrum Prompt/IRF. Are you sure?", "Reset Prompt Spectrum (Prompt/IRF)?", QMessageBox::Yes|QMessageBox::No);

    if ( reply == QMessageBox::Yes )
        resetLTSpectrumCoincidence(source);
}

void DRS4ScopeDlg::resetLTSpectrumMergedByPushButton(const FunctionSource &source)
{
    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Resetting Merged Spectrum. Are you sure?", "Reset Merged Spectrum (Spec (Merged)?", QMessageBox::Yes|QMessageBox::No);

    if ( reply == QMessageBox::Yes )
        resetLTSpectrumMerged(source);
}

void DRS4ScopeDlg::resetAreaPlotAByPushButton(const FunctionSource &source)
{
    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Reset Pulse-Area Filter Plot A?", "Reset Pulse-Area Filter Plot A?", QMessageBox::Yes|QMessageBox::No);

    if ( reply == QMessageBox::Yes )
        resetAreaPlotA(source);
}

void DRS4ScopeDlg::resetAreaPlotBByPushButton(const FunctionSource &source)
{
    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Reset Pulse-Area Filter Plot B?", "Reset Pulse-Area Filter Plot B?", QMessageBox::Yes|QMessageBox::No);

    if ( reply == QMessageBox::Yes )
        resetAreaPlotB(source);
}

void DRS4ScopeDlg::resetRiseTimePlotAByPushButton(const FunctionSource &source)
{
    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Reset Rise-Time Filter Plot A?", "Reset Rise-Time Filter Plot A?", QMessageBox::Yes|QMessageBox::No);

    if ( reply == QMessageBox::Yes )
        resetRiseTimePlotA(source);
}

void DRS4ScopeDlg::resetRiseTimePlotBByPushButton(const FunctionSource &source)
{
    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Reset Rise-Time Filter Plot B?", "Reset Rise-Time Filter Plot B?", QMessageBox::Yes|QMessageBox::No);

    if ( reply == QMessageBox::Yes )
        resetRiseTimePlotB(source);
}

void DRS4ScopeDlg::resetPersistancePlotAByPushButton(const FunctionSource &source)
{
    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Reset Persistance Plot A?", "Reset Persistance Plot A?", QMessageBox::Yes|QMessageBox::No);

    if ( reply == QMessageBox::Yes )
        resetPersistancePlotA(source);
}

void DRS4ScopeDlg::resetPersistancePlotBByPushButton(const FunctionSource &source)
{
    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Reset Persistance Plot B?", "Reset Persistance Plot B?", QMessageBox::Yes|QMessageBox::No);

    if ( reply == QMessageBox::Yes )
        resetPersistancePlotB(source);
}

void DRS4ScopeDlg::autosaveAllSpectra()
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return;

    m_autoSaveSpectraTimer->stop();

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    saveABSpectrum(true);
    saveBASpectrum(true);
    saveCoincidenceSpectrum(true);
    saveMergedSpectrum(true);

    m_worker->setBusy(false);

    m_autoSaveSpectraTimer->start();
}

void DRS4ScopeDlg::resetPersistancePlotA(const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return;

    DUNUSED_PARAM(source);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    ui->widget_persistanceA->curve().at(0)->clearCurveContent();
    ui->widget_persistanceA->replot();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetPersistancePlotB(const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return;

    DUNUSED_PARAM(source);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    ui->widget_persistanceB->curve().at(0)->clearCurveContent();
    ui->widget_persistanceB->replot();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetAllLTSpectra(const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->resetABSpectrum();
    m_worker->resetBASpectrum();
    m_worker->resetCoincidenceSpectrum();
    m_worker->resetMergedSpectrum();

    clearCoincidenceFitData();
    clearMergedFitData();
    clearABFitData();
    clearBAFitData();

    locker.unlock();

    resetPersistancePlotA(source);
    resetPersistancePlotB(source);

    resetAreaPlotA(source);
    resetAreaPlotB(source);

    resetRiseTimePlotA(source);
    resetRiseTimePlotB(source);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetLTSpectrumAB(const FunctionSource &source)
{
    if (!m_worker)
        return;

    QMutexLocker locker(&m_mutex);

    DUNUSED_PARAM(source);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->resetABSpectrum();
    clearABFitData();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetLTSpectrumBA(const FunctionSource &source)
{
    if (!m_worker)
        return;

    QMutexLocker locker(&m_mutex);

    DUNUSED_PARAM(source);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->resetBASpectrum();
    clearBAFitData();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetLTSpectrumCoincidence(const FunctionSource &source)
{
    if (!m_worker)
        return;

    QMutexLocker locker(&m_mutex);

    DUNUSED_PARAM(source);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->resetCoincidenceSpectrum();
    clearCoincidenceFitData();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetLTSpectrumMerged(const FunctionSource &source)
{
    if (!m_worker)
        return;

    DUNUSED_PARAM(source);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    resetLTSpectrumAB();
    resetLTSpectrumBA();

    m_worker->resetMergedSpectrum();
    clearMergedFitData();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetAreaPlotA(const FunctionSource &source)
{
    if (!m_worker)
        return;

    QMutexLocker locker(&m_mutex);

    DUNUSED_PARAM(source);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->resetAreaFilterA();

    ui->widget_plotAreaFilterA->curve().at(0)->clearCurveContent();
    ui->widget_plotAreaFilterA->replot();

    updatePulseAreaFilterALimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetAreaPlotB(const FunctionSource &source)
{
    if (!m_worker)
        return;

    QMutexLocker locker(&m_mutex);

    DUNUSED_PARAM(source);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->resetAreaFilterB();

    ui->widget_plotAreaFilterB_2->curve().at(0)->clearCurveContent();
    ui->widget_plotAreaFilterB_2->replot();

    updatePulseAreaFilterBLimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRiseTimeFilterEnabled(bool on, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_activateRiseTimeFilter->setChecked(on);
        ui->checkBox_activateRiseTimeFilter->setChecked(on);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setRiseTimeFilterEnabled(on);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRiseTimeFilterPlotEnabled(bool on, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_enableRiseTimeFilter->clicked(on);
        ui->checkBox_enableRiseTimeFilter->setChecked(on);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setRiseTimeFilterPlotEnabled(on);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRiseTimeFilterScaleInNanosecondsA(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_riseTimeFilterTimeScaleA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setRiseTimeFilterScaleInNanosecondsOfA(value);

    resetRiseTimePlotA(source);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRiseTimeFilterScaleInNanosecondsB(double value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_riseTimeFilterTimeScaleB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setRiseTimeFilterScaleInNanosecondsOfB(value);

    resetRiseTimePlotB(source);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRiseTimeFilterBinningCountA(int value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_riseTimeFilter_binningA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setRiseTimeFilterBinningOfA(value);

    resetRiseTimePlotA(source);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRiseTimeFilterBinningCountB(int value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_riseTimeFilter_binningB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setRiseTimeFilterBinningOfB(value);

    resetRiseTimePlotB(source);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRiseTimeFilterLeftWindowA(int value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_riseTimerFilterLeftA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setRiseTimeFilterLeftWindowOfA(value);

    updateRiseTimeFilterALimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRiseTimeFilterLeftWindowB(int value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_riseTimerFilterLeftB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setRiseTimeFilterLeftWindowOfB(value);

    updateRiseTimeFilterBLimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRiseTimeFilterRightWindowA(int value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_riseTimeFilterRightA->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setRiseTimeFilterRightWindowOfA(value);

    updateRiseTimeFilterALimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeRiseTimeFilterRightWindowB(int value, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_riseTimeFilterRightB->setValue(value);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setRiseTimeFilterRightWindowOfB(value);

    updateRiseTimeFilterBLimits();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetRiseTimePlotA(const FunctionSource &source)
{
    if (!m_worker)
        return;

    QMutexLocker locker(&m_mutex);

    DUNUSED_PARAM(source);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->resetRiseTimeFilterA();

    ui->widget_plotrRiseTimeFilterA->curve().at(0)->clearCurveContent();
    ui->widget_plotrRiseTimeFilterA->replot();

    updateRiseTimeFilterALimits();

    ui->widget_plotrRiseTimeFilterA->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfA());

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::resetRiseTimePlotB(const FunctionSource &source)
{
    if (!m_worker)
        return;

    QMutexLocker locker(&m_mutex);

    DUNUSED_PARAM(source);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    m_worker->resetRiseTimeFilterB();

    ui->widget_plotrRiseTimeFilterB->curve().at(0)->clearCurveContent();
    ui->widget_plotrRiseTimeFilterB->replot();

    updateRiseTimeFilterBLimits();

    ui->widget_plotrRiseTimeFilterB->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfB());

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeMedianFilterAEnabled(bool on, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_medianFilterAActivate->clicked(on);
        ui->checkBox_medianFilterAActivate->setChecked(on);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setMedianFilterAEnabled(on);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeMedianFilterBEnabled(bool on, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_medianFilterBActivate->clicked(on);
        ui->checkBox_medianFilterBActivate->setChecked(on);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setMedianFilterBEnabled(on);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeMedianFilterWindowSizeA(int size, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_medianFilterWindowSizeA->setValue(size);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setMedianFilterWindowSizeA(size);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeMedianFilterWindowSizeB(int size, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_medianFilterWindowSizeB->setValue(size);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setMedianFilterWindowSizeB(size);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeTriggerLevelA(int levelMilliVolt, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->verticalSlider_triggerLevelA->setValue(levelMilliVolt);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4BoardManager::sharedInstance()->currentBoard()->SetIndividualTriggerLevel(DRS4SettingsManager::sharedInstance()->channelNumberA(), ((double)levelMilliVolt/1000.0f));

    DRS4SettingsManager::sharedInstance()->setTriggerLevelAmV(levelMilliVolt);

    ui->lineEdit_triggerA->setText(QString::number(levelMilliVolt, 'f', 0));

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4PulseGenerator::sharedInstance()->update();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeTriggerLevelA2()
{
    ui->verticalSlider_triggerLevelA->setValue(QVariant(ui->lineEdit_triggerA->text()).toInt());
}

void DRS4ScopeDlg::changeTriggerLevelB(int levelMilliVolt, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->verticalSlider_triggerLevelB->setValue(levelMilliVolt);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4BoardManager::sharedInstance()->currentBoard()->SetIndividualTriggerLevel(DRS4SettingsManager::sharedInstance()->channelNumberB(), ((double)levelMilliVolt/1000.0f));

    DRS4SettingsManager::sharedInstance()->setTriggerLevelBmV(levelMilliVolt);

    ui->lineEdit_triggerB->setText(QString::number(levelMilliVolt, 'f', 0));

    m_worker->setBusy(false);
}

int DRS4ScopeDlg::triggerLevelAInMillivolt() const
{
    QMutexLocker locker(&m_mutex);

    return ui->verticalSlider_triggerLevelA->value();
}

int DRS4ScopeDlg::triggerLevelBInMillivolt() const
{
    QMutexLocker locker(&m_mutex);

    return ui->verticalSlider_triggerLevelB->value();
}

int DRS4ScopeDlg::triggerDelayInNanoseconds() const
{
    QMutexLocker locker(&m_mutex);

    return ui->horizontalSlider_triggerDelay->value();
}

void DRS4ScopeDlg::changeTriggerLevelB2()
{
    ui->verticalSlider_triggerLevelB->setValue(QVariant(ui->lineEdit_triggerB->text()).toInt());
}

void DRS4ScopeDlg::changeCFDLevelA(double cfd, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_cfdA->setValue(cfd);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setCFDLevelA(cfd/100.0f);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeCFDLevelB(double cfd, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_cfdB->setValue(cfd);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setCFDLevelB(cfd/100.0f);

    m_worker->setBusy(false);
}

double DRS4ScopeDlg::CFDLevelA() const
{
    QMutexLocker locker(&m_mutex);

    return ui->doubleSpinBox_cfdA->value();
}

double DRS4ScopeDlg::CFDLevelB() const
{
    QMutexLocker locker(&m_mutex);

    return ui->doubleSpinBox_cfdB->value();
}

void DRS4ScopeDlg::changeIgnoreBusyState(bool ignore)
{
    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setIgnoreBusyState(ignore);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeTriggerDelay(int delayInNs, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->horizontalSlider_triggerDelay->setValue(delayInNs);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4BoardManager::sharedInstance()->currentBoard()->SetTriggerDelayNs(delayInNs);

    DRS4SettingsManager::sharedInstance()->setTriggerDelayInNs(delayInNs);

    ui->label_triggerDelay->setNum(delayInNs);

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4PulseGenerator::sharedInstance()->update();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeChannelSettingsAB(double sett)
{
    DUNUSED_PARAM(sett);

    DRS4SettingsManager::sharedInstance()->setChannelCntAB(ui->spinBox_chnCountAB->value());
    DRS4SettingsManager::sharedInstance()->setOffsetInNSAB(ui->doubleSpinBox_offsetAB->value());
    DRS4SettingsManager::sharedInstance()->setScalerInNSAB(ui->doubleSpinBox_scalerAB->value());

    const QString res = QString::number(1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSAB()/(double)DRS4SettingsManager::sharedInstance()->channelCntAB(), 'f', 4);
    ui->label_channelResAB->setText(res + " [ps/#]");

    initABLTSpectrum();
    resetLTSpectrumAB();
}

void DRS4ScopeDlg::changeChannelSettingsAB2(int sett)
{
    changeChannelSettingsAB((double)sett);
}

void DRS4ScopeDlg::changeChannelSettingsBA(double sett)
{
    DUNUSED_PARAM(sett);

    DRS4SettingsManager::sharedInstance()->setChannelCntBA(ui->spinBox_chnCountBA->value());
    DRS4SettingsManager::sharedInstance()->setOffsetInNSBA(ui->doubleSpinBox_offsetBA->value());
    DRS4SettingsManager::sharedInstance()->setScalerInNSBA(ui->doubleSpinBox_scalerBA->value());

    const QString res = QString::number(1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSBA()/(double)DRS4SettingsManager::sharedInstance()->channelCntBA(), 'f', 4);
    ui->label_channelResolutionBA->setText(res + " [ps/#]");

    initBALTSpectrum();
    resetLTSpectrumBA();
}

void DRS4ScopeDlg::changeChannelSettingsBA2(int sett)
{
    changeChannelSettingsBA((double)sett);
}

void DRS4ScopeDlg::changeChannelSettingsCoincidence(double sett)
{
    DUNUSED_PARAM(sett);

    DRS4SettingsManager::sharedInstance()->setChannelCntCoincindence(ui->spinBox_chnCountCoincidence->value());
    DRS4SettingsManager::sharedInstance()->setOffsetInNSCoincidence(ui->doubleSpinBox_offsetCoincidence->value());
    DRS4SettingsManager::sharedInstance()->setScalerInNSCoincidence(ui->doubleSpinBox_scalerCoincidence->value());

    const QString res = QString::number(1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence()/(double)DRS4SettingsManager::sharedInstance()->channelCntCoincindence(), 'f', 4);
    ui->label_channelResolutionCoincidence->setText(res + " [ps/#]");

    initCoincidenceLTSpectrum();
    resetLTSpectrumCoincidence();
}

void DRS4ScopeDlg::changeChannelSettingsCoincidence2(int sett)
{
    changeChannelSettingsCoincidence((double)sett);
}

void DRS4ScopeDlg::changeChannelSettingsMerged(double sett)
{
    DUNUSED_PARAM(sett);

    DRS4SettingsManager::sharedInstance()->setChannelCntMerged(ui->spinBox_chnCountMerged->value());
    DRS4SettingsManager::sharedInstance()->setOffsetInNSMerged(ui->doubleSpinBox_offsetMerged->value());
    DRS4SettingsManager::sharedInstance()->setScalerInNSMerged(ui->doubleSpinBox_scalerMerged->value());

    const QString res = QString::number(1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSMerged()/(double)DRS4SettingsManager::sharedInstance()->channelCntMerged(), 'f', 4);
    ui->label_channelResMerged->setText(res + " [ps/#]");

    initMergedLTSpectrum();
    resetLTSpectrumMerged();
}

void DRS4ScopeDlg::changeChannelSettingsMerged2(int sett)
{
    changeChannelSettingsMerged((double)sett);
}

void DRS4ScopeDlg::changeTriggerLogic(int index)
{
    DRS4SettingsManager::sharedInstance()->setTriggerSource(index);

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4BoardManager::sharedInstance()->currentBoard()->SetTriggerSource(DRS4SettingsManager::sharedInstance()->triggerSource());

    m_worker->setBusy(false);
}

int DRS4ScopeDlg::getIndexForSweep(int sweep)
{
    int index = 0;

    switch ( sweep )
    {
    case 200: //5GHz
        index = 0;
        break;

    case 500: //2GHz
        index = 1;
        break;

    case 1000: //1GHz
        index = 2;
        break;

    case 2000: //0.5GHz
        index  = 3;
        break;

    default:
        break;
    }

    return index;
}

void DRS4ScopeDlg::addSampleSpeedWarningMessage(bool on, bool accessFromScript)
{
    if (!accessFromScript) {
        if (on) {
            ui->label_SampleSpeed->setText("Sample-Speed: [Note: Do not change this value until you finished the Data-Stream]");
            ui->label_SampleSpeed->setStyleSheet("color: red");
        }
        else {
            ui->label_SampleSpeed->setText("Sample-Speed:");
            ui->label_SampleSpeed->setStyleSheet("color: black");
        }
    }
    else {
        emit signalAddSampleSpeedWarningMessage(on, !accessFromScript);
    }
}

void DRS4ScopeDlg::changeSampleSpeed(int index, bool accessFromScript)
{
    if (accessFromScript) {
        emit signalChangeSampleSpeed(index, !accessFromScript);
        return;
    }

    const int oldValue = ui->horizontalSlider_triggerDelay->value();
    const double oldSweep = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();
    const double ratio = (double)oldValue/oldSweep;

    switch ( index )
    {
    case 0: //5GHz
        DRS4SettingsManager::sharedInstance()->setSweepInNanoseconds(200.0f);
        DRS4SettingsManager::sharedInstance()->setSampleSpeedInGHz(5.12f);
        break;

    case 1: //2GHz
        DRS4SettingsManager::sharedInstance()->setSweepInNanoseconds(500.0f);
        DRS4SettingsManager::sharedInstance()->setSampleSpeedInGHz(2.0f);
        break;

    case 2: //1GHz
        DRS4SettingsManager::sharedInstance()->setSweepInNanoseconds(1000.0f);
        DRS4SettingsManager::sharedInstance()->setSampleSpeedInGHz(1.0f);
        break;

    case 3: //0.5GHz
        DRS4SettingsManager::sharedInstance()->setSweepInNanoseconds(2000.0f);
        DRS4SettingsManager::sharedInstance()->setSampleSpeedInGHz(0.5f);
        break;

    default:
        break;
    }

    disconnect(ui->comboBox_sampleSpeed, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSampleSpeed(int)));
    ui->comboBox_sampleSpeed->setCurrentIndex(index);
    connect(ui->comboBox_sampleSpeed, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSampleSpeed(int)));

    //plot-scopes:
    ui->widget_plotA->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());
    ui->widget_plotB->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());

    ui->widget_plotA->updatePlotView();
    ui->widget_plotB->updatePlotView();

    ui->horizontalSlider_triggerDelay->setRange(0, DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());
    ui->horizontalSlider_triggerDelay->setValue(ratio*DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        DRS4PulseGenerator::sharedInstance()->update();

        m_worker->setBusy(false);

        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4BoardManager::sharedInstance()->currentBoard()->SetFrequency(DRS4SettingsManager::sharedInstance()->sampleSpeedInGHz(), true);

    m_worker->setBusy(false);

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ) {
        const bool validTimingCalib = DRS4BoardManager::sharedInstance()->currentBoard()->IsTimingCalibrationValid();

        if (!validTimingCalib) {
            ui->label_timingCalibrated->setText("missing calibration");
        }
        else
            ui->label_timingCalibrated->setText("");
    }
}

void DRS4ScopeDlg::updatePulseAreaFilterALimits()
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    double x1_lower = 0;
    double y1_lower = DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitLowerLeftA();

    double x2_lower = kNumberOfBins-1;
    double y2_lower = DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitLowerRightA();

    double x1_upper = 0;
    double y1_upper = DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitUpperLeftA();

    double x2_upper = kNumberOfBins-1;
    double y2_upper = DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitUpperRightA();

    double slope_lower = (y2_lower-y1_lower)/(x2_lower-x1_lower);
    double intercept_lower = y1_lower-slope_lower*x1_lower;

    double slope_upper = (y2_upper-y1_upper)/(x2_upper-x1_upper);
    double intercept_upper = y1_upper-slope_upper*x1_upper;

    m_areaFilterASlopeLower = slope_lower;
    m_areaFilterAInterceptLower = intercept_lower;

    m_areaFilterASlopeUpper = slope_upper;
    m_areaFilterAInterceptUpper = intercept_upper;

    if ( y1_lower < 0 ) {
        y1_lower = 0;
        x1_lower = -intercept_lower/slope_lower;
    }

    if ( y1_lower > DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA() ) {
        y1_lower = DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA();
        x1_lower = ((double)(DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA())-intercept_lower)/slope_lower;
    }

    if ( y2_lower < 0 ) {
        y2_lower = 0;
        x2_lower = -intercept_lower/slope_lower;
    }

    if ( y2_lower > DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA() ) {
        y2_lower = DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA();
        x2_lower = ((double)(DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA())-intercept_lower)/slope_lower;
    }


    if ( y1_upper < 0 ) {
        y1_upper = 0;
        x1_upper = -intercept_upper/slope_upper;
    }

    if ( y1_upper > DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA() ) {
        y1_upper= DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA();
        x1_upper = ((double)(DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA())-intercept_upper)/slope_upper;
    }

    if ( y2_upper < 0 ) {
        y2_upper = 0;
        x2_upper = -intercept_upper/slope_upper;
    }

    if ( y2_upper > DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA() ) {
        y2_upper= DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA();
        x2_upper = ((double)(DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA())-intercept_upper)/slope_upper;
    }

    QVector<QPointF> limitsUpper;
    limitsUpper.append(QPointF(x1_upper, y1_upper));
    limitsUpper.append(QPointF(x2_upper, y2_upper));

    QVector<QPointF> limitsLower;
    limitsLower.append(QPointF(x1_lower, y1_lower));
    limitsLower.append(QPointF(x2_lower, y2_lower));

    QVector<QPointF> phsStartMin;
    phsStartMin.append(QPointF(DRS4SettingsManager::sharedInstance()->startChanneAMin(), 0));
    phsStartMin.append(QPointF(DRS4SettingsManager::sharedInstance()->startChanneAMin(), DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA()));

    QVector<QPointF> phsStartMax;
    phsStartMax.append(QPointF(DRS4SettingsManager::sharedInstance()->startChanneAMax(), 0));
    phsStartMax.append(QPointF(DRS4SettingsManager::sharedInstance()->startChanneAMax(), DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA()));

    QVector<QPointF> phsStopMin;
    phsStopMin.append(QPointF(DRS4SettingsManager::sharedInstance()->stopChanneAMin(), 0));
    phsStopMin.append(QPointF(DRS4SettingsManager::sharedInstance()->stopChanneAMin(), DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA()));

    QVector<QPointF> phsStopMax;
    phsStopMax.append(QPointF(DRS4SettingsManager::sharedInstance()->stopChanneAMax(), 0));
    phsStopMax.append(QPointF(DRS4SettingsManager::sharedInstance()->stopChanneAMax(), DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA()));

    ui->widget_plotAreaFilterA->curve().at(1)->clearCurveContent();
    ui->widget_plotAreaFilterA->curve().at(1)->addData(limitsUpper);

    ui->widget_plotAreaFilterA->curve().at(2)->clearCurveContent();
    ui->widget_plotAreaFilterA->curve().at(2)->addData(limitsLower);

    ui->widget_plotAreaFilterA->curve().at(3)->clearCurveContent();
    ui->widget_plotAreaFilterA->curve().at(3)->addData(phsStartMin);

    ui->widget_plotAreaFilterA->curve().at(4)->clearCurveContent();
    ui->widget_plotAreaFilterA->curve().at(4)->addData(phsStartMax);

    ui->widget_plotAreaFilterA->curve().at(5)->clearCurveContent();
    ui->widget_plotAreaFilterA->curve().at(5)->addData(phsStopMin);

    ui->widget_plotAreaFilterA->curve().at(6)->clearCurveContent();
    ui->widget_plotAreaFilterA->curve().at(6)->addData(phsStopMax);

    ui->widget_plotAreaFilterA->replot();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::updatePulseAreaFilterBLimits()
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    double x1_lower = 0;
    double y1_lower = DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitLowerLeftB();

    double x2_lower = kNumberOfBins-1;
    double y2_lower = DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitLowerRightB();

    double x1_upper = 0;
    double y1_upper = DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitUpperLeftB();

    double x2_upper = kNumberOfBins-1;
    double y2_upper = DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitUpperRightB();

    double slope_lower = (y2_lower-y1_lower)/(x2_lower-x1_lower);
    double intercept_lower = y1_lower-slope_lower*x1_lower;

    double slope_upper = (y2_upper-y1_upper)/(x2_upper-x1_upper);
    double intercept_upper = y1_upper-slope_upper*x1_upper;

    m_areaFilterBSlopeLower = slope_lower;
    m_areaFilterBInterceptLower = intercept_lower;

    m_areaFilterBSlopeUpper = slope_upper;
    m_areaFilterBInterceptUpper = intercept_upper;

    if ( y1_lower < 0 ) {
        y1_lower = 0;
        x1_lower = -intercept_lower/slope_lower;
    }

    if ( y1_lower > DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB() ) {
        y1_lower = DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB();
        x1_lower = ((double)(DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB())-intercept_lower)/slope_lower;
    }

    if ( y2_lower < 0 ) {
        y2_lower = 0;
        x2_lower = -intercept_lower/slope_lower;
    }

    if ( y2_lower > DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB() ) {
        y2_lower = DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB();
        x2_lower = ((double)(DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB())-intercept_lower)/slope_lower;
    }

    if ( y1_upper < 0 ) {
        y1_upper = 0;
        x1_upper = -intercept_upper/slope_upper;
    }

    if ( y1_upper > DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB() ) {
        y1_upper= DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB();
        x1_upper = ((double)(DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB())-intercept_upper)/slope_upper;
    }

    if ( y2_upper < 0 ) {
        y2_upper = 0;
        x2_upper = -intercept_upper/slope_upper;
    }

    if ( y2_upper > DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB() ) {
        y2_upper= DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB();
        x2_upper = ((double)(DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB())-intercept_upper)/slope_upper;
    }

    QVector<QPointF> limitsUpper;
    limitsUpper.append(QPointF(x1_upper, y1_upper));
    limitsUpper.append(QPointF(x2_upper, y2_upper));

    QVector<QPointF> limitsLower;
    limitsLower.append(QPointF(x1_lower, y1_lower));
    limitsLower.append(QPointF(x2_lower, y2_lower));

    ui->widget_plotAreaFilterB_2->curve().at(1)->clearCurveContent();
    ui->widget_plotAreaFilterB_2->curve().at(1)->addData(limitsUpper);

    ui->widget_plotAreaFilterB_2->curve().at(2)->clearCurveContent();
    ui->widget_plotAreaFilterB_2->curve().at(2)->addData(limitsLower);

    QVector<QPointF> phsStartMin;
    phsStartMin.append(QPointF(DRS4SettingsManager::sharedInstance()->startChanneBMin(), 0));
    phsStartMin.append(QPointF(DRS4SettingsManager::sharedInstance()->startChanneBMin(), DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB()));

    QVector<QPointF> phsStartMax;
    phsStartMax.append(QPointF(DRS4SettingsManager::sharedInstance()->startChanneBMax(), 0));
    phsStartMax.append(QPointF(DRS4SettingsManager::sharedInstance()->startChanneBMax(), DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB()));

    QVector<QPointF> phsStopMin;
    phsStopMin.append(QPointF(DRS4SettingsManager::sharedInstance()->stopChanneBMin(), 0));
    phsStopMin.append(QPointF(DRS4SettingsManager::sharedInstance()->stopChanneBMin(), DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB()));

    QVector<QPointF> phsStopMax;
    phsStopMax.append(QPointF(DRS4SettingsManager::sharedInstance()->stopChanneBMax(), 0));
    phsStopMax.append(QPointF(DRS4SettingsManager::sharedInstance()->stopChanneBMax(), DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB()));

    ui->widget_plotAreaFilterB_2->curve().at(3)->clearCurveContent();
    ui->widget_plotAreaFilterB_2->curve().at(3)->addData(phsStartMin);

    ui->widget_plotAreaFilterB_2->curve().at(4)->clearCurveContent();
    ui->widget_plotAreaFilterB_2->curve().at(4)->addData(phsStartMax);

    ui->widget_plotAreaFilterB_2->curve().at(5)->clearCurveContent();
    ui->widget_plotAreaFilterB_2->curve().at(5)->addData(phsStopMin);

    ui->widget_plotAreaFilterB_2->curve().at(6)->clearCurveContent();
    ui->widget_plotAreaFilterB_2->curve().at(6)->addData(phsStopMax);

    ui->widget_plotAreaFilterB_2->replot();

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::updateRiseTimeFilterALimits()
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    int yMax = m_worker->riseTimeFilterADataMax();

    const QPointF leftWindow_1(DRS4SettingsManager::sharedInstance()->riseTimeFilterLeftWindowOfA(), 1);
    const QPointF leftWindow_2(DRS4SettingsManager::sharedInstance()->riseTimeFilterLeftWindowOfA(), yMax<=/*999?999*/10?10:yMax-1);

    QVector<QPointF> leftLimits;
    leftLimits.append(leftWindow_1);
    leftLimits.append(leftWindow_2);
    leftLimits.append(leftWindow_1);

    const QPointF rightWindow_1(DRS4SettingsManager::sharedInstance()->riseTimeFilterRightWindowOfA(), 1);
    const QPointF rightWindow_2(DRS4SettingsManager::sharedInstance()->riseTimeFilterRightWindowOfA(), yMax<=/*999?999*/10?10:yMax-1);

    QVector<QPointF> rightLimits;
    rightLimits.append(rightWindow_1);
    rightLimits.append(rightWindow_2);
    rightLimits.append(rightWindow_1);

    ui->widget_plotrRiseTimeFilterA->curve().at(1)->clearCurveContent();
    ui->widget_plotrRiseTimeFilterA->curve().at(1)->addData(leftLimits);

    ui->widget_plotrRiseTimeFilterA->curve().at(2)->clearCurveContent();
    ui->widget_plotrRiseTimeFilterA->curve().at(2)->addData(rightLimits);

    ui->widget_plotrRiseTimeFilterA->replot();

    ui->spinBox_riseTimerFilterLeftA->setRange(0, DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfA());
    ui->spinBox_riseTimeFilterRightA->setRange(0, DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfA());

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::updateRiseTimeFilterBLimits()
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const int yMax = m_worker->riseTimeFilterBDataMax();

    const QPointF leftWindow_1(DRS4SettingsManager::sharedInstance()->riseTimeFilterLeftWindowOfB(), 1);
    const QPointF leftWindow_2(DRS4SettingsManager::sharedInstance()->riseTimeFilterLeftWindowOfB(), yMax<=/*999?999*/10?10:yMax-1);

    QVector<QPointF> leftLimits;
    leftLimits.append(leftWindow_1);
    leftLimits.append(leftWindow_2);
    leftLimits.append(leftWindow_1);

    const QPointF rightWindow_1(DRS4SettingsManager::sharedInstance()->riseTimeFilterRightWindowOfB(), 1);
    const QPointF rightWindow_2(DRS4SettingsManager::sharedInstance()->riseTimeFilterRightWindowOfB(), yMax<=/*999?999*/10?10:yMax-1);

    QVector<QPointF> rightLimits;
    rightLimits.append(rightWindow_1);
    rightLimits.append(rightWindow_2);
    rightLimits.append(rightWindow_1);

    ui->widget_plotrRiseTimeFilterB->curve().at(1)->clearCurveContent();
    ui->widget_plotrRiseTimeFilterB->curve().at(1)->addData(leftLimits);

    ui->widget_plotrRiseTimeFilterB->curve().at(2)->clearCurveContent();
    ui->widget_plotrRiseTimeFilterB->curve().at(2)->addData(rightLimits);

    ui->widget_plotrRiseTimeFilterB->replot();

    ui->spinBox_riseTimerFilterLeftB->setRange(0, DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfB());
    ui->spinBox_riseTimeFilterRightB->setRange(0, DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfB());

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeMeanCableDelayPs(double ps, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->doubleSpinBox_meanCoincidence->setValue(ps);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setMeanCableDelayInNs(ps/1000.0f);

    m_worker->setBusy(false);
}

double DRS4ScopeDlg::meanCableDelayInPicoseconds() const
{
    QMutexLocker locker(&m_mutex);

    return ui->doubleSpinBox_meanCoincidence->value();
}

void DRS4ScopeDlg::changeCoincidenceFitIterations(int iterations, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_fitIterations->setValue(iterations);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setFitIterations(iterations);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeMergedFitIterations(int iterations, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_fitIterations_Merged->setValue(iterations);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setFitIterationsMerged(iterations);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeABFitIterations(int iterations, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_fitIterations_AB->setValue(iterations);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setFitIterationsAB(iterations);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBAFitIterations(int iterations, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        ui->spinBox_fitIterations_BA->setValue(iterations);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setFitIterationsBA(iterations);

    m_worker->setBusy(false);
}

int DRS4ScopeDlg::coincidenceFitIterations() const
{
    QMutexLocker locker(&m_mutex);

    return ui->spinBox_fitIterations->value();
}

int DRS4ScopeDlg::ABFitIterations() const
{
    QMutexLocker locker(&m_mutex);

    return ui->spinBox_fitIterations_AB->value();
}

int DRS4ScopeDlg::BAFitIterations() const
{
    QMutexLocker locker(&m_mutex);

    return ui->spinBox_fitIterations_BA->value();
}

int DRS4ScopeDlg::mergedFitIterations() const
{
    QMutexLocker locker(&m_mutex);

    return ui->spinBox_fitIterations_Merged->value();
}

void DRS4ScopeDlg::saveABSpectrum(bool autosave, const QString &fileNameAutosave)
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    QString fileName = "";

    if ( !autosave ) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                              DRS4ProgramSettingsManager::sharedInstance()->saveDataFilePath(),
                                                              tr("Data (*.dat *.txt *.log)"));

        if ( fileName.isEmpty() ) {
            m_worker->setBusy(false);
            return;
        }

        DRS4ProgramSettingsManager::sharedInstance()->setSaveDataFilePath(fileName);
    }
    else
        fileName = fileNameAutosave;

    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const QString res = QString::number(1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSAB()/(double)DRS4SettingsManager::sharedInstance()->channelCntAB(), 'f', 4);

        stream << "#" << "Lifetime: [Channel2 - Channel1]\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Channel-Resolution: " << res << "ps\n";
        stream << "# Total Counts: " << QString::number(QVariant(m_worker->countsSpectrumAB()).toDouble(), 'f', 0) << "[#]\n";
        stream << "channel\tcounts\n";

        for ( int i = 0 ; i < DRS4SettingsManager::sharedInstance()->channelCntAB() ; ++ i ) {
            stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->spectrumAB()->at(i)).toString() << "\n";
        }

        file.close();
        m_worker->setBusy(false);
    }
    else {
        if ( !autosave ) {
            MSGBOX("Error while writing file!");
        }

        m_worker->setBusy(false);
    }
}

void DRS4ScopeDlg::saveABSpectrumDQuickLTFit(bool autosave, const QString &fileNameAutosave)
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    QString fileName = "";

    if ( !autosave ) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                              DRS4ProgramSettingsManager::sharedInstance()->saveDataFilePath(),
                                                              tr("DQuickLTFit Project File (*.dquicklt)"));

        if ( fileName.isEmpty() ) {
            m_worker->setBusy(false);
            return;
        }

        DRS4ProgramSettingsManager::sharedInstance()->setSaveDataFilePath(fileName);
    }
    else
        fileName = fileNameAutosave;

    PALSProjectManager::sharedInstance()->createEmptyProject();


    QList<QPointF> data;
    double bkgrd = 0.0;
    const int range = DRS4SettingsManager::sharedInstance()->channelCntAB()*0.05;
    int index_where_ymax = 0;
    int index_where_fwhm = 0;
    double max_y = 0.0;

    for ( int i = 0 ; i < DRS4SettingsManager::sharedInstance()->channelCntAB() ; ++ i ) {
        data.append(QPointF(i, m_worker->spectrumAB()->at(i)));

        if ( m_worker->spectrumAB()->at(i) > max_y) {
            index_where_ymax = i;
            max_y = m_worker->spectrumAB()->at(i);
        }

        if (i >= DRS4SettingsManager::sharedInstance()->channelCntAB() - range)
            bkgrd += m_worker->spectrumAB()->at(i);
    }

    bkgrd /= range;

    for ( int i = 0 ; i < index_where_ymax ; ++ i ) {
        if ( m_worker->spectrumAB()->at(i) > 0.5*max_y ) {
            index_where_fwhm = i;
            break;
        }
    }

    const double res = 1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSAB()/(double)DRS4SettingsManager::sharedInstance()->channelCntAB();

    const double t0 = index_where_ymax*res;
    const double fwhm = abs(index_where_ymax - index_where_fwhm)*res;

    PALSProjectManager::sharedInstance()->getDataStructure()->getDataSetPtr()->setLifeTimeData(data);
    PALSProjectManager::sharedInstance()->getDataStructure()->getDataSetPtr()->setBinFactor(1);
    PALSProjectManager::sharedInstance()->getDataStructure()->getDataSetPtr()->setLifeTimeRawData(data);
    PALSProjectManager::sharedInstance()->setChannelRanges(0, DRS4SettingsManager::sharedInstance()->channelCntAB()-1);
    PALSProjectManager::sharedInstance()->setASCIIDataName("data imported from DDRS4PALS software");
    PALSProjectManager::sharedInstance()->setFileName(fileName);
    PALSProjectManager::sharedInstance()->getDataStructure()->setName(fileName);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setChannelResolution(res);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setMaximumIterations(200);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setStartChannel(0);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setStopChannel(DRS4SettingsManager::sharedInstance()->channelCntAB()-1);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setCountsInRange(m_worker->countsSpectrumAB());

    // estimated t0 and fwhm value
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getDeviceResolutionParamPtr()->getParameterAt(1)->setStartValue(t0);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getDeviceResolutionParamPtr()->getParameterAt(0)->setStartValue(fwhm);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getDeviceResolutionParamPtr()->getParameterAt(2)->setAsFixed(true);

    // estimated bkgrd value
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getBackgroundParamPtr()->getParameter()->setStartValue(bkgrd);

    if (PALSProjectManager::sharedInstance()->save(fileName)) {
        /* ok */
    }
    else {
        if ( !autosave ) {
            m_worker->setBusy(false);
            MSGBOX("Error while writing file!");
        }
    }

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::saveBASpectrum(bool autosave, const QString &fileNameAutosave)
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    QString fileName = "";

    if ( !autosave ) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                DRS4ProgramSettingsManager::sharedInstance()->saveDataFilePath(),
                                                tr("Data (*.dat *.txt *.log)"));

        if ( fileName.isEmpty() ) {
            m_worker->setBusy(false);
            return;
        }

        DRS4ProgramSettingsManager::sharedInstance()->setSaveDataFilePath(fileName);
    }
    else
        fileName = fileNameAutosave;

    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const QString res = QString::number(1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSBA()/(double)DRS4SettingsManager::sharedInstance()->channelCntBA(), 'f', 4);

        stream << "#" << "Lifetime: [Channel1 - Channel2]\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Channel-Resolution: " << res << "ps\n";
        stream << "# Total Counts: " << QString::number(QVariant(m_worker->countsSpectrumBA()).toDouble(), 'f', 0) << "[#]\n";
        stream << "channel\tcounts\n";

        for ( int i = 0 ; i < DRS4SettingsManager::sharedInstance()->channelCntBA() ; ++ i ) {
            stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->spectrumBA()->at(i)).toString() << "\n";
        }

        file.close();
        m_worker->setBusy(false);
    }
    else {
        if (!autosave) {
            MSGBOX("Error while writing file!");
        }

        m_worker->setBusy(false);
    }
}

void DRS4ScopeDlg::saveBASpectrumDQuickLTFit(bool autosave, const QString &fileNameAutosave)
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    QString fileName = "";

    if ( !autosave ) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                              DRS4ProgramSettingsManager::sharedInstance()->saveDataFilePath(),
                                                              tr("DQuickLTFit Project File (*.dquicklt)"));

        if ( fileName.isEmpty() ) {
            m_worker->setBusy(false);
            return;
        }

        DRS4ProgramSettingsManager::sharedInstance()->setSaveDataFilePath(fileName);
    }
    else
        fileName = fileNameAutosave;

    PALSProjectManager::sharedInstance()->createEmptyProject();


    QList<QPointF> data;
    double bkgrd = 0.0;
    const int range = DRS4SettingsManager::sharedInstance()->channelCntBA()*0.05;
    int index_where_ymax = 0;
    int index_where_fwhm = 0;
    double max_y = 0.0;

    for ( int i = 0 ; i < DRS4SettingsManager::sharedInstance()->channelCntBA() ; ++ i ) {
        data.append(QPointF(i, m_worker->spectrumBA()->at(i)));

        if ( m_worker->spectrumBA()->at(i) > max_y) {
            index_where_ymax = i;
            max_y = m_worker->spectrumBA()->at(i);
        }

        if (i >= DRS4SettingsManager::sharedInstance()->channelCntBA() - range)
            bkgrd += m_worker->spectrumBA()->at(i);
    }

    bkgrd /= range;

    for ( int i = 0 ; i < index_where_ymax ; ++ i ) {
        if ( m_worker->spectrumBA()->at(i) > 0.5*max_y ) {
            index_where_fwhm = i;
            break;
        }
    }

    const double res = 1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSBA()/(double)DRS4SettingsManager::sharedInstance()->channelCntBA();

    const double t0 = index_where_ymax*res;
    const double fwhm = abs(index_where_ymax - index_where_fwhm)*res;

    PALSProjectManager::sharedInstance()->getDataStructure()->getDataSetPtr()->setLifeTimeData(data);
    PALSProjectManager::sharedInstance()->getDataStructure()->getDataSetPtr()->setBinFactor(1);
    PALSProjectManager::sharedInstance()->getDataStructure()->getDataSetPtr()->setLifeTimeRawData(data);
    PALSProjectManager::sharedInstance()->setChannelRanges(0, DRS4SettingsManager::sharedInstance()->channelCntBA()-1);
    PALSProjectManager::sharedInstance()->setASCIIDataName("data imported from DDRS4PALS software");
    PALSProjectManager::sharedInstance()->setFileName(fileName);
    PALSProjectManager::sharedInstance()->getDataStructure()->setName(fileName);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setChannelResolution(res);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setMaximumIterations(200);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setStartChannel(0);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setStopChannel(DRS4SettingsManager::sharedInstance()->channelCntBA()-1);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setCountsInRange(m_worker->countsSpectrumBA());

    // estimated t0 and fwhm value
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getDeviceResolutionParamPtr()->getParameterAt(1)->setStartValue(t0);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getDeviceResolutionParamPtr()->getParameterAt(0)->setStartValue(fwhm);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getDeviceResolutionParamPtr()->getParameterAt(2)->setAsFixed(true);

    // estimated bkgrd value
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getBackgroundParamPtr()->getParameter()->setStartValue(bkgrd);

    if (PALSProjectManager::sharedInstance()->save(fileName)) {
        /* ok */
    }
    else {
        if ( !autosave ) {
            m_worker->setBusy(false);
            MSGBOX("Error while writing file!");
        }
    }

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::saveCoincidenceSpectrum(bool autosave, const QString &fileNameAutosave)
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    QString fileName = "";

    if (!autosave) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                DRS4ProgramSettingsManager::sharedInstance()->saveDataFilePath(),
                                                tr("Data (*.dat *.txt *.log)"));

        if ( fileName.isEmpty() ) {
            m_worker->setBusy(false);
            return;
        }

        DRS4ProgramSettingsManager::sharedInstance()->setSaveDataFilePath(fileName);
    }
    else
        fileName = fileNameAutosave;

    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const QString res = QString::number(1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence()/(double)DRS4SettingsManager::sharedInstance()->channelCntCoincindence(), 'f', 4);

        stream << "#" << "Lifetime: Prompt [Channel2 (Stop)- Channel1 (Stop)]\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Channel-Resolution: " << res << "ps\n";
        stream << "# Total Counts: " << QString::number(QVariant(m_worker->countsSpectrumCoincidence()).toDouble(), 'f', 0) << "[#]\n";
        stream << "channel\tcounts\n";

        for ( int i = 0 ; i < DRS4SettingsManager::sharedInstance()->channelCntCoincindence() ; ++ i ) {
            stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->spectrumCoincidence()->at(i)).toString() << "\n";
        }

        file.close();
        m_worker->setBusy(false);
    }
    else {
        if ( !autosave ) {
            MSGBOX("Error while writing file!");
        }

        m_worker->setBusy(false);
    }
}

void DRS4ScopeDlg::saveCoincidenceSpectrumDQuickLTFit(bool autosave, const QString &fileNameAutosave)
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    QString fileName = "";

    if ( !autosave ) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                              DRS4ProgramSettingsManager::sharedInstance()->saveDataFilePath(),
                                                              tr("DQuickLTFit Project File (*.dquicklt)"));

        if ( fileName.isEmpty() ) {
            m_worker->setBusy(false);
            return;
        }

        DRS4ProgramSettingsManager::sharedInstance()->setSaveDataFilePath(fileName);
    }
    else
        fileName = fileNameAutosave;

    PALSProjectManager::sharedInstance()->createEmptyProject();


    QList<QPointF> data;
    double bkgrd = 0.0;
    const int range = DRS4SettingsManager::sharedInstance()->channelCntCoincindence()*0.05;
    int index_where_ymax = 0;
    int index_where_fwhm = 0;
    double max_y = 0.0;

    for ( int i = 0 ; i < DRS4SettingsManager::sharedInstance()->channelCntCoincindence() ; ++ i ) {
        data.append(QPointF(i, m_worker->spectrumCoincidence()->at(i)));

        if ( m_worker->spectrumCoincidence()->at(i) > max_y) {
            index_where_ymax = i;
            max_y = m_worker->spectrumCoincidence()->at(i);
        }

        if (i >= DRS4SettingsManager::sharedInstance()->channelCntCoincindence() - range)
            bkgrd += m_worker->spectrumCoincidence()->at(i);
    }

    bkgrd /= range;

    for ( int i = 0 ; i < index_where_ymax ; ++ i ) {
        if ( m_worker->spectrumCoincidence()->at(i) > 0.5*max_y ) {
            index_where_fwhm = i;
            break;
        }
    }

    const double res = 1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence()/(double)DRS4SettingsManager::sharedInstance()->channelCntCoincindence();

    const double t0 = index_where_ymax*res;
    const double fwhm = abs(index_where_ymax - index_where_fwhm)*res;

    PALSProjectManager::sharedInstance()->getDataStructure()->getDataSetPtr()->setLifeTimeData(data);
    PALSProjectManager::sharedInstance()->getDataStructure()->getDataSetPtr()->setBinFactor(1);
    PALSProjectManager::sharedInstance()->getDataStructure()->getDataSetPtr()->setLifeTimeRawData(data);
    PALSProjectManager::sharedInstance()->setChannelRanges(0, DRS4SettingsManager::sharedInstance()->channelCntCoincindence()-1);
    PALSProjectManager::sharedInstance()->setASCIIDataName("data imported from DDRS4PALS software");
    PALSProjectManager::sharedInstance()->setFileName(fileName);
    PALSProjectManager::sharedInstance()->getDataStructure()->setName(fileName);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setChannelResolution(res);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setMaximumIterations(200);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setStartChannel(0);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setStopChannel(DRS4SettingsManager::sharedInstance()->channelCntCoincindence()-1);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setCountsInRange(m_worker->countsSpectrumCoincidence());

    // estimated t0 and fwhm value
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getDeviceResolutionParamPtr()->getParameterAt(1)->setStartValue(t0);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getDeviceResolutionParamPtr()->getParameterAt(0)->setStartValue(fwhm);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getDeviceResolutionParamPtr()->getParameterAt(2)->setAsFixed(true);

    // estimated bkgrd value
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getBackgroundParamPtr()->getParameter()->setStartValue(bkgrd);

    if (PALSProjectManager::sharedInstance()->save(fileName)) {
        /* ok */
    }
    else {
        if ( !autosave ) {
            m_worker->setBusy(false);
            MSGBOX("Error while writing file!");
        }
    }

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::saveMergedSpectrum(bool autosave, const QString &fileNameAutosave)
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    QString fileName = "";

    if ( !autosave ) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                DRS4ProgramSettingsManager::sharedInstance()->saveDataFilePath(),
                                                tr("Data (*.dat *.txt *.log)"));

        if ( fileName.isEmpty() ) {
            m_worker->setBusy(false);
            return;
        }

        DRS4ProgramSettingsManager::sharedInstance()->setSaveDataFilePath(fileName);
    }
    else {
        fileName = fileNameAutosave;
    }

    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const QString res = QString::number(1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSMerged()/(double)DRS4SettingsManager::sharedInstance()->channelCntMerged(), 'f', 4);

        stream << "#" << "Lifetime: [Merged]\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Channel-Resolution: " << res << "ps\n";
        stream << "# Total Counts: " << QString::number(QVariant(m_worker->countsSpectrumMerged()).toDouble(), 'f', 0) << "[#]\n";
        stream << "channel\tcounts\n";

        for ( int i = 0 ; i < DRS4SettingsManager::sharedInstance()->channelCntMerged() ; ++ i ) {
            stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->spectrumMerged()->at(i)).toString() << "\n";
        }

        file.close();
        m_worker->setBusy(false);
    }
    else {
        if ( !autosave ) {
            MSGBOX("Error while writing file!");
        }

        m_worker->setBusy(false);
    }
}

void DRS4ScopeDlg::saveMergedSpectrumDQuickLTFit(bool autosave, const QString &fileNameAutosave)
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    QString fileName = "";

    if ( !autosave ) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                              DRS4ProgramSettingsManager::sharedInstance()->saveDataFilePath(),
                                                              tr("DQuickLTFit Project File (*.dquicklt)"));

        if ( fileName.isEmpty() ) {
            m_worker->setBusy(false);
            return;
        }

        DRS4ProgramSettingsManager::sharedInstance()->setSaveDataFilePath(fileName);
    }
    else
        fileName = fileNameAutosave;

    PALSProjectManager::sharedInstance()->createEmptyProject();


    QList<QPointF> data;
    double bkgrd = 0.0;
    const int range = DRS4SettingsManager::sharedInstance()->channelCntMerged()*0.05;
    int index_where_ymax = 0;
    int index_where_fwhm = 0;
    double max_y = 0.0;

    for ( int i = 0 ; i < DRS4SettingsManager::sharedInstance()->channelCntMerged() ; ++ i ) {
        data.append(QPointF(i, m_worker->spectrumMerged()->at(i)));

        if ( m_worker->spectrumMerged()->at(i) > max_y) {
            index_where_ymax = i;
            max_y = m_worker->spectrumMerged()->at(i);
        }

        if (i >= DRS4SettingsManager::sharedInstance()->channelCntMerged() - range)
            bkgrd += m_worker->spectrumMerged()->at(i);
    }

    bkgrd /= range;

    for ( int i = 0 ; i < index_where_ymax ; ++ i ) {
        if ( m_worker->spectrumMerged()->at(i) > 0.5*max_y ) {
            index_where_fwhm = i;
            break;
        }
    }

    const double res = 1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSMerged()/(double)DRS4SettingsManager::sharedInstance()->channelCntMerged();

    const double t0 = index_where_ymax*res;
    const double fwhm = abs(index_where_ymax - index_where_fwhm)*res;

    PALSProjectManager::sharedInstance()->getDataStructure()->getDataSetPtr()->setLifeTimeData(data);
    PALSProjectManager::sharedInstance()->getDataStructure()->getDataSetPtr()->setBinFactor(1);
    PALSProjectManager::sharedInstance()->getDataStructure()->getDataSetPtr()->setLifeTimeRawData(data);
    PALSProjectManager::sharedInstance()->setChannelRanges(0, DRS4SettingsManager::sharedInstance()->channelCntMerged()-1);
    PALSProjectManager::sharedInstance()->setASCIIDataName("data imported from DDRS4PALS software");
    PALSProjectManager::sharedInstance()->setFileName(fileName);
    PALSProjectManager::sharedInstance()->getDataStructure()->setName(fileName);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setChannelResolution(res);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setMaximumIterations(200);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setStartChannel(0);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setStopChannel(DRS4SettingsManager::sharedInstance()->channelCntMerged()-1);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->setCountsInRange(m_worker->countsSpectrumMerged());

    // estimated t0 and fwhm value
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getDeviceResolutionParamPtr()->getParameterAt(1)->setStartValue(t0);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getDeviceResolutionParamPtr()->getParameterAt(0)->setStartValue(fwhm);
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getDeviceResolutionParamPtr()->getParameterAt(2)->setAsFixed(true);

    // estimated bkgrd value
    PALSProjectManager::sharedInstance()->getDataStructure()->getFitSetPtr()->getBackgroundParamPtr()->getParameter()->setStartValue(bkgrd);

    if (PALSProjectManager::sharedInstance()->save(fileName)) {
        /* ok */
    }
    else {
        if ( !autosave ) {
            m_worker->setBusy(false);
            MSGBOX("Error while writing file!");
        }
    }

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::savePHSA(bool autosave, const QString &fileNameAutosave)
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    QString fileName = "";

    if (!autosave) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                              DRS4ProgramSettingsManager::sharedInstance()->savePHSDataFilePath(),
                                                              tr("Data (*.dat *.txt *.log)"));

        if ( fileName.isEmpty() ) {
            m_worker->setBusy(false);
            return;
        }

        DRS4ProgramSettingsManager::sharedInstance()->setSavePHSDataFilePath(fileName);
    }
    else
        fileName = fileNameAutosave;

    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const QString res = QString::number(500.0f/((float)kNumberOfBins), 'f', 3);

        stream << "#" << "PHS - A\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Channel-Resolution: " << res << "mV\n";
        stream << "# Total Counts: " << QString::number((double)m_worker->phsACounts(), 'f', 0) << "[#]\n";
        stream << "channel\tcounts\n";

        for ( int i = 0 ; i < m_worker->phsA()->size() ; ++ i ) {
            stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->phsA()->at(i)).toString() << "\n";
        }

        file.close();
        m_worker->setBusy(false);
    }
    else {
        if (!autosave) {
            MSGBOX("Error while writing file!");
        }

        m_worker->setBusy(false);
    }
}

void DRS4ScopeDlg::savePHSB(bool autosave, const QString &fileNameAutosave)
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    QString fileName = "";

    if (!autosave) {
        fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                              DRS4ProgramSettingsManager::sharedInstance()->savePHSDataFilePath(),
                                                              tr("Data (*.dat *.txt *.log)"));

        if ( fileName.isEmpty() ) {
            m_worker->setBusy(false);
            return;
        }

        DRS4ProgramSettingsManager::sharedInstance()->setSavePHSDataFilePath(fileName);
    }
    else
        fileName = fileNameAutosave;

    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const QString res = QString::number(500.0f/((float)kNumberOfBins), 'f', 3);

        stream << "#" << "PHS - B\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Channel-Resolution: " << res << "mV\n";
        stream << "# Total Counts: " << QString::number((double)m_worker->phsBCounts(), 'f', 0) << "[#]\n";
        stream << "channel\tcounts\n";

        for ( int i = 0 ; i < m_worker->phsB()->size() ; ++ i ) {
            stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->phsB()->at(i)).toString() << "\n";
        }

        file.close();
        m_worker->setBusy(false);
    }
    else {
        if (!autosave) {
            MSGBOX("Error while writing file!");
        }

        m_worker->setBusy(false);
    }
}

void DRS4ScopeDlg::saveRiseTimeDistributionA()
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

     const QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                                     DRS4ProgramSettingsManager::sharedInstance()->saveRiseTimeDistributionDataFilePath(),
                                                                     tr("Data (*.dat *.txt *.log)"));

    if ( fileName.isEmpty() ) {
        m_worker->setBusy(false);
        return;
    }

    DRS4ProgramSettingsManager::sharedInstance()->setSaveRiseTimeDistributionDataFilePath(fileName);


    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const QString res = QString::number(DRS4SettingsManager::sharedInstance()->riseTimeFilterScaleInNanosecondsOfA()/DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfA(), 'f', 3);

        stream << "#" << "Rise-Time (10% >> 90% CF Level) Distribution - A\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Bin-Resolution [ns]: " << res << "\n";
        stream << "# Total Counts: " << QString::number((double)m_worker->m_riseTimeFilterACounter, 'f', 0) << "[#]\n";
        stream << "bin\tcounts\n";

        for ( int i = 0 ; i < m_worker->m_riseTimeFilterDataA.size() ; ++ i ) {
            if (m_worker->m_riseTimeFilterDataA.at(i) < 0.0) {
                stream << QVariant(i).toString() << "\t0" << "\n";
            }
            else {
                stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->m_riseTimeFilterDataA.at(i)).toString() << "\n";
            }
        }

        file.close();
        m_worker->setBusy(false);
    }
    else {
        MSGBOX("Error while writing file!");

        m_worker->setBusy(false);
    }
}

void DRS4ScopeDlg::saveRiseTimeDistributionB()
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                                     DRS4ProgramSettingsManager::sharedInstance()->saveRiseTimeDistributionDataFilePath(),
                                                                     tr("Data (*.dat *.txt *.log)"));

    if ( fileName.isEmpty() ) {
        m_worker->setBusy(false);
        return;
    }

    DRS4ProgramSettingsManager::sharedInstance()->setSaveRiseTimeDistributionDataFilePath(fileName);


    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const QString res = QString::number(DRS4SettingsManager::sharedInstance()->riseTimeFilterScaleInNanosecondsOfB()/DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfB(), 'f', 3);

        stream << "#" << "Rise-Time (10% >> 90% CF Level) Distribution - B\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "# Bin-Resolution [ns]: " << res << "\n";
        stream << "# Total Counts: " << QString::number((double)m_worker->m_riseTimeFilterBCounter, 'f', 0) << "[#]\n";
        stream << "bin\tcounts\n";

        for ( int i = 0 ; i < m_worker->m_riseTimeFilterDataB.size() ; ++ i ) {
            if (m_worker->m_riseTimeFilterDataB.at(i) < 0.0) {
                stream << QVariant(i).toString() << "\t0" << "\n";
            }
            else {
                stream << QVariant(i).toString() << "\t" <<  QVariant(m_worker->m_riseTimeFilterDataB.at(i)).toString() << "\n";
            }
        }

        file.close();
        m_worker->setBusy(false);
    }
    else {
        MSGBOX("Error while writing file!");

        m_worker->setBusy(false);
    }
}

void DRS4ScopeDlg::saveAreaDistributionA()
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

     const QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                                     DRS4ProgramSettingsManager::sharedInstance()->saveAreaDistributionDataFilePath(),
                                                                     tr("Data (*.dat *.txt *.log)"));

    if ( fileName.isEmpty() ) {
        m_worker->setBusy(false);
        return;
    }

    DRS4ProgramSettingsManager::sharedInstance()->setSaveAreaDistributionDataFilePath(fileName);


    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const double res = 500.0f/(float)kNumberOfBins;

        stream << "#" << "Area Distribution - A\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "amplitude [mV]\tarea mean [a.u.]\tarea stddev [a.u.]\n";

        for ( int i = 0 ; i < m_worker->m_areaFilterCollectedDataA.size() ; ++ i ) {
                stream << QString::number(i*res, 'f', 3) << "\t" <<  QString::number(m_worker->areaFilterACollectedData()->at(i).x(), 'g', 6) << "\t" <<  QString::number(m_worker->areaFilterACollectedData()->at(i).y(), 'g', 6)  << "\n";
        }

        file.close();
        m_worker->setBusy(false);
    }
    else {
        MSGBOX("Error while writing file!");

        m_worker->setBusy(false);
    }
}

void DRS4ScopeDlg::saveAreaDistributionB()
{
    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

     const QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                                     DRS4ProgramSettingsManager::sharedInstance()->saveAreaDistributionDataFilePath(),
                                                                     tr("Data (*.dat *.txt *.log)"));

    if ( fileName.isEmpty() ) {
        m_worker->setBusy(false);
        return;
    }

    DRS4ProgramSettingsManager::sharedInstance()->setSaveAreaDistributionDataFilePath(fileName);


    QFile file(fileName);
    QTextStream stream(&file);

    if ( file.open(QIODevice::WriteOnly) ) {
        const double res = 500.0f/(float)kNumberOfBins;

        stream << "#" << "Area Distribution - B\n";
        stream << "#" << QDateTime::currentDateTime().toString() << "\n";
        stream << "amplitude [mV]\tarea mean [a.u.]\tarea stddev [a.u.]\n";

        for ( int i = 0 ; i < m_worker->m_areaFilterCollectedDataB.size() ; ++ i ) {
                stream << QString::number(i*res, 'f', 3) << "\t" <<  QString::number(m_worker->areaFilterBCollectedData()->at(i).x(), 'g', 6) << "\t" <<  QString::number(m_worker->areaFilterBCollectedData()->at(i).y(), 'g', 6)  << "\n";
        }

        file.close();
        m_worker->setBusy(false);
    }
    else {
        MSGBOX("Error while writing file!");

        m_worker->setBusy(false);
    }
}

void DRS4ScopeDlg::changeNegativeLTAvailable(bool on)
{
    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setNegativeLifetimeAccepted(!on);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePositivTriggerPolarity(bool on, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_positiveTriggerPol->clicked(on);
        ui->checkBox_positiveTriggerPol->setChecked(on);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4BoardManager::sharedInstance()->currentBoard()->SetTriggerPolarity(!on);

    DRS4SettingsManager::sharedInstance()->setTriggerPolarityPositive(!on);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePositivSignal(bool on, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_positivSignal->clicked(on);
        ui->checkBox_positivSignal->setChecked(on);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPositivSignal(on);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseAreaFilterEnabled(bool on, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_activateAreaFilter->clicked(on);
        ui->checkBox_activateAreaFilter->setChecked(on);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterEnabled(on);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePulseAreaFilterPlotEnabled(bool on, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_enableAreaFilter->clicked(on);
        ui->checkBox_enableAreaFilter->setChecked(on);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPulseAreaFilterPlotEnabled(on);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeForceCoincidence(bool force)
{
    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setForceCoincidence(force);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeBurstModeEnabled(bool on, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_burstMode->clicked(on);
        ui->checkBox_burstMode->setChecked(on);
        return;
    }

    if ( on ) {
        if (DRS4TextFileStreamRangeManager::sharedInstance()->isArmed())
            DRS4TextFileStreamRangeManager::sharedInstance()->abort();

        if (DRS4TextFileStreamManager::sharedInstance()->isArmed())
            DRS4TextFileStreamManager::sharedInstance()->abort();

        if ( m_pulseSaveDlg->isVisible() ) {
            m_pulseSaveDlg->close();
            DDELETE_SAFETY(m_pulseSaveDlg);
            m_pulseSaveDlg = new DRS4PulseSaveDlg(m_worker);
        }

        if ( m_pulseSaveRangeDlg->isVisible() ) {
            m_pulseSaveRangeDlg->close();
            DDELETE_SAFETY(m_pulseSaveRangeDlg);
            m_pulseSaveRangeDlg = new DRS4PulseSaveRangeDlg(m_worker);
        }

        if (m_addInfoDlg->isVisible()) {
            m_addInfoDlg->close();
        }
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    if ( on ) {
        if ( m_worker->isRecordingForPulseShapeFilterA() ) {
            m_worker->stopRecordingForShapeFilterA();
        }

        if ( m_worker->isRecordingForPulseShapeFilterB() ) {
            m_worker->stopRecordingForShapeFilterB();
        }
    }

    for ( int i = 0 ; i < ui->tabWidget->count() ; ++ i ) {
        if ( !ui->tabWidget->widget(i) )
            continue;

        ui->tabWidget->widget(i)->setEnabled(!on);
    }

    ui->tab_14->setEnabled(!on); //Persistence A
    ui->tab_15->setEnabled(!on); //Persistence B

    ui->tab_22->setEnabled(!on); //Intrinsic Filters

    ui->widget_playPulseShapeFilterA->enableWidget(!on);

    if (on)
        ui->widget_stopPulseShapeFilterA->enableWidget(!on);

    ui->widget_playPulseShapeFilterB->enableWidget(!on);

    if (on)
        ui->widget_stopPulseShapeFilterB->enableWidget(!on);

    ui->tab_23->setEnabled(!on); //External Filters

    DRS4SettingsManager::sharedInstance()->setBurstMode(on);

    ui->menuDRS4_Board_Information->setEnabled(!on);

    if (DRS4ProgramSettingsManager::sharedInstance()->isMulticoreThreadingEnabled()
            && !on) {
        ui->actionSave_next_N_Pulses->setEnabled(false);
        ui->actionSave_next_N_Pulses_in_Range->setEnabled(false);
    }
    else {
        ui->actionSave_next_N_Pulses->setEnabled(!on);
        ui->actionSave_next_N_Pulses_in_Range->setEnabled(!on);
    }

    ui->actionLoad->setEnabled(!on);
    ui->actionLoad_Autosave->setEnabled(!on);
    ui->actionSave->setEnabled(!on);
    ui->actionSave_as->setEnabled(!on);

    ui->label_phsACounts->setEnabled(!on);
    ui->label_phsBCounts->setEnabled(!on);
    ui->label_valiLTPerSec->setEnabled(!on);
    ui->label_validCoincidencePerSec->setEnabled(!on);

    ui->pushButton_resetAll->setEnabled(!on);
    ui->pushButton_writeAComment->setEnabled(!on);

    if ( on ) {
        m_pulseRequestTimer->stop();
        m_phsRequestTimer->stop();
        m_areaRequestTimer->stop();
        m_lifetimeRequestTimer->stop();
        m_riseTimeRequestTimer->stop();
        m_persistanceRequestTimer->stop();

        m_burstModeTimer->start();
    }
    else {
        m_burstModeTimer->stop();

        m_pulseRequestTimer->start();
        m_phsRequestTimer->start();
        m_areaRequestTimer->start();
        m_lifetimeRequestTimer->start();
        m_riseTimeRequestTimer->start();
        m_persistanceRequestTimer->start();
    }

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changeMulticoreThreadingEnabled(bool on, const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_hyperthreading->clicked(on);
        ui->checkBox_hyperthreading->setChecked(on);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4ProgramSettingsManager::sharedInstance()->setEnableMulticoreThreading(on);

    if (on) {
        if ( m_pulseSaveRangeDlg->isVisible() ) {
            m_pulseSaveRangeDlg->close();
            DDELETE_SAFETY(m_pulseSaveRangeDlg);
            m_pulseSaveRangeDlg = new DRS4PulseSaveRangeDlg(m_worker);
        }

        if ( m_pulseSaveDlg->isVisible() ) {
            m_pulseSaveDlg->close();
            DDELETE_SAFETY(m_pulseSaveDlg);
            m_pulseSaveDlg = new DRS4PulseSaveDlg(m_worker);
        }

        if(DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed()) {
            DRS4FalseTruePulseStreamManager::sharedInstance()->stopAndSave();
        }
    }

    ui->actionSave_next_N_Pulses->setEnabled(!on);
    ui->actionSave_next_N_Pulses_in_Range->setEnabled(!on);
    ui->tabWidget_persistanceB->setEnabled(!on);
    ui->tabWidget_persistanceB->setEnabled(!on);
    ui->checkBox_enablePersistance->setEnabled(!on);
    ui->actionStart_True_False_Pulse_Streaming->setEnabled(!on);

    if (DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed())
        ui->actionStop_True_False_Pulse_Streaming->setEnabled(!on);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePersistancePlotEnabled(bool on, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_enablePersistance->clicked(on);
        ui->checkBox_enablePersistance->setChecked(on);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPersistanceEnabled(on);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePersistancePlotUsingCFDB_For_A(bool on, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_usingCFDB_A->clicked(on);
        ui->checkBox_usingCFDB_A->setChecked(on);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPersistanceUsingCFDBAsRefForA(on);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::changePersistancePlotUsingCFDA_For_B(bool on, const FunctionSource &source)
{
    if ( source == FunctionSource::AccessFromScript )
    {
        emit ui->checkBox_usingCFDA_B->clicked(on);
        ui->checkBox_usingCFDA_B->setChecked(on);
        return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPersistanceUsingCFDAAsRefForB(on);

    m_worker->setBusy(false);
}

bool DRS4ScopeDlg::isPositivTriggerPolarity() const
{
    QMutexLocker locker(&m_mutex);

    return ui->checkBox_positiveTriggerPol->isChecked();
}

bool DRS4ScopeDlg::isPositiveSignal() const
{
    QMutexLocker locker(&m_mutex);

    return ui->checkBox_positivSignal->isChecked();
}

void DRS4ScopeDlg::updateTemperature() {
    double opTemp = 0.;

    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    try {
        m_lastTemperatureInDegree = DRS4BoardManager::sharedInstance()->currentBoard()->GetTemperature();
        opTemp = DRS4BoardManager::sharedInstance()->currentBoard()->GetCalibratedTemperature();
    } catch ( ... ) {
        /* nothing here */
    }

    m_worker->setBusy(false);

    ui->progressBar_boardTemperature->setValue(m_lastTemperatureInDegree);

    if (m_lastTemperatureInDegree >= opTemp ) {
        ui->label_temperatureState->setStyleSheet("QLabel {color: green}");
        ui->label_temperatureState->setText(QString("operation temperature reached (calibrated at %1 °C)").arg(opTemp));
    }
    else if (m_lastTemperatureInDegree < opTemp) {
        ui->label_temperatureState->setStyleSheet("QLabel {color: red}");
        ui->label_temperatureState->setText(QString("warming up to operation temperature (calibrated at %1 °C)").arg(opTemp));
    }
}

void DRS4ScopeDlg::checkForConnection() {
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const int T = DRS4BoardManager::sharedInstance()->currentBoard()->GetTemperature();

    m_worker->setBusy(false);

    if (!T) {
        m_bConnectionLost = true;

        stopThread();

        if (m_addInfoDlg)
            m_addInfoDlg->close();

        if (m_scriptDlg)
            m_scriptDlg->close();

        if (m_pulseSaveDlg)
            m_pulseSaveDlg->close();

        if (m_pulseSaveRangeDlg)
            m_pulseSaveRangeDlg->close();

        if (m_calculatorDlg)
            m_calculatorDlg->close();

        if (m_boardInfoDlg)
            m_boardInfoDlg->close();

        if ( DRS4StreamManager::sharedInstance()->isArmed() )
            DRS4StreamManager::sharedInstance()->stopAndSave();

        if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed() )
            DRS4FalseTruePulseStreamManager::sharedInstance()->stopAndSave();

        if ( DRS4StreamDataLoader::sharedInstance()->isArmed() )
            DRS4StreamDataLoader::sharedInstance()->stop();

        if ( DRS4TextFileStreamManager::sharedInstance()->isArmed() )
            DRS4TextFileStreamManager::sharedInstance()->abort();

        if ( DRS4TextFileStreamRangeManager::sharedInstance()->isArmed() )
            DRS4TextFileStreamRangeManager::sharedInstance()->abort();

        setEnabled(false);
        ui->menubar->setEnabled(false);

        /* gray out screen */
        setGraphicsEffect(new DRS4ConnectionLostEffect);

        /* place splashscreen at the center of the screen */
        DRS4ConnectionLostSplashScreen splash;
        splash.setPixmap(QPixmap::fromImage(QImage(":/images/connection_lost").scaledToWidth(QApplication::desktop()->availableGeometry().width()*0.4, Qt::SmoothTransformation)));
        splash.show();

        /* show this forever */
        forever
            QCoreApplication::processEvents();
    }
}

void DRS4ScopeDlg::scaleRange()
{
    double minScale = ((double)DRS4SettingsManager::sharedInstance()->startCell()/1024.0f)*DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();
    double maxScale = ((double)DRS4SettingsManager::sharedInstance()->stopCell()/1024.0f)*DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();

    ui->widget_plotA->xBottom()->setAxisMinValue(minScale);
    ui->widget_plotA->xBottom()->setAxisMaxValue(maxScale);

    ui->widget_plotA->xTop()->setAxisMinValue(minScale);
    ui->widget_plotA->xTop()->setAxisMaxValue(maxScale);

    ui->widget_plotB->xBottom()->setAxisMinValue(minScale);
    ui->widget_plotB->xBottom()->setAxisMaxValue(maxScale);

    ui->widget_plotB->xTop()->setAxisMinValue(minScale);
    ui->widget_plotB->xTop()->setAxisMaxValue(maxScale);

    ui->widget_plotA->updatePlotView();
    ui->widget_plotB->updatePlotView();
}

void DRS4ScopeDlg::fullRange()
{
    ui->widget_plotA->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());
    ui->widget_plotA->xTop()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());

    ui->widget_plotB->xBottom()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());
    ui->widget_plotB->xTop()->setAxisRange(0, DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());

    ui->widget_plotA->updatePlotView();
    ui->widget_plotB->updatePlotView();
}

void DRS4ScopeDlg::startStreaming()
{
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Stream Data to..."),
                               DRS4ProgramSettingsManager::sharedInstance()->streamInputFilePath(),
                               QString("DRS4 Data Stream (*" + EXT_PULSE_STREAM_FILE + ")"));

    if ( fileName.isEmpty() )
        return;

    DRS4ProgramSettingsManager::sharedInstance()->setStreamInputFilePath(fileName);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4StreamManager::sharedInstance()->init(fileName, this);

    if ( DRS4StreamManager::sharedInstance()->start() )
    {
        m_worker->setBusy(false);

        ui->action_StartDAQStreaming->setEnabled(false);
        ui->actionStop_Running->setEnabled(true);
    }
    else
    {
        m_worker->setBusy(false);
        MSGBOX("Sorry, An Error occurred while starting Pulse-Stream!");
    }
}

void DRS4ScopeDlg::stopStreaming()
{
    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4StreamManager::sharedInstance()->stopAndSave();

    ui->action_StartDAQStreaming->setEnabled(true);
    ui->actionStop_Running->setEnabled(false);

    m_worker->setBusy(false);

    MSGBOX("OK. Finished Streaming!");
}

void DRS4ScopeDlg::startTrueFalsePulseStreaming()
{
    /*const QString fileName = QFileDialog::getSaveFileName(this, tr("Stream Data to..."),
                               DRS4ProgramSettingsManager::sharedInstance()->streamInputFilePath(),
                               QString("DRS4 Data Stream (*" + EXT_PULSE_STREAM_FILE + ")"));*/

    QFileDialog *fd = new QFileDialog(this);
    fd->setWindowTitle(tr("Stream Data to.."));
    fd->setAcceptMode(QFileDialog::AcceptSave);
    fd->setDirectory(DRS4ProgramSettingsManager::sharedInstance()->streamInputFilePath());
    fd->setViewMode(QFileDialog::Detail);
    fd->setDefaultSuffix(EXT_PULSE_STREAM_FILE);
    fd->setFileMode(QFileDialog::AnyFile);
    fd->setNameFilter(QString("DRS4 Data Stream (*" + EXT_PULSE_STREAM_FILE + ")"));

    fd->setOption(QFileDialog::DontUseNativeDialog, true);

    QGridLayout *layout = static_cast<QGridLayout*>(fd->layout());

    QHBoxLayout *hboxLayout = new QHBoxLayout;

    QLabel *label = new QLabel("stream for branch A (otherwise B)?");
    QCheckBox *streamCheckBox = new QCheckBox;
    streamCheckBox->setChecked(true);

    hboxLayout->addWidget(label);
    hboxLayout->addWidget(streamCheckBox);

    layout->addLayout(hboxLayout, layout->rowCount()-1, layout->columnCount());

    if (!fd->exec())
        return;

    const QString fileName = !fd->selectedFiles().isEmpty()?fd->selectedFiles().first():QString("");
    const bool bStreamForA = streamCheckBox->isChecked();

    DDELETE_SAFETY(label);
    DDELETE_SAFETY(streamCheckBox);
    DDELETE_SAFETY(hboxLayout);
    DDELETE_SAFETY(fd);


    if ( fileName.isEmpty() )
        return;

    DRS4ProgramSettingsManager::sharedInstance()->setStreamInputFilePath(fileName);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4FalseTruePulseStreamManager::sharedInstance()->init(fileName, this);

    if ( DRS4FalseTruePulseStreamManager::sharedInstance()->start(bStreamForA) ) {
        m_worker->setBusy(false);

        ui->actionStart_True_False_Pulse_Streaming->setEnabled(false);
        ui->actionStop_True_False_Pulse_Streaming->setEnabled(true);
    }
    else {
        m_worker->setBusy(false);
        MSGBOX("Sorry, an error occurred while starting the pulse-stream!");
    }
}

void DRS4ScopeDlg::stopTrueFalsePulseStreaming()
{
    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4FalseTruePulseStreamManager::sharedInstance()->stopAndSave();

    ui->actionStart_True_False_Pulse_Streaming->setEnabled(true);
    ui->actionStop_True_False_Pulse_Streaming->setEnabled(false);

    m_worker->setBusy(false);

    MSGBOX("OK. Finished streaming!");
}

void DRS4ScopeDlg::saveSettings()
{
    if ( m_currentSettingsPath == NO_SETTINGS_FILE_PLACEHOLDER
         || m_currentSettingsPath.contains("__drs4AutoSave.drs4LTSettings") )
    {
        saveAsSettings();
        return;
    }

    if ( m_currentSettingsPath.isEmpty() )
        return;

    if ( !DRS4SettingsManager::sharedInstance()->save(m_currentSettingsPath) )
    {
        MSGBOX("An Error occurred while saving DRS4 settings!");
    }

    updateCurrentFileLabel();
}

void DRS4ScopeDlg::saveAsSettings()
{
    const QString fileName = QFileDialog::getSaveFileName(this, tr("Save DRS4 Board Settings..."),
                               DRS4ProgramSettingsManager::sharedInstance()->settingsFilePath(),
                               QString("DRS4 Board Settings (*" + EXT_LT_SETTINGS_FILE + ")"));

    if ( fileName.isEmpty() )
        return;


    DRS4ProgramSettingsManager::sharedInstance()->setSettingsFilePath(fileName);

    if ( DRS4SettingsManager::sharedInstance()->save(fileName) )
    {
        m_currentSettingsPath = fileName;
    }
    else
    {
        MSGBOX("An Error occurred while saving DRS4 settings!");
    }

    updateCurrentFileLabel();
}

void DRS4ScopeDlg::loadSettings()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Load DRS4 Board Settings..."),
                               DRS4ProgramSettingsManager::sharedInstance()->settingsFilePath(),
                               QString("DRS4 Board Settings (*" + EXT_LT_SETTINGS_FILE + ")"));

    if ( fileName.isEmpty() )
        return;


    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const int oldValue = ui->horizontalSlider_triggerDelay->value();
    const double oldSweep = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();
    const double ratio = (double)oldValue/oldSweep;

    DRS4ProgramSettingsManager::sharedInstance()->setSettingsFilePath(fileName);

    if ( !DRS4SettingsManager::sharedInstance()->load(fileName) ) {
        MSGBOX("An Error occurred while loading DRS4 settings!");
        m_worker->setBusy(false);
        return;
    }
    else
        m_currentSettingsPath = fileName;

    m_worker->setBusy(false);

    setup(oldValue, oldSweep, ratio);
}

void DRS4ScopeDlg::setup(double oldValue, double oldSweep, double ratio)
{
    QMutexLocker locker(&m_mutex);

    DUNUSED_PARAM(oldValue);

    locker.unlock();

    const bool threadWasRunning = m_worker->isRunning();
    if ( threadWasRunning ) {        
            stopThread();
    }

    ui->verticalSlider_triggerLevelA->setValue(DRS4SettingsManager::sharedInstance()->triggerLevelAmV());
    ui->verticalSlider_triggerLevelB->setValue(DRS4SettingsManager::sharedInstance()->triggerLevelBmV());

    ui->spinBox_startCell->setValue(DRS4SettingsManager::sharedInstance()->startCell());
    ui->spinBox_stopCell->setValue(DRS4SettingsManager::sharedInstance()->stopCell());

    ui->doubleSpinBox_cfdA->setValue(DRS4SettingsManager::sharedInstance()->cfdLevelA()*100);
    ui->doubleSpinBox_cfdB->setValue(DRS4SettingsManager::sharedInstance()->cfdLevelB()*100);

    ui->spinBox_fitIterations->setValue(DRS4SettingsManager::sharedInstance()->fitIterations());
    ui->spinBox_fitIterations_Merged->setValue(DRS4SettingsManager::sharedInstance()->fitIterationsMerged());
    ui->spinBox_fitIterations_AB->setValue(DRS4SettingsManager::sharedInstance()->fitIterationsAB());
    ui->spinBox_fitIterations_BA->setValue(DRS4SettingsManager::sharedInstance()->fitIterationsBA());

    ui->doubleSpinBox_meanCoincidence->setValue(DRS4SettingsManager::sharedInstance()->meanCableDelay()*1000.0f);

    //plot-related (double-spin)-boxes:
    disconnect(ui->doubleSpinBox_offsetAB, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsAB(double)));
    disconnect(ui->doubleSpinBox_offsetBA, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsBA(double)));
    disconnect(ui->doubleSpinBox_offsetCoincidence, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsCoincidence(double)));
    disconnect(ui->doubleSpinBox_offsetMerged, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsMerged(double)));

    disconnect(ui->doubleSpinBox_scalerAB, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsAB(double)));
    disconnect(ui->doubleSpinBox_scalerBA, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsBA(double)));
    disconnect(ui->doubleSpinBox_scalerCoincidence, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsCoincidence(double)));
    disconnect(ui->doubleSpinBox_scalerMerged, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsMerged(double)));

    disconnect(ui->spinBox_chnCountAB, SIGNAL(valueChanged(int)), this, SLOT(changeChannelSettingsAB2(int)));
    disconnect(ui->spinBox_chnCountBA, SIGNAL(valueChanged(int)), this, SLOT(changeChannelSettingsBA2(int)));
    disconnect(ui->spinBox_chnCountCoincidence, SIGNAL(valueChanged(int)), this, SLOT(changeChannelSettingsCoincidence2(int)));
    disconnect(ui->spinBox_chnCountMerged, SIGNAL(valueChanged(int)), this, SLOT(changeChannelSettingsMerged2(int)));

    disconnect(ui->spinBox_startMinA, SIGNAL(valueChanged(int)), this, SLOT(changePHSStartMinA(int)));
    disconnect(ui->spinBox_startMaxA, SIGNAL(valueChanged(int)), this, SLOT(changePHSStartMaxA(int)));
    disconnect(ui->spinBox_stopMinA, SIGNAL(valueChanged(int)), this, SLOT(changePHSStopMinA(int)));
    disconnect(ui->spinBox_stopMaxA, SIGNAL(valueChanged(int)), this, SLOT(changePHSStopMaxA(int)));

    disconnect(ui->spinBox_startMinB, SIGNAL(valueChanged(int)), this, SLOT(changePHSStartMinB(int)));
    disconnect(ui->spinBox_startMaxB, SIGNAL(valueChanged(int)), this, SLOT(changePHSStartMaxB(int)));
    disconnect(ui->spinBox_stopMinB, SIGNAL(valueChanged(int)), this, SLOT(changePHSStopMinB(int)));
    disconnect(ui->spinBox_stopMaxB, SIGNAL(valueChanged(int)), this, SLOT(changePHSStopMaxB(int)));

    disconnect(ui->doubleSpinBox_areaANorm, SIGNAL(valueChanged(double)), this, SLOT(changeNormalizationForPulseAreaFilterA(double)));
    disconnect(ui->doubleSpinBox_areaBNorm_2, SIGNAL(valueChanged(double)), this, SLOT(changeNormalizationForPulseAreaFilterB(double)));

    disconnect(ui->spinBox_areaBinningCntA, SIGNAL(valueChanged(int)), this, SLOT(changePulseAreaFilterA(int)));
    disconnect(ui->spinBox_areaBinningCntB_2, SIGNAL(valueChanged(int)), this, SLOT(changePulseAreaFilterB(int)));

    disconnect(ui->doubleSpinBox_areaFilter_leftA_lower, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsLowerLeftA(double)));
    disconnect(ui->doubleSpinBox_areaFilter_rightA_lower, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsLowerRightA(double)));
    disconnect(ui->doubleSpinBox_areaFilter_leftA_upper, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsUpperLeftA(double)));
    disconnect(ui->doubleSpinBox_areaFilter_rightA_upper, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsUpperRightA(double)));

    disconnect(ui->doubleSpinBox_areaFilter_leftB_lower_2, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsLowerLeftB(double)));
    disconnect(ui->doubleSpinBox_areaFilter_rightB_lower_2, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsLowerRightB(double)));
    disconnect(ui->doubleSpinBox_areaFilter_leftB_upper_2, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsUpperLeftB(double)));
    disconnect(ui->doubleSpinBox_areaFilter_rightB_upper_2, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsUpperRightB(double)));

    disconnect(ui->doubleSpinBox_riseTimeFilterTimeScaleA, SIGNAL(valueChanged(double)), this, SLOT(changeRiseTimeFilterScaleInNanosecondsA(double)));
    disconnect(ui->doubleSpinBox_riseTimeFilterTimeScaleB, SIGNAL(valueChanged(double)), this, SLOT(changeRiseTimeFilterScaleInNanosecondsB(double)));
    disconnect(ui->spinBox_riseTimeFilter_binningA, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterBinningCountA(int)));
    disconnect(ui->spinBox_riseTimeFilter_binningB, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterBinningCountB(int)));
    disconnect(ui->spinBox_riseTimerFilterLeftA, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterLeftWindowA(int)));
    disconnect(ui->spinBox_riseTimeFilterRightA, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterRightWindowA(int)));
    disconnect(ui->spinBox_riseTimerFilterLeftB, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterLeftWindowB(int)));
    disconnect(ui->spinBox_riseTimeFilterRightB, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterRightWindowB(int)));

    disconnect(ui->spinBox_NPulsesShapeFilterA, SIGNAL(valueChanged(int)), this, SLOT(changePulseShapeFilterPulseRecordNumberA(int)));
    disconnect(ui->spinBox_NPulsesShapeFilterB, SIGNAL(valueChanged(int)), this, SLOT(changePulseShapeFilterPulseRecordNumberB(int)));

    disconnect(ui->doubleSpinBox_shapeFilterLeftCFDA, SIGNAL(valueChanged(double)), this, SLOT(changeLeftAPulseShapeFilter(double)));
    disconnect(ui->doubleSpinBox_shapeFilterRightCFDA, SIGNAL(valueChanged(double)), this, SLOT(changeRightAPulseShapeFilter(double)));

    disconnect(ui->doubleSpinBox_shapeFilterLeftCFDROIA, SIGNAL(valueChanged(double)), this, SLOT(changeROILeftAPulseShapeFilter(double)));
    disconnect(ui->doubleSpinBox_shapeFilterRightCFDROIA, SIGNAL(valueChanged(double)), this, SLOT(changeROIRightAPulseShapeFilter(double)));

    disconnect(ui->doubleSpinBox_shapeFilterLeftCFDB, SIGNAL(valueChanged(double)), this, SLOT(changeLeftBPulseShapeFilter(double)));
    disconnect(ui->doubleSpinBox_shapeFilterRightCFDB, SIGNAL(valueChanged(double)), this, SLOT(changeRightBPulseShapeFilter(double)));

    disconnect(ui->doubleSpinBox_shapeFilterLeftCFDROIB, SIGNAL(valueChanged(double)), this, SLOT(changeROILeftBPulseShapeFilter(double)));
    disconnect(ui->doubleSpinBox_shapeFilterRightCFDROIB, SIGNAL(valueChanged(double)), this, SLOT(changeROIRightBPulseShapeFilter(double)));

    disconnect(ui->doubleSpinBox_shapeFilterLLStddevA, SIGNAL(valueChanged(double)), this, SLOT(changeLowerStdDevFractionAPulseShapeFilter(double)));
    disconnect(ui->doubleSpinBox_shapeFilterULStddevA, SIGNAL(valueChanged(double)), this, SLOT(changeUpperStdDevFractionAPulseShapeFilter(double)));

    disconnect(ui->doubleSpinBox_shapeFilterLLStddevB, SIGNAL(valueChanged(double)), this, SLOT(changeLowerStdDevFractionBPulseShapeFilter(double)));
    disconnect(ui->doubleSpinBox_shapeFilterULStddevB, SIGNAL(valueChanged(double)), this, SLOT(changeUpperStdDevFractionBPulseShapeFilter(double)));

    disconnect(ui->spinBox_baseLineStartCell_A, SIGNAL(valueChanged(int)), this, SLOT(changeBaselineStartCellA(int)));
    disconnect(ui->spinBox_baseLine_region_A, SIGNAL(valueChanged(int)), this, SLOT(changeBaselineRegionA(int)));
    disconnect(ui->doubleSpinBox_baseLineValue_A, SIGNAL(valueChanged(double)), this, SLOT(changeBaselineShiftValueA(double)));
    disconnect(ui->doubleSpinBox_baseLineLimit_A, SIGNAL(valueChanged(double)), this, SLOT(changeBaselineFilterRejectionLimitA(double)));

    disconnect(ui->spinBox_baseLineStartCell_B, SIGNAL(valueChanged(int)), this, SLOT(changeBaselineStartCellB(int)));
    disconnect(ui->spinBox_baseLine_region_B, SIGNAL(valueChanged(int)), this, SLOT(changeBaselineRegionB(int)));
    disconnect(ui->doubleSpinBox_baseLineValue_B, SIGNAL(valueChanged(double)), this, SLOT(changeBaselineShiftValueB(double)));
    disconnect(ui->doubleSpinBox_baseLineLimit_B, SIGNAL(valueChanged(double)), this, SLOT(changeBaselineFilterRejectionLimitB(double)));

    disconnect(ui->spinBox_medianFilterWindowSizeA, SIGNAL(valueChanged(int)), this, SLOT(changeMedianFilterWindowSizeA(int)));
    disconnect(ui->spinBox_medianFilterWindowSizeB, SIGNAL(valueChanged(int)), this, SLOT(changeMedianFilterWindowSizeB(int)));

    disconnect(ui->doubleSpinBox_persistanceLeftA, SIGNAL(valueChanged(double)), this, SLOT(changeLeftAPersistance(double)));
    disconnect(ui->doubleSpinBox_persistanceLeftB, SIGNAL(valueChanged(double)), this, SLOT(changeLeftBPersistance(double)));
    disconnect(ui->doubleSpinBox_persistanceRightA, SIGNAL(valueChanged(double)), this, SLOT(changeRightAPersistance(double)));
    disconnect(ui->doubleSpinBox_persistanceRightB, SIGNAL(valueChanged(double)), this, SLOT(changeRightBPersistance(double)));

        ui->doubleSpinBox_offsetAB->setValue(DRS4SettingsManager::sharedInstance()->offsetInNSAB());
        ui->doubleSpinBox_offsetBA->setValue(DRS4SettingsManager::sharedInstance()->offsetInNSBA());
        ui->doubleSpinBox_offsetCoincidence->setValue(DRS4SettingsManager::sharedInstance()->offsetInNSCoincidence());
        ui->doubleSpinBox_offsetMerged->setValue(DRS4SettingsManager::sharedInstance()->offsetInNSMerged());

        ui->doubleSpinBox_scalerAB->setValue(DRS4SettingsManager::sharedInstance()->scalerInNSAB());
        ui->doubleSpinBox_scalerBA->setValue(DRS4SettingsManager::sharedInstance()->scalerInNSBA());
        ui->doubleSpinBox_scalerCoincidence->setValue(DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence());
        ui->doubleSpinBox_scalerMerged->setValue(DRS4SettingsManager::sharedInstance()->scalerInNSMerged());

        ui->spinBox_chnCountAB->setValue(DRS4SettingsManager::sharedInstance()->channelCntAB());
        ui->spinBox_chnCountBA->setValue(DRS4SettingsManager::sharedInstance()->channelCntBA());
        ui->spinBox_chnCountCoincidence->setValue(DRS4SettingsManager::sharedInstance()->channelCntCoincindence());
        ui->spinBox_chnCountMerged->setValue(DRS4SettingsManager::sharedInstance()->channelCntMerged());

        ui->spinBox_startMinA->setValue(DRS4SettingsManager::sharedInstance()->startChanneAMin());
        ui->spinBox_startMaxA->setValue(DRS4SettingsManager::sharedInstance()->startChanneAMax());
        ui->spinBox_startMinB->setValue(DRS4SettingsManager::sharedInstance()->startChanneBMin());
        ui->spinBox_startMaxB->setValue(DRS4SettingsManager::sharedInstance()->startChanneBMax());

        ui->spinBox_stopMinA->setValue(DRS4SettingsManager::sharedInstance()->stopChanneAMin());
        ui->spinBox_stopMaxA->setValue(DRS4SettingsManager::sharedInstance()->stopChanneAMax());
        ui->spinBox_stopMinB->setValue(DRS4SettingsManager::sharedInstance()->stopChanneBMin());
        ui->spinBox_stopMaxB->setValue(DRS4SettingsManager::sharedInstance()->stopChanneBMax());

        ui->doubleSpinBox_areaANorm->setValue(DRS4SettingsManager::sharedInstance()->pulseAreaFilterNormalizationA());
        ui->doubleSpinBox_areaBNorm_2->setValue(DRS4SettingsManager::sharedInstance()->pulseAreaFilterNormalizationB());

        ui->spinBox_areaBinningCntA->setValue(DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningA());
        ui->spinBox_areaBinningCntB_2->setValue(DRS4SettingsManager::sharedInstance()->pulseAreaFilterBinningB());

        ui->doubleSpinBox_areaFilter_leftA_lower->setValue(DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitLowerLeftA());
        ui->doubleSpinBox_areaFilter_rightA_lower->setValue(DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitLowerRightA());
        ui->doubleSpinBox_areaFilter_leftA_upper->setValue(DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitUpperLeftA());
        ui->doubleSpinBox_areaFilter_rightA_upper->setValue(DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitUpperRightA());
        ui->doubleSpinBox_areaFilter_leftB_lower_2->setValue(DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitLowerLeftB());
        ui->doubleSpinBox_areaFilter_rightB_lower_2->setValue(DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitLowerRightB());
        ui->doubleSpinBox_areaFilter_leftB_upper_2->setValue(DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitUpperLeftB());
        ui->doubleSpinBox_areaFilter_rightB_upper_2->setValue(DRS4SettingsManager::sharedInstance()->pulseAreaFilterLimitUpperRightB());

        ui->doubleSpinBox_riseTimeFilterTimeScaleA->setValue(DRS4SettingsManager::sharedInstance()->riseTimeFilterScaleInNanosecondsOfA());
        ui->doubleSpinBox_riseTimeFilterTimeScaleB->setValue(DRS4SettingsManager::sharedInstance()->riseTimeFilterScaleInNanosecondsOfB());
        ui->spinBox_riseTimeFilter_binningA->setValue(DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfA());
        ui->spinBox_riseTimeFilter_binningB->setValue(DRS4SettingsManager::sharedInstance()->riseTimeFilterBinningOfB());
        ui->spinBox_riseTimerFilterLeftA->setValue(DRS4SettingsManager::sharedInstance()->riseTimeFilterLeftWindowOfA());
        ui->spinBox_riseTimeFilterRightA->setValue(DRS4SettingsManager::sharedInstance()->riseTimeFilterRightWindowOfA());
        ui->spinBox_riseTimerFilterLeftB->setValue(DRS4SettingsManager::sharedInstance()->riseTimeFilterLeftWindowOfB());
        ui->spinBox_riseTimeFilterRightB->setValue(DRS4SettingsManager::sharedInstance()->riseTimeFilterRightWindowOfB());

        ui->spinBox_NPulsesShapeFilterA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedA());
        ui->spinBox_NPulsesShapeFilterB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterNumberOfPulsesToBeRecordedB());

        ui->doubleSpinBox_shapeFilterLeftCFDA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterLeftInNsOfA());
        ui->doubleSpinBox_shapeFilterRightCFDA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterRightInNsOfA());

        ui->doubleSpinBox_shapeFilterLeftCFDROIA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfA());
        ui->doubleSpinBox_shapeFilterRightCFDROIA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfA());

        ui->doubleSpinBox_shapeFilterLeftCFDB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterLeftInNsOfB());
        ui->doubleSpinBox_shapeFilterRightCFDB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterRightInNsOfB());

        ui->doubleSpinBox_shapeFilterLeftCFDROIB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROILeftInNsOfB());
        ui->doubleSpinBox_shapeFilterRightCFDROIB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterROIRightInNsOfB());

        ui->doubleSpinBox_shapeFilterLLStddevA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevLowerFractionA());
        ui->doubleSpinBox_shapeFilterULStddevA->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevUpperFractionA());

        ui->doubleSpinBox_shapeFilterLLStddevB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevLowerFractionB());
        ui->doubleSpinBox_shapeFilterULStddevB->setValue(DRS4SettingsManager::sharedInstance()->pulseShapeFilterStdDevUpperFractionB());

        m_pulseShapeSplineDataA = *DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrA();
        m_pulseShapeSplineDataB = *DRS4SettingsManager::sharedInstance()->pulseShapeFilterDataPtrB();

        ui->spinBox_baseLineStartCell_A->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationStartCellA());
        ui->spinBox_baseLine_region_A->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationRegionA());
        ui->doubleSpinBox_baseLineValue_A->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationShiftValueInMVA());
        ui->doubleSpinBox_baseLineLimit_A->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitInPercentageA());

        ui->spinBox_baseLineStartCell_B->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationStartCellB());
        ui->spinBox_baseLine_region_B->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationRegionB());
        ui->doubleSpinBox_baseLineValue_B->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationShiftValueInMVB());
        ui->doubleSpinBox_baseLineLimit_B->setValue(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitInPercentageB());

        ui->spinBox_medianFilterWindowSizeA->setValue(DRS4SettingsManager::sharedInstance()->medianFilterWindowSizeA());
        ui->spinBox_medianFilterWindowSizeB->setValue(DRS4SettingsManager::sharedInstance()->medianFilterWindowSizeB());

        ui->doubleSpinBox_persistanceLeftA->setValue(DRS4SettingsManager::sharedInstance()->persistanceLeftInNsOfA());
        ui->doubleSpinBox_persistanceLeftB->setValue(DRS4SettingsManager::sharedInstance()->persistanceLeftInNsOfB());
        ui->doubleSpinBox_persistanceRightA->setValue(DRS4SettingsManager::sharedInstance()->persistanceRightInNsOfA());
        ui->doubleSpinBox_persistanceRightB->setValue(DRS4SettingsManager::sharedInstance()->persistanceRightInNsOfB());

    connect(ui->doubleSpinBox_offsetAB, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsAB(double)));
    connect(ui->doubleSpinBox_offsetBA, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsBA(double)));
    connect(ui->doubleSpinBox_offsetCoincidence, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsCoincidence(double)));
    connect(ui->doubleSpinBox_offsetMerged, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsMerged(double)));

    connect(ui->doubleSpinBox_scalerAB, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsAB(double)));
    connect(ui->doubleSpinBox_scalerBA, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsBA(double)));
    connect(ui->doubleSpinBox_scalerCoincidence, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsCoincidence(double)));
    connect(ui->doubleSpinBox_scalerMerged, SIGNAL(valueChanged(double)), this, SLOT(changeChannelSettingsMerged(double)));

    connect(ui->spinBox_chnCountAB, SIGNAL(valueChanged(int)), this, SLOT(changeChannelSettingsAB2(int)));
    connect(ui->spinBox_chnCountBA, SIGNAL(valueChanged(int)), this, SLOT(changeChannelSettingsBA2(int)));
    connect(ui->spinBox_chnCountCoincidence, SIGNAL(valueChanged(int)), this, SLOT(changeChannelSettingsCoincidence2(int)));
    connect(ui->spinBox_chnCountMerged, SIGNAL(valueChanged(int)), this, SLOT(changeChannelSettingsMerged2(int)));

    connect(ui->spinBox_startMinA, SIGNAL(valueChanged(int)), this, SLOT(changePHSStartMinA(int)));
    connect(ui->spinBox_startMaxA, SIGNAL(valueChanged(int)), this, SLOT(changePHSStartMaxA(int)));
    connect(ui->spinBox_stopMinA, SIGNAL(valueChanged(int)), this, SLOT(changePHSStopMinA(int)));
    connect(ui->spinBox_stopMaxA, SIGNAL(valueChanged(int)), this, SLOT(changePHSStopMaxA(int)));

    connect(ui->spinBox_startMinB, SIGNAL(valueChanged(int)), this, SLOT(changePHSStartMinB(int)));
    connect(ui->spinBox_startMaxB, SIGNAL(valueChanged(int)), this, SLOT(changePHSStartMaxB(int)));
    connect(ui->spinBox_stopMinB, SIGNAL(valueChanged(int)), this, SLOT(changePHSStopMinB(int)));
    connect(ui->spinBox_stopMaxB, SIGNAL(valueChanged(int)), this, SLOT(changePHSStopMaxB(int)));

    connect(ui->doubleSpinBox_areaANorm, SIGNAL(valueChanged(double)), this, SLOT(changeNormalizationForPulseAreaFilterA(double)));
    connect(ui->doubleSpinBox_areaBNorm_2, SIGNAL(valueChanged(double)), this, SLOT(changeNormalizationForPulseAreaFilterB(double)));

    connect(ui->spinBox_areaBinningCntA, SIGNAL(valueChanged(int)), this, SLOT(changePulseAreaFilterA(int)));
    connect(ui->spinBox_areaBinningCntB_2, SIGNAL(valueChanged(int)), this, SLOT(changePulseAreaFilterB(int)));

    connect(ui->doubleSpinBox_areaFilter_leftA_lower, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsLowerLeftA(double)));
    connect(ui->doubleSpinBox_areaFilter_rightA_lower, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsLowerRightA(double)));
    connect(ui->doubleSpinBox_areaFilter_leftA_upper, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsUpperLeftA(double)));
    connect(ui->doubleSpinBox_areaFilter_rightA_upper, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsUpperRightA(double)));

    connect(ui->doubleSpinBox_areaFilter_leftB_lower_2, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsLowerLeftB(double)));
    connect(ui->doubleSpinBox_areaFilter_rightB_lower_2, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsLowerRightB(double)));
    connect(ui->doubleSpinBox_areaFilter_leftB_upper_2, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsUpperLeftB(double)));
    connect(ui->doubleSpinBox_areaFilter_rightB_upper_2, SIGNAL(valueChanged(double)), this, SLOT(changePulseAreaFilterLimitsUpperRightB(double)));

    connect(ui->doubleSpinBox_riseTimeFilterTimeScaleA, SIGNAL(valueChanged(double)), this, SLOT(changeRiseTimeFilterScaleInNanosecondsA(double)));
    connect(ui->doubleSpinBox_riseTimeFilterTimeScaleB, SIGNAL(valueChanged(double)), this, SLOT(changeRiseTimeFilterScaleInNanosecondsB(double)));
    connect(ui->spinBox_riseTimeFilter_binningA, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterBinningCountA(int)));
    connect(ui->spinBox_riseTimeFilter_binningB, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterBinningCountB(int)));
    connect(ui->spinBox_riseTimerFilterLeftA, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterLeftWindowA(int)));
    connect(ui->spinBox_riseTimeFilterRightA, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterRightWindowA(int)));
    connect(ui->spinBox_riseTimerFilterLeftB, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterLeftWindowB(int)));
    connect(ui->spinBox_riseTimeFilterRightB, SIGNAL(valueChanged(int)), this, SLOT(changeRiseTimeFilterRightWindowB(int)));

    connect(ui->spinBox_NPulsesShapeFilterA, SIGNAL(valueChanged(int)), SLOT(changePulseShapeFilterPulseRecordNumberA(int)));
    connect(ui->spinBox_NPulsesShapeFilterB, SIGNAL(valueChanged(int)), SLOT(changePulseShapeFilterPulseRecordNumberB(int)));

    connect(ui->doubleSpinBox_shapeFilterLeftCFDA, SIGNAL(valueChanged(double)), SLOT(changeLeftAPulseShapeFilter(double)));
    connect(ui->doubleSpinBox_shapeFilterRightCFDA, SIGNAL(valueChanged(double)), SLOT(changeRightAPulseShapeFilter(double)));

    connect(ui->doubleSpinBox_shapeFilterLeftCFDROIA, SIGNAL(valueChanged(double)), SLOT(changeROILeftAPulseShapeFilter(double)));
    connect(ui->doubleSpinBox_shapeFilterRightCFDROIA, SIGNAL(valueChanged(double)), SLOT(changeROIRightAPulseShapeFilter(double)));

    connect(ui->doubleSpinBox_shapeFilterLeftCFDB, SIGNAL(valueChanged(double)), SLOT(changeLeftBPulseShapeFilter(double)));
    connect(ui->doubleSpinBox_shapeFilterRightCFDB, SIGNAL(valueChanged(double)), SLOT(changeRightBPulseShapeFilter(double)));

    connect(ui->doubleSpinBox_shapeFilterLeftCFDROIB, SIGNAL(valueChanged(double)), SLOT(changeROILeftBPulseShapeFilter(double)));
    connect(ui->doubleSpinBox_shapeFilterRightCFDROIB, SIGNAL(valueChanged(double)), SLOT(changeROIRightBPulseShapeFilter(double)));

    connect(ui->doubleSpinBox_shapeFilterLLStddevA, SIGNAL(valueChanged(double)), SLOT(changeLowerStdDevFractionAPulseShapeFilter(double)));
    connect(ui->doubleSpinBox_shapeFilterULStddevA, SIGNAL(valueChanged(double)), SLOT(changeUpperStdDevFractionAPulseShapeFilter(double)));

    connect(ui->doubleSpinBox_shapeFilterLLStddevB, SIGNAL(valueChanged(double)), SLOT(changeLowerStdDevFractionBPulseShapeFilter(double)));
    connect(ui->doubleSpinBox_shapeFilterULStddevB, SIGNAL(valueChanged(double)), SLOT(changeUpperStdDevFractionBPulseShapeFilter(double)));

    connect(ui->spinBox_baseLineStartCell_A, SIGNAL(valueChanged(int)), this, SLOT(changeBaselineStartCellA(int)));
    connect(ui->spinBox_baseLine_region_A, SIGNAL(valueChanged(int)), this, SLOT(changeBaselineRegionA(int)));
    connect(ui->doubleSpinBox_baseLineValue_A, SIGNAL(valueChanged(double)), this, SLOT(changeBaselineShiftValueA(double)));
    connect(ui->doubleSpinBox_baseLineLimit_A, SIGNAL(valueChanged(double)), this, SLOT(changeBaselineFilterRejectionLimitA(double)));

    connect(ui->spinBox_baseLineStartCell_B, SIGNAL(valueChanged(int)), this, SLOT(changeBaselineStartCellB(int)));
    connect(ui->spinBox_baseLine_region_B, SIGNAL(valueChanged(int)), this, SLOT(changeBaselineRegionB(int)));
    connect(ui->doubleSpinBox_baseLineValue_B, SIGNAL(valueChanged(double)), this, SLOT(changeBaselineShiftValueB(double)));
    connect(ui->doubleSpinBox_baseLineLimit_B, SIGNAL(valueChanged(double)), this, SLOT(changeBaselineFilterRejectionLimitB(double)));

    connect(ui->spinBox_medianFilterWindowSizeA, SIGNAL(valueChanged(int)), this, SLOT(changeMedianFilterWindowSizeA(int)));
    connect(ui->spinBox_medianFilterWindowSizeB, SIGNAL(valueChanged(int)), this, SLOT(changeMedianFilterWindowSizeB(int)));

    connect(ui->doubleSpinBox_persistanceLeftA, SIGNAL(valueChanged(double)), this, SLOT(changeLeftAPersistance(double)));
    connect(ui->doubleSpinBox_persistanceLeftB, SIGNAL(valueChanged(double)), this, SLOT(changeLeftBPersistance(double)));
    connect(ui->doubleSpinBox_persistanceRightA, SIGNAL(valueChanged(double)), this, SLOT(changeRightAPersistance(double)));
    connect(ui->doubleSpinBox_persistanceRightB, SIGNAL(valueChanged(double)), this, SLOT(changeRightBPersistance(double)));

    ui->comboBox_triggerLogic->setCurrentIndex(DRS4SettingsManager::sharedInstance()->triggerSource_index());

    switch ( (int)DRS4SettingsManager::sharedInstance()->sweepInNanoseconds() ) {
    case 200: //5GHz
        ui->comboBox_sampleSpeed->setCurrentIndex(0);
        break;
    case 500: //2GHz
        ui->comboBox_sampleSpeed->setCurrentIndex(1);
        break;
    case 1000: //1GHz
        ui->comboBox_sampleSpeed->setCurrentIndex(2);
        break;
    case 2000: //0.5GHz
        ui->comboBox_sampleSpeed->setCurrentIndex(3);
        break;
    default:
        break;
    }

   switch (DRS4SettingsManager::sharedInstance()->channelNumberA()) {
   case 0:
       emit ui->checkBox_chn1_A->clicked(true);
       break;

   case 1:
       emit ui->checkBox_chn2_A->clicked(true);
       break;

   case 2:
       emit ui->checkBox_chn3_A->clicked(true);
       break;

   case 3:
       emit ui->checkBox_chn4_A->clicked(true);
       break;
   }

   switch (DRS4SettingsManager::sharedInstance()->channelNumberB()) {
   case 0:
       emit ui->checkBox_chn1_B->clicked(true);
       break;

   case 1:
       emit ui->checkBox_chn2_B->clicked(true);
       break;

   case 2:
       emit ui->checkBox_chn3_B->clicked(true);
       break;

   case 3:
       emit ui->checkBox_chn4_B->clicked(true);
       break;
   }

    ui->comboBox_pulseShapeFilterRecordScheme->setCurrentIndex((int)DRS4SettingsManager::sharedInstance()->pulseShapeFilterRecordScheme());

    ui->checkBox_forceStopStopCoincindence->setChecked(DRS4SettingsManager::sharedInstance()->isforceCoincidence());

    ui->checkBox_positiveTriggerPol->setChecked(!DRS4SettingsManager::sharedInstance()->isTriggerPolarityPositive());
    ui->checkBox_positivSignal->setChecked(DRS4SettingsManager::sharedInstance()->isPositiveSignal());

    ui->checkBox_activateAreaFilter->setChecked(DRS4SettingsManager::sharedInstance()->isPulseAreaFilterEnabled());
    ui->checkBox_enableAreaFilter->setChecked(DRS4SettingsManager::sharedInstance()->isPulseAreaFilterPlotEnabled());

    ui->checkBox_activateRiseTimeFilter->setChecked(DRS4SettingsManager::sharedInstance()->isRiseTimeFilterEnabled());
    ui->checkBox_enableRiseTimeFilter->setChecked(DRS4SettingsManager::sharedInstance()->isRiseTimeFilterPlotEnabled());

    ui->checkBox_medianFilterAActivate->setChecked(DRS4SettingsManager::sharedInstance()->medianFilterAEnabled());
    ui->checkBox_medianFilterBActivate->setChecked(DRS4SettingsManager::sharedInstance()->medianFilterBEnabled());

    ui->checkBox_activatePulseShapeFilterA->setChecked(DRS4SettingsManager::sharedInstance()->pulseShapeFilterEnabledA());
    ui->checkBox_activatePulseShapeFilterB->setChecked(DRS4SettingsManager::sharedInstance()->pulseShapeFilterEnabledB());

    ui->checkBox_activateBaselineCorrection_A->setChecked(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationEnabledA());
    ui->checkBox_baseLineReject_A->setChecked(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitRejectLimitA());

    ui->checkBox_activateBaselineCorrection_B->setChecked(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationEnabledB());
    ui->checkBox_baseLineReject_B->setChecked(DRS4SettingsManager::sharedInstance()->baselineCorrectionCalculationLimitRejectLimitB());

    ui->checkBox_enablePersistance->setChecked(DRS4SettingsManager::sharedInstance()->isPersistanceEnabled());
    ui->checkBox_usingCFDB_A->setChecked(DRS4SettingsManager::sharedInstance()->persistanceUsingCFDBAsRefForA());
    ui->checkBox_usingCFDA_B->setChecked(DRS4SettingsManager::sharedInstance()->persistanceUsingCFDAAsRefForB());

    ui->horizontalSlider_triggerDelay->setRange(0, DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());

    if ( DRS4SettingsManager::sharedInstance()->sweepInNanoseconds() == oldSweep )
        ui->horizontalSlider_triggerDelay->setValue(DRS4SettingsManager::sharedInstance()->triggerDelayInNs());
    else
        ui->horizontalSlider_triggerDelay->setValue(ratio*DRS4SettingsManager::sharedInstance()->sweepInNanoseconds());

    /* CFD algorithm */
    ui->widget_CFDAlgorithm->load();

    changeStartCell();
    changeStopCell();

    resetPHSA();
    resetPHSB();

    resetLTSpectrumAB();
    resetLTSpectrumBA();
    resetLTSpectrumCoincidence();
    resetLTSpectrumMerged();

    resetAreaPlotA();
    resetAreaPlotB();

    resetRiseTimePlotA();
    resetRiseTimePlotB();

    resetPersistancePlotA();
    resetPersistancePlotB();

    adaptPersistancePlotA();
    adaptPersistancePlotB();

    adaptPulseShapeFilterPlotA();
    adaptPulseShapeFilterPlotB();

    if ( DRS4ScriptManager::sharedInstance()->isArmed() )
        emit signalUpdateInfoDlgFromScript(DRS4SettingsManager::sharedInstance()->comment());
    else {
        ui->pushButton_shapeFilterCalcA->setText(QString("calculate (0)"));
        ui->pushButton_shapeFilterCalcB->setText(QString("calculate (0)"));

        m_addInfoDlg->setAddInfo(DRS4SettingsManager::sharedInstance()->comment());
    }

    changeChannelSettingsAB(0);
    changeChannelSettingsBA(0);
    changeChannelSettingsCoincidence(0);
    changeChannelSettingsMerged(0);

    if ( DRS4ScriptManager::sharedInstance()->isArmed() ) {
        const QString currentFileName = updateCurrentFileLabel(true);

        emit signalUpdateCurrentFileLabelFromScript(currentFileName);
    }
    else
        updateCurrentFileLabel();

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4PulseGenerator::sharedInstance()->update();

    if ( threadWasRunning ) {
        startThread();
    }
}

void DRS4ScopeDlg::loadSimulationToolSettings()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Load Simulation Input-File..."),
                               DRS4ProgramSettingsManager::sharedInstance()->simulationInputFilePath(),
                               QString("Simulation Input-File (*" + EXT_SIMULATION_INPUT_FILE + ")"));

    if ( fileName.isEmpty() )
        return;


    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4ProgramSettingsManager::sharedInstance()->setSimulationInputFilePath(fileName);

    if ( !DRS4SimulationSettingsManager::sharedInstance()->load(fileName) )
    {
        MSGBOX("An Error occurred while loading Simulation Input-File!");
        m_worker->setBusy(false);
        return;
    }

    DRS4BoardManager::sharedInstance()->setDemoFromStreamData(false);

    if ( DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4PulseGenerator::sharedInstance()->update();

    m_worker->setBusy(false);

    updateCurrentFileLabel();
}

void DRS4ScopeDlg::loadStreamingData()
{
    if ( DRS4StreamDataLoader::sharedInstance()->isArmed() )
    {
        const QMessageBox::StandardButton reply = QMessageBox::question(this, "Stop Pulse-Generation from Data-Stream?", "Stop Pulse Generation from Data-Stream?", QMessageBox::Yes|QMessageBox::No);

        if ( reply == QMessageBox::Yes )
        {
            DRS4StreamDataLoader::sharedInstance()->stop();
            return;
        }
        else
            return;
    }

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const QString fileName = QFileDialog::getOpenFileName(this, tr("Load DRS4 Data-Stream..."),
                               DRS4ProgramSettingsManager::sharedInstance()->streamInputFilePath(),
                               QString("DRS4 Data Stream (*" + EXT_PULSE_STREAM_FILE + ")"));

    if ( fileName.isEmpty() ) {
        m_worker->setBusy(false);

        return;
    }

    DRS4ProgramSettingsManager::sharedInstance()->setStreamInputFilePath(fileName);

    if ( !DRS4StreamDataLoader::sharedInstance()->init(fileName, this) )
    {
        MSGBOX("Sorry, cannot load Data-Stream File!");
        m_worker->setBusy(false);
        return;
    }

    DRS4BoardManager::sharedInstance()->setDemoFromStreamData(true);

    m_worker->setBusy(false);

    updateCurrentFileLabel();
}

void DRS4ScopeDlg::runningFromDataStreamStarted()
{
    ui->actionLoad_Simulation_Input->setEnabled(false);
    ui->actionLoad_Streaming_Data_File->setText("Abort Pulse-Generation from Data-Stream...");
}

void DRS4ScopeDlg::runningFromDataStreamFinished()
{
    ui->actionLoad_Simulation_Input->setEnabled(true);
    ui->actionLoad_Streaming_Data_File->setText("Load Data-Stream File...");
}

void DRS4ScopeDlg::autoSave()
{
    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->save("__drs4AutoSave" + EXT_LT_SETTINGS_FILE, true);

    m_worker->setBusy(false);
}

void DRS4ScopeDlg::loadAutoSave()
{
    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const int oldValue = ui->horizontalSlider_triggerDelay->value();
    const double oldSweep = DRS4SettingsManager::sharedInstance()->sweepInNanoseconds();
    const double ratio = (double)oldValue/oldSweep;

    if ( !DRS4SettingsManager::sharedInstance()->load("__drs4AutoSave" + EXT_LT_SETTINGS_FILE) )
    {
        MSGBOX("Sorry, No Autosave-File available!");
        m_worker->setBusy(false);
        return;
    }
    else
        m_currentSettingsPath = "__drs4AutoSave" + EXT_LT_SETTINGS_FILE;

    m_worker->setBusy(false);

    setup(oldValue, oldSweep, ratio);
}

void DRS4ScopeDlg::fitCoincidenceData(const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return;

    if ( source == FunctionSource::AccessFromScript ) {
        ui->pushButton_CoincidenceFit->click();
        return;
    }

    m_lifetimeRequestTimer->stop();

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    double yMax = -INT_MAX;

    double xAtYMax = 0;
    double xAtHalfYMax = 0;

    int xVal = 0;

    QVector<int> data = *m_worker->spectrumCoincidence();
    for ( int yVal : data ) //find good startValue for Mean:
    {
        if ( yVal >= yMax )
        {
            yMax = yVal;
            xAtYMax = xVal;
        }

        xVal ++;
    }

    xVal = 0;
    for ( int yVal : data ) //find good startValue for sigma:
    {
        if ( yVal >= yMax/2.0f )
        {
            xAtHalfYMax = xVal;
            break;
        }

        xVal ++;
    }

    double sigmaStart = (xAtYMax-xAtHalfYMax)*2.0f/2.3548; //~sigma
    m_fitPoints.clear();

    const double channelResPs = 1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence()/((double)DRS4SettingsManager::sharedInstance()->channelCntCoincindence());

    QString strReturnValue = "";

    if ( FitEngine::sharedInstance()->runGaussian(data, DRS4SettingsManager::sharedInstance()->fitIterations(), 5, &sigmaStart, &xAtYMax, &yMax, &m_fitPoints, &strReturnValue) )
    {
        ui->widget_ltConicidence->curve().at(1)->clearCurveContent();
        ui->widget_ltConicidence->curve().at(1)->addData(m_fitPoints);

        ui->widget_ltConicidence->replot();

        m_lastFWHMCoincidenceInPs = sigmaStart*channelResPs*2.3548f;
        m_lastt0CoincidenceInPs = xAtYMax*channelResPs - DRS4SettingsManager::sharedInstance()->offsetInNSCoincidence()*1000.0f;

        if ( strReturnValue == PALSFitErrorCodeStringBuilder::errorString(1) )
            m_lastFitResultCoincidence = 0;
        else
            m_lastFitResultCoincidence = 1;

        ui->label_CoincidenceFit->setText(QString::number(sigmaStart*channelResPs, 'f', 3) + " (" + QString::number(m_lastFWHMCoincidenceInPs, 'f', 3) + ")");
        ui->label_meanFitCoincidence->setText(QString::number(m_lastt0CoincidenceInPs, 'f', 3));

        ui->label_fitReturnValue->setStyleSheet("color: green");
        ui->label_fitReturnValue->setText(strReturnValue);
    }
    else
    {
        m_lastFitResultCoincidence = -1;

        m_lastFWHMCoincidenceInPs = 0.0f;
        m_lastt0CoincidenceInPs = 0.0f;

        m_fitPoints.clear();
        ui->widget_ltConicidence->curve().at(1)->clearCurveContent();
        ui->widget_ltConicidence->replot();

        ui->label_CoincidenceFit->setText("Fit aborted!");
        ui->label_meanFitCoincidence->setText("Fit Aborted");

        ui->label_fitReturnValue->setStyleSheet("color: red");
        ui->label_fitReturnValue->setText(strReturnValue);
    }

    m_worker->setBusy(false);

    m_lifetimeRequestTimer->start();
}

void DRS4ScopeDlg::fitMergedData(const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return;

    if ( source == FunctionSource::AccessFromScript ) {
        ui->pushButton_MergedFit->click();
        return;
    }

    m_lifetimeRequestTimer->stop();

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    double yMax = -INT_MAX;

    double xAtYMax = 0;
    double xAtHalfYMax = 0;
    double xAtYMaxAnd20Perc = 0;
    double xAt5Perc = 0;

    QVector<int> dataAdaptedToLeftSide;

    double bkgrd = 0;

    int xVal = 0;
    int bkrgdAvg = 0;

    QVector<int> data = *m_worker->spectrumMerged();
    for ( int yVal : data ) //find good startValue for Mean:
    {
        if ( yVal >= yMax )
        {
            yMax = yVal;
            xAtYMax = xVal;
        }

        if ( yVal < yMax && yVal >= 0.98*yMax )
        {
            xAtYMaxAnd20Perc = xVal;
        }

        if ( /*xVal < m_lifeTimeDataAB.size() && xVal >= m_lifeTimeDataAB.size() - 100*/xVal >= 0 && xVal < 40 )
        {
            bkgrd += yVal;
            bkrgdAvg ++;
        }

        xVal ++;
    }

    bkgrd /= (double)bkrgdAvg;

    xVal = 0;
    for ( int yVal : data ) //find good startValue for sigma:
    {
        if ( yVal >= yMax/2.0f )
        {
            xAtHalfYMax = xVal;
            break;
        }

        xVal ++;
    }

    if ( xAtYMaxAnd20Perc < 0 || xAtYMax <= 0 )
    {
        m_fitPointsMerged_Single.clear();

        m_worker->setBusy(false);
        m_lifetimeRequestTimer->start();
        return;
    }

    if ( (int)xAt5Perc >= (int)xAtYMaxAnd20Perc )
    {
        m_fitPointsMerged_Single.clear();

        m_worker->setBusy(false);
        m_lifetimeRequestTimer->start();
        return;
    }

    for ( int i = 0 ; i < (int)xAtYMaxAnd20Perc ; ++ i )
        dataAdaptedToLeftSide.append(data.at(i));

    /*for ( int i = 0 ; i <= (int)xAtYMax ; ++ i )
        dataAdaptedToLeftSide.append(m_lifeTimeDataMerged.at(i));

    for ( int i = (int)xAtYMax - 1 ; i >= 0 ; -- i )
        dataAdaptedToLeftSide.append(m_lifeTimeDataMerged.at(i));*/

    double sigmaStart = (xAtYMax-xAtHalfYMax)*2.0f/2.3548; //~sigma
    m_fitPointsMerged_Single.clear();

    const double channelResPs = 1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSMerged()/((double)DRS4SettingsManager::sharedInstance()->channelCntMerged());

    QString strReturnValue = "";

    if ( FitEngine::sharedInstance()->runGaussianWithBkgrd(dataAdaptedToLeftSide, DRS4SettingsManager::sharedInstance()->fitIterationsMerged(), 5, &sigmaStart, &xAtYMax, &yMax, &bkgrd, &m_fitPointsMerged_Single, &strReturnValue) )
    {
        ui->widget_ltMerged->curve().at(1)->clearCurveContent();
        ui->widget_ltMerged->curve().at(1)->addData(m_fitPointsMerged_Single);

        ui->widget_ltMerged->replot();

        m_lastFWHMMergedInPs = sigmaStart*channelResPs*2.3548f;
        m_lastt0MergedInPs = xAtYMax*channelResPs - DRS4SettingsManager::sharedInstance()->offsetInNSMerged()*1000.0f;

        if ( strReturnValue == PALSFitErrorCodeStringBuilder::errorString(1) )
            m_lastFitResultMerged = 0;
        else
            m_lastFitResultMerged = 1;

        ui->label_MergedFitFWHM->setText(QString::number(sigmaStart*channelResPs, 'f', 3) + " (" + QString::number(m_lastFWHMMergedInPs, 'f', 3) + ")");
        ui->label_meanFitMerged->setText(QString::number(m_lastt0MergedInPs, 'f', 3));

        ui->label_fitReturnValue_Merged->setStyleSheet("color: green");
        ui->label_fitReturnValue_Merged->setText(strReturnValue);
    }
    else
    {
        m_lastFitResultMerged = -1;

        m_lastFWHMMergedInPs = 0.0f;
        m_lastt0MergedInPs = 0.0f;

        m_fitPointsMerged_Single.clear();
        ui->widget_ltMerged->curve().at(1)->clearCurveContent();
        ui->widget_ltMerged->replot();

        ui->label_MergedFitFWHM->setText("Fit aborted!");
        ui->label_meanFitMerged->setText("Fit Aborted");

        ui->label_fitReturnValue_Merged->setStyleSheet("color: red");
        ui->label_fitReturnValue_Merged->setText(strReturnValue);
    }

    m_worker->setBusy(false);

    m_lifetimeRequestTimer->start();
}

void DRS4ScopeDlg::fitABData(const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return;

    if ( source == FunctionSource::AccessFromScript ) {
        ui->pushButton_ABFit->click();
        return;
    }

    m_lifetimeRequestTimer->stop();

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    double yMax = -INT_MAX;

    double xAtYMax = 0;
    double xAtHalfYMax = 0;
    double xAtYMaxAnd20Perc = 0;
    double xAt5Perc = 0;

    QVector<int> dataAdaptedToLeftSide;

    double bkgrd = 0;

    int xVal = 0;
    int bkrgdAvg = 0;

    QVector<int> data = *m_worker->spectrumAB();
    for ( int yVal : data ) //find good startValue for Mean:
    {
        if ( yVal >= yMax )
        {
            yMax = yVal;
            xAtYMax = xVal;
        }

        if ( yVal < yMax && yVal >= 0.98*yMax )
        {
            xAtYMaxAnd20Perc = xVal;
        }

        if ( /*xVal < m_lifeTimeDataAB.size() && xVal >= m_lifeTimeDataAB.size() - 100*/xVal >= 0 && xVal < 40 )
        {
            bkgrd += yVal;
            bkrgdAvg ++;
        }

        xVal ++;
    }

    bkgrd /= (double)bkrgdAvg;

    xVal = 0;
    for ( int yVal : data ) //find good startValue for sigma:
    {
        if ( yVal >= yMax/2.0f )
        {
            xAtHalfYMax = xVal;
            break;
        }

        xVal ++;
    }

    if ( xAtYMaxAnd20Perc < 0 || xAtYMax <= 0 )
    {
        m_fitPointsAB_Single.clear();

        m_worker->setBusy(false);
        m_lifetimeRequestTimer->start();
        return;
    }

    if ( (int)xAt5Perc >= (int)xAtYMaxAnd20Perc )
    {
        m_fitPointsAB_Single.clear();

        m_worker->setBusy(false);
        m_lifetimeRequestTimer->start();
        return;
    }

    for ( int i = 0 ; i < (int)xAtYMaxAnd20Perc ; ++ i )
        dataAdaptedToLeftSide.append(data.at(i));

    /*for ( int i = 0 ; i <= (int)xAtYMax ; ++ i )
        dataAdaptedToLeftSide.append(m_lifeTimeDataAB.at(i));

    for ( int i = (int)xAtYMax - 1 ; i >= 0 ; -- i )
        dataAdaptedToLeftSide.append(m_lifeTimeDataAB.at(i));*/

    double sigmaStart = (xAtYMax-xAtHalfYMax)*2.0f/2.3548; //~sigma
    m_fitPointsAB_Single.clear();

    const double channelResPs = 1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSAB()/((double)DRS4SettingsManager::sharedInstance()->channelCntAB());

    QString strReturnValue = "";

    if ( FitEngine::sharedInstance()->runGaussianWithBkgrd(dataAdaptedToLeftSide, DRS4SettingsManager::sharedInstance()->fitIterationsAB(), 5, &sigmaStart, &xAtYMax, &yMax, &bkgrd, &m_fitPointsAB_Single, &strReturnValue) )
    {
        ui->widget_ltAB->curve().at(1)->clearCurveContent();
        ui->widget_ltAB->curve().at(1)->addData(m_fitPointsAB_Single);

        ui->widget_ltAB->replot();

        m_lastFWHMABInPs = sigmaStart*channelResPs*2.3548f;
        m_lastt0ABInPs = xAtYMax*channelResPs - DRS4SettingsManager::sharedInstance()->offsetInNSAB()*1000.0f;

        if ( strReturnValue == PALSFitErrorCodeStringBuilder::errorString(1) )
            m_lastFitResultAB = 0;
        else
            m_lastFitResultAB = 1;

        ui->label_ABFitFWHM->setText(QString::number(sigmaStart*channelResPs, 'f', 3) + " (" + QString::number(m_lastFWHMABInPs, 'f', 3) + ")");
        ui->label_meanFitAB->setText(QString::number(m_lastt0ABInPs, 'f', 3));

        ui->label_fitReturnValue_AB->setStyleSheet("color: green");
        ui->label_fitReturnValue_AB->setText(strReturnValue);
    }
    else
    {
        m_lastFitResultAB = -1;

        m_lastFWHMABInPs = 0.0f;
        m_lastt0ABInPs = 0.0f;

        m_fitPointsAB_Single.clear();
        ui->widget_ltAB->curve().at(1)->clearCurveContent();
        ui->widget_ltAB->replot();

        ui->label_ABFitFWHM->setText("Fit aborted!");
        ui->label_meanFitAB->setText("Fit Aborted");

        ui->label_fitReturnValue_AB->setStyleSheet("color: red");
        ui->label_fitReturnValue_AB->setText(strReturnValue);
    }

    m_worker->setBusy(false);

    m_lifetimeRequestTimer->start();
}

void DRS4ScopeDlg::fitBAData(const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return;

    if ( source == FunctionSource::AccessFromScript ) {
        ui->pushButton_BAFit->click();
        return;
    }

    m_lifetimeRequestTimer->stop();

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    double yMax = -INT_MAX;

    double xAtYMax = 0;
    double xAtHalfYMax = 0;
    double xAtYMaxAnd20Perc = 0;
    double xAt5Perc = 0;

    QVector<int> dataAdaptedToLeftSide;

    double bkgrd = 0;

    int xVal = 0;
    int bkrgdAvg = 0;

    QVector<int> data = *m_worker->spectrumBA();
    for ( int yVal : data ) //find good startValue for Mean:
    {
        if ( yVal >= yMax )
        {
            yMax = yVal;
            xAtYMax = xVal;
        }

        if ( yVal < yMax && yVal >= 0.98*yMax )
        {
            xAtYMaxAnd20Perc = xVal;
        }

        if ( /*xVal < m_lifeTimeDataAB.size() && xVal >= m_lifeTimeDataAB.size() - 100*/xVal >= 0 && xVal < 40 )
        {
            bkgrd += yVal;
            bkrgdAvg ++;
        }

        xVal ++;
    }

    bkgrd /= (double)bkrgdAvg;

    xVal = 0;
    for ( int yVal : data ) //find good startValue for sigma:
    {
        if ( yVal >= yMax/2.0f )
        {
            xAtHalfYMax = xVal;
            break;
        }

        xVal ++;
    }

    if ( xAtYMaxAnd20Perc < 0 || xAtYMax <= 0 )
    {
        m_fitPointsBA_Single.clear();

        m_worker->setBusy(false);
        m_lifetimeRequestTimer->start();
        return;
    }

    if ( (int)xAt5Perc >= (int)xAtYMaxAnd20Perc )
    {
        m_fitPointsBA_Single.clear();

        m_worker->setBusy(false);
        m_lifetimeRequestTimer->start();
        return;
    }

    for ( int i = 0 ; i < (int)xAtYMaxAnd20Perc ; ++ i )
        dataAdaptedToLeftSide.append(data.at(i));

    /*for ( int i = 0 ; i <= (int)xAtYMax ; ++ i )
        dataAdaptedToLeftSide.append(m_lifeTimeDataBA.at(i));

    for ( int i = (int)xAtYMax - 1 ; i >= 0 ; -- i )
        dataAdaptedToLeftSide.append(m_lifeTimeDataBA.at(i));*/

    double sigmaStart = (xAtYMax-xAtHalfYMax)*2.0f/2.3548; //~sigma
    m_fitPointsBA_Single.clear();

    const double channelResPs = 1000.0f*DRS4SettingsManager::sharedInstance()->scalerInNSBA()/((double)DRS4SettingsManager::sharedInstance()->channelCntBA());

    QString strReturnValue = "";

    if ( FitEngine::sharedInstance()->runGaussianWithBkgrd(dataAdaptedToLeftSide, DRS4SettingsManager::sharedInstance()->fitIterationsBA(), 5, &sigmaStart, &xAtYMax, &yMax, &bkgrd, &m_fitPointsBA_Single, &strReturnValue) )
    {
        ui->widget_ltBA->curve().at(1)->clearCurveContent();
        ui->widget_ltBA->curve().at(1)->addData(m_fitPointsBA_Single);

        ui->widget_ltBA->replot();

        m_lastFWHMBAInPs = sigmaStart*channelResPs*2.3548f;
        m_lastt0BAInPs = xAtYMax*channelResPs - DRS4SettingsManager::sharedInstance()->offsetInNSBA()*1000.0f;

        if ( strReturnValue == PALSFitErrorCodeStringBuilder::errorString(1) )
            m_lastFitResultBA = 0;
        else
            m_lastFitResultBA = 1;

        ui->label_BAFitFWHM->setText(QString::number(sigmaStart*channelResPs, 'f', 3) + " (" + QString::number(m_lastFWHMBAInPs, 'f', 3) + ")");
        ui->label_meanFitBA->setText(QString::number(m_lastt0BAInPs, 'f', 3));

        ui->label_fitReturnValue_BA->setStyleSheet("color: green");
        ui->label_fitReturnValue_BA->setText(strReturnValue);
    }
    else
    {
        m_lastFitResultBA = -1;

        m_lastFWHMBAInPs = 0.0f;
        m_lastt0BAInPs = 0.0f;

        m_fitPointsBA_Single.clear();
        ui->widget_ltBA->curve().at(1)->clearCurveContent();
        ui->widget_ltBA->replot();

        ui->label_BAFitFWHM->setText("Fit aborted!");
        ui->label_meanFitBA->setText("Fit Aborted");

        ui->label_fitReturnValue_BA->setStyleSheet("color: red");
        ui->label_fitReturnValue_BA->setText(strReturnValue);
    }

    m_worker->setBusy(false);

    m_lifetimeRequestTimer->start();
}

double DRS4ScopeDlg::lastFWHMABInPs() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastFWHMABInPs;
}

double DRS4ScopeDlg::lastFWHMBAInPs() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastFWHMBAInPs;
}

double DRS4ScopeDlg::lastFWHMMergedInPs() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastFWHMMergedInPs;
}

double DRS4ScopeDlg::lastFWHMCoincidenceInPs() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastFWHMCoincidenceInPs;
}

double DRS4ScopeDlg::lastt0ABInPs() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastt0ABInPs;
}

double DRS4ScopeDlg::lastt0BAInPs() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastt0BAInPs;
}

double DRS4ScopeDlg::lastt0MergedInPs() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastt0MergedInPs;
}

double DRS4ScopeDlg::lastt0CoincidenceInPs() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastt0CoincidenceInPs;
}

int DRS4ScopeDlg::lastFitResultAB() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastFitResultAB;
}

int DRS4ScopeDlg::lastFitResultBA() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastFitResultBA;
}

int DRS4ScopeDlg::lastFitResultMerged() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastFitResultMerged;
}

int DRS4ScopeDlg::lastFitResultCoincidence() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastFitResultCoincidence;
}

double DRS4ScopeDlg::lastBoardTemperature() const
{
    QMutexLocker locker(&m_mutex);

    return m_lastTemperatureInDegree;
}

int DRS4ScopeDlg::countsOfABSpectrum() const
{
    if (!m_worker)
        return -1;

    //QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const int val = m_worker->countsSpectrumAB();

    m_worker->setBusy(false);

    return val;
}

int DRS4ScopeDlg::countsOfBASpectrum() const
{
    if (!m_worker)
        return -1;

    //QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const int val = m_worker->countsSpectrumBA();

    m_worker->setBusy(false);

    return val;
}

int DRS4ScopeDlg::countsOfMergedSpectrum() const
{
    if (!m_worker)
        return -1;

    //QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const int val = m_worker->countsSpectrumMerged();

    m_worker->setBusy(false);

    return val;
}

int DRS4ScopeDlg::countsOfCoincidenceSpectrum() const
{
    if (!m_worker)
        return -1;

    //QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const int val = m_worker->countsSpectrumCoincidence();

    m_worker->setBusy(false);

    return val;
}

void DRS4ScopeDlg::clearCoincidenceFitData()
{
    m_lastFitResultCoincidence = -1;

    m_lastFWHMCoincidenceInPs = 0.0f;
    m_lastt0CoincidenceInPs = 0.0f;

    m_fitPoints.clear();
    ui->widget_ltConicidence->curve().at(1)->clearCurveContent();
    ui->widget_ltConicidence->replot();

    ui->label_CoincidenceFit->setText("0.0");
    ui->label_meanFitCoincidence->setText("0.0");
}

void DRS4ScopeDlg::clearMergedFitData()
{
    m_lastFitResultMerged = -1;

    m_lastFWHMMergedInPs = 0.0f;
    m_lastt0MergedInPs = 0.0f;

    m_fitPointsMerged_Single.clear();
    ui->widget_ltMerged->curve().at(1)->clearCurveContent();
    ui->widget_ltMerged->replot();

    ui->label_MergedFitFWHM->setText("0.0");
    ui->label_meanFitMerged->setText("0.0");
}

void DRS4ScopeDlg::clearABFitData()
{
    m_lastFitResultAB = -1;

    m_lastFWHMABInPs = 0.0f;
    m_lastt0ABInPs = 0.0f;

    m_fitPointsAB_Single.clear();
    ui->widget_ltAB->curve().at(1)->clearCurveContent();
    ui->widget_ltAB->replot();

    ui->label_ABFitFWHM->setText("0.0");
    ui->label_meanFitAB->setText("0.0");
}

void DRS4ScopeDlg::clearBAFitData()
{
    m_lastFitResultBA = -1;

    m_lastFWHMBAInPs = 0.0f;
    m_lastt0BAInPs = 0.0f;

    m_fitPointsBA_Single.clear();
    ui->widget_ltBA->curve().at(1)->clearCurveContent();
    ui->widget_ltBA->replot();

    ui->label_BAFitFWHM->setText("0.0");
    ui->label_meanFitBA->setText("0.0");
}

void DRS4ScopeDlg::showAddInfoBox()
{
    m_addInfoDlg->show();
}

void DRS4ScopeDlg::showBoardInfoBox()
{
    m_boardInfoDlg->show();
}

void DRS4ScopeDlg::showScriptBox()
{
    m_scriptDlg->show();
}

void DRS4ScopeDlg::showAboutBox()
{
    SHOW_ABOUT();
}

void DRS4ScopeDlg::showSavePulses()
{
    m_pulseSaveDlg->show();
}

void DRS4ScopeDlg::showSavePulsesRange()
{
    m_pulseSaveRangeDlg->show();
}

void DRS4ScopeDlg::showCalculator()
{
    m_calculatorDlg->show();
}

void DRS4ScopeDlg::showGPL()
{
    m_gplDialog->show();
}

void DRS4ScopeDlg::showLGPL()
{
    m_lgplDialog->show();
}

void DRS4ScopeDlg::showUsedGPL()
{
    m_usedgplDialog->show();
}

void DRS4ScopeDlg::showServerConfig()
{
    m_serverDlg->show();
}

void DRS4ScopeDlg::showRCServerConfig()
{
    m_rcServerDlg->show();
}

void DRS4ScopeDlg::changeServerState(bool active)
{
    if (sender() == qobject_cast<QObject*>(DRS4WebServer::sharedInstance())) {
        QString msg = "";

        if (active) {
            msg = QString("http server is active (listening to port %1)").arg(DRS4ProgramSettingsManager::sharedInstance()->httpServerPort());
        }
        else {
            msg = QString("http server is not activated ...");
        }

        ui->widget_serverState->setActive(active, msg);
    }
    else if (sender() == qobject_cast<QObject*>(DRS4RemoteControlServer::sharedInstance())) {
        QString msg = "";

        if (active) {
            msg = QString("remote control server is active (listening to port %1)").arg(DRS4ProgramSettingsManager::sharedInstance()->remoteControlServerPort());
        }
        else {
            msg = QString("remote control server is not activated ...");
        }

        ui->widget_rcServerState->setActive(active, msg);
    }
}

void DRS4ScopeDlg::updateInBurstMode()
{
    QMutexLocker locker(&m_mutex);

    if (!m_worker)
        return;

    QVector<QPointF> phsAListStart, phsAListStop, phsBListStart, phsBListStop;
    QVector<QPointF> phsA, phsB;

    int yMaxA = -INT_MAX;
    int yMaxB = -INT_MAX;

    m_burstModeTimer->stop();

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const double countABHz = m_worker->currentLifetimeABCountRateInHz();
    const double avgCountABHz = m_worker->avgLifetimeABCountRateInHz();

    const double countBAHz = m_worker->currentLifetimeBACountRateInHz();
    const double avgCountBAHz = m_worker->avgLifetimeBACountRateInHz();

    const double countMergedHz = m_worker->currentLifetimeMergedCountRateInHz();
    const double avgCountMergedHz = m_worker->avgLifetimeMergedCountRateInHz();

    const double countCoincidenceHz = m_worker->currentLifetimeCoincidenceCountRateInHz();
    const double avgCountCoincidenceHz = m_worker->avgLifetimeCoincidenceCountRateInHz();

    const int yABMax = m_worker->maxYValueABSpectrum();
    const int yBAMax = m_worker->maxYValueBASpectrum();
    const int yMergedMax = m_worker->maxYValueMergedSpectrum();
    const int yCoincidenceMax = m_worker->maxYValueCoincidenceSpectrum();

    const int abCounts = m_worker->countsSpectrumAB();
    const int baCounts = m_worker->countsSpectrumBA();
    const int mergedCounts = m_worker->countsSpectrumMerged();
    const int coincidenceCounts = m_worker->countsSpectrumCoincidence();

    const int cntA = m_worker->phsACounts();
    const int cntB = m_worker->phsBCounts();

    int cntAStart = 0, cntAStop = 0;
    int cntBStart = 0, cntBStop = 0;

    for ( int i = 0 ; i < kNumberOfBins ; ++ i ) {
        const int yA = (*m_worker->phsA())[i];
        const int yB = (*m_worker->phsB())[i];

        const QPointF valueA(i, yA);
        const QPointF valueB(i, yB);

        phsA.append(valueA);
        phsB.append(valueB);

        if ( i <= DRS4SettingsManager::sharedInstance()->startChanneAMax()
             && i >= DRS4SettingsManager::sharedInstance()->startChanneAMin() ) {
            cntAStart += yA;
            phsAListStart.append(valueA);
        }

        if ( i <= DRS4SettingsManager::sharedInstance()->startChanneBMax()
             && i >= DRS4SettingsManager::sharedInstance()->startChanneBMin() ) {
            cntBStart += yB;
            phsBListStart.append(valueB);
        }

        if ( i <= DRS4SettingsManager::sharedInstance()->stopChanneAMax()
             && i >= DRS4SettingsManager::sharedInstance()->stopChanneAMin() ) {
            cntAStop += yA;
            phsAListStop.append(valueA);
        }

        if ( i <= DRS4SettingsManager::sharedInstance()->stopChanneBMax()
             && i >= DRS4SettingsManager::sharedInstance()->stopChanneBMin() ) {
            cntBStop += yB;
            phsBListStop.append(valueB);
        }

        if ( i >= 30 ) { // <- spurious values?
            yMaxA = qMax(yMaxA, yA);
            yMaxB = qMax(yMaxB, yB);
        }
    }

    m_worker->setBusy(false);

    ui->label_countsIntergralAB->setNum(abCounts);
    ui->label_countsIntergralBA->setNum(baCounts);
    ui->label_countsIntergralMerged->setNum(mergedCounts);
    ui->label_countsIntergralCoincidence->setNum(coincidenceCounts);

    ui->label_phsACounts->setNum(cntA);
    ui->label_phsBCounts->setNum(cntB);

    ui->label_countsStartA->setNum(cntAStart);
    ui->label_countsStartB->setNum(cntBStart);
    ui->label_countsStopA->setNum(cntAStop);
    ui->label_countsStopB->setNum(cntBStop);

    ui->label_valiLTPerSec->setText("Lifetime Efficiency\t[Hz]:\t[A-B] " + QString::number(avgCountABHz, 'f', 2) + " (" + QString::number(countABHz, 'f', 2) + ") [B-A] " + QString::number(avgCountBAHz, 'f', 2) + " (" + QString::number(countBAHz, 'f', 2) + ") [Merged] " + QString::number(avgCountMergedHz, 'f', 2) + " (" + QString::number(countMergedHz, 'f', 2) + ")");
    ui->label_validCoincidencePerSec->setText("Prompt Efficiency\t[Hz]:\t" + QString::number(avgCountCoincidenceHz, 'f', 2) + " (" + QString::number(countCoincidenceHz, 'f', 2) + ")" );

    ui->label_phsCntPerSecA->setText("Sample Rate [Hz]: " + QString::number(m_worker->avgPulseCountRateInHz(), 'f', 2) + " (" + QString::number(m_worker->currentPulseCountRateInHz(), 'f', 2) + ")" );

    m_burstModeTimer->start();
}

bool DRS4ScopeDlg::isAcquisitionStopped() const
{
    QMutexLocker locker(&m_mutex);

    return !m_workerThread->isRunning();
}

bool DRS4ScopeDlg::isPulseGenerationFromDataStreamRunning() const
{
    QMutexLocker locker(&m_mutex);

    return DRS4StreamDataLoader::sharedInstance()->isArmed();
}

bool DRS4ScopeDlg::isDataStreamArmed() const
{
    QMutexLocker locker(&m_mutex);

    return DRS4StreamManager::sharedInstance()->isArmed();
}

bool DRS4ScopeDlg::isBurstModeRunning() const
{
    QMutexLocker locker(&m_mutex);

    return DRS4SettingsManager::sharedInstance()->isBurstMode();
}

bool DRS4ScopeDlg::isMulticoreThreadingEnabled() const
{
    QMutexLocker locker(&m_mutex);

    return DRS4ProgramSettingsManager::sharedInstance()->isMulticoreThreadingEnabled();
}

void DRS4ScopeDlg::changeChannels_A(int set_chn) {
    DRS4SettingsManager::sharedInstance()->setChannelNumberA(set_chn);

    changeTriggerLogic(ui->comboBox_triggerLogic->currentIndex());
    changeTriggerLevelA(DRS4SettingsManager::sharedInstance()->triggerLevelAmV());
}

void DRS4ScopeDlg::changeChannels_B(int set_chn)
{
    DRS4SettingsManager::sharedInstance()->setChannelNumberB(set_chn);

    changeTriggerLogic(ui->comboBox_triggerLogic->currentIndex());
    changeTriggerLevelB(DRS4SettingsManager::sharedInstance()->triggerLevelBmV());
}

void DRS4ScopeDlg::changeChannel1_A(bool on)
{
    if (on) {
        changeChannels_A(0);

        ui->checkBox_chn1_A->setChecked(on);
        ui->checkBox_chn2_A->setChecked(!on);
        ui->checkBox_chn3_A->setChecked(!on);
        ui->checkBox_chn4_A->setChecked(!on);

        ui->checkBox_chn1_A->setDisabled(on);
        ui->checkBox_chn2_A->setDisabled(!on);
        ui->checkBox_chn3_A->setDisabled(!on);
        ui->checkBox_chn4_A->setDisabled(!on);
    }
}

void DRS4ScopeDlg::changeChannel2_A(bool on)
{
    if (on) {
        changeChannels_A(1);

        ui->checkBox_chn2_A->setChecked(on);
        ui->checkBox_chn1_A->setChecked(!on);
        ui->checkBox_chn3_A->setChecked(!on);
        ui->checkBox_chn4_A->setChecked(!on);

        ui->checkBox_chn2_A->setDisabled(on);
        ui->checkBox_chn1_A->setDisabled(!on);
        ui->checkBox_chn3_A->setDisabled(!on);
        ui->checkBox_chn4_A->setDisabled(!on);
    }
}

void DRS4ScopeDlg::changeChannel3_A(bool on)
{
    if (on) {
        changeChannels_A(2);

        ui->checkBox_chn3_A->setChecked(on);
        ui->checkBox_chn1_A->setChecked(!on);
        ui->checkBox_chn2_A->setChecked(!on);
        ui->checkBox_chn4_A->setChecked(!on);

        ui->checkBox_chn3_A->setDisabled(on);
        ui->checkBox_chn1_A->setDisabled(!on);
        ui->checkBox_chn2_A->setDisabled(!on);
        ui->checkBox_chn4_A->setDisabled(!on);
    }
}

void DRS4ScopeDlg::changeChannel4_A(bool on)
{
    if (on) {
        changeChannels_A(3);

        ui->checkBox_chn4_A->setChecked(on);
        ui->checkBox_chn1_A->setChecked(!on);
        ui->checkBox_chn2_A->setChecked(!on);
        ui->checkBox_chn3_A->setChecked(!on);

        ui->checkBox_chn4_A->setDisabled(on);
        ui->checkBox_chn1_A->setDisabled(!on);
        ui->checkBox_chn2_A->setDisabled(!on);
        ui->checkBox_chn3_A->setDisabled(!on);
    }
}

void DRS4ScopeDlg::changeChannel1_B(bool on)
{
    if (on) {
        changeChannels_B(0);

        ui->checkBox_chn1_B->setChecked(on);
        ui->checkBox_chn2_B->setChecked(!on);
        ui->checkBox_chn3_B->setChecked(!on);
        ui->checkBox_chn4_B->setChecked(!on);

        ui->checkBox_chn1_B->setDisabled(on);
        ui->checkBox_chn2_B->setDisabled(!on);
        ui->checkBox_chn3_B->setDisabled(!on);
        ui->checkBox_chn4_B->setDisabled(!on);
    }
}

void DRS4ScopeDlg::changeChannel2_B(bool on)
{
    if (on) {
        changeChannels_B(1);

        ui->checkBox_chn2_B->setChecked(on);
        ui->checkBox_chn1_B->setChecked(!on);
        ui->checkBox_chn3_B->setChecked(!on);
        ui->checkBox_chn4_B->setChecked(!on);

        ui->checkBox_chn2_B->setDisabled(on);
        ui->checkBox_chn1_B->setDisabled(!on);
        ui->checkBox_chn3_B->setDisabled(!on);
        ui->checkBox_chn4_B->setDisabled(!on);
    }
}

void DRS4ScopeDlg::changeChannel3_B(bool on)
{
    if (on) {
        changeChannels_B(2);

        ui->checkBox_chn3_B->setChecked(on);
        ui->checkBox_chn1_B->setChecked(!on);
        ui->checkBox_chn2_B->setChecked(!on);
        ui->checkBox_chn4_B->setChecked(!on);

        ui->checkBox_chn3_B->setDisabled(on);
        ui->checkBox_chn1_B->setDisabled(!on);
        ui->checkBox_chn2_B->setDisabled(!on);
        ui->checkBox_chn4_B->setDisabled(!on);
    }
}

void DRS4ScopeDlg::changeChannel4_B(bool on)
{
    if (on) {
        changeChannels_B(3);

        ui->checkBox_chn4_B->setChecked(on);
        ui->checkBox_chn1_B->setChecked(!on);
        ui->checkBox_chn2_B->setChecked(!on);
        ui->checkBox_chn3_B->setChecked(!on);

        ui->checkBox_chn4_B->setDisabled(on);
        ui->checkBox_chn1_B->setDisabled(!on);
        ui->checkBox_chn2_B->setDisabled(!on);
        ui->checkBox_chn3_B->setDisabled(!on);
    }
}

void DRS4ScopeDlg::startAcquisition(const FunctionSource &source)
{
    QMutexLocker locker(&m_mutex);

    DUNUSED_PARAM(source);
    startStopThread();
}

void DRS4ScopeDlg::stopAcquisition(const FunctionSource &source)
{
    DUNUSED_PARAM(source);

    startStopThread();
}

bool DRS4ScopeDlg::isAcquisitionRunning() const
{
    return !isAcquisitionStopped();
}

QString DRS4ScopeDlg::updateCurrentFileLabel(bool accessFromScript)
{
    QString str = "";
    if ( !accessFromScript ) {
        if (  m_currentSettingsPath != NO_SETTINGS_FILE_PLACEHOLDER )
            m_currentSettingsFileLabel->setText(m_currentSettingsPath + " <font color=\"blue\">[" + DRS4SettingsManager::sharedInstance()->lastSaveDate() + "]</font>");
        else
            m_currentSettingsFileLabel->setText(m_currentSettingsPath);

        update();
    }
    else {
        if (  m_currentSettingsPath != NO_SETTINGS_FILE_PLACEHOLDER )
            str = QString(m_currentSettingsPath + " <font color=\"blue\">[" + DRS4SettingsManager::sharedInstance()->lastSaveDate() + "]</font>");
        else
            str = QString(m_currentSettingsPath);
    }

    return str;
}

void DRS4ScopeDlg::plotPulseScope()
{
    QMutexLocker locker(&m_mutex);

    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
        return;

    if (!m_worker)
        return;

    if (!ui->tab->isVisible())
        return;

    if (!m_workerThread->isRunning())
        return;

    m_pulseRequestTimer->stop();

    ui->widget_plotA->curve().at(0)->clearCurveContent();
    ui->widget_plotA->curve().at(1)->clearCurveContent();

    ui->widget_plotB->curve().at(0)->clearCurveContent();
    ui->widget_plotB->curve().at(1)->clearCurveContent();

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    ui->widget_plotA->curve().at(0)->addData(*m_worker->pulseDataA());
    ui->widget_plotA->curve().at(1)->addData(*m_worker->pulseSplineDataA());

    ui->widget_plotB->curve().at(0)->addData(*m_worker->pulseDataB());
    ui->widget_plotB->curve().at(1)->addData(*m_worker->pulseSplineDataB());

    m_worker->setBusy(false);

    ui->widget_plotA->replot();
    ui->widget_plotB->replot();

    m_pulseRequestTimer->start();
}

void DRS4ScopeDlg::plotPHS()
{
    QMutexLocker locker(&m_mutex);

    if ( DRS4SettingsManager::sharedInstance()->isBurstMode() )
        return;

    if (!m_worker)
        return;

    if (!ui->tab_2->isVisible()) {
        m_phsRequestTimer->stop();

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        ui->label_phsCntPerSecA->setText("Sample Rate [Hz]: " + QString::number(m_worker->avgPulseCountRateInHz(), 'f', 2) + " (" + QString::number(m_worker->currentPulseCountRateInHz(), 'f', 2) + ")" );

        m_worker->setBusy(false);

        m_phsRequestTimer->start();

        return;
    }

    m_phsRequestTimer->stop();

    ui->widget_phs_A->curve().at(0)->clearCurveContent();
    ui->widget_phs_A->curve().at(1)->clearCurveContent();
    ui->widget_phs_A->curve().at(6)->clearCurveContent();
    ui->widget_phs_A->curve().at(7)->clearCurveContent();

    ui->widget_phs_B->curve().at(0)->clearCurveContent();
    ui->widget_phs_B->curve().at(1)->clearCurveContent();
    ui->widget_phs_B->curve().at(6)->clearCurveContent();
    ui->widget_phs_B->curve().at(7)->clearCurveContent();

    QVector<QPointF> phsAListStart, phsAListStop, phsBListStart, phsBListStop;
    QVector<QPointF> phsA, phsB;

    int yMaxA = -INT_MAX;
    int yMaxB = -INT_MAX;

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    const int cntA = m_worker->phsACounts();
    const int cntB = m_worker->phsBCounts();

    int cntAStart = 0, cntAStop = 0;
    int cntBStart = 0, cntBStop = 0;

    for ( int i = 0 ; i < kNumberOfBins ; ++ i ) {
        const int yA = (*m_worker->phsA())[i];
        const int yB = (*m_worker->phsB())[i];

        const QPointF valueA(i, yA);
        const QPointF valueB(i, yB);

        phsA.append(valueA);
        phsB.append(valueB);

        if ( i <= DRS4SettingsManager::sharedInstance()->startChanneAMax()
             && i >= DRS4SettingsManager::sharedInstance()->startChanneAMin() ) {
            cntAStart += yA;
            phsAListStart.append(valueA);
        }

        if ( i <= DRS4SettingsManager::sharedInstance()->startChanneBMax()
             && i >= DRS4SettingsManager::sharedInstance()->startChanneBMin() ) {
            cntBStart += yB;
            phsBListStart.append(valueB);
        }

        if ( i <= DRS4SettingsManager::sharedInstance()->stopChanneAMax()
             && i >= DRS4SettingsManager::sharedInstance()->stopChanneAMin() ) {
            cntAStop += yA;
            phsAListStop.append(valueA);
        }

        if ( i <= DRS4SettingsManager::sharedInstance()->stopChanneBMax()
             && i >= DRS4SettingsManager::sharedInstance()->stopChanneBMin() ) {
            cntBStop += yB;
            phsBListStop.append(valueB);
        }

        if ( i >= 30 ) { // <- spurious values?
            yMaxA = qMax(yMaxA, yA);
            yMaxB = qMax(yMaxB, yB);
        }
    }

    ui->label_phsCntPerSecA->setText("Sample Rate [Hz]: " + QString::number(m_worker->avgPulseCountRateInHz(), 'f', 2) + " (" + QString::number(m_worker->currentPulseCountRateInHz(), 'f', 2) + ")" );

    m_worker->setBusy(false);

    ui->widget_phs_A->yLeft()->setAxisRange(0, yMaxA);
    ui->widget_phs_B->yLeft()->setAxisRange(0, yMaxB);

    ui->widget_phs_A->curve().at(1)->addData(phsA);
    ui->widget_phs_A->curve().at(6)->addData(phsAListStart);
    ui->widget_phs_A->curve().at(7)->addData(phsAListStop);

    ui->widget_phs_B->curve().at(1)->addData(phsB);
    ui->widget_phs_B->curve().at(6)->addData(phsBListStart);
    ui->widget_phs_B->curve().at(7)->addData(phsBListStop);

    plotPHSWindows();

    ui->label_phsACounts->setNum(cntA);
    ui->label_phsBCounts->setNum(cntB);

    ui->label_countsStartA->setNum(cntAStart);
    ui->label_countsStartB->setNum(cntBStart);
    ui->label_countsStopA->setNum(cntAStop);
    ui->label_countsStopB->setNum(cntBStop);

    m_phsRequestTimer->start();
}

void DRS4ScopeDlg::showEvent(QShowEvent *event)
{
    event->accept();
    QMainWindow::showEvent(event);
}

void DRS4ScopeDlg::closeEvent(QCloseEvent *event)
{
    if (m_bConnectionLost) {
        event->ignore();
        return;
    }

    const QMessageBox::StandardButton reply = QMessageBox::question(this, "Quit Program?", "Exit Program?", QMessageBox::Yes|QMessageBox::No);

    if (reply != QMessageBox::Yes) {
        event->ignore();
        return;
    }

    if ( m_worker->isRunning() ) {
        MSGBOX("Please stop the acquisition or simulation before closing the software!");
        event->ignore();
        return;
    }

    if (m_addInfoDlg)
        m_addInfoDlg->close();

    if (m_scriptDlg)
        m_scriptDlg->close();

    if (m_pulseSaveDlg)
        m_pulseSaveDlg->close();

    if (m_pulseSaveRangeDlg)
        m_pulseSaveRangeDlg->close();

    if (m_calculatorDlg)
        m_calculatorDlg->close();

    if (m_boardInfoDlg)
        m_boardInfoDlg->close();

    if (m_gplDialog)
        m_gplDialog->close();

    if (m_lgplDialog)
        m_lgplDialog->close();

    if (m_usedgplDialog)
        m_usedgplDialog->close();

    if (m_serverDlg)
        m_serverDlg->close();

    if (m_rcServerDlg)
        m_rcServerDlg->close();

    if (DRS4WebServer::sharedInstance()->isListening())
        DRS4WebServer::sharedInstance()->stop();

    if (DRS4RemoteControlServer::sharedInstance()->isListening())
        DRS4RemoteControlServer::sharedInstance()->stop();

    if ( !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() )
        DRS4BoardManager::sharedInstance()->currentBoard()->SetCooldown(10000);

    if ( DRS4StreamManager::sharedInstance()->isArmed() )
        DRS4StreamManager::sharedInstance()->stopAndSave();

    if ( DRS4FalseTruePulseStreamManager::sharedInstance()->isArmed() )
        DRS4FalseTruePulseStreamManager::sharedInstance()->stopAndSave();

    if ( DRS4StreamDataLoader::sharedInstance()->isArmed() )
        DRS4StreamDataLoader::sharedInstance()->stop();

    if ( DRS4TextFileStreamManager::sharedInstance()->isArmed() )
        DRS4TextFileStreamManager::sharedInstance()->abort();

    if ( DRS4TextFileStreamRangeManager::sharedInstance()->isArmed() )
        DRS4TextFileStreamRangeManager::sharedInstance()->abort();

    event->accept();
    QMainWindow::closeEvent(event);
}

void DRS4ScopeDlg::arrangeIcons()
{
    ui->action_StartDAQStreaming->setIcon(QIcon(":/images/images/play_click.svg"));
    ui->actionStop_Running->setIcon(QIcon(":/images/images/stop_hover.svg"));

    ui->actionStart_True_False_Pulse_Streaming->setIcon(QIcon(":/images/images/play_click.svg"));
    ui->actionStop_True_False_Pulse_Streaming->setIcon(QIcon(":/images/images/stop_hover.svg"));

    ui->actionSave_next_N_Pulses->setIcon(QIcon(":/images/images/001-heart-rate-monitor.png"));
    ui->actionSave_next_N_Pulses_in_Range->setIcon(QIcon(":/images/images/001-heart-rate-monitor.png"));

    ui->actionLoad->setIcon(QIcon(":/images/images/007-folder-3.png"));
    ui->actionSave->setIcon(QIcon(":/images/images/008-folder-2.png"));
    ui->actionSave_as->setIcon(QIcon(":/images/images/008-folder-2.png"));
    ui->actionLoad_Autosave->setIcon(QIcon(":/images/images/008-folder-2.png"));

    ui->actionLoad_Simulation_Input->setIcon(QIcon(":/images/images/simulation.png"));
    ui->actionLoad_Streaming_Data_File->setIcon(QIcon(":/images/images/006-binary-code-loading-symbol.png"));

    ui->actionInfo->setIcon(QIcon(":/images/images/002-information.png"));
    ui->actionLicense_GPLv3->setIcon(QIcon(":/images/images/002-information.png"));
    ui->actionLicense_LGPLv3->setIcon(QIcon(":/images/images/002-information.png"));
    ui->actionUsed_License_GPLv3->setIcon(QIcon(":/images/images/002-information.png"));
    ui->actionOpen_script->setIcon(QIcon(":/images/images/004-computing-script.png"));
    ui->actionOpen->setIcon(QIcon(":/images/images/003-chat.png"));
    ui->actionOpen_2->setIcon(QIcon(":/images/images/001-settings.png"));

    ui->actionOpen_calculator->setIcon(QIcon(":/images/images/calculator73.svg"));

    ui->actionOPen->setIcon(QIcon(":/images/images/001-heart-rate-monitor.png"));
    ui->actionOpen_serverConfig->setIcon(QIcon(":/images/server"));
    ui->actionRemote_Control_server->setIcon(QIcon(":/images/rc"));
}
