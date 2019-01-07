/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2019 Danny Petschke
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

#include "slider.h"

DVerticalDoubleSlider::DVerticalDoubleSlider(QWidget *parent) :
    QWidget(parent),
    m_min(0.0f),
    m_max(10.0f),
    m_valueLS(0.0f),
    m_valueUS(10.0f),
    m_stepWidth(1.0f),
    m_color(DVERTICAL_SLIDER_COLOR) {}

DVerticalDoubleSlider::~DVerticalDoubleSlider() {}

double DVerticalDoubleSlider::valueLowerLimit() const
{
    return m_valueLS;
}

double DVerticalDoubleSlider::valueUpperLimit() const
{
    return m_valueUS;
}

double DVerticalDoubleSlider::minimum() const
{
    return m_min;
}

double DVerticalDoubleSlider::maximum() const
{
    return m_max;
}

DColor DVerticalDoubleSlider::color() const
{
    return m_color;
}

void DVerticalDoubleSlider::setRange(double min, double max)
{
    if ( max <= min )
        return;

    setMinimum(min);
    setMaximum(max);

    update();
}

void DVerticalDoubleSlider::setMinimum(double value)
{
    m_min = value;
}

void DVerticalDoubleSlider::setMaximum(double value)
{
    m_max = value;
}

bool DVerticalDoubleSlider::setValueLowerSlider(double value)
{
    if ( value < m_min || value > m_max || value > m_valueUS || fabsf(value-m_valueUS) < m_stepWidth )
        return false;

    m_valueLS = value;

    emit lowerValueChanged(m_valueLS);
    emit valuesChanged(m_valueLS, m_valueUS);

    rangeChanged(m_valueLS, m_valueUS);

    update();

    return true;
}

bool DVerticalDoubleSlider::setValueUpperSlider(double value)
{
    if ( value < m_min || value > m_max || value < m_valueLS || fabsf(value-m_valueLS) < m_stepWidth )
        return false;

    m_valueUS = value;

    emit upperValueChanged(m_valueUS);
    emit valuesChanged(m_valueLS, m_valueUS);

    rangeChanged(m_valueLS, m_valueUS);

    update();

    return true;
}

void DVerticalDoubleSlider::setStepWidth(double stepWidth)
{
    if ( fabs(stepWidth) > fabs(m_min-m_max) )
        return;

    m_stepWidth = stepWidth;
}

void DVerticalDoubleSlider::setColor(const DColor &color)
{
    m_color = color;
}

void DVerticalDoubleSlider::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing);

    const double lSliderYvalue = convertToYPixel(m_valueLS);
    const double uSliderYvalue = convertToYPixel(m_valueUS);

    QPen axisPen(Qt::lightGray); axisPen.setWidth(2);
    QPen axisPenLower(m_color); axisPenLower.setWidth(2);
    QPen axisPenUpper(m_color); axisPenUpper.setWidth(2);

    p.setPen(axisPen);
    p.drawLine(QPointF((float)geometry().width()/2.0f, DVERTICAL_SLIDER_RADIUS), QPointF((float)geometry().width()/2.0f, geometry().height()-DVERTICAL_SLIDER_RADIUS));
    p.setPen(axisPenLower);
    p.drawLine(QPointF((float)geometry().width()/2.0f, geometry().height()-DVERTICAL_SLIDER_RADIUS), QPointF((float)geometry().width()/2.0f, lSliderYvalue - DVERTICAL_SLIDER_RADIUS));
    p.setPen(axisPenUpper);
    p.drawLine(QPointF((float)geometry().width()/2.0f, uSliderYvalue - DVERTICAL_SLIDER_RADIUS), QPointF((float)geometry().width()/2.0f, DVERTICAL_SLIDER_RADIUS));


    QPen upperSliderPen(m_color); upperSliderPen.setWidth(1);
    QPen lowerSliderPen(m_color);  lowerSliderPen.setWidth(1);

    m_lSRect = QRectF((float)geometry().width()/2.0f - DVERTICAL_SLIDER_RADIUS, lSliderYvalue - DVERTICAL_SLIDER_RADIUS-1, 2.0f*DVERTICAL_SLIDER_RADIUS, 2.0f*DVERTICAL_SLIDER_RADIUS);
    m_uSRect = QRectF((float)geometry().width()/2.0f - DVERTICAL_SLIDER_RADIUS, uSliderYvalue - DVERTICAL_SLIDER_RADIUS+1, 2.0f*DVERTICAL_SLIDER_RADIUS, 2.0f*DVERTICAL_SLIDER_RADIUS);

    p.setBrush(m_color);

    p.setPen(lowerSliderPen);
    p.drawEllipse(m_lSRect);

    p.setPen(upperSliderPen);
    p.drawEllipse(m_uSRect);

    QWidget::paintEvent(event);
}

void DVerticalDoubleSlider::mousePressEvent(QMouseEvent *event)
{
    m_lSActive = m_uSActive = false;

    if ( m_lSRect.contains(event->pos()) )
        m_lSActive = true;
    else if ( m_uSRect.contains(event->pos()) )
        m_uSActive = true;

    QWidget::mousePressEvent(event);
}

void DVerticalDoubleSlider::mouseMoveEvent(QMouseEvent *event)
{
    if ( m_lSActive )
    {
        if ( event->pos().y() > geometry().height()-DVERTICAL_SLIDER_RADIUS || event->pos().y() < DVERTICAL_SLIDER_RADIUS || convertFromYPixel(event->pos().y()) >= m_valueUS )
            return;

        if ( fabsf(convertFromYPixel(event->pos().y())-m_valueUS) < m_stepWidth )
            return;

        m_valueLS = convertFromYPixel(event->pos().y());

        emit lowerValueChanged(m_valueLS);
        emit valuesChanged(m_valueLS, m_valueUS);

        rangeChanged(m_valueLS, m_valueUS);
    }
    else if ( m_uSActive )
    {
        if ( event->pos().y() > geometry().height()-DVERTICAL_SLIDER_RADIUS || event->pos().y() < DVERTICAL_SLIDER_RADIUS || convertFromYPixel(event->pos().y()) <= m_valueLS )
            return;

        if ( fabsf(convertFromYPixel(event->pos().y())-m_valueLS) < m_stepWidth )
            return;

        m_valueUS = convertFromYPixel(event->pos().y());

        emit upperValueChanged(m_valueUS);
        emit valuesChanged(m_valueLS, m_valueUS);

        rangeChanged(m_valueLS, m_valueUS);
    }

    update();

    QWidget::mouseMoveEvent(event);
}

void DVerticalDoubleSlider::mouseReleaseEvent(QMouseEvent *event)
{
    m_lSActive = m_uSActive = false;

    QWidget::mouseReleaseEvent(event);
}

int DVerticalDoubleSlider::convertToYPixel(double v)
{
    return DVERTICAL_SLIDER_RADIUS + (geometry().height()-2.0f*DVERTICAL_SLIDER_RADIUS)*(m_max-v)/(m_max-m_min);
}

double DVerticalDoubleSlider::convertFromYPixel(int px)
{
    return m_max-((double)px-DVERTICAL_SLIDER_RADIUS)*(m_max-m_min)/(geometry().height()-2.0f*DVERTICAL_SLIDER_RADIUS);
}


DHorizontalDoubleSlider::DHorizontalDoubleSlider(QWidget *parent) :
    QWidget(parent),
    m_min(0.0f),
    m_max(10.0f),
    m_valueLS(0.0f),
    m_valueUS(10.0f),
    m_stepWidth(1.0f),
    m_color(DHORIZONTAL_SLIDER_COLOR) {}

DHorizontalDoubleSlider::~DHorizontalDoubleSlider() {}

double DHorizontalDoubleSlider::valueLowerLimit() const
{
    return m_valueLS;
}

double DHorizontalDoubleSlider::valueUpperLimit() const
{
    return m_valueUS;
}

double DHorizontalDoubleSlider::minimum() const
{
    return m_min;
}

double DHorizontalDoubleSlider::maximum() const
{
    return m_max;
}

DColor DHorizontalDoubleSlider::color() const
{
    return m_color;
}

void DHorizontalDoubleSlider::setRange(double min, double max)
{
    if ( max <= min )
        return;

    setMinimum(min);
    setMaximum(max);

    update();
}

void DHorizontalDoubleSlider::setMinimum(double value)
{
    m_min = value;
}

void DHorizontalDoubleSlider::setMaximum(double value)
{
    m_max = value;
}

bool DHorizontalDoubleSlider::setValueLowerSlider(double value)
{
    if ( value < m_min || value > m_max || value > m_valueUS || fabsf(value-m_valueUS) < m_stepWidth )
        return false;

    m_valueLS = value;

    emit lowerValueChanged(m_valueLS);
    emit valuesChanged(m_valueLS, m_valueUS);

    rangeChanged(m_valueLS, m_valueUS);

    update();

    return true;
}

bool DHorizontalDoubleSlider::setValueUpperSlider(double value)
{
    if ( value < m_min || value > m_max || value < m_valueLS || fabsf(value-m_valueLS) < m_stepWidth )
        return false;

    m_valueUS = value;

    emit upperValueChanged(m_valueUS);
    emit valuesChanged(m_valueLS, m_valueUS);

    rangeChanged(m_valueLS, m_valueUS);

    update();

    return true;
}

void DHorizontalDoubleSlider::setStepWidth(double stepWidth)
{
    if ( fabs(stepWidth) > fabs(m_min-m_max) )
        return;

    m_stepWidth = stepWidth;
}

void DHorizontalDoubleSlider::setColor(const DColor &color)
{
    m_color = color;
}

void DHorizontalDoubleSlider::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing);

    const double lSliderYvalue = convertToXPixel(m_valueLS);
    const double uSliderYvalue = convertToXPixel(m_valueUS);

    QPen axisPen(Qt::lightGray); axisPen.setWidth(2);
    QPen axisPenLower(m_color); axisPenLower.setWidth(2);
    QPen axisPenUpper(m_color); axisPenUpper.setWidth(2);

    p.setPen(axisPen);
    p.drawLine(QPointF(DHORIZONTAL_SLIDER_RADIUS, (float)geometry().height()/2.0f), QPointF(geometry().width()-DHORIZONTAL_SLIDER_RADIUS, (float)geometry().height()/2.0f));
    p.setPen(axisPenLower);
    p.drawLine(QPointF(DHORIZONTAL_SLIDER_RADIUS, (float)geometry().height()/2.0f), QPointF(lSliderYvalue - DHORIZONTAL_SLIDER_RADIUS, (float)geometry().height()/2.0f));
    p.setPen(axisPenUpper);
    p.drawLine(QPointF(uSliderYvalue - DHORIZONTAL_SLIDER_RADIUS, (float)geometry().height()/2.0f), QPointF(geometry().width()-DHORIZONTAL_SLIDER_RADIUS, (float)geometry().height()/2.0f));


    QPen upperSliderPen(m_color); upperSliderPen.setWidth(1);
    QPen lowerSliderPen(m_color);  lowerSliderPen.setWidth(1);

    m_lSRect = QRectF(lSliderYvalue - DHORIZONTAL_SLIDER_RADIUS+1, (float)geometry().height()/2.0f - DHORIZONTAL_SLIDER_RADIUS, 2.0f*DHORIZONTAL_SLIDER_RADIUS, 2.0f*DHORIZONTAL_SLIDER_RADIUS);
    m_uSRect = QRectF(uSliderYvalue - DHORIZONTAL_SLIDER_RADIUS-1, (float)geometry().height()/2.0f - DHORIZONTAL_SLIDER_RADIUS, 2.0f*DHORIZONTAL_SLIDER_RADIUS, 2.0f*DHORIZONTAL_SLIDER_RADIUS);

    p.setBrush(m_color);

    p.setPen(lowerSliderPen);
    p.drawEllipse(m_lSRect);

    p.setPen(upperSliderPen);
    p.drawEllipse(m_uSRect);

    QWidget::paintEvent(event);
}

void DHorizontalDoubleSlider::mousePressEvent(QMouseEvent *event)
{
    m_lSActive = m_uSActive = false;

    if ( m_lSRect.contains(event->pos()) )
        m_lSActive = true;
    else if ( m_uSRect.contains(event->pos()) )
        m_uSActive = true;

    QWidget::mousePressEvent(event);
}

void DHorizontalDoubleSlider::mouseMoveEvent(QMouseEvent *event)
{
    if ( m_lSActive )
    {
        if ( event->pos().x() > geometry().width()-DHORIZONTAL_SLIDER_RADIUS || event->pos().x() < DHORIZONTAL_SLIDER_RADIUS || convertFromXPixel(event->pos().x()) >= m_valueUS )
            return;

        if ( fabsf(convertFromXPixel(event->pos().x())-m_valueUS) < m_stepWidth )
            return;

        m_valueLS = convertFromXPixel(event->pos().x());

        emit lowerValueChanged(m_valueLS);
        emit valuesChanged(m_valueLS, m_valueUS);

        rangeChanged(m_valueLS, m_valueUS);
    }
    else if ( m_uSActive )
    {
        if ( event->pos().x() > geometry().width()-DHORIZONTAL_SLIDER_RADIUS || event->pos().x() < DHORIZONTAL_SLIDER_RADIUS || convertFromXPixel(event->pos().x()) <= m_valueLS )
            return;

        if ( fabsf(convertFromXPixel(event->pos().x())-m_valueLS) < m_stepWidth )
            return;

        m_valueUS = convertFromXPixel(event->pos().x());

        emit upperValueChanged(m_valueUS);
        emit valuesChanged(m_valueLS, m_valueUS);

        rangeChanged(m_valueLS, m_valueUS);
    }

    update();

    QWidget::mouseMoveEvent(event);
}

void DHorizontalDoubleSlider::mouseReleaseEvent(QMouseEvent *event)
{
    m_lSActive = m_uSActive = false;

    QWidget::mouseReleaseEvent(event);
}

int DHorizontalDoubleSlider::convertToXPixel(double v)
{
    return DHORIZONTAL_SLIDER_RADIUS + (geometry().width()-2.0f*DHORIZONTAL_SLIDER_RADIUS)*(v-m_min)/(m_max-m_min);
}

double DHorizontalDoubleSlider::convertFromXPixel(int px)
{
    return ((double)px-DHORIZONTAL_SLIDER_RADIUS)*(m_max-m_min)/(geometry().width()-2.0f*DHORIZONTAL_SLIDER_RADIUS) + m_min;
}
