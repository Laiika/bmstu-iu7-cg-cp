#ifndef MESH_H
#define MESH_H

#include <camera.h>

#include <vector>
#include <cmath>
#include <QPoint>
#include <QMatrix4x4>
#include <iostream>
#include <QImage>

class Mesh
{
private:
    // Матрицы преобразования
    QMatrix4x4 matrix;

    QMatrix4x4 transX;
    QMatrix4x4 transY;
    QMatrix4x4 transZ;
    QMatrix4x4 trans;

    QMatrix4x4 scalX;
    QMatrix4x4 scalY;
    QMatrix4x4 scalZ;
    QMatrix4x4 scal;

    QMatrix4x4 rotX;
    QMatrix4x4 rotY;
    QMatrix4x4 rotZ;

public:
    // текстура
    QImage* texture;

    double size;
    int type;

    std::vector<QPoint> points;     // 2d точки сетки
    std::vector<Point3D> vertices;  // 3d точки сетки (в собственном пространстве)
    std::vector<Point3D> Tnodes;    // Узлы после пересчета с матрицей (в мировом пространстве)
    std::vector<long> triangles;
    std::vector<Point3D> NormVectors;   // Нормальные векторы всех треугольников

    // точки текстуры и ее итератор
    std::vector<QPoint> txtPs;
    int txtI = 0;

    double distanceToCamera;

public:
    Mesh();
    Mesh(Mesh* parent, QImage* tex, double length, double startX, double startY, double depth);

    // основные функции сетки
    void addTriangle(long a, long b, long c);
    void addQuad(long a, long b, long c, long d);
    void mapTo2D();
    void mapTo2D(Camera *camera);
    void refresh();
    void refresh_trans();
    void clear();

    virtual void initializeTexturePoints();

    // математические расчеты
    void calculateNormVectors();
    Point3D countNormalVector(int i, int i2, int i3);
    Point3D countNormalVector(Point3D p1, Point3D p2, Point3D p3);
    Point3D interpolNormalVectorVertice(int a, Point3D);
    Point3D countCamera3DPoint(Point3D a, Camera* camera);  // подсчитать 3D-точку камеры, прежде чем отображать ее на 2D-плоскость

    // матричные операции
    // функции, возвращающие все перемноженные матрицы
    QMatrix4x4 multiplyMatrixes();

    void translateX(int r);
    void translateY(int r);
    void translateZ(int r);
    void translate(int a, int b, int c);

    void scaleX(double s);
    void scaleY(double s);
    void scaleZ(double s);
    void scale(double s);

    void rotateX(int alfa);
    void rotateY(int alfa);
    void rotateZ(int alfa);
};

#endif // MESH_H
