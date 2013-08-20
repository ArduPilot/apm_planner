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

    property string label: "Text Button label"
    property int minWidth: 75
    property int minHeight: 0
    property int margin: 5

    width: textBox.width
    height: 72
    anchors.verticalCenter: parent.verticalCenter
    color: "black"

    signal buttonClick()

    onButtonClick: {
        console.log("APMToolBar:" + label + " clicked calling signal")
        clicked()
    }

    // Highlighting and ativation section
    property color buttonColor: "black"
    property color onHoverbuttonColor: "lightblue"
    property color onHoverColor: "darkblue"
    property color borderColor: "white"

    Rectangle {
        width: textButtonLabel.paintedwidth
        anchors.centerIn: parent

        Rectangle{
            id: textBox
            anchors.centerIn: parent
            width: minWidth > textButtonLabel.paintedWidth + margin ? minWidth : textButtonLabel.paintedWidth + margin
            height: minHeight > textButtonLabel.paintedHeight + margin ? minHeight : textButtonLabel.paintedHeight + margin

            Text {
                id: textButtonLabel
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.margins: 2
                text: label
                color: "white"
                font.pointSize: 11
            }

            MouseArea {
                id: textButtonMouseArea
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
            border.width: 1
        }
   }
}
