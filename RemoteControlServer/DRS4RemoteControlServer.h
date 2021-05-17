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

#ifndef DRS4RemoteControlServer_H
#define DRS4RemoteControlServer_H

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
#include <QTcpServer>

#include "dversion.h"
#include "drs4worker.h"
#include "Stream/drs4streamdataloader.h"
#include "Stream/drs4streammanager.h"
#include "CPUUsage/drs4cpuusage.h"
#include "DLib.h"

class DRS4RCClientConnection;

typedef struct {
    enum code {
        ok = 0,
        failed = 1
    };
} DRS4RCReturnCode;

class DRS4RemoteControlServer : public QTcpServer  {
    Q_OBJECT
public:
    static DRS4RemoteControlServer *sharedInstance();

public slots:
    void start(DRS4Worker *worker);
    void stop();

private slots:
    void handleIncomingConnection();
    void handleServerError(QAbstractSocket::SocketError error);

signals:
    void stateChanged(bool active);

    void startAcquisition();
    void stopAcquisition();

private:
    DRS4RemoteControlServer();
    virtual ~DRS4RemoteControlServer();

    DRS4Worker *m_worker;

    QList<QTcpSocket*> m_sockerList;
};

class DRS4RCClientConnection : public QObject {
    Q_OBJECT
public:
    DRS4RCClientConnection(QTcpSocket *socket, DRS4Worker *worker, QObject *parent = DNULLPTR);
    virtual ~DRS4RCClientConnection();

private slots:
    void readyRead();
    void handleMessage(const QString &data);

private:
    void respond(const DRS4RCReturnCode::code& code, int request_id, const QString& response = "");

private:
    QTcpSocket *m_socket;

    DRS4Worker *m_worker;

    mutable QMutex m_mutex;
};

#endif // DRS4RemoteControlServer_H
