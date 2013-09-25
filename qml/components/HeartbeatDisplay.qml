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
    id:heartbeatDisplay

    property bool heartbeat: false
    property bool stopAnimation: false
    property alias heartbeatBackgroundColor: heartbeatDisplay.color
    property color heartbeatBorderColor: "White"

    onHeartbeatChanged: {
       if (heartbeat) {
           console.debug("HBD: Heartbeat started")
           warnAnimator.stop()
           animator.start()

        } else {
            console.debug("HBD: Heartbeat stopped")
            animator.stop()
            warnAnimator.start()
        }
    }

    onStopAnimationChanged: {
        if (stopAnimation) {
            console.debug("HBD: Stop heartbeat anitmations")
            warnAnimator.stop()
            animator.stop()
            heartbeatDisplay.color = "black"
        } else {
            console.debug("HBD: Stop heartbeat anitmations")
            if (heartbeat) {
                animator.start()
            } else {
                warnAnimator.start();
            }
        }
    }

    width: parent.height/3
    height: parent.height/3
    anchors.verticalCenter: parent.verticalCenter
    radius: 3
    border.color: heartbeatBorderColor
    border.width: 1

    SequentialAnimation on color {
        id: animator
        running: false
        loops: Animation.Infinite
        ColorAnimation {id:toDark; from: "#24e448"; to: "#003000"; duration: 1000 }
        ColorAnimation {id:toLight; from: "#003000"; to: "#24e448"; duration: 1000 }
    }

    SequentialAnimation on color {
        id: warnAnimator
        running: false
        loops: Animation.Infinite
        ColorAnimation {id:toRed; to: "#FF0000"; duration: 500 }
        ColorAnimation {id:toBlack; to: "#000000"; duration: 500 }
    }
}
