#ifndef AREA_H
#define AREA_H

#include <cmath>
#include <QObject>
#include <QtMath>
#include <QDebug>
#include <QFile>
#include "types.h"

double distance(double x1, double x2, double y1, double y2);
double areaTrapezoid(double l1, double l2, double l3, double l4);
double areaTriangle(double l1, double l2, double l3);
double defectArea(Vertex v1, Vertex v2, QVector<QPair<Vertex, Vertex>> defectSegments);
double chestArea();

#endif // AREA_H
