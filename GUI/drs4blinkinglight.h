#ifndef DRS4BLINKINGLIGHT_H
#define DRS4BLINKINGLIGHT_H

#include <QWidget>
#include <QFrame>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPen>
#include <QTimer>

#include "dversion.h"
#include "DLib.h"

namespace Ui {
class DRS4BlinkingLight;
}

class DRS4CircleFrame : public QFrame {
    Q_OBJECT

public:
    explicit DRS4CircleFrame(QWidget *parent = DNULLPTR);
    virtual ~DRS4CircleFrame();

public slots:
    void start(int interval);
    void stop();

private slots:
    void blink();

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QPen m_pen;
    QPen m_pen_active;
    QPen m_pen_transparent;
    QPen m_pen_inactive;

    QTimer m_timer;
};

class DRS4BlinkingLight : public QWidget {
    Q_OBJECT

public:
    explicit DRS4BlinkingLight(QWidget *parent = DNULLPTR);
    virtual ~DRS4BlinkingLight();

public slots:
    void setActive(bool active, const QString& msg = "your message", int interval = 500);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void clicked();

private:
    Ui::DRS4BlinkingLight *ui;
};

#endif // DRS4BLINKINGLIGHT_H
