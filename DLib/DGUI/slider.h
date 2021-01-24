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

#ifndef SLIDER_H
#define SLIDER_H

#include "../DTypes/types.h"

#define DVERTICAL_SLIDER_RADIUS    6
#define DVERTICAL_SLIDER_COLOR     QColor(145, 185, 199)

#define DHORIZONTAL_SLIDER_RADIUS  6
#define DHORIZONTAL_SLIDER_COLOR   QColor(145, 185, 199)


class DVerticalDoubleSlider;
class DHorizontalDoubleSlider;

class DVerticalDoubleSlider : public QWidget
{
    Q_OBJECT
public:
    explicit DVerticalDoubleSlider(QWidget *parent = 0);
    virtual ~DVerticalDoubleSlider();

    double valueLowerLimit() const;
    double valueUpperLimit() const;

    double minimum() const;
    double maximum() const;

    DColor color() const;

protected:
    inline virtual void rangeChanged(double lowerValue, double upperValue) {}

public slots:
    void setRange(double min, double max);
    void setMinimum(double value);
    void setMaximum(double value);
    bool setValueLowerSlider(double value);
    bool setValueUpperSlider(double value);
    void setStepWidth(double stepWidth);
    void setColor(const DColor& color);

signals:
    void upperValueChanged(double);
    void lowerValueChanged(double);
    void valuesChanged(double, double);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    int convertToYPixel(double v);
    double convertFromYPixel(int px);

private:
    double m_valueLS, m_valueUS;
    double m_min, m_max;
    double m_stepWidth;

    QRectF m_lSRect, m_uSRect;
    bool m_lSActive, m_uSActive;

    DColor m_color;
};


class DHorizontalDoubleSlider : public QWidget
{
    Q_OBJECT
public:
    explicit DHorizontalDoubleSlider(QWidget *parent = 0);
    virtual ~DHorizontalDoubleSlider();

    double valueLowerLimit() const;
    double valueUpperLimit() const;

    double minimum() const;
    double maximum() const;

    DColor color() const;

protected:
    inline virtual void rangeChanged(double lowerValue, double upperValue) {}

public slots:
    void setRange(double min, double max);
    void setMinimum(double value);
    void setMaximum(double value);
    bool setValueLowerSlider(double value);
    bool setValueUpperSlider(double value);
    void setStepWidth(double stepWidth);
    void setColor(const DColor& color);

signals:
    void upperValueChanged(double);
    void lowerValueChanged(double);
    void valuesChanged(double, double);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    int convertToXPixel(double v);
    double convertFromXPixel(int px);

private:
    double m_valueLS, m_valueUS;
    double m_min, m_max;
    double m_stepWidth;

    QRectF m_lSRect, m_uSRect;
    bool m_lSActive, m_uSActive;

    DColor m_color;
};

#endif // SLIDER_H
