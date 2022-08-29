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

#include "GUI/drs4scopedlg.h"
#include "GUI/drs4startdlg.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QSplashScreen>
#include <QSharedMemory>

#include "DLib.h"

#include "dversion.h"

#include <QDebug>

int main(int argc, char *argv[]) {
    /* check for another running instance */
    QSharedMemory mem("ckdkhfvakdjvhabsdfcjanöspofiäpansoucfdhbusvbdhfcPOIUXÄI");

    if ( !mem.create(1) ) {
        MSGBOX("Cannot start DDRS4PALS. Another instance of DDRS4PALS is already running!");
        exit(0);
    }

    QApplication a(argc, argv);

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    /* place splashscreen at the center of the screen */
    QSplashScreen splash;
    splash.setPixmap(QPixmap::fromImage(QImage(":/images/images/PALS.JPG").scaledToWidth(QApplication::desktop()->availableGeometry().width()*0.5)));
    splash.show();

    splash.showMessage((QString(QString("<b>") + PROGRAM_NAME + "</b><br>" + COPYRIGHT_NOTICE)), Qt::AlignLeft | Qt::AlignTop, Qt::darkGray);

    const QTime dieTime= QTime::currentTime().addSecs(3);

    /* show as long as requesting updates from github finished or at least 3 seconds */
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents();

    ProgramStartType startType = ProgramStartType::Unknown;

    /* setup start type of the program */
    DRS4StartDlg dlg(&startType);
    dlg.show();
    splash.finish(&dlg);

    while (!dlg.isFinished())
        QApplication::processEvents();

    if ( startType == ProgramStartType::Abort )
        return a.exit();

    DRS4ScopeDlg scopeDlg(startType);

    scopeDlg.showMaximized();

    return a.exec();
}
