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

#include "drs4updatenotifier.h"

static DRS4UpdateNotifier *__drs4UpdateNotifier = DNULLPTR;

DRS4UpdateNotifier *DRS4UpdateNotifier::sharedInstance()
{
    if (!__drs4UpdateNotifier)
        __drs4UpdateNotifier = new DRS4UpdateNotifier;

    return __drs4UpdateNotifier;
}

bool DRS4UpdateNotifier::checkingForUpdates()
{
    if (!m_networkManager)
            return false;

    emit checkingUpdatesStarted();

    m_networkManager->get(QNetworkRequest(GITHUB_LATEST_RELEASE_GET_URL));

    return true;
}

void DRS4UpdateNotifier::updateReplyFinished(QNetworkReply *reply)
{
    if (!reply
            || !m_networkManager)
        return;

    if (!reply->error()) {
        const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

        /* valid document ? */
        if (!doc.isEmpty()
                && !doc.isNull()) {
            const QJsonObject obj = doc.object();

            const QString latestReleaseTag = obj.value("tag_name").toString();
            const QString latestReleaseUrl = obj.value("html_url").toString();
            const QDateTime latestReleasePublishDateTime = QDateTime::fromString(obj.value("published_at").toString(), Qt::ISODate);

            if (!latestReleaseTag.isEmpty()
                    && !latestReleaseTag.isNull()) {
                const QStringList versionList = latestReleaseTag.split(".");

                if (versionList.size() == 2) {
                    const int majorVersion = QVariant(versionList.at(0)).toInt();
                    const int minorVersion = QVariant(versionList.at(1)).toInt();

                    /* update available ?*/
                    if (majorVersion > QVariant(MAJOR_VERSION).toInt()
                            || (majorVersion == QVariant(MAJOR_VERSION).toInt() && minorVersion > QVariant(MINOR_VERSION).toInt())) {
                        m_updateUrl = latestReleaseUrl;

                        emit updateAvailable(latestReleaseTag, latestReleaseUrl, latestReleasePublishDateTime);
                    }
                    /* latest release ? */
                    else if (majorVersion == QVariant(MAJOR_VERSION).toInt()
                             && minorVersion == QVariant(MINOR_VERSION).toInt()) {
                        emit latestReleaseRunning();
                    }
                    else {
                        /* error */
                        emit error();
                    }
                }
                else {
                    /* error */
                    emit error();
                }
            }
            else {
                /* error */
                emit error();
            }
        }
        else {
           /* error */
            emit error();
        }
    }
    else {
        /* error */
         emit error();
    }

    disconnect(m_networkManager, &QNetworkAccessManager::finished, this, &DRS4UpdateNotifier::updateReplyFinished);
}

void DRS4UpdateNotifier::redirectToUpdate()
{
    if (!m_updateUrl.isEmpty()
            && !m_updateUrl.isNull())
        QDesktopServices::openUrl(QUrl(m_updateUrl));
}

DRS4UpdateNotifier::DRS4UpdateNotifier() :
    QObject(DNULLPTR),
    m_updateUrl("")
{
    m_networkManager = new QNetworkAccessManager(this);

    connect(m_networkManager, &QNetworkAccessManager::finished, this, &DRS4UpdateNotifier::updateReplyFinished);
}

DRS4UpdateNotifier::~DRS4UpdateNotifier()
{
    if (m_networkManager)
        disconnect(m_networkManager, &QNetworkAccessManager::finished, this, &DRS4UpdateNotifier::updateReplyFinished);

    DDELETE_SAFETY(m_networkManager);
    DDELETE_SAFETY(__drs4UpdateNotifier);
}

