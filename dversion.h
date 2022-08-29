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

#ifndef DVERSION
#define DVERSION

#include "include/DLTPulseGenerator/DLTPulseGenerator.h"

/* change here before releasing a new version **/
#define NAME                               QString("DDRS4PALS")
#define MAJOR_VERSION          QString("1")
#define MINOR_VERSION           QString("18")
#define VERSION_EXTENSION  QString("- official -")
#define DATE_EXTENSION         QString("2022-08-29")

/* streaming on external storage device */
#define DATA_STREAM_VERSION 1

/* complete program name */
#define PROGRAM_NAME QString(NAME + " v" + MAJOR_VERSION + "." + MINOR_VERSION + " " + VERSION_EXTENSION + " (" + DATE_EXTENSION + ")")

/* settings file extension */
#define EXT_LT_SETTINGS_FILE   QString(".drs4LTSettings")

/* version of settings file */
#define VERSION_SETTINGS_FILE 4 /* 23.11.2018 */

/* simulation input file extension */
#define EXT_SIMULATION_INPUT_FILE QString(".drs4SimulationInputFile")

/* streaming file extension */
#define EXT_PULSE_STREAM_FILE   QString(".drs4DataStream")

/* script file extension */
#define EXT_SCRIPT_FILE QString(".drs4Script")

/* program settings file extension */
#define EXT_PROGRAM_SETTINGS_FILE   QString(".drs4ProgramSettings")

#define NO_SETTINGS_FILE_PLACEHOLDER    QString("<no settings-file loaded>")
#define NO_SCRIPT_FILE_PLACEHOLDER    QString("<no script-file loaded>")

#define PROGRAM_SETTINGS_ENCRYPTION_KEY QString("x§ThisYsoftwareCis;written_by?Danny)Petschke.&Copyrightp2016-2018.+All%rights!reserved.#???=")

#define MSGBOX(__text__) DMSGBOX2(__text__, PROGRAM_NAME);

#define COPYRIGHT_DATE "2016-2022"
#define COPYRIGHT_NOTICE QString("(C) Copyright %1 by Dr. Danny Petschke. All rights reserved.").arg(COPYRIGHT_DATE)

#define SHOW_ABOUT() { const QString text = PROGRAM_NAME + "<br><br>" + COPYRIGHT_NOTICE + "<br><br>"; \
    const QString contact = "contact: <a href=\"danny.petschke@uni-wuerzburg.de\">danny.petschke@uni-wuerzburg.de</a><br><br>"; \
    const QString publication1 = "<b>Related publications:</b><br><br><a href=\"https://www.sciencedirect.com/science/article/pii/S2352711019300676\">DDRS4PALS: A software for the acquisition and simulation of lifetime spectra using the DRS4 evaluation board</a><br><br>"; \
    const QString publication2 = "<a href=\"https://www.sciencedirect.com/science/article/pii/S2352340918315142?via%3Dihub\">Data on pure tin by Positron Annihilation Lifetime Spectroscopy (PALS) acquired with a semi-analog/digital setup using DDRS4PALS</a><br><br><br>"; \
    const QString publication3 = "<a href=\"https://www.sciencedirect.com/science/article/pii/S2352711018300530\">DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses</a><br><br><br>"; \
    const QString publication4 = "<a href=\"https://www.sciencedirect.com/science/article/pii/S2352711018300694\">Update (v1.1) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses</a><br><br><br>"; \
    const QString publication5 = "<a href=\"https://www.sciencedirect.com/science/article/pii/S2352711018301092\">Update (v1.2) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses</a><br><br><br>"; \
    const QString publication6 = "<a href=\"https://www.sciencedirect.com/science/article/pii/S235271101930038X\">Update (v1.3) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses</a><br><br><br>"; \
    const QString license = "<nobr>Fit Algorithm by: <br><a href=\"https://www.physics.wisc.edu/~craigm/idl/cmpfit.html\">MPFIT</a>: A MINPACK-1 Least Squares Fitting Library in C</nobr><br><br>"; \
    const QString license2 = "<nobr>CF Interpolation by:<br><a href=\"http://www.alglib.net/\">ALGLIB</a>: A Numerical Analysis and Data Processing Library</nobr><br>and<br><a href=\"http://kluge.in-chemnitz.de/opensource/spline/\">Tino Kluge</a>: Cubic Spline Interpolation in C++<br><br>"; \
    const QString license2_1 = "<nobr>Spike Removal by:<br><a href=\"http://www.sergejusz.com/engineering_tips/median_filter.htm/\">S. Zabinskis</a>: A Fast Median Filter in C++<br><br>";\
    const QString license4 = "<nobr>Logo designed by: <br><a href=\"https://www.researchgate.net/profile/Hannah_Heil\">Hannah Heil</a></nobr><br><br><br>"; \
    const QString acknowlegment = "<b>Acknowledgment:</b><br><br><nobr>I thank Jakub Cizek (Charles University in Prague)<br><a href=\"https://www.researchgate.net/profile/Jakub_Cizek\">Jakub Cizek</a><br><br> and Martin Petriska (Slovak University of Technology in Bratislava)<br><a href=\"https://www.researchgate.net/profile/Martin_Petriska\">Martin Petriska</a><br><br> for fruitful discussions concerning pulse shape filtering and digital positron lifetime spectroscopy on the <a href=\"https://www.bgsu.edu/events/icpa-18.html\">ICPA-18</a>.</nobr><br><br>Furthermore, I thank <a href=\"https://www.researchgate.net/profile/Ricardo_Helm\">Ricardo Helm</a> for extensively testing this software."; \
    QString versionStr; \
    versionStr = QString("v%1.%2").arg(DLTPULSEGENERATOR_MAJOR_VERSION).arg(DLTPULSEGENERATOR_MINOR_VERSION); \
    const QString license3 = "<nobr><a href=\"https://github.com/dpscience/DLTPulseGenerator\">DLTPulseGenerator</a> " + versionStr + " for Spectra-Simulation by: <br><a href=\"https://www.researchgate.net/profile/Danny_Petschke\">Danny Petschke</a></nobr><br><br>"; \
    QMessageBox::about(this, PROGRAM_NAME, "<b>" + text + "</b>" + contact + license + license2 + license2_1 + license3 + license4 + publication1 + publication2 + publication3 + publication4 + publication5 + publication6 + acknowlegment); }

/* update notifier request URL */
#define GITHUB_LATEST_RELEASE_GET_URL QUrl("https://api.github.com/repos/dpscience/DDRS4PALS/releases/latest")

enum class ProgramStartType : int {
    Unknown = -1,
    DataAcquisition = 0,
    Simulation = 1,
    Abort = 2
};

#endif // DVERSION

