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

#include "plot2DXAxis.h"

#if WIN32
#include <iostream>
#endif

#include "stdlib.h"
#include "math.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

#define DEFAULT_AXIS_VALUE_MIN            0.0     //[a.u.] or [ms]
#define DEFAULT_AXIS_VALUE_MAX            10000.0 //[a.u.] or [ms]
#define DEFAULT_AXIS_DISTRIBUTION         5
#define DEFAULT_INCREMENT_LENGTH          3
#define DEFAULT_HELP_INCREMENT_LENGTH     2
#define DEFAULT_HELP_AXIS_DISTRIBUTION    5
#define DEFAULT_TEXT_FONT                 QFont("Helvetica",9)
#define DEFAULT_INCREMENT_TEXT_COLOR      QColor(Qt::black)
#define DEFAULT_AXIS_WIDTH                1   //[px]
#define DEFAULT_AXIS_COLOR                QColor(Qt::black)
#define DEFAULT_NUMBER_FORMAT             'f' //for detail see: Qt-docs: QString::number(...,...,...)
#define DEFAULT_NUMBER_PRECISION          3
#define DEFAULT_AXIS_LABEL_TEXT           QString("_axisLabel_")
#define DEFAULT_AXIS_LABEL_COLOR          QColor(Qt::black)

#define AXIS_TO_BORDER_SPACE              4   //[px]
#define AXIS_TO_BORDER_OFFSET             30  //[px]
#define INCREMENT_TEXT_TO_INCREMENT_SPACE 4   //[px]

using namespace std;

plot2DXAxis::plot2DXAxis(plot2DXAxis::axisDockingPosition docking,
                                   plot2DXAxis::axisPlotType plotType,
                                   axisScaling scaling,
                                   const QRect &rect,
                                   QWidget *parent) :
    QWidget(parent),

    m_docking(docking),
    m_axisPlotType(plotType),
    m_axisScaling(scaling),
    m_axisMinValue(DEFAULT_AXIS_VALUE_MIN),
    m_axisMaxValue(DEFAULT_AXIS_VALUE_MAX),
    m_lastDisplayedValue(m_axisMaxValue),
    m_axisSpan(m_axisMaxValue-m_axisMinValue),
    m_axisDistribution(DEFAULT_AXIS_DISTRIBUTION),
    m_width(0),
    m_height(0),
    m_xOffset(0),
    m_yOffset(0),
    m_incrementLength(DEFAULT_INCREMENT_LENGTH),
    m_helpIncrementLength(DEFAULT_HELP_INCREMENT_LENGTH),
    m_helpAxisDistribution(DEFAULT_HELP_AXIS_DISTRIBUTION),
    m_showHelpIncrements(true),
    m_textFont(DEFAULT_TEXT_FONT),
    m_incrementTextColor(DEFAULT_INCREMENT_TEXT_COLOR),
    m_axisColor(DEFAULT_AXIS_COLOR),
    m_axisWidth(DEFAULT_AXIS_WIDTH),
    m_numberFormat(DEFAULT_NUMBER_FORMAT),
    m_numberPrecision(DEFAULT_NUMBER_PRECISION),
    m_labelPosition(valueStart),
    m_axisLabelText(DEFAULT_AXIS_LABEL_TEXT),
    m_axisLabelColor(DEFAULT_AXIS_LABEL_COLOR),
    m_axisLabelShown(true),
    m_valueDisplay(numbers),
    m_visible(true)
{
    setFont(getLabelFont());

    m_axisPen.setColor(m_axisColor);
    m_axisPen.setWidth(m_axisWidth);

    m_incrementPen.setColor(m_incrementTextColor);

    m_labelPen.setColor(m_axisLabelColor);

    setGeometry(calculateDockingGeometry(parent->geometry(),rect));
}

plot2DXAxis::~plot2DXAxis(){}

void plot2DXAxis::setAxisRange(double axisMin, double axisMax)
{
    if ( axisMin > axisMax ){
        m_axisMinValue = axisMax;
        m_axisMaxValue = axisMin;
    }else{
        m_axisMinValue = axisMin;
        m_axisMaxValue = axisMax;
    }

    m_axisSpan = (m_axisMaxValue - m_axisMinValue);

    if ( axisMax <= m_axisSpan )
        m_lastDisplayedValue = m_axisMaxValue;

    updateAxis();

    if ( getAxisPlotType() != timePlot )
        emit scalingPropertyChanged();
}

void plot2DXAxis::setAxisMinValue(double axisMin)
{
    if ( axisMin > getAxisMaxValue() ){
        m_axisMinValue = m_axisMaxValue;
        m_axisMaxValue = axisMin;
    }else{
        m_axisMinValue = axisMin;
    }

    m_axisSpan = (m_axisMaxValue - m_axisMinValue);
    m_lastDisplayedValue = m_axisMaxValue;

    updateAxis();

    if ( getAxisPlotType() != timePlot )
        emit scalingPropertyChanged();
}

void plot2DXAxis::setAxisMaxValue(double axisMax)
{
    if ( axisMax < getAxisMinValue() ){
        m_axisMaxValue = m_axisMinValue;
        m_axisMinValue = axisMax;
    }else{
        m_axisMaxValue = axisMax;
    }

    m_axisSpan = (m_axisMaxValue - m_axisMinValue);
    m_lastDisplayedValue = m_axisMaxValue;

    updateAxis();

    if ( getAxisPlotType() != timePlot )
        emit scalingPropertyChanged();
}

void plot2DXAxis::setAxisPlotType(plot2DXAxis::axisPlotType plotType)
{
    const bool bDocking =  ( getAxisDockingPosition() == yLeft || getAxisDockingPosition() == yRight );
    const bool bTimePlot = ( plotType == timePlot );

    if (bDocking && bTimePlot){
#ifdef QT_DEBUG
        qDebug() << "error: time-value plot-mode is only valid for the x-axis!";
#endif
        return;
    }

    m_axisPlotType = plotType;

    updateAxis();

    emit scalingPropertyChanged();
}

void plot2DXAxis::setAxisScaling(plot2DXAxis::axisScaling scaling)
{
    if ( getAxisPlotType() == timePlot &&
         scaling != linear ){
#ifdef QT_DEBUG
        qDebug() << "error: time-value plot-mode provides only a linear axis-scaling!";
#endif
        return;
    }

    m_axisScaling = scaling;

    updateAxis();

    emit scalingPropertyChanged();
}

void plot2DXAxis::setAxisDistribution(int count)
{
//    if ( count <= 1 ){
//#ifdef QT_DEBUG
//        qDebug() << "error: axis-distribution must be > 1!";
//#endif
//        return;
//    }

    m_axisDistribution = count;
    m_lastDisplayedValue = getAxisMaxValue();

    updateAxis();

    emit scalingPropertyChanged();
}

void plot2DXAxis::setIncrementLength(int length)
{
    if ( length < 1 ){
#ifdef QT_DEBUG
        qDebug() << "error: axis-increment length must be > 1!";
#endif
        return;
    }

    m_incrementLength = length;

    updateAxis();
}

void plot2DXAxis::setHelpIncrementLength(int length)
{
    if ( length < 1 ){
#ifdef QT_DEBUG
        qDebug() << "error: axis-helpincrement length must be > 1!";
#endif
        return;
    }

    m_helpIncrementLength = length;

    updateAxis();
}

void plot2DXAxis::setAxisHelpDistribution(int distribution)
{
    if ( distribution <= 1 ){
#ifdef QT_DEBUG
        qDebug() << "error: axis-help-distribution must be > 1!";
#endif
        return;
    }

    m_helpAxisDistribution = distribution;

    updateAxis();
}

void plot2DXAxis::setLabelFont(const QFont &font)
{
    m_textFont = font;

    updateAxis();
}

void plot2DXAxis::setIncrementLabelColor(const QColor &color)
{
    m_incrementTextColor = color;
    m_incrementPen.setColor(color);

    updateAxis();
}

void plot2DXAxis::setAxisColor(const QColor &color)
{
    m_axisColor = color;
    m_axisPen.setColor(color);

    updateAxis();
}

void plot2DXAxis::setAxisWidth(int width)
{
    if ( width < 1 ){
#ifdef QT_DEBUG
        qDebug() << "error: axis-width must be > 1!";
#endif
        return;
    }

    m_axisWidth = width;
    m_axisPen.setWidth(width);

    updateAxis();
}

void plot2DXAxis::setNumberFormat(plot2DXAxis::numberFormat format)
{
    switch ( format ){
    case floating:
        m_numberFormat = 'f';
        break;
    case exponential:
        m_numberFormat = 'e';
        break;
    case automatic:
        m_numberFormat = 'g';
        break;

    default:
        break;
    }

    updateAxis();
}

void plot2DXAxis::setNumberPrecision(int precision)
{
    if ( precision < 1 && precision > 10 ){
#ifdef QT_DEBUG
        qDebug() << "error: number-precision 'x' must be in range:  1 <= x <= 10 ";
#endif
        return;
    }

    m_numberPrecision = precision;

    updateAxis();
}

void plot2DXAxis::setAxisLabelPosition(plot2DXAxis::axisLabelPosition position)
{
    m_labelPosition = position;

    updateAxis();
}

void plot2DXAxis::setAxisLabelText(const QString &text)
{
    m_axisLabelText = text;

    updateAxis();
}

void plot2DXAxis::setAxisLabelColor(const QColor &color)
{
    m_axisLabelColor = color;
    m_labelPen.setColor(color);

    updateAxis();
}

void plot2DXAxis::showAxisLabel(bool show)
{
    m_axisLabelShown = show;

    updateAxis();
}

void plot2DXAxis::showHelpIncrements(bool show)
{
    m_showHelpIncrements = show;

    updateAxis();
}

void plot2DXAxis::setValueDisplay(plot2DXAxis::axisValueDisplay display)
{
    m_valueDisplay = display;

    updateAxis();
}

void plot2DXAxis::setVisible(bool visible)
{
    /* It might be possible there should be an axis-adaption if only a few axis should be shown.
       In that case the setVisible() function of QWidget must be overwritten */
    /*if (visible){}
    else{}

    updateAxis();
    */

    emit visibleStateChanged(visible);

    QWidget::setVisible(visible);
}

void plot2DXAxis::setBackgroundColor(const QColor &color)
{
    m_bgrdColor = color;

    updateAxis();
}

double plot2DXAxis::getAxisMinValue() const
{
    return m_axisMinValue;
}

double plot2DXAxis::getAxisMaxValue() const
{
    return m_axisMaxValue;
}

double plot2DXAxis::getAxisSpan() const
{
    return m_axisSpan;
}

int plot2DXAxis::getAxisDistribution() const
{
    return m_axisDistribution;
}

plot2DXAxis::axisDockingPosition plot2DXAxis::getAxisDockingPosition() const
{
    return m_docking;
}

plot2DXAxis::axisPlotType plot2DXAxis::getAxisPlotType() const
{
    return m_axisPlotType;
}

plot2DXAxis::axisScaling plot2DXAxis::getAxisScaling() const
{
    return m_axisScaling;
}

int plot2DXAxis::getIncrementLength() const
{
    return m_incrementLength;
}

int plot2DXAxis::getHelpIncrementLength() const
{
    return m_helpIncrementLength;
}

int plot2DXAxis::getAxisHelpDistribution() const
{
    return m_helpAxisDistribution;
}

QFont plot2DXAxis::getLabelFont() const
{
    return m_textFont;
}

QColor plot2DXAxis::getIncrementLabelColor() const
{
    return m_incrementTextColor;
}

QColor plot2DXAxis::getAxisColor() const
{
    return m_axisColor;
}

int plot2DXAxis::getAxisWidth() const
{
    return m_axisWidth;
}

int plot2DXAxis::getAxisLength() const
{
    if ( m_docking == yLeft || m_docking == yRight )
        return ( m_height - 2*AXIS_TO_BORDER_OFFSET );
    else
        return ( m_width - 2*AXIS_TO_BORDER_OFFSET );
}

plot2DXAxis::numberFormat plot2DXAxis::getNumberFormat() const
{
    switch ( m_numberFormat ){
    case 'f':
        return floating;
    case 'e':
        return exponential;
    case 'g':
        return automatic;
    default:
        return automatic;
    }

    Q_UNREACHABLE();
}

int plot2DXAxis::getNumberPrecision() const
{
    return m_numberPrecision;
}

plot2DXAxis::axisLabelPosition plot2DXAxis::getAxisLabelPosition() const
{
    return m_labelPosition;
}

QString plot2DXAxis::getAxisLabelText() const
{
    return m_axisLabelText;
}

QColor plot2DXAxis::getAxisLabelColor() const
{
    return m_axisLabelColor;
}

bool plot2DXAxis::isAxisLabelShown() const
{
    return m_axisLabelShown;
}

bool plot2DXAxis::isHelpIncrementShown() const
{
    return m_showHelpIncrements;
}

plot2DXAxis::axisValueDisplay plot2DXAxis::getValueDisplay() const
{
    return m_valueDisplay;
}

QColor plot2DXAxis::getBackgroundColor() const
{
    return m_bgrdColor;
}

QRect plot2DXAxis::calculateDockingGeometry(const QRect &parentGeometry,
                                                 const QRect &canvasGeometry)
{
    int x_offset = 0;
    int y_offset = 0;
    int width = 0;
    int height = 0;


    switch ( getAxisDockingPosition() )
    {
    case yLeft:
    {
        /*x_offset = 0;*/
        y_offset = canvasGeometry.y();
        width    = /*parentGeometry.width() - canvasGeometry.bottomRight().x()*/canvasGeometry.bottomLeft().x();//TODO!!!!!
        height   = canvasGeometry.height();
    }
        break;


    case yRight:
    {
        x_offset = canvasGeometry.bottomRight().x();
        y_offset = canvasGeometry.y();
        width    = parentGeometry.width() - canvasGeometry.bottomRight().x();
        height   = canvasGeometry.height();
    }
        break;


    case xTop:
    {
        x_offset = canvasGeometry.x();
        /*y_offset = 0;*/
        width    = canvasGeometry.width();
        height   = canvasGeometry.top();
    }
        break;


    case xBottom:
    {
        x_offset = canvasGeometry.x();
        y_offset = canvasGeometry.bottom();
        width    = canvasGeometry.width();
        height   = parentGeometry.height() - canvasGeometry.bottom();
    }
        break;


    default:
        /*x_offset = 0;
        y_offset = 0;
        width = 0;
        height = 0;*/
        break;
    }

    m_width   = width;
    m_height  = height;
    m_xOffset = x_offset;
    m_yOffset = y_offset;


    return QRect(m_xOffset,m_yOffset,m_width,m_height);
}

int plot2DXAxis::ConvertToPixel(double point,
                                     plot2DXAxis::axisScaling scaling)
{
    int pxValue = 0;

    switch ( scaling ){
    case linear:
    {
        if ( m_docking == yLeft || m_docking == yRight ){
            pxValue = (double)m_height - (double)AXIS_TO_BORDER_OFFSET - (( point - getAxisMinValue() )/(getAxisSpan()))*(double)getAxisLength();
        }else{
            pxValue = (double)AXIS_TO_BORDER_OFFSET + (( point - getAxisMinValue() )/(getAxisSpan()))*(double)getAxisLength();
        }
    }
        break;
    case logarithmic:
    {
        if ( m_docking == yLeft || m_docking == yRight ){
            pxValue = (double)m_height - (double)AXIS_TO_BORDER_OFFSET - (( log10(point) - log10(getAxisMinValue()) )/( log10(getAxisMaxValue()) - log10(getAxisMinValue()) ))*(double)getAxisLength();
        }else{
            pxValue = (double)AXIS_TO_BORDER_OFFSET + (( log10(point) - log10(getAxisMinValue()) )/(log10(getAxisMaxValue()) - log10(getAxisMinValue())))*(double)getAxisLength();
        }
    }
        break;
    default:
        break;
    }

    return pxValue;
}

double plot2DXAxis::ConvertFromPixel(int point,
                                          plot2DXAxis::axisScaling scaling)
{
    double dblValue = 0;

    switch ( scaling ){
    case linear:
    {
        if ( m_docking == yLeft || m_docking == yRight ){
            dblValue = (-((double)point - (double)m_height + (double)AXIS_TO_BORDER_OFFSET)*(getAxisSpan()/(double)getAxisLength())) + getAxisMinValue();
        }else{
            dblValue = ((double)point - (double)AXIS_TO_BORDER_OFFSET)*(getAxisSpan()/(double)getAxisLength()) + getAxisMinValue();
        }
    }
        break;
    case logarithmic:
    {
        if ( m_docking == yLeft || m_docking == yRight ){
            dblValue = (double)getAxisMaxValue()*pow(10,((double)m_height-(double)point-(double)AXIS_TO_BORDER_OFFSET)/(double)getAxisLength());
        }else{
            dblValue = (double)getAxisMaxValue()*pow(10,((double)point-(double)AXIS_TO_BORDER_OFFSET)/(double)getAxisLength());
        }
    }
        break;
    default:
        break;
    }

    return dblValue;
}

void plot2DXAxis::adaptGeometry(const QRect &parentGeometry,
                                     const QRect &canvasGeometry)
{
    setGeometry(calculateDockingGeometry(parentGeometry,
                                         canvasGeometry));

    updateAxis();

    emit scalingPropertyChanged();
}

void plot2DXAxis::updateAxis()
{
    update(); //repaint();
}

void plot2DXAxis::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    renew();
}

void plot2DXAxis::renew()
{
    drawAxis();
    drawIncrements();
    drawAxisLabel();
}

void plot2DXAxis::drawAxis()
{
    QPainter axisPainter(this);

    axisPainter.setPen(m_axisPen);

    switch ( getAxisDockingPosition() )
    {
    case yLeft:
    {
        //draw axis:
        const QLine axis_yLeft(m_width - AXIS_TO_BORDER_SPACE,AXIS_TO_BORDER_OFFSET,
                               m_width - AXIS_TO_BORDER_SPACE,m_height - AXIS_TO_BORDER_OFFSET);

        axisPainter.fillRect(this->rect(),getBackgroundColor());

        axisPainter.drawLine(axis_yLeft);
    }
        break;


    case yRight:
    {
        //draw axis:
        const QLine axis_yRight(AXIS_TO_BORDER_SPACE,AXIS_TO_BORDER_OFFSET,
                                AXIS_TO_BORDER_SPACE,m_height - AXIS_TO_BORDER_OFFSET);

        axisPainter.fillRect(this->rect(),getBackgroundColor());

        axisPainter.drawLine(axis_yRight);
    }
        break;


    case xTop:
    {
        //draw axis:
        const QLine axis_xTop(AXIS_TO_BORDER_OFFSET,m_height - AXIS_TO_BORDER_SPACE,
                              m_width - AXIS_TO_BORDER_OFFSET,m_height - AXIS_TO_BORDER_SPACE);

        axisPainter.fillRect(this->rect(),getBackgroundColor());

        axisPainter.drawLine(axis_xTop);
    }
        break;

    case xBottom:
    {
        //draw axis:
        const QLine axis_xBottom(AXIS_TO_BORDER_OFFSET,AXIS_TO_BORDER_SPACE,
                                 m_width - AXIS_TO_BORDER_OFFSET,AXIS_TO_BORDER_SPACE);

        axisPainter.fillRect(this->rect(),getBackgroundColor());

        axisPainter.drawLine(axis_xBottom);
    }
        break;

    default:
        break;
    }

    axisPainter.end();
}

void plot2DXAxis::drawIncrements()
{
    QPainter axisPainter(this);

    axisPainter.setPen(m_axisPen);


    const double axisStep     = getAxisSpan()/(double)(getAxisDistribution());
    const double helpAxisStep = axisStep/(double)(getAxisHelpDistribution());

    switch ( getAxisDockingPosition() )
    {
    case yLeft:
    {
        for ( int i = 0 ; i <= getAxisDistribution() ; i ++ ){

            const double value  = getAxisMinValue() + i*axisStep;
            const int stepPixel = ConvertToPixel( value,getAxisScaling() );

            const QPoint incrementStartPoint(m_width - AXIS_TO_BORDER_SPACE,stepPixel);
            const QPoint incrementEndPoint(m_width - AXIS_TO_BORDER_SPACE - getIncrementLength(),stepPixel);

            axisPainter.drawLine( incrementStartPoint,
                                  incrementEndPoint );

            //draw help-increments;
            if ( isHelpIncrementShown() && i != getAxisDistribution() ){
                for ( int a = 0 ; a < getAxisHelpDistribution() ; a ++ ){

                    const double value  = getAxisMinValue() + i*axisStep + a*helpAxisStep;
                    const int stepPixel = ConvertToPixel( value,getAxisScaling() );

                    const QPoint incrementStartPoint(m_width - AXIS_TO_BORDER_SPACE,stepPixel);
                    const QPoint incrementEndPoint(m_width - AXIS_TO_BORDER_SPACE - getHelpIncrementLength(),stepPixel);

                    axisPainter.drawLine( incrementStartPoint,
                                          incrementEndPoint );
                }
            }

            //draw increment labels:
            axisPainter.save();

            drawIncrementLabelText(&axisPainter,
                                   value,
                                   incrementEndPoint);
            axisPainter.restore();
        }
    }
        break;


    case yRight:
    {
        for ( int i = 0 ; i <= getAxisDistribution() ; i ++ ){

            const double value  = getAxisMinValue() + i*axisStep;
            const int stepPixel = ConvertToPixel( value,getAxisScaling() );

            const QPoint incrementStartPoint(AXIS_TO_BORDER_SPACE,stepPixel);
            const QPoint incrementEndPoint(AXIS_TO_BORDER_SPACE + getIncrementLength(),stepPixel);

            axisPainter.drawLine(incrementStartPoint,
                                 incrementEndPoint );

            //draw help-increments;
            if ( isHelpIncrementShown() && i != getAxisDistribution() ){
                for ( int a = 0 ; a < getAxisHelpDistribution() ; a ++ ){

                    const double value  = getAxisMinValue() + i*axisStep + a*helpAxisStep;
                    const int stepPixel = ConvertToPixel( value,getAxisScaling() );

                    const QPoint incrementStartPoint(AXIS_TO_BORDER_SPACE,stepPixel);
                    const QPoint incrementEndPoint(AXIS_TO_BORDER_SPACE + getHelpIncrementLength(),stepPixel);

                    axisPainter.drawLine( incrementStartPoint,
                                          incrementEndPoint );
                }
            }


            //draw increment labels:
            axisPainter.save();

            axisPainter.setPen(m_incrementPen);

            drawIncrementLabelText(&axisPainter,
                                   value,
                                   incrementEndPoint);
            axisPainter.restore();
        }
    }
        break;


    case xTop:
    {
        if ( getAxisPlotType() == valuePlot )
        {
            for ( int i = 0 ; i <= getAxisDistribution() ; i ++ ){

                const double value  = getAxisMinValue() + i*axisStep;
                const int stepPixel = ConvertToPixel( value,getAxisScaling() );

                const QPoint incrementStartPoint(stepPixel, m_height - AXIS_TO_BORDER_SPACE);
                const QPoint incrementEndPoint(stepPixel, m_height - AXIS_TO_BORDER_SPACE - getIncrementLength());

                axisPainter.drawLine( incrementStartPoint,
                                      incrementEndPoint );

                //draw help-increments;
                if ( isHelpIncrementShown() && i != getAxisDistribution() ){
                    for ( int a = 0 ; a < getAxisHelpDistribution() ; a ++ ){

                        const double value  = getAxisMinValue() + i*axisStep + a*helpAxisStep;
                        const int stepPixel = ConvertToPixel( value,getAxisScaling() );

                        const QPoint incrementStartPoint(stepPixel, m_height - AXIS_TO_BORDER_SPACE);
                        const QPoint incrementEndPoint(stepPixel, m_height - AXIS_TO_BORDER_SPACE - getHelpIncrementLength());

                        axisPainter.drawLine( incrementStartPoint,
                                              incrementEndPoint );
                    }
                }

                //draw increment labels:
                axisPainter.save();

                drawIncrementLabelText(&axisPainter,
                                       value,
                                       incrementEndPoint);
                axisPainter.restore();
            }
        }

        else if ( getAxisPlotType() == timePlot ){

            if ( getAxisMaxValue() > getAxisSpan() ){

                if ( fabs(getAxisMaxValue() - m_lastDisplayedValue) >= axisStep ){
                    if ( fabs(getAxisMaxValue() - m_lastDisplayedValue) > getAxisSpan() )
                        m_lastDisplayedValue = getAxisMaxValue();
                    else
                        m_lastDisplayedValue = getAxisMaxValue() - fabs(fabs(getAxisMaxValue() - m_lastDisplayedValue) - axisStep);
                }

                for ( int i = getAxisDistribution() + 1 ; i >= 0  ; i -- ){

                    const double value = m_lastDisplayedValue - i*axisStep;
                    const int timeXValue = ConvertToPixel(value,linear);


                    //draw help-increments;
                    if ( isHelpIncrementShown() ){
                        for ( int a = getAxisHelpDistribution() + 1 ; a >= 0 ; a -- ){

                            const double value  = m_lastDisplayedValue - (i-1)*axisStep - (a-1)*helpAxisStep;
                            const int stepPixel = ConvertToPixel( value,linear );

                            if ( value < getAxisMinValue() ||
                                 value > getAxisMaxValue() )
                                continue;

                            const QPoint incrementStartPoint(stepPixel, m_height - AXIS_TO_BORDER_SPACE);
                            const QPoint incrementEndPoint(stepPixel, m_height - AXIS_TO_BORDER_SPACE - getHelpIncrementLength());

                            axisPainter.drawLine( incrementStartPoint,
                                                  incrementEndPoint );
                        }
                    }


                    if ( value < getAxisMinValue() )
                        continue;

                    const QPoint incrementStartPoint(timeXValue, m_height - AXIS_TO_BORDER_SPACE);
                    const QPoint incrementEndPoint(timeXValue, m_height - AXIS_TO_BORDER_SPACE - getIncrementLength());

                    axisPainter.drawLine( incrementStartPoint,
                                          incrementEndPoint );


                    //draw increment labels:
                    axisPainter.save();

                    drawIncrementLabelText(&axisPainter,
                                           value,
                                           incrementEndPoint);
                    axisPainter.restore();
                }
            }
            else
            {
                if ( fabs(getAxisMaxValue() - m_lastDisplayedValue) >= axisStep ){
                    if ( fabs(getAxisMaxValue() - m_lastDisplayedValue) > getAxisSpan() )
                        m_lastDisplayedValue = getAxisMaxValue();
                    else
                        m_lastDisplayedValue = getAxisMaxValue() - fabs(fabs(getAxisMaxValue() - m_lastDisplayedValue) - axisStep);
                }else{
                    m_lastDisplayedValue = getAxisMaxValue();
                }

                for ( int i = 0 ; i <= getAxisDistribution() + 1 ; i ++ ){

                    const double value  = getAxisMinValue() + i*axisStep;
                    const int stepPixel = ConvertToPixel( value,getAxisScaling() );


                    //draw help-increments;
                    if ( isHelpIncrementShown() ){
                        for ( int a = getAxisHelpDistribution() + 1 ; a >= 0 ; a -- ){

                            const double value  = m_lastDisplayedValue - (i-1)*axisStep - (a-1)*helpAxisStep;
                            const int stepPixel = ConvertToPixel( value,linear );

                            if ( value < getAxisMinValue() ||
                                 value > getAxisMaxValue() )
                                continue;

                            const QPoint incrementStartPoint(stepPixel, m_height - AXIS_TO_BORDER_SPACE);
                            const QPoint incrementEndPoint(stepPixel, m_height - AXIS_TO_BORDER_SPACE - getHelpIncrementLength());

                            axisPainter.drawLine( incrementStartPoint,
                                                  incrementEndPoint );
                        }
                    }

                    const QPoint incrementStartPoint(stepPixel, m_height - AXIS_TO_BORDER_SPACE);
                    const QPoint incrementEndPoint(stepPixel, m_height - AXIS_TO_BORDER_SPACE - getIncrementLength());

                    axisPainter.drawLine( incrementStartPoint,
                                          incrementEndPoint );

                    //draw increment labels:
                    axisPainter.save();

                    drawIncrementLabelText(&axisPainter,
                                           value,
                                           incrementEndPoint);
                    axisPainter.restore();
                }
            }
        }
    }
        break;


    case xBottom:
    {
        if ( getAxisPlotType() == valuePlot )
        {
            for ( int i = 0 ; i <= getAxisDistribution() ; i ++ ){

                const double value  = getAxisMinValue() + i*axisStep;
                const int stepPixel = ConvertToPixel( value,getAxisScaling() );

                const QPoint incrementStartPoint(stepPixel, AXIS_TO_BORDER_SPACE);
                const QPoint incrementEndPoint(stepPixel, AXIS_TO_BORDER_SPACE + getIncrementLength());

                axisPainter.drawLine( incrementStartPoint,
                                      incrementEndPoint );

                //draw help-increments;
                if ( isHelpIncrementShown() && i != getAxisDistribution() ){
                    for ( int a = 0 ; a < getAxisHelpDistribution() ; a ++ ){

                        const double value  = getAxisMinValue() + i*axisStep + a*helpAxisStep;
                        const int stepPixel = ConvertToPixel( value,getAxisScaling() );

                        const QPoint incrementStartPoint(stepPixel, AXIS_TO_BORDER_SPACE);
                        const QPoint incrementEndPoint(stepPixel, AXIS_TO_BORDER_SPACE + getHelpIncrementLength());

                        axisPainter.drawLine( incrementStartPoint,
                                              incrementEndPoint );
                    }
                }

                //draw increment labels:
                axisPainter.save();

                drawIncrementLabelText(&axisPainter,
                                       value,
                                       incrementEndPoint);
                axisPainter.restore();
            }
        }

        else if ( getAxisPlotType() == timePlot ){

            if ( getAxisMaxValue() > getAxisSpan() ){

                if ( fabs(getAxisMaxValue() - m_lastDisplayedValue) >= axisStep ){
                    if ( fabs(getAxisMaxValue() - m_lastDisplayedValue) > getAxisSpan() )
                        m_lastDisplayedValue = getAxisMaxValue();
                    else
                        m_lastDisplayedValue = getAxisMaxValue() - fabs(fabs(getAxisMaxValue() - m_lastDisplayedValue) - axisStep);
                }

                for ( int i = getAxisDistribution() + 1 ; i >= 0 ; i -- ){

                    const double value = m_lastDisplayedValue - i*axisStep;
                    const int timeXValue = ConvertToPixel(value,linear);

                    //draw help-increments;
                    if ( isHelpIncrementShown() ){
                        for ( int a = getAxisHelpDistribution() + 1 ; a >= 0 ; a -- ){

                            const double value  = m_lastDisplayedValue - (i-1)*axisStep - (a-1)*helpAxisStep;
                            const int stepPixel = ConvertToPixel( value,linear );

                            if ( value < getAxisMinValue() ||
                                 value > getAxisMaxValue() )
                                continue;

                            const QPoint incrementStartPoint(stepPixel, AXIS_TO_BORDER_SPACE);
                            const QPoint incrementEndPoint(stepPixel, AXIS_TO_BORDER_SPACE + getHelpIncrementLength());

                            axisPainter.drawLine( incrementStartPoint,
                                                  incrementEndPoint );
                        }
                    }


                    if ( value < getAxisMinValue() )
                        continue;

                    const QPoint incrementStartPoint(timeXValue, AXIS_TO_BORDER_SPACE);
                    const QPoint incrementEndPoint(timeXValue, AXIS_TO_BORDER_SPACE + getIncrementLength());

                    axisPainter.drawLine( incrementStartPoint,
                                          incrementEndPoint );

                    //draw increment labels:
                    axisPainter.save();

                    drawIncrementLabelText(&axisPainter,
                                           value,
                                           incrementEndPoint);
                    axisPainter.restore();
                }
            }
            else
            {
                if ( fabs(getAxisMaxValue() - m_lastDisplayedValue) >= axisStep ){
                    if ( fabs(getAxisMaxValue() - m_lastDisplayedValue) > getAxisSpan() )
                        m_lastDisplayedValue = getAxisMaxValue();
                    else
                        m_lastDisplayedValue = getAxisMaxValue() - fabs(fabs(getAxisMaxValue() - m_lastDisplayedValue) - axisStep);
                }else{
                    m_lastDisplayedValue = getAxisMaxValue();
                }

                for ( int i = 0 ; i <= getAxisDistribution() + 1 ; i ++ ){

                    const double value  = getAxisMinValue() + i*axisStep;
                    const int stepPixel = ConvertToPixel( value,linear );

                    const QPoint incrementStartPoint(stepPixel, AXIS_TO_BORDER_SPACE);
                    const QPoint incrementEndPoint(stepPixel, AXIS_TO_BORDER_SPACE + getIncrementLength());

                    //draw help-increments;
                    if ( isHelpIncrementShown() ){
                        for ( int a = getAxisHelpDistribution() + 1 ; a >= 0 ; a -- ){

                            const double value  = m_lastDisplayedValue - (i-1)*axisStep - (a-1)*helpAxisStep;
                            const int stepPixel = ConvertToPixel( value,linear );

                            if ( value < getAxisMinValue() ||
                                 value > getAxisMaxValue() )
                                continue;

                            const QPoint incrementStartPoint(stepPixel, AXIS_TO_BORDER_SPACE);
                            const QPoint incrementEndPoint(stepPixel, AXIS_TO_BORDER_SPACE + getHelpIncrementLength());

                            axisPainter.drawLine( incrementStartPoint,
                                                  incrementEndPoint );
                        }
                    }

                    axisPainter.drawLine( incrementStartPoint,
                                          incrementEndPoint );

                    //draw increment labels:
                    axisPainter.save();

                    drawIncrementLabelText(&axisPainter,
                                           value,
                                           incrementEndPoint);
                    axisPainter.restore();
                }
            }
        }
    }
        break;


    default:
        break;
    }

    axisPainter.end();
}

void plot2DXAxis::drawAxisLabel()
{
    if ( !isAxisLabelShown() )
        return;

    QPainter painter(this);

    painter.setPen(m_labelPen);

    const QFontMetrics metrics(getLabelFont());

    const QString labelText = getAxisLabelText();

    const int heightF = metrics.tightBoundingRect(labelText).height();
    const int lengthFleft = metrics.tightBoundingRect(labelText).width();

    QPoint point(0,0);


    switch ( getAxisDockingPosition() ){

    case yLeft:
    {
        switch ( getAxisLabelPosition() ){
        case valueStart:
            point = QPoint(AXIS_TO_BORDER_SPACE,m_height - AXIS_TO_BORDER_SPACE);
            break;
        case valueEnd:
            point = QPoint(AXIS_TO_BORDER_SPACE,AXIS_TO_BORDER_SPACE + heightF);
            break;
        case middle:
            point = QPoint(AXIS_TO_BORDER_SPACE,AXIS_TO_BORDER_SPACE + (double)m_height/2);
            break;
        default:
            break;
        }
    }
        break;

    case yRight:
    {
        switch ( getAxisLabelPosition() ){
        case valueStart:
            point = QPoint(m_width - AXIS_TO_BORDER_SPACE - lengthFleft,m_height - AXIS_TO_BORDER_SPACE);
            break;
        case valueEnd:
            point = QPoint(m_width - AXIS_TO_BORDER_SPACE - lengthFleft,AXIS_TO_BORDER_SPACE + heightF);
            break;
        case middle:
            point = QPoint(m_width - AXIS_TO_BORDER_SPACE - lengthFleft,AXIS_TO_BORDER_SPACE + (double)m_height/2);
            break;
        default:
            break;
        }
    }
        break;

    case xTop:
    {
        switch ( getAxisLabelPosition() ){
        case valueStart:
            point = QPoint(AXIS_TO_BORDER_SPACE,AXIS_TO_BORDER_SPACE + heightF);
            break;
        case valueEnd:
            point = QPoint(m_width - lengthFleft - AXIS_TO_BORDER_SPACE,AXIS_TO_BORDER_SPACE + heightF);
            break;
        case middle:
            point = QPoint((double)m_width/2 - (double)lengthFleft/2 - AXIS_TO_BORDER_SPACE,AXIS_TO_BORDER_SPACE + heightF);
            break;
        default:
            break;
        }
    }
        break;

    case xBottom:
    {
        switch ( getAxisLabelPosition() ){
        case valueStart:
            point = QPoint(AXIS_TO_BORDER_SPACE,m_height - AXIS_TO_BORDER_SPACE);
            break;
        case valueEnd:
            point = QPoint(m_width - lengthFleft - AXIS_TO_BORDER_SPACE,m_height - AXIS_TO_BORDER_SPACE);
            break;
        case middle:
            point = QPoint((double)m_width/2 - (double)lengthFleft/2 - AXIS_TO_BORDER_SPACE,m_height - AXIS_TO_BORDER_SPACE);
            break;
        default:
            break;
        }
    }
        break;

    default:
        break;
    }

    painter.drawText(point,getAxisLabelText());

    painter.end();
}

void plot2DXAxis::drawIncrementLabelText(QPainter *painter,
                                              double value,
                                              const QPoint &incrementEndPoint)
{
    painter->setPen(m_incrementPen);


    const QString valueString(labelDisplayText(value));

    const QFontMetrics metrics(getLabelFont());

    const int heightF = metrics.tightBoundingRect(valueString).height();
    const int lengthFleft = metrics.tightBoundingRect(valueString).width();
    const int lengthFRight = metrics.lineWidth();


    switch ( getAxisDockingPosition() ){

    case yLeft:
        painter->drawText( QPoint(incrementEndPoint.x() - lengthFleft - INCREMENT_TEXT_TO_INCREMENT_SPACE ,
                                  incrementEndPoint.y() + (double)heightF/2),
                           valueString);
        break;

    case yRight:
        painter->drawText( QPoint(incrementEndPoint.x() + lengthFRight + INCREMENT_TEXT_TO_INCREMENT_SPACE ,
                                  incrementEndPoint.y() + (double)heightF/2),
                           valueString);
        break;

    case xTop:
        painter->drawText( QPoint(incrementEndPoint.x() - (double)lengthFleft/2,
                                  incrementEndPoint.y() - INCREMENT_TEXT_TO_INCREMENT_SPACE ),
                           valueString);
        break;

    case xBottom:
        painter->drawText( QPoint(incrementEndPoint.x() - (double)lengthFleft/2,
                                  incrementEndPoint.y() + heightF + INCREMENT_TEXT_TO_INCREMENT_SPACE ),
                           valueString);
        break;

    default:
        break;
    }
}

QString plot2DXAxis::labelDisplayText(double value)
{
    QString displayString = "";

    switch ( getValueDisplay() )
    {
    case numbers:
        displayString = QString::number(value,m_numberFormat,getNumberPrecision());
        break;


    case hh_mm_ss:
    {
        int seconds = qRound(value/1000.0);

        const int _hour = seconds/3600;
        const int _minute = (seconds % 3600)/60;
        const int _seconds = (seconds % 3600) % 60;


        QString hourString;
        QString minuteString;
        QString secondString;

        if ( _hour < 10 )
            hourString = "0" + QVariant(_hour).toString();
        else
            hourString = QVariant(_hour).toString();

        if ( _minute < 10 )
            minuteString = "0" + QVariant(_minute).toString();
        else
            minuteString = QVariant(_minute).toString();

        if ( _seconds < 10 )
            secondString = "0" + QVariant(_seconds).toString();
        else
            secondString = QVariant(_seconds).toString();


        displayString = hourString + ":" + minuteString + "," + secondString;
    }
        break;


    case hh_mm:
    {
        int seconds = qRound(value/60000.0);

        const int _hour = (seconds % 3600)/60;
        const int _minute = (seconds % 3600) % 60;


        QString hourString;
        QString minuteString;

        if ( _hour < 10 )
            hourString = "0" + QVariant(_hour).toString();
        else
            hourString = QVariant(_hour).toString();

        if ( _minute < 10 )
            minuteString = "0" + QVariant(_minute).toString();
        else
            minuteString = QVariant(_minute).toString();


        displayString = hourString + ":" + minuteString;
    }
        break;


    case mm_ss:
    {
        int seconds = qRound(value/1000.0);

        const int _hour = seconds/3600;
        const int _minute = (seconds % 3600)/60 + _hour*60;
        const int _seconds = (seconds % 3600) % 60;


        QString minuteString;
        QString secondString;

        if ( _minute < 10 )
            minuteString = "0" + QVariant(_minute).toString();
        else
            minuteString = QVariant(_minute).toString();

        if ( _seconds < 10 )
            secondString = "0" + QVariant(_seconds).toString();
        else
            secondString = QVariant(_seconds).toString();


        displayString = minuteString + ":" + secondString;
    }
        break;


    case ss:
    {
        const double seconds = value/1000.0;

        displayString = QString::number(seconds,m_numberFormat,getNumberPrecision());
    }
        break;


    default:
        break;
    }


    return displayString;
}
