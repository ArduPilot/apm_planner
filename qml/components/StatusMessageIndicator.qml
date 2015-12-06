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

    property string message
    property color messageColor

    onMessageColorChanged: {
        messageText.color = messageColor;
    }

    anchors.fill: parent

    border.color: messageColor
    border.width: 5

    color: "Transparent"

    Text {
        id: messageText
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -50
        anchors.horizontalCenter: parent.horizontalCenter
        color: messageColor
        style: Text.Outline
        styleColor: "black"
        text: message
        smooth: true
        font.pointSize: 24
    }

    SequentialAnimation on border.color {
        id: animator
        running: true
        loops: Animation.Infinite
        ColorAnimation {id:toMessageColor; to: messageColor; duration: 1000 }
        ColorAnimation {id:toTransparent; to: "transparent"; duration: 1000 }
    }
}
