#ifndef D2DSPLINEWIDGET_H
#define D2DSPLINEWIDGET_H


#include "../DTypes/types.h"

class spline;

class D2DSplineControlPoint
{
    double m_x, m_y;           //holds the real-value property
    int    m_width, m_height;  //holds the rectangular-size in pixel-coordinates
public:
    inline D2DSplineControlPoint() : m_x(0.0), m_y(0.0), m_width(0), m_height(0) {}
    inline D2DSplineControlPoint(const QRectF& rect) : m_x(rect.x()), m_y(rect.y()), m_width((int)rect.width()), m_height((int)rect.height()) {}
    inline D2DSplineControlPoint(const QPointF& point, const QSize& size) : m_x(point.x()), m_y(point.y()), m_width((int)size.width()), m_height((int)size.height()) {}

    inline D2DSplineControlPoint& operator =(const D2DSplineControlPoint& other)
    {
        m_x      = other.x();
        m_y      = other.y();
        m_width  = other.width();
        m_height = other.height();

        return *this;
    }

    inline void setRect(double x, double y, int width, int height)
    {
        m_x      = x;
        m_y      = y;
        m_width  = width;
        m_height = height;
    }

    inline void setRect(const QRectF& rect)
    {
        m_x      = rect.x();
        m_y      = rect.y();
        m_width  = (int)rect.width();
        m_height = (int)rect.height();
    }

    inline void setX(double x)
    {
        m_x = x;
    }

    inline void setPosition(const QPointF& pos)
    {
        m_x = pos.x();
        m_y = pos.y();
    }

    inline double x() const    { return m_x; }
    inline double y() const    { return m_y; }
    inline QPointF point() const { return QPointF(m_x, m_y); }
    inline int width() const   { return m_width; }
    inline int height() const  { return m_height; }
    inline QRectF rect() const { return QRectF(m_x, m_y, (double)m_width, (double)m_height); }
};

class D2DSplineCtrlPointList : public QList<D2DSplineControlPoint*>
{
    friend class D2DSplineWidget;

private:
    inline D2DSplineCtrlPointList() {}
    inline virtual ~D2DSplineCtrlPointList()
    {
        for ( int i = 0 ; i <= size() ; ++ i ){
            D2DSplineControlPoint* item = takeFirst();
            if ( item ){
                delete item;
                item = 0;
            }
        }
    }

    void quickSortAscending();
    bool quickSortAscending(long int left, long int right);
};

class D2DSplineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit D2DSplineWidget(QWidget *parent = 0);
    virtual ~D2DSplineWidget();

    QPen ctrlPointPen() const;
    QPen splinePen()    const;

    double xMin() const;
    double xMax() const;
    double yMin() const;
    double yMax() const;

    int splineResolution() const;

    bool isAutoscaleEnabled() const;

    double getInterpolatedYValue(double x) const;
    QList<QPointF> getCtrlPoints() const;

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

    virtual void drawForground(QPaintEvent *event);
    virtual void drawBackground(QPaintEvent *event);

private:
    QPoint  convertToPixel(double x, double y) const;
    QPointF convertFromPixel(int x, int y)     const;

    bool containsPoint(D2DSplineControlPoint* ctrlP, const QPoint& p);

    void addCtrlPoint(const QPoint& pixelCoord);
    void removeCtrlPoint(D2DSplineControlPoint* ctrlP);

signals:
    void calculateSpline();

public slots:
    void updateSplinePath();

    void setXRange(double xMin, double xMax);
    void setYRange(double yMin, double yMax);

    void setCtrlPointPen(const QPen& pen);
    void setSplinePen(const QPen& pen);

    void setResolution(int cnt);

    void enableAutoscale(bool enable);

    void setCtrlPoints(const QList<QPointF> ctrlPoints);

private:
    double m_minY, m_maxY, m_minX, m_maxX;
    double m_splineMinY, m_splineMaxY;

    D2DSplineCtrlPointList m_ctrlPoints;
    D2DSplineControlPoint* m_activeCtrlP;

    int m_ctrlPRectLength;
    int m_splineResolution;

    QPen m_ctrlPPen;
    QPen m_polygonPen;

    bool m_autoScale;

    spline *m_spline;
};

#endif // D2DSPLINEWIDGET_H
