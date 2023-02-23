#ifndef PAINTER_H
#define PAINTER_H

#include <mesh.h>

#include <QImage>
#include <vector>
#include <cmath>
#include <QColor>

class Painter
{
private:
    int width;
    int height;
    Point3D lightSource;
    std::vector<std::vector<int>> zBuffer;

public:
    Camera* camera;
    Point3D star;
    Painter();
    Painter(int w, int h, Camera* cam);
    void setPixel(QImage *img, int x, int y, short red, short green, short blue);
    void setColor(int x, int y, QColor kolor, double light, QImage *img);
    void drawLine(int x0, int y0, int x1, int y1, QImage *img);

    // простой текстурный треугольник
    void textureTriangle(std::vector<QPoint> points, std::vector<QPoint> texturePoints, QImage *img, QImage *img2, double light);
    //texture triangle with light
    void textureTriangle(Mesh mesh, int a, int b, int c, std::vector<QPoint> points, std::vector<QPoint> texturePoints, QImage *img, QImage *img2, double light);
    void textureSpTriangle(Mesh mesh, int a, int b, int c, std::vector<QPoint> points, std::vector<QPoint> texturePoints, QImage *img, QImage *img2, double light, Point3D);

    // затенение (закраска) по Гуро
    //Args Точки A, B, C и их интенсивности, точки x, y и разрезы x1, x2 треугольными линиями
    double guroShading(QPoint A, QPoint B, QPoint C, double Ia, double Ib, double Ic, double x, double y, double x1, double x2);

    void drawMesh(Mesh &mesh, QImage *img);
    void drawMesh(Mesh &mesh, QImage *img, QImage *texture, Camera *camera, Point3D);

    // функция, которые текстурируют сторону (стену) куба
    void textureWall(QPoint p1, QPoint p2, QPoint p3, QPoint p4, QImage *MainImage, QImage *TextureImage, double light);


    double countLightIntense(Point3D N, Point3D point);
    double countStarLightIntense(Point3D N, Point3D point);
    Point3D* getLightSource();

    void clearBuffer();
};

#endif // PAINTER_H
