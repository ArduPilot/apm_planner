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
import "./components"


Rectangle {
    id: toolbar

    property int rowSpacerSize: 3
    property int linkDeviceSize: 100

    property alias backgroundColor : toolbar.color
    property alias linkNameLabel: linkDevice.label
    property alias baudrateLabel: baudrate.label
    property bool connected: false
    property bool armed: false
    property string armedstr: "status"
    property bool enableStatusDisplay: true

    property alias modeText: modeTextId.modeText
    property alias modeTextColor: modeTextId.modeTextColor
    property alias modeBkgColor: modeTextId.modeBackgroundColor
    property alias modeBorderColor: modeTextId.modeBorderColor

    property alias heartbeat: heartbeatDisplayId.heartbeat
    property bool stopAnimation: false
    property alias disableConnectWidget: connectionWidget.disable

    function setArmed(armedState) {
        if (armedState) {
            statusDisplayId.statusText = "ARMED"
            statusDisplayId.statusTextColor = "red"
            statusDisplayId.statusBackgroundColor = "#FF880000"

        } else {
            statusDisplayId.statusText = "DISARMED"
            statusDisplayId.statusTextColor = "yellow"
            statusDisplayId.statusBackgroundColor = "black"
        }
    }

    function clearArmedMode() {
        // clear indicators from showing info
        statusDisplayId.statusText = "status"
        statusDisplayId.statusTextColor = "yellow"
        statusDisplayId.statusBackgroundColor = "black"
        modeTextId.modeText = "mode"
    }

    function setAdvancedMode(state){
        terminalView.visible = state
    }

    width: toolbar.width
    height: 72
    color: "black"
    border.color: "black"

    onArmedChanged: {
        setArmed(armed)
    }

    onConnectedChanged: {
        if (connected){
            console.log("APM Tool BAR QML: connected")
            connectButton.image = "./resources/apmplanner/toolbar/disconnect.png"
            connectButton.label = "DISCONNECT"
            setArmed(armed)

        } else {
            console.log("APM Tool BAR QML: disconnected")
            connectButton.image = "./resources/apmplanner/toolbar/connect.png"
            connectButton.label = "CONNECT"
            stopAnimation = true;
            clearArmedMode()
        }
    }

    onEnableStatusDisplayChanged: {
        if (enableStatusDisplay) {
            statusSpacerId.visible = true
            statusDisplayId.visible = true
        } else {
            statusSpacerId.visible = false
            statusDisplayId.visible = false
        }
    }

// [BB] The code below should work, not sure why. replaced with code above
//    Connections {
//            target: globalObj
//            onMAVConnected: {
//                console.log("QML Change Connection " + connected)
//                if (connected){
//                    console.log("connected")
//                    connectButton.image = "./resources/apmplanner/toolbar/disconnect.png"
//                } else {
//                    console.log("disconnected")
//                    connectButton.image = "./resources/apmplanner/toolbar/connect.png"
//                }
//            }
//    }

    Row {
        anchors.left: parent.left
        spacing: rowSpacerSize

        Rectangle { // Spacer
            width: 5
            height: parent.height
            color: "black"
        }

        Button {
            id: flightDataView
            label: "FLIGHT DATA"
            image: "./resources/apmplanner/toolbar/flightdata.png"
            onClicked: {
                globalObj.triggerFlightView()
            }
        }

        Button {
            id: flightPlanView
            label: "FLIGHT PLAN"
            image: "./resources/apmplanner/toolbar/flightplanner.png"
            onClicked: globalObj.triggerFlightPlanView()
        }

        Button {
            id: initialSetupView
            label: "INITIAL SETUP"
            image: "./resources/apmplanner/toolbar/light_initialsetup_icon.png"
//            margins: 8
            onClicked: globalObj.triggerInitialSetupView()
        }

        Button {
            id: configTuningView
            label: "CONFIG/TUNING"
            image: "./resources/apmplanner/toolbar/light_tuningconfig_icon.png"
//            margins: 8
            onClicked: globalObj.triggerConfigTuningView()
        }

        Button {
            id: plotView
            label: "GRAPHS"
            image: "./resources/apmplanner/toolbar/simulation.png"
            onClicked: globalObj.triggerPlotView()
        }

// [TODO] removed from toolbar until we have simulation working
//        Button {
//            id: simulationView
//            label: "SIMULATION"
//            image: "./resources/apmplanner/toolbar/simulation.png"
//            onClicked: globalObj.triggerSimulationView()
//        }

        Button {
            id: terminalView
            label: "TERMINAL"
            image: "./resources/apmplanner/toolbar/terminal.png"
            onClicked: globalObj.triggerTerminalView()
            visible: false
        }

        Rectangle { // Spacer
            id: statusSpacerId
            width: 5
            height: parent.height
            color: "black"
        }

        StatusDisplay {
            id: statusDisplayId
            width: 110
            statusText: "status"
            statusTextColor: "yellow"
            statusBackgroundColor: "black"
        }

        Rectangle { // Spacer
            width: 5
            height: parent.height
            color: "black"
        }

        ModeDisplay {
            id:modeTextId
            modeText: "mode"
            modeTextColor: "red"
            modeBackgroundColor: "black"
            modeBorderColor: "white"
        }

        Rectangle { // Spacer
            width: 5
            height: parent.height
            color: "black"
        }

        HeartbeatDisplay {
            id:heartbeatDisplayId
            heartbeatBackgroundColor: "black"
            stopAnimation: stopAnimation
        }

//            DigitalDisplay { // Information Pane
//                title: "Speed"
//                textValue: "11.0m/s"
//                color: "black"
//            }
//            DigitalDisplay { // Information Pane
//                title: "Alt"
//                textValue: "20.0m"
//                color: "black"
//            }
//            DigitalDisplay { // Information Pane
//                title: "Volts"
//                textValue: "14.8V"
//                color: "black"
//            }
//            DigitalDisplay { // Information Pane
//                title: "Current"
//                textValue: "12.0A"
//                color: "black"
//            }
//            DigitalDisplay { // Information Pane
//                title: "Level"
//                textValue: "77%"
//                color: "black"
//            }

    }

    Row {
        id: connectionWidget
        anchors.right: parent.right
        spacing: 2

        property bool disable: false

        onDisableChanged:{
            if(disable){
                visible = false;
            } else {
                visible = true;
            }
        }

        TextButton {
            id: linkDevice
            label: "none"
            minWidth: linkDeviceSize

            onClicked: globalObj.showConnectionDialog()
        }

        TextButton {
            id: baudrate
            label: "none"
            minWidth: 70

            onClicked: globalObj.showConnectionDialog()
        }

        Rectangle {
            width: 5
            height: parent.height
            color: "black"
        }

        Button {
            id: connectButton
            label: "CONNECT"
            image: "./resources/apmplanner/toolbar/connect.png"
            onClicked: globalObj.connectMAV()
        }

        Rectangle { // Spacer
            width: 5
            height: parent.height
            color: "black"
        }
    }
}
