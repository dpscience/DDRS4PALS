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

#ifndef PLOT2DXCURVE_H
#define PLOT2DXCURVE_H

#include <QObject>

#include <QList>
#include <QVector>
#include <QPair>
#include <QColor>
#include <QPointF>
#include <QPoint>
#include <QRectF>

class plot2DXCurve : QObject {
    friend class plot2DXWidget;

    Q_OBJECT
public:
    typedef enum {
        yLeft_xBottom = 0,
        yLeft_xTop = 1,
        yRight_xBottom = 2,
        yRight_xTop = 3
    } scaleAxis;

    typedef enum {
        line = 0,
        point = 1,
        cross = 2,
        rect = 3,
        circle = 4
    } curveStyle;

    plot2DXCurve();
    virtual ~plot2DXCurve();

public slots:
    void setAxis(scaleAxis axis);
    void setCurveWidth(int width);
    void setCurveColor(const QColor& color);
    void setCurveStyle(curveStyle style);
    void showCurve(bool show);

    void addData(double x_value, double y_value);
    void addData(const QVector<QPointF> &dataset);
    void addData(const QVector<QPointF> &dataset, bool bDirection);
    void addDataVec(const QVector<int> &dataset);
    void addData(const QPointF& dataPoint);

    void clearCurveContent();
    void clearCurveContent(int from, int to);
    void clearCurveCache();
    void clearCurveCache(int from, int to);
    void setMaxContainerSize(int size);

signals:
    void curvePropertyChanged();
    void maxValueChanged(double minx, double miny, double maxx, double maxy, plot2DXCurve::scaleAxis axis);

public:
    scaleAxis getAxis() const;

    int getCurveWidth() const;
    QColor getCurveColor() const;
    curveStyle getCurveStyle() const;

    bool isCurveShown() const;

    QVector<QPointF> getData() const;
    QVector<QPointF> getCache() const;

    int setMaxContainerSize() const;

    int getDataSize() const;
    int getCacheSize() const;

private slots:
    void reset();
    void setLastValueBeforeReplot(const QPointF& lastValue);
    void swapToContainer();

private:
    QPointF getLastValueBeforeReplot() const;

private:
    QVector<QPointF> m_cache;
    QVector<QPointF> m_dataContainer;

    scaleAxis m_scale;

    int m_lineWidth;
    QColor m_lineColor;

    curveStyle m_curveStyle;

    bool m_shown;

    QPointF m_lastValueBeforeReplot;

    int m_maxCount;

    //stored max/min values
    double x_min, x_max;
    double y_min, y_max;
};

#endif // PLOT2DXCURVE_H
