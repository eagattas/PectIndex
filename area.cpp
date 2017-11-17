#include "area.h"

double distance(double x1, double x2, double y1, double y2) {
    double length = 0.0;
    double x = x2 - x1;
    double y = y2 - y1;
    length = sqrt(pow(x, 2.0) + pow(y, 2.0));
    return length;
}

//l2 and l3 must be the parellel lines of the trapezoid
// for this to work the arms need to be removed, otherwise there could be error
double areaTrapezoid(double l1, double l2, double l3, double l4) {
    double area = 0.0;
    double temp;
    if (l2 < l3) {
        temp = l2;
        l2 = l3;
        l3 = temp;
    }
    double temp1 = (l2 + l1 - l3 + l4);
    double temp2 = (l2 - l1 - l3 + l4);
    double temp3 = (l2 + l1 - l3 - l4);
    double temp4 = (-l2 + l1 + l3 + l4);

    if (temp1 < .00001) {
        temp1 = 0.0;
    }
    if (temp2 < .00001) {
        temp2 = 0.0;
    }
    if (temp3 < .00001) {
        temp3 = 0.0;
    }
    if (temp4 < .00001) {
        temp4 = 0.0;
    }

    double temp5 = temp1*temp2*temp3*temp4;
    if (temp5 < 0.0) {
        qDebug() << "Trapezoid area less than 0: " << l1 << l2 << l3 << l4;
    }
    else {
        area = sqrt(temp1*temp2*temp3*temp4);
        area = area*(l2+l3)/(4*(l2-l3));
    }
    return area;
}

double areaTriangle(double l1, double l2, double l3) {
    double area = 0.0;
    double S = (l1 + l2 + l3) / 2;
    double temp1 = S - l1;
    double temp2 = S - l2;
    double temp3 = S - l3;

    area = S * temp1 * temp2 * temp3;
    qDebug() << S << temp1 << temp2 << temp3 << area;
    if (area < 0) {
        qDebug() << "Area less than 0: " << l1 << l2 << l3 << area;
    }
    area = sqrt(area);
    return area;
}

