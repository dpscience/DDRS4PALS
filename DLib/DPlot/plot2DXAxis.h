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

#ifndef PLOT2DXAXIS_H
#define PLOT2DXAXIS_H

#include <QWidget>

#include <QFont>
#include <QColor>
#include <QPen>
#include <QRect>
#include <QPainter>
#include <QString>
#include <QVariant>
#include <QPoint>
#include <QStringBuilder>

#include "plot2DXCanvas.h"

class plot2DXAxis : public QWidget {
    friend class plot2DXWidget;

    Q_OBJECT
public:
    typedef enum {
        yLeft = 0,
        yRight = 1,
        xTop = 2,
        xBottom = 3
    } axisDockingPosition;

    typedef enum {
        timePlot = 0,
        valuePlot = 1
    } axisPlotType;

    typedef enum {
        linear = 0,
        logarithmic = 1,
        errorfunction = 2
    } axisScaling;

    typedef enum {
        floating = 0,
        exponential = 1,
        automatic = 2
    } numberFormat;

    typedef enum {
        valueStart = 0,
        valueEnd = 1,
        middle = 2
    } axisLabelPosition;

    /**
      * value-display type:
      *
      * numbers:  60000.0  (display-mode depends on 'numberFormat')
      * hh_mm_ss: 00:01,00 (display-mode is indepent on 'numberFormat')
      * hh_mm:    00:01    (display-mode is indepent on 'numberFormat')
      * mm_ss:    01:00    (display-mode is indepent on 'numberFormat')
      * ss:       60,00    (display-mode depends on 'numberFormat')
      **/
    typedef enum {
        numbers = 0,
        hh_mm_ss = 1,
        hh_mm = 2,
        mm_ss = 3,
        ss = 4
    } axisValueDisplay;

    plot2DXAxis(axisDockingPosition docking = yLeft,
                     axisPlotType plotType       = timePlot,
                     axisScaling scaling         = linear,
                     const QRect& rect           = QRect(),
                     QWidget *parent             = 0);

    virtual ~plot2DXAxis();

public slots:
    void setAxisRange(double axisMin, double axisMax);
    void setAxisMinValue(double axisMin);
    void setAxisMaxValue(double axisMax);
    void setAxisPlotType(axisPlotType plotType);
    void setAxisScaling(axisScaling scaling);
    void setAxisDistribution(int count);
    void setAxisHelpDistribution(int distribution);
    void setIncrementLength(int length);
    void setHelpIncrementLength(int length);
    void setLabelFont(const QFont& font);
    void setIncrementLabelColor(const QColor& color);
    void setAxisColor(const QColor& color);
    void setAxisWidth(int width);
    void setNumberFormat(numberFormat format);
    void setNumberPrecision(int precision);
    void setAxisLabelPosition(axisLabelPosition position);
    void setAxisLabelText(const QString& text);
    void setAxisLabelColor(const QColor& color);
    void showAxisLabel(bool show);
    void showHelpIncrements(bool show);
    void setValueDisplay(axisValueDisplay display);

    virtual void setVisible(bool visible);
    void adaptGeometry(const QRect& parentGeometry, const QRect& canvasGeometry);

public:
    double getAxisMinValue() const;
    double getAxisMaxValue() const;
    double getAxisSpan() const;
    int getAxisDistribution() const;
    axisDockingPosition getAxisDockingPosition() const;
    axisPlotType getAxisPlotType() const;
    axisScaling getAxisScaling() const;
    int getIncrementLength() const;
    int getHelpIncrementLength() const;
    int getAxisHelpDistribution() const;
    QFont getLabelFont() const;
    QColor getIncrementLabelColor() const;
    QColor getAxisColor() const;
    int getAxisWidth() const;
    int getAxisLength() const;
    numberFormat getNumberFormat() const;
    int getNumberPrecision() const;
    axisLabelPosition getAxisLabelPosition() const;
    QString getAxisLabelText() const;
    QColor getAxisLabelColor() const;
    bool isAxisLabelShown() const;
    bool isHelpIncrementShown() const;
    axisValueDisplay getValueDisplay() const;

    int ConvertToPixel(double point, axisScaling scaling);
    double ConvertFromPixel(int point, axisScaling scaling);

signals:
    void scalingPropertyChanged();
    void visibleStateChanged(bool);

protected:
    virtual void updateAxis();
    virtual void paintEvent(QPaintEvent *event);

private slots:
    void renew();

    void drawAxis();
    void drawIncrements();
    void drawAxisLabel();

private:
    void drawIncrementLabelText(QPainter *painter, double value, const QPoint& incrementEndPoint);
    void setBackgroundColor(const QColor& color);
    QString labelDisplayText(double value);
    QRect calculateDockingGeometry(const QRect& parentGeometry, const QRect& canvasGeometry);
    QColor getBackgroundColor() const;

private:
    axisDockingPosition m_docking;
    axisPlotType m_axisPlotType;
    axisScaling m_axisScaling;

    double m_axisMinValue;
    double m_axisMaxValue;
    double m_axisSpan;

    int m_width;
    int m_height;
    int m_xOffset;
    int m_yOffset;

    int m_axisDistribution;
    double m_axisDistributionRange;
    int m_incrementLength;
    int m_helpIncrementLength;
    int m_helpAxisDistribution;

    bool m_showHelpIncrements;

    QFont m_textFont;
    QColor m_incrementTextColor;

    QColor m_axisColor;
    int m_axisWidth;

    QPen m_axisPen;
    QPen m_incrementPen;
    QPen m_labelPen;

    char m_numberFormat;
    int m_numberPrecision;

    axisLabelPosition m_labelPosition;
    QString m_axisLabelText;
    QColor m_axisLabelColor;

    bool m_axisLabelShown;

    axisValueDisplay m_valueDisplay;

    double m_lastDisplayedValue;

    QColor m_bgrdColor;

    bool m_visible;
};

#endif // PLOT2DXAXIS_H
