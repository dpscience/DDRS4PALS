/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2022 Dr. Danny Petschke
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
**  @author: Dr. Danny Petschke
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

#include "plot2DXWidget.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

#define MAX_CURVE_NUMBER              20
#define DEFAULT_BACKGROUND_COLOR      QColor(Qt::white)

//#define CANVAS_X_OFFSET             80
//#define CANVAS_Y_OFFSET             50

#define CANVAS_Y_OFFSET_ON_VISIBLE    50
#define CANVAS_Y_OFFSET_ON_UNVISIBLE  10

#define CANVAS_X_OFFSET_ON_UNVISIBLE  10
#define CANVAS_X_OFFSET_ON_VISIBLE    80

#define DEFAULT_GRID_PEN              QPen(Qt::lightGray, 1, Qt::DashLine)

plot2DXWidget::plot2DXWidget(QWidget *parent) :
    QWidget(parent),
    m_canvasRect(CANVAS_X_OFFSET_ON_VISIBLE,
                 CANVAS_Y_OFFSET_ON_UNVISIBLE,
                 geometry().width()-CANVAS_X_OFFSET_ON_UNVISIBLE-CANVAS_X_OFFSET_ON_VISIBLE,
                 geometry().height()-CANVAS_Y_OFFSET_ON_VISIBLE),
    m_yLeftGridShown(true),
    m_yRightGridShown(false),
    m_xBottomGridShown(true),
    m_xTopGridShown(false),
    m_yLeftGridPen(DEFAULT_GRID_PEN),
    m_yRightGridPen(DEFAULT_GRID_PEN),
    m_xBottomGridPen(DEFAULT_GRID_PEN),
    m_xTopGridPen(DEFAULT_GRID_PEN),
    m_replotEnabled(true)
{
    /**
      **********************************************
      initialize the canvas-pointer (drawing-object):
      **********************************************
      */
    m_canvas = new plot2DXCanvas(this);
    m_canvas->setGeometry(m_canvasRect);

    connect(m_canvas,SIGNAL(canvasPropertyChanged()),this,SLOT(updatePlotView()));

    /**
      **********************************************
      initialize the axis-pointer (scaling-object):
      **********************************************
      */
    yRightAxis = new plot2DXAxis(plot2DXAxis::yRight,
                                      plot2DXAxis::valuePlot,
                                      plot2DXAxis::linear,
                                      m_canvasRect,
                                      this);

    yLeftAxis = new plot2DXAxis(plot2DXAxis::yLeft,
                                     plot2DXAxis::valuePlot,
                                     plot2DXAxis::linear,
                                     m_canvasRect,
                                     this);

    xTopAxis = new plot2DXAxis(plot2DXAxis::xTop,
                                    plot2DXAxis::valuePlot,
                                    plot2DXAxis::linear,
                                    m_canvasRect,
                                    this);

    xBottomAxis = new plot2DXAxis(plot2DXAxis::xBottom
                                       ,plot2DXAxis::valuePlot,
                                       plot2DXAxis::linear,
                                       m_canvasRect,
                                       this);

    connect(yRightAxis,SIGNAL(scalingPropertyChanged()),this,SLOT(updatePlotView()));
    connect(yLeftAxis,SIGNAL(scalingPropertyChanged()),this,SLOT(updatePlotView()));
    connect(xTopAxis,SIGNAL(scalingPropertyChanged()),this,SLOT(updatePlotView()));
    connect(xBottomAxis,SIGNAL(scalingPropertyChanged()),this,SLOT(updatePlotView()));

    yRightAxis->setVisible(false);
    xTopAxis  ->setVisible(false);


    /**
      **********************************************
      initialize the curve-pointer (curve-object):
      **********************************************
      */
    for ( int i = 0 ; i < MAX_CURVE_NUMBER ; ++i ){
        m_curveList.append(new plot2DXCurve());
        connect(m_curveList[i],SIGNAL(curvePropertyChanged()),this,SLOT(updatePlotView()));
    }

    //set the view´s and axis´ background-color:
    setBackgroundColor(DEFAULT_BACKGROUND_COLOR);
}

plot2DXWidget::~plot2DXWidget()
{
    delete yRightAxis;
    delete yLeftAxis;
    delete xTopAxis;
    delete xBottomAxis;

    yRightAxis = 0;
    yLeftAxis = 0;
    xTopAxis = 0;
    xBottomAxis = 0;

    delete m_canvas;
    m_canvas = 0;
}

void plot2DXWidget::adaptAxisGeometry(bool visible)
{
    Q_UNUSED(visible);
}

void plot2DXWidget::replot()
{
    if ( !isReplotEnabled() )
        return;

    if ( xBottom()->getAxisPlotType() != xTop()->getAxisPlotType() ){
#ifdef QT_DEBUG
        qDebug() << "error while calling 'replot()': \nx-axis-plottype can´t be mixed-up!\n"\
                    "x-bottom-axis and x-top-axis must be either 'time-value-plot' or 'value-plot'!";
#endif
        return;
    }

    if ( xBottom()->getAxisPlotType() == plot2DXAxis::valuePlot &&
         xTop()->getAxisPlotType()    == plot2DXAxis::valuePlot )
    {
        if ( isYLeftGridShown() && yLeft()->isVisible() ){

            const double ySpan = fabs(yLeft()->getAxisMaxValue() - yLeft()->getAxisMinValue());
            const int sectorCount = yLeft()->getAxisDistribution();
            const double sectorDelta = ySpan/sectorCount;

            QVector<double> gridYPixelList;

            for ( int i = 0 ; i <= sectorCount ; ++i ){
                const int px = yLeft()->ConvertToPixel(yLeft()->getAxisMinValue() + i*sectorDelta,yLeft()->getAxisScaling());

                gridYPixelList.append(px);
            }

            canvas()->drawYLeftGrid(gridYPixelList,getYLeftGridPen());
        }
        if ( isYRightGridShown() && yRight()->isVisible() ){

            const double ySpan = fabs(yRight()->getAxisMaxValue() - yRight()->getAxisMinValue());
            const int sectorCount = yRight()->getAxisDistribution();
            const double sectorDelta = ySpan/sectorCount;

            QVector<double> gridYPixelList;

            for ( int i = 0 ; i <= sectorCount ; ++i ){
                const int px = yRight()->ConvertToPixel(yRight()->getAxisMinValue() + i*sectorDelta,yRight()->getAxisScaling());

                gridYPixelList.append(px);
            }

            canvas()->drawYRightGrid(gridYPixelList,getYRightGridPen());
        }
        if ( isXBottomGridShown() && xBottom()->isVisible() ){

            const double xSpan = fabs(xBottom()->getAxisMaxValue() - xBottom()->getAxisMinValue());
            const int sectorCount = xBottom()->getAxisDistribution();
            const double sectorDelta = xSpan/sectorCount;

            QVector<double> gridXPixelList;

            for ( int i = 0 ; i <= sectorCount ; ++i ){
                const int px = xBottom()->ConvertToPixel(xBottom()->getAxisMinValue() + i*sectorDelta,xBottom()->getAxisScaling());

                gridXPixelList.append(px);
            }

            canvas()->drawXBottomGrid(gridXPixelList,getXBottomGridPen());
        }
        if ( isXTopGridShown() && xTop()->isVisible() ){

            const double xSpan = fabs(xTop()->getAxisMaxValue() - xTop()->getAxisMinValue());
            const int sectorCount = xTop()->getAxisDistribution();
            const double sectorDelta = xSpan/sectorCount;

            QVector<double> gridXPixelList;

            for ( int i = 0 ; i <= sectorCount ; ++i ){
                const int px = xTop()->ConvertToPixel(xTop()->getAxisMinValue() + i*sectorDelta,xTop()->getAxisScaling());

                gridXPixelList.append(px);
            }

            canvas()->drawXTopGrid(gridXPixelList,getXTopGridPen());
        }

        for ( int index = 0 ; index < MAX_CURVE_NUMBER ; index ++ ){
            QVector<QPoint> cachePixelList = pixelList(curve().at(index)->getCache(),
                                                     curve().at(index)->getAxis());


            if ( cachePixelList.isEmpty() )
                continue;

            switch ( curve().at(index)->getAxis() ){

            case plot2DXCurve::yLeft_xBottom:
            {
                const QPoint lastValue( xBottom()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().x(),xBottom()->getAxisScaling()),
                                        yLeft()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().y(),yLeft()->getAxisScaling()) );

                //cachePixelList.insert(0,lastValue);
            }
                break;


            case plot2DXCurve::yLeft_xTop:
            {
                const QPoint lastValue( xTop()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().x(),xTop()->getAxisScaling()),
                                        yLeft()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().y(),yLeft()->getAxisScaling()) );

                //cachePixelList.insert(0,lastValue);
            }
                break;


            case plot2DXCurve::yRight_xTop:
            {
                const QPoint lastValue( xTop()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().x(),xTop()->getAxisScaling()),
                                        yRight()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().y(),yRight()->getAxisScaling()) );

                //cachePixelList.insert(0,lastValue);
            }
                break;


            case plot2DXCurve::yRight_xBottom:
            {
                const QPoint lastValue( xBottom()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().x(),xBottom()->getAxisScaling()),
                                        yRight()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().y(),yRight()->getAxisScaling()) );

                //cachePixelList.insert(0,lastValue);
            }
                break;


            default:
                break;
            }

            curve().at(index)->swapToContainer();

            if ( !curve().at(index)->isCurveShown() )
                continue;

            canvas()->drawCurve(curve().at(index)->getCurveWidth(),
                                curve().at(index)->getCurveColor(),
                                curve().at(index)->getCurveStyle(),
                                cachePixelList);
        }
    }

    if ( xBottom()->getAxisPlotType() == plot2DXAxis::timePlot &&
         xTop()->getAxisPlotType()    == plot2DXAxis::timePlot )
    {
        double maxXValue = xBottom()->getAxisMaxValue();

        for ( int index = 0 ; index < MAX_CURVE_NUMBER ; index ++ ){
            const QVector<QPointF> cacheDataList = curve().at(index)->getCache();

            if ( cacheDataList.isEmpty() )
                continue;

            const double xValue = getMaximumXValue(cacheDataList);

            if ( xValue > maxXValue )
                maxXValue = xValue;
        }

        const double maxXValue_old = xBottom()->getAxisMaxValue();
        const bool shift = (maxXValue > maxXValue_old);

        const double xSpan = xBottom()->getAxisSpan();

        const double newXMaxValue = maxXValue;
        const double newXMinValue = (newXMaxValue - xSpan);

        xBottom()->setAxisRange(newXMinValue,newXMaxValue);
        xTop()->setAxisRange(newXMinValue,newXMaxValue);

        int xShift = 0;

        if ( shift )
            xShift = (-1)*abs(xBottom()->ConvertToPixel(xBottom()->getAxisMaxValue(),plot2DXAxis::linear) - xBottom()->ConvertToPixel(maxXValue_old,plot2DXAxis::linear));

        if ( xShift != 0 )
            canvas()->shiftPixmap(xShift);

        for ( int index = 0 ; index < MAX_CURVE_NUMBER ; index ++ ){

            QVector<QPoint> cachePixelList = pixelList(curve().at(index)->getCache(),
                                                     curve().at(index)->getAxis());

            if ( cachePixelList.isEmpty() ){
                curve().at(index)->swapToContainer();

                if ( !curve().at(index)->isCurveShown() )
                    continue;

                canvas()->drawCurve(curve().at(index)->getCurveWidth(),
                                    curve().at(index)->getCurveColor(),
                                    curve().at(index)->getCurveStyle(),
                                    cachePixelList);

                continue;
            }


            const QPointF lastValueF = curve().at(index)->getLastValueBeforeReplot();

            switch ( curve().at(index)->getAxis() ){

            case plot2DXCurve::yLeft_xBottom:
            {
                const double xMinValue = xBottom()->getAxisMinValue();
                const double xMaxValue = xBottom()->getAxisMaxValue();
                const double yMinValue = yLeft()->getAxisMinValue();
                const double yMaxValue = yLeft()->getAxisMaxValue();

                const QPoint lastValue( xBottom()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().x(),plot2DXAxis::linear),
                                        yLeft()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().y(),plot2DXAxis::linear) );

                if ( insideCanvas(lastValueF,xMinValue,xMaxValue,yMinValue,yMaxValue) )
                    cachePixelList.insert(0,lastValue);
            }
                break;


            case plot2DXCurve::yLeft_xTop:
            {
                const double xMinValue = xBottom()->getAxisMinValue();
                const double xMaxValue = xBottom()->getAxisMaxValue();
                const double yMinValue = yLeft()->getAxisMinValue();
                const double yMaxValue = yLeft()->getAxisMaxValue();

                const QPoint lastValue( xBottom()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().x(),plot2DXAxis::linear),
                                        yLeft()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().y(),plot2DXAxis::linear) );

                if ( insideCanvas(lastValueF,xMinValue,xMaxValue,yMinValue,yMaxValue) )
                    cachePixelList.insert(0,lastValue);
            }
                break;


            case plot2DXCurve::yRight_xTop:
            {
                const double xMinValue = xBottom()->getAxisMinValue();
                const double xMaxValue = xBottom()->getAxisMaxValue();
                const double yMinValue = yRight()->getAxisMinValue();
                const double yMaxValue = yRight()->getAxisMaxValue();

                const QPoint lastValue( xBottom()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().x(),plot2DXAxis::linear),
                                        yRight()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().y(),plot2DXAxis::linear) );

                if ( insideCanvas(lastValueF,xMinValue,xMaxValue,yMinValue,yMaxValue) )
                    cachePixelList.insert(0,lastValue);
            }
                break;


            case plot2DXCurve::yRight_xBottom:
            {
                const double xMinValue = xBottom()->getAxisMinValue();
                const double xMaxValue = xBottom()->getAxisMaxValue();
                const double yMinValue = yRight()->getAxisMinValue();
                const double yMaxValue = yRight()->getAxisMaxValue();

                const QPoint lastValue( xBottom()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().x(),plot2DXAxis::linear),
                                        yRight()->ConvertToPixel(curve().at(index)->getLastValueBeforeReplot().y(),plot2DXAxis::linear) );

                if ( insideCanvas(lastValueF,xMinValue,xMaxValue,yMinValue,yMaxValue) )
                    cachePixelList.insert(0,lastValue);
            }
                break;


            default:
                break;
            }

            curve().at(index)->swapToContainer();

            if ( !curve().at(index)->isCurveShown() )
                continue;

            canvas()->drawCurve(curve().at(index)->getCurveWidth(),
                                curve().at(index)->getCurveColor(),
                                curve().at(index)->getCurveStyle(),
                                cachePixelList);
        }
    }
}

void plot2DXWidget::updatePlotView()
{
    canvas()->clear();

    if ( isYLeftGridShown() && yLeft()->isVisible() ){

        const double ySpan = fabs(yLeft()->getAxisMaxValue() - yLeft()->getAxisMinValue());
        const int sectorCount = yLeft()->getAxisDistribution();
        const double sectorDelta = ySpan/sectorCount;

        QVector<double> gridYPixelList;

        for ( int i = 0 ; i <= sectorCount ; ++i ){
            const int px = yLeft()->ConvertToPixel(yLeft()->getAxisMinValue() + i*sectorDelta,yLeft()->getAxisScaling());

            gridYPixelList.append(px);
        }

        canvas()->drawYLeftGrid(gridYPixelList, getYLeftGridPen());
    }
    if ( isYRightGridShown() && yRight()->isVisible() ){

        const double ySpan = fabs(yRight()->getAxisMaxValue() - yRight()->getAxisMinValue());
        const int sectorCount = yRight()->getAxisDistribution();
        const double sectorDelta = ySpan/sectorCount;

        QVector<double> gridYPixelList;

        for ( int i = 0 ; i <= sectorCount ; ++i ){
            const int px = yRight()->ConvertToPixel(yRight()->getAxisMinValue() + i*sectorDelta,yRight()->getAxisScaling());

            gridYPixelList.append(px);
        }

        canvas()->drawYRightGrid(gridYPixelList, getYRightGridPen());
    }
    if ( isXBottomGridShown() && xBottom()->isVisible() ){

        const double xSpan = fabs(xBottom()->getAxisMaxValue() - xBottom()->getAxisMinValue());
        const int sectorCount = xBottom()->getAxisDistribution();
        const double sectorDelta = xSpan/sectorCount;

        QVector<double> gridXPixelList;

        for ( int i = 0 ; i <= sectorCount ; ++i ){
            const int px = xBottom()->ConvertToPixel(xBottom()->getAxisMinValue() + i*sectorDelta,xBottom()->getAxisScaling());

            gridXPixelList.append(px);
        }

        canvas()->drawXBottomGrid(gridXPixelList, getXBottomGridPen());
    }
    if ( isXTopGridShown() && xTop()->isVisible() ){

        const double xSpan = fabs(xTop()->getAxisMaxValue() - xTop()->getAxisMinValue());
        const int sectorCount = xTop()->getAxisDistribution();
        const double sectorDelta = xSpan/sectorCount;

        QVector<double> gridXPixelList;

        for ( int i = 0 ; i <= sectorCount ; ++i ){
            const int px = xTop()->ConvertToPixel(xTop()->getAxisMinValue() + i*sectorDelta,xTop()->getAxisScaling());

            gridXPixelList.append(px);
        }

        canvas()->drawXTopGrid(gridXPixelList, getXTopGridPen());
    }

    for ( int i = 0 ; i < MAX_CURVE_NUMBER ; ++i ){
        if ( !curve().at(i)->isCurveShown() )
            continue;


        const QVector<QPoint> clippingList = pixelList(curve().at(i));

        canvas()->drawCurve(curve().at(i)->getCurveWidth(),
                            curve().at(i)->getCurveColor(),
                            curve().at(i)->getCurveStyle(),
                            clippingList
                            );
    }
}

void plot2DXWidget::enableReplot(bool on)
{
    m_replotEnabled = on;
}

void plot2DXWidget::setBackgroundColor(const QColor &color)
{
    m_bgrdColor = color;

    update();


    if ( yLeft() )
        yLeft()->setBackgroundColor(m_bgrdColor);
    if ( yRight() )
        yRight()->setBackgroundColor(m_bgrdColor);
    if ( xTop() )
        xTop()->setBackgroundColor(m_bgrdColor);
    if ( xBottom() )
        xBottom()->setBackgroundColor(m_bgrdColor);

    updatePlotView();
}

void plot2DXWidget::showYLeftGrid(bool on)
{
    m_yLeftGridShown = on;

    updatePlotView();
}

void plot2DXWidget::showYRightGrid(bool on)
{
    m_yRightGridShown = on;

    updatePlotView();
}

void plot2DXWidget::showXBottomGrid(bool on)
{
    m_xBottomGridShown = on;

    updatePlotView();
}

void plot2DXWidget::showXTopGrid(bool on)
{
    m_xTopGridShown = on;

    updatePlotView();
}

void plot2DXWidget::setYLeftGridPen(const QPen &pen)
{
    m_yLeftGridPen = pen;

    updatePlotView();
}

void plot2DXWidget::setYRightGridPen(const QPen &pen)
{
    m_yRightGridPen = pen;

    updatePlotView();
}

void plot2DXWidget::setXBottomGridPen(const QPen &pen)
{
    m_xBottomGridPen = pen;

    updatePlotView();
}

void plot2DXWidget::setXTopGridPen(const QPen &pen)
{
    m_xTopGridPen = pen;

    updatePlotView();
}

void plot2DXWidget::resizeEvent(QResizeEvent *event)
{
    m_canvasRect = QRect(CANVAS_X_OFFSET_ON_VISIBLE,
                         CANVAS_Y_OFFSET_ON_UNVISIBLE,
                         geometry().width()-CANVAS_X_OFFSET_ON_UNVISIBLE-CANVAS_X_OFFSET_ON_VISIBLE,
                         geometry().height()-CANVAS_Y_OFFSET_ON_VISIBLE);

    if ( canvas() )
        canvas()->setGeometry(m_canvasRect);

    if ( yRight() )
        yRight()->adaptGeometry(this->rect(),m_canvasRect);
    if ( yLeft() )
        yLeft()->adaptGeometry(this->rect(),m_canvasRect);
    if ( xTop() )
        xTop()->adaptGeometry(this->rect(),m_canvasRect);
    if ( xBottom() )
        xBottom()->adaptGeometry(this->rect(),m_canvasRect);

    QWidget::resizeEvent(event);
}

void plot2DXWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(this->rect(),m_bgrdColor);

    QWidget::paintEvent(event);
}

QVector<QPoint> plot2DXWidget::pixelList(plot2DXCurve *curve)
{
    QVector<QPoint> pixelList;

    switch (curve->getAxis()){

    case plot2DXCurve::yLeft_xBottom:
    {
        const double minXValue = xBottom()->getAxisMinValue();
        const double maxXValue = xBottom()->getAxisMaxValue();
        const double minYValue = yLeft()->getAxisMinValue();
        const double maxYValue = yLeft()->getAxisMaxValue();

        for ( int i = 0 ; i < curve->getData().size() ; i ++ ){

            const QPointF iterValue = curve->getData().at(i);

            if ( !insideCanvas(iterValue,minXValue,maxXValue,minYValue,maxYValue) )
                continue;

            const QPoint pxPoint(xBottom()->ConvertToPixel(iterValue.x(),
                                                           xBottom()->getAxisScaling()), //x-value

                                 yLeft()->ConvertToPixel(iterValue.y(),
                                                         yLeft()->getAxisScaling() )); //y-value

            pixelList.append(pxPoint);
        }
    }
        break;


    case plot2DXCurve::yLeft_xTop:
    {
        const double minXValue = xTop()->getAxisMinValue();
        const double maxXValue = xTop()->getAxisMaxValue();
        const double minYValue = yLeft()->getAxisMinValue();
        const double maxYValue = yLeft()->getAxisMaxValue();

        for ( int i = 0 ; i < curve->getData().size() ; i ++ ){

            const QPointF iterValue = curve->getData().at(i);

            if ( !insideCanvas(iterValue,minXValue,maxXValue,minYValue,maxYValue) )
                continue;

            const QPoint pxPoint(xTop()->ConvertToPixel(iterValue.x(),
                                                        xTop()->getAxisScaling()), //x-value

                                 yLeft()->ConvertToPixel(iterValue.y(),
                                                         yLeft()->getAxisScaling() )); //y-value

            pixelList.append(pxPoint);
        }
    }
        break;


    case plot2DXCurve::yRight_xBottom:
    {
        const double minXValue = xBottom()->getAxisMinValue();
        const double maxXValue = xBottom()->getAxisMaxValue();
        const double minYValue = yRight()->getAxisMinValue();
        const double maxYValue = yRight()->getAxisMaxValue();

        for ( int i = 0 ; i < curve->getData().size() ; i ++ ){

            const QPointF iterValue = curve->getData().at(i);

            if ( !insideCanvas(iterValue,minXValue,maxXValue,minYValue,maxYValue) )
                continue;

            const QPoint pxPoint(xBottom()->ConvertToPixel(iterValue.x(),
                                                           xBottom()->getAxisScaling()), //x-value

                                 yRight()->ConvertToPixel(iterValue.y(),
                                                          yRight()->getAxisScaling() )); //y-value

            pixelList.append(pxPoint);
        }
    }
        break;


    case plot2DXCurve::yRight_xTop:
    {
        const double minXValue = xTop()->getAxisMinValue();
        const double maxXValue = xTop()->getAxisMaxValue();
        const double minYValue = yRight()->getAxisMinValue();
        const double maxYValue = yRight()->getAxisMaxValue();

        for ( int i = 0 ; i < curve->getData().size() ; i ++ ){

            const QPointF iterValue = curve->getData().at(i);

            if ( !insideCanvas(iterValue,minXValue,maxXValue,minYValue,maxYValue) )
                continue;

            const QPoint pxPoint(xTop()->ConvertToPixel(iterValue.x(),
                                                        xTop()->getAxisScaling()), //x-value

                                 yRight()->ConvertToPixel(iterValue.y(),
                                                          yRight()->getAxisScaling() )); //y-value

            pixelList.append(pxPoint);
        }
    }
        break;


    default:
        break;
    }


    return pixelList;
}

QVector<QPoint> plot2DXWidget::pixelList(const QVector<QPointF> &curve,
                                          plot2DXCurve::scaleAxis axis)
{
    QVector<QPoint> pixelList;

    switch (axis){

    case plot2DXCurve::yLeft_xBottom:
    {
        const double minXValue = xBottom()->getAxisMinValue();
        const double maxXValue = xBottom()->getAxisMaxValue();
        const double minYValue = yLeft()->getAxisMinValue();
        const double maxYValue = yLeft()->getAxisMaxValue();

        for ( int i = 0 ; i < curve.size() ; i ++ ){

            const QPointF iterValue = curve.at(i);

            if ( !insideCanvas(iterValue,minXValue,maxXValue,minYValue,maxYValue) )
                continue;

            const QPoint pxPoint(xBottom()->ConvertToPixel(iterValue.x(),
                                                           xBottom()->getAxisScaling()), //x-value

                                 yLeft()->ConvertToPixel(iterValue.y(),
                                                         yLeft()->getAxisScaling() )); //y-value

            pixelList.append(pxPoint);
        }
    }
        break;


    case plot2DXCurve::yLeft_xTop:
    {
        const double minXValue = xTop()->getAxisMinValue();
        const double maxXValue = xTop()->getAxisMaxValue();
        const double minYValue = yLeft()->getAxisMinValue();
        const double maxYValue = yLeft()->getAxisMaxValue();

        for ( int i = 0 ; i < curve.size() ; i ++ ){

            const QPointF iterValue = curve.at(i);

            if ( !insideCanvas(iterValue,minXValue,maxXValue,minYValue,maxYValue) )
                continue;

            const QPoint pxPoint(xBottom()->ConvertToPixel(iterValue.x(),
                                                           xTop()->getAxisScaling()),    //x-value

                                 yLeft()->ConvertToPixel(iterValue.y(),
                                                         yLeft()->getAxisScaling() )); //y-value

            pixelList.append(pxPoint);
        }
    }
        break;


    case plot2DXCurve::yRight_xBottom:
    {
        const double minXValue = xBottom()->getAxisMinValue();
        const double maxXValue = xBottom()->getAxisMaxValue();
        const double minYValue = yRight()->getAxisMinValue();
        const double maxYValue = yRight()->getAxisMaxValue();

        for ( int i = 0 ; i < curve.size() ; i ++ ){

            const QPointF iterValue = curve.at(i);

            if ( !insideCanvas(iterValue,minXValue,maxXValue,minYValue,maxYValue) )
                continue;

            const QPoint pxPoint(xBottom()->ConvertToPixel(iterValue.x(),
                                                           xBottom()->getAxisScaling()),  //x-value

                                 yRight()->ConvertToPixel(iterValue.y(),
                                                          yRight()->getAxisScaling() )); //y-value

            pixelList.append(pxPoint);
        }
    }
        break;


    case plot2DXCurve::yRight_xTop:
    {
        const double minXValue = xTop()->getAxisMinValue();
        const double maxXValue = xTop()->getAxisMaxValue();
        const double minYValue = yRight()->getAxisMinValue();
        const double maxYValue = yRight()->getAxisMaxValue();

        for ( int i = 0 ; i < curve.size() ; i ++ ){

            const QPointF iterValue = curve.at(i);

            if ( !insideCanvas(iterValue,minXValue,maxXValue,minYValue,maxYValue) )
                continue;

            const QPoint pxPoint(xTop()->ConvertToPixel(iterValue.x(),
                                                        xTop()->getAxisScaling()),     //x-value

                                 yRight()->ConvertToPixel(iterValue.y(),
                                                          yRight()->getAxisScaling() )); //y-value

            pixelList.append(pxPoint);
        }
    }
        break;

    default:
        break;
    }


    return pixelList;
}

bool plot2DXWidget::insideCanvas(const QPointF &point,
                                    double xMin,
                                    double xMax,
                                    double yMin,
                                    double yMax)
{
    if ( point.x() < xMin || point.x() > xMax ||
         point.y() < yMin || point.y() > yMax )
        return false;
    else
        return true;
}

double plot2DXWidget::getMaximumXValue(const QVector<QPointF> &valueList)
{
    if ( valueList.isEmpty() )
        return 0.0;

    double maxXValue = valueList.first().x();

    for ( int i = 0; i < valueList.size() ; i ++ ){
        if ( valueList.at(i).x() > maxXValue )
            maxXValue = valueList.at(i).x();
    }

    return maxXValue;
}

plot2DXCanvas *plot2DXWidget::canvas() const
{
    return m_canvas;
}

plot2DXAxis *plot2DXWidget::yLeft() const
{
    return yLeftAxis;
}

plot2DXAxis *plot2DXWidget::yRight() const
{
    return yRightAxis;
}

plot2DXAxis *plot2DXWidget::xTop() const
{
    return xTopAxis;
}

plot2DXAxis *plot2DXWidget::xBottom() const
{
    return xBottomAxis;
}

QVector<plot2DXCurve *> plot2DXWidget::curve() const
{
    return m_curveList;
}

QColor plot2DXWidget::getBackgroundColor() const
{
    return m_bgrdColor;
}

bool plot2DXWidget::isReplotEnabled() const
{
    return m_replotEnabled;
}

bool plot2DXWidget::isYLeftGridShown() const
{
    return m_yLeftGridShown;
}

bool plot2DXWidget::isYRightGridShown() const
{
    return m_yRightGridShown;
}

bool plot2DXWidget::isXBottomGridShown() const
{
    return m_xBottomGridShown;
}

bool plot2DXWidget::isXTopGridShown() const
{
    return m_xTopGridShown;
}

QPen plot2DXWidget::getYLeftGridPen() const
{
    return m_yLeftGridPen;
}

QPen plot2DXWidget::getYRightGridPen() const
{
    return m_yRightGridPen;
}

QPen plot2DXWidget::getXBottomGridPen() const
{
    return m_xBottomGridPen;
}

QPen plot2DXWidget::getXTopGridPen() const
{
    return m_xTopGridPen;
}

void plot2DXWidget::reset()
{
    for ( int i = 0 ; i < curve().size() ; ++i ){
        curve().at(i)->clearCurveContent();
    }

    canvas()->clear();

    updatePlotView();
}

void plot2DXWidget::autoscale()
{
    if ( xTop()->getAxisPlotType() == plot2DXAxis::timePlot ||
         xBottom()->getAxisPlotType() == plot2DXAxis::timePlot )
        return;


    double yLeftMin = std::numeric_limits<double>::max();
    double yRightMin = std::numeric_limits<double>::max();
    double xBottomMin = std::numeric_limits<double>::max();
    double xTopMin = std::numeric_limits<double>::max();

    double yLeftMax = -std::numeric_limits<double>::max();
    double yRightMax = -std::numeric_limits<double>::max();
    double xBottomMax = -std::numeric_limits<double>::max();
    double xTopMax = -std::numeric_limits<double>::max();

    for ( int i = 0 ; i < MAX_CURVE_NUMBER ; ++i )
    {
        const QVector<QPointF> list = curve()[i]->getData();

        const int listSize = list.size();

        if ( listSize == 0 )
            continue;


        const plot2DXCurve::scaleAxis axis = curve()[i]->getAxis();

        for ( int index = 0 ; index < listSize ; ++index )
        {
            //check for yLeft-axis:
            if ( axis == plot2DXCurve::yLeft_xBottom ||
                 axis == plot2DXCurve::yLeft_xTop )
            {
                if ( list[index].y() < yLeftMin )
                    yLeftMin = list[index].y();
                if ( list[index].y() > yLeftMax )
                    yLeftMax = list[index].y();
            }

            //check for yRight-axis:
            if ( axis == plot2DXCurve::yRight_xBottom ||
                 axis == plot2DXCurve::yRight_xTop )
            {
                if ( list[index].y() < yRightMin )
                    yRightMin = list[index].y();
                if ( list[index].y() > yRightMax )
                    yRightMax = list[index].y();
            }

            //check for xBottom-axis:
            if ( axis == plot2DXCurve::yRight_xBottom ||
                 axis == plot2DXCurve::yLeft_xBottom )
            {
                if ( list[index].x() < xBottomMin )
                    xBottomMin = list[index].x();
                if ( list[index].x() > xBottomMax )
                    xBottomMax = list[index].x();
            }

            //check for xTop-axis:
            if ( axis == plot2DXCurve::yRight_xTop ||
                 axis == plot2DXCurve::yLeft_xTop )
            {
                if ( list[index].x() < xTopMin )
                    xTopMin = list[index].x();
                if ( list[index].x() > xTopMax )
                    xTopMax = list[index].x();
            }
        }
    }

    if ( xBottomMin != std::numeric_limits<double>::max() &&
         xBottomMax != -std::numeric_limits<double>::max() )
        xBottom()->setAxisRange(xBottomMin,xBottomMax);

    if ( xTopMin != std::numeric_limits<double>::max() &&
         xTopMax != -std::numeric_limits<double>::max() )
        xTop()->setAxisRange(xTopMin,xTopMax);

    if ( yLeftMin != std::numeric_limits<double>::max() &&
         yLeftMax != -std::numeric_limits<double>::max() )
        yLeft()->setAxisRange(yLeftMin,yLeftMax);

    if ( yRightMin != std::numeric_limits<double>::max() &&
         yRightMax != -std::numeric_limits<double>::max() )
        yRight()->setAxisRange(yRightMin,yRightMax);
}
