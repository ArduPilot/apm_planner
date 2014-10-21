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

import QtQuick 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtGStreamer 1.0
import QtQuick.LocalStorage 2.0
import "Storage.js" as Settings

import "./components"

Rectangle {
    // Property Defintions
    id:root
    property bool enableBackgroundVideo: false
    property string statusMessage: ""
    property bool showStatusMessage: false
	property bool enableFullScreen: false
	property string navMode: ""
	property bool popupVisible: false

    function activeUasSet() {
		rollPitchIndicator.rollAngle = Qt.binding(function() { return relpositionoverview.roll})
        rollPitchIndicator.pitchAngle = Qt.binding(function() { return  relpositionoverview.pitch})
        pitchIndicator.rollAngle = Qt.binding(function() { return relpositionoverview.roll})
        pitchIndicator.pitchAngle = Qt.binding(function() { return  relpositionoverview.pitch})
        speedIndicator.groundspeed = Qt.binding(function() { return relpositionoverview.groundspeed})
        informationIndicator.groundSpeed = Qt.binding(function() { return relpositionoverview.groundspeed})
        informationIndicator.airSpeed = Qt.binding(function() { return relpositionoverview.airspeed })
        informationIndicator.batVoltage = Qt.binding(function() { return vehicleoverview.voltage_battery/1000.0 })
        informationIndicator.batCurrent = Qt.binding(function() { return vehicleoverview.current_battery/100.0 })
        informationIndicator.batPercent = Qt.binding(function() { return vehicleoverview.battery_remaining })

        compassIndicator.heading = Qt.binding(function() {
            return (relpositionoverview.yaw < 0) ? relpositionoverview.yaw + 360 : relpositionoverview.yaw ;
        })
        speedIndicator.airspeed = Qt.binding(function() { return relpositionoverview.airspeed } )
        altIndicator.alt = Qt.binding(function() { return abspositionoverview.relative_alt } )
	}
    
	Component.onCompleted:
	{
		root.enableBackgroundVideo = Settings.get("enableBackgroundVideo", false) == 0 ? false : true
		rollPitchIndicator.enableRollPitch = Settings.get("enableRollPitchIndicator", true) == 0 ? false : true
		pitchIndicator.visible = Settings.get("enablePitchIndicator", true) == 0 ? false : true
		altIndicator.visible = Settings.get("enableAltIndicator", true) == 0 ? false : true
		speedIndicator.visible = Settings.get("enableSpeedIndicator", true) == 0 ? false : true
		compassIndicator.visible = Settings.get("enableCompassIndicator", true) == 0 ? false : true
		informationIndicator.visible = Settings.get("enableInformationIndicator", true) == 0 ? false : true
		brightnessSlider.value = Settings.get("brightness",0);
		contrastSlider.value = Settings.get("contrast",0);
		hueSlider.value = Settings.get("hue",0);
		saturationSlider.value = Settings.get("saturation",0);
	}
	
	function activeUasUnset() {
        console.log("PFD-QML: Active UAS is now unset");
        //Code to make display show a lack of connection here.
    }

    onShowStatusMessageChanged: {
        statusMessageTimer.start()
    }

    Timer{
        id: statusMessageTimer
        interval: 5000;
        repeat: false;
        onTriggered: showStatusMessage = false
    }

	VideoItem {
		id: video
		visible: enableBackgroundVideo
		width: root.width
		height: root.height
		surface: videoSurface1 //bound on the context from main()
	}

	Menu { 
		id: contextMenu
        
		MenuItem { 
            text: "Video"
			checkable: true
			checked: enableBackgroundVideo
			onTriggered: 
			{
				root.enableBackgroundVideo = !root.enableBackgroundVideo
				Settings.set("enableBackgroundVideo", root.enableBackgroundVideo)
			}
        }

        MenuItem { 
            text: "Roll/Pitch"
			checkable: true
			checked: rollPitchIndicator.enableRollPitch
			onTriggered: 
			{
				rollPitchIndicator.enableRollPitch = !rollPitchIndicator.enableRollPitch
				Settings.set("enableRollPitchIndicator", rollPitchIndicator.enableRollPitch)
			}
        }

        MenuItem { 
            text: "Pitch"
			checkable: true
			checked: pitchIndicator.visible
			onTriggered:
			{			
				pitchIndicator.visible = !pitchIndicator.visible
				Settings.set("enablePitchIndicator", pitchIndicator.visible)
			}
        }
		
        MenuItem { 
            text: "Altitude"
			checkable: true
			checked: altIndicator.visible
			onTriggered: 
			{
				altIndicator.visible = !altIndicator.visible
				Settings.set("enableAltIndicator", altIndicator.visible)
			}
        }

        MenuItem { 
            text: "Speed"
			checkable: true
			checked: speedIndicator.visible
			onTriggered: 
			{
				speedIndicator.visible = !speedIndicator.visible
				Settings.set("enableSpeedIndicator", speedIndicator.visible)
			}
        }

        MenuItem { 
            text: "Compass"
			checkable: true
			checked: compassIndicator.visible
			onTriggered: 
			{
				compassIndicator.visible = !compassIndicator.visible
				Settings.set("enableCompassIndicator", compassIndicator.visible)
			}
        }

        MenuItem { 
            text: "Information"
			checkable: true
			checked: informationIndicator.visible
			onTriggered: 
			{
				informationIndicator.visible = !informationIndicator.visible
				Settings.set("enableInformationIndicator", informationIndicator.visible)
			}
        }
		
		MenuItem {
            text: "FullScreen"
			checkable: true
			checked: root.enableFullScreen
			onTriggered: container.fullScreenMode = !container.fullScreenMode
		}
    }
	
	MouseArea {
		anchors.fill: parent
		acceptedButtons: Qt.LeftButton | Qt.RightButton
		onClicked: {
			if (mouse.button == Qt.RightButton)
			{
				contextMenu.popup()
			}
		}
	}	

	RollPitchIndicator {
		id: rollPitchIndicator
		anchors.centerIn: parent
		rollAngle: 0
		pitchAngle: 0
		enableBackgroundVideo: parent.enableBackgroundVideo
    }

    PitchIndicator {
        id: pitchIndicator
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        opacity: 0.6

        pitchAngle: 0
        rollAngle: 0
    }

    AltitudeIndicator {
        id: altIndicator
        anchors.right: parent.right
        width: 35
        alt: 0
    }

    SpeedIndicator {
        id: speedIndicator
        anchors.left: parent.left
        width: 35
        airspeed: 0
        groundspeed: 0
    }

    CompassIndicator {
        id: compassIndicator
        anchors.horizontalCenter: parent.horizontalCenter
        transform: Translate {
            y: 20
        }

        heading: 0
    }

    StatusMessageIndicator  {
        id: statusMessageIndicator
        anchors.fill: parent
        message: statusMessage
        visible: showStatusMessage
    }

    InformationOverlayIndicator{
        id: informationIndicator
        anchors.fill: parent
        airSpeed: 0
        groundSpeed: 0
        batVoltage: 0
        batCurrent: 0
        batPercent: 0
		navMode : parent.navMode
    }
	
	Rectangle
    {
		color: "lightgrey"
		border.color: "black"
		width: 10
        height:10
		visible: root.enableBackgroundVideo
		anchors
		{
			left: parent.left; top: parent.top
		}
 
        MouseArea
        {
            anchors.fill: parent
			hoverEnabled: true
 
            onEntered:
            {
                root.popupVisible = true
            }
        }
	}
	
	Rectangle
	{
		color: "lightgrey"
		width: 250
		height: 125
		z:3
		
		visible: root.popupVisible

		anchors
		{
			left: parent.left; top: parent.top
		}

		Column
		{
			anchors
			{
				left: parent.left; top: parent.top; leftMargin: 10; topMargin: 10
			}

			Text 
			{
				height: 20
				text: "Brightness"
			}
			Text 
			{
				height: 20
				text: "Contrast"
			}
			Text 
			{
				height: 20
				text: "Hue"
			}
			Text 
			{
				height: 20
				text: "Saturation"
			}
		}
		
		Column
		{
			width: 165
			z:3
			anchors
			{
				right: parent.right; top: parent.top; topMargin: 10; rightMargin: 10
			}

			Slider
			{
				id: brightnessSlider
				width: parent.width
				height: 20
				z:3
				minimumValue: -100
				maximumValue: 100
				stepSize: 1
				Binding { target: player; property: "brightness"; value: brightnessSlider.value }
				onValueChanged:
				{
					Settings.set("brightness", brightnessSlider.value)
				}
			}
			Slider
			{
				id: contrastSlider
				width: parent.width
				height: 20
				z:3
				minimumValue: -100
				maximumValue: 100
				stepSize: 1
				Binding { target: player; property: "contrast"; value: contrastSlider.value }
				onValueChanged:
				{
					Settings.set("contrast", contrastSlider.value)
				}
			}
			Slider
			{
				id: hueSlider
				width: parent.width
				height: 20
				z:3
				minimumValue: -100
				maximumValue: 100
				stepSize: 1
				Binding { target: player; property: "hue"; value: hueSlider.value }
				onValueChanged:
				{
					Settings.set("hue", hueSlider.value)
				}
			}
			Slider
			{
				id: saturationSlider
				width: parent.width
				height: 20
				z:3
				minimumValue: -100
				maximumValue: 100
				stepSize: 1
				Binding { target: player; property: "saturation"; value: saturationSlider.value }
				onValueChanged:
				{
					Settings.set("saturation", saturationSlider.value)
				}
			}
		}

		Action 
		{
			id: play
			enabled: player.stopped || player.paused
			iconSource: "../resources/components/primaryFlightDisplay/play.svg"
			tooltip: "Play"
			onTriggered: player.playing = true
		}

		Action 
		{
			id: pause
			enabled: player.playing
			iconSource: "../resources/components/primaryFlightDisplay/pause.svg"
			tooltip: "Pause"
			onTriggered: player.paused = true
		}
		
		Action 
		{
			id: stop
			enabled: player.playing
			iconSource: "../resources/components/primaryFlightDisplay/stop.svg"
			tooltip: "Stop"
			onTriggered: player.stopped = true
		}

		ToolBar 
		{
			z:3
			anchors
			{
				bottom: parent.bottom
			}
			RowLayout 
			{
				z:3
				ToolButton { action: play }
				ToolButton { action: stop }
				ToolButton { action: pause }
			}
		}
 
		MouseArea
		{
			anchors.fill: parent
			
			onClicked:
			{
			}
		}
	}
	
	MouseArea
	{
		anchors.fill: parent
		
		onClicked:
		{
			root.popupVisible = false
		}
	}
}

