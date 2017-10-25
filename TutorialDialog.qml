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
        height: parent.height - 50
        width: parent.width - 40
        anchors.centerIn: parent
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
                  "screen.  If the mouse is over the 3D Viewing area you will be able to left-click to change the position " +
                  "of the model or you can scroll to zoom in and zoom out.  Furthermore, there are sliders on the right " +
                  "of the 3D Viewer that contol rotation in the X, Y, and Z axes and zoom.  If you would like to change " +
                  "the color of the 3D model, this can be done in the settings page.</p><h4>Canvas</h4><p>The canvas is " +
                  "located in the bottom right of the application, and can be used to view the slices of the 3D model." +
                  "  To view a slice use the Slice slider found above the Canvas Area and then click Plot Slice.  " +
                  "To draw on the Canvas area, click the Pen button found above it, and then left-click to draw.  " +
                  "To erase part of the slice or any lines that are drawn, you can click the eraser button and left-click "+
                  "to erase.  To clear all of the drawn lines on the canvas, click the clear button.  "+
                  "You can change the color of the pen in Settings.</p><h4>Taking Notes</h4><p> To take notes, click the " +
                  "Notes button on the top right of the application.  Left-click the text area that appears after the button " +
                  "is clicked, and begin typing.  Click the notes button to disable the text area.</p>" +
                  "<h4>Erasing Arms</h4><p>After plotting the slice on the canvas, you may notice that the arms of the model " +
                  "appear. To fix this issue, click on the Erase Arms button to remove them. " +
                  "Currently, this button will only remove arms that are completely disconnected from the torso.</p>" +
                  "<h4>Haller Index</h4><p>To generate the Haller Index for the slice (the width of the chest divided its depth), " +
                  "click on the Haller Index button after you have selected your slice. Please note that this functionality it not " +
                  "complete, and may be inaccurate due to arms."
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
