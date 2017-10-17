#include "pectusprocessor.h"
#include <QFile>
#include <limits>
#include <cmath>

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

void PectusProcessor::drawLineSegments()
{
    QObject *canvas = rootQmlObject->findChild<QObject*>("canvas");
    double midpoint = (minx.x + maxx.x) / 2;
    Vertex v1, v2;//v1 and v2 will be two vertices that have the same x value (+- .01)
                 //as the midpoint of the horizontal distance, one will be at top of chest, one at bottom
    for (int i = 0; i < sliceSegments.size(); i++) {
        if((sliceSegments[i].first.x < midpoint && sliceSegments[i].first.x + .01 > midpoint) ||
           (sliceSegments[i].first.x > midpoint && sliceSegments[i].first.x - .01 < midpoint)){
            if(v1.x != 0)
                v2 = sliceSegments[i].first;
            else v1 = sliceSegments[i].first;
            //qDebug() << i;
        }
        QMetaObject::invokeMethod(canvas, "drawLine",
            Q_ARG(QVariant, sliceSegments[i].first.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, sliceSegments[i].first.z*CANVAS_DRAWING_FACTOR),
            Q_ARG(QVariant, sliceSegments[i].second.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, sliceSegments[i].second.z*CANVAS_DRAWING_FACTOR));
    }

    //NEED TO REWRITE THIS ALGORITHM TO ONLY CHECK FOR CLOSEST VERTEX THAT HASN"T BEEN VISITED YET, THEN IT CAN FORM A CLOSED SHAPE
    // fill in the gaps between the segments
//    for (int i = 0; i < sliceSegments.size() - 1; i++) {
//        double lowestDist = std::numeric_limits<double>::infinity();
//        int lowestIndex = std::numeric_limits<int>::max();

//        for (int j = 0; j < sliceSegments.size(); j++) {
//            if (j == i) continue;

//            double xDiff = std::fabs(sliceSegments[i].second.x - sliceSegments[j].first.x);
//            double zDiff = std::fabs(sliceSegments[i].second.z - sliceSegments[j].first.z);

//            double dist = (xDiff * xDiff) + (zDiff * zDiff);
//            if (dist < lowestDist) {
//                lowestDist = dist;
//                lowestIndex = j;
//            }
//        }

//        QMetaObject::invokeMethod(canvas, "drawLine",
//            Q_ARG(QVariant, sliceSegments[i].second.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, sliceSegments[i].second.z*CANVAS_DRAWING_FACTOR),
//            Q_ARG(QVariant, sliceSegments[lowestIndex].first.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, sliceSegments[lowestIndex].first.z*CANVAS_DRAWING_FACTOR));

//    }

    QMetaObject::invokeMethod(canvas, "drawLine",
        Q_ARG(QVariant, minx.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, minx.z*CANVAS_DRAWING_FACTOR),
        Q_ARG(QVariant, maxx.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, maxx.z*CANVAS_DRAWING_FACTOR));

    QMetaObject::invokeMethod(canvas, "drawLine",
        Q_ARG(QVariant, v1.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, v1.z*CANVAS_DRAWING_FACTOR),
        Q_ARG(QVariant, v2.x*CANVAS_DRAWING_FACTOR), Q_ARG(QVariant, v2.z*CANVAS_DRAWING_FACTOR));

    double horizontalDistance = std::sqrt(std::pow(maxx.x - minx.x, 2) + std::pow(maxx.z - minx.z, 2));
    double verticalDistance = std::sqrt(std::pow(v2.x - v1.x, 2) + std::pow(v2.z - v1.z, 2));
    hallerIndex = horizontalDistance / verticalDistance;
    hallerIndex = std::round(hallerIndex * 1000) / 1000;
    emit hallerIndexChanged(hallerIndex);

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
        } if (segment.first.x > maxx.x) {
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

using namespace QtDataVisualization;
void PectusProcessor::createSurfaceModel(const QVector<Vertex> & vertices, const QVector<Face> & faces) {
    int i = 0;
    QSurfaceDataArray *model_data = new QSurfaceDataArray;
    while (i < faces.size()){
        float v1x = vertices[faces[i].vertex1Index].x;
        float v1y = vertices[faces[i].vertex1Index].y;
        float v1z = vertices[faces[i].vertex1Index].z;
        float v2x = vertices[faces[i].vertex2Index].x;
        float v2y = vertices[faces[i].vertex2Index].y;
        float v2z = vertices[faces[i].vertex2Index].z;
        float v3x = vertices[faces[i].vertex3Index].x;
        float v3y = vertices[faces[i].vertex3Index].y;
        float v3z = vertices[faces[i].vertex3Index].z;

        QSurfaceDataRow * face = new QSurfaceDataRow;
        QVector3D * v1 = new QVector3D(v1x, v1y, v1z);
        QVector3D * v2 = new QVector3D(v2x, v2y, v2z);
        QVector3D * v3 = new QVector3D(v3x, v3y, v3z);

        *face << *v1 << *v2 << *v3;
        *model_data << face;
    }
    // need to bind this data to a surface like:
    //model_surface->resetArray(model_data);
}
