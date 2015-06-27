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
//    (c) 2014 Author: Bill Bonney <billbonney@communistech.com>
//

import QtQuick 2.0

Rectangle {
    id: root
    property real heading: 0
    property real scale: 0.8

    anchors.horizontalCenterOffset: -compassImage.width/2
    color: "transparent"
    border.color: "green"
    border.width: 5
    transform: Translate{
        y: parent.height - compassImage.height/2
    }

    Image { // Compass
        id: compassImage
        source: "../resources/components/rollPitchIndicator/compass.svg"
        smooth: true
        rotation: -heading
        scale: parent.scale
    }

    Image {
        id: compassIndicator
        y: -compassIndicator.height/2
        anchors.horizontalCenter: compassImage.horizontalCenter
        source: "../resources/components/rollPitchIndicator/compassIndicator.svg"
        smooth: true
        scale: parent.scale
    }

    Rectangle{
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: compassImage.height/2 - 40
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: compassImage.width/2

        height: 20
        width: 50
        scale: parent.scale

        border.color: "white"
        border.width: 2
        z:1
        color: "black"
        Text {
            anchors.centerIn: parent
            id: headingText
            text: heading.toFixed(0)
            color: "white"
            font.pixelSize: 0.8*parent.height
        }
    }
}
