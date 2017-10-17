import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import PectusViewer 1.1
import PectusProcessor 1.1
import QtQuick.Dialogs 1.2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick.Scene3D 2.0
import Qt.labs.settings 1.0


ApplicationWindow {
    id: root
    visible: true
    width: 1000
    height: 600
    title: qsTr("PectIndex")

    menuBar: MenuBar {
            Menu {
                title: "File"
                MenuItem {
                    text: "Open..."
                    onTriggered: fileDialog.open()
                }
                MenuItem {
                    text: "Close"
                    onTriggered: Qt.quit()
                }
            }

            Menu {
                title: "Help"
                MenuItem {
                    text: "Tutorial"
                    onTriggered: tutorial.open()
                }
                MenuItem { text: "Project ReadME" }
            }
            Menu {
                title: "Preferences"
                MenuItem {
                    text: "Settings"
                    onTriggered: settingsDialog.open()
                }
            }
    }

    Dialog {
        id: settingsDialog
        visible: false
        title: "Settings"

        contentItem: Rectangle {
            implicitWidth: 500
            implicitHeight: 150
            color: "lightGray"

            Row {
                id: modelColorRow
                spacing: 5
                anchors.top: parent.top
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20

                Text {
                    text: qsTr("Model Color:")
                }

                Button{
                    width: 50
                    height: 20
                    text: "Green"
                    onClicked: {
                        settings.modelColor = "green"
                    }
                }

                Button{
                    width: 50
                    height: 20
                    text: "Blue"
                    onClicked: {
                        settings.modelColor = "blue"
                    }
                }

                Button{
                    width: 50
                    height: 20
                    text: "Red"
                    onClicked: {
                        settings.modelColor = "red"
                    }
                }

                Button{
                    width: 50
                    height: 20
                    text: "Teal"
                    onClicked: {
                        settings.modelColor = "teal"
                    }
                }

                Button{
                    width: 50
                    height: 20
                    text: "Yellow"
                    onClicked: {
                        settings.modelColor = "yellow"
                    }
                }

                Button{
                    width: 50
                    height: 20
                    text: "Pink"
                    onClicked: {
                        settings.modelColor = "pink"
                    }
                }

                Button{
                    width: 50
                    height: 20
                    text: "Gray"
                    onClicked: {
                        settings.modelColor = "gray"
                    }
                }
            }

            Row {
                id: drawingColorRow
                spacing: 5
                anchors.top: modelColorRow.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20

                Text {
                    text: qsTr("Canvas Drawing Color:")
                }

                Button {
                    width: 50
                    height: 20
                    text: "Black"
                    onClicked: {
                        sliceCanvas.sliceColor = "black"
                    }
                }
                Button {
                    width: 50
                    height: 20
                    text: "Blue"
                    onClicked: {
                        sliceCanvas.sliceColor = "blue"
                    }
                }
                Button {
                    width: 50
                    height: 20
                    text: "Red"
                    onClicked: {
                        sliceCanvas.sliceColor = "red"
                    }
                }
                Button {
                    width: 50
                    height: 20
                    text: "Green"
                    onClicked: {
                        sliceCanvas.sliceColor = "darkGreen"
                    }
                }
                Button {
                    width: 50
                    height: 20
                    text: "Purple"
                    onClicked: {
                        sliceCanvas.sliceColor = "purple"
                    }
                }

            }

            Button{
                id: closeSettings
                width: 50
                height: 20
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 20
                text: "Close"
                onClicked: {
                    settingsDialog.close()
                }
            }
        }
    }

    Dialog {
        id: tutorial
        visible: false
        title: "Tutorial"
        contentItem: Rectangle {
            id: tutorialContainer
            implicitWidth: 800
            implicitHeight: 500
            color: "lightGray"

            Rectangle {
                height: parent.height - 50
                width: parent.width
                color: "lightGrey"
                Text {
                    id: tutorialContent
                    width: parent.width
                    anchors.top: parent.top
                    wrapMode: Text.WordWrap
                    text: "<h2>PectIndex Tutorial</h2><h4>Opening a File</h4>" +
                          "<p>When opening a file in PectIndex you can either go the the menu and " +
                          "select File>Open or you can select the [...] button on the right side of the screen." +
                          "  Once either option is completed, a dialog box will appear asking you to select a file" +
                          "  (This application only supports '.obj' files).  Once a file is selected, click the 'Open' " +
                          "button on the dialog box and then click the 'Render 3D Scan' button which is under the file name.</p>" +
                          "<h4>3D Viewer</h4><p>Once the 3D image is rendered, it will appear in the 3D Viewer on the left of " +
                          "screen.  If the mouse is over the 3D Viewing area you will be able to click to change the position " +
                          "of the model or you can scroll to zoom in and zoom out.  Furthermore, there are sliders on the right " +
                          "of the 3D Viewer that contol rotation in the X, Y, and Z axes and zoom.  If you would like to change " +
                          "the color of the 3D model, this can be done in the settings page.</p><h4>Canvas</h4><p>The canvas is " +
                          "located in the bottom right of the application, and can be used to view the slices of the 3D model." +
                          "  To view a slice use the Slice slider found above the Canvas Area and then click Plot Lines.  " +
                          "To draw on the Canvas area, click the Pen button found above it, and then left-click to draw.  " +
                          "To part of the slice or any lines that are drawn, you can click the eraser button and left-click "+
                          "to erase any of the lines.  To clear all of the drawn lines on the canvas, click the clear button.  "+
                          "You can change the color of the pen in Settings.</p><h4>Taking Notes</h4><p> To take notes, click the " +
                          "Notes button on the top right of the application.  Left-click the text area that appears after the button " +
                          "is clicked, and begin typing.  Click the notes button to disable the text area."
                }
            }

            Button{
                id: closeTutorial
                width: 50
                height: 20
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottomMargin: 20
                text: "Ok"
                onClicked: {
                    tutorial.close()
                }
            }
        }
    }

    Settings {
        id: settings
        property color modelColor: Qt.rgba(0.6, 0.6, 0.6, 1.0)
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "lightgrey"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (myViewer.notesActive){
                    myViewer.toggleNotesActive();
                }
            }
        }

        Rectangle {
            id: viewerContainer
            width: (parent.width / 2) - 40
            height: parent.height - 50
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 30

            color: parent.color
            border.color: "black"
            border.width: 1

            Text {
                id: placeholder
                width: parent.width / 3
                height: 30
                text: myViewer.scanFileName
                anchors.centerIn: parent
                layer.enabled: true
            }

            Scene3D {
                id: scene3d
                anchors.fill: parent
                aspects: ["input", "logic"]
                ChestModel {
                    scanSource: myViewer.renderStatus ? myViewer.scanFileName : ""
                    modelColor: settings.modelColor

                }
            }
        }

        Text {
            text: qsTr("3D Scan Viewer:")
            anchors.bottom: viewerContainer.top
            anchors.bottomMargin: 5
            anchors.left: viewerContainer.left
        }

        Rectangle {
            id: controlsContainer
            height: viewerContainer.height / 2.3
            width: viewerContainer.width / 2
            anchors.left: viewerContainer.right
            anchors.top: viewerContainer.top
            anchors.leftMargin: 10
            color: parent.color

            Text {
                id:xLabel
                text: qsTr("Rotate X:")
                anchors.top: controlsContainer.top
            }

            Slider {
                id: rotateSliderX
                visible: true
                width: parent.width
                anchors.top: xLabel.bottom
            }

            Text {
                id: yLabel
                text: qsTr("Rotate Y:")
                anchors.top: rotateSliderX.bottom
            }

            Slider {
                id: rotateSliderY
                visible: true
                width: parent.width
                anchors.top: yLabel.bottom
            }

            Text {
                id: zLabel
                text: qsTr("Rotate Z:")
                anchors.top: rotateSliderY.bottom
            }

            Slider {
                id: rotateSliderZ
                visible: true
                width: parent.width
                anchors.top: zLabel.bottom
            }

            Text {
                id: zoomLabel
                text: qsTr("Zoom:")
                anchors.top: rotateSliderZ.bottom
            }

            Slider {
                id: sliderZoom
                visible: true
                width: parent.width
                anchors.top: zoomLabel.bottom
                minimumValue: 0
                maximumValue: 10
                value: 8
            }

            Text {
                id: sliceLabel
                text: qsTr("Slice:")
                anchors.top: sliderZoom.bottom
                anchors.left: parent.left
            }

            Slider {
                id: sliceSlider
                width: parent.width
                anchors.top: sliceLabel.bottom
                minimumValue: 0
                maximumValue: 1
                onValueChanged: {

                }
            }

//            Text {
//                id: sliceRangeAmount
//                text: qsTr("Range Amount:")
//                anchors.top: sliceSlider.bottom
//                anchors.left: parent.left
//            }

//            Slider {
//                id: sliceRangeAmountSlider
//                width: parent.width
//                anchors.top: sliceRangeAmount.bottom
//                minimumValue: 0
//                maximumValue: 1
//            }

        }

        Rectangle {
            id: dialogHolder
            width: parent.width / 5
            height: 20
            anchors.left: controlsContainer.right
            anchors.top: controlsContainer.top
            anchors.topMargin: 30
            anchors.leftMargin: 30
            color: "white"
            border.color: "black"
            border.width: 1

            TextInput {
                id: fileNameInput
                width: parent.width - 10
                height: parent.height
                text: myViewer.scanFileName
                onAccepted: {
                    myViewer.setScanFileName(this.text);
                }
                selectByMouse: true
                layer.enabled: true
            }

            Text {
                text: qsTr("Select File for 3D Rendering:")
                anchors.bottom: fileNameInput.top
                anchors.left: fileNameInput.left
                anchors.bottomMargin: 3
            }

            Button {
                id: renderScanButton
                anchors.top: dialogHolder.bottom
                anchors.topMargin: 10
                anchors.horizontalCenter: dialogHolder.horizontalCenter
                width: 130
                height: 20
                text: "Render 3D Scan"
                onClicked: {
                    myViewer.renderScan();
                    myProcessor.setFileName(fileDialog.fileUrl);
                }
            }

            FileDialog {
                id: fileDialog
                title: "Choose a 3D Model File"
                folder: shortcuts.home
                nameFilters: ["3D Scan Files (*.obj)"]
                onAccepted: {
                    myViewer.setScanFileName(fileDialog.fileUrl);
                    this.close();
                }
                onRejected: {
                    this.close();
                }
            }

            Button {
                id: searchFiles
                width: 30
                height: 20
                anchors.left: fileNameInput.right
                anchors.leftMargin: 20
                text: "..."
                onClicked: {
                    fileDialog.open();
                }
            }
        }

        Rectangle {
            id: sliceRect
            anchors.top: controlsContainer.bottom
            anchors.left: viewerContainer.right
            anchors.leftMargin: 20
            anchors.right: parent.right
            anchors.rightMargin: 20

            height: 300

            Row {
                id: sliceButtonRow
                anchors.bottom: sliceCanvas.top
                anchors.bottomMargin: 5
                spacing: 3
                Button {
                    id: pen
                    text: "Pen"
                    onClicked: {
                        sliceCanvas.mode = 0
                    }
                }
                Button {
                    id: eraser
                    text: "Eraser"
                    onClicked: {
                        sliceCanvas.mode = 1
                    }
                }

                Button {
                   id: clear
                   text: "Clear"
                   onClicked: {
                        sliceCanvas.clear()
                   }
                }
                Button {
                   id: plotLine
                   text: "Plot Lines"
                   onClicked: {
                        myProcessor.calculateIntersection(myProcessor.getMinY() + sliceSlider.value*(myProcessor.getMaxY() - myProcessor.getMinY()))
                        myProcessor.drawLineSegments()
                   }
                }
            }

            SliceCanvas {
                id: sliceCanvas
                objectName: "canvas"
                anchors.fill: parent
            }
        }

        NotesTab {
            x: background.width - 80
            y: 0
        }
    }
}
