#ifndef SPHERE_H
#define SPHERE_H

#include <mesh.h>

class Sphere : public Mesh
{
private:
    int meridian, parallel;

public:
    QMatrix4x4 rot;
    std::vector<Point3D> points;
    double m;
    int cur;
    Point3D center;

    Sphere(QImage *tex, int, int, double X, double Y, double Z, double size, int t, double _m);
    virtual void initializeTexturePoints();

    void set_rot(double aX, double aY, double aZ);
    void calc_trajectory(Point3D& s2, Point3D& vs1, Point3D& vs2, double m2);
};

#endif // SPHERE_H
