import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import PectusViewer 1.1
import QtQuick.Dialogs 1.2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick.Scene3D 2.0
import Qt.labs.settings 1.0


ApplicationWindow {
    visible: true
    width: 1000
    height: 600
    title: qsTr("PectIndex")

    menuBar: MenuBar {
            Menu {
                title: "File"
                MenuItem { text: "Open..." }
                MenuItem { text: "Close" }
            }

            Menu {
                title: "Edit"
                MenuItem { text: "Cut" }
                MenuItem { text: "Copy" }
                MenuItem { text: "Paste" }
            }

            Menu {
                title: "Help"
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
            implicitWidth: 400
            implicitHeight: 100
            Button{
                id: greenModelButton
                width: 40
                height: 20
                text: "GREEN"
                anchors.leftMargin: 10
                onClicked: {
                    settings.modelColor = Qt.rgba(0.0, 0.6, 0.0, 1.0)
                }
            }
            Button{
                id: blueModelButton
                width: 40
                height: 20
                anchors.left: greenModelButton.right
                anchors.leftMargin: 10
                text: "BLUE"
                onClicked: {
                    settings.modelColor = Qt.rgba(0.0, 0.0, 0.6, 1.0)
                }
            }
            Button{
                id: redModelButton
                width: 40
                height: 20
                anchors.left: blueModelButton.right
                anchors.leftMargin: 10
                text: "RED"
                onClicked: {
                    settings.modelColor = Qt.rgba(0.6, 0.0, 0.0, 1.0)
                }
            }
                Button{
                    id: grayModelButton
                    width: 40
                    height: 20
                    anchors.left: redModelButton.right
                    anchors.leftMargin: 10
                    text: "GRAY"
                    onClicked: {
                        settings.modelColor = Qt.rgba(0.6, 0.6, 0.6, 1.0)
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

            Slider {
                id: rotateSlider
                visible: true
                anchors.top: scene3d.bottom
                width: scene3d.width
            }
        }

        Text {
            text: qsTr("3D Scan Viewer:")
            anchors.bottom: viewerContainer.top
            anchors.bottomMargin: 5
            anchors.left: viewerContainer.left
        }

        Button {
            id: searchFiles
            width: 30
            height: 20
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            text: "..."
            onClicked: {
                fileDialog.open();
            }
        }

        Rectangle {
            id: dialogHolder
            width: parent.width / 4
            height: 20
            anchors.right: searchFiles.left
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            color: "white"
            border.color: "black"
            border.width: 1

            TextInput {
                id: fileNameInput
                anchors.centerIn: parent
                width: parent.width - 10
                height: parent.height
                text: myViewer.scanFileName
                onAccepted: {
                    myViewer.setScanFileName(this.text)
                }
                selectByMouse: true
                layer.enabled: true
            }
        }

        Text {
            text: qsTr("Select File for 3D Rendering:")
            anchors.bottom: dialogHolder.top
            anchors.left: dialogHolder.left
            anchors.bottomMargin: 3
        }

        Button {
            anchors.top: dialogHolder.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: dialogHolder.horizontalCenter
            width: 130
            height: 20
            text: "Render 3D Scan"
            onClicked: {
                myViewer.renderScan();
            }
        }

        FileDialog {
            id: fileDialog
            title: "Choose a 3D Model File"
            folder: shortcuts.home
            nameFilters: ["3D Scan Files (*.obj)"]
            onAccepted: {
                myViewer.setScanFileName(fileDialog.fileUrl)
                this.close();
            }
            onRejected: {
                this.close();
            }
        }

        NotesTab {
            x: background.width - 80
            y: 0
        }
    }
}
