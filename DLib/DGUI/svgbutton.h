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

#ifndef SVGBUTTON_H
#define SVGBUTTON_H

#include "../DTypes/types.h"

#define DSVGButton_SVG_OFFSET     2

class DSVGButton;
class DSVGToolButton;

class DSVGButton : public QWidget
{
    Q_OBJECT
public:
    explicit DSVGButton(QWidget *parent = nullptr);
    explicit DSVGButton(const QString& pathLiteral, QWidget *parent = nullptr);
    explicit DSVGButton(const QString& defaultStateSVGPath, const QString& hoverStateSVGPath, const QString& clickStateSVGPath, QWidget *parent = nullptr);
    virtual ~DSVGButton();

    QString customStatusTip() const;

protected:
    typedef enum{
        hover = 0,
        leave = 1,
        click = 2,
        release = 3,
        undefined = 100
    }state;

    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual bool event(QEvent *event);

    virtual void updateSVGImage();

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

signals:
    void statusChanged(const QString&);
    void clicked();

public slots:
    void setLiteralSVG(const QString& pathLiteral);
    void setDefaultStateSVG(const QString& path);
    void setHoverStateSVG(const QString& path);
    void setClickedStateSVG(const QString& path);

    void setBackgroundColor(const QString& cssName);
    void setBackgroundColor(const QColor& color);
    void setCustomStatusTip(const QString&);
    void enableWidget(bool);

private:
    void manageLayout();

private:
    QSvgWidget *m_svgWidget;
    QHBoxLayout *m_layout;

    QString m_bgColor;
    state m_state;
    QString m_statusTip;
    QString m_defSVGPath;
    QString m_hoverSVGPath;
    QString m_clickedSVGPath;
    bool m_enabled;
};


class DSVGToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit DSVGToolButton(QWidget *parent = nullptr);
    explicit DSVGToolButton(const QString& pathLiteral, QWidget *parent = nullptr);
    explicit DSVGToolButton(const QString& defaultStateSVGPath, const QString& hoverStateSVGPath, const QString& clickStateSVGPath, QWidget *parent = nullptr);
    virtual ~DSVGToolButton();

    QString customStatusTip() const;

protected:
    typedef enum{
        hover = 0,
        leave = 1,
        click = 2,
        release = 3,
        undefined = 100
    }state;

    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual bool event(QEvent *event);

    virtual void updateSVGImage();

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);

signals:
    void statusChanged(const QString&);
    void clicked();

public slots:
    void setDefaultStateSVG(const QString& path);
    void setHoverStateSVG(const QString& path);
    void setClickedStateSVG(const QString& path);

    void setCustomStatusTip(const QString&);
    void enableWidget(bool);

private:
    void manageLayout();

private:
    QSvgWidget *m_svgWidget;
    QHBoxLayout *m_layout;

    state m_state;
    QString m_statusTip;
    QString m_defSVGPath;
    QString m_hoverSVGPath;
    QString m_clickedSVGPath;
    bool m_enabled;
};


#endif // SVGBUTTON_H
