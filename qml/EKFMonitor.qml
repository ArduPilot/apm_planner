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
        velocity.value = Qt.binding(function() { return vehicleOverview.velocity_variance})
        positionHoriz.value = Qt .binding(function() { return vehicleOverview.pos_horiz_variance})
        positionVert.value = Qt.binding(function() { return vehicleOverview.pos_vert_variance})
        compass.value = Qt.binding(function() { return vehicleOverview.compass_variance})
        terrainAlt.value = Qt.binding(function() { return vehicleOverview.terrain_alt_variance})
    }

    function activeUasUnset() {
        console.log("EKF Monitor: Active UAS is now unset");
    }

    Row {
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        spacing: 5

        BarGauge {
            id: velocity
            width: 40
            height: 0.8*parent.height
            anchors.verticalCenter: parent.verticalCenter
            warnValue: 0.5
            failValue: 0.8
            label: "X"
        }

        BarGauge {
            id: positionHoriz
            width: 40
            height: 0.8*parent.height
            anchors.verticalCenter: parent.verticalCenter
            warnValue: 0.5
            failValue: 0.8
            label: "Y"
        }

        BarGauge {
            id: positionVert
            width: 40
            height: 0.8*parent.height
            anchors.verticalCenter: parent.verticalCenter
            warnValue: 0.5
            failValue: 0.8
            label: "Z"
        }

        BarGauge {
            id: compass
            width: 40
            height: 0.8*parent.height
            anchors.verticalCenter: parent.verticalCenter
            warnValue: 0.5
            failValue: 0.8
            label: "Z"
        }

        BarGauge {
            id: terrainAlt
            width: 40
            height: 0.8*parent.height
            anchors.verticalCenter: parent.verticalCenter
            warnValue: 0.5
            failValue: 0.8
            label: "Z"
        }


        Column {
            anchors.verticalCenter: parent.verticalCenter

            Text {
                text: "<b>Flags</b>"
            }

//            Text {
//                id: clip0
//                property double value: 0.0
//                text: qsTr(" Primary \t") + value
//                font.pixelSize: 12
//            }

//            Text {
//                id: clip1
//                property double value: 0.0
//                text: qsTr(" Secondary \t") + value
//                font.pixelSize: 12
//            }

//            Text {
//                id: clip2
//                property double value: 0.0
//                text: qsTr(" Tertiary \t") + value
//                font.pixelSize: 12
//            }

        }
    }


}

