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

#include "plot2DXCurve.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

#define DEFAULT_CURVE_COLOR     Qt::red
#define DEFAULT_CURVE_WIDTH     2

#define CONTAINER_MAX_VALUE     100000000 //maximum count of data-pairs <double,double> the container contains

plot2DXCurve::plot2DXCurve() :
    m_scale(yLeft_xBottom),
    m_lineWidth(DEFAULT_CURVE_WIDTH),
    m_lineColor(DEFAULT_CURVE_COLOR),
    m_curveStyle(line),
    m_shown(true),
    m_lastValueBeforeReplot(QPointF(.0,.0)),
    m_maxCount(CONTAINER_MAX_VALUE),
    x_min(INT_MAX),
    y_min(INT_MAX),
    x_max(-INT_MAX),
    y_max(-INT_MAX){}

plot2DXCurve::~plot2DXCurve()
{
    clearCurveContent();
}

void plot2DXCurve::setAxis(plot2DXCurve::scaleAxis axis)
{
    m_scale = axis;

    emit curvePropertyChanged();
}

void plot2DXCurve::setCurveWidth(int width)
{
    m_lineWidth = width;

    emit curvePropertyChanged();
}

void plot2DXCurve::setCurveColor(const QColor &color)
{
    m_lineColor = color;

    emit curvePropertyChanged();
}

void plot2DXCurve::setCurveStyle(plot2DXCurve::curveStyle style)
{
    m_curveStyle = style;

    emit curvePropertyChanged();
}

void plot2DXCurve::showCurve(bool show)
{
    m_shown = show;

    emit curvePropertyChanged();
}

void plot2DXCurve::addData(double x_value, double y_value)
{
    const QPointF dataPair(x_value,y_value);

    m_cache.append(dataPair);
}

void plot2DXCurve::addData(const QVector<QPointF> &dataset)
{
    m_cache.append(dataset);
}

void plot2DXCurve::addData(const QVector<QPointF> &dataset, bool bDirection)
{
    if (bDirection) {
        m_cache.append(dataset);
    }
    else {
        for (int i = dataset.size()-1 ; i >= 0 ; -- i) {
            m_cache.append(dataset.at(i));
        }
    }
}

void plot2DXCurve::addDataVec(const QVector<int> &dataset)
{
    for ( int i = 0 ; i < dataset.size() ; ++ i ) {
        m_cache.append(QPointF(i, dataset.at(i)));
    }
}

void plot2DXCurve::addData(const QPointF &dataPoint)
{
    m_cache.append(dataPoint);
}

void plot2DXCurve::clearCurveContent()
{
    m_dataContainer.clear();
    m_cache.clear();

    emit curvePropertyChanged();
}

void plot2DXCurve::clearCurveContent(int from, int to)
{
    bool validIndex = (from >= 0 && to >= 0 && from < to );
    bool validSize  = ( to < m_dataContainer.size() );

    if ( !validIndex || !validSize )
        return;

    for ( int i = from ; i <= to ; i ++ ){
        m_dataContainer.removeAt(i);
    }

    emit curvePropertyChanged();
}

void plot2DXCurve::clearCurveCache(int from, int to)
{
    bool validIndex = (from >= 0 && to >= 0 && from < to );
    bool validSize  = ( to < m_cache.size() );

    if ( !validIndex || !validSize )
        return;

    for ( int i = from ; i <= to ; i ++ ){
        m_cache.removeAt(i);
    }
}

void plot2DXCurve::setMaxContainerSize(int size)
{
    if (size <= 1){
#ifdef QT_DEBUG
        qDebug() << "error: maximum size of data-container must be > 1!";
#endif
        return;
    }

    m_maxCount = size;

    emit curvePropertyChanged();
}

void plot2DXCurve::clearCurveCache()
{
    reset();
}

void plot2DXCurve::swapToContainer()
{
    const int cacheSize = m_cache.size();

    if ( cacheSize == 0 )
        return;


    bool extremeValue = false;

    for ( int i = 0 ; i < cacheSize ; ++i ){
        const QPointF value = m_cache[i];

        if ( value.x() < x_min ){
            x_min = value.x();
            extremeValue = true;
        }
        else if ( value.x() > x_max ){
            x_max = value.x();
            extremeValue = true;
        }

        if ( value.y() < y_min ){
            y_min = value.y();
            extremeValue = true;
        }
        else if ( value.y() > y_max ){
            y_max = value.y();
            extremeValue = true;
        }

        m_dataContainer.append(value);
    }

    if ( extremeValue )
        emit maxValueChanged(x_min,y_min,x_max,y_max,getAxis());


    //store the last value before swapping:
    setLastValueBeforeReplot(m_cache.last());

    //reduce container to limit:
    while ( m_dataContainer.size() > m_maxCount ){
        m_dataContainer.removeFirst();
    };

    //clear cache:
    reset();
}

plot2DXCurve::scaleAxis plot2DXCurve::getAxis() const
{
    return m_scale;
}

int plot2DXCurve::getCurveWidth() const
{
    return m_lineWidth;
}

QColor plot2DXCurve::getCurveColor() const
{
    return m_lineColor;
}

plot2DXCurve::curveStyle plot2DXCurve::getCurveStyle() const
{
    return m_curveStyle;
}

bool plot2DXCurve::isCurveShown() const
{
    return m_shown;
}

QVector<QPointF> plot2DXCurve::getData() const
{
    return m_dataContainer;
}

QVector<QPointF> plot2DXCurve::getCache() const
{
    return m_cache;
}

int plot2DXCurve::setMaxContainerSize() const
{
    return m_maxCount;
}

int plot2DXCurve::getDataSize() const
{
    return m_dataContainer.size();
}

int plot2DXCurve::getCacheSize() const
{
    return m_cache.size();
}

void plot2DXCurve::reset()
{
    m_cache.clear();
}

void plot2DXCurve::setLastValueBeforeReplot(const QPointF &lastValue)
{
    m_lastValueBeforeReplot = lastValue;
}

QPointF plot2DXCurve::getLastValueBeforeReplot() const
{
    return m_lastValueBeforeReplot;
}
