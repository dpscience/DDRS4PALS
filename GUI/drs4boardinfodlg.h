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

#ifndef DRS4BOARDINFODLG_H
#define DRS4BOARDINFODLG_H

#include "dversion.h"

#include <QWidget>
#include <QFileDialog>

#include "DLib.h"

#include "drs4settingsmanager.h"
#include "drs4boardmanager.h"
#include "drs4worker.h"

namespace Ui {
class DRS4BoardInfoDlg;
}

class DRS4BoardInfoDlg : public QWidget
{
    Q_OBJECT
public:
    explicit DRS4BoardInfoDlg(QWidget *parent = nullptr, DRS4Worker *worker = nullptr);
    virtual ~DRS4BoardInfoDlg();

public slots:
    void updateInfo();
    void save();

protected:
    virtual void showEvent(QShowEvent *event);

private:
    Ui::DRS4BoardInfoDlg *ui;

    DRS4Worker *m_worker;
};

#endif // DRS4BOARDINFODLG_H
