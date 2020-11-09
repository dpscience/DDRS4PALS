/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2020 Danny Petschke
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

#ifndef PLOT2DXWIDGET_H
#define PLOT2DXWIDGET_H

#include <QWidget>

#include <QColor>
#include <QRect>
#include <QList>
#include <QPoint>
#include <QPointF>
#include <QString>
#include <QStringBuilder>
#include <QVariant>
#include <QPen>

#include "math.h"

#include "plot2DXAxis.h"
#include "plot2DXCurve.h"
#include "plot2DXCanvas.h"

class plot2DXWidget : public QWidget {
    friend class plot2DXCurve;
    friend class plot2DXCanvas;
    friend class plot2DXAxis;

    Q_OBJECT
public:
    plot2DXWidget(QWidget *parent = 0);
    virtual ~plot2DXWidget();

private slots:
    void adaptAxisGeometry(bool visible);

public slots:
    virtual void replot();
    virtual void updatePlotView();

    void enableReplot(bool on);
    void setBackgroundColor(const QColor& color);

    void showYLeftGrid(bool on);
    void showYRightGrid(bool on);
    void showXBottomGrid(bool on);
    void showXTopGrid(bool on);

    void setYLeftGridPen(const QPen& pen);
    void setYRightGridPen(const QPen& pen);
    void setXBottomGridPen(const QPen& pen);
    void setXTopGridPen(const QPen& pen);

    void reset();
    void autoscale();

protected:    
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private:
    plot2DXCanvas *canvas() const;

    QVector<QPoint> pixelList(plot2DXCurve* curve);
    QVector<QPoint> pixelList(const QVector<QPointF> &curve, plot2DXCurve::scaleAxis axis);

    bool insideCanvas(const QPointF& point, double xMin, double xMax, double yMin, double yMax);
    double getMaximumXValue(const QVector<QPointF>& valueList);

public:
    plot2DXAxis *yLeft() const;
    plot2DXAxis* yRight() const;
    plot2DXAxis* xTop() const;
    plot2DXAxis* xBottom() const;

    QVector<plot2DXCurve*> curve() const;

    QColor getBackgroundColor() const;
    bool isReplotEnabled() const;

    bool isYLeftGridShown() const;
    bool isYRightGridShown() const;
    bool isXBottomGridShown() const;
    bool isXTopGridShown() const;

    QPen getYLeftGridPen() const;
    QPen getYRightGridPen() const;
    QPen getXBottomGridPen() const;
    QPen getXTopGridPen() const;

private:
    plot2DXAxis *yLeftAxis;
    plot2DXAxis *yRightAxis;
    plot2DXAxis *xTopAxis;
    plot2DXAxis *xBottomAxis;

    QVector<plot2DXCurve*> m_curveList;

    plot2DXCanvas *m_canvas;

    QRect m_canvasRect;

    QColor m_bgrdColor;

    bool m_replotEnabled;

    bool m_yLeftGridShown;
    bool m_yRightGridShown;
    bool m_xBottomGridShown;
    bool m_xTopGridShown;

    QPen m_yLeftGridPen;
    QPen m_yRightGridPen;
    QPen m_xBottomGridPen;
    QPen m_xTopGridPen;
};

#endif // PLOT2DXWIDGET_H
