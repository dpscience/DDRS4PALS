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
