import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
    id: root
    property real value: 0
    property color valueColor: "white"
    property real minimum: 0.0
    property real maximum: 1.0

    property string label: "value"
    property color labelColor: "black"

    property real warnValue: 0.3
    property real failValue: 0.6

    property color normalColor: "#32af4f"
    property color warnColor: "orange"
    property color failColor: "red"

    property color actualColor: normalColor

    onValueChanged: {
        if (value > failValue*bar.maximumValue){
            actualColor = failColor
            return;
        }
        if (value > warnValue*bar.maximumValue ) {
            actualColor = warnColor;
            return;
        }
        actualColor = normalColor
    }

    ProgressBar {
        id: bar
        width: parent.width
        height: parent.height
        minimumValue: root.minimum
        maximumValue: root.maximum
        value: root.value
        orientation: Qt.Vertical
        style: ProgressBarStyle {
            background: Rectangle {
                radius: 2
                color: "lightgray"
                border.color: "gray"
                border.width: 1
                implicitWidth: bar.width
                implicitHeight: bar.height
            }
            progress: Rectangle {
                id: barProgressStyle
                color: actualColor
                border.color: "black"
            }
        }
    }

    Text {
        anchors.centerIn: parent
        text: root.value.toFixed(1)
        color: root.valueColor
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: root.horizontalCenter
        text: root.label
        color: root.labelColor
    }
}
