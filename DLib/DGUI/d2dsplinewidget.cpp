#include "d2dsplinewidget.h"
#include "../DMath/dspline.h"

D2DSplineWidget::D2DSplineWidget(QWidget *parent) :
    QWidget(parent),
    m_minX(0), m_minY(0), m_maxX(1.0), m_maxY(1.0),
    m_splineMinY(0),
    m_splineMaxY(1.0),
    m_ctrlPRectLength(8),
    m_splineResolution(5000),
    m_activeCtrlP(0),
    m_spline(0),
    m_autoScale(false)
{
    m_ctrlPPen.setColor(QColor(56, 166, 126));
    m_ctrlPPen.setBrush(QColor(186, 199, 194));
    m_ctrlPPen.setWidthF(1.0);
    m_ctrlPPen.setJoinStyle(Qt::RoundJoin);


    m_polygonPen.setColor(QColor(117, 111, 111));
    m_ctrlPPen.setWidthF(1.0);
    m_ctrlPPen.setJoinStyle(Qt::RoundJoin);

    m_ctrlPoints.append(new D2DSplineControlPoint(QPointF(m_minX+0.02, m_minY+0.02), QSize(m_ctrlPRectLength, m_ctrlPRectLength)));
    m_ctrlPoints.append(new D2DSplineControlPoint(QPointF(m_maxX-0.02, m_maxY-0.02), QSize(m_ctrlPRectLength, m_ctrlPRectLength)));

    connect(this, SIGNAL(calculateSpline()), this, SLOT(updateSplinePath()));
    emit calculateSpline();
}

D2DSplineWidget::~D2DSplineWidget(){}

QPen D2DSplineWidget::ctrlPointPen() const
{
    return m_ctrlPPen;
}

QPen D2DSplineWidget::splinePen() const
{
    return m_polygonPen;
}

double D2DSplineWidget::xMin() const
{
    return m_minX;
}

double D2DSplineWidget::yMin() const
{
    return m_minY;
}

double D2DSplineWidget::xMax() const
{
    return m_maxX;
}

double D2DSplineWidget::yMax() const
{
    return m_maxY;
}

int D2DSplineWidget::splineResolution() const
{
    return m_splineResolution;
}

bool D2DSplineWidget::isAutoscaleEnabled() const
{
    return m_autoScale;
}

double D2DSplineWidget::getInterpolatedYValue(double x) const
{
    if ( m_spline )
        return (*m_spline)(x);
    else
        return 0.0;
}

QList<QPointF> D2DSplineWidget::getCtrlPoints() const
{
    QList<QPointF> ctrlList;
    for ( int i = 0 ; i < m_ctrlPoints.size() ; ++ i ){
        ctrlList.append(m_ctrlPoints.at(i)->point());
    }

    return ctrlList;
}

void D2DSplineWidget::paintEvent(QPaintEvent *event)
{
    drawBackground(event);
    drawForground(event);

    QWidget::paintEvent(event);
}

void D2DSplineWidget::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() & Qt::LeftButton ){
        for ( int i = 0 ; i < m_ctrlPoints.size() ; ++ i ){
            if ( containsPoint(m_ctrlPoints.at(i), event->pos()) ){
                m_activeCtrlP = m_ctrlPoints.at(i);

                break;
            }
        }
    }
    else if ( event->button() & Qt::RightButton ){
        bool ctrlPRemoved = false;
        if ( m_ctrlPoints.size() > 2 ){
            for ( int i = 0 ; i < m_ctrlPoints.size() ; ++ i ){
                if ( containsPoint(m_ctrlPoints.at(i), event->pos()) ){
                    removeCtrlPoint(m_ctrlPoints.at(i));
                    ctrlPRemoved = true;
                    break;
                }
            }
        }

        if ( !ctrlPRemoved )
            addCtrlPoint(event->pos());
    }

    QWidget::mousePressEvent(event);
}

void D2DSplineWidget::mouseMoveEvent(QMouseEvent *event)
{
    //if ( event->button() & Qt::LeftButton ){


        if ( !m_activeCtrlP )
            return;


        const QPointF p(convertFromPixel(event->pos().x()-m_activeCtrlP->width()/2, event->pos().y()-m_activeCtrlP->height()/2));

        m_activeCtrlP->setPosition(p);
        emit calculateSpline();

    //}

    QWidget::mouseMoveEvent(event);
}

void D2DSplineWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_activeCtrlP = 0;

    QWidget::mouseReleaseEvent(event);
}

void D2DSplineWidget::drawForground(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    painter.setPen(m_polygonPen);

    //draw Spline:
    if ( m_spline ){
        const double splineIncrement = (m_maxX-m_minX)/m_splineResolution;

        QPointF *path = new QPointF[m_splineResolution+2];
        int pcnt   = 0;
        path[pcnt] = convertToPixel(m_minX, (*m_spline)(m_minX));
        pcnt ++;

        for ( double x = m_minX+splineIncrement ; x <= m_maxX ; x += splineIncrement ){
            path[pcnt] = convertToPixel(x, (*m_spline)(x));
            pcnt ++;
        }

        painter.drawPolyline(path, pcnt);
        delete path;
        path = 0;
    }


    painter.setPen(m_ctrlPPen);

    //draw Control-Points:
    for ( int i = 0 ; i < m_ctrlPoints.size() ; ++ i ){
        QRect ctrlPRect(convertToPixel(m_ctrlPoints.at(i)->rect().x(), m_ctrlPoints.at(i)->rect().y()), QSize(m_ctrlPoints.at(i)->rect().size().toSize()));

        ctrlPRect.setX(ctrlPRect.x() - m_ctrlPoints.at(i)->width() /2);
        ctrlPRect.setY(ctrlPRect.y() - m_ctrlPoints.at(i)->height()/2);

        painter.setPen(Qt::NoPen);
        painter.setBrush(m_ctrlPPen.brush());
        painter.drawEllipse(ctrlPRect);

        painter.setPen(m_ctrlPPen);
        painter.drawEllipse(ctrlPRect);
    }
}

void D2DSplineWidget::drawBackground(QPaintEvent *event)
{
    //reserved for your style ;)!
}

QPoint D2DSplineWidget::convertToPixel(double x, double y) const
{
    if ( !m_autoScale ){
        const int pxX = geometry().width()*(x-m_minX)/(m_maxX-m_minX);
        const int pxY = geometry().height() - geometry().height()*(y-m_minY)/(m_maxY-m_minY);

        return QPoint(pxX, pxY);
    }else{
        const int pxX = geometry().width()*(x-m_minX)/(m_maxX-m_minX);
        const int pxY = geometry().height() - geometry().height()*(y-m_splineMinY)/(m_splineMaxY-m_splineMinY);

        return QPoint(pxX, pxY);
    }

    Q_UNREACHABLE();
}

QPointF D2DSplineWidget::convertFromPixel(int x, int y) const
{
    if ( !m_autoScale ){
        const double xVal = m_minX + ((double)x/(double)geometry().width())*(m_maxX-m_minX);
        const double yVal = m_minY + (((double)geometry().height()-(double)y)*(m_maxY-m_minY)/(double)geometry().height());

        return QPointF(xVal, yVal);
    }else{
        const double xVal = m_minX + ((double)x/(double)geometry().width())*(m_maxX-m_minX);
        const double yVal = m_splineMinY + (((double)geometry().height()-(double)y)*(m_splineMaxY-m_splineMinY)/(double)geometry().height());

        return QPointF(xVal, yVal);
    }

    Q_UNREACHABLE();
}

bool D2DSplineWidget::containsPoint(D2DSplineControlPoint *ctrlP, const QPoint &p)
{
    const QPoint screenP = convertToPixel(ctrlP->x(), ctrlP->y());
    const QSize  size    = ctrlP->rect().size().toSize();

    return p.x() >= screenP.x() && p.x() <= screenP.x()+size.width() && p.y() >= screenP.y() && p.y() <= screenP.y()+size.height();
}

void D2DSplineWidget::addCtrlPoint(const QPoint &pixelCoord)
{
    m_ctrlPoints.append(new D2DSplineControlPoint(convertFromPixel(pixelCoord.x(), pixelCoord.y()), QSize(m_ctrlPRectLength, m_ctrlPRectLength)));
    emit calculateSpline();
}

void D2DSplineWidget::removeCtrlPoint(D2DSplineControlPoint *ctrlP)
{
    if ( !ctrlP )
        return;

    D2DSplineControlPoint* cpyCtrlP = m_ctrlPoints.takeAt(m_ctrlPoints.indexOf(ctrlP));

    if ( cpyCtrlP ){
        delete cpyCtrlP;
        cpyCtrlP = 0;
    }

    emit calculateSpline();
}

void D2DSplineWidget::updateSplinePath()
{
    std::vector<double> xValues(m_ctrlPoints.size()), yValues(m_ctrlPoints.size());

    m_ctrlPoints.quickSortAscending();
    for ( int i = 0 ; i < m_ctrlPoints.size() ; ++ i ){
        xValues[i] = m_ctrlPoints.at(i)->x();
        yValues[i] = m_ctrlPoints.at(i)->y();
    }

    if ( m_spline ){
        delete m_spline;
        m_spline = 0;
    }

    m_spline = new spline;
    m_spline->set_points(xValues, yValues);

    if ( m_autoScale ){
        const double splineIncrement = (m_maxX-m_minX)/m_splineResolution;

        m_splineMaxY = m_maxY;
        m_splineMinY = m_minY;

        for ( double x = m_minX+splineIncrement ; x <= m_maxX ; x += splineIncrement ){
            const double y = (*m_spline)(x);
            m_splineMaxY = qMax(m_splineMaxY, y);
            m_splineMinY = qMin(m_splineMinY, y);
        }
    }

    update();
}

void D2DSplineWidget::setXRange(double xMin, double xMax)
{
    m_minX = xMin;
    m_maxX = xMax;

    emit calculateSpline();
}

void D2DSplineWidget::setYRange(double yMin, double yMax)
{
    m_minY = yMin;
    m_maxY = yMax;

    emit calculateSpline();
}

void D2DSplineWidget::setCtrlPointPen(const QPen &pen)
{
    m_ctrlPPen = pen;
    update();
}

void D2DSplineWidget::setSplinePen(const QPen &pen)
{
    m_polygonPen = pen;
    update();
}

void D2DSplineWidget::setResolution(int cnt)
{
    m_splineResolution = cnt;
    update();
}

void D2DSplineWidget::enableAutoscale(bool enable)
{
    m_autoScale = enable;
    updateSplinePath();
}

void D2DSplineWidget::setCtrlPoints(const QList<QPointF> ctrlPoints)
{
    for ( int i = 0 ; i <= m_ctrlPoints.size() ; ++ i ){
        D2DSplineControlPoint *p = m_ctrlPoints.takeFirst();
        if ( p ){
            delete p;
            p = 0;
        }
    }

    m_ctrlPoints.clear();

    for ( int a = 0 ; a < ctrlPoints.size() ; ++ a ){
        addCtrlPoint(convertToPixel(ctrlPoints.at(a).x(), ctrlPoints.at(a).y()));
    }

    updateSplinePath();
}


void D2DSplineCtrlPointList::quickSortAscending()
{
    quickSortAscending(0, size()-1);
}

bool D2DSplineCtrlPointList::quickSortAscending(long int left, long int right)
{
    const bool error = right == 0 ;

    if ( error )
        return false;

    long int i = left;
    long int j = right;

    const double pivot = at((left + right)/2)->x();

    /* partition */
    while (i <= j)
    {
        while ( at(i)->x() < pivot )
            i ++;
        while ( at(j)->x() > pivot )
            j --;

        if ( i <= j )
        {
            QRectF v_1 = at(i)->rect();
            QRectF v_2 = at(j)->rect();

            at(i)->setRect(v_2);
            at(j)->setRect(v_1);

            i ++;
            j --;
        }
    }

    /* recursion */
    if ( left < j )
        quickSortAscending(left, j);
    if ( i < right )
        quickSortAscending(i, right);


    return true;
}
