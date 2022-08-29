# ****************************************************************************
#
#  DDRS4PALS, a software for the acquisition of lifetime spectra using the
#  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
#
#  Copyright (C) 2016-2022 Dr. Danny Petschke
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see http://www.gnu.org/licenses/.
#
# *****************************************************************************
#
#  @author: Danny Petschke
#  @contact: danny.petschke@uni-wuerzburg.de
#
# *****************************************************************************
#
# related publications:
#
# when using DDRS4PALS for your research purposes please cite:
#
# DDRS4PALS: A software for the acquisition and simulation of lifetime spectra using the DRS4 evaluation board:
# https://www.sciencedirect.com/science/article/pii/S2352711019300676
#
# and
#
# Data on pure tin by Positron Annihilation Lifetime Spectroscopy (PALS) acquired with a semi-analog/digital setup using DDRS4PALS
# https://www.sciencedirect.com/science/article/pii/S2352340918315142?via%3Dihub
#
# when using the integrated simulation tool /DLTPulseGenerator/ of DDRS4PALS for your research purposes please cite:
#
# DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses
# https://www.sciencedirect.com/science/article/pii/S2352711018300530
#
# Update (v1.1) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses
# https://www.sciencedirect.com/science/article/pii/S2352711018300694
#
# Update (v1.2) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses
# https://www.sciencedirect.com/science/article/pii/S2352711018301092
#
# Update (v1.3) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses
# https://www.sciencedirect.com/science/article/pii/S235271101930038X
#
# ****************************************************************************

QT       += core gui concurrent script printsupport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DDRS4PALS_1_0_18
TEMPLATE = app

RC_FILE = softwareIcon.rc

include(Dlib.pri)
include(AlgLib.pri)

X64 = $$find(QT_INSTALL_PREFIX, 64)
DEFINES+= X64

DEFINES+= HAVE_USB
DEFINES+= NOMINMAX
DEFINES+= HAVE_LIBUSB10
DEFINES+= USE_DRS_QT_MUTEX

SOURCES += main.cpp\
    DRS/drs507/DRS.cpp \
    DRS/drs507/averager.cpp\
    DRS/drs507/musbstd.c\
    DRS/drs507/mxml.c\
    DRS/drs507/strlcpy.c\
    drs4worker.cpp \
    GUI/drs4scopedlg.cpp \
    drs4boardmanager.cpp \
    drs4settingsmanager.cpp \
    Fit/mpfit.c \
    Fit/fitengine.cpp \
    Fit/dspline.cpp \
    GUI/drs4addinfodlg.cpp \
    GUI/drs4boardinfodlg.cpp \
    Stream/drs4streammanager.cpp \
    drs4simulationsettingsmanager.cpp \
    drs4pulsegenerator.cpp \
    drs4programsettingsmanager.cpp \
    Script/drs4scriptingengineaccessmanager.cpp \
    GUI/drs4scriptdlg.cpp \
    Script/drs4scriptmanager.cpp \
    Stream/drs4streamdataloader.cpp \
    GUI/drs4startdlg.cpp \
    GUI/drs4pulsesavedlg.cpp \
    GUI/drs4statelogdlg.cpp \
    GUI/drs4pulsesaverangedlg.cpp \
    GUI/drs4calculatordlg.cpp \
    GUI/drs4cfdalgorithmdlg.cpp \
    DQuickLTFit/settings.cpp \
    DQuickLTFit/projectmanager.cpp \
    UpdateNotifier/drs4updatenotifier.cpp \
    CPUUsage/drs4cpuusage.cpp \
    GUI/drs4doublespinbox.cpp \
    GUI/drs4spinbox.cpp \
    GUI/drs4licensetextbox.cpp \
    GUI/drs4boardcalibrationdlg.cpp \
    WebServer/drs4webserver.cpp \
    RemoteControlServer\drs4remotecontrolserver.cpp \
    GUI/drs4httpserverconfigdlg.cpp \
    GUI/drs4blinkinglight.cpp \
    GUI/drs4remotecontrolserverconfigdlg.cpp

HEADERS  += DRS/drs507/DRS.h \
    DRS/drs507/averager.h\
    DRS/drs507/usb.h \
    DRS/drs507/musbstd.h\
    DRS/drs507/mxml.h\
    DRS/drs507/strlcpy.h\
    include/libusb-1.0/libusb.h \
    include/DLTPulseGenerator/DLTPulseGenerator.h \
    drs4worker.h \
    GUI/drs4scopedlg.h \
    drs4boardmanager.h \
    drs4settingsmanager.h \
    Fit/mpfit.h \
    Fit/mpfit_DISCLAIMER \
    Fit/fitengine.h \
    Fit/dspline.h \
    GUI/drs4addinfodlg.h \
    GUI/drs4boardinfodlg.h \
    Stream/drs4streammanager.h \
    drs4simulationsettingsmanager.h \
    drs4pulsegenerator.h \
    drs4programsettingsmanager.h \
    Script/drs4scriptingengineaccessmanager.h \
    GUI/drs4scriptdlg.h \
    Script/drs4scriptmanager.h \
    Stream/drs4streamdataloader.h \
    dversion.h \
    GUI/drs4startdlg.h \
    GUI/drs4pulsesavedlg.h \
    GUI/drs4statelogdlg.h \
    GUI/drs4pulsesaverangedlg.h \
    GUI/drs4calculatordlg.h \
    GUI/drs4cfdalgorithmdlg.h \
    DQuickLTFit/settings.h \
    DQuickLTFit/projectmanager.h \
    UpdateNotifier/drs4updatenotifier.h \
    CPUUsage/drs4cpuusage.h \
    GUI/drs4doublespinbox.h \
    GUI/drs4spinbox.h \
    GUI/drs4licensetextbox.h \
    GUI/drs4boardcalibrationdlg.h \
    WebServer/drs4webserver.h \
    RemoteControlServer\drs4remotecontrolserver.h \
    GUI/drs4httpserverconfigdlg.h \
    GUI/drs4blinkinglight.h \
    GUI/drs4remotecontrolserverconfigdlg.h

FORMS    += GUI/drs4scopedlg.ui \
    GUI/drs4addinfodlg.ui \
    GUI/drs4boardinfodlg.ui \
    GUI/drs4scriptdlg.ui \
    GUI/drs4startdlg.ui \
    GUI/drs4pulsesavedlg.ui \
    GUI/drs4statelogdlg.ui \
    GUI/drs4pulsesaverangedlg.ui \
    GUI/drs4calculatordlg.ui \
    GUI/drs4cfdalgorithmdlg.ui \
    GUI/drs4licensetextbox.ui \
    GUI/drs4boardcalibrationdlg.ui \
    GUI/drs4httpserverconfigdlg.ui \
    GUI/drs4blinkinglight.ui \
    GUI/drs4remotecontrolserverconfigdlg.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -llibusb-1.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -llibusb-1.0

INCLUDEPATH += $$PWD/include/libusb-1.0
DEPENDPATH += $$PWD/include/libusb-1.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lDLTPulseGenerator
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lDLTPulseGenerator

INCLUDEPATH += $$PWD/include/DLTPulseGenerator
DEPENDPATH += $$PWD/include/DLTPulseGenerator

RESOURCES += drs4resources.qrc
