#include "scene.h"

Camera *Scene::getCamera() const
{
    return camera;
}

Scene::Scene()
{
    camera = new Camera();
}

Scene::Scene(Painter *p, QImage *img)
{
    camera = new Camera();
    painter = p;
    MainImage = img;
}

bool cmp(Mesh* a, Mesh* b)
{
    return a->distanceToCamera <  b->distanceToCamera;
}

void Scene::refresh()
{
    camera->refresh();
    painter->clearBuffer();
    painter->star = saturn->center;

    // сброс настроек камеры
    camera->clear();

    MainImage->fill(QColor(0,0,0,255));
    MainImage->load(":/images/sky.jpg");
    *MainImage = MainImage->scaled(QSize(900, 900), Qt::IgnoreAspectRatio, Qt::FastTransformation);
    for(int i = 0; i < Objects.size(); i++)
    {
        Objects[i]->mapTo2D(camera);
        painter->drawMesh(*Objects[i], MainImage, Objects[i]->texture, camera, pos[i]);
        Objects[i]->txtI = 0;
    }
    camera->setCameraLight(painter->getLightSource());
}

void Scene::generateWorld()
{
    int rou = 18;
    Point3D vs1 = Point3D(0, 0, 1.1);
    Point3D vs2 = Point3D(0, 0, -1.32231);
    Point3D earthPos, jowPos;
    // Создание всех сфер системы

    QImage* EarTex = new QImage(900, 900, QImage::Format_ARGB32);
    EarTex->load(":/images/sun.jpg");
    *EarTex = EarTex->scaled(QSize(900, 900), Qt::IgnoreAspectRatio, Qt::FastTransformation);
    earthPos = Point3D(-1.5,0,0);
    earth = new Sphere(EarTex,rou, rou,earthPos.x,earthPos.y,earthPos.z,37, 1, 24e+17);
    //earth->set_rot(0.0, 15, 0.0);
    Objects.push_back(earth);

    QImage* JowTex = new QImage(900, 900, QImage::Format_ARGB32);
    JowTex->load(":/images/mars.jpg");
    *JowTex = JowTex->scaled(QSize(900, 900), Qt::IgnoreAspectRatio, Qt::FastTransformation);
    jowPos = Point3D(1.8,0,0);
    jowisz = new Sphere(JowTex,rou, rou,jowPos.x,jowPos.y,jowPos.z,35, 1, 20e+17);
    //jowisz->set_rot(0.0, 15, 0.0);
    Objects.push_back(jowisz);

    earth->calc_trajectory(jowPos, vs1, vs2, jowisz->m);
    jowisz->calc_trajectory(earthPos, vs2, vs1, earth->m);
    earth->center = Point3D(-150,0,0);
    jowisz->center = Point3D(180,0,0);
    pos.push_back(earthPos);
    pos.push_back(jowPos);

    Point3D p;
    for (int i = 0; i < 207; i++)
    {
        earth->points[i].x *= 150;
        earth->points[i].z *= 150;
        jowisz->points[i].x *= 150;
        jowisz->points[i].z *= 150;
    }

    QImage* SatTex = new QImage(900, 900, QImage::Format_ARGB32);
    SatTex->load(":/images/saturn.jpg");
    *SatTex = SatTex->scaled(QSize(900, 900), Qt::IgnoreAspectRatio, Qt::FastTransformation);
    p = Point3D(-earth->points[140].x * 5, earth->points[140].y, earth->points[140].z * 5);
    saturn = new Sphere(SatTex,rou, rou,p.x,p.y,p.z,32, 1, 0);
    //saturn->set_rot(0, -1, 0);

    for (int i = 0; i < 207; i++)
    {
        p = Point3D(-earth->points[i].x * 5, earth->points[i].y, earth->points[i].z * 5);
        saturn->points.push_back(p);
    }

    pos.push_back(p);
    saturn->center = p;
    saturn->cur = 140;
    Objects.push_back(saturn);

    QImage* UraTex = new QImage(900, 900, QImage::Format_ARGB32);
    UraTex->load(":/images/mercury.jpg");
    *UraTex = UraTex->scaled(QSize(900, 900), Qt::IgnoreAspectRatio, Qt::FastTransformation);
    pos.push_back(p);
    uran = new Sphere(UraTex,rou, rou,p.x,p.y,p.z,25, 0, 0);
    uran->center = Point3D(0,0,-620);
    uran->set_rot(0, 14, 0);
    Objects.push_back(uran);

    refresh();
}

void Scene::planetRound(Sphere *planet, int i, int step)
{
    /*
    if (i >= 2)
    {
        QMatrix4x4 rot = planet->rot;
        stPos = Point3D(stPos.x * rot(0,0) +stPos.y * rot(0,1) +stPos.z * rot(0,2),stPos.x * rot(1,0) +stPos.y * rot(1,1) +stPos.z * rot(1,2), stPos.x * rot(2,0) +stPos.y * rot(2,1) +stPos.z * rot(2,2));
    }
    else
    {*/
        if (planet->cur >= 207)
            planet->cur = 0;

        planet->center = planet->points[planet->cur];
        planet->cur = planet->cur + step;

    pos[i] = planet->center;
    planet->translate(planet->center.x, planet->center.y, planet->center.z);
    planet->refresh_trans();
}

void Scene::sputnikRound(Sphere *sputnik, Point3D &stPos)
{
    QMatrix4x4 rot = sputnik->rot;
    Point3D spPos = sputnik->center;
    Point3D spPos2 = Point3D(spPos.x * rot(0,0) +spPos.y * rot(0,1) +spPos.z * rot(0,2),spPos.x * rot(1,0) +spPos.y * rot(1,1) +spPos.z * rot(1,2), spPos.x * rot(2,0) +spPos.y * rot(2,1) +spPos.z * rot(2,2));
    //stPos = Point3D(stPos.x * rot2(0,0) +stPos.y * rot2(0,1) +stPos.z * rot2(0,2),stPos.x * rot2(1,0) +stPos.y * rot2(1,1) +stPos.z * rot2(1,2), stPos.x * rot2(2,0) +stPos.y * rot2(2,1) +stPos.z * rot2(2,2));

    sputnik->translate(stPos.x + spPos2.x - spPos.x, stPos.y, stPos.z + spPos2.z - spPos.z);
    pos[3] = Point3D(stPos.x + spPos2.x - spPos.x, stPos.y, stPos.z + spPos2.z - spPos.z);
    sputnik->refresh_trans();
    sputnik->center = spPos2;
}
