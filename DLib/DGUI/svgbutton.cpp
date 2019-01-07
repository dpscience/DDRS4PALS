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

#include "svgbutton.h"

DSVGButton::DSVGButton(QWidget *parent) :
    QWidget(parent),
    m_enabled(true),
    m_state(undefined),
    m_bgColor("transparent"),
    m_svgWidget(nullptr),
    m_layout(nullptr)
{
    setStyleSheet("QWidget{background-color: " + m_bgColor + "}");    
    manageLayout();
}

DSVGButton::DSVGButton(const QString &pathLiteral, QWidget *parent) :
    QWidget(parent),
    m_defSVGPath(pathLiteral % "_default.svg"),
    m_hoverSVGPath(pathLiteral % "_hover.svg"),
    m_clickedSVGPath(pathLiteral % "_click.svg"),
    m_enabled(true),
    m_state(undefined),
    m_bgColor("transparent"),
    m_svgWidget(nullptr),
    m_layout(nullptr)
{
    setStyleSheet("QWidget{background-color: " + m_bgColor + "}");    
    manageLayout();
}

DSVGButton::DSVGButton(const QString &defaultStateSVGPath, const QString &hoverStateSVGPath, const QString &clickStateSVGPath, QWidget *parent) :
    QWidget(parent),
    m_defSVGPath(defaultStateSVGPath),
    m_hoverSVGPath(hoverStateSVGPath),
    m_clickedSVGPath(clickStateSVGPath),
    m_enabled(true),
    m_state(undefined),
    m_bgColor("transparent"),
    m_svgWidget(nullptr),
    m_layout(nullptr)
{
    setStyleSheet("QWidget{background-color: " % m_bgColor % "}");
    manageLayout();
}

DSVGButton::~DSVGButton()
{
    DDELETE_SAFETY(m_svgWidget);
    DDELETE_SAFETY(m_layout);
}

QString DSVGButton::customStatusTip() const
{
    return m_statusTip;
}

void DSVGButton::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
}

void DSVGButton::resizeEvent(QResizeEvent *event)
{
    m_svgWidget->setGeometry(2*DSVGButton_SVG_OFFSET, 2*DSVGButton_SVG_OFFSET, width()-2*DSVGButton_SVG_OFFSET, height()-2*DSVGButton_SVG_OFFSET);
    QWidget::resizeEvent(event);
}

bool DSVGButton::event(QEvent *event)
{
    if ( event->type() == QEvent::Enter )
    {
        m_state = hover;
        updateSVGImage();

        if ( !m_statusTip.isEmpty() )
            emit statusChanged(m_statusTip);
    }
    else if ( event->type() == QEvent::Leave )
    {
        m_state = leave;
        updateSVGImage();

        emit statusChanged("");
    }
    else if ( event->type() == QEvent::MouseButtonPress )
    {
        m_state = click;
        updateSVGImage();

        if ( !m_statusTip.isEmpty() )
            emit statusChanged(m_statusTip);
    }
    else if ( event->type() == QEvent::MouseButtonRelease )
    {
        m_state = release;
        updateSVGImage();

        if ( !m_statusTip.isEmpty() )
            emit statusChanged(m_statusTip);

        if (m_enabled)
            emit clicked();
    }

    return QWidget::event(event);
}

void DSVGButton::updateSVGImage()
{
    if ( m_state == hover || m_state == release || !m_enabled )
        m_svgWidget->load(m_hoverSVGPath);
    else if ( m_state == click && m_enabled )
        m_svgWidget->load(m_clickedSVGPath);
    else if ( (m_state == leave || m_state == undefined) && m_enabled )
        m_svgWidget->load(m_defSVGPath);
}

void DSVGButton::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
}

void DSVGButton::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore();
}

void DSVGButton::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}

void DSVGButton::setLiteralSVG(const QString &pathLiteral)
{
    m_defSVGPath     = pathLiteral % "_default.svg";
    m_hoverSVGPath   = pathLiteral % "_hover.svg";
    m_clickedSVGPath = pathLiteral % "_click.svg";

    updateSVGImage();
}

void DSVGButton::setDefaultStateSVG(const QString &path)
{
    m_defSVGPath = path;

    updateSVGImage();
}

void DSVGButton::setHoverStateSVG(const QString &path)
{
    m_hoverSVGPath = path;

    updateSVGImage();
}

void DSVGButton::setClickedStateSVG(const QString &path)
{
    m_clickedSVGPath = path;

    updateSVGImage();
}

void DSVGButton::setBackgroundColor(const QString &cssName)
{
    m_bgColor = cssName;
    setStyleSheet("QWidget{background-color: " + m_bgColor + "}");

    updateSVGImage();
}

void DSVGButton::setBackgroundColor(const QColor &color)
{
    m_bgColor = QString("rgb(" % QVariant(color.red()).toString() % ", " + QVariant(color.green()).toString() % ", " % QVariant(color.blue()).toString() % ")");
    setStyleSheet("QWidget{background-color: " % m_bgColor % "}");

    updateSVGImage();
}

void DSVGButton::setCustomStatusTip(const QString& statusTip)
{
    m_statusTip = statusTip;

    updateSVGImage();
}

void DSVGButton::enableWidget(bool enabled)
{
    m_enabled = enabled;
    QWidget::setEnabled(enabled);

    updateSVGImage();
}

void DSVGButton::manageLayout()
{
    m_svgWidget = new QSvgWidget;
    m_layout = new QHBoxLayout(this);

    m_layout->addWidget(m_svgWidget);
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->setContentsMargins(2*DSVGButton_SVG_OFFSET, 2*DSVGButton_SVG_OFFSET, 2*DSVGButton_SVG_OFFSET, 2*DSVGButton_SVG_OFFSET);

    setLayout(m_layout);
}


DSVGToolButton::DSVGToolButton(QWidget *parent) :
    QToolButton(parent),
    m_enabled(true),
    m_state(undefined),
    m_svgWidget(nullptr),
    m_layout(nullptr)
{
    setStyleSheet("QWidget{background-color: transparent}");
    manageLayout();
}

DSVGToolButton::DSVGToolButton(const QString &pathLiteral, QWidget *parent) :
    QToolButton(parent),
    m_defSVGPath(pathLiteral % "_default.svg"),
    m_hoverSVGPath(pathLiteral % "_hover.svg"),
    m_clickedSVGPath(pathLiteral % "_click.svg"),
    m_enabled(true),
    m_state(undefined),
    m_svgWidget(nullptr),
    m_layout(nullptr)
{
    setStyleSheet("QWidget{background-color: transparent}");
    manageLayout();
}

DSVGToolButton::DSVGToolButton(const QString &defaultStateSVGPath, const QString &hoverStateSVGPath, const QString &clickStateSVGPath, QWidget *parent) :
    QToolButton(parent),
    m_defSVGPath(defaultStateSVGPath),
    m_hoverSVGPath(hoverStateSVGPath),
    m_clickedSVGPath(clickStateSVGPath),
    m_enabled(true),
    m_state(undefined),
    m_svgWidget(nullptr),
    m_layout(nullptr)
{
    setStyleSheet("QWidget{background-color: transparent}");
    manageLayout();
}

DSVGToolButton::~DSVGToolButton()
{
    DDELETE_SAFETY(m_svgWidget);
    DDELETE_SAFETY(m_layout);
}

QString DSVGToolButton::customStatusTip() const
{
    return m_statusTip;
}

void DSVGToolButton::updateSVGImage()
{
    if ( m_state == hover || m_state == release || !m_enabled )
        m_svgWidget->load(m_hoverSVGPath);
    else if ( m_state == click && m_enabled )
        m_svgWidget->load(m_clickedSVGPath);
    else if ( (m_state == leave || m_state == undefined) && m_enabled )
        m_svgWidget->load(m_defSVGPath);
}

void DSVGToolButton::paintEvent(QPaintEvent *event)
{
    QToolButton::paintEvent(event);
}

void DSVGToolButton::resizeEvent(QResizeEvent *event)
{
    m_svgWidget->setGeometry(2*DSVGButton_SVG_OFFSET, 2*DSVGButton_SVG_OFFSET, width()-2*DSVGButton_SVG_OFFSET, height()-2*DSVGButton_SVG_OFFSET);
    QToolButton::resizeEvent(event);
}

bool DSVGToolButton::event(QEvent *event)
{
    if ( event->type() == QEvent::Enter )
    {
        m_state = hover;
        updateSVGImage();

        if ( !m_statusTip.isEmpty() )
            emit statusChanged(m_statusTip);
    }
    else if ( event->type() == QEvent::Leave )
    {
        m_state = leave;
        updateSVGImage();

        emit statusChanged("");
    }
    else if ( event->type() == QEvent::MouseButtonPress )
    {
        m_state = click;
        updateSVGImage();

        if ( !m_statusTip.isEmpty() )
            emit statusChanged(m_statusTip);
    }
    else if ( event->type() == QEvent::MouseButtonRelease )
    {
        m_state = release;
        updateSVGImage();

        if ( !m_statusTip.isEmpty() )
            emit statusChanged(m_statusTip);

        if (m_enabled)
            emit clicked();
    }

    return QToolButton::event(event);
}

void DSVGToolButton::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
}

void DSVGToolButton::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore();
}

void DSVGToolButton::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}

void DSVGToolButton::setDefaultStateSVG(const QString &path)
{
    m_defSVGPath = path;

    updateSVGImage();
}

void DSVGToolButton::setHoverStateSVG(const QString &path)
{
    m_hoverSVGPath = path;

    updateSVGImage();
}

void DSVGToolButton::setClickedStateSVG(const QString &path)
{
    m_clickedSVGPath = path;

    updateSVGImage();
}

void DSVGToolButton::setCustomStatusTip(const QString& statusTip)
{
    m_statusTip = statusTip;

    updateSVGImage();
}

void DSVGToolButton::enableWidget(bool enabled)
{
    m_enabled = enabled;
    QToolButton::setEnabled(enabled);

    updateSVGImage();
}

void DSVGToolButton::manageLayout()
{
    m_svgWidget = new QSvgWidget;
    m_layout = new QHBoxLayout(this);

    m_layout->addWidget(m_svgWidget);
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->setContentsMargins(2*DSVGButton_SVG_OFFSET, 2*DSVGButton_SVG_OFFSET, 2*DSVGButton_SVG_OFFSET, 2*DSVGButton_SVG_OFFSET);

    setLayout(m_layout);
}
