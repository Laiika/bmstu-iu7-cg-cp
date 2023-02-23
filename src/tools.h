#ifndef TOOLS
#define TOOLS

#define _USE_MATH_DEFINES
#include <cmath>
#include <QMatrix4x4>

struct Point3D
{
    double x, y, z;

    Point3D() : x(0), y(0), z(0){}
    Point3D(double v) : x(v), y(v), z(v) {}
    Point3D(double ix, double iy, double iz) : x(ix), y(iy), z(iz) {}
    Point3D operator +(const Point3D &other) const { return Point3D(x + other.x, y + other.y, z + other.z); }
    Point3D operator -(const Point3D &other) const { return Point3D(x - other.x, y - other.y, z - other.z); }
    Point3D operator *(const Point3D &other) const { return Point3D(x * other.x, y * other.y, z * other.z); }
    Point3D operator *(const double &other) const { return Point3D(x * other, y * other, z * other); }
    Point3D operator *(const int &other) const { return Point3D(x * other, y * other, z * other); }

    double dot(const Point3D &other) // скалярное произведение
    {
        return x * other.x + y * other.y + z * other.z;
    }
    Point3D normal()
    {
        double l = sqrt(dot(Point3D(x,y,z)));
        if (l == 0)
        {
            return Point3D(0,0,0);
        }
        return Point3D(x/l, y/l, z/l);
    }
    Point3D cross(const Point3D &p) // перекрестное произведение
    {
        return Point3D(y * p.z - z * p.y,z * p.x - x * p.z, x * p.y - y * p.x);
    }
    // возвращает вектор от двух точек
    Point3D vector2Pts(Point3D p1, Point3D p2)
    {
        return Point3D(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
    }
};

double trianglePole(Point3D A, Point3D B, Point3D C);
// функция проверки точки P и треугольника со сторонами A и B и вектором нормали N, на какой стороне плоскости расположена точка P
double stronaPlaszczyzny(Point3D &P, Point3D &A, Point3D &B, Point3D &N);
QMatrix4x4 generateRotMat(double alfaX, double alfaY, double alfaZ);

#endif // TOOLS
