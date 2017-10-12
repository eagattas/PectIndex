#include "pectusprocessor.h"
#include <QFile>
#include <limits>
#include <cmath>

PectusProcessor::PectusProcessor(QObject *parent) : QObject(parent), m_fileName(""), vertices(), faces(){

}

void PectusProcessor::setFileName(const QString & filename){
    QString newname = filename.right(filename.size() - 8);//removes file:///
    m_fileName = newname;
    processFile();
}

void PectusProcessor::processFile(){
    QFile inputFile(m_fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList parts = line.split(" ");
          if(parts[0] == "v"){
              Vertex v(parts[1].toDouble(), parts[2].toDouble(), parts[3].toDouble());
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
       this->calculateIntersection(0.3);
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

    for (int i = 0; i < sliceSegments.size(); i++) {

        QMetaObject::invokeMethod(canvas, "drawLine",
            Q_ARG(QVariant, sliceSegments[i].first.x*750), Q_ARG(QVariant, sliceSegments[i].first.z*750),
            Q_ARG(QVariant, sliceSegments[i].second.x*750), Q_ARG(QVariant, sliceSegments[i].second.z*750));
    }

    // fill in the gaps between the segments
    for (int i = 0; i < sliceSegments.size() - 1; i++) {
        double lowestDist = std::numeric_limits<double>::infinity();
        int lowestIndex = std::numeric_limits<int>::max();

        for (int j = 0; j < sliceSegments.size(); j++) {
            if (j == i) continue;

            double xDiff = std::fabs(sliceSegments[i].second.x - sliceSegments[j].first.x);
            double zDiff = std::fabs(sliceSegments[i].second.z - sliceSegments[j].first.z);

            double dist = (xDiff * xDiff) + (zDiff * zDiff);
            if (dist < lowestDist) {
                lowestDist = dist;
                lowestIndex = j;
            }
        }

        QMetaObject::invokeMethod(canvas, "drawLine",
            Q_ARG(QVariant, sliceSegments[i].second.x*750), Q_ARG(QVariant, sliceSegments[i].second.z*750),
            Q_ARG(QVariant, sliceSegments[lowestIndex].first.x*750), Q_ARG(QVariant, sliceSegments[lowestIndex].first.z*750));

    }

}

QString PectusProcessor::getFileName(){
    return m_fileName;
}

void PectusProcessor::calculateIntersection(double yPlane){
    QVector<Face> intersectedFaces;

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
    //need to find intersection on each face now
    for (int i = 0; i < intersectedFaces.size(); i++){
        sliceSegments.push_back(findSegment(intersectedFaces[i], yPlane));
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
