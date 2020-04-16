/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2020 Danny Petschke
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

#ifndef DRS4DOUBLESPINBOX_H
#define DRS4DOUBLESPINBOX_H

#include <QObject>
#include <QWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QEvent>
#include <QToolTip>
#include <QtCore>

class DRS4DoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit DRS4DoubleSpinBox(QWidget *parent = nullptr);
    virtual ~DRS4DoubleSpinBox();

protected:
    virtual bool event(QEvent *event);
};



#endif // DRS4DOUBLESPINBOX_H
