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
        console.log("Vibration Monitor: Active UAS is now set");
        gaugeX.value = Qt.binding(function() { return vehicleOverview.vibration_x})
        gaugeY.value = Qt.binding(function() { return vehicleOverview.vibration_y})
        gaugeZ.value = Qt.binding(function() { return vehicleOverview.vibration_z})

        clip0.value = Qt.binding(function() { return vehicleOverview.clipping_0})
        clip1.value = Qt.binding(function() { return vehicleOverview.clipping_1})
        clip2.value = Qt.binding(function() { return vehicleOverview.clipping_2})
    }

    function activeUasUnset() {
        console.log("Vibration Monitor: Active UAS is now unset");
        //Code to make display show a lack of connection here.
    }

    property color normalColor: "#32af4f"
    property color warnColor: "orange"
    property color failColor: "red"

    Row {
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.fill: parent
        spacing: 5

        BarGauge {
            id: gaugeX
            width: 40
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            minimum: 0
            maximum: 100
            label: "X"
        }

        BarGauge {
            id: gaugeY
            width: 40
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            minimum: 0
            maximum: 100
            label: "Y"
        }

        BarGauge {
            id: gaugeZ
            width: 40
            height: parent.height
            anchors.verticalCenter: parent.verticalCenter
            minimum: 0
            maximum: 100
            label: "Z"
        }

        Column {
            width: 88
            anchors.margins: 10
            anchors.verticalCenter: parent.verticalCenter

            Text {
                width: 50
                text: "<b>Clipping</b>"
            }

            Text {
                id: clip0
                property double value: 0.0
                text: qsTr("Primary \t") + value
                font.pixelSize: 12
            }

            Text {
                id: clip1
                property double value: 0.0
                text: qsTr("Secondary \t") + value
                font.pixelSize: 12
            }

            Text {
                id: clip2
                property double value: 0.0
                text: qsTr("Tertiary \t") + value
                font.pixelSize: 12
            }

            Text {
                id: spacer
                text: qsTr("\n")
                font.pixelSize: 12
            }

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

