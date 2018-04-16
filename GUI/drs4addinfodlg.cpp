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

#include "drs4addinfodlg.h"
#include "ui_drs4addinfodlg.h"

DRS4AddInfoDlg::DRS4AddInfoDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DRS4AddInfoDlg)
{
    ui->setupUi(this);

    ui->textBrowser_addInfo->setPlaceholderText("Write your Comment here...");

    connect(ui->textBrowser_addInfo, SIGNAL(textChanged()), this, SLOT(updateText()));

    if (DRS4BoardManager::sharedInstance()->isDemoModeEnabled()  )
        setWindowTitle(PROGRAM_NAME + " - SIMULATION-MODE");
    else
        setWindowTitle(PROGRAM_NAME);
}

DRS4AddInfoDlg::~DRS4AddInfoDlg()
{
    DDELETE_SAFETY(ui);
}

void DRS4AddInfoDlg::setAddInfo(const QString &info)
{
    QMutexLocker locker(&m_mutex);

    ui->textBrowser_addInfo->clear();
    ui->textBrowser_addInfo->setHtml(info);
}

QString DRS4AddInfoDlg::getInfo() const
{
    return ui->textBrowser_addInfo->toHtml();
}

void DRS4AddInfoDlg::updateText()
{
   DRS4SettingsManager::sharedInstance()->setComment(getInfo());
}
