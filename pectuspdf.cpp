#include "pectuspdf.h"
#include <QDebug>
#include <QPainter>
#include <QPrinter>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QWidget>
#include <QGuiApplication>

void PectusPDF::createPDF(int width, int height, int x, int y)
{

    qDebug() << "Creating pdf";
    QObject *canvas = rootQmlObject->findChild<QObject*>("canvas");

    QPrinter pdf;
    pdf.setOutputFormat(QPrinter::PdfFormat);
    pdf.setOutputFileName(QDir::currentPath() + "/test.pdf");

    QPainter painter;

    QQuickWindow* window = qobject_cast<QQuickWindow*>(rootQmlObject);
    QImage image;
    if (window) {
        image = window->grabWindow();
        image = image.copy(x, y, width, height);
        //image.save("C:/Users/dillo/Desktop/img.png");
    }

    painter.begin(&pdf);
    painter.drawText(50, 50, "Hello PDF");
    painter.drawImage(0, 250, image);

    painter.end();

}


PectusPDF::PectusPDF(QObject *parent) : QObject(parent)
{
}

void PectusPDF::setRootQmlObject(QObject *obj)
{
    rootQmlObject = obj;
}

