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

Item {
    id: root
    property real airSpeed: 0
    property real groundSpeed: 0
    property color color: "white"
    property color colorOutline: "black"

    Column{
        anchors {left: parent.left; bottom: parent.bottom}
        Text {
            color: root.color
            styleColor: root.colorOutline
            style: Text.Outline
            text: "AS: " + airSpeed.toFixed(1)
        }
        Text {
            color: root.color
            styleColor: root.colorOutline
            style: Text.Outline
            text: "GS: " + groundSpeed.toFixed(1)
        }
    }
}
