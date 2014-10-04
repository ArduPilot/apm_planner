/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.ardupilot.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/
/**
 * @file
 *   @brief Droneshare API Query Object
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 */

#include "QsLog.h"
#include "DroneshareAPIBroker.h"

DroneshareAPIBroker::DroneshareAPIBroker(QObject *parent) :
    QObject(parent),
    m_networkReply(NULL),
    m_httpRequestAborted(false)
{
}

DroneshareAPIBroker::~DroneshareAPIBroker()
{
    delete m_networkReply;
    m_networkReply = NULL;
}

void DroneshareAPIBroker::cancel()
{
     m_httpRequestAborted = true;
     m_networkReply->abort();
}

void DroneshareAPIBroker::httpFinished()
{
    QLOG_DEBUG() << "DroneshareUpload::httpFinished()";
    if (!m_httpRequestAborted) {
        // Finished uploading the log
        if (m_networkReply->error()) {
            // upload failed
            emit queryFailed(m_networkReply->errorString());
        } else {
            // Upload success
            QString reply = QString(m_networkReply->readAll());
            QLOG_DEBUG() << "Droneshare: JSON reply: " << reply;
            emit queryComplete(reply);
        }
    }
    if(m_networkReply) {
        m_networkReply->deleteLater();
        m_networkReply = NULL;
    }
}

void DroneshareAPIBroker::downloadProgress(qint64 bytesWritten, qint64 totalBytes)
{
    if (m_httpRequestAborted)
        return;
    QLOG_DEBUG() << "DroneshareQuery: " << m_url << " :"<< bytesWritten << "/" << totalBytes;
    emit queryProgress(bytesWritten, totalBytes);
}

void DroneshareAPIBroker::addQueryItem(const QString &key, const QString &value)
{
    if (!m_url.isEmpty() && (key.length() > 0) && (value.length() > 0)){
        QString query = m_url.query();
        QUrlQuery q(query);
        q.addQueryItem(key,value);
        m_url.setQuery(q);
    }
}

void DroneshareAPIBroker::addQuery(const QString &queryString)
{
    if (queryString.length()>0){
        m_url.clear();
        m_url.setUrl(m_droneshareBaseUrl + queryString);
    }
}

void DroneshareAPIBroker::addBaseUrl(const QString &baseUrl)
{
    m_droneshareBaseUrl = baseUrl;
}

const QUrl &DroneshareAPIBroker::getUrl() const
{
    return m_url;
}

void DroneshareAPIBroker::sendQueryRequest()
{
    if (m_url.isEmpty()){
        QLOG_ERROR() << "Query request invalid";
        return;
    }
    QLOG_DEBUG() << "Droneshare: sendQueryRequest" << m_url;
    QNetworkRequest request(m_url);
    m_networkReply = m_networkAccessManager.get(request);
    connect(m_networkReply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(m_networkReply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(downloadProgress(qint64,qint64)));
}

