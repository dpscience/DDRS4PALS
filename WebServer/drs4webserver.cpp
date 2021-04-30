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

#include "drs4webserver.h"

static DRS4WebServer *__drs4WebServer = DNULLPTR;

#include <QDebug>

DRS4WebServer *DRS4WebServer::sharedInstance() {
    if (!__drs4WebServer)
        __drs4WebServer = new DRS4WebServer;

    return __drs4WebServer;
}

void DRS4WebServer::start(DRS4Worker *worker) {
    m_worker = worker;

    listen(QHostAddress::Any, DRS4ProgramSettingsManager::sharedInstance()->httpServerPort());

    emit stateChanged(true);
}

void DRS4WebServer::stop() {
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

DRS4WebServer::DRS4WebServer() :
    QTcpServer(DNULLPTR),
    m_worker(DNULLPTR) {
    connect(this, SIGNAL(newConnection()), this, SLOT(handleIncomingConnection()));
    connect(this, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(handleServerError(QAbstractSocket::SocketError)));

    emit stateChanged(false);
}

DRS4WebServer::~DRS4WebServer() {
    DDELETE_SAFETY(__drs4WebServer);
}

void DRS4WebServer::handleServerError(QAbstractSocket::SocketError error) {
    qDebug() << error;
}

void DRS4WebServer::handleIncomingConnection() {
    QTcpSocket *connection = nextPendingConnection();

    if (!connection)
        return;

    m_sockerList.append(connection);

    new DRS4ClientConnection(connection, m_worker, this);
}

DRS4ClientConnection::DRS4ClientConnection(QTcpSocket *socket, DRS4Worker *worker, QObject* parent) :
    QObject(parent),
    m_socket(socket),
    m_worker(worker) {
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

DRS4ClientConnection::~DRS4ClientConnection() {}

void DRS4ClientConnection::readyRead() {
    if (!m_socket)
        return;

    QString msg("");

    while (m_socket->canReadLine())
        msg.append(m_socket->readLine());

    handleMessage(msg);
}

void DRS4ClientConnection::handleMessage(const QString &data) {
    if (!m_socket)
        return;

    DString msg(data);
    DString request = msg.parseBetween("GET", "HTTP/1.1");

    request = request.trimmed();

    /* open main page ... */
    if (request.isEmpty()
            || request == "/") {
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        double t = -1;
        if (!DRS4BoardManager::sharedInstance()->isDemoModeEnabled())
            t = DRS4BoardManager::sharedInstance()->currentBoard()->GetTemperature();

        const double freq = m_worker->currentPulseCountRateInHz();
        const bool isRunning = m_worker->isRunning();

        const bool bParallel = DRS4ProgramSettingsManager::sharedInstance()->isMulticoreThreadingEnabled();
        const int cores = m_worker->maxThreads();

        const int trigger_id = !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ? DRS4SettingsManager::sharedInstance()->triggerSource_index() : -1;
        const double boardFreq = !DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ? DRS4SettingsManager::sharedInstance()->sampleSpeedInGHz() : -1;

        const double cpu = DRS4CPUUsageManager::sharedInstance()->releaseCPULoad();
        const double cfA = DRS4SettingsManager::sharedInstance()->cfdLevelA()*100.;
        const double cfB = DRS4SettingsManager::sharedInstance()->cfdLevelB()*100.;
        const int chnA = DRS4SettingsManager::sharedInstance()->channelNumberA();
        const int chnB = DRS4SettingsManager::sharedInstance()->channelNumberB();

        const int no_chn_AB = DRS4SettingsManager::sharedInstance()->channelCntAB();
        const double offset_AB = DRS4SettingsManager::sharedInstance()->offsetInNSAB();
        const double scale_AB = DRS4SettingsManager::sharedInstance()->scalerInNSAB();
        const double bin_width_AB = 1000.*scale_AB/no_chn_AB;
        const int counts_AB = m_worker->countsSpectrumAB();
        const double efficiency_AB = m_worker->currentLifetimeABCountRateInHz();

        const int no_chn_BA = DRS4SettingsManager::sharedInstance()->channelCntBA();
        const double offset_BA = DRS4SettingsManager::sharedInstance()->offsetInNSBA();
        const double scale_BA = DRS4SettingsManager::sharedInstance()->scalerInNSBA();
        const double bin_width_BA = 1000.*scale_BA/no_chn_BA;
        const int counts_BA = m_worker->countsSpectrumBA();
        const double efficiency_BA = m_worker->currentLifetimeABCountRateInHz();

        const int no_chn_merged = DRS4SettingsManager::sharedInstance()->channelCntMerged();
        const double offset_merged = DRS4SettingsManager::sharedInstance()->offsetInNSMerged();
        const double scale_merged = DRS4SettingsManager::sharedInstance()->scalerInNSMerged();
        const double bin_width_merged = 1000.*scale_merged/no_chn_merged;
        const int counts_merged = m_worker->countsSpectrumMerged();
        const double efficiency_merged = m_worker->currentLifetimeMergedCountRateInHz();

        const int no_chn_prompt = DRS4SettingsManager::sharedInstance()->channelCntCoincindence();
        const double offset_prompt = DRS4SettingsManager::sharedInstance()->offsetInNSCoincidence();
        const double scale_prompt = DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence();
        const double bin_width_prompt = 1000.*scale_prompt/no_chn_prompt;
        const int counts_prompt = m_worker->countsSpectrumCoincidence();
        const double efficiency_prompt = m_worker->currentLifetimeCoincidenceCountRateInHz();

        // hard-drive >> ...
        const bool loaderArmed = DRS4StreamDataLoader::sharedInstance()->isArmed() && DRS4BoardManager::sharedInstance()->isDemoModeEnabled();
        const qint64 fileSizeL = loaderArmed ? DRS4StreamDataLoader::sharedInstance()->fileSizeInMegabyte() : 0;
        const qint64 loadedFileSizeL = loaderArmed ? DRS4StreamDataLoader::sharedInstance()->loadedFileSizeInMegabyte() : 0;
        const QString fileNameL = loaderArmed ? DRS4StreamDataLoader::sharedInstance()->fileName() : "";
        const QString simFileL = DRS4BoardManager::sharedInstance()->isDemoModeEnabled() ? DRS4SimulationSettingsManager::sharedInstance()->fileName() : "";

        // hard-drive << ...
        const bool streamerArmed = DRS4StreamManager::sharedInstance()->isArmed();
        const double fileSizeS = streamerArmed ? (DRS4StreamManager::sharedInstance()->streamedContentInBytes()/1024.0f)/1000.0f : 0;
        const QString fileNameS = streamerArmed ? DRS4StreamManager::sharedInstance()->fileName() : "";

        m_worker->setBusy(false);

        QString html_template_form = ":/webcontent/main_template_online";

        if (DRS4BoardManager::sharedInstance()->isDemoModeEnabled())
            html_template_form = ":/webcontent/main_template_offline";

        QFile file(html_template_form);

        QString response = "";

        if (file.open(QIODevice::ReadOnly)) {
            response.append(file.readAll());

            response.replace("UPDATE_RATE", QString(QVariant(DRS4ProgramSettingsManager::sharedInstance()->httpServerUrlUpdateRate()).toString()));

            if (bParallel) {
                response.replace("PARALLEL_INFO_COLOR", QString("#1c8000"));
            }
            else {
                response.replace("PARALLEL_INFO_COLOR", QString("#0938e3"));
            }

            response.replace("PARALLEL_INFO", bParallel ? QString("on (running on %1 CPU cores)").arg(cores) : QString("off"));

            if (!int(freq)) {
                response.replace("SAMPLING_VALUE_COLOR", QString("#fc0303")); // red
            }
            else {
                response.replace("SAMPLING_VALUE_COLOR", QString("#1c8000")); // green
            }

            if (streamerArmed) {
                response.replace("STREAMING_STATE_COLOR", QString("#1c8000")); // green
                response.replace("STREAMING_STATE", QString("active (%1)").arg(fileNameS));
            }
            else {
                response.replace("STREAMING_STATE_COLOR", QString("#fc0303")); // red
                response.replace("STREAMING_STATE", "deactivated");
            }

            response.replace("STREAMING_FILE_SIZE_COLOR", QString("#0938e3")); // blue
            response.replace("STREAMING_FILE_SIZE", QVariant(int(fileSizeS)).toString());


            if (loaderArmed) {
                response.replace("DOWNLOAD_STATE_COLOR", QString("#1c8000")); // green
                response.replace("DOWNLOAD_STATE", QString("active (%1)").arg(fileNameL));
            }
            else {
                response.replace("DOWNLOAD_STATE_COLOR", QString("#0938e3")); // blue
                response.replace("DOWNLOAD_STATE", QString("running from simulation file (%1)").arg(simFileL));
            }

            response.replace("DOWNLOAD_PROGRESS_COLOR", QString("#0938e3")); // blue

            if (loaderArmed) {
                response.replace("DOWNLOAD_PROGRESS", QVariant(100.0*double(loadedFileSizeL)/double(fileSizeL)).toString());
            }
            else {
                response.replace("DOWNLOAD_PROGRESS", "---");
            }

            if (!isRunning) {
                response.replace("MEASUREMENT_STATE_COLOR", QString("#0938e3")); // blue
            }
            else {
                response.replace("MEASUREMENT_STATE_COLOR", QString("#1c8000")); // green
            }

            response.replace("CPU_VALUE_COLOR", QString("#0938e3")); // blue

            if (!DRS4BoardManager::sharedInstance()->isDemoModeEnabled()) {
                if (t <= 38.) {
                    response.replace("TEMP_VALUE_COLOR", QString("#fc0303")); // red
                }
                else if (t > 38. && t < 42.) {
                    response.replace("TEMP_VALUE_COLOR", QString("#e6ae09")); // orange
                }
                else {
                    response.replace("TEMP_VALUE_COLOR", QString("#1c8000")); // green
                }

                response.replace("TRIGGER_LOGIC_COLOR", QString("#0938e3")); // blue
                response.replace("SPEED_COLOR", QString("#0938e3")); // blue

                QString logic;

                if (trigger_id == 0) {
                    logic = "A";
                }
                else if (trigger_id == 1) {
                    logic = "B";
                }
                else if (trigger_id == 2) {
                    logic = "A && B";
                }
                else if (trigger_id == 3) {
                    logic = "A || B";
                }
                else if (trigger_id == 4) {
                    logic = "external";
                }
                else {
                    /* nothing here */
                }

                response.replace("TRIGGER_LOGIC", logic);
                response.replace("SPEED", QString::number(boardFreq, 'f', 2));
            }

            response.replace("VERSION_INFO", QString(MAJOR_VERSION + "." + MINOR_VERSION + " (" + DATE_EXTENSION + ")"));
            response.replace("COPYRIGHT_DATE", QString(COPYRIGHT_DATE));
            response.replace("MEASUREMENT_STATE", isRunning ? QString("running") : QString("idle"));
            response.replace("SAMPLING_VALUE", QString(QString("%1").arg(freq)));
            response.replace("CPU_VALUE", QString("%1").arg(cpu));
            response.replace("CF_VALUE_A", QString("%1").arg(cfA));
            response.replace("CF_VALUE_B", QString("%1").arg(cfB));

            if (!DRS4BoardManager::sharedInstance()->isDemoModeEnabled()) {
                response.replace("TEMP_VALUE", QString("%1").arg(t));

                for (int i = 0 ; i < 4 ; ++ i) {
                    response.replace(QString("CHN_STATE_A%1").arg(i+1), i == chnA ? QString("checked") : QString("disabled"));
                    response.replace(QString("CHN_STATE_B%1").arg(i+1), i == chnB ? QString("checked") : QString("disabled"));
                }
            }

            if (!int(efficiency_AB)) {
                response.replace("EFFICIENCY_AB_COLOR", QString("#fc0303")); // red
            }
            else {
                response.replace("EFFICIENCY_AB_COLOR", QString("#1c8000")); // green
            }

            response.replace("COUNTS_AB_COLOR", QString("#0938e3")); // blue

            response.replace("OFFSET_VALUE_AB", QString::number(offset_AB, 'f', 2));
            response.replace("SCALE_VALUE_AB", QString::number(scale_AB, 'f', 2));
            response.replace("BIN_WIDTH_AB", QString::number(bin_width_AB, 'f', 2));
            response.replace("NO_CHANNEL_AB", QVariant(no_chn_AB).toString());
            response.replace("EFFICIENCY_AB", QString::number(efficiency_AB, 'f', 2));
            response.replace("COUNTS_AB", QVariant(counts_AB).toString());

            if (!int(efficiency_BA)) {
                response.replace("EFFICIENCY_BA_COLOR", QString("#fc0303")); // red
            }
            else {
                response.replace("EFFICIENCY_BA_COLOR", QString("#1c8000")); // green
            }

            response.replace("COUNTS_BA_COLOR", QString("#0938e3")); // blue

            response.replace("OFFSET_VALUE_BA", QString::number(offset_BA, 'f', 2));
            response.replace("SCALE_VALUE_BA", QString::number(scale_BA, 'f', 2));
            response.replace("BIN_WIDTH_BA", QString::number(bin_width_BA, 'f', 2));
            response.replace("NO_CHANNEL_BA", QVariant(no_chn_BA).toString());
            response.replace("EFFICIENCY_BA", QString::number(efficiency_BA, 'f', 2));
            response.replace("COUNTS_BA", QVariant(counts_BA).toString());

            if (!int(efficiency_prompt)) {
                response.replace("EFFICIENCY_PROMPT_COLOR", QString("#fc0303")); // red
            }
            else {
                response.replace("EFFICIENCY_PROMPT_COLOR", QString("#1c8000")); // green
            }

            response.replace("COUNTS_PROMPT_COLOR", QString("#0938e3")); // blue

            response.replace("OFFSET_VALUE_PROMPT", QString::number(offset_prompt, 'f', 2));
            response.replace("SCALE_VALUE_PROMPT", QString::number(scale_prompt, 'f', 2));
            response.replace("BIN_WIDTH_PROMPT", QString::number(bin_width_prompt, 'f', 2));
            response.replace("NO_CHANNEL_PROMPT", QVariant(no_chn_prompt).toString());
            response.replace("EFFICIENCY_PROMPT", QString::number(efficiency_prompt, 'f', 2));
            response.replace("COUNTS_PROMPT", QVariant(counts_prompt).toString());

            if (!int(efficiency_merged)) {
                response.replace("EFFICIENCY_MERGED_COLOR", QString("#fc0303")); // red
            }
            else {
                response.replace("EFFICIENCY_MERGED_COLOR", QString("#1c8000")); // green
            }

            response.replace("COUNTS_MERGED_COLOR", QString("#0938e3")); // blue

            response.replace("OFFSET_VALUE_MERGED", QString::number(offset_merged, 'f', 2));
            response.replace("SCALE_VALUE_MERGED", QString::number(scale_merged, 'f', 2));
            response.replace("BIN_WIDTH_MERGED", QString::number(bin_width_merged, 'f', 2));
            response.replace("NO_CHANNEL_MERGED", QVariant(no_chn_merged).toString());
            response.replace("EFFICIENCY_MERGED", QString::number(efficiency_merged, 'f', 2));
            response.replace("COUNTS_MERGED", QVariant(counts_merged).toString());

            response.replace("URL", "/");
        }
        else {
            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        file.close();

        respond(DRS4HttpReturnCode::code::ok, response);

        return;
    }
    else if (request.contains("data-")) {
        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        int no_chn = 0;
        double offset = 0.;
        double scale = 0.;
        double bin_width = 0.;
        int counts = 0;
        double efficiency = 0.;

        QVector<int> data;

        if (request == "/data-A-B") {
            no_chn = DRS4SettingsManager::sharedInstance()->channelCntAB();
            offset = DRS4SettingsManager::sharedInstance()->offsetInNSAB();
            scale = DRS4SettingsManager::sharedInstance()->scalerInNSAB();
            bin_width = 1000.*scale/no_chn;
            counts = m_worker->countsSpectrumAB();
            efficiency = m_worker->currentLifetimeABCountRateInHz();

            data = *m_worker->spectrumAB();
        }
        else if (request == "/data-B-A") {
            no_chn = DRS4SettingsManager::sharedInstance()->channelCntBA();
            offset = DRS4SettingsManager::sharedInstance()->offsetInNSBA();
            scale = DRS4SettingsManager::sharedInstance()->scalerInNSBA();
            bin_width = 1000.*scale/no_chn;
            counts = m_worker->countsSpectrumBA();
            efficiency = m_worker->currentLifetimeBACountRateInHz();

            data = *m_worker->spectrumBA();
        }
        else if (request == "/data-merged") {
            no_chn = DRS4SettingsManager::sharedInstance()->channelCntMerged();
            offset = DRS4SettingsManager::sharedInstance()->offsetInNSMerged();
            scale = DRS4SettingsManager::sharedInstance()->scalerInNSMerged();
            bin_width = 1000.*scale/no_chn;
            counts = m_worker->countsSpectrumMerged();
            efficiency = m_worker->currentLifetimeMergedCountRateInHz();

            data = *m_worker->spectrumMerged();
        }
        else if (request == "/data-prompt") {
            no_chn = DRS4SettingsManager::sharedInstance()->channelCntCoincindence();
            offset = DRS4SettingsManager::sharedInstance()->offsetInNSCoincidence();
            scale = DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence();
            bin_width = 1000.*scale/no_chn;
            counts = m_worker->countsSpectrumCoincidence();
            efficiency = m_worker->currentLifetimeCoincidenceCountRateInHz();

            data = *m_worker->spectrumCoincidence();
        }
        else {
            m_worker->setBusy(false);

            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        m_worker->setBusy(false);

        QString response = "<!doctype html><html><head></head><body>";

        response.append("===>> please copy and paste your data ... <<===<br><br>");
        response.append("# bin-width: " + QString::number(bin_width, 'f', 2) + " ps<br>");
        response.append("# counts: " + QVariant(counts).toString() + "<br><br>");

        for (int i = 0 ; i < data.size() ; ++ i)
            response.append(QVariant(data[i]).toString() + "<br>");

        response.append("</body></html>");

        respond(DRS4HttpReturnCode::code::ok, response);

        return;
    }
    else if (request.contains("/spectrum-")) {
        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const bool isRunning = m_worker->isRunning();

        int no_chn = 0;
        double offset = 0.;
        double scale = 0.;
        double bin_width = 0.;
        int counts = 0;
        double efficiency = 0.;

        QVector<int> data;

        QString headline = "";
        QString data_url = "";

        if (request == "/spectrum-A-B") {
            no_chn = DRS4SettingsManager::sharedInstance()->channelCntAB();
            offset = DRS4SettingsManager::sharedInstance()->offsetInNSAB();
            scale = DRS4SettingsManager::sharedInstance()->scalerInNSAB();
            bin_width = 1000.*scale/no_chn;
            counts = m_worker->countsSpectrumAB();
            efficiency = m_worker->currentLifetimeABCountRateInHz();

            data = *m_worker->spectrumAB();

            headline = "lifetime spectrum A-B";
            data_url = "/data-A-B";
        }
        else if (request == "/spectrum-B-A") {
            no_chn = DRS4SettingsManager::sharedInstance()->channelCntBA();
            offset = DRS4SettingsManager::sharedInstance()->offsetInNSBA();
            scale = DRS4SettingsManager::sharedInstance()->scalerInNSBA();
            bin_width = 1000.*scale/no_chn;
            counts = m_worker->countsSpectrumBA();
            efficiency = m_worker->currentLifetimeBACountRateInHz();

            data = *m_worker->spectrumBA();

            headline = "lifetime spectrum B-A";
            data_url = "/data-B-A";
        }
        else if (request == "/spectrum-merged") {
            no_chn = DRS4SettingsManager::sharedInstance()->channelCntMerged();
            offset = DRS4SettingsManager::sharedInstance()->offsetInNSMerged();
            scale = DRS4SettingsManager::sharedInstance()->scalerInNSMerged();
            bin_width = 1000.*scale/no_chn;
            counts = m_worker->countsSpectrumMerged();
            efficiency = m_worker->currentLifetimeMergedCountRateInHz();

            data = *m_worker->spectrumMerged();

            headline = "merged lifetime spectrum";
            data_url = "/data-merged";
        }
        else if (request == "/spectrum-prompt") {
            no_chn = DRS4SettingsManager::sharedInstance()->channelCntCoincindence();
            offset = DRS4SettingsManager::sharedInstance()->offsetInNSCoincidence();
            scale = DRS4SettingsManager::sharedInstance()->scalerInNSCoincidence();
            bin_width = 1000.*scale/no_chn;
            counts = m_worker->countsSpectrumCoincidence();
            efficiency = m_worker->currentLifetimeCoincidenceCountRateInHz();

            data = *m_worker->spectrumCoincidence();

            headline = "prompt spectrum";
            data_url = "/data-prompt";
        }
        else {
            m_worker->setBusy(false);

            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        m_worker->setBusy(false);

        QString html_template_form = ":/webcontent/plot_template";

        QFile file(html_template_form);

        QString response = "";

        if (file.open(QIODevice::ReadOnly)) {
            response.append(file.readAll());

            response.replace("UPDATE_RATE", QString(QVariant(DRS4ProgramSettingsManager::sharedInstance()->httpServerUrlUpdateRate()).toString()));

            int cnt = 0;

            QString spec_x = "";
            QString spec_y = "";

            for (cnt = 0 ; cnt < data.size() ; ++ cnt) {
                spec_x.append(QVariant(cnt).toString() + ",");
                spec_y.append(QVariant(data[cnt]).toString() + ",");
            }

            response.replace("DATA_X", spec_x);
            response.replace("DATA_Y", spec_y);

            response.replace("DATA_URL", data_url);

            response.replace("HEADLINE_COLOR", QString("#3366ff"));
            response.replace("HEADLINE", headline);

            if (!int(efficiency)) {
                response.replace("EFFICIENCY_COLOR", QString("#fc0303")); // red
            }
            else {
                response.replace("EFFICIENCY_COLOR", QString("#1c8000")); // green
            }

            response.replace("COUNTS_COLOR", QString("#0938e3")); // blue

            response.replace("OFFSET", QString::number(offset, 'f', 2));
            response.replace("SCALE", QString::number(scale, 'f', 2));
            response.replace("BIN_WIDTH", QString::number(bin_width, 'f', 2));
            response.replace("NO_CHANNEL", QVariant(no_chn).toString());
            response.replace("EFFICIENCY", QString::number(efficiency, 'f', 2));
            response.replace("COUNTS", QVariant(counts).toString());

            response.replace("VERSION_INFO", QString(MAJOR_VERSION + "." + MINOR_VERSION + " (" + DATE_EXTENSION + ")"));
            response.replace("COPYRIGHT_DATE", QString(COPYRIGHT_DATE));
            response.replace("MEASUREMENT_STATE", isRunning ? QString("running") : QString("idle"));

            response.replace("URL", request);
            response.replace("TITLE", request.remove("/spectrum-"));
        }
        else {
            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        file.close();

        respond(DRS4HttpReturnCode::code::ok, response);

        return;
    }
    else if (request == "/hardware-info-plain") {
        if (DRS4BoardManager::sharedInstance()->isDemoModeEnabled()) {
            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        QJsonDocument doc = DRS4BoardManager::sharedInstance()->hardwareInfo();

        m_worker->setBusy(false);

        QString response = "";

        if (!doc.isEmpty()) {
            response.append("<!doctype html>");
            response.append("<html>");
            response.append("<head>");
            response.append("<title>");
            response.append("DRS4 hardware-configuration & specifications");
            response.append("</title>");
            response.append("</head>");
            response.append("</html>");
            response.append("<body>");
            response.append(QString(doc.toJson()));
            response.append("</body>");
        }
        else {
            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        respond(DRS4HttpReturnCode::code::ok, response);

        return;

        respond(DRS4HttpReturnCode::code::ok, response);

        return;
    }
    else if (request == "/hardware-info") {
        if (DRS4BoardManager::sharedInstance()->isDemoModeEnabled()) {
            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        QJsonDocument doc = DRS4BoardManager::sharedInstance()->hardwareInfo();

        m_worker->setBusy(false);

        QFile file(":/webcontent/info_template");

        QString response = "";

        if (file.open(QIODevice::ReadOnly)) {
            response.append(file.readAll());

            response.replace("UPDATE_RATE", QString(QVariant(DRS4ProgramSettingsManager::sharedInstance()->httpServerUrlUpdateRate()).toString()));

            response.replace("VERSION_INFO", QString(MAJOR_VERSION + "." + MINOR_VERSION + " (" + DATE_EXTENSION + ")"));
            response.replace("COPYRIGHT_DATE", QString(COPYRIGHT_DATE));
            response.replace("TITLE", "DRS4 hardware-configuration & specifications");
            response.replace("URL", request);

            QJsonObject jsonObj = doc.object();
            QString content;

            for (int m = 0 ; m < jsonObj.keys().size() ; ++ m) {
                const QString key = jsonObj.keys().at(m);

                QJsonObject obj = jsonObj[key].toObject();

                if (m)
                    content.append("<hr />");
                // else
                //    content.append("<div class=\"text-center\"><button onclick=\"window.location.href='/hardware-info-plain'\">export as json ...</button></div>");

                content.append(QString("<div class=\"text-center\"><h3><span style=\"color: #000000;\"><strong>/%1</strong></span></h3></div>").arg(key));

                if (obj.keys().size()) {
                    for (int i = 0 ; i < obj.keys().size(); ++ i) {
                        content.append(QString("<div class=\"text-center\"><h4><span style=\"color: #65665f;\"><strong>/%1: </strong></span><span style=\"color: #0938e3;\">%2</span></h4></div>").arg(obj.keys().at(i)).arg(QVariant(obj[obj.keys().at(i)].toVariant()).toString()));
                    }
                }
                else {
                    content.append(QString("<div class=\"text-center\"><h4><span style=\"color: #0938e3;\">%1</span></h4></div>").arg(QVariant(jsonObj[key].toVariant()).toString()));
                }
            }

            response.replace("CONTENT", content);
        }
        else {
            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        file.close();

        respond(DRS4HttpReturnCode::code::ok, response);

        return;
    }
    else if (request == "/software-settings-plain") {
        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        const QString doc = DRS4SettingsManager::sharedInstance()->xmlContent();

        m_worker->setBusy(false);

        QString response = "";

        if (!doc.isEmpty()) {
            response.append("<!doctype html>");
            response.append("<html>");
            response.append("<head>");
            response.append("<title>");
            response.append("software-settings");
            response.append("</title>");
            response.append("</head>");
            response.append("</html>");
            response.append("<body>");
            response.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
            response.append(doc);
            response.append("</body>");
        }
        else {
            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        respond(DRS4HttpReturnCode::code::ok, response);

        return;

        respond(DRS4HttpReturnCode::code::ok, response);

        return;
    }
    else if (request == "/software-settings") {
        if (!DRS4SettingsManager::sharedInstance()->parentNode()) {
            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        QString content = "<ul id=\"myUL\">";

        createTableRecursively(DRS4SettingsManager::sharedInstance()->parentNode(), &content);

        content.append("</ul>");

        m_worker->setBusy(false);

        QFile file(":/webcontent/info_template");

        QString response = "";

        if (file.open(QIODevice::ReadOnly)) {
            response.append(file.readAll());

            response.replace("UPDATE_RATE", QString(QVariant(DRS4ProgramSettingsManager::sharedInstance()->httpServerUrlUpdateRate()).toString()));

            // content.prepend("<div class=\"text-center\"><button onclick=\"window.location.href='/software-settings-plain'\">export as xml ...</button></div>");
            response.replace("VERSION_INFO", QString(MAJOR_VERSION + "." + MINOR_VERSION + " (" + DATE_EXTENSION + ")"));
            response.replace("COPYRIGHT_DATE", QString(COPYRIGHT_DATE));
            response.replace("TITLE", QString("software settings (%1)").arg(DRS4SettingsManager::sharedInstance()->fileName()));

            response.replace("CONTENT", content);
            response.replace("URL", request);
        }
        else {
            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        file.close();

        respond(DRS4HttpReturnCode::code::ok, response);

        return;
    }
    else if (request == "/simulation-input") {
        if (!DRS4BoardManager::sharedInstance()->isDemoModeEnabled()) {
            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        if (!DRS4SimulationSettingsManager::sharedInstance()->parentNode()) {
            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        QMutexLocker locker(&m_mutex);

        if (!m_worker)
            return;

        m_worker->setBusy(true);

        while(!m_worker->isBlocking()) {}

        QString content = "<ul id=\"myUL\">";

        createTableRecursively(DRS4SimulationSettingsManager::sharedInstance()->parentNode(), &content);

        content.append("</ul>");

        m_worker->setBusy(false);

        QFile file(":/webcontent/info_template");

        QString response = "";

        if (file.open(QIODevice::ReadOnly)) {
            response.append(file.readAll());

            response.replace("UPDATE_RATE", QString(QVariant(DRS4ProgramSettingsManager::sharedInstance()->httpServerUrlUpdateRate()).toString()));

            // content.prepend("<div class=\"text-center\"><button onclick=\"window.location.href='/software-settings-plain'\">export as xml ...</button></div>");
            response.replace("VERSION_INFO", QString(MAJOR_VERSION + "." + MINOR_VERSION + " (" + DATE_EXTENSION + ")"));
            response.replace("COPYRIGHT_DATE", QString(COPYRIGHT_DATE));
            response.replace("TITLE", QString("simulation input (%1)").arg(DRS4SimulationSettingsManager::sharedInstance()->fileName()));

            response.replace("CONTENT", content);
            response.replace("URL", request);
        }
        else {
            respond(DRS4HttpReturnCode::code::failed);

            return;
        }

        file.close();

        respond(DRS4HttpReturnCode::code::ok, response);

        return;
    }

    respond(DRS4HttpReturnCode::code::failed);
}

void DRS4ClientConnection::respond(const DRS4HttpReturnCode::code &code, const QString &response) {
    if (!m_socket)
        return;

    if (code == DRS4HttpReturnCode::code::ok) {
        const int length = response.size();

        m_socket->write("HTTP/1.1 200 OK\r\n");
        m_socket->write("Connection: keep-alive\r\n");
        m_socket->write("Content-Type: text/html; charset=UTF-8\r\n");
        m_socket->write(QString(QString("Content-Length: %1\r\n").arg(length)).toUtf8());
        m_socket->write("\r\n");

        if (length)
            m_socket->write(response.toUtf8());
    }
    else {
        m_socket->write("HTTP/1.1 400 Bad Request\r\n");
        m_socket->write("Connection: keep-alive\r\n");
        m_socket->write("Content-Type: text/html; charset=UTF-8\r\n");
        m_socket->write("Content-Length: 0\r\n");
        m_socket->write("\r\n");
    }

    m_socket->flush();
}

void DRS4ClientConnection::createTableRecursively(DSimpleXMLNode *node, QString *content) {
    if (!content
            || !node)
        return;

    if (node->hasValue()) { // type child
        /* nothing here */
    }
    else { // parent type
        content->append("<li>");
        content->append(QString(QString("<span class=\"caret\"><strong>%1</strong></span>").arg(node->nodeName())));
        content->append("<ul class=\"nested\">");
    }

    for (DSimpleXMLNode *child : node->getChilds()) {
        if (!child)
            continue;

        if (child->hasValue()) {
            content->append(QString(QString("<li><span style=\"color: #65665f;\">%1 =</span><span style=\"color: #0938e3;\"> %2</span></li>").arg(child->nodeName()).arg(child->getValue().toString())));
        }

        if (child->hasChilds())
            createTableRecursively(child, content);
    }

    if (node->hasValue()) { // type child
        /* nothing here*/
    }
    else { // parent type
        content->append("</ul>");
        content->append("</li>");
    }
}
