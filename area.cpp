#include "area.h"

double distance(double x1, double x2, double y1, double y2) {
    double length = 0.0;
    double x = x2 - x1;
    double y = y2 - y1;
    length = sqrt(pow(x, 2.0) + pow(y, 2.0));
    return length;
}

//l2 and l3 must be the parellel lines of the trapezoid
double areaTrapezoid(double l1, double l2, double l3, double l4) {
    double area = 0.0;
    double temp;
    l1 = l1 * 1000;
    l2 = l2 * 1000;
    l3 = l3 * 1000;
    l4 = l4 * 1000;
    if (l2 < l3) {
        temp = l2;
        l2 = l3;
        l3 = temp;
    }
    double temp1 = (l2 + l1 - l3 + l4);
    double temp2 = (l2 - l1 - l3 + l4);
    double temp3 = (l2 + l1 - l3 - l4);
    double temp4 = (-l2 + l1 + l3 + l4);
    double temp5 = temp1*temp2*temp3*temp4;
    qDebug() << temp1 << temp2 << temp3 << temp4 << temp5;
    area = sqrt(temp1*temp2*temp3*temp4);
    area = area*(l2+l3)/(4*(l2-l3));
    return area;
}

double areaTriangle(double l1, double l2, double l3) {
    double area = 0.0;
    double S = (l1 + l2 + l3) / 2;
    area = S * (S - l1) * (S - l2) * (S - l3);
    area = sqrt(area);
    return area;
}

