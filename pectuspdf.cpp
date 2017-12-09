#include "pectuspdf.h"
#include <QDebug>
#include <QPainter>
#include <QPrinter>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QWidget>
#include <QGuiApplication>

void PectusPDF::createPDF()
{
    qDebug() << "Creating pdf";

    QPrinter pdf(QPrinter::HighResolution);
    pdf.setOutputFormat(QPrinter::PdfFormat);

    QString fileName = processor->getFileName();
    fileName = fileName.split('/').back();

    if (fileName.isEmpty()){
        fileName = "No_Scan_Loaded";
    }

    pdf.setOutputFileName(QDir::homePath() + "/Downloads/" + fileName.split(".")[0] + ".pdf");

    QPainter painter;
    QQuickWindow* window = qobject_cast<QQuickWindow*>(rootQmlObject);

    if (window) {
        painter.begin(&pdf);
        QFont font;
        font.setPointSize(16);
        painter.setFont(font);

        painter.drawText(200, 300, "Summary for: " + fileName);

        font.setBold(false);
        font.setPointSize(12);
        painter.setFont(font);

        addNotes(painter);
        addScanViewer(painter, window);
        addSlice(painter, window);
        addSummary(painter);

        painter.end();
    }

//    qDebug() << window->devicePixelRatio();
}

// Adds the user's notes to the PDF
void PectusPDF::addNotes(QPainter & painter){
//    qDebug() << viewer->getNotes();
    QString notes = viewer->getNotes();
    if (notes.size() > 5000){
        QString newString = "";
        for (int i = 0; i < 999; i++){
            newString.push_back(notes[i]);
        }
        notes = newString;
    }
    //QRectF rect(200, 500, 9300, 5000);

    QFont font;
    font.setPointSize(16);
    painter.setFont(font);

    painter.drawText(200, 7200, "Notes:");

    font.setBold(false);
    font.setPointSize(12);
    painter.setFont(font);

    QRectF rect(200, 7500, 9300, 4500);
    painter.drawText(rect, notes);
}

// Adds the contents of the 3D Scan Viewer to the PDF
void PectusPDF::addScanViewer(QPainter & painter, QQuickWindow * window){

    QObject * scanObject = rootQmlObject->findChild<QObject*>("viewerContainer");
    QQuickItem * scan = qobject_cast<QQuickItem *>(scanObject);

    if (scan) {
        double ratio = window->devicePixelRatio();
        QImage image;
        image = window->grabWindow();

        double x = scan->x() * ratio;
#if defined(Q_OS_WIN)
        QObject * scene3dControlsObject = rootQmlObject->findChild<QObject*>("scene3dControls");
        QQuickItem * scene3dControls = qobject_cast<QQuickItem *>(scene3dControlsObject);
        double y = (scan->y() + scene3dControls->height()) * ratio;
#else
        double y = scan->y() * ratio;
#endif
        double width = scan->width() * ratio;
        double height = scan->height() * ratio;

        qDebug() << x << y << width << height;
        image = image.copy(x, y, width, height);
        //image = image.scaled(475, 570, Qt::AspectRatioMode::KeepAspectRatio);
        image = image.scaled(4500 * ratio, 4500 * ratio, Qt::AspectRatioMode::KeepAspectRatio);

//        image.save(QDir::homePath() + "/scanViewer.png");

//        qDebug() << image.width() << image.height();

        //QPointF point(40, 6000);

        QPointF point(200, 1700);
        painter.drawImage(point, image);
    }
}

// Adds the contents of the Slice Canvas to the PDF
void PectusPDF::addSlice(QPainter & painter, QQuickWindow * window){
    QObject * canvasObject = rootQmlObject->findChild<QObject*>("sliceRect");
    QQuickItem * canvas = qobject_cast<QQuickItem *>(canvasObject);

    if (canvas) {
        double ratio = window->devicePixelRatio();
        QImage image;
        image = window->grabWindow();


        double x = canvas->x() * ratio;
#if defined(Q_OS_WIN)
        QObject * sliceButtonRowObject = canvasObject->findChild<QObject*>("sliceButtonRow");
        QQuickItem * sliceButtonRow = qobject_cast<QQuickItem *>(sliceButtonRowObject);
        double y = (canvas->y() + sliceButtonRow->height()) * ratio;
#else
        double y = canvas->y() * ratio;
#endif
        double width = canvas->width() * ratio;
        double height = canvas->height() * ratio;

        image = image.copy(x, y, width, height);
        //image = image.scaled(480, 300, Qt::AspectRatioMode::KeepAspectRatio);
        image = image.scaled(4500 * ratio, 4500 * ratio, Qt::AspectRatioMode::KeepAspectRatio);

//        image.save(QDir::homePath() + "/sliceCanvas.png");

//        qDebug() << image.width() << image.height();

        //QPointF point(4600, 6000);
        QPointF point(4600, 1700);
        painter.drawImage(point, image);
    }
}

// Adds a summary detailing the index calculations to the PDF
void PectusPDF::addSummary(QPainter & painter){
    QFont font;
    font.setPointSize(16);
    painter.setFont(font);

    PectusProcessor::OperationType lastOperation = processor->getLastOperation();
    qDebug() << lastOperation;
    // If the program has not performed an operation on this scan, there is no data to print.
    if (lastOperation == PectusProcessor::OperationType::NoOperation){
        painter.drawText(200, 700, "No Data Generated");
    }
    else if (lastOperation == PectusProcessor::OperationType::SliceMode){
        painter.drawText(200, 700, "Slice Index Data:");
        font.setPointSize(12);
        painter.setFont(font);
        painter.drawText(200, 950, "Haller Index: \t" + QString::number(processor->getHallerIndex()));
        painter.drawText(200, 1200, "Asymmetric index: \t" + QString::number(processor->getAsymmetricIndexValue()));
        painter.drawText(200, 1450, "Defect Volume Index: \t" + QString::number(processor->getVolumeDefectIndexValue()));
    }
    else {
        // TODO: Update with chosen values from Bounds mode calculation
        painter.drawText(200, 700, "Bounds Index Data:");
        font.setPointSize(12);
        painter.setFont(font);
        painter.drawText(200, 950, "Worst Case Haller Index: \t\t" + QString::number(processor->getHallerIndex()));
        painter.drawText(200, 1200, "Worst Case Asymmetric index: \t" + QString::number(processor->getAsymmetricIndexValue()));
        painter.drawText(200, 1450, "Average Defect Volume Index: \t" + QString::number(processor->getVolumeDefectIndexValue()));
    }
}


PectusPDF::PectusPDF(PectusViewer * viewer_input, PectusProcessor * processor_input, QObject *parent) :  QObject(parent), viewer(viewer_input), processor(processor_input)
{

}

void PectusPDF::setRootQmlObject(QObject *obj)
{
    rootQmlObject = obj;
}

