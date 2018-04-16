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

#ifndef DRS4PULSESAVERANGEDLG_H
#define DRS4PULSESAVERANGEDLG_H

#include <QWidget>
#include <QFileDialog>

#include "DLib.h"

#include "drs4worker.h"
#include "drs4programsettingsmanager.h"
#include "Stream/drs4streammanager.h"

namespace Ui {
class DRS4PulseSaveRangeDlg;
}

class DRS4PulseSaveRangeDlg : public QWidget
{
    Q_OBJECT

public:
    explicit DRS4PulseSaveRangeDlg(DRS4Worker *worker, QWidget *parent = 0);
    virtual ~DRS4PulseSaveRangeDlg();

public slots:
    void updateState(bool state);
    void updateState2(int state);
    void save();

    void setAsRunning();
    void resetAsRunning();

protected:
    virtual void showEvent(QShowEvent *event);

private:
    Ui::DRS4PulseSaveRangeDlg *ui;

    DRS4Worker *m_worker;
};

#endif // DRS4PULSESAVERANGEDLG_H
