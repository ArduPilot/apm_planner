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
//    (c) Author: Bill Bonney <billbonney@communistech.com>
//

import QtQuick 1.1

Rectangle {
    signal clicked

    property string label: "button label"
    property alias image: buttonImage.source
    property int margins: 2

    id: button
    width: 76
    height: 72
    radius: 3
    smooth: true
    border.width: 2

    Text {
        id: buttonLabel
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 5
        text: label
        color: "white"
        font.pixelSize: 9
    }

    Image {
        id: buttonImage
        anchors.horizontalCenter: button.horizontalCenter
        anchors.top: buttonLabel.bottom
        anchors.margins: margins
        source: image
        fillMode: Image.PreserveAspectFit
        width: image.width
        height: image.height
    }

    signal buttonClick()

    onButtonClick: {
        console.log("APMToolBar:" + buttonLabel.text + " clicked calling signal")
        clicked()
    }

    // Highlighting and ativation section
    property color buttonColor: "black"
    property color onHoverbuttonColor: "lightblue"
    property color onHoverColor: "darkblue"
    property color borderColor: "black"

    MouseArea {
        id: buttonMouseArea
        anchors.fill: parent
        onClicked: buttonClick()
        hoverEnabled: true
        onEntered: {
            parent.border.color = onHoverColor
            parent.color = onHoverbuttonColor
        }
        onExited: {
            parent.border.color = borderColor
            parent.color = buttonColor
        }
        onPressed: parent.color = Qt.darker(onHoverbuttonColor, 1.5)
        onReleased: parent.color = buttonColor
    }
    color: buttonColor
    border.color: borderColor
}

