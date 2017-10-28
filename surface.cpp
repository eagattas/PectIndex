#include "surface.h"
void Surface::createSurfaceModel(const QVector<Vertex> & vertices, const QVector<Face> & faces) {
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
    model->resetArray(model_data);
}

void Surface::resetModel() {
    model = new Q3DSurface();
}
