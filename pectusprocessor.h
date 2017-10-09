#ifndef PECTUSPROCESSOR_H
#define PECTUSPROCESSOR_H

#include <QObject>
#include <QDebug>
#include <vector>

class PectusProcessor : public QObject
{

    Q_OBJECT
    Q_PROPERTY(QString fileName READ getFileName NOTIFY fileNameChanged)

public:

    explicit PectusProcessor(QObject *parent = nullptr);

    QString getFileName();
    void processFile();

    Q_INVOKABLE void setFileName(const QString & filename);

private:
    QString m_fileName;
    std::vector<std::vector<double>> vertices, textures, normals;
    std::vector<std::vector<QString>> faces;


signals:
    void fileNameChanged(const QString & arg);

public slots:

};

#endif // PECTUSPROCESSOR_H
