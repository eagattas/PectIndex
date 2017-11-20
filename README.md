# PectIndex
EECS 498-010 Fall 2017 Senior Design Project

Team Members:
	Evan Agattas
	Evan Brunt
	Dillon Kesto
	Trevor Rees

PectIndex is a desktop application for doctors to analyze scans of patients with Pectus Excavatum deformities. It allows the doctor to upload a scan of a patient, render the image, and select a slice or range of slices to analyze for different medical indexes. The application is designed for Windows 10 only.

## Alpha Release Screenshot
Adi requested that we add a screenshot of our UI. A screenshot of our Alpha Release is Images/PectIndex\_Screenshot\_0.png
![PectIndex Screenshot](https://github.com/eagattas/PectIndex/blob/master/Images/PectIndex_Screenshot_0.png)
Format: ![Alt Text](url)

## Running the Program
Executable (Windows):

First, download the executable from [this link (umich email required).](https://drive.google.com/drive/folders/0BwjIAH_fBQ0Rb283cjJ5ZmFjM1U?usp=sharing)

Download the entire folder pectindex (right-click->download), then simply extract the zip file to any folder, and run the exectuable inside.  The excutable can be found in the bin folder and the scans to upload to our application can be found the the scans folder.

From Source (Mac and Windows):

To run our program from the source, first you must download Qt from [this link.](https://www1.qt.io/download-open-source/?hsCtaTracking=f977210e-de67-475f-a32b-65cec207fd03%7Cd62710cd-e1db-46aa-8d4d-2f1c1ffdacea)

Afterwards, clone this repo at https://github.com/eagattas/PectIndex.git

Lastly, load the project in Qt (using pectindex.pro), build and run it.


## Tutorial:

The same tutorial can be found inside the program, but is also pasted here for your convenience

##### Opening a File:
To open a file, either click File > Open or the [...] button on the right side of the screen. Then select an .obj file and click the <b>Render 3D Model</b> button below the file name.
                
##### 3D Viewer: 
The 3D viewer can be found on the left half of the window. The name of the file can be found above the viewer, as well as the three different modes the 3D viewer can be in. If you would like to change the model color this can be done in the settings page of this application. 
**Camera Mode:** In this mode, you can left-click and drag to rotate the model right-click and drag to pan the model, and scroll to zoom in and out of the model. 
**Slice Mode:** In this mode, left-click on the model to select a slice, and the slice will appear in the Canvas area on the right side of the screen. 
**Bounds Mode:** This mode is similar to slice mode, except you will pick two points on the model. The application will automatically calculate all of the indexes on all of the slices between the range, as well as a 3D volume defect index.

##### Canvas: 
The canvas is located in the bottom right of the application, and is used to view slices after the 3D model is clicked in slice mode. There are two checkboxes above the Canvas Automatic Arm Removal and Run All Indexes. The **Automatic Arm Removal** checkbox will attempt to remove arms that are connected from the body. Arms that are disconnected from the body are already removed. The **Run All Indexes** checkbox will automatically run all of the indexes when the slice is selected. There are also 5 buttons above the canvas area. The first three buttons are **Haller Index**, **Asymmetric Index**, and **Volume Defect Index**. When either of these buttons are pressed, the corresponding index will be computed on the slice that is selected.  If a slice is not selected, these buttons will not do anything. The next two buttons are **Clear** and **Reset**.  **Clear** will remove any drawing from the Canvas and **Reset** will reset a slice to what it was before any arm removal occurs. 
**Modes:** The Canvas has 3 modes **Pen**, **Eraser**, and **Arm Removal**. **Pen Mode:** allows you to left-click and drag on the canvas to draw a line. **Eraser Mode:** you can left-click and drag to erase any line on the Canvas. **Arm Removal Mode:** you can draw a line through a connected arm to remove it. Lastly, if you would like to change the color of the pen, you can choose a color in the Settings page.

##### Taking Notes

To take notes, click the Notes button on the top right of the application. Left-click the text area that appears after the button is clicked, and begin typing. Click the notes button to disable the text area.
