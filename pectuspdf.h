#ifndef PECTUSPDF_H
#define PECTUSPDF_H

#include <QObject>
#include <QVariant>
#include <QVector>
#include <QQuickWindow>
#include <QQuickItem>
#include <QSharedPointer>
#include <QQuickItemGrabResult>

class PectusPDF : public QObject
{   
    Q_OBJECT
public:
    explicit PectusPDF(QObject *parent = nullptr);

    void setRootQmlObject(QObject* obj);

    Q_INVOKABLE void createPDF(int width, int height, int x, int y);

private:
    QObject* rootQmlObject;

};

#endif // PECTUSPDF_H
