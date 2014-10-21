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

import QtQuick 2.1

Rectangle {

    property alias title: displayTitle.text
    property string textValue: "none"

    width: 110
    height: parent.height/3
    anchors.verticalCenter: parent.verticalCenter
    border.color: "white"

    Text {
        id:displayTitle
        anchors.left: parent.left
        anchors.leftMargin: 3
        anchors.verticalCenter: parent.verticalCenter
        text: "blank"
        color: "white"
    }

    Text {
        id:displayValue
        anchors.right: parent.right
        anchors.rightMargin: 3
        anchors.verticalCenter: parent.verticalCenter
        text: textValue
        color: "white"
    }
}
