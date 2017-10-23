import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import PectusViewer 1.1

Rectangle {
    id: root
    property color modelColor
    property color canvasColor
    signal closePressed

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
                root.modelColor = "green"
            }
        }

        Button{
            width: 50
            height: 20
            text: "Blue"
            onClicked: {
                root.modelColor = "blue"
            }
        }

        Button{
            width: 50
            height: 20
            text: "Red"
            onClicked: {
                root.modelColor = "red"
            }
        }

        Button{
            width: 50
            height: 20
            text: "Teal"
            onClicked: {
                root.modelColor = "teal"
            }
        }

        Button{
            width: 50
            height: 20
            text: "Pink"
            onClicked: {
                root.modelColor = "pink"
            }
        }

        Button{
            width: 50
            height: 20
            text: "Gray"
            onClicked: {
                root.modelColor = "gray"
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
                root.canvasColor = "black"

            }
        }
        Button {
            width: 50
            height: 20
            text: "Blue"
            onClicked: {
                root.canvasColor = "blue"
            }
        }
        Button {
            width: 50
            height: 20
            text: "Red"
            onClicked: {
                root.canvasColor = "red"
            }
        }
        Button {
            width: 50
            height: 20
            text: "Green"
            onClicked: {
                root.canvasColor = "darkGreen"
            }
        }
        Button {
            width: 50
            height: 20
            text: "Purple"
            onClicked: {
                root.canvasColor = "purple"
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
            root.closePressed();
        }
    }
}
