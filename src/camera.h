#ifndef CAMERA_H
#define CAMERA_H

#include <tools.h>

class Camera
{
public:
    double cX, cY, cZ;  // положение камеры
    double tX, tY, tZ;  //katy
    double eX, eY, eZ;  // позиция наблюдателя

    QMatrix4x4 rot;

public:
    Camera();
    void clear();
    void refresh();
    void setCameraLight(Point3D *lightSource);  // метод, который позиционирует источник света как камеру

    void rotateX(int alfa);
    void rotateY(int alfa);
    void rotateZ(int alfa);
};

#endif // CAMERA_H
