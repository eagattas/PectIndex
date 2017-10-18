# PectIndex
EECS 498-010 Fall 2017 Senior Design Project

Team Members:
	Evan Agattas
	Evan Brunt
	Dillon Kesto
	Trevor Rees

PectIndex is a desktop application for doctors to analyze scans of patients with Pectus Excavatum deformities. It allows the doctor to upload a scan of a patient, render the image, and select a slice or range of slices to analyze for different medical indexes. The application is designed for Windows 10 only.

## Running the Program

First, download the executable from [this link (umich email required).](https://drive.google.com/drive/folders/0BwjIAH_fBQ0Rb283cjJ5ZmFjM1U?usp=sharing)

Download the entire folder pectindex1 (right-click->download), then simply extract the zip file to any folder, and run the exectuable inside. If you notice that the graphics are not rendering well (can be the case for systems with no dedicated GPU), try downloading pectindex2, which uses a different OpenGL implementation. If you download pectindex2, the application must be in the same directory as the two provided .dll files.

Both pectindex1 and pectindex2 contain a folder with scans that you can use with the program.

From Source

To run our program from the source, first you must download Qt from [this link.](https://www1.qt.io/download-open-source/?hsCtaTracking=f977210e-de67-475f-a32b-65cec207fd03%7Cd62710cd-e1db-46aa-8d4d-2f1c1ffdacea)

Afterwards, clone this repo at https://github.com/eagattas/PectIndex.git

Lastly, load the project in Qt (using pectindex.pro) and run it.


## Tutorial:

The same tutorial can be found inside the program, but is also pasted here for your convenience

###### Opening a File

When opening a file in PectIndex you can either go the the menu and select File>Open or you can select the [...] button on the right side of the screen. Once either option is completed, a dialog box will appear asking you to select a file (This application only supports '.obj' files).  Once a file is selected, click the 'Open' button on the dialog box and then click the 'Render 3D Scan' button which is under the file name.


###### 3D Viewer

Once the 3D image is rendered, it will appear in the 3D Viewer on the left of screen. If the mouse is over the 3D Viewing area you will be able to left-click to change the position of the model or you can scroll to zoom in and zoom out.  Furthermore, there are sliders on the right of the 3D Viewer that contol rotation in the X, Y, and Z axes and zoom.  If you would like to change the color of the 3D model, this can be done in the settings page.

###### Canvas

The canvas is located in the bottom right of the application, and can be used to view the slices of the 3D model. To view a slice use the Slice slider found above the Canvas Area and then click Plot Slice. To draw on the Canvas area, click the Pen button found above it, and then left-click to draw. To erase part of the slice or any lines that are drawn, you can click the eraser button and left-click to erase. To clear all of the drawn lines on the canvas, click the clear button. You can change the color of the pen in Settings.

###### Taking Notes

To take notes, click the Notes button on the top right of the application. Left-click the text area that appears after the button is clicked, and begin typing. Click the notes button to disable the text area.

###### Erasing Arms

After plotting the slice on the canvas, you may notice that the arms of the model appear. To fix this issue, click on the Erase Arms button to remove them. Currently, this button will only remove arms that are completely disconnected from the torso.

###### Haller Index

To generate the Haller Index for the slice (the width of the chest divided its depth), click on the Haller Index button after you have selected your slice. Please note that this functionality it not complete, and may be inaccurate due to arms.