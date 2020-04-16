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

#ifndef DRS4UPDATENOTIFIER_H
#define DRS4UPDATENOTIFIER_H

#include <QObject>

#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDesktopServices>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

#include "dversion.h"
#include "DLib.h"

class DRS4UpdateNotifier : public QObject
{
    Q_OBJECT
public:
    static DRS4UpdateNotifier *sharedInstance();

public slots:
    bool checkingForUpdates();
    void redirectToUpdate();

signals:
    void checkingUpdatesStarted();
    void updateAvailable(QString, QString, QDateTime);
    void latestReleaseRunning();
    void error();

private slots:
    void updateReplyFinished(QNetworkReply *reply);

private:
    DRS4UpdateNotifier();
    ~DRS4UpdateNotifier();

    QNetworkAccessManager *m_networkManager;
    QString m_updateUrl;
};

#endif // DRS4UPDATENOTIFIER_H
