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

#ifndef DRS4SCOPEDLG_H
#define DRS4SCOPEDLG_H

#include "dversion.h"

#include <QMainWindow>
#include <QThread>
#include <QFileDialog>
#include <QDateTime>
#include <QTimer>

#include "GUI/drs4addinfodlg.h"
#include "GUI/drs4boardinfodlg.h"
#include "GUI/drs4scriptdlg.h"
#include "GUI/drs4pulsesavedlg.h"
#include "GUI/drs4pulsesaverangedlg.h"
#include "GUI/drs4calculatordlg.h"
#include "GUI/drs4licensetextbox.h"
#include "GUI/drs4httpserverconfigdlg.h"
#include "GUI/drs4remotecontrolserverconfigdlg.h"

#include "drs4boardmanager.h"
#include "drs4worker.h"
#include "drs4settingsmanager.h"
#include "drs4pulsegenerator.h"
#include "drs4simulationsettingsmanager.h"
#include "drs4programsettingsmanager.h"

#include "Script/drs4scriptingengineaccessmanager.h"

#include "CPUUsage/drs4cpuusage.h"

#include "Stream/drs4streammanager.h"
#include "Stream/drs4streamdataloader.h"

#include "DLib/DPlot/plot2DXWidget.h"
#include "Fit/fitengine.h"

#include "WebServer/drs4webserver.h"
#include "RemoteControlServer/drs4remotecontrolserver.h"

#include "alglib.h"

#include "DQuickLTFit/projectmanager.h"

#define ACCESSED_BY_SCRIPT_AND_GUI  /*Function can be accessed by GUI and Script!*/

typedef enum
{
    AccessFromGUI = 0,
    AccessFromScript = 1
} FunctionSource;

class DRS4ScriptDlg;
class DRS4HttpServerConfigDlg;
class DRS4RemoteControlServerConfigDlg;

namespace Ui {
class DRS4ScopeDlg;
}

class DRS4ScopeDlg : public QMainWindow
{
    Q_OBJECT

    friend class DRS4ScriptingEngineAccessManager;
    friend class DRS4StreamDataLoader;
    friend class DRS4StreamManager;
    friend class DRS4FalseTruePulseStreamManager;

public:
    explicit DRS4ScopeDlg(const ProgramStartType& startType, QWidget *parent = DNULLPTR);
    virtual ~DRS4ScopeDlg();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private:
    void arrangeIcons();
    int getIndexForSweep(int sweep);

    void initScopePlots();
    void initPulseHeightSpectraPlots();
    void initABLTSpectrum();
    void initBALTSpectrum();
    void initCoincidenceLTSpectrum();
    void initMergedLTSpectrum();
    void initBoardTPlot();
    void initPulseAreaFilterA();
    void initPulseAreaFilterB();
    void initPulseRiseTimeFilterA();
    void initPulseRiseTimeFilterB();
    void initPersistancePlots();
    void initPulseShapeFilterPlots();

    void adaptPersistancePlotA();
    void adaptPersistancePlotB();

private:
    bool ACCESSED_BY_SCRIPT_AND_GUI loadSimulationToolSettingsFromExtern(const QString& path, bool checkForExtension = true);

    bool ACCESSED_BY_SCRIPT_AND_GUI loadDataStreamFromExtern(const QString& path, bool checkForExtension = true);
    bool ACCESSED_BY_SCRIPT_AND_GUI stopDataFromDataStream();

    bool ACCESSED_BY_SCRIPT_AND_GUI loadSettingsFileFromExtern(const QString& path, bool checkForExtension = true);
    bool ACCESSED_BY_SCRIPT_AND_GUI saveSettingsFileFromExtern(const QString& path, bool checkForExtension = true);

    bool ACCESSED_BY_SCRIPT_AND_GUI saveABSpectrumFromExtern(const QString& fileName);
    bool ACCESSED_BY_SCRIPT_AND_GUI saveBASpectrumFromExtern(const QString& fileName);
    bool ACCESSED_BY_SCRIPT_AND_GUI saveMergedSpectrumFromExtern(const QString& fileName);
    bool ACCESSED_BY_SCRIPT_AND_GUI saveCoincidenceSpectrumFromExtern(const QString& fileName);

    bool ACCESSED_BY_SCRIPT_AND_GUI savePHSAFromExtern(const QString& fileName);
    bool ACCESSED_BY_SCRIPT_AND_GUI savePHSBFromExtern(const QString& fileName);

    bool ACCESSED_BY_SCRIPT_AND_GUI startStreamingFromExtern(const QString& fileName, bool checkForExtension = true);
    bool ACCESSED_BY_SCRIPT_AND_GUI stopStreamingFromExtern();

signals:
    void signalUpdateCurrentFileLabelFromScript(const QString& currentFile);
    void signalUpdateInfoDlgFromScript(const QString& comment);
    void signalUpdateThreadRunning(const QString& state, const QString& styleSheet);
    void signalAddSampleSpeedWarningMessage(bool on, bool scriptAccess);
    void signalChangeSampleSpeed(int index, bool accessFromScript);

private slots:
    void runDQuickLTFit(const QString &projectPath = "");

    void openDQuickLTFitABSpectrum();
    void openDQuickLTFitBASpectrum();
    void openDQuickLTFitCoincidenceSpectrum();
    void openDQuickLTFitMergedSpectrum();

    void updatePulseAreaFilterALimits();
    void updatePulseAreaFilterBLimits();

    void updateRiseTimeFilterALimits();
    void updateRiseTimeFilterBLimits();

    void updateCurrentFileLabelFromScript(const QString& currentFile);
    void updateInfoDlgFromScript(const QString& comment);
    void updateThreadRunning(const QString& state, const QString& styleSheet);
    void addSampleSpeedWarningMessage(bool on, bool accessFromScript);

    void startStopThread();
    void startThread();
    void stopThread();

    /* Pulse Shape Filter */

    void startAcquisitionOfPulseShapeFilterDataA();
    void stopAcquisitionOfPulseShapeFilterDataA();

    void resetPulseShapeFilterProgressA();
    void incrementPulseShapeFilterProgressA();

    void swapPulseShapeFilterDataA();

    void startAcquisitionOfPulseShapeFilterDataB();
    void stopAcquisitionOfPulseShapeFilterDataB();

    void resetPulseShapeFilterProgressB();
    void incrementPulseShapeFilterProgressB();

    void swapPulseShapeFilterDataB();

    void ACCESSED_BY_SCRIPT_AND_GUI changePulseShapeFilterPulseRecordNumberA(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePulseShapeFilterPulseRecordNumberB(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changeLeftAPulseShapeFilter(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeLeftBPulseShapeFilter(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeRightAPulseShapeFilter(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeRightBPulseShapeFilter(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changeROILeftAPulseShapeFilter(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeROILeftBPulseShapeFilter(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeROIRightAPulseShapeFilter(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeROIRightBPulseShapeFilter(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changeLowerStdDevFractionAPulseShapeFilter(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeUpperStdDevFractionAPulseShapeFilter(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changeLowerStdDevFractionBPulseShapeFilter(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeUpperStdDevFractionBPulseShapeFilter(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void adaptPulseShapeFilterPlotA();
    void adaptPulseShapeFilterPlotB();

    void calculatePulseShapeFilterSplineA();
    void calculatePulseShapeFilterSplineB();

    void ACCESSED_BY_SCRIPT_AND_GUI changePulseShapeFilterEnabledA(bool activate, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePulseShapeFilterEnabledB(bool activate, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void changePulseShapeFilterRecordScheme(int index);

    /* Baseline Correction/Baseline Filter */

    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineStartCellA(int startCell, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineRegionA(int region, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineShiftValueA(double baseline, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineFilterRejectionLimitA(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineCorrectionEnabledA(bool enabled, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineFilterEnabledA(bool enabled, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineMethodA(int method, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselinePeakStartCellA(int startCell, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineWindowA(int window, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineStartCellB(int startCell, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineRegionB(int region, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineShiftValueB(double baseline, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineFilterRejectionLimitB(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineCorrectionEnabledB(bool enabled, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineFilterEnabledB(bool enabled, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineMethodB(int method, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselinePeakStartCellB(int startCell, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBaselineWindowB(int window, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void resetAllLTSpectraByPushButton(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void resetLTSpectrumABByPushButton(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void resetLTSpectrumBAByPushButton(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void resetLTSpectrumCoincidenceByPushButton(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void resetLTSpectrumMergedByPushButton(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void resetAreaPlotAByPushButton(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void resetAreaPlotBByPushButton(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void resetRiseTimePlotAByPushButton(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void resetRiseTimePlotBByPushButton(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void resetPersistancePlotAByPushButton(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void resetPersistancePlotBByPushButton(const FunctionSource& source = FunctionSource::AccessFromGUI);

    void autosaveAllSpectra();

    void saveABSpectrum(bool autosave = false, const QString& fileNameAutosave = QCoreApplication::applicationDirPath() + "//__autosaveSpecAB.txt");
    void saveABSpectrumDQuickLTFit(bool autosave = false, const QString& fileNameAutosave = QCoreApplication::applicationDirPath() + "//__autosaveSpecAB.dquicklt");
    void saveBASpectrum(bool autosave = false, const QString& fileNameAutosave = QCoreApplication::applicationDirPath() + "//__autosaveSpecBA.txt");
    void saveBASpectrumDQuickLTFit(bool autosave = false, const QString& fileNameAutosave = QCoreApplication::applicationDirPath() + "//__autosaveSpecBA.dquicklt");
    void saveCoincidenceSpectrum(bool autosave = false, const QString& fileNameAutosave = QCoreApplication::applicationDirPath() + "//__autosaveSpecPrompt.txt");
    void saveCoincidenceSpectrumDQuickLTFit(bool autosave = false, const QString& fileNameAutosave = QCoreApplication::applicationDirPath() + "//__autosaveSpecPrompt.dquicklt");
    void saveMergedSpectrum(bool autosave = false, const QString& fileNameAutosave = QCoreApplication::applicationDirPath() + "//__autosaveSpecMerged.txt");
    void saveMergedSpectrumDQuickLTFit(bool autosave = false, const QString& fileNameAutosave = QCoreApplication::applicationDirPath() + "//__autosaveSpecMerged.dquicklt");
    void savePHSA(bool autosave = false, const QString& fileNameAutosave = QCoreApplication::applicationDirPath() + "//__autosavePHSA.txt");
    void savePHSB(bool autosave = false, const QString& fileNameAutosave = QCoreApplication::applicationDirPath() + "//__autosavePHSB.txt");
    void saveRiseTimeDistributionA();
    void saveRiseTimeDistributionB();
    void saveAreaDistributionA();
    void saveAreaDistributionB();

    void saveSettings();
    void saveAsSettings();
    void loadSettings();

    void autoSave();
    void loadAutoSave();

    void setup(double oldValue, double oldSweep, double ratio);

    void startStreaming();
    void stopStreaming();

    void startTrueFalsePulseStreaming();
    void stopTrueFalsePulseStreaming();

    void loadSimulationToolSettings();
    void loadStreamingData();

    void runningFromDataStreamStarted();
    void runningFromDataStreamFinished();

    void clearCoincidenceFitData();
    void clearMergedFitData();
    void clearABFitData();
    void clearBAFitData();

    void scaleRange();
    void fullRange();

    QString updateCurrentFileLabel(bool accessFromScript = false);

    void showAddInfoBox();
    void showBoardInfoBox();
    void showScriptBox();
    void showAboutBox();
    void showSavePulses();
    void showSavePulsesRange();
    void showCalculator();
    void showGPL();
    void showLGPL();
    void showUsedGPL();
    void showServerConfig();
    void showRCServerConfig();

private slots:
    void changeServerState(bool active);

    void updateInBurstMode();

    void plotPulseScope();
    void plotPHS();
    void plotPHSWindows();
    void plotPulseAreaFilterData();
    void plotRiseTimeFilterData();
    void plotLifetimeSpectra();
    void plotPersistance();

    void updateTemperature();
    void checkForConnection();

public slots:
    void ACCESSED_BY_SCRIPT_AND_GUI resetAllLTSpectra(const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI startAcquisition(const FunctionSource &source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI stopAcquisition(const FunctionSource &source);

    bool ACCESSED_BY_SCRIPT_AND_GUI isAcquisitionRunning() const;
    bool ACCESSED_BY_SCRIPT_AND_GUI isAcquisitionStopped() const;

    bool ACCESSED_BY_SCRIPT_AND_GUI isPulseGenerationFromDataStreamRunning() const;
    bool ACCESSED_BY_SCRIPT_AND_GUI isDataStreamArmed() const;

    void ACCESSED_BY_SCRIPT_AND_GUI changeBurstModeEnabled(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeMulticoreThreadingEnabled(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);

    bool ACCESSED_BY_SCRIPT_AND_GUI isBurstModeRunning() const;
    bool ACCESSED_BY_SCRIPT_AND_GUI isMulticoreThreadingEnabled() const;

    void changeChannel1_A(bool on);
    void changeChannel2_A(bool on);
    void changeChannel3_A(bool on);
    void changeChannel4_A(bool on);

    void changeChannels_A(int set_chn);

    void changeChannel1_B(bool on);
    void changeChannel2_B(bool on);
    void changeChannel3_B(bool on);
    void changeChannel4_B(bool on);

    void changeChannels_B(int set_chn);

    QString ACCESSED_BY_SCRIPT_AND_GUI currentSettingsFile() const;

    void ACCESSED_BY_SCRIPT_AND_GUI changePulsePairChunkSize(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    int ACCESSED_BY_SCRIPT_AND_GUI pulsePairChunkSize() const;

    /* Persistance */
    void ACCESSED_BY_SCRIPT_AND_GUI changePersistancePlotEnabled(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changePersistancePlotUsingCFDB_For_A(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePersistancePlotUsingCFDA_For_B(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changeLeftAPersistance(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeLeftBPersistance(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeRightAPersistance(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeRightBPersistance(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI resetPersistancePlotA(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI resetPersistancePlotB(const FunctionSource& source = FunctionSource::AccessFromGUI);

    /* PHS */
    void ACCESSED_BY_SCRIPT_AND_GUI changePHSStartMinA(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePHSStartMaxA(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePHSStopMinA(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePHSStopMaxA(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changePHSStartMinB(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePHSStartMaxB(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePHSStopMinB(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePHSStopMaxB(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI resetPHSA(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI resetPHSB(const FunctionSource& source = FunctionSource::AccessFromGUI);

    /* Pulse - Scope */
    void changeNegativeLTAvailable(bool on);
    void changeForceCoincidence(bool force);
    void changeIgnoreBusyState(bool ignore);

    void ACCESSED_BY_SCRIPT_AND_GUI changeStartCell(int value = -1, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeStopCell(int value = -1, const FunctionSource& source = FunctionSource::AccessFromGUI);

    int ACCESSED_BY_SCRIPT_AND_GUI startCell() const;
    int ACCESSED_BY_SCRIPT_AND_GUI stopCell() const;

    void ACCESSED_BY_SCRIPT_AND_GUI changeTriggerLevelA(int levelMilliVolt, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeTriggerLevelB(int levelMilliVolt, const FunctionSource& source = FunctionSource::AccessFromGUI);

    int ACCESSED_BY_SCRIPT_AND_GUI triggerLevelAInMillivolt() const;
    int ACCESSED_BY_SCRIPT_AND_GUI triggerLevelBInMillivolt() const;

    int ACCESSED_BY_SCRIPT_AND_GUI triggerDelayInNanoseconds() const;

    void changeTriggerLevelA2();
    void changeTriggerLevelB2();

    void ACCESSED_BY_SCRIPT_AND_GUI changeCFDLevelA(double cfd, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeCFDLevelB(double cfd, const FunctionSource& source = FunctionSource::AccessFromGUI);

    double ACCESSED_BY_SCRIPT_AND_GUI CFDLevelA() const;
    double ACCESSED_BY_SCRIPT_AND_GUI CFDLevelB() const;

    void ACCESSED_BY_SCRIPT_AND_GUI changeTriggerDelay(int delayInNs, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void changeTriggerLogic(int index);
    void changeSampleSpeed(int index, bool accessFromScript = false);

    void ACCESSED_BY_SCRIPT_AND_GUI changePositivTriggerPolarity(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePositivSignal(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);

    bool ACCESSED_BY_SCRIPT_AND_GUI isPositivTriggerPolarity() const;
    bool ACCESSED_BY_SCRIPT_AND_GUI isPositiveSignal() const;

    /* Pulse - Area - Filter */
    void ACCESSED_BY_SCRIPT_AND_GUI changePulseAreaFilterEnabled(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePulseAreaFilterPlotEnabled(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changePulseAreaFilterA(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePulseAreaFilterB(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changeNormalizationForPulseAreaFilterA(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeNormalizationForPulseAreaFilterB(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changePulseAreaFilterLimitsUpperLeftA(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePulseAreaFilterLimitsLowerLeftA(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePulseAreaFilterLimitsUpperRightA(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePulseAreaFilterLimitsLowerRightA(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changePulseAreaFilterLimitsUpperLeftB(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePulseAreaFilterLimitsLowerLeftB(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePulseAreaFilterLimitsUpperRightB(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changePulseAreaFilterLimitsLowerRightB(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI resetAreaPlotA(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI resetAreaPlotB(const FunctionSource& source = FunctionSource::AccessFromGUI);

    /* Rise - Time - Filter */
    void ACCESSED_BY_SCRIPT_AND_GUI changeRiseTimeFilterEnabled(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeRiseTimeFilterPlotEnabled(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changeRiseTimeFilterScaleInNanosecondsA(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeRiseTimeFilterScaleInNanosecondsB(double value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changeRiseTimeFilterBinningCountA(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeRiseTimeFilterBinningCountB(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changeRiseTimeFilterLeftWindowA(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeRiseTimeFilterLeftWindowB(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changeRiseTimeFilterRightWindowA(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeRiseTimeFilterRightWindowB(int value, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI resetRiseTimePlotA(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI resetRiseTimePlotB(const FunctionSource& source = FunctionSource::AccessFromGUI);

    /* Median - Filter */
    void ACCESSED_BY_SCRIPT_AND_GUI changeMedianFilterAEnabled(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeMedianFilterBEnabled(bool on, const FunctionSource& source = FunctionSource::AccessFromGUI);

    void ACCESSED_BY_SCRIPT_AND_GUI changeMedianFilterWindowSizeA(int size, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeMedianFilterWindowSizeB(int size, const FunctionSource& source = FunctionSource::AccessFromGUI);

    /* Lifetime - Spectra */
    void ACCESSED_BY_SCRIPT_AND_GUI resetLTSpectrumAB(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI resetLTSpectrumBA(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI resetLTSpectrumCoincidence(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI resetLTSpectrumMerged(const FunctionSource& source = FunctionSource::AccessFromGUI);

    void changeChannelSettingsAB(double sett);
    void changeChannelSettingsAB2(int sett);

    void changeChannelSettingsBA(double sett);
    void changeChannelSettingsBA2(int sett);

    void changeChannelSettingsCoincidence(double sett);
    void changeChannelSettingsCoincidence2(int sett);

    void changeChannelSettingsMerged(double sett);
    void changeChannelSettingsMerged2(int sett);

    void ACCESSED_BY_SCRIPT_AND_GUI changeMeanCableDelayPs(double ps, const FunctionSource& source = FunctionSource::AccessFromGUI);
    double ACCESSED_BY_SCRIPT_AND_GUI meanCableDelayInPicoseconds() const;

    void ACCESSED_BY_SCRIPT_AND_GUI changeCoincidenceFitIterations(int iterations, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeMergedFitIterations(int iterations, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeABFitIterations(int iterations, const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI changeBAFitIterations(int iterations, const FunctionSource& source = FunctionSource::AccessFromGUI);

    int ACCESSED_BY_SCRIPT_AND_GUI coincidenceFitIterations() const;
    int ACCESSED_BY_SCRIPT_AND_GUI ABFitIterations() const;
    int ACCESSED_BY_SCRIPT_AND_GUI BAFitIterations() const;
    int ACCESSED_BY_SCRIPT_AND_GUI mergedFitIterations() const;

    void ACCESSED_BY_SCRIPT_AND_GUI fitCoincidenceData(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI fitMergedData(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI fitABData(const FunctionSource& source = FunctionSource::AccessFromGUI);
    void ACCESSED_BY_SCRIPT_AND_GUI fitBAData(const FunctionSource& source = FunctionSource::AccessFromGUI);

    double ACCESSED_BY_SCRIPT_AND_GUI lastFWHMABInPs() const;
    double ACCESSED_BY_SCRIPT_AND_GUI lastFWHMBAInPs() const;
    double ACCESSED_BY_SCRIPT_AND_GUI lastFWHMMergedInPs() const;
    double ACCESSED_BY_SCRIPT_AND_GUI lastFWHMCoincidenceInPs() const;

    double ACCESSED_BY_SCRIPT_AND_GUI lastt0ABInPs() const;
    double ACCESSED_BY_SCRIPT_AND_GUI lastt0BAInPs() const;
    double ACCESSED_BY_SCRIPT_AND_GUI lastt0MergedInPs() const;
    double ACCESSED_BY_SCRIPT_AND_GUI lastt0CoincidenceInPs() const;

    int ACCESSED_BY_SCRIPT_AND_GUI lastFitResultAB() const;
    int ACCESSED_BY_SCRIPT_AND_GUI lastFitResultBA() const;
    int ACCESSED_BY_SCRIPT_AND_GUI lastFitResultMerged() const;
    int ACCESSED_BY_SCRIPT_AND_GUI lastFitResultCoincidence() const;

    double ACCESSED_BY_SCRIPT_AND_GUI lastBoardTemperature() const;

    int ACCESSED_BY_SCRIPT_AND_GUI countsOfABSpectrum() const;
    int ACCESSED_BY_SCRIPT_AND_GUI countsOfBASpectrum() const;
    int ACCESSED_BY_SCRIPT_AND_GUI countsOfMergedSpectrum() const;
    int ACCESSED_BY_SCRIPT_AND_GUI countsOfCoincidenceSpectrum() const;

private:
    Ui::DRS4ScopeDlg *ui;

    mutable QMutex m_mutex;

    /* Optional GUI */
    QLabel *m_currentSettingsFileLabel;
    QString m_currentSettingsPath;

    /* Worker - Thread */
    DRS4WorkerDataExchange *m_dataExchange;
    DRS4Worker *m_worker;
    QThread *m_workerThread;

    /* Burst - Mode - Timer */
    QTimer *m_burstModeTimer;

    /* Pulse-Scope */
    QTimer *m_pulseRequestTimer;

    /* PHS */
    QTimer *m_phsRequestTimer;

    /* Area - Filter */
    QTimer *m_areaRequestTimer;

    /* Rise-Time Filter */
    QTimer *m_riseTimeRequestTimer;

    /* Lifetime-Spectra */
    QTimer *m_lifetimeRequestTimer;

    /* Persistance - Plot */
    QTimer *m_persistanceRequestTimer;
    bool m_bSwapDirection;

    /* Pulse Shape Filter (during recording) */
    QTimer *m_pulseShapeFilterTimerA;
    QTimer *m_pulseShapeFilterTimerB;

    QVector<QPointF> m_pulseShapeDataA;
    QVector<QPointF> m_pulseShapeDataB;

    DRS4PulseShapeFilterData m_pulseShapeSplineDataA;
    DRS4PulseShapeFilterData m_pulseShapeSplineDataB;

   /* Fit - Data */
    QVector<QPointF> m_fitPoints;
    QVector<QPointF> m_fitPointsMerged_Single;
    QVector<QPointF> m_fitPointsAB_Single;
    QVector<QPointF> m_fitPointsBA_Single;

    /* Board - Temperature */
    QTimer *m_temperatureTimer;
    double m_lastTemperatureInDegree;
    //qint64 m_time;

    /* Autosave */
    QTimer *m_autoSaveTimer;

    /* Autosave - Spectra */
    QTimer *m_autoSaveSpectraTimer;

    /* Connection Check Timer */
    QTimer *m_connectionTimer;

    /* Dialogs */
    DRS4AddInfoDlg *m_addInfoDlg;
    DRS4BoardInfoDlg *m_boardInfoDlg;
    DRS4ScriptDlg *m_scriptDlg;
    DRS4PulseSaveDlg *m_pulseSaveDlg;
    DRS4PulseSaveRangeDlg *m_pulseSaveRangeDlg;
    DRS4CalculatorDlg *m_calculatorDlg;
    DRS4LicenseTextBox *m_gplDialog;
    DRS4LicenseTextBox *m_lgplDialog;
    DRS4LicenseTextBox *m_usedgplDialog;
    DRS4HttpServerConfigDlg *m_serverDlg;
    DRS4RemoteControlServerConfigDlg *m_rcServerDlg;

    /* Fit - Results */
    double m_lastFWHMABInPs;
    double m_lastFWHMBAInPs;
    double m_lastFWHMMergedInPs;
    double m_lastFWHMCoincidenceInPs;

    double m_lastt0ABInPs;
    double m_lastt0BAInPs;
    double m_lastt0MergedInPs;
    double m_lastt0CoincidenceInPs;

    int m_lastFitResultAB;
    int m_lastFitResultBA;
    int m_lastFitResultMerged;
    int m_lastFitResultCoincidence;

    /* Shared Data with Worker-Thread: Area - Filter */
    double m_areaFilterASlopeUpper;
    double m_areaFilterAInterceptUpper;

    double m_areaFilterASlopeLower;
    double m_areaFilterAInterceptLower;

    double m_areaFilterBSlopeUpper;
    double m_areaFilterBInterceptUpper;

    double m_areaFilterBSlopeLower;
    double m_areaFilterBInterceptLower;

    bool m_bConnectionLost;
};

#endif // DRS4SCOPEDLG_H
