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

    QPrinter pdf;
    pdf.setOutputFormat(QPrinter::PdfFormat);
    pdf.setOutputFileName(QDir::homePath() + "/test.pdf");

    QPainter painter;
    QQuickWindow* window = qobject_cast<QQuickWindow*>(rootQmlObject);

    if (window) {
        painter.begin(&pdf);
        QFont font;
        font.setPointSize(16);
        painter.setFont(font);

        QString fileName = processor->getFileName();
        fileName = fileName.split('/').back();
        painter.drawText(40, 65, "Summary for: " + fileName);

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
    QRectF rect(40, 80, 500, 212);
    painter.drawText(rect, viewer->getNotes());
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
        double y = scan->y() * ratio;
        double width = scan->width() * ratio;
        double height = scan->height() * ratio;

        qDebug() << x << y << width << height;
        image = image.copy(x, y, width, height);
        image = image.scaled(475, 570, Qt::AspectRatioMode::KeepAspectRatio);
//        image.save(QDir::homePath() + "/scanViewer.png");

//        qDebug() << image.width() << image.height();

        QPointF point(40, 300);
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
        double y = canvas->y() * ratio;
        double width = canvas->width() * ratio;
        double height = canvas->height() * ratio;

        image = image.copy(x, y, width, height);
        image = image.scaled(480, 300, Qt::AspectRatioMode::KeepAspectRatio);
//        image.save(QDir::homePath() + "/sliceCanvas.png");

//        qDebug() << image.width() << image.height();

        QPointF point(300, 375);
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
        painter.drawText(40, 650, "No Data Generated");
    }
    else if (lastOperation == PectusProcessor::OperationType::SliceMode){
        painter.drawText(40, 650, "Slice Index Data:");
        font.setPointSize(12);
        painter.setFont(font);
        painter.drawText(40, 670, "Haller Index: \t\t" + QString::number(processor->getHallerIndex()));
        painter.drawText(40, 685, "Asymmetric index: \t" + QString::number(processor->getAsymmetricIndexValue()));
        painter.drawText(40, 700, "Defect Volume Index: \t" + QString::number(processor->getVolumeDefectIndexValue()));
    }
    else {
        // TODO: Update with chosen values from Bounds mode calculation
        painter.drawText(40, 650, "Bounds Index Data:");
        font.setPointSize(12);
        painter.setFont(font);
        painter.drawText(40, 670, "Worst Case Haller Index: \t\t" + QString::number(processor->getHallerIndex()));
        painter.drawText(40, 685, "Worst Case Asymmetric index: \t" + QString::number(processor->getAsymmetricIndexValue()));
        painter.drawText(40, 700, "Worst Case Defect Volume Index: \t" + QString::number(processor->getVolumeDefectIndexValue()));
        painter.drawText(40, 715, "Average Haller Index: \t\t" + QString::number(processor->getHallerIndex()));
        painter.drawText(40, 730, "Average Asymmetric index: \t\t" + QString::number(processor->getAsymmetricIndexValue()));
        painter.drawText(40, 745, "Average Defect Volume Index: \t" + QString::number(processor->getVolumeDefectIndexValue()));
    }
}


PectusPDF::PectusPDF(PectusViewer * viewer_input, PectusProcessor * processor_input, QObject *parent) : viewer(viewer_input), processor(processor_input), QObject(parent)
{

}

void PectusPDF::setRootQmlObject(QObject *obj)
{
    rootQmlObject = obj;
}

