import QtQuick 2.9

// Vertical bar gauge. Rewritten to use plain QtQuick primitives so it builds on
// both Qt5 and Qt6: the old QtQuick.Controls 1.2 ProgressBar + ProgressBarStyle
// (with minimumValue/maximumValue/orientation/style) were removed in Qt6.
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

    // Fraction of the bar to fill, clamped to [0, 1].
    property real fillFraction: {
        if (maximum <= minimum)
            return 0;
        var f = (value - minimum) / (maximum - minimum);
        return Math.max(0, Math.min(1, f));
    }

    onValueChanged: {
        if (value > failValue*root.maximum){
            actualColor = failColor
            return;
        }
        if (value > warnValue*root.maximum ) {
            actualColor = warnColor;
            return;
        }
        actualColor = normalColor
    }

    // Bar background
    Rectangle {
        id: bar
        anchors.fill: parent
        radius: 2
        color: "lightgray"
        border.color: "gray"
        border.width: 1

        // Progress fill, growing from the bottom
        Rectangle {
            id: barProgress
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: bar.border.width
            height: (bar.height - 2*bar.border.width) * root.fillFraction
            color: root.actualColor
            border.color: "black"
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
