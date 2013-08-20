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
//    (c) 2013 Author: Bill Bonney <billbonney@communistech.com>
//

import QtQuick 1.1

Rectangle {
    id:modeDisplay

    property alias modeText: modeTextId.text
    property alias modeTextColor: modeTextId.color
    property alias modeBackgroundColor: modeDisplay.color
    property color modeBorderColor: "White"

    width: 155
    height: parent.height/3
    anchors.verticalCenter: parent.verticalCenter
    radius: 3
    border.color: modeBorderColor
    border.width: 1

    Text {
        id:modeTextId
        anchors.centerIn: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 14
        font.bold: true
        smooth: true
    }
}
