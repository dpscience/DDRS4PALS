#-------------------------------------------------
#
# Project created by QtCreator 2016-08-11T10:03:42
#
#-------------------------------------------------

QT       += core gui concurrent script printsupport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DDRS4PALS_1_0_12
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
    GUI/drs4spinbox.cpp

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
    GUI/drs4spinbox.h

FORMS    += GUI/drs4scopedlg.ui \
    GUI/drs4addinfodlg.ui \
    GUI/drs4boardinfodlg.ui \
    GUI/drs4scriptdlg.ui \
    GUI/drs4startdlg.ui \
    GUI/drs4pulsesavedlg.ui \
    GUI/drs4statelogdlg.ui \
    GUI/drs4pulsesaverangedlg.ui \
    GUI/drs4calculatordlg.ui \
    GUI/drs4cfdalgorithmdlg.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -llibusb-1.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -llibusb-1.0

INCLUDEPATH += $$PWD/include/libusb-1.0
DEPENDPATH += $$PWD/include/libusb-1.0

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lDLTPulseGenerator
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lDLTPulseGenerator

INCLUDEPATH += $$PWD/include/DLTPulseGenerator
DEPENDPATH += $$PWD/include/DLTPulseGenerator

RESOURCES += drs4resources.qrc
