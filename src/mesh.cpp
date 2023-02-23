#include "mesh.h"

// дефолтный конструктор сетки создает корневую сетку
Mesh::Mesh()
{
    matrix = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

    transX = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
    transY = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
    transZ = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

    scalX = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
    scalY = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
    scalZ = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

    rotX = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
    rotY = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
    rotZ = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
}

Mesh::Mesh(Mesh *parent ,QImage *tex, double length, double startX, double startY, double depth)
{
    texture = tex;
    initializeTexturePoints();

    // инициализация базы (задняя сторона)
    vertices.push_back(Point3D(-length/2,-length/2,-length/2));
    vertices.push_back(Point3D(-length/2,length/2,-length/2));
    vertices.push_back(Point3D(length/2,length/2,-length/2));
    vertices.push_back(Point3D(length/2,-length/2,-length/2));

    // инициализация второй базы (лицевая сторона)
    vertices.push_back(Point3D(-length/2,-length/2,length/2));
    vertices.push_back(Point3D(-length/2,length/2,length/2));
    vertices.push_back(Point3D(length/2,length/2,length/2));
    vertices.push_back(Point3D(length/2,-length/2,length/2));

    // точки в центре стен
    vertices.push_back(Point3D(-length/2,0,0));
    vertices.push_back(Point3D(length/2,0,0));

    vertices.push_back(Point3D(0,-length/2,0));
    vertices.push_back(Point3D(0,length/2,0));

    vertices.push_back(Point3D(0,0,-length/2));
    vertices.push_back(Point3D(0,0,length/2));

    addQuad(0,1,2,3);
    addQuad(4,7,6,5);
    addQuad(0,4,5,1);
    addQuad(2,6,7,3);
    addQuad(0,3,7,4);
    addQuad(1,5,6,2);

    transX = QMatrix4x4(1,0,0,startX,0,1,0,0,0,0,1,0,0,0,0,1);
    transY = QMatrix4x4(1,0,0,0,0,1,0,startY,0,0,1,0,0,0,0,1);
    transZ = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,depth,0,0,0,1);

    refresh();
}


void Mesh::addTriangle(long a, long b, long c)
{
    triangles.push_back(a);
    triangles.push_back(b);
    triangles.push_back(c);
}

void Mesh::addQuad(long a, long b, long c, long d)
{
    triangles.push_back(a);
    triangles.push_back(b);
    triangles.push_back(c);

    triangles.push_back(c);
    triangles.push_back(d);
    triangles.push_back(a);
}

// простое отображение в 2D без учета положения камеры и поворота
void Mesh::mapTo2D()
{
    double d = 100000.0;
    points.clear();
    for(int i = 0; i < vertices.size(); i++)
    {
        points.push_back(QPoint(Tnodes[i].x/(1 + Tnodes[i].z/d), Tnodes[i].y/(1 + Tnodes[i].z/d)));
    }
}

void Mesh::mapTo2D(Camera *camera)
{
    QPoint b;
    points.clear();

    QMatrix4x4 rot = camera->rot;

    for(int i = 0; i < vertices.size(); i++)
    {
        Point3D ac = Point3D(Tnodes[i].x - camera->cX,Tnodes[i].y - camera->cY,Tnodes[i].z - camera->cZ);
        Point3D d = Point3D(rot(0,0) * ac.x + rot(0,1) * ac.y + rot(0,2) * ac.z, rot(1,0) * ac.x + rot(1,1) * ac.y + rot(1,2) * ac.z, rot(2,0) * ac.x + rot(2,1) * ac.y + rot(2,2) * ac.z);
        double mian = d.z;
        if(mian >= 0) mian = -1;
        b = QPoint(camera->eZ/mian * d.x - camera->eX,camera->eZ/mian * d.y - camera->eY);
        points.push_back(b);
    }
}


void Mesh::refresh()
{
    Tnodes.assign(vertices.size(),Point3D(0,0,0));
    matrix = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
    matrix *= transX * transY * transZ *  rotX * rotY * rotZ * scal;

    for(int i = 0; i < vertices.size(); i++){
        Tnodes[i].x = matrix(0,0) * vertices[i].x + matrix(0,1) * vertices[i].y + matrix(0,2) * vertices[i].z + matrix(0,3);
        Tnodes[i].y = matrix(1,0) * vertices[i].x + matrix(1,1) * vertices[i].y + matrix(1,2) * vertices[i].z + matrix(1,3);
        Tnodes[i].z = matrix(2,0) * vertices[i].x + matrix(2,1) * vertices[i].y + matrix(2,2) * vertices[i].z + matrix(2,3);
    }
    //calculateNormVectors();
    distanceToCamera = 0;
}

void Mesh::refresh_trans()
{
    Tnodes.assign(vertices.size(),Point3D(0,0,0));
    matrix = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
    matrix *= trans * rotX * scal;

    for(int i = 0; i < vertices.size(); i++){
        Tnodes[i].x = matrix(0,0) * vertices[i].x + matrix(0,1) * vertices[i].y + matrix(0,2) * vertices[i].z + matrix(0,3);
        Tnodes[i].y = matrix(1,0) * vertices[i].x + matrix(1,1) * vertices[i].y + matrix(1,2) * vertices[i].z + matrix(1,3);
        Tnodes[i].z = matrix(2,0) * vertices[i].x + matrix(2,1) * vertices[i].y + matrix(2,2) * vertices[i].z + matrix(2,3);
    }
    //calculateNormVectors();
    distanceToCamera = 0;
}

void Mesh::clear()
{
    vertices.clear();
    Tnodes.clear();
    points.clear();
    triangles.clear();
}

void Mesh::initializeTexturePoints()
{
    txtPs.clear();
    txtI = 0;
    if(texture)
    {
        txtPs.push_back(QPoint(0,0));
        txtPs.push_back(QPoint(0,texture->width()));
        txtPs.push_back(QPoint(texture->width(),texture->height()));

        txtPs.push_back(QPoint(0,0));
        txtPs.push_back(QPoint(0,texture->height()));
        txtPs.push_back(QPoint(texture->width(),texture->height()));
    }
}

void Mesh::calculateNormVectors()
{
    NormVectors.clear();
    for(int i = 0; i < triangles.size(); i += 3)
    {
        NormVectors.push_back(countNormalVector(triangles[i], triangles[i+1], triangles[i+2]));
    }
}

Point3D Mesh::countNormalVector(int i, int i2, int i3)
{
    if(i >= Tnodes.size() || i2  >= Tnodes.size() || i3 >= Tnodes.size())
    {
        return Point3D(0,0,0);
    }
    struct Point3D a = Point3D(Tnodes[i2].x - Tnodes[i].x, Tnodes[i2].y - Tnodes[i].y, Tnodes[i2].z - Tnodes[i].z);
    struct Point3D b = Point3D(Tnodes[i3].x - Tnodes[i].x, Tnodes[i3].y - Tnodes[i].y, Tnodes[i3].z - Tnodes[i].z);
    struct Point3D res = Point3D(a.y * b.z - a.z * b.y,a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    double length = sqrt(res.x * res.x + res.y * res.y + res.z * res.z);

    if(length != 0.0)
    {
        res = Point3D(res.x/length, res.y/length, res.z/length);
    }
    else
    {
        res = a;
    }
    return res;
}

Point3D Mesh::countNormalVector(Point3D p1, Point3D p2, Point3D p3)
{
    struct Point3D a = Point3D(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
    struct Point3D b = Point3D(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
    struct Point3D res = Point3D(a.y * b.z - a.z * b.y,a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    double length = sqrt(res.x * res.x + res.y * res.y + res.z * res.z);
    if(length != 0.0)
    {
        res = Point3D(res.x/length, res.y/length, res.z/length);
    }
    else
    {
        res = a;
    }
    return res;
}

Point3D Mesh::interpolNormalVectorVertice(int a, Point3D p)
{
    Point3D res = Point3D(0,0,0);
    for(int i = 0; i < triangles.size(); i++)
    {
        if(triangles[i] == a && i < Tnodes.size())
        {
            res = Point3D(Tnodes[i].x - p.x, Tnodes[i].y - p.y, Tnodes[i].z - p.z);
        }
    }
    return res;
}

Point3D Mesh::countCamera3DPoint(Point3D a, Camera *camera)
{
    camera->refresh();
    QMatrix4x4 rot = camera->rot;
    Point3D ac = Point3D(a.x - camera->cX,a.y - camera->cY,a.z - camera->cZ);
    Point3D d = Point3D(rot(0,0) * ac.x + rot(0,1) * ac.y + rot(0,2) * ac.z, rot(1,0) * ac.x + rot(1,1) * ac.y + rot(1,2) * ac.z, rot(2,0) * ac.x + rot(2,1) * ac.y + rot(2,2) * ac.z);
    return d;
}

QMatrix4x4 Mesh::multiplyMatrixes()
{
    QMatrix4x4 mat = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
    mat *= transX * transY * transZ *  rotX * rotY * rotZ * scalX * scalY * scalZ; // * shrX * shrY * shrZ;
    return mat;
}

void Mesh::translateX(int r)
{
    transX = QMatrix4x4(1,0,0,r,0,1,0,0,0,0,1,0,0,0,0,1);
}

void Mesh::translateY(int r)
{
    transY = QMatrix4x4(1,0,0,0,0,1,0,r,0,0,1,0,0,0,0,1);
}

void Mesh::translateZ(int r)
{
    transZ = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,r,0,0,0,1);
}

void Mesh::translate(int a, int b, int c)
{
    trans = QMatrix4x4(1,0,0,a,0,1,0,b,0,0,1,c,0,0,0,1);
}

void Mesh::scaleX(double s)
{
    scalX = QMatrix4x4(s,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
}

void Mesh::scaleY(double s)
{
    scalY = QMatrix4x4(1,0,0,0,0,s,0,0,0,0,1,0,0,0,0,1);
}

void Mesh::scaleZ(double s)
{
    scalZ = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,s,0,0,0,0,1);
}

void Mesh::scale(double s)
{
    scal = QMatrix4x4(s,0,0,0,0,s,0,0,0,0,s,0,0,0,0,1);
}

void Mesh::rotateX(int alfa)
{
    double rad;
    rad = alfa/360.0 * 2 * 3.141592;
    rotX = QMatrix4x4(1,0,0,0,0,cos(rad),-sin(rad),0,0,sin(rad),cos(rad),0,0,0,0,1);
}

void Mesh::rotateY(int alfa)
{
    double rad;
    rad = alfa/360.0 * 2 * 3.141592;
    rotY = QMatrix4x4(cos(rad),0,sin(rad),0,0,1,0,0,-sin(rad),0,cos(rad),0,0,0,0,1);
}

void Mesh::rotateZ(int alfa)
{
    double rad;
    rad = alfa/360.0 * 2 * 3.141592;
    rotZ = QMatrix4x4(cos(rad),-sin(rad),0,0,sin(rad),cos(rad),0,0,0,0,1,0,0,0,0,1);
}
