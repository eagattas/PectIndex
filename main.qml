import QtQuick 2.6
import QtQuick.Window 2.3 as QW
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0
import QtQuick.Scene3D 2.0

QW.Window {
    visible: true
    width: 700
    height: 700
    title: qsTr("Hello World")

    Rectangle {
        id: scene
        width: 500
        height: 500

        Scene3D {
            id: scene3d
            anchors.fill: parent
            aspects: ["input", "logic"]
            ChestModel {}

        }
    }
}
