#ifndef AREA_H
#define AREA_H

#include <cmath>
#include <QObject>
#include <QtMath>
#include <QDebug>
#include <QFile>
#include "pectusprocessor.h"

double distance(double x1, double x2, double y1, double y2);
double areaTrapezoid(double l1, double l2, double l3, double l4);
double areaTriangle(double l1, double l2, double l3);
double defectArea(Vertex v1, Vertex v2, QVector<QPair<Vertex, Vertex>> defectSegments);
double chestArea(Vertex minx, Vertex maxx, QVector<QPair<Vertex, Vertex>> sliceSegments);

#endif // AREA_H
