import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import PectusViewer 1.1

Item {
    id: root

    Rectangle {
        id: tab
        width: 80
        height: 30
        anchors.top: root.topAnchor
        anchors.right: root.rightAnchor
        color: myViewer.notesActive ? "#EEEEEE" : "lightGrey"
        border.color: "black"

        Text {
            anchors.centerIn: parent
            text: "Notes"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                myViewer.toggleNotesActive();
            }
        }
    }

    Rectangle {
        id: notesBox
        width: 300
        height: 400
        anchors.top: tab.bottom
        anchors.right: tab.right
        color: "white"
        border.color: "black"
        border.width: 3
        visible: myViewer.notesActive

        TextArea {
            anchors.fill: parent
            text: myViewer.notes

            onEditingFinished: {
                myViewer.updateNotes(this.text)
            }
        }
    }
}
