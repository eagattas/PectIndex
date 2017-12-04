#include "pectuspdf.h"
#include <QDebug>
#include <QPainter>
#include <QPrinter>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QWidget>

void PectusPDF::createPDF(int width, int height)
{

    qDebug() << "Creating pdf";

    QPrinter pdf;
    pdf.setOutputFormat(QPrinter::PdfFormat);
    pdf.setOutputFileName(QDir::currentPath() + "/test.pdf");

    QPainter painter;

    painter.begin(&pdf);
    painter.drawText(50, 50, "Hello PDF");

    //QImage imageData(imageData.data(), width, height, QImage::Format::Format_RGB32);

    //imageData.save("C:/Users/dillo/Desktop/test.png");



    QSharedPointer<QQuickItemGrabResult> r = canvasQItem->grabToImage();

    //QObject::connect(canvasQItem->grabToImage().data(), &QQuickItemGrabResult::ready,
      //               this, &PectusPDF::finished2Dimage);

    painter.end();

}

void PectusPDF::fillImageData(uchar d)
{
    imageData.push_back(255 - d);
}

void PectusPDF::finished2Dimage()
{
    qDebug() << "done grabbing";
    grabbedResult->saveToFile("C:/Users/dillo/Desktop/test2.png");
}

PectusPDF::PectusPDF(QObject *parent) : QObject(parent)
{
    QObject *canvas = rootQmlObject->findChild<QObject*>("canvas");
    canvasQItem = qobject_cast<QQuickItem*>(canvas);



}

void PectusPDF::setRootQmlObject(QObject *obj)
{
    rootQmlObject = obj;
}

