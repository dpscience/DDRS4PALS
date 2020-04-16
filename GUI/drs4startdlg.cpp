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

#include "drs4startdlg.h"
#include "ui_drs4startdlg.h"

DRS4StartDlg::DRS4StartDlg(ProgramStartType *startType, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DRS4StartDlg),
    m_startType(startType),
    m_drs4BoardConnected(true),
    m_simulationLibIsLoaded(true),
    m_isFinished(false)
{
    ui->setupUi(this);

    ui->toolButton->hide();

    setWindowTitle(PROGRAM_NAME);

    *m_startType = ProgramStartType::Abort;
}

DRS4StartDlg::~DRS4StartDlg()
{
    DDELETE_SAFETY(ui);
}

void DRS4StartDlg::startDemoMode()
{
    *m_startType = ProgramStartType::Simulation;

    close();
}

void DRS4StartDlg::startDataAquistion()
{
    *m_startType = ProgramStartType::DataAcquisition;

    close();
}

void DRS4StartDlg::quit()
{
    *m_startType = ProgramStartType::Abort;

    close();
}

void DRS4StartDlg::showAbout()
{
    SHOW_ABOUT();
}

void DRS4StartDlg::showUpdateRequestError()
{
    ui->toolButton->hide();

    ui->updateLabel->setStyleSheet("color: red");
    ui->updateLabel->setText("No internet connection. Cannot check for updates on GitHub.");
}

void DRS4StartDlg::showUpdateRequestUpdateAvailable(QString tag, QString url, QDateTime releaseDateTime)
{
    ui->toolButton->show();

    connect(ui->toolButton, SIGNAL(clicked()), DRS4UpdateNotifier::sharedInstance(), SLOT(redirectToUpdate()));

    ui->updateLabel->setStyleSheet("color: blue");
    ui->updateLabel->setText("Update to DDRS4PALS v" + tag + " (" + releaseDateTime.toString() + ") is available ... ");
}

void DRS4StartDlg::showUpdateRequestStartUp()
{
    ui->toolButton->hide();

    ui->updateLabel->setStyleSheet("color: blue");
    ui->updateLabel->setText("Checking for updates on GitHub ... ");
}

void DRS4StartDlg::showUpdateRequestLatestReleaseIsRunning()
{
    ui->toolButton->hide();

    ui->updateLabel->setStyleSheet("color: green");
    ui->updateLabel->setText("You are running the latest release of DDRS4PALS.");
}

void DRS4StartDlg::showEvent(QShowEvent *event)
{
    connect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(checkingUpdatesStarted()), this, SLOT(showUpdateRequestStartUp()));
    connect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(latestReleaseRunning()), this, SLOT(showUpdateRequestLatestReleaseIsRunning()));
    connect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(error()), this, SLOT(showUpdateRequestError()));
    connect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(updateAvailable(QString,QString,QDateTime)), this, SLOT(showUpdateRequestUpdateAvailable(QString,QString,QDateTime)));

    DRS4UpdateNotifier::sharedInstance()->checkingForUpdates();

    //Real-Mode:
    if ( !DRS4BoardManager::sharedInstance()->connect() )
        m_drs4BoardConnected = false;

    //Simulation-Mode:
    QLibrary lib( "DLTPulseGenerator.dll" );
    m_simulationLibIsLoaded = lib.load();

    connect(ui->pushButton_simulation, SIGNAL(clicked()), this, SLOT(startDemoMode()));
    connect(ui->pushButton_real, SIGNAL(clicked()), this, SLOT(startDataAquistion()));
    connect(ui->pushButton_quit, SIGNAL(clicked()), this, SLOT(quit()));
    connect(ui->pushButton_info, SIGNAL(clicked()), this, SLOT(showAbout()));

    if ( !m_drs4BoardConnected )
    {
        ui->label_boardConnectionState->setStyleSheet("QLabel {color: red;}");

        ui->pushButton_real->setEnabled(false);

        if ( m_simulationLibIsLoaded )
        {
            const QString versionStr = QString("v%1.%2").arg(DLTPULSEGENERATOR_MAJOR_VERSION).arg(DLTPULSEGENERATOR_MINOR_VERSION);

            ui->label_simulationLibraryVersion->setText("DLTPulseGenerator.dll " + versionStr + " (" + DLTPULSEGENERATOR_VERSION_RELEASE_DATE + ") loaded!");
            ui->label_simulationLibraryVersion->setStyleSheet("QLabel {color: green;}");

            ui->label_boardConnectionState->setText("(DRS4-Board not connected!)");
        }
        else
        {
            ui->label_simulationLibraryVersion->setText("(DLTPulseGenerator.dll missing!)");
            ui->label_simulationLibraryVersion->setStyleSheet("QLabel {color: red;}");

            ui->pushButton_simulation->setEnabled(false);
        }
    }
    else
    {
        ui->pushButton_real->setEnabled(true);

        ui->label_boardConnectionState->setStyleSheet("QLabel {color: green;}");

        if ( m_simulationLibIsLoaded )
        {
            const QString versionStr = QString("v%1.%2").arg(DLTPULSEGENERATOR_MAJOR_VERSION).arg(DLTPULSEGENERATOR_MINOR_VERSION);

            ui->label_simulationLibraryVersion->setText("DLTPulseGenerator.dll " + versionStr + " (" + DLTPULSEGENERATOR_VERSION_RELEASE_DATE + ") loaded!");
            ui->label_simulationLibraryVersion->setStyleSheet("QLabel {color: green;}");

            ui->label_boardConnectionState->setText("(DRS4-Board: " + QVariant(DRS4BoardManager::sharedInstance()->currentBoard()->GetBoardSerialNumber()).toString() + " connected!)");
        }
        else
        {
            ui->label_simulationLibraryVersion->setText("(DLTPulseGenerator.dll missing!)");
            ui->label_simulationLibraryVersion->setStyleSheet("QLabel {color: red;}");

            ui->pushButton_simulation->setEnabled(false);
            ui->pushButton_simulation->setStyleSheet("color: white;");
        }
    }

    QMainWindow::showEvent(event);
}

void DRS4StartDlg::closeEvent(QCloseEvent *event)
{
    m_isFinished = true;

    disconnect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(checkingUpdatesStarted()), this, SLOT(showUpdateRequestStartUp()));
    disconnect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(latestReleaseRunning()), this, SLOT(showUpdateRequestLatestReleaseIsRunning()));
    disconnect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(error()), this, SLOT(showUpdateRequestError()));
    disconnect(DRS4UpdateNotifier::sharedInstance(), SIGNAL(updateAvailable(QString,QString,QDateTime)), this, SLOT(showUpdateRequestUpdateAvailable(QString,QString,QDateTime)));
    disconnect(ui->toolButton, SIGNAL(clicked()), DRS4UpdateNotifier::sharedInstance(), SLOT(redirectToUpdate()));

    QMainWindow::closeEvent(event);
}

bool DRS4StartDlg::isFinished() const
{
    return m_isFinished;
}
