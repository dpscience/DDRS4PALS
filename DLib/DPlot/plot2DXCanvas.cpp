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

#include "plot2DXCanvas.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

#define DEFAULT_CANVAS_BACKGROUND_COLOR    Qt::white

plot2DXCanvas::plot2DXCanvas(QWidget *parent) :
    QWidget(parent),
    m_canvasPixmap(parent->size()),
    m_bgdColor(DEFAULT_CANVAS_BACKGROUND_COLOR)
{
    m_canvasPixmap.fill(getBackgroundColor());
}

void plot2DXCanvas::setBackgroundColor(const QColor &color)
{
    m_bgdColor = color;

    m_canvasPixmap = QPixmap(rect().size());
    m_canvasPixmap.fill(getBackgroundColor());

    update();

    emit canvasPropertyChanged();
}

void plot2DXCanvas::clear()
{
    m_canvasPixmap = QPixmap(rect().size());
    m_canvasPixmap.fill(getBackgroundColor());

    update();
}

QColor plot2DXCanvas::getBackgroundColor() const
{
    return m_bgdColor;
}

void plot2DXCanvas::drawCurve(int curveWidth,
                                   const QColor& curveColor,
                                   plot2DXCurve::curveStyle style,
                                   const QVector<QPoint>& pixelList)
{
    if ( m_canvasPixmap.isNull() )
        return;

    const QPen pen(QBrush(curveColor),
                   curveWidth);

    QPainter painter(pixmap());

    painter.setPen(pen);

    switch ( style ){
    case plot2DXCurve::point:
    {
        for ( int i = 0 ; i < pixelList.size() ; i ++ ){
            drawPoints(pixelList.at(i),&painter);
        }
    }
        break;

    case plot2DXCurve::line:
        drawLine(pixelList,&painter);
        break;

    case plot2DXCurve::cross:
    {
        for ( int i = 0 ; i < pixelList.size() ; i ++ ){
            drawCross(pixelList.at(i),&painter);
        }
    }
        break;

    case plot2DXCurve::rect:
    {
        for ( int i = 0 ; i < pixelList.size() ; i ++ ){
            drawRect(pixelList.at(i),&painter);
        }
    }
        break;

    case plot2DXCurve::circle:
    {
        for ( int i = 0 ; i < pixelList.size() ; i ++ ){
            drawCircle(pixelList.at(i),&painter);
        }
    }
        break;

    default:
        break;
    }

    painter.end();

    update();
}

void plot2DXCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( m_canvasPixmap.isNull() )
        return;

    QPainter painter(this);

    painter.drawPixmap(rect(),*pixmap());
}

QPixmap* plot2DXCanvas::pixmap()
{
    return &m_canvasPixmap;
}

void plot2DXCanvas::shiftPixmap(int shift)
{
    if ( shift == 0 )
        return;

    if ( abs(shift) > pixmap()->rect().width() ){
        this->clear();
        return;
    }

    QRegion region;

    //scroll and determine the exposed region:
    pixmap()->scroll(shift,0,pixmap()->rect(),&region);

    QPixmap exposedPixmap = QPixmap(region.boundingRect().size());

    exposedPixmap.fill(getBackgroundColor());


    QPainter painter(pixmap());

    if ( shift < 0 )
        painter.drawPixmap(pixmap()->rect().width() + shift,0,exposedPixmap);
    else if ( shift > 0 )
        painter.drawPixmap(shift,0,exposedPixmap);

    painter.end();
}

void plot2DXCanvas::drawPoints(const QPoint &pixel,
                                    QPainter *painter)
{
    painter->drawPoint(pixel);
}

void plot2DXCanvas::drawCross(const QPoint &pixel,
                                   QPainter *painter)
{
    const int width = painter->pen().width();

    painter->save();

    painter->setPen(QPen(QBrush(painter->pen().brush()),1));

    QPainterPath path;

    path.moveTo(QPoint(pixel.x()-width/2,pixel.y()-width/2));
    path.lineTo(QPoint(pixel.x()+width/2,pixel.y()+width/2));
    path.moveTo(QPoint(pixel.x()+width/2,pixel.y()-width/2));
    path.lineTo(QPoint(pixel.x()-width/2,pixel.y()+width/2));

    painter->drawPath(path);

    painter->restore();
}

void plot2DXCanvas::drawRect(const QPoint &pixel,
                                  QPainter *painter)
{
    const int width = painter->pen().width();

    painter->save();

    painter->setPen(QPen(QBrush(painter->pen().brush()),1));
    painter->drawRect(QRect(pixel.x()-width/2,pixel.y()-width/2,width,width));

    painter->restore();
}

void plot2DXCanvas::drawLine(const QVector<QPoint> &pixelList,
                                  QPainter *painter)
{
    if (pixelList.size() <= 1)
        return;

    QVector<QLineF> lineVec;
    for ( int i = 0 ; i < pixelList.size() - 1 ; i ++ )
        lineVec.append(QLineF(QPointF(pixelList.at(i)), QPointF(pixelList.at(i+1))));

    painter->drawLines(lineVec);
}

void plot2DXCanvas::drawCircle(const QPoint &pixel,
                                    QPainter *painter)
{
    const int width = painter->pen().width();

    painter->save();

    painter->setPen(QPen(QBrush(painter->pen().brush()),1));
    painter->drawEllipse(QRect(pixel.x()-width/2,pixel.y()-width/2,width,width));

    painter->restore();
}

void plot2DXCanvas::drawYLeftGrid(const QVector<double> &yPxList, const QPen &pen)
{
    QPainter painter(pixmap());

    painter.setPen(pen);

    for ( int i = 0 ; i < yPxList.size() ; ++i ){
        const QPoint begin(0,yPxList[i]);
        const QPoint end(pixmap()->width(),yPxList[i]);

        painter.drawLine(begin,end);
    }

    update();
}

void plot2DXCanvas::drawYRightGrid(const QVector<double> &yPxList, const QPen &pen)
{
    QPainter painter(pixmap());

    painter.setPen(pen);

    for ( int i = 0 ; i < yPxList.size() ; ++i ){
        const QPoint begin(0,yPxList[i]);
        const QPoint end(pixmap()->width(),yPxList[i]);

        painter.drawLine(begin,end);
    }

    update();
}

void plot2DXCanvas::drawXBottomGrid(const QVector<double> &xPxList, const QPen &pen)
{
    QPainter painter(pixmap());

    painter.setPen(pen);

    for ( int i = 0 ; i < xPxList.size() ; ++i ){
        const QPoint begin(xPxList[i],0);
        const QPoint end(xPxList[i],pixmap()->height());

        painter.drawLine(begin,end);
    }

    update();
}

void plot2DXCanvas::drawXTopGrid(const QVector<double> &xPxList, const QPen &pen)
{
    QPainter painter(pixmap());

    painter.setPen(pen);

    for ( int i = 0 ; i < xPxList.size() ; ++i ){
        const QPoint begin(xPxList[i],0);
        const QPoint end(xPxList[i],pixmap()->height());

        painter.drawLine(begin,end);
    }

    update();
}
