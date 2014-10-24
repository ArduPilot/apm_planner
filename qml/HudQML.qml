/*
    Copyright (C) 2012 Collabora Ltd. <info@collabora.com>
      @author George Kiagiadakis <george.kiagiadakis@collabora.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.1
import QtGStreamer 1.0
import APMPlanner 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.LocalStorage 2.0
import "Storage.js" as Settings

Rectangle {
    id: root
	property bool popupVisible: false

	Component.onCompleted:
	{
		brightnessSlider.value = Settings.get("brightness",0);
		contrastSlider.value = Settings.get("contrast",0);
		hueSlider.value = Settings.get("hue",0);
		saturationSlider.value = Settings.get("saturation",0);
	}	

	VideoItem {
		id: video
		width: root.width
		height: root.height
		surface: videoSurface1 //bound on the context from main()
	}

	PaintedItemDelegate {
		id: hud
		width: root.width
		height: root.height
	}
	
	Menu { 
		id: contextMenu
        
		MenuItem { 
            text: "Video"
			checkable: true
			checked: container.videoEnabled
			onTriggered: container.videoEnabled = !container.videoEnabled
        }
		
		MenuItem {
            text: "FullScreen"
			checkable: true
			checked: container.fullScreenMode
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
	
	Rectangle
    {
		color: "lightgrey"
		border.color: "black"
		width: 10
        height:10
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
