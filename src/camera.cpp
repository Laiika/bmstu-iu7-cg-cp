#include "camera.h"

Camera::Camera()
{
    // начальное положение камеры
    cX = 0;
    cY = -1000;
    cZ = 2500;

    tX = 15;
    tY = 0;
    tZ = 0;

    // начальная позиция наблюдателя
    eX = -300;
    eY = -300;
    eZ = -1500;
    clear();
}

void Camera::clear()
{}

void Camera::refresh()
{
    clear();
    double rad;
    QMatrix4x4 roX;
    QMatrix4x4 roY;
    QMatrix4x4 roZ;

    rad = tX / 360.0 * 2 * M_PI;
    roX = QMatrix4x4(1,0,0,0,0,cos(rad),sin(rad),0,0,-sin(rad),cos(rad),0,0,0,0,1);

    rad = tY / 360.0 * 2 * M_PI;
    roY = QMatrix4x4(cos(rad),0,-sin(rad),0,0,1,0,0,sin(rad),0,cos(rad),0,0,0,0,1);

    rad = tZ / 360.0 * 2 * M_PI;
    roZ = QMatrix4x4(cos(rad),sin(rad),0,0,-sin(rad),cos(rad),0,0,0,0,1,0,0,0,0,1);

    rot = roX * roY * roZ;
}

void Camera::setCameraLight(Point3D *lightSource)
{
    lightSource->x = cX;
    lightSource->y = cY;
    lightSource->z = cZ;
}

void Camera::rotateX(int alfa)
{
    double rad;
    rad = alfa/360.0 * 2 * 3.141592;
    QMatrix4x4 matrix = QMatrix4x4(1,0,0,0,0,cos(rad),-sin(rad),0,0,sin(rad),cos(rad),0,0,0,0,1);

    eX = matrix(0,0) * eX + matrix(0,1) * eY + matrix(0,2) * eZ + matrix(0,3);
    eY = matrix(1,0) * eX + matrix(1,1) * eY + matrix(1,2) * eZ + matrix(1,3);
    eZ = matrix(2,0) * eX + matrix(2,1) * eY + matrix(2,2) * eZ + matrix(2,3);

    cX = matrix(0,0) * cX + matrix(0,1) * cY + matrix(0,2) * cZ + matrix(0,3);
    cY = matrix(1,0) * cX + matrix(1,1) * cY + matrix(1,2) * cZ + matrix(1,3);
    cZ = matrix(2,0) * cX + matrix(2,1) * cY + matrix(2,2) * cZ + matrix(2,3);
}

void Camera::rotateY(int alfa)
{
    double rad;
    rad = alfa/360.0 * 2 * 3.141592;
    QMatrix4x4 matrix = QMatrix4x4(cos(rad),0,sin(rad),0,0,1,0,0,-sin(rad),0,cos(rad),0,0,0,0,1);

    Point3D d = Point3D(eX - cX, eY - cY, eZ - cZ);

    cX = matrix(0,0) * cX + matrix(0,1) * cY + matrix(0,2) * cZ + matrix(0,3);
    cZ = matrix(2,0) * cX + matrix(2,1) * cY + matrix(2,2) * cZ + matrix(2,3);

    eZ = (cZ + sin(rad) * d.x + cos(rad) * d.z);
    eX = (cX + cos(rad) * d.x - sin(rad) * d.z);
}

void Camera::rotateZ(int alfa)
{
    double rad;
    rad = alfa/360.0 * 2 * 3.141592;
    QMatrix4x4 matrix = QMatrix4x4(cos(rad),-sin(rad),0,0,sin(rad),cos(rad),0,0,0,0,1,0,0,0,0,1);

    eX = matrix(0,0) * eX + matrix(0,1) * eY + matrix(0,2) * eZ + matrix(0,3);
    eY = matrix(1,0) * eX + matrix(1,1) * eY + matrix(1,2) * eZ + matrix(1,3);
    eZ = matrix(2,0) * eX + matrix(2,1) * eY + matrix(2,2) * eZ + matrix(2,3);

    cX = matrix(0,0) * cX + matrix(0,1) * cY + matrix(0,2) * cZ + matrix(0,3);
    cY = matrix(1,0) * cX + matrix(1,1) * cY + matrix(1,2) * cZ + matrix(1,3);
    cZ = matrix(2,0) * cX + matrix(2,1) * cY + matrix(2,2) * cZ + matrix(2,3);
}
