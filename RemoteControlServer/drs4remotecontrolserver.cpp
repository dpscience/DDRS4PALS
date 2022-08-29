/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2022 Dr. Danny Petschke
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
**  @author: Dr. Danny Petschke
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

#include "DRS4RemoteControlServer.h"

static DRS4RemoteControlServer *__DRS4RemoteControlServer = DNULLPTR;

#include <QDebug>

DRS4RemoteControlServer *DRS4RemoteControlServer::sharedInstance() {
    if (!__DRS4RemoteControlServer)
        __DRS4RemoteControlServer = new DRS4RemoteControlServer;

    return __DRS4RemoteControlServer;
}

void DRS4RemoteControlServer::start(DRS4Worker *worker) {
    m_worker = worker;

    listen(QHostAddress::Any/*QHostAddress(DRS4ProgramSettingsManager::sharedInstance()->remoteControlServerIP())*/, DRS4ProgramSettingsManager::sharedInstance()->remoteControlServerPort());

    emit stateChanged(true);
}

void DRS4RemoteControlServer::stop() {
    if (isListening()) {
        for (QTcpSocket *socket : m_sockerList) {
            if (socket)
                socket->close();
        }

        m_sockerList.clear();
    }

    close();

    emit stateChanged(false);
}

DRS4RemoteControlServer::DRS4RemoteControlServer() :
    QTcpServer(DNULLPTR),
    m_worker(DNULLPTR) {
    connect(this, SIGNAL(newConnection()), this, SLOT(handleIncomingConnection()));
    connect(this, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(handleServerError(QAbstractSocket::SocketError)));

    emit stateChanged(false);
}

DRS4RemoteControlServer::~DRS4RemoteControlServer() {
    DDELETE_SAFETY(__DRS4RemoteControlServer);
}

void DRS4RemoteControlServer::handleServerError(QAbstractSocket::SocketError error) {
    qDebug() << error;
}

void DRS4RemoteControlServer::handleIncomingConnection() {
    QTcpSocket *connection = nextPendingConnection();

    if (!connection)
        return;

    m_sockerList.append(connection);

    new DRS4RCClientConnection(connection, m_worker, this);
}

DRS4RCClientConnection::DRS4RCClientConnection(QTcpSocket *socket, DRS4Worker *worker, QObject* parent) :
    QObject(parent),
    m_socket(socket),
    m_worker(worker) {
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

DRS4RCClientConnection::~DRS4RCClientConnection() {}

void DRS4RCClientConnection::readyRead() {
    if (!m_socket)
        return;

    QString msg("");

    msg.append(m_socket->readAll());

    handleMessage(msg);
}

void DRS4RCClientConnection::handleMessage(const QString &data) {
    if (!m_socket)
        return;

    DString request(data);

    const QString cmd_id = request.parseBetween("<request>", "</request>");

    if (cmd_id.isEmpty()) {
        respond(DRS4RCReturnCode::code::failed, -1);

        return;
    }

    bool ok = false;
    const int id = QVariant(cmd_id).toInt(&ok);

    if (!ok) {
        respond(DRS4RCReturnCode::code::failed, -1);

        return;
    }

    if (id == 0) { // start acqusition ...
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const bool bRunning = m_worker->isRunning();

        m_worker->setBusy(false);

        if (bRunning) {
            respond(DRS4RCReturnCode::code::ok, id, "1");

            return;
        }

        emit qobject_cast<DRS4RemoteControlServer*>(parent())->startAcquisition();

        respond(DRS4RCReturnCode::code::ok, id, "1");
    }
    else if (id == 1) { // stop acqusition ...
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const bool bRunning = m_worker->isRunning();

        m_worker->setBusy(false);

        if (!bRunning) {
            respond(DRS4RCReturnCode::code::ok, id, "1");

            return;
        }

        emit qobject_cast<DRS4RemoteControlServer*>(parent())->stopAcquisition();

        respond(DRS4RCReturnCode::code::ok, id, "1");
    }
    else if (id == 2) { // is acqusition running ?
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const bool bRunning = m_worker->isRunning();

        m_worker->setBusy(false);

        respond(DRS4RCReturnCode::code::ok, id, QVariant(int(bRunning)).toString());
    }
    else if (id == 3) { // reset all (A-B, B-A, merged, prompt) spectra ...
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        m_worker->resetABSpectrum();
        m_worker->resetBASpectrum();
        m_worker->resetMergedSpectrum();
        m_worker->resetCoincidenceSpectrum();

        m_worker->setBusy(false);

        respond(DRS4RCReturnCode::code::ok, id, "1");
    }
    else if (id == 4) { // reset A-B...
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        m_worker->resetABSpectrum();

        m_worker->setBusy(false);

        respond(DRS4RCReturnCode::code::ok, id, "1");
    }
    else if (id == 5) { // reset B-A ...
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        m_worker->resetBASpectrum();

        m_worker->setBusy(false);

        respond(DRS4RCReturnCode::code::ok, id, "1");
    }
    else if (id == 6) { // reset merged ...
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        m_worker->resetMergedSpectrum();

        m_worker->setBusy(false);

        respond(DRS4RCReturnCode::code::ok, id, "1");
    }
    else if (id == 7) { // reset prompt ...
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        m_worker->resetCoincidenceSpectrum();

        m_worker->setBusy(false);

        respond(DRS4RCReturnCode::code::ok, id, "1");
    }
    else if (id == 8) { // data A-B ?
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const QVector<int> data = *m_worker->spectrumAB();
        const int counts = m_worker->countsSpectrumAB();

        const int no_chn = DRS4SettingsManager::sharedInstance()->channelCntAB();
        const double scale = DRS4SettingsManager::sharedInstance()->scalerInNSAB();

        m_worker->setBusy(false);

        QString sData = QString("<channel-width-ps>%1</channel-width-ps>").arg(1000.*scale/no_chn);
        sData.append(QString("<number-of-channel>%1</number-of-channel>").arg(no_chn));
        sData.append(QString("<integral-counts>%1</integral-counts>").arg(counts));

        sData.append("<data>");
        for (int val : data)
            sData.append(QString("{%1}").arg(val));

        sData.append("</data>");

        respond(DRS4RCReturnCode::code::ok, id, sData);
    }
    else if (id == 9) { // data B-A ?
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const QVector<int> data = *m_worker->spectrumBA();
        const int counts = m_worker->countsSpectrumBA();

        const int no_chn = DRS4SettingsManager::sharedInstance()->channelCntBA();
        const double scale = DRS4SettingsManager::sharedInstance()->scalerInNSBA();

        m_worker->setBusy(false);

        QString sData = QString("<channel-width-ps>%1</channel-width-ps>").arg(1000.*scale/no_chn);
        sData.append(QString("<number-of-channel>%1</number-of-channel>").arg(no_chn));
        sData.append(QString("<integral-counts>%1</integral-counts>").arg(counts));

        sData.append("<data>");
        for (int val : data)
            sData.append(QString("{%1}").arg(val));

        sData.append("</data>");

        respond(DRS4RCReturnCode::code::ok, id, sData);
    }
    else if (id == 10) { // data merged ?
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const QVector<int> data = *m_worker->spectrumMerged();
        const int counts = m_worker->countsSpectrumMerged();

        const int no_chn = DRS4SettingsManager::sharedInstance()->channelCntMerged();
        const double scale = DRS4SettingsManager::sharedInstance()->scalerInNSMerged();

        m_worker->setBusy(false);

        QString sData = QString("<channel-width-ps>%1</channel-width-ps>").arg(1000.*scale/no_chn);
        sData.append(QString("<number-of-channel>%1</number-of-channel>").arg(no_chn));
        sData.append(QString("<integral-counts>%1</integral-counts>").arg(counts));

        sData.append("<data>");
        for (int val : data)
            sData.append(QString("{%1}").arg(val));

        sData.append("</data>");

        respond(DRS4RCReturnCode::code::ok, id, sData);
    }
    else if (id == 11) { // data prompt ?
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const QVector<int> data = *m_worker->spectrumCoincidence();
        const int counts = m_worker->countsSpectrumCoincidence();

        const int no_chn = DRS4SettingsManager::sharedInstance()->channelCntCoincindence();
        const double scale = DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence();

        m_worker->setBusy(false);

        QString sData = QString("<channel-width-ps>%1</channel-width-ps>").arg(1000.*scale/no_chn);
        sData.append(QString("<number-of-channel>%1</number-of-channel>").arg(no_chn));
        sData.append(QString("<integral-counts>%1</integral-counts>").arg(counts));

        sData.append("<data>");
        for (int val : data)
            sData.append(QString("{%1}").arg(val));

        sData.append("</data>");

        respond(DRS4RCReturnCode::code::ok, id, sData);
    }
    else if (id == 12) { // counts A-B ?
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const int counts = m_worker->countsSpectrumAB();

        m_worker->setBusy(false);

        respond(DRS4RCReturnCode::code::ok, id, QVariant(counts).toString());
    }
    else if (id == 13) { // counts B-A ?
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const int counts = m_worker->countsSpectrumBA();

        m_worker->setBusy(false);

        respond(DRS4RCReturnCode::code::ok, id, QVariant(counts).toString());
    }
    else if (id == 14) { // counts merged ?
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const int counts = m_worker->countsSpectrumMerged();

        m_worker->setBusy(false);

        respond(DRS4RCReturnCode::code::ok, id, QVariant(counts).toString());
    }
    else if (id == 15) { // counts prompt ?
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const int counts = m_worker->countsSpectrumCoincidence();

        m_worker->setBusy(false);

        respond(DRS4RCReturnCode::code::ok, id, QVariant(counts).toString());
    }
    else if (id == 16) { // get settings
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const QString sData = DRS4SettingsManager::sharedInstance()->xmlContent();

        m_worker->setBusy(false);

        respond(DRS4RCReturnCode::code::ok, id, sData);
    } // get version
    else if (id == 17) {
        respond(DRS4RCReturnCode::code::ok, id, QString("<major>%1</major><minor>%2</minor>").arg(MAJOR_VERSION).arg(MINOR_VERSION));
    }
    else
        respond(DRS4RCReturnCode::code::failed, -1);
}

void DRS4RCClientConnection::respond(const DRS4RCReturnCode::code &code, int request_id, const QString &response) {
    if (!m_socket)
        return;

    const QString replyString = QString("<reply><request-id>%1</request-id><request-valid?>%2</request-valid?><reply-data>%3</reply-data></reply>").arg(request_id).arg(code == DRS4RCReturnCode::code::ok ? 1 : 0).arg(response);

    m_socket->write(replyString.toStdString().c_str());
}
