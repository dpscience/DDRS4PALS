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

#include "GUI/drs4scopedlg.h"
#include "GUI/drs4startdlg.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QSplashScreen>
#include <QSharedMemory>

#include "DLib.h"

#include "dversion.h"

int main(int argc, char *argv[])
{
    QSharedMemory mem("ckdkhfvakdjvhabsdfcjanöspofiäpansoucfdhbusvbdhfcPOIUXÄI");

    if ( !mem.create(1) ) {
        MSGBOX("An instance of DDRS4PALS is already running!");
        exit(0);
    }

    QApplication a(argc, argv);

    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QSplashScreen splash;
    splash.setPixmap(QPixmap::fromImage(QImage(":/images/images/PALS.JPG").scaledToWidth(QApplication::desktop()->availableGeometry().width()*0.5)));
    splash.show();

#if !defined(Q_OS_WIN)
    splash.setFont(QFont("Arial", 12));
#endif

    splash.showMessage((QString(QString("<b>") + PROGRAM_NAME + "</b><br>(C) Copyright 2016-2019 by Danny Petschke. All rights reserved.")), Qt::AlignLeft | Qt::AlignTop, Qt::darkGray);

    const QTime dieTime= QTime::currentTime().addSecs(3);
    while ( QTime::currentTime() < dieTime )
       QCoreApplication::processEvents();

    ProgramStartType startType = ProgramStartType::Unknown;

    DRS4StartDlg dlg(&startType);
    dlg.show();
    splash.finish(&dlg);

    while (!dlg.isFinished()) { QApplication::processEvents(); }

    if ( startType == ProgramStartType::Abort )
        return a.exit();

    bool connectionNotPossible = false;

    DRS4ScopeDlg scopeDlg(startType, &connectionNotPossible);

    if ( startType != ProgramStartType::Simulation
         && connectionNotPossible ) {
        MSGBOX("Please connect your DRS4-Board and restart this Software!");
        return a.exit();
    }

    scopeDlg.showMaximized();

    return a.exec();
}
