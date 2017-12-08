#ifndef PECTUSPDF_H
#define PECTUSPDF_H

#include <QObject>
#include <QVariant>
#include <QVector>
#include <QQuickWindow>
#include <QQuickItem>
#include <QSharedPointer>
#include <QQuickItemGrabResult>
#include "pectusviewer.h"
#include "pectusprocessor.h"

class PectusPDF : public QObject
{   
    Q_OBJECT
public:
    explicit PectusPDF(PectusViewer * viewer_input, PectusProcessor * processor_input, QObject *parent = nullptr);

    void setRootQmlObject(QObject* obj);

    Q_INVOKABLE void createPDF();

private:
    QObject* rootQmlObject;
    PectusViewer * viewer;
    PectusProcessor * processor;

    void addNotes(QPainter & painter);
    void addScanViewer(QPainter & painter, QQuickWindow * window);
    void addSlice(QPainter & painter, QQuickWindow * window);
    void addSummary(QPainter & painter);

};

#endif // PECTUSPDF_H
