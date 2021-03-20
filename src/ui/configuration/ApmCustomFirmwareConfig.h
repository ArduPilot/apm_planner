/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2021 APM_PLANNER PROJECT <http://www.ardupilot.com>

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
 * @file ApmCustomFirmwareConfig.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 20 Mrz 2021
 * @brief File providing header for for selecting firmware for flashing.
 */


#ifndef APMCUSTOMFIRMWARECONFIG_H
#define APMCUSTOMFIRMWARECONFIG_H

#include "PX4FirmwareUploader.h"

#include <QSerialPortInfo>
#include <QTableWidgetItem>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QMessageBox>
#include <QFileInfo>

// UI forward declaration allow faster compiling on UI changes
namespace Ui
{
class ApmCustomFWConfig;
}

/**
 * @brief The ApmCustomFirmwareConfig class provides all functionality needed
 *        to select a firmware which can be flashed into a device.
 */
class ApmCustomFirmwareConfig : public QWidget
{
    Q_OBJECT

public:
    ApmCustomFirmwareConfig(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    enum class DeviceListColumName
    {
        Port,
        Description,
        Manufacturer,
        Location,
        Vendor,
        Product,
        ColumCount  //!< This must always be the last entry
    };

    enum class FirmwareListColumName
    {
        BoardId,
        Manufacturer,
        BrandName,
        FwType,
        FileName,
        ColumCount  //!< This must always be the last entry
    };

    enum class Operation
    {
        None,
        DownloadOnly,
        DownloadAndFlash
    };


    static constexpr const char *s_JSONUrl      {"https://firmware.ardupilot.org/manifest.json"};  //!< URL string for fetching firmware info
    static constexpr const char *s_JSONFormat   {"format-version"};  //!< Format version field name
    static constexpr const char *s_JSONFormatVer{"1.0.0"};           //!< Format version we can accept
    static constexpr const char *s_JSONFWName   {"firmware"};        //!< Name fo the firmwares array in JSON data

    // Some keys used for json parsing
    static constexpr const char *s_Url          {"url"};
    static constexpr const char *s_Platform     {"platform"};
    static constexpr const char *s_FWFormat     {"format"};
    static constexpr const char *s_FWVersion    {"mav-firmware-version"};
    static constexpr const char *s_FWVersionType{"mav-firmware-version-type"};
    static constexpr const char *s_MavType      {"mav-type"};

    static constexpr int s_InvalidIndex = -1;   //!< value for invalid index



    Ui::ApmCustomFWConfig *mp_Ui;          //!< Pointer to UI.

    QList<QSerialPortInfo> m_portInfoList; //!< List of useable serial ports (with devices).

    QJsonObject            m_firmwareData;  //!< Firmware Data if a file was opened
    QByteArray             m_flashImage;    //!< Flash Image which can directly flashed into the FC

    QNetworkAccessManager *mp_networkManager{nullptr};                        //!< Network manager
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> mp_networkReply; //!< Network reply object for downloading all Firmware data

    QJsonArray             m_availableFirmwares;    //!< array with all available firmwares
    QVector<int>           m_categoryIndex;         //!< index to all FW of the selected category (stable, beta, ...)
    QVector<int>           m_platformIndex;         //!< index to all FW of the selected category and platform (fmuv2, edge, navio, ...)
    QVector<int>           m_mavTypeIndex;          //!< index to all FW of the selected category, platform and mavtype (copter, rover, ...)
    QVector<int>           m_versionIndex;          //!< index to all FW of the selected category, platform, mavtype and version

    Operation              m_operation{Operation::None};    //!< Operation - Download only or download and flash

    int                    m_selectedFwIndex{s_InvalidIndex};     //!< List index of the selected Firmware
    int                    m_selectedDeviceIndex{s_InvalidIndex}; //!< List index of the device to flash.

    QScopedPointer<QMessageBox> mp_requestDeviceReplug;

    QScopedPointer<PX4FirmwareUploader, QScopedPointerDeleteLater> mp_px4Updater;  //!< Pointer to the px4 Updater object
    // TODO should not use this temp file!
    QScopedPointer<QTemporaryFile> mp_tempFWFile;       //!< Pointer to the downloaded firmwarefile for flashing

    /**
     * @brief createItem - Creates an item for the table widged with the default settings
     *                     used in the table. The ownership of the item is passed to the caller.
     * @param itemText - Text for this item.
     * @return Pointer to the created item.
     */
    QTableWidgetItem *createItem(const QString &itemText) const;

    bool openFirmwareFile(const QFileInfo &firmwareFile);

    void StartFlashFirmware(const QString &firmwareFileName);

    void fetchAvailableFirmware();

    void startDownloadFirmware();

    void SetButtonState();

private slots:
    void FillDeviceList();

    void deviceTableCellClicked(int row, int column);

    void firmwareTableCellClicked(int row, int column);

    void flashLocalFWButtonClicked();

    void flashFWButtonClicked();

    void firmwareDownloadBtnClicked();

    void availFirmwarefetchFinished();

    void firmwareFetchError(QNetworkReply::NetworkError error);

    void fillFirmwareVersionBox(int index);

    void fillMavtypeBox(int index);

    void fillPlatformBox(int index);

    void fillFirmwareCategoryBox();

    void fillFirmwareList(int index);

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    void downloadFirmwareFinished();

    // slots used by the flashing tool

    void statusUpdate(QString update);

    void startFlashing();

    void flashWarn(QString warning);

    void flashError(QString error);

    void flashingDone();

    void requestDeviceReplug();

    void deviceReplugDetected();

    void cancelButtonClicked();

};

#endif // APMCUSTOMFIRMWARECONFIG_H
