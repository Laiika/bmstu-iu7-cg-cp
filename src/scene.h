#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <mesh.h>
#include <sphere.h>
#include <painter.h>
#include <algorithm>

class Scene
{
private:
    Camera* camera;
    Painter* painter;
    QImage* MainImage;

public:
    std::vector<Mesh*> Objects;
    // сферы
    Sphere *earth,*jowisz,*saturn,*uran;
    // стартовые (предыдущие) позиции сфер
    std::vector<Point3D> pos;

public:
    Scene();
    Scene(Painter *p, QImage* img);
    void refresh();
    void generateWorld();

    // функции, вносящие движение в систему
    void planetRound(Sphere *planet, int i, int step);
    void sputnikRound(Sphere *sputnik, Point3D &stPos);

    Camera *getCamera() const;
};

#endif // SCENE_H
