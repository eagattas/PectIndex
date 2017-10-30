#include "pectusprocessor.h"
#include <QFile>
#include <limits>
#include <cmath>
#include <QtMath>

const int CANVAS_DRAWING_FACTOR = 750;

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

         QMetaObject::invokeMethod(canvas, "drawLine",
             Q_ARG(QVariant, sliceSegments[i].first.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, sliceSegments[i].first.z*CANVAS_DRAWING_FACTOR),
             Q_ARG(QVariant, sliceSegments[i].second.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, sliceSegments[i].second.z*CANVAS_DRAWING_FACTOR));
     }


}

QString PectusProcessor::getFileName(){
    return m_fileName;
}

void PectusProcessor::calculateIntersection(double yPlane){
    QVector<Face> intersectedFaces;
    sliceSegments.clear();

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
        if (segment.first.x < minx.x) {
            minx = segment.first;
        } if (segment.first.x > maxx.x) {
            maxx = segment.first;
        }
        if (segment.second.x < minx.x) {
            minx = segment.second;
        } if (segment.second.x > maxx.x) {
            maxx = segment.second;
        }
        if (segment.first.z < minz.z) {
            minz = segment.first;
        } if (segment.first.z > maxz.z) {
            maxz = segment.first;
        }
        if (segment.second.z < minz.z) {
            minz = segment.second;
        } if (segment.second.z > maxz.z) {
            maxz = segment.second;
        }
    }
    // Now, need to graph segments in 2D space.
}

void PectusProcessor::getFixedIntersection(){
    if (sliceSegments.isEmpty()){
        return;
    }
    minx.x = std::numeric_limits<double>::max();
    minz.z = std::numeric_limits<double>::max();
    maxx.x = std::numeric_limits<double>::min();
    maxz.z = std::numeric_limits<double>::min();

    sliceSegments = findLargestSet();
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

    findDefectPoint();

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

void PectusProcessor::eraseArms(int canvasWidth, int canvasHeight)
{
    // get the start of the left and right arms
    int leftArmStart = getArmStart(canvasWidth, true);
    int rightArmStart = getArmStart(canvasWidth, false);

    // scenario where theres only one arm thats disconnected
    // check if the start is on the right or left side of the drawing
    if (leftArmStart != -1 && leftArmStart == rightArmStart) {
        if (leftArmStart > (maxx.x + minx.x) / 2) {
            leftArmStart = -1;
        }
        else {
            rightArmStart = -1;
        }
    }

    QObject *canvas = rootQmlObject->findChild<QObject*>("canvas");

    if (leftArmStart != -1) {

        // clear the left arm by clearing a rectangle from its start to the left edge
        QMetaObject::invokeMethod(canvas, "eraseRect",
            Q_ARG(QVariant, 0), Q_ARG(QVariant, 0),
            Q_ARG(QVariant, leftArmStart), Q_ARG(QVariant, canvasHeight));

        // erase sliceSegments that corresponded to the left arm
        for (int i = 0; i < sliceSegments.size(); i++) {
            if (sliceSegments[i].first.x * CANVAS_DRAWING_FACTOR < leftArmStart ||
                    sliceSegments[i].second.x * CANVAS_DRAWING_FACTOR < leftArmStart) {
                sliceSegments.remove(i--);
            }
        }

        // TODO, erase faces and/or calculate intersection again instead?

    }

    if (rightArmStart != -1) {

        // clear the right arm by clearing a rectangle from its start to the right edge
        QMetaObject::invokeMethod(canvas, "eraseRect",
            Q_ARG(QVariant, rightArmStart), Q_ARG(QVariant, 0),
            Q_ARG(QVariant, canvasWidth - rightArmStart), Q_ARG(QVariant, canvasHeight));

        // erase sliceSegments that corresponded to the right arm
        for (int i = 0; i < sliceSegments.size(); i++) {
            if (sliceSegments[i].first.x * CANVAS_DRAWING_FACTOR > rightArmStart ||
                    sliceSegments[i].second.x * CANVAS_DRAWING_FACTOR > rightArmStart) {
                sliceSegments.remove(i--);
            }
        }

        // TODO, erase faces and/or calculate intersection again instead?
    }

}

// Finds the point of deepest defect
// TODO: adjust in case drawing is flipped
// TODO: handle random line segments inside the torso
void PectusProcessor::findDefectPoint() {

    QVector<QPair<Vertex, Vertex>> possible_points;
    double max = std::numeric_limits<double>::max();
    QPair<Vertex, Vertex> minXSegment = { {max, max, max}, {max, max, max} };
    QPair<Vertex, Vertex> maxXSegment = { {-1, -1, -1}, {-1, -1, -1} };
    QPair<Vertex, Vertex> maxZSegment = { {0, 0, 0}, {0, 0, 0} };

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
        // check if points are too far down
        if (sliceSegments[i].first.z > midPointZ || sliceSegments[i].second.z > midPointZ)
            continue;

        // get the respective limits (min x, max x, max z)
        double minPx = getMinXofLine(sliceSegments[i]);
        double minXx = getMinXofLine(minXSegment);

        double maxPx = getMaxXofLine(sliceSegments[i]);
        double maxXx = getMaxXofLine(maxXSegment);

        double maxPz = getMaxZofLine(sliceSegments[i]);
        double maxZz = getMaxZofLine(maxZSegment);

        if (minPx < minXx) {
            minXSegment = sliceSegments[i];
        }

        if (maxPx > maxXx) {
            maxXSegment = sliceSegments[i];
        }

        if (maxPz > maxZz) {
            maxZSegment = sliceSegments[i];
        }

        possible_points.push_back(sliceSegments[i]);
        visited.insert(i);
    }

    // find right and left most segments
    getDefectLeftRightLimits(visited, possible_points, true, minXSegment, maxZSegment);
    getDefectLeftRightLimits(visited, possible_points, false, maxXSegment, maxZSegment);

    double midXOfDefect = (maxZSegment.first.x + maxZSegment.second.x) / 2;

    // find the slice Segment to attach the vertical line to
    int lowest = -1;
    for (int i = 0; i < sliceSegments.size(); i++) {

        if (visited.find(i) != visited.end())
            continue;

        if ((sliceSegments[i].first.x >= midXOfDefect &&
             sliceSegments[i].second.x <= midXOfDefect) ||
                (sliceSegments[i].second.x >= midXOfDefect &&
                 sliceSegments[i].first.x <= midXOfDefect)) {

            if (lowest == -1) {
                lowest = i;
                continue;
            }

            double lowestBest = getMaxZofLine(sliceSegments[lowest]);
            double iBest = getMaxZofLine(sliceSegments[i]);

            if (iBest < lowestBest) {
                lowest = i;
            }
        }
    }

    double slopeOfLowest = getSlopeOfLine(sliceSegments[lowest]);
    double endZ = slopeOfLowest * (midXOfDefect - sliceSegments[lowest].first.x) + sliceSegments[lowest].first.z;

    double slopeOfDefectSegment = getSlopeOfLine(maxZSegment);
    double startZ = slopeOfDefectSegment * (midXOfDefect - maxZSegment.first.x) + maxZSegment.first.z;

    QObject *canvas = rootQmlObject->findChild<QObject*>("canvas");

    QMetaObject::invokeMethod(canvas, "drawLine",
        Q_ARG(QVariant, midXOfDefect*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, startZ*CANVAS_DRAWING_FACTOR),
        Q_ARG(QVariant, midXOfDefect*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, endZ * CANVAS_DRAWING_FACTOR));


    hallerV1 = {midXOfDefect, sliceSegments[lowest].first.y, startZ};
    hallerV2 = {midXOfDefect, sliceSegments[lowest].first.y, endZ};

    leftDefectLimit = minXSegment;
    rightDefectLimit = maxXSegment;

}

// Extends the points that could be the defect, also retrieves left and right most point from defect
// TODO: remove maxZSegment comparison to reduce chance of outlier line segment messing up vertical line?
void PectusProcessor::getDefectLeftRightLimits(QSet<int> &visited, QVector<QPair<Vertex, Vertex> > &possible_points, bool isLeft, QPair<Vertex, Vertex> & leftRightX, QPair<Vertex, Vertex> & maxZSegment)
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

        double slope = getSlopeOfLine(sliceSegments[bestIndex]);

        // what the slope should be may not be intuitive
        // because y increases as you go down the canvas
        if (isLeft) {
            if (slope < 0)
                break;
        }
        else {
            if (slope > 0)
                break;
        }

        double maxPz = getMaxZofLine(sliceSegments[bestIndex]);
        double maxZz = getMaxZofLine(maxZSegment);

        if (maxPz > maxZz) {
            maxZSegment = sliceSegments[bestIndex];
        }

        possible_points.push_back(sliceSegments[bestIndex]);
        leftRightX = sliceSegments[bestIndex];
        visited.insert(bestIndex);
    }
}

int PectusProcessor::getArmStart(int canvasWidth, bool isLeft) {

    // number of times we transition from empty white space to the drawing
    int numIntersectionCrossings = 0;

    // number of times we transition from the drawing to empty whitespace
    int numEmptyCrossings = 0;

    // keeps track of whether we are passing through the slice or empty whitespace
    bool inDrawingCrossing = false;

    // start of the arm that will be deleted
    int armStart = -1;

    // keeps track of the previous x value of the drawing before moving to empty whitespace
    int lastIntersectionCross = -1;

    for (int i = 0; i <= canvasWidth; i++) {

        // actual vertical line that we want depending on if we are looking for a right or left arm
        int offset = !isLeft ? canvasWidth - i : i;

        bool foundIntersection = false;
        for (int j = 0; j < sliceSegments.size(); j++) {
            // check for intersection between y = offset and the slice segment
            if ((sliceSegments[j].first.x * CANVAS_DRAWING_FACTOR >= offset &&
                 sliceSegments[j].second.x * CANVAS_DRAWING_FACTOR <= offset) ||
                    (sliceSegments[j].second.x * CANVAS_DRAWING_FACTOR >= offset &&
                     sliceSegments[j].first.x * CANVAS_DRAWING_FACTOR <= offset)) {
                foundIntersection = true;
                break;
            }
        }

        if (foundIntersection) {
            if (!inDrawingCrossing) {
                // We've moved from empty white space to drawing now
                numIntersectionCrossings++;
                inDrawingCrossing = true;
                // this case means that we hit the beginning of the body on the left side, no need to continue
                if (numEmptyCrossings == 1 && numIntersectionCrossings == 2) {
                    armStart = (lastIntersectionCross + offset) / 2 ;
                    break;
                }
            }
        }
        else {
            if (inDrawingCrossing) {
                // We've moved from drawing to empty whitespace now
                numEmptyCrossings++;
                inDrawingCrossing = false;
                lastIntersectionCross = isLeft ? offset - 1 : offset + 1;
            }
        }
    }

    return armStart;

}


double PectusProcessor::getSlopeOfLine(QPair<Vertex, Vertex> &segment)
{
    return (segment.second.z - segment.first.z) / (segment.second.x - segment.first.x);
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
    int maxSetSize = 0;
    int largestSet = -1;
    qDebug() << "There are " << sets.size() << " sets in this slice";
    for(int i = 0; i < sets.size(); i++){
        if (sets[i].size() > maxSetSize){
            maxSetSize = sets[i].size();
            largestSet = i;
        }
    }
    // Populate the vector of vertices
    QVector<QPair<Vertex, Vertex>> connected;
    for(int i = 0; i < sets[largestSet].size(); i++){
        connected.push_back(sliceSegments[sets[largestSet][i]]);
    }
    return connected;
}

double PectusProcessor::chestArea() {
    double area = 0.0;
    for (long i = 0; i < sliceSegments.size(); ++i) {
        Vertex slice_v1 = sliceSegments[i].first;
        Vertex slice_v2 = sliceSegments[i].second;

        double slope = (minx.z - maxx.z) / (minx.x - maxx.x);

        double z_v1 = slope * (slice_v1.x - maxx.x) + maxx.z;
        double z_v2 = slope * (slice_v2.x - maxx.x) + maxx.z;

        //qDebug() << slope << z_v1 << z_v2;
        //Size of the segment on the chest
        double line1 = distance(slice_v1.x, slice_v1.z, slice_v2.x, slice_v2.z);
        //From vertex on the chest to the horizontal line
        double line2 = distance(slice_v1.x, slice_v1.z, slice_v1.x, z_v1);
        double line3 = distance(slice_v2.x, slice_v2.z, slice_v2.x, z_v2);
        //length on the horizontal line
        double line4 = distance(slice_v1.x, z_v1, slice_v2.x, z_v2);

        qDebug() << line1 << line2 << line3 << line4;

        double temp_area = areaTrapezoid(line1, line2, line3, line4);
        //qDebug() << temp_area;
        area += temp_area;
    }
    qDebug() << "Chest Area: " << area;
    return area;
}

double PectusProcessor::defectArea(Vertex v1, Vertex v2, QVector<QPair<Vertex, Vertex>> defectSegments) {
    double area = 0.0;
    double x = (v1.x + v2.x) / 2;
    double y = (v1.y + v2.y) / 2;

    for (int i = 0; i < defectSegments.size(); ++i) {
        double l1 = distance(x, defectSegments[i].first.x, y, defectSegments[i].first.y);
        double l2 = distance(x, defectSegments[i].second.x, y, defectSegments[i].second.y);
        double l3 = distance(defectSegments[i].second.x, defectSegments[i].first.x,
                             defectSegments[i].second.y, defectSegments[i].first.y);
        area += areaTriangle(l1, l2, l3);
    }
    return area;
}
