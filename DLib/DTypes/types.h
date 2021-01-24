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

#ifndef DTYPES_H
#define DTYPES_H

#include "../DTypes/defines.h"

class DPointF : public QObject
{
    double m_x, m_y;

    Q_OBJECT
public:
    inline DPointF() : QObject(DNULLPTR), m_x(0.0f), m_y(0.0f) {}
    inline DPointF(const double& x, const double& y) : QObject(DNULLPTR), m_x(x), m_y(y) {}
    inline DPointF(const DPointF& p) : QObject(DNULLPTR)
    {
        m_x = p.getX();
        m_y = p.getY();
    }

    inline DPointF& operator=(const DPointF& p)
    {
        m_x = p.getX();
        m_y = p.getY();

        return *this;
    }

    inline virtual ~DPointF() {}

    inline double getX() const { return m_x; }
    inline double getY() const { return m_y; }

public slots:
    inline void setX(const double& x) { m_x = x; }
    inline void setY(const double& y) { m_y = y; }
};


class DColor : public QColor
{
public:
    inline DColor() : QColor() {}
    inline DColor(const QColor& color) : QColor(color) {}
    inline DColor(const DColor& color) : QColor((QColor)color) {}

    inline virtual ~DColor() {}
};


class DString : public QString
{
public:
    inline DString() : QString("") {}
    inline DString(const QString& val) : QString(val) {}
    inline DString(const DString& val) : QString((QString)val) {}
    inline DString(const QChar* val) : QString(val) {}

    virtual ~DString();

    DString parseBetween(const DString& start, const DString& stop) const;
    DString parseBetween(const QString& start, const QString& stop) const;
    QStringList parseBetween2(const QString &start, const QString &stop) const;
    QStringList parseBetween2(const DString &start, const DString &stop) const;
};


class DSVGImage
{
public:
    static QImage getImage(const QString &document, double width, double height, bool antialiasing = true);
    static QImage getImage(const DString &document, double width, double height, bool antialiasing = true);

private:
    inline DSVGImage()  {}
    inline virtual ~DSVGImage() {}
};

#endif // DTYPES_H
