/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>

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
 *   @brief DroneshareUpload helper
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 */

#include "QsLog.h"
#include "DroneshareUpload.h"

DroneshareUpload::DroneshareUpload(QObject *parent) :
    QObject(parent),
    m_uploadFile(NULL),
    m_networkReply(NULL),
    m_httpRequestAborted(false)
{
}

DroneshareUpload::~DroneshareUpload()
{
    delete m_networkReply;
    m_networkReply = NULL;
    delete m_uploadFile;
    m_uploadFile = NULL;
}

void DroneshareUpload::cancel()
{
     m_httpRequestAborted = true;
     m_networkReply->abort();
}

void DroneshareUpload::httpFinished()
{
    QLOG_DEBUG() << "DroneshareUpload::httpFinished()";
    if (!m_httpRequestAborted) {
       // Finished uploading the log
        if (m_networkReply->error()) {
            // upload failed
            emit uploadFailed(QString(m_networkReply->readAll()),
                                      m_networkReply->errorString());
        } else {
            // Upload success
            emit uploadComplete(QString(m_networkReply->readAll()));
        }
    }
}

void DroneshareUpload::updateDataWriteProgress(qint64 bytesWritten, qint64 totalBytes)
{
    if (m_httpRequestAborted)
        return;
    QLOG_DEBUG() << "Uploading: " << m_url << " :"<< bytesWritten << "/" << totalBytes;
    emit uploadProgress(bytesWritten, totalBytes);
}

void DroneshareUpload::uploadLog(const QString &filename, const QString &user, const QString &password,
                                 const QString &vehicleID, const QString &apiKey)
{
    if (m_networkReply != NULL){
        delete m_networkReply;
        m_networkReply = NULL;
    }
    if (m_uploadFile){
        delete m_uploadFile;
        m_uploadFile = NULL;
    }
    // Cleanup from previous upload
    m_url.clear();
    if (m_uploadFile)
        m_uploadFile->close();

    // Setup request url
    m_url.setUrl(DroneshareBaseUrl + "/mission/upload/" + vehicleID);
    QUrlQuery q;
    q.addQueryItem("api_key", apiKey);
    q.addQueryItem("login", user);
    q.addQueryItem("password", password);
    q.addQueryItem("autoCreate", "false");
    m_url.setQuery(q);


    // Open file

    m_uploadFile = new QFile(filename);

    QLOG_DEBUG() << " Droneshare: sending file " << filename << " to " << m_url
                    << "size: " << m_uploadFile->size();

    if (!m_uploadFile->open(QFile::ReadOnly)){
        QLOG_ERROR() << "Failed to open upload file: " << filename;
        return;
    }

    QNetworkRequest request(m_url);
    request.setHeader(QNetworkRequest::ContentLengthHeader, m_uploadFile->size());

    if (filename.endsWith(".tlog")){
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/vnd.mavlink.tlog");
    } else if (filename.endsWith(".blog") || filename.endsWith(".bin")) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/vnd.mavlink.blog");
    } else if (filename.endsWith(".log") || filename.endsWith(".flog")) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/vnd.mavlink.flog");
    } else {
        QLOG_ERROR() << " Error, incorrect file type";
        return;
    }

    m_networkReply = m_networkAccessManager.post(request, m_uploadFile);
    connect(m_networkReply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(m_networkReply, SIGNAL(uploadProgress(qint64,qint64)),
            this, SLOT(updateDataWriteProgress(qint64,qint64)));
}

