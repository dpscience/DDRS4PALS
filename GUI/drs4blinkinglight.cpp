#include "drs4blinkinglight.h"
#include "ui_drs4blinkinglight.h"

DRS4BlinkingLight::DRS4BlinkingLight(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DRS4BlinkingLight) {
    ui->setupUi(this);
}

DRS4BlinkingLight::~DRS4BlinkingLight() {
    DDELETE_SAFETY(ui);
}

void DRS4BlinkingLight::setActive(bool active, const QString &msg, int interval) {
    if (active) {
        ui->frame_light->start(interval);
        ui->label_msg->setText(msg);
        ui->label_msg->setStyleSheet("color: green");
    }
    else {
        ui->frame_light->stop();
        ui->label_msg->setText(msg);
        ui->label_msg->setStyleSheet("color: red");
    }
}

void DRS4BlinkingLight::resizeEvent(QResizeEvent *event) {
    const int newHeight = ui->label_msg->geometry().height();

    ui->frame_light->setMaximumSize(newHeight, newHeight);
    ui->frame_light->setMinimumSize(newHeight, newHeight);

    QWidget::resizeEvent(event);
}

void DRS4BlinkingLight::mouseReleaseEvent(QMouseEvent *event) {
    emit clicked();

    QWidget::mouseReleaseEvent(event);
}

DRS4CircleFrame::DRS4CircleFrame(QWidget *parent) :
    QFrame(parent) {
    m_pen_active.setBrush(QBrush(Qt::darkGreen));
    m_pen_transparent.setBrush(Qt::transparent);
    m_pen_inactive.setBrush(Qt::red);

    m_pen = m_pen_transparent;
}

DRS4CircleFrame::~DRS4CircleFrame() {}

void DRS4CircleFrame::start(int interval) {
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(blink()));

    m_timer.setInterval(interval);
    m_timer.start();

    m_pen = m_pen_active;

    update();
}

void DRS4CircleFrame::stop() {
    m_timer.stop();
    disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(blink()));

    m_pen = m_pen_inactive;

    update();
}

void DRS4CircleFrame::blink() {
    if (m_pen == m_pen_active)
        m_pen = m_pen_transparent;
    else
        m_pen = m_pen_active;

    update();
}

void DRS4CircleFrame::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setPen(m_pen);
    painter.setBrush(m_pen.brush());

    const int radius = qMin(geometry().height()*0.5, geometry().width()*0.5);

    painter.drawEllipse(geometry().width()*0.5, geometry().height()*0.5, radius-1, radius-1);

    QFrame::paintEvent(event);
}
