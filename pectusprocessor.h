#ifndef PECTUSPROCESSOR_H
#define PECTUSPROCESSOR_H

#include <QObject>
#include <QDebug>

struct Vertex{
    double x,y,z;
    Vertex(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {}
    Vertex() : x(0), y(0), z(0) {}
};

struct Texture{
    double x,y;
    Texture(double xx, double yy) : x(xx), y(yy) {}
    Texture() : x(0), y(0) {}
};

struct Face{
    QString vertexX, vertexY, vertexZ;
    Face(QString x, QString y, QString z) : vertexX(x), vertexY(y), vertexZ(z) {}
    Face() : vertexX(""), vertexY(""), vertexZ("") {}
};

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
    QVector<Vertex> vertices, normals;
    QVector<Texture> textures;
    QVector<Face> faces;


signals:
    void fileNameChanged(const QString & arg);

public slots:

};

#endif // PECTUSPROCESSOR_H
