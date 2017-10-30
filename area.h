#ifndef AREA_H
#define AREA_H

#include <cmath>
#include <QObject>
#include <QtMath>
#include <QDebug>
#include <QFile>

double distance(double x1, double x2, double y1, double y2);
double areaTrapezoid(double l1, double l2, double l3, double l4);
double areaTriangle(double l1, double l2, double l3);

#endif // AREA_H
