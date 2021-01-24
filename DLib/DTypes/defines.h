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

#ifndef DEFINES_H
#define DEFINES_H

#include <QDebug>
#include <Qt>
#include <QtGlobal>
#include <QMessageBox>
#include <QObject>
#include <QWidget>
#include <QSlider>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QToolButton>
#include <qdom.h>
#include <QEvent>
#include <QFile>
#include <QTextStream>
#include <QImage>
#include <QString>
#include <QSlider>
#include <QList>
#include <QStringBuilder>
#include <QtSvg/QSvgRenderer>
#include <QtXml/QDomDocument>
#include <QValidator>
#include <QLayout>
#include <QStringListModel>
#include <QTextEdit>
#include <QCompleter>
#include <QToolTip>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QKeyEvent>
#include <QTextBlock>
#include <QCryptographicHash>
#include <QSvgWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <stddef.h>
#include <sstream>
#include <new>
#include <cstdint>
#include <cmath>
#include <stdarg.h>
#include <stdint.h>
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <map>
#include <cstdlib>
#include <math.h>

#if defined(Q_OS_WIN)
#include "Windows.h"
#include "SetupAPI.h"
#endif


#define DMSGBOX(__text__)            { QMessageBox msgBox; msgBox.setText(QVariant(__text__).toString()); msgBox.exec(); }
#define DMSGBOX2(__text__, __title__)            { QMessageBox msgBox; msgBox.setWindowTitle(__title__); msgBox.setText(QVariant(__text__).toString()); msgBox.exec(); }

#define DLOG(format, ...)            d_log(format, ##__VA_ARGS__);
#define DERRORLOG(format, ...)       d_errorLog(__FILE__, __LINE__, format, ##__VA_ARGS__);

#define DDELETE_SAFETY(__ptr__)      if ( __ptr__ ) { delete __ptr__; __ptr__ = nullptr; }
#define DUNUSED_PARAM(x)             (void)x;

#define DNULLPTR nullptr


#define DZERO_MEMORY(__struct__)      memset(&__struct__ , 0, sizeof(__struct__));

#if defined(Q_OS_WIN)
#if defined(Q_OS_WIN64)
    typedef __int64 DVEC_INDEX;
#elif defined(Q_OS_WIN32) && !defined(Q_OS_WIN64)
    typedef __int32 DVEC_INDEX;
#endif
#elif defined(Q_OS_MAC) || defined(Q_OS_OSX) || defined(Q_OS_MACX)
#if defined(Q_OS_MAC32)
    typedef __int32_t DVEC_INDEX;
#elif defined(Q_OS_MAC64)
    typedef __int64_t DVEC_INDEX;
#endif
#endif

#define DMAX_LOG_LENGTH               16384

void d_log(const char* format, ...);
void d_errorLog(const char* file, int line, const char* format, ...);

#endif // DEFINES_H
