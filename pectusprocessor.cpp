#include "pectusprocessor.h"
#include <QFile>
#include <limits>
#include <cmath>
#include <QtMath>

const int CANVAS_DRAWING_FACTOR = 750;
const int AREA_FACTOR = 1000;

PectusProcessor::PectusProcessor(QObject *parent) : QObject(parent), m_fileName(""), vertices(), faces(){

}

void PectusProcessor::setFileName(const QString & filename){
#if defined(Q_OS_WIN)
    QString newname = filename.right(filename.size() - 8);//removes file:///
#else
    QString newname = filename.right(filename.size() - 7);
#endif
    m_fileName = newname;
    processFile();
}

void PectusProcessor::processFile(){
    QFile inputFile(m_fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       // Clear out values from previous file
       vertices.clear();
       normals.clear();
       textures.clear();
       faces.clear();
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList parts = line.split(" ");
          if(parts[0] == "v"){
              double x = parts[1].toDouble(), y = parts[2].toDouble(), z = parts[3].toDouble();
              Vertex v(x, y, z);
              if (y < miny) {
                  miny = y;
              } if (y > maxy) {
                  maxy = y;
              }
              vertices.push_back(v);
          }
          else if(parts[0] == "vt"){
              Texture v(parts[1].toDouble(), parts[2].toDouble());
              textures.push_back(v);
          }
          else if(parts[0] == "vn"){
              Vertex v(parts[1].toDouble(), parts[2].toDouble(), parts[3].toDouble());
              normals.push_back(v);
          }
          else if(parts[0] == "f"){
              Face v(parts[1], parts[2], parts[3]);
              v.vertex1Index = v.vertex1String.split("/")[0].toInt() - 1;
              v.vertex2Index = v.vertex2String.split("/")[0].toInt() - 1;
              v.vertex3Index = v.vertex3String.split("/")[0].toInt() - 1;
              faces.push_back(v);
          }
       }
       inputFile.close();
    }
    else{
        qDebug() << QString("Not open");
    }
}

void PectusProcessor::setRootQmlObject(QObject *obj)
{
    rootQmlObject = obj;
}

//from http://www.geeksforgeeks.org/program-for-point-of-intersection-of-two-lines/
Vertex lineIntersection(Vertex A, Vertex B, Vertex C, Vertex D){
    double a1 = B.z - A.z;
    double b1 = A.x - B.x;
    double c1 = a1*(A.x) + b1*(A.z);

    double a2 = D.z - C.z;
    double b2 = C.x - D.x;
    double c2 = a2*(C.x)+ b2*(C.z);

    double determinant = a1*b2 - a2*b1;

    return Vertex((b2*c1 - b1*c2)/determinant, 0 ,(a1*c2 - a2*c1)/determinant);
}

void PectusProcessor::drawLineSegments()
{
    QObject *canvas = rootQmlObject->findChild<QObject*>("canvas");
    double midpoint = (minx.x + maxx.x) / 2;
    Vertex v1, v2;//v1 and v2 will be two vertices that have the same x value (+- .01)
                 //as the midpoint of the horizontal distance, one will be at top of chest, one at bottom
    for (int i = 0; i < sliceSegments.size(); i++) {
        if((sliceSegments[i].first.x < midpoint && sliceSegments[i].first.x + .005 > midpoint) ||
           (sliceSegments[i].first.x > midpoint && sliceSegments[i].first.x - .005 < midpoint)){
            if(v1.x != 0)
                v2 = sliceSegments[i].first;
            else v1 = sliceSegments[i].first;
        }

    }

    double rotationAngle = qAtan((maxx.z - minx.z) / (maxx.x - minx.x)); // slope of "horizontal" diameter
    Vertex centerOfSlice = lineIntersection(minx, maxx, v1, v2);
    minx.x = std::numeric_limits<double>::max();
    maxx.x = std::numeric_limits<double>::min();
     for (int i = 0; i < sliceSegments.size(); i++) {
         sliceSegments[i].first.x = qCos(rotationAngle) * (sliceSegments[i].first.x - centerOfSlice.x) + qSin(rotationAngle) * (sliceSegments[i].first.z - centerOfSlice.z) + centerOfSlice.x;
         sliceSegments[i].first.z = -qSin(rotationAngle) * (sliceSegments[i].first.x - centerOfSlice.x) + qCos(rotationAngle) * (sliceSegments[i].first.z - centerOfSlice.z) + centerOfSlice.z;
         sliceSegments[i].second.x = qCos(rotationAngle) * (sliceSegments[i].second.x - centerOfSlice.x) + qSin(rotationAngle) * (sliceSegments[i].second.z - centerOfSlice.z) + centerOfSlice.x;
         sliceSegments[i].second.z = -qSin(rotationAngle) * (sliceSegments[i].second.x - centerOfSlice.x) + qCos(rotationAngle) * (sliceSegments[i].second.z - centerOfSlice.z) + centerOfSlice.z;

         if (sliceSegments[i].first.x < minx.x) {
             minx = sliceSegments[i].first;
         } if (sliceSegments[i].first.x > maxx.x) {
             maxx = sliceSegments[i].first;
         }
         if (sliceSegments[i].second.x < minx.x) {
             minx = sliceSegments[i].second;
         } if (sliceSegments[i].first.x > maxx.x) {
             maxx = sliceSegments[i].second;
         }
         if (sliceSegments[i].first.x > 1.0 || sliceSegments[i].first.y > 1.0 || sliceSegments[i].second.x > 1.0 || sliceSegments[i].second.y > 1.0) {
             qDebug() << "Vertex value greater than 1: " << sliceSegments[i].first.x << sliceSegments[i].first.y << sliceSegments[i].second.x << sliceSegments[i].second.y;
         }
         QMetaObject::invokeMethod(canvas, "drawLine",
             Q_ARG(QVariant, sliceSegments[i].first.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, sliceSegments[i].first.z*CANVAS_DRAWING_FACTOR),
             Q_ARG(QVariant, sliceSegments[i].second.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, sliceSegments[i].second.z*CANVAS_DRAWING_FACTOR));
     }


}

QString PectusProcessor::getFileName(){
    return m_fileName;
}

void PectusProcessor::calculateIntersection(double yPlane){
    lastYPlane = yPlane;
    QVector<Face> intersectedFaces;
    sliceSegments.clear();
    rightArmRemoved = false;
    leftArmRemoved = false;

    for(int i = 0; i < faces.size(); i++){
        bool above = false, below = false;
        int on = 0; //Still intersects if 2 points are on the plane (3 shouldn't happen)
        Vertex v1 = vertices[faces[i].vertex1Index];
        Vertex v2 = vertices[faces[i].vertex2Index];
        Vertex v3 = vertices[faces[i].vertex3Index];
        if(v1.y > yPlane || v2.y > yPlane || v3.y > yPlane)
            above = true;
        if(v1.y < yPlane || v2.y < yPlane || v3.y < yPlane)
            below = true;
        if(v1.y == yPlane)
            on++;
        if(v2.y == yPlane)
            on++;
        if(v3.y == yPlane)
            on++;
        if((above && below) || on == 2){
            intersectedFaces.push_back(faces[i]);
        }
    }

    minx.x = std::numeric_limits<double>::max();
    minz.z = std::numeric_limits<double>::max();
    maxx.x = std::numeric_limits<double>::min();
    maxz.z = std::numeric_limits<double>::min();


    //need to find intersection on each face now
    for (int i = 0; i < intersectedFaces.size(); i++){
        QPair<Vertex, Vertex> segment = findSegment(intersectedFaces[i], yPlane);
        sliceSegments.push_back(segment);
    }
    if (sliceSegments.isEmpty())
        return;

    setLimits();
    sliceSegments = findLargestSet();
    setLimits();
    connectOpenSegments();
    orderSegments();

    if (armRemovalEnabled){
        findRemovalPoints();
    }
    setLimits();
}

// Sets limits on the given slice (min and max x and z)
void PectusProcessor::setLimits(){
    for(int i = 0; i < sliceSegments.size(); i++){
        if (sliceSegments[i].first.x < minx.x) {
            minx = sliceSegments[i].first;
        }
        if (sliceSegments[i].first.x > maxx.x) {
            maxx = sliceSegments[i].first;
        }
        if (sliceSegments[i].second.x < minx.x) {
            minx = sliceSegments[i].second;
        }
        if (sliceSegments[i].second.x > maxx.x) {
            maxx = sliceSegments[i].second;
        }
        if (sliceSegments[i].first.z < minz.z) {
            minz = sliceSegments[i].first;
        }
        if (sliceSegments[i].first.z > maxz.z) {
            maxz = sliceSegments[i].first;
        }
        if (sliceSegments[i].second.z < minz.z) {
            minz = sliceSegments[i].second;
        }
        if (sliceSegments[i].second.z > maxz.z) {
            maxz = sliceSegments[i].second;
        }
    }
}

// Returns the line segment (represented by a pair of vertices) where Face f intersects plane yPlane
QPair<Vertex, Vertex> PectusProcessor::findSegment(const Face & f, double yPlane){
    QPair<Vertex, Vertex> segment;
    Vertex a(vertices[f.vertex1Index]);
    Vertex b(vertices[f.vertex2Index]);
    Vertex c(vertices[f.vertex3Index]);
    if(a.y == yPlane || b.y == yPlane || c.y == yPlane){
        // Each possible case
        // A-B is the segment
        if (a.y == yPlane && b.y == yPlane){
            segment.first = a;
            segment.second = b;
        }
        // A-C is the segment
        else if (a.y == yPlane && c.y == yPlane){
            segment.first = a;
            segment.second = c;
        }
        // B-C is the segment
        else if (b.y == yPlane && c.y == yPlane){
            segment.first = b;
            segment.second = c;
        }
        // A is in the segment
        else if (a.y == yPlane){
            segment.first = a;
            segment.second = findVertex(b, c, yPlane);
        }
        // B is in the segment
        else if (b.y == yPlane){
            segment.first = b;
            segment.second = findVertex(a, c, yPlane);
        }
        // C is in the segment
        else if (c.y == yPlane){
            segment.first = c;
            segment.second = findVertex(a, b, yPlane);
        }
    }
    // A and B are on the same side of the plane
    // The vertices are on AC and BC
    else if ((a.y > yPlane && b.y > yPlane) ||
             (a.y < yPlane && b.y < yPlane)){
        segment.first = findVertex(a, c, yPlane);
        segment.second = findVertex(b, c, yPlane);
    }
    // A and C are on the same side of the plane
    // The vertices are on AB and BC
    else if ((a.y > yPlane && c.y > yPlane) ||
             (a.y < yPlane && c.y < yPlane)){
        segment.first = findVertex(a, b, yPlane);
        segment.second = findVertex(b, c, yPlane);
    }
    // B and C are on the same side of the plane
    // The vertices are on AB and AC
    else if ((b.y > yPlane && c.y > yPlane) ||
             (b.y < yPlane && c.y < yPlane)){
        segment.first = findVertex(a, b, yPlane);
        segment.second = findVertex(a, c, yPlane);
    }
    else {
        qDebug() << "An error ocurred trying to find an intersection for a plane";
        segment.first = Vertex(0, 0, 0);
        segment.second = Vertex(0, 0, 0);
    }
    return segment;
}

// Returns the vertex where the line segment (a-b) intersects the plane.
Vertex PectusProcessor::findVertex(const Vertex & a, const Vertex & b, double yPlane){
    double xcross = a.x - ((a.y - yPlane) / (a.y - b.y) * (a.x - b.x));
    double zcross = a.z - ((a.y - yPlane) / (a.y - b.y) * (a.z - b.z));
    return Vertex(xcross, yPlane, zcross);
}

double PectusProcessor:: getMaxY() {
    return maxy;
}

double PectusProcessor:: getMinY() {
    return miny;
}

double PectusProcessor:: getHallerIndex() {
    return hallerIndex;
}

bool PectusProcessor::getHallerIndexVisible() {
    return hallerIndexVisible;
}

void PectusProcessor::calculateHallerIndex(){
    if (sliceSegments.isEmpty())
        return;

    QObject *canvas = rootQmlObject->findChild<QObject*>("canvas");

    QMetaObject::invokeMethod(canvas, "drawLine",
        Q_ARG(QVariant, minx.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, minx.z*CANVAS_DRAWING_FACTOR),
        Q_ARG(QVariant, maxx.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, maxx.z*CANVAS_DRAWING_FACTOR));

    findDefectLine();

    double horizontalDistance = std::sqrt(std::pow(maxx.x - minx.x, 2) + std::pow(maxx.z - minx.z, 2));
    double verticalDistance = std::sqrt(std::pow(hallerV2.x - hallerV1.x, 2) + std::pow(hallerV2.z - hallerV1.z, 2));
    qDebug() << horizontalDistance << "  " <<verticalDistance;
    qDebug() << hallerV2.x << " " << hallerV1.x << " " <<hallerV2.z << " " << hallerV1.z;
    hallerIndex = horizontalDistance / verticalDistance;
    hallerIndex = std::round(hallerIndex * 1000) / 1000;
    emit hallerIndexChanged(hallerIndex);
    hallerIndexVisible = true;
    emit hallerIndexVisibleChanged(true);
}


void PectusProcessor::findDefectLine() {
    // Cannot run if sliceSegments is empty
    if (sliceSegments.isEmpty())
        return;

    double defectLimitAndPointDiff = 0;
    Vertex bottomDefectPoint = findDefectPoint(false, defectLimitAndPointDiff);
    Vertex topDefectPoint = findDefectPoint(true, defectLimitAndPointDiff);

    QObject *canvas = rootQmlObject->findChild<QObject*>("canvas");

    QMetaObject::invokeMethod(canvas, "drawLine",
        Q_ARG(QVariant, topDefectPoint.x * CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, topDefectPoint.z * CANVAS_DRAWING_FACTOR),
        Q_ARG(QVariant, bottomDefectPoint.x * CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, bottomDefectPoint.z * CANVAS_DRAWING_FACTOR));

}

void PectusProcessor::printDefectSegments() {

    QObject *canvas = rootQmlObject->findChild<QObject*>("canvas");
    for (int i = 0; i < defectSegments.size(); i++) {
        QMetaObject::invokeMethod(canvas, "drawLine",
            Q_ARG(QVariant, defectSegments[i].first.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, defectSegments[i].first.z*CANVAS_DRAWING_FACTOR),
            Q_ARG(QVariant, defectSegments[i].second.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, defectSegments[i].second.z*CANVAS_DRAWING_FACTOR));
    }

}

void PectusProcessor::manualRemoveConnectedArms(double xStart, double zStart, double xEnd, double zEnd, double canvasWidth, double canvasHeight) {

    // the line that was drawn
    QPair<Vertex, Vertex> lineDrawn = { { xStart, 0, zStart }, { xEnd, 0, zEnd } };

    // y = mx + b
    // b = y - mx
    double slopeOfLineDrawn = getSlopeOfLine(lineDrawn);
    double bOfLineDrawn = (zStart - (slopeOfLineDrawn * xStart));

    QVector<QPair<Vertex, Vertex>> intersections;
    for (int i = 0; i < sliceSegments.size(); i++) {

        // mSeg * x + bSeg = mDrawn * x + bDrawn
        // mSeg * x - mDrawn * x = bDrawn - bSeg
        // (mSeg - mDrawn) * x = bDrawn - bSeg
        // x = (bDrawn - bSeg) / (mSeg - mDrawn)

        double slopeOfSliceSegment = getSlopeOfLine(sliceSegments[i]);
        double bOfSegment = (sliceSegments[i].first.z * CANVAS_DRAWING_FACTOR) - (slopeOfSliceSegment * sliceSegments[i].first.x * CANVAS_DRAWING_FACTOR);
        double xIntersection = (bOfLineDrawn - bOfSegment) / (slopeOfSliceSegment - slopeOfLineDrawn);
        double zIntersection = slopeOfLineDrawn * xIntersection + bOfLineDrawn;

        double minXOfSegment = getMinXofLine(sliceSegments[i]) * CANVAS_DRAWING_FACTOR;
        double maxXOfSegment = getMaxXofLine(sliceSegments[i]) * CANVAS_DRAWING_FACTOR;
        double maxZOfDrawn = zStart > zEnd ? zStart : zEnd;
        double minZOfDrawn = zStart < zEnd ? zStart : zEnd;

        if (minXOfSegment <= xIntersection && xIntersection <= maxXOfSegment &&
                minZOfDrawn <= zIntersection && zIntersection <= maxZOfDrawn ) {
            intersections.push_back(sliceSegments[i]);
        }
    }

    // assumption is made that to erase an arm the line drawn must cross at least two lines
    if (intersections.size() != 2) {
        return;
    }

    double minXofDrawn = getMinXofLine(lineDrawn);
    double maxXofDrawn = getMaxXofLine(lineDrawn);
    bool isLeft;
    if (minXofDrawn < canvasWidth / 2 && maxXofDrawn < canvasWidth / 2) {
        isLeft = true;
    }
    else if (minXofDrawn > canvasWidth / 2 && maxXofDrawn > canvasWidth / 2) {
        isLeft = false;
    }
    else {
        // the line drawn goes over both halves of the canvas, which probably isn't right
        return;
    }

    for (int i = 0; i < sliceSegments.size(); i++) {

        Vertex vertexOfLineClosestToDrawn;
        if (isLeft) {
            if (sliceSegments[i].first.x < sliceSegments[i].second.x) {
                vertexOfLineClosestToDrawn = sliceSegments[i].second;
            }
            else {
                vertexOfLineClosestToDrawn = sliceSegments[i].first;
            }
        }
        else {
            if (sliceSegments[i].first.x > sliceSegments[i].second.x) {
                vertexOfLineClosestToDrawn = sliceSegments[i].second;
            }
            else {
                vertexOfLineClosestToDrawn = sliceSegments[i].first;
            }
        }

        // y = mx + b
        // (y - b) / m = x
        double xOfDrawnToEraseFrom = (vertexOfLineClosestToDrawn.z * CANVAS_DRAWING_FACTOR - bOfLineDrawn) / slopeOfLineDrawn;

        if (isLeft) {
            if (vertexOfLineClosestToDrawn.x * CANVAS_DRAWING_FACTOR < xOfDrawnToEraseFrom) {
                sliceSegments.erase(sliceSegments.begin() + i);
                i--;
            }
        }
        else {
            if (vertexOfLineClosestToDrawn.x * CANVAS_DRAWING_FACTOR > xOfDrawnToEraseFrom) {
                sliceSegments.erase(sliceSegments.begin() + i);
                i--;
            }
        }
    }

    // connect the intersection
    Vertex firstPoint;
    Vertex secondPoint;

    if (isLeft) {
        if (intersections[0].first.x < intersections[0].second.x) {
            firstPoint = intersections[0].first;
        }
        else {
            firstPoint = intersections[0].second;
        }
        if (intersections[1].first.x < intersections[1].second.x) {
            secondPoint = intersections[1].first;
        }
        else {
            secondPoint = intersections[1].second;
        }
    }
    else {
        if (intersections[0].first.x > intersections[0].second.x) {
            firstPoint = intersections[0].first;
        }
        else {
            firstPoint = intersections[0].second;
        }
        if (intersections[1].first.x > intersections[1].second.x) {
            secondPoint = intersections[1].first;
        }
        else {
            secondPoint = intersections[1].second;
        }
    }

    QPair<Vertex, Vertex> newLine = { firstPoint, secondPoint };
    sliceSegments.push_back(newLine);

    QObject *canvas = rootQmlObject->findChild<QObject*>("canvas");

    QMetaObject::invokeMethod(canvas, "eraseRect",
        Q_ARG(QVariant, 0), Q_ARG(QVariant, 0),
        Q_ARG(QVariant, canvasWidth), Q_ARG(QVariant, canvasHeight));

    for (QPair<Vertex, Vertex> & line : sliceSegments) {
        QMetaObject::invokeMethod(canvas, "drawLine",
            Q_ARG(QVariant, line.first.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, line.first.z*CANVAS_DRAWING_FACTOR),
            Q_ARG(QVariant, line.second.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, line.second.z*CANVAS_DRAWING_FACTOR));

    }

}

// Finds the point of deepest defect respective to the bottom or top of the torso
Vertex PectusProcessor::findDefectPoint(bool isTop, double & defectLimitAndPointDiff) {

    QVector<QPair<Vertex, Vertex>> possiblePoints;
    double max = std::numeric_limits<double>::max();
    QPair<Vertex, Vertex> minXSegment = { {max, max, max}, {max, max, max} };
    QPair<Vertex, Vertex> maxXSegment = { {-1, -1, -1}, {-1, -1, -1} };

    QPair<Vertex, Vertex> minMaxZSegment;
    if (isTop) {
        minMaxZSegment = { { 0, 0, 0}, {0, 0, 0} };
    }
    else {
        minMaxZSegment = { { max, max, max}, {max, max, max} };
    }

    QSet<int> visited;

    double midPointX = (minx.x + maxx.x) / 2;
    double midPointZ = (minz.z + maxz.z) / 2;
    double xOffset = 0.05 * (maxx.x - minx.x);

    for (int i = 0; i < sliceSegments.size(); i++) {

        // check if points are too far to the left
        if (sliceSegments[i].first.x < midPointX - xOffset || sliceSegments[i].second.x < midPointX - xOffset)
            continue;
        // check if points are too far to the right
        if (sliceSegments[i].first.x > midPointX + xOffset || sliceSegments[i].second.x > midPointX + xOffset)
            continue;

        // check if points are too far down or too far up
        if (isTop) {
            if (sliceSegments[i].first.z > midPointZ || sliceSegments[i].second.z > midPointZ)
                continue;
        }
        else {
            if (sliceSegments[i].first.z < midPointZ || sliceSegments[i].second.z < midPointZ)
                continue;
        }

        // get the respective limits (min x, max x, max z)
        double minPx = getMinXofLine(sliceSegments[i]);
        double minXx = getMinXofLine(minXSegment);

        double maxPx = getMaxXofLine(sliceSegments[i]);
        double maxXx = getMaxXofLine(maxXSegment);

        double minMaxPz, minMaxZz;
        if (isTop) {
            minMaxPz = getMaxZofLine(sliceSegments[i]);
            minMaxZz = getMaxZofLine(minMaxZSegment);
        }
        else {
            minMaxPz = getMinZofLine(sliceSegments[i]);
            minMaxZz = getMinZofLine(minMaxZSegment);
        }

        if (minPx < minXx) {
            minXSegment = sliceSegments[i];
        }

        if (maxPx > maxXx) {
            maxXSegment = sliceSegments[i];
        }

        if (isTop) {
            if (minMaxPz > minMaxZz) {
                minMaxZSegment = sliceSegments[i];
            }
        }
        else {
            if (minMaxPz < minMaxZz) {
                minMaxZSegment = sliceSegments[i];
            }
        }

        possiblePoints.push_back(sliceSegments[i]);
        visited.insert(i);
    }

    // find right and left most segments
    getDefectLeftRightLimits(visited, possiblePoints, true, isTop, minXSegment, minMaxZSegment);
    getDefectLeftRightLimits(visited, possiblePoints, false, isTop, maxXSegment, minMaxZSegment);

    double xOfDefect;
    if (isTop) {
        xOfDefect = minMaxZSegment.first.z > minMaxZSegment.second.z ? minMaxZSegment.first.x : minMaxZSegment.second.x;
    }
    else {
        xOfDefect = minMaxZSegment.first.z < minMaxZSegment.second.z ? minMaxZSegment.first.x : minMaxZSegment.second.x;
    }

    double zOfDefect;
    if (isTop) {
        zOfDefect = getMaxZofLine(minMaxZSegment);
        hallerV1 = { xOfDefect, minMaxZSegment.first.y, zOfDefect };
    }
    else {
        zOfDefect = getMinZofLine(minMaxZSegment);
        hallerV2 = { xOfDefect, minMaxZSegment.first.y, zOfDefect };
    }

    // the left and right defect limits, or "peaks" will be determined by the biggest difference
    // between the defect point and the defect limit.
    double minMaxZOfDefectLimits;
    if (isTop) {
        minMaxZOfDefectLimits = getMinZofLine(minXSegment) < getMinZofLine(maxXSegment) ?
            getMinZofLine(minXSegment) : getMinZofLine(maxXSegment);
    }
    else {
        minMaxZOfDefectLimits = getMaxZofLine(minXSegment) < getMaxZofLine(maxXSegment) ?
            getMinZofLine(maxXSegment) : getMaxZofLine(maxXSegment);
    }

    double defectDiff = std::fabs(zOfDefect - minMaxZOfDefectLimits);
    if (defectDiff > defectLimitAndPointDiff) {
        defectLimitAndPointDiff = defectDiff;

        // these lines represent the "peaks" of the chest
        leftDefectLimit = minXSegment;
        rightDefectLimit = maxXSegment;
        defectSegments = possiblePoints;
    }

    return { xOfDefect, minMaxZSegment.first.y, zOfDefect };

}

// Extends the points that could be the defect, also retrieves left and right most point from defect
void PectusProcessor::getDefectLeftRightLimits(QSet<int> &visited, QVector<QPair<Vertex, Vertex>> &possiblePoints,
                                               bool isLeft, bool isTop, QPair<Vertex, Vertex> & leftRightX,
                                               QPair<Vertex, Vertex> & minMaxZSegment)
{
    // extend a side to find the first opposite slope
    while (true) {
        double lowestDist = std::numeric_limits<double>::max();
        int bestIndex = -1;
        for (int j = 0; j < sliceSegments.size(); j++) {
            if (visited.find(j) != visited.end()) continue;

            double xDiff = std::fabs(leftRightX.second.x - sliceSegments[j].first.x);
            double zDiff = std::fabs(leftRightX.second.z - sliceSegments[j].first.z);

            double dist = (xDiff * xDiff) + (zDiff * zDiff);
            if (dist < lowestDist) {
                lowestDist = dist;
                bestIndex= j;
            }
        }
        if (bestIndex == -1)
            return;

        double slope = getSlopeOfLine(sliceSegments[bestIndex]);

        // what the slope should be may not be intuitive
        // because y increases as you go down the canvas
        if (isLeft) {
            if (isTop) {
                if (slope < 0)
                    break;
            }
            else {
                if (slope > 0)
                    break;
            }
        }
        else {
            if (isTop) {
                if (slope > 0)
                    break;
            }
            else {
                if (slope < 0)
                    break;
            }
        }

        double minMaxPz;
        double minMaxZz;
        if (isTop) {
            minMaxPz = getMaxZofLine(sliceSegments[bestIndex]);
            minMaxZz = getMaxZofLine(minMaxZSegment);
        }
        else {
            minMaxPz = getMinZofLine(sliceSegments[bestIndex]);
            minMaxZz = getMinZofLine(minMaxZSegment);
        }

        if (isTop) {
            if (minMaxPz > minMaxZz) {
                minMaxZSegment = sliceSegments[bestIndex];
            }
        }
        else {
            if (minMaxPz < minMaxZz) {
                minMaxZSegment = sliceSegments[bestIndex];
            }
        }

        possiblePoints.push_back(sliceSegments[bestIndex]);
        leftRightX = sliceSegments[bestIndex];
        visited.insert(bestIndex);
    }

    // erase the last segment (which will be equal to left or right defect limit
    if (!possiblePoints.isEmpty()) {
        possiblePoints.removeLast();
    }
}


double PectusProcessor::getSlopeOfLine(QPair<Vertex, Vertex> &segment)
{
    if (std::fabs(segment.second.x - segment.first.x) < 0.000001) {
        return 1000000000;
    }
    else {
        return (segment.second.z - segment.first.z) / (segment.second.x - segment.first.x);
    }

}

double PectusProcessor::getMinXofLine(QPair<Vertex, Vertex> &segment)
{
    return segment.first.x < segment.second.x ? segment.first.x : segment.second.x;
}

double PectusProcessor::getMinZofLine(QPair<Vertex, Vertex> &segment)
{
    return segment.first.z < segment.second.z ? segment.first.z : segment.second.z;
}

double PectusProcessor::getMaxXofLine(QPair<Vertex, Vertex> &segment)
{
    return segment.first.x > segment.second.x ? segment.first.x : segment.second.x;
}

double PectusProcessor::getMaxZofLine(QPair<Vertex, Vertex> &segment)
{
    return segment.first.z > segment.second.z ? segment.first.z : segment.second.z;
}

//Given a set of line segments, finds the largest set of connected line segments.
QVector<QPair<Vertex,Vertex>> PectusProcessor::findLargestSet(){
    QVector<bool> found = QVector<bool>(sliceSegments.size(), false);
    QVector<QVector<int>> sets;

    // Iterate through all segments to find a start point
    for(int i = 0; i < sliceSegments.size(); i++){
        // ignore segments already included in a set
        if(found[i]){
            continue;
        }
        found[i] = true;
        QVector<int> currentSet;
        currentSet.push_back(i);
        bool nextFound = true;
        int currentSegment = i;

        // Find all segments in the set connected to the start point
        while(nextFound){
            nextFound = false;
            for(int j = 0; j < sliceSegments.size(); j++){
                if(found[j])
                    continue;
                // check if the segment is connected to the current segment.
                if(sliceSegments[j].first == sliceSegments[currentSegment].first ||
                   sliceSegments[j].first == sliceSegments[currentSegment].second ||
                   sliceSegments[j].second == sliceSegments[currentSegment].first ||
                   sliceSegments[j].second == sliceSegments[currentSegment].second){
                    nextFound = true;
                    currentSet.push_back(j);
                    found[j] = true;
                    currentSegment = j;
                    break;
                }
            }

            // If you cannot find another segment, check the "other side" by moving back
            // to the start point. If you fail to find anything from the start point,
            // the set is complete.
            if(!nextFound){
                if(currentSegment != currentSet[0]){
                    currentSegment = currentSet[0];
                    nextFound = true;
                }
            }
        }
        sets.push_back(currentSet);
    }

    // Determine the largest connected set
    int maxSetSize = 0;
    int largestSet = -1;
    qDebug() << "There are " << sets.size() << " sets in this slice";
    for(int i = 0; i < sets.size(); i++){
        if (sets[i].size() > maxSetSize){
            maxSetSize = sets[i].size();
            largestSet = i;
        }
    }

    // Find the number of disconnected arms removed by this function
    double avgX = (minx.x + maxx.x) / 2; // ~middle of slice (exact not needed)
    numArmsRemoved = 0;
    for(int i = 0; i < sets.size(); i++){
        if(i == largestSet)
            continue;
        if (sets[i].size() > 50){
            numArmsRemoved++;
            if (sliceSegments[sets[i][0]].first.x < avgX){
                leftArmRemoved = true;
                qDebug() << "Left";
            }
            else if (sliceSegments[sets[i][0]].first.x >= avgX) {
                rightArmRemoved = true;
                qDebug() << "Right";
            }
        }
    }
    qDebug() << numArmsRemoved << " separated arm(s) removed from this slice";

    // Populate the vector of vertices
    QVector<QPair<Vertex, Vertex>> connected;
    for(int i = 0; i < sets[largestSet].size(); i++){
        connected.push_back(sliceSegments[sets[largestSet][i]]);
    }
    return connected;
}

// Orders the segments in sliceSegments
void PectusProcessor::orderSegments(){
    if (sliceSegments.size() < 2){
        return;
    }

    // Order the segments in a circular fashion.
    QVector<QPair<Vertex, Vertex>> newSegments;
    QVector<bool> included = QVector<bool>(sliceSegments.size(), false);
    newSegments.push_back(sliceSegments[0]);
    while (newSegments.size() < sliceSegments.size()){
        bool pushed = false;

        // Search for a segment that connects to the last segment in the new vector.
        for (int i = 0; i < sliceSegments.size(); i++){
            if (included[i]){
                continue;
            }
            // Don't repeat the same segment.
            if ((newSegments[newSegments.size() - 1].first == sliceSegments[i].first ||
                    newSegments[newSegments.size() - 1].first == sliceSegments[i].second)
                    &&
                    (newSegments[newSegments.size() - 1].second == sliceSegments[i].first ||
                     newSegments[newSegments.size() - 1].second == sliceSegments[i].second)){
                continue;
            }


            if (newSegments[newSegments.size() - 1].second == sliceSegments[i].first){
                newSegments.push_back(sliceSegments[i]);
                included[i] = true;
                pushed = true;
                break;
            }
            else if (newSegments[newSegments.size() - 1].second == sliceSegments[i].second){
                QPair<Vertex, Vertex> flipped;
                flipped.first = sliceSegments[i].second;
                flipped.second = sliceSegments[i].first;
                newSegments.push_back(flipped);
                included[i] = true;
                pushed = true;
                break;
            }
        }
        if (!pushed){
            // Otherwise, this is a disconnected slice?
            for(int i = 0; i < sliceSegments.size(); i++){
                if (!included[i]){
                    newSegments.push_back(sliceSegments[i]);
                    included[i] = true;
                }
            }
        }
    }
    sliceSegments = newSegments;
}

void PectusProcessor::connectOpenSegments(){
    // Find segments with open ends.
    QVector<QPair<Vertex, Vertex>> openSegments;
    QVector<int> connectedSides;

    for(int i = 0; i < sliceSegments.size(); i++){
        int numConnected = 0;
        int connectedSide = 1;
        for(int j = 0; j < sliceSegments.size(); j++){
            if (i == j){
                continue;
            }
            if (sliceSegments[i].first == sliceSegments[j].first ||
                    sliceSegments[i].first == sliceSegments[j].second){
                numConnected++;
                connectedSide = 1;
            }
            else if (sliceSegments[i].second == sliceSegments[j].first ||
                     sliceSegments[i].second == sliceSegments[j].second){
                numConnected++;
                connectedSide = 2;
            }
        }
//        if (numConnected == 0){
//            qDebug() << "Something went wrong";
//            exit(1);
//        }
        if (numConnected == 1){
            qDebug() << "Open segment at " << i;
            openSegments.push_back(sliceSegments[i]);
            connectedSides.push_back(connectedSide);
        }
    }

    // Connect the two open-ended segments
    while (openSegments.size() >= 2) {
        double shortestDistance = 99999;
        int idx = -1;
        // Connect closest pairs of open points
        for (int i = 1; i < openSegments.size(); i++){
            double dist;
            if (connectedSides[0] == 1 && connectedSides[i] == 1){
                dist = distance(openSegments[0].second.x, openSegments[i].second.x, openSegments[0].second.z, openSegments[i].second.z);
            }
            else if (connectedSides[0] == 1 && connectedSides[i] == 2){
                dist = distance(openSegments[0].second.x, openSegments[i].first.x, openSegments[0].second.z, openSegments[i].first.z);
            }
            else if (connectedSides[0] == 2 && connectedSides[i] == 1){
                dist = distance(openSegments[0].first.x, openSegments[i].second.x, openSegments[0].first.z, openSegments[i].second.z);
            }
            else {
                dist = distance(openSegments[0].first.x, openSegments[i].first.x, openSegments[0].first.z, openSegments[i].first.z);
            }
            if (dist < shortestDistance){
                shortestDistance = dist;
                idx = i;
            }
        }

        if (idx == -1){
            break;
        }
        QPair<Vertex, Vertex> segmentToAdd;
        if (connectedSides[0] == 1){
            segmentToAdd.first = openSegments[0].second;
        }
        else {
            segmentToAdd.first = openSegments[0].first;
        }
        if (connectedSides[idx] == 1){
            segmentToAdd.second = openSegments[idx].second;
        }
        else {
            segmentToAdd.second = openSegments[idx].first;
        }
        sliceSegments.push_back(segmentToAdd);
        openSegments.remove(idx);
        connectedSides.remove(idx);
        openSegments.remove(0);
        connectedSides.remove(0);
    }

//    if (openSegments.size() == 2 && connectedSides.size() == 2){
//        qDebug() << connectedSides[0] << " " << connectedSides[1];
//        QPair<Vertex, Vertex> segmentToAdd;
//        if (connectedSides[0] == 1){
//            segmentToAdd.first = openSegments[0].second;
//        }
//        else {
//            segmentToAdd.first = openSegments[0].first;
//        }
//        if (connectedSides[1] == 1){
//            segmentToAdd.second = openSegments[1].second;
//        }
//        else {
//            segmentToAdd.second = openSegments[1].first;
//        }
//        sliceSegments.push_back(segmentToAdd);
//    }
}

double PectusProcessor::chestArea(bool asymmetric) {
    double area = 0.0;
    Vertex v1, v2;
    for (long i = 0; i < sliceSegments.size(); ++i) {
        v1 = sliceSegments[i].first;
        v2 = sliceSegments[i].second;
        if (asymmetric) {
            if (v1.x >= hallerV1.x && v2.x >= hallerV1.x)
                continue;
            else if (v1.x >= hallerV1.x) {
                v1.x = hallerV1.x;
            }
            else if (v2.x >= hallerV1.x) {
                v2.x = hallerV1.x;
            }
        }
        double slope = (minx.z - maxx.z) / (minx.x - maxx.x);
        double v3_z = slope * (v1.x - maxx.x) + maxx.z;
        double v4_z = slope * (v2.x - maxx.x) + maxx.z;

        double line1 = AREA_FACTOR * distance(v1.x, v2.x, v1.z, v2.z);
        double line2 = AREA_FACTOR * distance(v1.x, v1.x, v1.z, v3_z);
        double line3 = AREA_FACTOR * distance(v2.x, v2.x, v2.z, v4_z);
        double line4 = AREA_FACTOR * distance(v1.x, v2.x, v3_z, v4_z);


        if (line1 > line2 && line1 > line3) {
            qDebug() << "Lines: " << line1 << line2 << line3 << line4;
        }
        else if (line4 > line2 && line4 > line3) {
            qDebug() << "Lines: " << line1 << line2 << line3 << line4;
        }

        if (line2 < .00001) {
            area += areaTriangle(line1, line3, line4);
        }

        else if (line3 < .00001) {
            area += areaTriangle(line1, line2, line4);
        }

        else {
            area += areaTrapezoid(line1, line2, line3, line4);
        }
    }
    qDebug() << area;
    return area;
}

double PectusProcessor::defectArea() {
    double area = 0.0;
    double x = (leftDefectLimit.first.x + rightDefectLimit.first.x) / 2;
    double y = (leftDefectLimit.first.y + rightDefectLimit.first.y) / 2;

    for (int i = 0; i < defectSegments.size(); ++i) {
        double l1 = distance(x, defectSegments[i].first.x, y, defectSegments[i].first.y);
        double l2 = distance(x, defectSegments[i].second.x, y, defectSegments[i].second.y);
        double l3 = distance(defectSegments[i].second.x, defectSegments[i].first.x,
                             defectSegments[i].second.y, defectSegments[i].first.y);
        area += areaTriangle(l1, l2, l3);
    }
    return area;
}

double PectusProcessor::volumeDefectIndex() {
    double chest_area = chestArea(false);
    double defect_area = defectArea();
    // need to talk with Dr. Campbell about the ratio
    return defect_area / (chest_area + defect_area);
}

void PectusProcessor::asymmetricIndex() {
    asymmetricIndexVisible = true;
    asymmetricIndexVisibleChanged(asymmetricIndexVisible);
    double total_chest = chestArea(false);
    double half_chest = chestArea(true);
    total_chest -= half_chest;
    qDebug() << half_chest/total_chest;
    asymmetricIndexValue = half_chest/total_chest;
    emit asymmetricIndexValueChanged(asymmetricIndexValue);
}

double PectusProcessor::getVolumeDefectIndexValue() {
    return volumeDefectIndexValue;
}
bool PectusProcessor::getVolumeDefectIndexVisible() {
    return volumeDefectIndexVisible;
}

// Prints all values of line segments to the console
void PectusProcessor::printSegments(){
    qDebug() << "There are " << sliceSegments.size() << " segments in this slice.";
    for (int i = 0; i < sliceSegments.size(); i++){
        qDebug() << "First---- x: " << sliceSegments[i].first.x << "; z: " << sliceSegments[i].first.z << ";";
        qDebug() << "Second--- x: " << sliceSegments[i].second.x << "; z: " << sliceSegments[i].second.z << ";";
    }
}


double PectusProcessor::getAsymmetricIndexValue(){
    return asymmetricIndexValue;
}

bool PectusProcessor::getAsymmetricIndexVisable() {
    return asymmetricIndexVisible;
}

void PectusProcessor::enableArmRemoval(bool arg){
    armRemovalEnabled = arg;
    emit armRemovalEnabledChanged(arg);
}

bool PectusProcessor::getArmRemovalEnabled(){
    return armRemovalEnabled;
}

bool PectusProcessor::getFirstClickPressed(){
    return firstClickPressed;
}

double PectusProcessor::getFirstClickLocation(){
    return firstClickLocation;
}

void PectusProcessor::setFirstClickPressed(bool pressed){
    firstClickPressed = pressed;
}

void PectusProcessor::setFirstClickLocation(double yPlane){
    firstClickLocation = yPlane;
}

// Removing connected arms from 2D slice.
void PectusProcessor::findRemovalPoints(){
    if (sliceSegments.size() < 2){
        return;
    }
    qDebug() << "Running removeArms()";
    QVector<Vertex> localMaxes;
    QVector<int> localMaxIndices;
    QVector<Vertex> localMins;
    QVector<int> localMinIndices;
    bool zIncreasing = sliceSegments[sliceSegments.size() - 1].second.z - sliceSegments[sliceSegments.size() - 1].first.z > 0;

    // Find local minimums and maximums
    for(int i = 0; i < sliceSegments.size(); i++){
        double zDifference = sliceSegments[i].second.z - sliceSegments[i].first.z;
        if (zDifference > 0){
            if (!zIncreasing){
                localMins.push_back(sliceSegments[i].first);
                localMinIndices.push_back(i);
                zIncreasing = true;
            }
        }
        else {// zDifference <= 0
            if (zIncreasing){
                localMaxes.push_back(sliceSegments[i].first);
                localMaxIndices.push_back(i);
                zIncreasing = false;
            }
        }
    }

    // Number of arms that still need to be removed from the slice
    int numArmsToRemove = 2 - numArmsRemoved;

    // localMin, localMax pairs that have been used
    QVector<QPair<int, int>> usedPairs;
    double avgX = (minx.x + maxx.x) / 2; // Approximation for middle of the slice

    // Remove as many arms as needed
    int numLoops = 0;
    while(numArmsToRemove > 0 && numLoops < 2){
        numLoops++;
        double shortestDistance = 99999;
        int armMin = -1;
        int armMax = -1;

        // Loop through local minimums
        for(int i = 0; i < localMins.size(); i++){

            // Don't use local mins on a side of the slice
            // where an arm has already been removed.
            if (localMins[i].x < avgX && leftArmRemoved){
                continue;
            }
            if (localMins[i].x >= avgX && rightArmRemoved){
                continue;
            }

            // Loop through local maximums
            for(int j = 0; j < localMaxes.size(); j++){
                bool used = false;

                // Find if this pair has already been used in arm removal.
                for(int k = 0; k < usedPairs.size(); k++){
                    if(localMinIndices[i] == usedPairs[k].first && localMaxIndices[j] == usedPairs[k].second){
                        used = true;
                        break;
                    }
                }
                if(used){
                    continue;
                }
                double dist = distance(localMins[i].x, localMaxes[j].x, localMins[i].z, localMaxes[j].z);
                if (dist < shortestDistance){
                    if (std::abs(localMinIndices[i] - localMaxIndices[j]) < 25){
                        continue;
                    }
                    shortestDistance = dist;
                    armMin = localMinIndices[i];
                    armMax = localMaxIndices[j];
                }
            }// Local maxes
        }// Local mins

        if (armMin == -1 || armMax == -1)
            continue;

        qDebug() << "Removing arm outside " << armMin << " and " << armMax << " with distance " << shortestDistance;
        usedPairs.push_back(QPair<int, int>(armMin, armMax));
        numArmsToRemove--;

        // If this arm is on the left side of the torso, lock the left side from further arm removal
        if (sliceSegments[armMin].first.x < avgX){
            leftArmRemoved = true;
        }
        // Same goes for the right side.
        else if (sliceSegments[armMin].first.x >= avgX){
            rightArmRemoved = true;
        }
    }

    QVector<QPair<Vertex, Vertex>> usedPairVertices;
    for(int i = 0; i < usedPairs.size(); i++){
        usedPairVertices.push_back(QPair<Vertex, Vertex>(sliceSegments[usedPairs[i].first].first, sliceSegments[usedPairs[i].second].first));
    }

    for(int i = 0; i < usedPairVertices.size(); i++){
        removeArms(usedPairVertices[i]);
    }
}

// Removes arms to the outside of a given pair of points.
void PectusProcessor::removeArms(QPair<Vertex, Vertex> & points){
    Vertex pointA = points.first;
    Vertex pointB = points.second;

    // Possibly change to defect x value.
    double avgX = (minx.x + maxx.x) / 2;
    if((pointA.x < avgX && pointB.x > avgX) ||
            (pointA.x > avgX && pointB.x < avgX)){
        qDebug() << "Points are on opposite sides of the midpoint";
        return;
    }

    double outsideX;
    double insideX;
    double outsideZ;
    QVector<QPair<Vertex, Vertex>> newSegments;
    int angleType = 0;
    /*  Angle Types
     *      0:  |
     *      1:  /
     *      2:  \
     */
    if ((pointA.x - pointB.x > 0 && pointA.z - pointB.z > 0) ||
            (pointA.x - pointB.x < 0 && pointA.z - pointB.z < 0)){
        angleType = 1;
    }
    else if ((pointA.x - pointB.x < 0 && pointA.z - pointB.z > 0) ||
             (pointA.x - pointB.x > 0 && pointA.z - pointB.z < 0)){
        angleType = 2;
    }
    if (pointA.x < avgX){ // Remove to the left
        outsideX = std::min(pointA.x, pointB.x);
        insideX = std::max(pointA.x, pointB.x);
        if (angleType < 2){
            outsideZ = std::min(pointA.z, pointB.z);
        }
        else {
            outsideZ = std::max(pointA.z, pointB.z);
        }

        for (int i = 0; i < sliceSegments.size(); i++){

            // If the segment is outside the cut point
            if (sliceSegments[i].first.x < outsideX ||
                    sliceSegments[i].second.x < outsideX){
                continue;
            }

            // If the segment is outside the inside x and outside
            // the outside z
            if (sliceSegments[i].first.x < insideX ||
                    sliceSegments[i].second.x < insideX){
                if (angleType == 1){
                    if (sliceSegments[i].first.z > outsideZ ||
                            sliceSegments[i].second.z > outsideZ){
                        continue;
                    }
                }
                else if (angleType == 2){
                    if (sliceSegments[i].first.z < outsideZ ||
                            sliceSegments[i].second.z < outsideZ){
                        continue;
                    }
                }
            }

            // Otherwise, keep the segment.
            newSegments.push_back(sliceSegments[i]);
        }
    }
    else { // Remove to the right
        outsideX = std::max(pointA.x, pointB.x);
        insideX = std::min(pointA.x, pointB.x);
        if (angleType < 2){
            outsideZ = std::max(pointA.z, pointB.z);
        }
        else {
            outsideZ = std::min(pointA.z, pointB.z);
        }

        for (int i = 0; i < sliceSegments.size(); i++){

            // If the segment is outside the cut point
            if (sliceSegments[i].first.x > outsideX ||
                    sliceSegments[i].second.x > outsideX){
                continue;
            }

            // If the segment is outside the inside x and outside
            // the outside z
            if (sliceSegments[i].first.x > insideX ||
                    sliceSegments[i].second.x > insideX){
                if (angleType == 1){
                    if (sliceSegments[i].first.z < outsideZ ||
                            sliceSegments[i].second.z < outsideZ){
                        continue;
                    }
                }
                else if (angleType == 2){
                    if (sliceSegments[i].first.z > outsideZ ||
                            sliceSegments[i].second.z > outsideZ){
                        continue;
                    }
                }
            }

            // Otherwise, keep the segment
            newSegments.push_back(sliceSegments[i]);
        }
    }
    newSegments.push_back(QPair<Vertex, Vertex>(pointA, pointB));
    sliceSegments = newSegments;
    connectOpenSegments();
    orderSegments();
}

double PectusProcessor::getLastYPlane(){
    return lastYPlane;
}

bool PectusProcessor::getRunAllIndexes(){
    return runAllIndexes;
}

void PectusProcessor::setRunAllIndexes(bool arg){
    runAllIndexes = arg;
    emit runAllIndexesChanged(arg);
}
