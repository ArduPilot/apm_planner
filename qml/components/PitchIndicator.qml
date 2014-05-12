import QtQuick 1.1

Item {
    id: pitchIndicator
    property real pitchAngle: 0
    property real rollAngle: 0

    width: parent.width
    z:3
    clip: true
    smooth: true
    Column{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: 45

        Repeater {
            model: ["60", "50", "40", "30", "20", "10", "0", "-10", "-20", "-30", "-40", "-50" , "-60"]
            Rectangle { // Graticule Light
                id:graticuleLight
                width: 50
                height: 2
                color: "white"
                smooth: true
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.horizontalCenterOffset: -40
                    anchors.verticalCenter: parent.verticalCenter
                    smooth: true
                    text: modelData
                    color: "white"
                }
            }
        }
    }
    transform: [ Translate {
            id: artHorizonPitch
            y: pitchAngle * 4.5
            },
        Rotation {
            id: artHorizonRotation
            origin.x: width/2
            origin.y: height/2
            //horizon angle
            angle: -rollAngle
            }]
}
