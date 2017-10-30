#include "area.h"

double distance(double x1, double x2, double y1, double y2) {
    double length = 0.0;
    double x = x2 - x1;
    double y = y2 - y1;
    length = sqrt(pow(x, 2.0) + pow(y, 2.0));
    return length;
}

double areaTrapezoid(double l1, double l2, double l3, double l4) {
    double area = 0.0;
    double temp;
    if (l2 < l3) {
        temp = l2;
        l2 = l3;
        l3 = temp;
    }
    area = (l2+l3) / (4 * (l2-l3));
    temp = (l2 + l1 - l3 + l4) * (l2 - l1 - l3 + l4) * (l2 + l1 - l3 - l4) * (-l2 + l1 + l3 + l4);
    area = area * sqrt(temp);
    return area;
}

double areaTriangle(double l1, double l2, double l3) {
    double area = 0.0;
    double S = (l1 + l2 + l3) / 2;
    area = S * (S - l1) * (S - l2) * (S - l3);
    area = sqrt(area);
    return area;
}
double chestArea(Vertex minx, Vertex maxx, QVector<QPair<Vertex, Vertex>> sliceSegments) {
    double area = 0.0;

    for (long i = 0; i < sliceSegments.size(); ++i) {
        Vertex slice_v1 = sliceSegments[i].first;
        Vertex slice_v2 = sliceSegments[i].second;

        double slope = (minx.x - maxx.x) / (minx.y - maxx.y);
        double z_v1 = slope * (slice_v1.x - maxx.x) + maxx.y;
        double z_v2 = slope * (slice_v2.x - maxx.x) + maxx.y;

        //Size of the segment on the chest
        double line1 = distance(slice_v1.x, slice_v1.z, slice_v2.x, slice_v2.z);
        //From vertex on the chest to the horizontal line
        double line2 = distance(slice_v1.x, slice_v1.z, slice_v1.x, z_v1);
        double line3 = distance(slice_v2.x, slice_v2.z, slice_v2.x, z_v2);
        //length on the horizontal line
        double line4 = distance(slice_v1.x, z_v1, slice_v2.x, z_v2);

        double temp_area = areaTrapezoid(line1, line2, line3, line4);
        area += temp_area;
    }
    return area;
}

double defectArea(Vertex v1, Vertex v2, QVector<QPair<Vertex, Vertex>> defectSegments) {
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
