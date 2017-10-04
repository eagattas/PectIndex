#ifndef PECTUSVIEWER_H
#define PECTUSVIEWER_H

#include <QObject>
#include <QDebug>

class PectusViewer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString scanFileName READ getScanFileName NOTIFY scanFileNameChanged)
    Q_PROPERTY(bool renderStatus READ getRenderStatus NOTIFY renderStatusChanged)

public:
    explicit PectusViewer(QObject *parent = nullptr);

    QString getScanFileName();
    bool getRenderStatus();
    Q_INVOKABLE void setScanFileName(const QString & arg);
    Q_INVOKABLE void renderScan();

private:
    QString m_scanFileName;
    bool m_renderStatus;


signals:
    void scanFileNameChanged(const QString & arg);
    void renderStatusChanged(bool arg);


public slots:
};

#endif // PECTUSVIEWER_H
