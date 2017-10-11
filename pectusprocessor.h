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
    QString vertex1String, vertex2String, vertex3String;
    int vertex1Index, vertex2Index, vertex3Index;
    Face(QString x, QString y, QString z) : vertex1String(x), vertex2String(y), vertex3String(z) {}
    Face() : vertex1String(""), vertex2String(""), vertex3String("") {}
};

class PectusProcessor : public QObject
{

    Q_OBJECT
    Q_PROPERTY(QString fileName READ getFileName NOTIFY fileNameChanged)

public:

    explicit PectusProcessor(QObject *parent = nullptr);

    QString getFileName();
    void processFile();
    void setRootQmlObject(QObject* obj);

    Q_INVOKABLE void drawLineSegments();

    Q_INVOKABLE void setFileName(const QString & filename);
    Q_INVOKABLE void calculateIntersection(double yPlane);




private:
    QString m_fileName;
    QVector<Vertex> vertices, normals;
    QVector<Texture> textures;
    QVector<Face> faces;

    QPair<Vertex, Vertex> findSegment(const Face & f, double yPlane);
    Vertex findVertex(const Vertex & a, const Vertex & b, double yPlane);

    // rootQmlObject - allows interaction with QML
    QObject* rootQmlObject;


signals:
    void fileNameChanged(const QString & arg);

public slots:

};

#endif // PECTUSPROCESSOR_H
