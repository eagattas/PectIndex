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

    bool operator != (const Vertex & other){
        return !(*this == other);
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
    Q_PROPERTY(bool armRemovalEnabled READ getArmRemovalEnabled NOTIFY armRemovalEnabledChanged)
    Q_PROPERTY(bool firstClickPressed READ getFirstClickPressed NOTIFY firstClickPressedChanged)
    Q_PROPERTY(double firstClickLocation READ getFirstClickLocation NOTIFY firstClickLocationChanged)
    Q_PROPERTY(bool asymmetricIndexVisible READ getAsymmetricIndexVisible NOTIFY asymmetricIndexVisibleChanged)
    Q_PROPERTY(double asymmetricIndexValue READ getAsymmetricIndexValue NOTIFY asymmetricIndexValueChanged)
    Q_PROPERTY(bool volumeDefectIndexVisible READ getVolumeDefectIndexVisible NOTIFY volumeDefectIndexVisibleChanged)
    Q_PROPERTY(double volumeDefectIndexValue READ getVolumeDefectIndexValue NOTIFY volumeDefectIndexChanged)
    Q_PROPERTY(bool runAllIndexes READ getRunAllIndexes NOTIFY runAllIndexesChanged)
    Q_PROPERTY(OperationType lastOperation READ getLastOperation NOTIFY lastOperationChanged)

public:

    enum class OperationType {
        NoOperation = 0,
        SliceMode,
        BoundsMode
    };
    Q_ENUM(OperationType)

    explicit PectusProcessor(QObject *parent = nullptr);

    double hallerIndex = 0;
    QString getFileName();
    void processFile();
    void setRootQmlObject(QObject* obj);
    void createSurfaceModel(const QVector<Vertex> & vertices, const QVector<Face> & faces);

    Q_INVOKABLE void drawLineSegments();
    Q_INVOKABLE void drawLineSegmentsWithoutRotation();


    Q_INVOKABLE void setFileName(const QString & filename);
    Q_INVOKABLE void calculateIntersection(double yPlane);
    Q_INVOKABLE double getMaxY();
    Q_INVOKABLE double getMinY();
    Q_INVOKABLE double getHallerIndex();
    Q_INVOKABLE bool getHallerIndexVisible();
    Q_INVOKABLE void calculateHallerIndex();
    //Q_INVOKABLE void getFixedIntersection();
    Q_INVOKABLE void volumeDefectIndex();
    Q_INVOKABLE void asymmetricIndex();

    // Erases arms that are completely disconnected from the drawing
    //Q_INVOKABLE void eraseArms(int canvasWidth, int canvasHeight);
    Q_INVOKABLE double chestArea(bool asymmetric);
    Q_INVOKABLE double defectArea();
    Q_INVOKABLE void printSegments();
    Q_INVOKABLE void printDefectSegments();
    Q_INVOKABLE void manualRemoveConnectedArms(double xStart, double zStart, double xEnd, double zEnd, double canvasWidth, double canvasHeight);

    // finds points of defect
    Q_INVOKABLE void findDefectLine(bool draw);

    Q_INVOKABLE double getAsymmetricIndexValue();
    Q_INVOKABLE bool getAsymmetricIndexVisible();
    Q_INVOKABLE double getVolumeDefectIndexValue();
    Q_INVOKABLE bool getVolumeDefectIndexVisible();

    Q_INVOKABLE bool getFirstClickPressed();
    Q_INVOKABLE double getFirstClickLocation();
    Q_INVOKABLE void setFirstClickLocation(double yPlane);
    Q_INVOKABLE void setFirstClickPressed(bool pressed);
    Q_INVOKABLE void selectBounds(double yPlane);

    Q_INVOKABLE void enableArmRemoval(bool arg);
    bool getArmRemovalEnabled();
    Q_INVOKABLE double getLastYPlane();

    bool getRunAllIndexes();
    Q_INVOKABLE void setRunAllIndexes(bool arg);

    OperationType getLastOperation();
    Q_INVOKABLE void setLastOperation(OperationType arg);

private:
    // rootQmlObject - allows interaction with QML
    QObject* rootQmlObject;

    QString m_fileName;
    QVector<Vertex> vertices, normals;
    QVector<Texture> textures;
    QVector<Face> faces;
    QVector<QPair<Vertex, Vertex>> sliceSegments;
    QVector<QPair<Vertex, Vertex>> defectSegments;
    Vertex minx, maxx, minz, maxz;
    double miny = std::numeric_limits<double>::max();
    double maxy = std::numeric_limits<double>::min();
    bool hallerIndexVisible = false;
    Vertex hallerV1 = Vertex(0, 0, 0);
    Vertex hallerV2 = Vertex(0, 0, 0);

    bool asymmetricIndexVisible = false;
    bool volumeDefectIndexVisible = false;
    double asymmetricIndexValue = 0.0;
    double volumeDefectIndexValue = 0.0;

    bool armRemovalEnabled = false;
    int numArmsRemoved;
    bool rightArmRemoved = false;
    bool leftArmRemoved = false;
    double lastYPlane = 0;

    bool firstClickPressed  = false;
    double firstClickLocation = 0;
    double secondClickLocation = 0;

    bool runAllIndexes = false;

    OperationType lastOperation = OperationType::NoOperation;

    // These two lines describe the furthest lines
    // from the defect where the slope has not changed signs
    QPair<Vertex, Vertex> leftDefectLimit;
    QPair<Vertex, Vertex> rightDefectLimit;

    QPair<Vertex, Vertex> findSegment(const Face & f, double yPlane);
    Vertex findVertex(const Vertex & a, const Vertex & b, double yPlane);


    // helper functions for finding the defect line
    void getDefectLeftRightLimits(QSet<int> &visited, QVector<QPair<Vertex, Vertex> > &possible_points,
                                  bool isLeft, bool isTop, QPair<Vertex, Vertex> & leftRightX,
                                  QPair<Vertex, Vertex> & maxZSegment);
    Vertex findDefectPoint(bool isTop, double & defectLimitAndPointDiff);

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

    void setLimits();
    QVector<QPair<Vertex,Vertex>> findLargestSet();
    void connectOpenSegments();
    void orderSegments();
    void findRemovalPoints();
    void removeArms(QPair<Vertex, Vertex> & points);
    void disableAllIndexVisibilities();
    double calculateHallerIndexWithoutDrawing();

signals:
    void fileNameChanged(const QString & arg);
    void hallerIndexChanged(const double & arg);
    void hallerIndexVisibleChanged(const bool arg);
    void volumeDefectIndexChanged(const double & arg);
    void volumeDefectIndexVisibleChanged(const bool arg);
    void asymmetricIndexValueChanged(const double & arg);
    void asymmetricIndexVisibleChanged(const bool arg);
    void armRemovalEnabledChanged(const bool arg);
    void firstClickLocationChanged(const double & arg);
    void firstClickPressedChanged(const bool arg);
    void runAllIndexesChanged(const bool arg);
    void lastOperationChanged(const OperationType arg);

public slots:

};

#endif // PECTUSPROCESSOR_H
