import QtQuick 2.6
import QtQuick.Window 2.2 as QW
import QtQuick.Controls 2.2
import PectusViewer 1.1
import QtQuick.Dialogs 1.2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick.Scene3D 2.0

QW.Window {
    visible: true
    width: 1000
    height: 600
    title: qsTr("PectIndex")

    Rectangle {
        id: background
        anchors.fill: parent
        color: "lightgrey"

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
                }
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
    }
}
