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

#ifndef DRONESHAREUPLOADDIALOG_H
#define DRONESHAREUPLOADDIALOG_H

#include <QDialog>

class DroneshareUpload;
class DroneshareAPIBroker;
class UASInterface;

namespace Ui {
class DroneshareUploadDialog;
}

class DroneshareUploadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DroneshareUploadDialog(QWidget *parent = 0);
    ~DroneshareUploadDialog();

public slots:
    void activeUASSet(UASInterface *uas);

    void closeButtonClicked();
    void uploadClicked();
    void acceptUserLogin(QString& username, QString& password, int indexNumber);
    void vehicleQueryComplete(const QString& jsonRepsonse);
    void vehicleQueryFailed(const QString& errorString);
    void startLogUpload(const QString &vehicleUuid);
    void uploadComplete(const QString& jsonResponse);
    void uploadFailed(const QString &jsonResponse, const QString &error);
    void uploadProgress(int bytesRead, int totalBytes);

private:
    void resetStatusText();
    void hideEvent(QHideEvent *evt);

private:
    Ui::DroneshareUploadDialog *ui;
    UASInterface* m_uasInterface;
    QString m_filename;
    DroneshareUpload* m_droneshareUpload;
    DroneshareAPIBroker* m_droneshareQuery;
    QString m_username;
    QString m_password;
};

#endif // DRONESHAREUPLOADDIALOG_H
