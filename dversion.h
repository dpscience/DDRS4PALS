/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2018 Danny Petschke
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

#ifndef DVERSION
#define DVERSION

#include "include/DLTPulseGenerator/DLTPulseGenerator.h"

//comment-out to use in-line Pulse-Generator - deprecated (07/2017):
#define __DPALSPULSEGENERATOR_DEPRECATED

/** To Change before Release **/

#define NAME                       QString("DDRS4PALS")
#define MAJOR_VERSION       QString("1")
#define MINOR_VERSION    QString("01")
#define VERSION_EXTENSION QString("(official)")
#define DATE_EXTENSION  QString("16.04.2018")

#define DATA_STREAM_VERSION 1

/** ---------------------------------- **/

#define PROGRAM_NAME QString(NAME + " v" + MAJOR_VERSION + "." + MINOR_VERSION + " " + VERSION_EXTENSION + " (" + DATE_EXTENSION + ")")

/**          File-Extensions         **/

/**             User Files               **/
//Extension for settings-file:
#define EXT_LT_SETTINGS_FILE   QString(".drs4LTSettings")

#define VERSION_SETTINGS_FILE 1

//Extension for simulation-input-file:
#define EXT_SIMULATION_INPUT_FILE QString(".drs4SimulationInputFile")
#define EXT_SIMULATION_INPUT_FILE_DEPRECATED QString(".dSimulationToolSettings")

//Extension for pulse-stream-file:
#define EXT_PULSE_STREAM_FILE   QString(".drs4DataStream")

//Extension for script-file:
#define EXT_SCRIPT_FILE QString(".drs4Script")

/**      Internal/Program Files    **/
//Extension for general-settings:
#define EXT_PROGRAM_SETTINGS_FILE   QString(".drs4ProgramSettings")

/** ------------------------------------ **/

#define NO_SETTINGS_FILE_PLACEHOLDER    QString("<no settings-file loaded>")
#define NO_SCRIPT_FILE_PLACEHOLDER    QString("<no script-file loaded>")

#define PROGRAM_SETTINGS_ENCRYPTION_KEY QString("x§ThisYsoftwareCis;written_by?Danny)Petschke.&Copyrightp2016-2018.+All%rights!reserved.#???=")

#define MSGBOX(__text__) DMSGBOX2(__text__, PROGRAM_NAME);

#define SHOW_ABOUT() {const QString text = PROGRAM_NAME + "<br><br>(C) Copyright 2016-2018 by Danny Petschke<br>All rights reserved.<br><br>"; \
    const QString contact = "contact: <a href=\"danny.petschke@uni-wuerzburg.de\">danny.petschke@uni-wuerzburg.de</a><br><br>"; \
    const QString license = "<nobr>Fit Algorithm by: <br><a href=\"https://www.physics.wisc.edu/~craigm/idl/cmpfit.html\">MPFIT</a>: A MINPACK-1 Least Squares Fitting Library in C</nobr><br><br>"; \
    const QString license2 = "<nobr>CFD Interpolation by:<br><a href=\"http://www.alglib.net/\">ALGLIB</a>: A Numerical Analysis and Data Processing Library</nobr><br>and<br><a href=\"http://kluge.in-chemnitz.de/opensource/spline/\">Tino Kluge</a>: Cubic Spline Interpolation in C++<br><br>"; \
    const QString license4 = "<nobr>Logo designed by: Hannah Heil</nobr>"; \
    QString versionStr; \
    versionStr = QString("v%1.%2").arg(DLTPULSEGENERATOR_MAJOR_VERSION).arg(DLTPULSEGENERATOR_MINOR_VERSION); \
    const QString license3 = "<nobr><a href=\"https://github.com/dpscience/DLTPulseGenerator\">DLTPulseGenerator</a> " + versionStr + " for Spectra-Simulation by: <br>Danny Petschke</nobr><br><br>"; \
    QMessageBox::about(this, PROGRAM_NAME, text + contact + license + license2 + license3 + license4);}

enum class ProgramStartType {
    Unknown,
    DataAcquisition,
    Simulation,
    Abort
};

#endif // DVERSION

