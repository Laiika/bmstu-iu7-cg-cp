#include <tools.h>

double trianglePole(Point3D A, Point3D B, Point3D C)
{
    double pole;
    Point3D v1 = A.vector2Pts(A,B);
    Point3D v2 = A.vector2Pts(A,C);
    pole = 0.5 * sqrt(v1.cross(v2).dot(v1.cross(v2)));
    return pole;
}

double stronaPlaszczyzny(Point3D &P, Point3D &A, Point3D &B, Point3D &N)
{
    Point3D V = Point3D(B.x - A.x, B.y - A.y, B.z - A.z);
    V = V.normal();
    Point3D Nplas = V.cross(N);
    Nplas = Nplas.normal();

    return Nplas.x*(P.x - A.x) + Nplas.y*(P.y - A.y) + Nplas.z*(P.z - A.z);
}

QMatrix4x4 generateRotMat(double alfaX, double alfaY, double alfaZ)
{
    double rad;
    rad = alfaX / 360.0 * 2 * 3.141592;
    QMatrix4x4 rotX = QMatrix4x4(1,0,0,0,0,cos(rad),-sin(rad),0,0,sin(rad),cos(rad),0,0,0,0,1);

    rad = alfaY / 360.0 * 2 * 3.141592;
    QMatrix4x4 rotY = QMatrix4x4(cos(rad),0,sin(rad),0,0,1,0,0,-sin(rad),0,cos(rad),0,0,0,0,1);

    rad = alfaZ / 360.0 * 2 * 3.141592;
    QMatrix4x4 rotZ = QMatrix4x4(cos(rad),-sin(rad),0,0,sin(rad),cos(rad),0,0,0,0,1,0,0,0,0,1);

    return rotX * rotY * rotZ;
}
