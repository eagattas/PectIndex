#ifndef PECTUSPROCESSOR_H
#define PECTUSPROCESSOR_H

#include <QObject>
#include <QDebug>
#include <limits>

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtWidgets/QSlider>
#include <cmath>
#include "area.h"

struct Vertex {
    double x,y,z;
    Vertex(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {}
    Vertex() : x(0), y(0), z(0) {}
    bool operator == (const Vertex & other){
        return (std::fabs(this->x - other.x) < 0.000001 &&
                std::fabs(this->y - other.y) < 0.000001 &&
                std::fabs(this->z - other.z) < 0.000001);
//        return (this->x == other.x &&
//                this->y == other.y &&
//                this->z == other.z);
    }
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
    Q_PROPERTY(double hallerIndex READ getHallerIndex NOTIFY hallerIndexChanged)
    Q_PROPERTY(bool hallerIndexVisible READ getHallerIndexVisible NOTIFY hallerIndexVisibleChanged)

public:

    explicit PectusProcessor(QObject *parent = nullptr);

    double hallerIndex = 0;
    QString getFileName();
    void processFile();
    void setRootQmlObject(QObject* obj);
    void createSurfaceModel(const QVector<Vertex> & vertices, const QVector<Face> & faces);

    Q_INVOKABLE void drawLineSegments();

    Q_INVOKABLE void setFileName(const QString & filename);
    Q_INVOKABLE void calculateIntersection(double yPlane);
    Q_INVOKABLE double getMaxY();
    Q_INVOKABLE double getMinY();
    Q_INVOKABLE double getHallerIndex();
    Q_INVOKABLE bool getHallerIndexVisible();
    Q_INVOKABLE void calculateHallerIndex();
    Q_INVOKABLE void getFixedIntersection();
    Q_INVOKABLE double chestArea();
    Q_INVOKABLE double defectArea(Vertex v1, Vertex v2, QVector<QPair<Vertex, Vertex>> defectSegments);

    // Erases arms that are completely disconnected from the drawing
    Q_INVOKABLE void eraseArms(int canvasWidth, int canvasHeight);

    // finds point of defect
    Q_INVOKABLE void findDefectPoint();

private:
    // rootQmlObject - allows interaction with QML
    QObject* rootQmlObject;

    QString m_fileName;
    QVector<Vertex> vertices, normals;
    QVector<Texture> textures;
    QVector<Face> faces;
    QVector<QPair<Vertex, Vertex>> sliceSegments;
    Vertex minx, maxx, minz, maxz;
    double miny = std::numeric_limits<double>::max();
    double maxy = std::numeric_limits<double>::min();
    bool hallerIndexVisible = false;
    Vertex hallerV1 = Vertex(0, 0, 0);
    Vertex hallerV2 = Vertex(0, 0, 0);


    // These two lines describe the furthest lines
    // from the defect where the slope has not changed signs
    QPair<Vertex, Vertex> leftDefectLimit;
    QPair<Vertex, Vertex> rightDefectLimit;



    QPair<Vertex, Vertex> findSegment(const Face & f, double yPlane);
    Vertex findVertex(const Vertex & a, const Vertex & b, double yPlane);


    // for gettin the start of an arm to delete
    int getArmStart(int canvasWidth, bool isLeft);

    void getDefectLeftRightLimits(QSet<int> &visited, QVector<QPair<Vertex, Vertex> > &possible_points,
                                  bool isLeft, QPair<Vertex, Vertex> & leftRightX, QPair<Vertex, Vertex> & maxZSegment);

    // get the slope of a line
    double getSlopeOfLine(QPair<Vertex, Vertex> & segment);

    // get the min X of a line
    double getMinXofLine(QPair<Vertex, Vertex> & segment);

    // get the min Z of a line
    double getMinZofLine(QPair<Vertex, Vertex> & segment);

    // get the max X of a line
    double getMaxXofLine(QPair<Vertex, Vertex> & segment);

    // get the max Z of a line
    double getMaxZofLine(QPair<Vertex, Vertex> & segment);

    QVector<QPair<Vertex,Vertex>> findLargestSet();

signals:
    void fileNameChanged(const QString & arg);
    void hallerIndexChanged(const double & arg);
    void hallerIndexVisibleChanged(const bool arg);
    void volumeDefectIndexChanged(const double & arg);
    void volumeDefectIndexVisibleChanged(const bool arg);

public slots:

};

#endif // PECTUSPROCESSOR_H
