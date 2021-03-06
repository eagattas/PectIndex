import QtQuick 2.6
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
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
    minimumHeight: 600
    maximumHeight: 600
    minimumWidth: 1000
    maximumWidth: 1000
    title: qsTr("PectIndex")

    property string patientFilename: "No_Scan_Loaded.pdf"

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
                MenuItem {
                    text: "Create PDF"
                    onTriggered: {
                        var filename = myProcessor.fileName.split("/")
                        filename = filename[filename.length - 1].split(".")[0]
                        if (filename !== "") {
                            filename = filename + ".pdf"
                            root.patientFilename = filename
                        }
                        myPDF.createPDF()

                        var d = pdfComponent.createObject(null)
                        d.open()
                    }
                }
            }

            Menu {
                title: "Help"
                MenuItem {
                    text: "Tutorial"
                    onTriggered: {
                        var d = tutorialComponent.createObject(null)
                        d.open()
                    }
                }
            }
            Menu {
                title: "Preferences"
                MenuItem {
                    text: "Settings"
                    onTriggered: {
                        var d = settingsComponent.createObject(null)
                        d.open()
                    }
                }
                MenuItem {
                    text: "Color"
                    onTriggered: {
                        var d = colorComponent.createObject(null)
                        d.open()
                    }
                }
            }
    }

    Component {
        id: colorComponent
        ColorDialog {
            id: colorDialog

            onAccepted: {
                sliceCanvas.sliceColor = color
                colorDialog.destroy()
            }

            onRejected: {
                colorDialog.destroy()
            }

        }
    }

    Component {
        id: settingsComponent
        Dialog {
            id: settingsDialogContainer
            visible: false
            title: "Settings"
            contentItem: SettingsDialog {
                id: settingsDialog
                modelColor: "gray"
                canvasColor: "blue"
                onClosePressed: {
                    settingsDialogContainer.close();
                    settingsDialogContainer.destroy()
                }
                onModelColorChanged: {
                    scene3d.mColor = this.modelColor
                }

                onCanvasColorChanged: {
                    sliceCanvas.sliceColor = this.canvasColor
                }
            }
        }
    }

    Component {
        id: tutorialComponent
        Dialog {
            id: tutorialDialogContainer
            visible: false
            title: "Tutorial"
            contentItem: TutorialDialog {
                id: tutorialContainer
                onClosePressed: {
                    tutorialDialogContainer.close()
                    tutorialDialogContainer.destroy()
                }
            }
        }
    }

    Component {
        id: pdfComponent
        Dialog {
            id: pdfPopUp

            contentItem: Rectangle {

                implicitWidth: 300
                implicitHeight: 75
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
                        text: root.patientFilename + " created successfully in Downloads folder"
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
                        pdfPopUp.close()
                        pdfPopUp.destroy()
                    }
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
                    myViewer.updateNotes(notesTab.notes);
                    myViewer.toggleNotesActive();
                }
            }
        }
        Rectangle {
            id: viewerContainer
            objectName: "viewerContainer"
            width: (parent.width / 2) - 40
            height: parent.height - 50
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 30
            z: 100

            color: "transparent"

            property double previousX: 0.0
            property double previousY: 0.0

            property double xRot: 0.0
            property double yRot: 0.0

            property double xCamera: 0.0
            property double yCamera: 0.0

            MouseArea {
                id: viewerContainerMouseArea
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onMouseXChanged: {
                    if (pressedButtons == Qt.LeftButton) {
                        if (viewerContainer.previousX !== 0) {
                           viewerContainer.xRot += mouseX - viewerContainer.previousX
                        }
                    }
                    else if (pressedButtons == Qt.RightButton) {
                        if (viewerContainer.previousX !== 0) {
                            viewerContainer.xCamera -= mouseX - viewerContainer.previousX
                            //mainCamera.translate(Qt.vector3d(-1 * (mouseX - viewerContainer.previousX) / 750, 0, 0))
                        }
                    }
                    viewerContainer.previousX = mouseX
                }

                onMouseYChanged: {
                    if (pressedButtons == Qt.LeftButton) {
                        if (viewerContainer.previousY !== 0) {
                           viewerContainer.yRot += mouseY - viewerContainer.previousY
                        }
                    }
                    else if (pressedButtons == Qt.RightButton) {
                        if (viewerContainer.previousY !== 0) {
                            viewerContainer.yCamera -= mouseY - viewerContainer.previousY
                            //mainCamera.translate(Qt.vector3d(0, (mouseY - viewerContainer.previousY) / 750, 0), Camera.TranslateViewCenter)
                        }
                    }
                    viewerContainer.previousY = mouseY
                }

                onWheel: {
                    mainCamera.translate(Qt.vector3d(0, 0, wheel.angleDelta.y / 1000), Camera.DontTranslateViewCenter)
                }

                onReleased: {
                    viewerContainer.previousX = 0
                    viewerContainer.previousY = 0
                }
            }



        }

        Scene3D {
            id: scene3d
            width: (parent.width / 2) - 40
            height: parent.height - 50
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.top: parent.top
            anchors.topMargin: 30
            z: 0


            property color mColor: "gray"

            aspects: ["input", "logic"]

            ChestModel {
                mainCamera: Camera {
                    id: mainCamera
                    projectionType: CameraLens.PerspectiveProjection
                    fieldOfView: 22.5
                    position: Qt.vector3d( 0.19195, 0.180172, 4)
                    viewCenter: Qt.vector3d(0, 0, 0)
                }
                id: chestModel
                scanSource: myViewer.renderStatus ? myViewer.scanFileName : ""
                modelColor: scene3d.mColor
            }
        }

        Text {
            id: scanViewerText
            text: qsTr("3D Scan Viewer:")
            anchors.bottom: viewerContainer.top
            anchors.bottomMargin: 5
            anchors.left: viewerContainer.left
        }

        Text {
            id: fileNameText
            text: ""
            anchors.left: scanViewerText.right
            anchors.bottom: viewerContainer.top
            anchors.leftMargin: 5
            anchors.bottomMargin: 5
        }

        Row {
            id: scene3dControls
            objectName: "scene3dControls"
            spacing: 3
            anchors.right: viewerContainer.right
            anchors.bottom: viewerContainer.top


            Button {
                id: cameraModeButton
                text: "Camera Mode"
                checked: true
                onClicked: {
                    viewerContainer.z = 100
                    scene3d.z = 0
                    cameraModeButton.checked = true
                    sliceModeButton.checked = false
                    boundsModeButton.checked = false
                    myProcessor.setFirstClickPressed(false)
                    volumeText.text = "Defect Volume Index: "
                }
                style: ButtonStyle {
                       background:
                            Rectangle {
                               color: cameraModeButton.checked ? "#FFFFFF" : "#A9A9A9";
                                radius: 1;
                            }
                   }
                ToolTip.visible: hovered
                ToolTip.delay: 800
                ToolTip.text: qsTr("Left click and drag to rotate image. \nRight click and drag to pan the image. \nScroll to zoom in and out.")
            }
            Button {
                id: sliceModeButton
                text: "Slice Mode"
                checked: false
                onClicked: {
                    viewerContainer.z = 0
                    scene3d.z = 100
                    sliceModeButton.checked = true
                    cameraModeButton.checked = false
                    boundsModeButton.checked = false
                    myProcessor.setFirstClickPressed(false)
                    volumeText.text = "Defect Volume Index: "
                }
                style: ButtonStyle {
                       background:
                            Rectangle {
                               color: sliceModeButton.checked ? "#FFFFFF" : "#A9A9A9";
                                radius: 1;
                            }
                   }
                ToolTip.visible: hovered
                ToolTip.delay: 800
                ToolTip.text: qsTr("Click a spot on the scan. This \nwill create a slice at this point on the \nbody, which will display on the canvas.")
            }
            Button {
                id: boundsModeButton
                text: "Bounds Mode"
                checked: false
                onClicked: {
                    viewerContainer.z = 0
                    scene3d.z = 100
                    cameraModeButton.checked = false
                    sliceModeButton.checked = false
                    boundsModeButton.checked = true
                    volumeText.text = "Average Defect Volume Index: "
                }
                style: ButtonStyle {
                       background:
                            Rectangle {
                               color: boundsModeButton.checked ? "#FFFFFF" : "#A9A9A9";
                                radius: 1;
                            }
                   }
                ToolTip.visible: hovered
                ToolTip.delay: 800
                ToolTip.text: qsTr("Click two spots on the scan. They will \nbe the bounds for a range of slices starting \nat the first click and ending at the second click.\nThe average defect volume index for the range\nwill be computed and the slice with the worst\nhaller index will be displayed.")
            }

        }

        Rectangle {
            id: dialogHolder
            width: parent.width / 5
            height: 20
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.rightMargin: 160
            anchors.topMargin: 50
            color: "white"
            border.color: "black"
            border.width: 1

            TextInput {
                id: fileNameInput
                width: parent.width - 10
                height: parent.height
                text: myViewer.visibleFileName
                onAccepted: {
                    myViewer.setVisibleFileName(this.text)
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
                text: "Render 3D Model"
                onClicked: {
                    var filename = fileDialog.fileUrl.toString().split('/');
                    fileNameText.text = filename[filename.length - 1];
                    myViewer.setScanFileName(myViewer.visibleFileName);
                    myViewer.renderScan();
                    myProcessor.setFileName(fileDialog.fileUrl);
                    myProcessor.setLastOperation(PectusProcessor.NoOperation)
                }
                ToolTip.visible: hovered
                ToolTip.delay: 800
                ToolTip.text: qsTr("Render the .obj file in the 3D scan viewer.")
            }

            Rectangle {
                id: indiceBox
                anchors.top: renderScanButton.bottom
                anchors.topMargin: 20

                Text {
                    id: hallerText
                    text: "Haller Index: "
                    font.pointSize: 10
                    anchors.top: indiceBox.top
                    anchors.topMargin: 10
                }
                Text{
                    id: hallerTextValue
                    visible: myProcessor.hallerIndexVisible
                    anchors.top: hallerText.top
                    anchors.left: hallerText.right
                    text: myProcessor.hallerIndex.toPrecision(4)
                    font.pointSize: 10
                }

                Text {
                    id: asymmetricText
                    text: "Asymmetric Index: "
                    font.pointSize: 10
                    anchors.top: hallerText.bottom
                }
                Text{
                    id: asymmetricTextValue
                    visible: myProcessor.asymmetricIndexVisible
                    anchors.top: asymmetricText.top
                    anchors.left: asymmetricText.right
                    text: myProcessor.asymmetricIndexValue.toPrecision(4)
                    font.pointSize: 10
                }
                Text {
                    id: volumeText
                    text: "Defect Volume Index: "
                    font.pointSize: 10
                    anchors.top: asymmetricText.bottom
                }
                Text{
                    id: volumeTextValue
                    visible: myProcessor.volumeDefectIndexVisible
                    anchors.top: volumeText.top
                    anchors.left: volumeText.right
                    text: myProcessor.volumeDefectIndexValue.toPrecision(4)
                    font.pointSize: 10
                }
            }

            FileDialog {
                id: fileDialog
                title: "Choose a 3D Model File"
                folder: shortcuts.home
                nameFilters: ["3D Scan Files (*.obj)"]

                onAccepted: {
                    myViewer.setVisibleFileName(fileDialog.fileUrl);
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

                ToolTip.visible: hovered
                ToolTip.delay: 800
                ToolTip.text: qsTr("Select a Wavefront .obj file to render in \nthe 3D Scan Viewer.")
            }
        }

        Rectangle {
            id: sliceRect
            objectName: "sliceRect"
            anchors.bottom: parent.bottom
            anchors.left: viewerContainer.right
            anchors.leftMargin: 20
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.bottomMargin: 20

            height: 300
            Row {
                id: checkBoxRow
                anchors.bottom: indexButtonRow.top
                anchors.bottomMargin: 5
                spacing: 10

                CheckBox {
                    id: armRemovalEnabled
                    text: "Automatic Arm Removal"
                    checked: myProcessor.armRemovalEnabled
                    onClicked: {
                        myProcessor.enableArmRemoval(!myProcessor.armRemovalEnabled);
                    }
                    anchors.verticalCenter: parent.verticalCenter
                    ToolTip.visible: hovered
                    ToolTip.delay: 800
                    ToolTip.text: qsTr("Automatically remove connected arms \nwhen a slice is created.")
                }

                CheckBox {
                    id: runAllIndexes
                    text: "Run All Indexes"
                    checked: myProcessor.runAllIndexes
                    onClicked: {
                        myProcessor.setRunAllIndexes(!myProcessor.runAllIndexes);
                    }
                    anchors.verticalCenter: parent.verticalCenter
                    ToolTip.visible: hovered
                    ToolTip.delay: 800
                    ToolTip.text: qsTr("Calculate all indexes when a slice \nis created.")
                }
            }

            Row {
                id: indexButtonRow
                anchors.bottom: sliceButtonRow.top
                anchors.bottomMargin: 5
                spacing: 3
                Button {
                    id: hallerIndex
                    text: "Haller"
                    onClicked: {
                        myProcessor.calculateHallerIndex();
                    }
                    ToolTip.visible: hovered
                    ToolTip.delay: 800
                    ToolTip.text: qsTr("Calculates the Haller Index \n(width / depth).")
                }
//                Button {
//                    id: defectLine
//                    text: "Defect"
//                    onClicked: {
//                        myProcessor.findDefectLine(false);
//                    }
//                    ToolTip.visible: hovered
//                    ToolTip.delay: 800
//                    ToolTip.text: qsTr("Finds the line from the deepest \npoint of the defect to the spine")
//                }
                Button {
                    id: chestArea
                    text: "Asymmetric"
                    onClicked: {
                        myProcessor.asymmetricIndex();
                    }
                    ToolTip.visible: hovered
                    ToolTip.delay: 800
                    ToolTip.text: qsTr("Calculates (Left Side Area) / (Right Side Area)")
                }
                Button {
                    id: volumeDefect
                    text: "Defect Volume"
                    onClicked: {
                        myProcessor.volumeDefectIndex();
                    }
                    ToolTip.visible: hovered
                    ToolTip.delay: 800
                    ToolTip.text: qsTr("Calculates (Defect Area) / (Chest Area + Defect Area)")
                }
                Button {
                   id: clear
                   text: "Clear"
                   onClicked: {
                        sliceCanvas.clear()
                   }

                   ToolTip.visible: hovered
                   ToolTip.delay: 800
                   ToolTip.text: qsTr("Erase everything on the canvas.")
                }
                Button {
                    id: reset
                    text: "Reset"
                    onClicked: {
                        if (myProcessor.fileName == ""){
                            return
                        }
                        var armRemovalEnabled = myProcessor.armRemovalEnabled;
                        var lastYPlane = myProcessor.getLastYPlane();
                        myProcessor.enableArmRemoval(false);
                        myProcessor.calculateIntersection(lastYPlane);
                        myProcessor.drawLineSegments();
                        myProcessor.enableArmRemoval(armRemovalEnabled);
                    }

                    ToolTip.visible: hovered
                    ToolTip.delay: 800
                    ToolTip.text: qsTr("If attached arms were removed \n"
                                   + "on the last slice, draws the same \n"
                                   + "slice without connected arms removed.")
                }
            }

            Row {
                id: sliceButtonRow
                objectName: "sliceButtonRow"
                anchors.bottom: sliceCanvas.top
                anchors.left: sliceCanvas.left
                spacing: 3
                Button {
                    id: pen
                    text: "Pen"
                    checked: true
                    onClicked: {
                        pen.checked = true;
                        eraser.checked = false;
                        armRemover.checked = false;
                    }
                    style: ButtonStyle {
                           background:
                                Rectangle {
                                   color: pen.checked ? "#FFFFFF" : "#A9A9A9";
                                    radius: 1;
                                }
                       }
                    ToolTip.visible: hovered
                    ToolTip.delay: 800
                    ToolTip.text: qsTr("Use a pen on the canvas.")
                }

                Button {
                    id: eraser
                    text: "Eraser"
                    checked: false
                    onClicked: {
                        pen.checked = false;
                        eraser.checked = true;
                        armRemover.checked = false;
                    }
                    style: ButtonStyle {
                           background:
                                Rectangle {
                                   color: eraser.checked ? "#FFFFFF" : "#A9A9A9";
                                    radius: 1;
                                }
                       }
                    ToolTip.visible: hovered
                    ToolTip.delay: 800
                    ToolTip.text: qsTr("Use an eraser on the canvas.")
                }

                Button {
                    id: armRemover
                    text: "Arm Removal"
                    checked: false
                    onClicked: {
                        pen.checked = false;
                        eraser.checked = false;
                        armRemover.checked = true;
                    }
                    style: ButtonStyle {
                           background:
                                Rectangle {
                                   color: armRemover.checked ? "#FFFFFF" : "#A9A9A9";
                                    radius: 1;
                                }
                       }
                    ToolTip.visible: hovered
                    ToolTip.delay: 800
                    ToolTip.text: qsTr("Remove connected arms from the slice.")
                }
            }

            SliceCanvas {
                id: sliceCanvas
                objectName: "canvas"
                anchors.fill: parent
                sliceColor: "blue"
            }

        }

        NotesTab {
            id: notesTab
            x: background.width - 80
            y: 0
        }
    }
}
