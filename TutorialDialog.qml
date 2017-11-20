import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import PectusViewer 1.1

Rectangle {
    id: root
    signal closePressed

    implicitWidth: 800
    implicitHeight: 500
    color: "lightGray"

    Rectangle {
        height: parent.height - 10
        width: parent.width - 10
        anchors.centerIn: parent
        color: "lightGrey"
        Text {
            id: tutorialContent
            width: parent.width
            anchors.top: parent.top
            wrapMode: Text.WordWrap
            text: "<h3>PectIndex Tutorial</h3><p><b>Opening a File: </b>" +
                  "To open a file, either click File > Open or the [...] button on the right side of the screen. " +
                  "Then select an .obj file and click the 'Render 3D Model' button below the file name.</p></br>" +
                  "<p><b>3D Viewer: </b>The 3D viewer can be found on the left half of the window. " +
                  "The name of the file can be found above the viewer, as well as the three different modes the 3D viewer can be in. " +
                  "If you would like to change the model color this can be done in the settings page of this application. " +
                  "<b>Camera Mode: </b>In this mode, you can left-click and drag to rotate the model " +
                  "right-click and drag to pan the model, and scroll to zoom in and out of the model. <b>Slice Mode: </b>" +
                  "In this mode, left-click on the model to select a slice, and the slice will appear in the Canvas area on the right side of the screen. " +
                  "<b>Bounds Mode:</b> This mode is similar to slice mode, except you will pick two points on the model. " +
                  "The application will automatically calculate all of the indexes on all of the slices between the range, as well as a 3D volume defect index.</p>" +
                  "<br><p><b>Canvas:</b> The canvas is located in the bottom right of the application, and is used to view slices after the 3D model is " +
                  "clicked in slice mode. There are two checkboxes above the Canvas Automatic Arm Removal and Run All Indexes. " +
                  "The <b>Automatic Arm Removal</b> checkbox will attempt to remove arms that are connected from the body. Arms that are disconnected from the body are already removed. " +
                  "The <b>Run All Indexes</b> checkbox will automatically run all of the indexes when the slice is selected. " +
                  "There are also 5 buttons above the canvas area. The first three buttons are <b>Haller Index</b>, <b>Asymmetric Index</b>, and <b>Volume Defect Index</b>. " +
                  "When either of these buttons are pressed, the corresponding index will be computed on the slice that is selected.  If a slice is not selected, " +
                  "these buttons will not do anything. The next two buttons are <b>Clear</b> and <b>Reset</b>.  <b>Clear</b> will remove any drawing from the Canvas, " +
                  "and <b>Reset</b> will reset a slice to what it was before any arm removal occurs. <b>Canvas Modes:</b> The Canvas has 3 modes " +
                  "<b>Pen</b>, <b>Eraser</b>, and <b>Arm Removal</b>. When the canvas is in <b>Pen Mode</b> , it allows you to left-click and drag on the canvas to draw a line. " +
                  "When the canvas is in <b>Eraser Mode</b>, you can left-click and drag to erase any line on the Canvas. Finally, if the Canvas is in <b>Arm Removal Mode</b>, " +
                  "you can draw a line through a connected arm to remove it. Lastly, if you would like to change the color of the pen, you can choose a color in the Settings page.</p><br>" +
                  "<p><b>Taking Notes:</b> To take notes, click the Notes button on the top right of the application. Left-click the text area that appears after the button is clicked, and begin typing. Click the notes button to disable the text area.</p>"

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
            root.closePressed();
        }
    }
}
