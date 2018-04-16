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

#ifndef PLOT2DXCANVAS_H
#define PLOT2DXCANVAS_H

#include <QWidget>

#include <QColor>
#include <QPainter>
#include <QPen>
#include <QList>
#include <QPoint>
#include <QPixmap>

#include "plot2DXCurve.h"

class plot2DXCanvas : public QWidget
{
    friend class plot2DXWidget;

    Q_OBJECT
public:
    plot2DXCanvas(QWidget *parent = 0);

public slots:
    void setBackgroundColor(const QColor& color);
    void clear();

signals:
    void canvasPropertyChanged();

public:
    QColor getBackgroundColor() const;
    void drawCurve(int curveWidth, const QColor &curveColor, plot2DXCurve::curveStyle style, const QVector<QPoint> &pixelList);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QPixmap* pixmap();

private slots:
    /**
     * This function shifts the pixmap on the given value 'shift' to the left.
     * It's used if the 'timePlot'-Mode for the x-axis is set ('plot2DXAxis').
     */
    void shiftPixmap(int shift);

    void drawPoints(const QPoint& pixel, QPainter* painter);
    void drawCross(const QPoint& pixel, QPainter* painter);
    void drawRect(const QPoint& pixel, QPainter* painter);
    void drawLine(const QVector<QPoint>& pixelList, QPainter* painter);
    void drawCircle(const QPoint& pixel, QPainter* painter);

    void drawYLeftGrid(const QVector<double>& yPxList, const QPen& pen);
    void drawYRightGrid(const QVector<double>& yPxList, const QPen& pen);
    void drawXBottomGrid(const QVector<double>& xPxList, const QPen &pen);
    void drawXTopGrid(const QVector<double>& xPxList, const QPen &pen);

private:
    QPixmap m_canvasPixmap;
    QColor m_bgdColor;
};

#endif // PLOT2DXCANVAS_H
