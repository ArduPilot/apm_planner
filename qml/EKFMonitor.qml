//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    (c) 2015 Author: Bill Bonney <billbonney@communistech.com>
//

import QtQuick 2.2

import "./components"

Rectangle {
    // Property Defintions
    id:root

    function activeUasSet() {
        console.log("EKF Monitor: Active UAS is now set");
        ekfVelocity.value = Qt.binding(function() { return vehicleOverview.ekf_velocity_variance})
        ekfPositionHoriz.value = Qt .binding(function() { return vehicleOverview.ekf_pos_horiz_variance})
        ekfPositionVert.value = Qt.binding(function() { return vehicleOverview.ekf_pos_vert_variance})
        ekfCompass.value = Qt.binding(function() { return vehicleOverview.ekf_compass_variance})
        ekfTerrainAlt.value = Qt.binding(function() { return vehicleOverview.ekf_terrain_alt_variance})
    }

    function activeUasUnset() {
        console.log("EKF Monitor: Active UAS is now unset");
    }

    property color normalColor: "#32af4f"
    property color warnColor: "orange"
    property color failColor: "red"

        Row {
            id: row1
            anchors.rightMargin: 10
            anchors.topMargin: 10
            anchors.bottomMargin: 10
            anchors.leftMargin: 10
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: key.top
            spacing: 5

            BarGauge {
                id: ekfVelocity
                width: 40
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                warnValue: 0.5
                failValue: 0.8
                label: "Vel"
            }

            BarGauge {
                id: ekfPositionHoriz
                width: 40
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                warnValue: 0.5
                failValue: 0.8
                label: "Pos H"
            }

            BarGauge {
                id: ekfPositionVert
                width: 40
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                warnValue: 0.5
                failValue: 0.8
                label: "Pos V"
            }

            BarGauge {
                id: ekfCompass
                width: 40
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                warnValue: 0.5
                failValue: 0.8
                label: "Mag"
            }

            BarGauge {
                id: ekfTerrainAlt
                width: 40
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                warnValue: 0.5
                failValue: 0.8
                label: "Terrain"
            }
        }

        Rectangle {
            id: key
            width: parent.width
            height: 45
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            Column {
                anchors.margins: 5
                anchors.left: parent.left
                Text {
                    color: normalColor
                    text: "Green = GOOD"
                }
                Text {
                    color: warnColor
                    text: "Oange = WARN"
                }
                Text {
                    color: failColor
                    text: "Red = FAIL"
                }
            }
        }
}

