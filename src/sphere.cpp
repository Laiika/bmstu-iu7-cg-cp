#include "sphere.h"

double const G = 6e-18;

Sphere::Sphere(QImage* tex, int meridians, int parallels, double X, double Y, double Z, double size_, int t, double _m)
{
    meridian = meridians;
    parallel = parallels;
    size = size_;
    type = t;
    m = _m;
    center = Point3D(X, Y, Z);
    clear();
    texture = tex;
    // создание граней сетки сферы
    this->vertices.push_back(Point3D(0.0, 1.0, 0.0));
    for (long j = 0; j < parallels - 1; ++j)
    {
        double const polar = M_PI * double(j+1) / double(parallels);
        double const sp = std::sin(polar);
        double const cp = std::cos(polar);
        for (long i = 0; i < meridians; ++i)
        {
            double const azimuth = 2.0 * M_PI * double(i) / double(meridians);
            double const sa = std::sin(azimuth);
            double const ca = std::cos(azimuth);
            double const x = sp * ca;
            double const y = cp;
            double const z = sp * sa;
            this->vertices.push_back(Point3D(x, y, z));
        }
    }
    this->vertices.push_back(Point3D(0.0, -1.0, 0.0));

    // треугольники с вершиной в y_max
    for (long i = 0; i < meridians; ++i)
    {
        long const a = i + 1;
        long const b = (i + 1) % meridians + 1;
        this->addTriangle(0, b, a);
    }
    // все остальные треугольники по параллелям
    for (long j = 0; j < parallels - 2; ++j)
    {
        long aStart = j * meridians + 1;
        long bStart = (j + 1) * meridians + 1;
        for (long i = 0; i < meridians; ++i)
        {
            const long a = aStart + i;
            const long a1 = aStart + (i + 1) % meridians;
            const long b = bStart + i;
            const long b1 = bStart + (i + 1) % meridians;
            this->addQuad(a, a1, b1, b);
        }
    }
    // треугольники с вершиной в y_min
    for (long i = 0; i < meridians; ++i)
    {
        long const a = i + meridians * (parallels - 2) + 1;
        long const b = (i + 1) % meridians + meridians * (parallels - 2) + 1;
        this->addTriangle(this->vertices.size() - 1, a, b);
    }
    this->translateX(X);
    this->translateY(Y);
    this->translateZ(Z);
    this->scale(size);
    this->rotateX(180);
    this->refresh();

    initializeTexturePoints();
}

void Sphere::initializeTexturePoints()
{
    txtPs.clear();
    txtI = 0;

    int w = texture->width()/meridian;
    int h = texture->height()/parallel;

    for(int i = 0; i < meridian; i++)
    {
        txtPs.push_back(QPoint(texture->width()/2,0));
        txtPs.push_back(QPoint((i+1)*w,h));
        txtPs.push_back(QPoint(i*w,h));
    }

    for(int i = 1; i < parallel-1; i++)
    {
        for(int j = 0; j < meridian; j++)
        {
            txtPs.push_back(QPoint(j * w,i *h));
            txtPs.push_back(QPoint((j+1) * w,i * h));
            txtPs.push_back(QPoint(j* w,(i + 1) *h));

            txtPs.push_back(QPoint((j + 1) * w,(i + 1) *h));
            txtPs.push_back(QPoint(j * w, (i+1)*h));
            txtPs.push_back(QPoint(j * w,i *h));
        }
    }
    for(int i = 0; i < meridian; i++)
    {
        txtPs.push_back(QPoint(texture->width()/2,texture->height()));
        txtPs.push_back(QPoint(i*w,texture->height()-h));
        txtPs.push_back(QPoint((i+1)*w,texture->height()-h));
    }
}

void Sphere::set_rot(double aX, double aY, double aZ)
{
    rot = generateRotMat(aX,aY,aZ);
}

void Sphere::calc_trajectory(Point3D& s2, Point3D& vs1, Point3D& vs2, double m2)
{
    Point3D fs, es, p, s1 = center;
    double m1 = m;
    double rasts = sqrt((s1.x - s2.x) * (s1.x - s2.x) + (s1.z - s2.z) * (s1.z - s2.z));
    double dt = 0.025;
    points.clear();

    double xp2 = s2.x, xp1 = s1.x;
    double zp2 = s2.z, zp1 = s1.z;
    double xc1 = xp1 + vs1.x * dt - (dt * dt * 0.5 * G * m2 * (s1.x - s2.x)) / (rasts * rasts * rasts);
    double xc2 = xp2 + vs2.x * dt + (dt * dt * 0.5 * G * m1 * (s1.x - s2.x)) / (rasts * rasts * rasts);
    double zc1 = zp1 + vs1.z * dt - (dt * dt * 0.5 * G * m2 * (s1.z - s2.z)) / (rasts * rasts * rasts);
    double zc2 = zp2 + vs2.z * dt + (dt * dt * 0.5 * G * m1 * (s1.z - s2.z)) / (rasts * rasts * rasts);

    s2.x = xc2;
    s2.z = zc2;
    s1.x = xc1;
    s1.z = zc1;

    for (int i = 0; i < 207; i++)
    {
        points.push_back(s1);
        //сила гравитационного взаимодействия между объектами
        rasts =  sqrt((s1.x - s2.x) * (s1.x - s2.x) + (s1.z - s2.z) * (s1.z- s2.z));//расстояние между звездами

        es.x = (s1.x - s2.x) / rasts;//косинус угла между вектором соединячющим две звезды и осью ox
        es.z = (s1.z - s2.z) / rasts;//синус угла между вектором соединяющим две звезды и осью ox

        fs.x = (G * m1 * m2 * es.x) / (rasts * rasts);//тут нет второй массы потому что при вычислении ускорения там делится на массу поэтому я в следующих формулах не делила
        fs.z = (G * m1 * m2 * es.z) / (rasts * rasts);//проекция  на ось y

        //тот же метод для координат, тут для звезды 1
        s1.x = 2 * xc1 - xp1 - (fs.x) * dt * dt / m1;
        s1.z = 2 * zc1 - zp1 - (fs.z) * dt * dt / m1;
        xp1 = xc1;
        xc1 = s1.x;
        zp1 = zc1;
        zc1 = s1.z;
        //координаты звезды 2
        s2.x = 2 * xc2 - xp2 + (fs.x) * dt * dt / m2;
        s2.z = 2 * zc2 - zp2 + (fs.z) * dt * dt / m2;
        xp2 = xc2;
        xc2 = s2.x;
        zp2 = zc2;
        zc2 = s2.z;
    }
    cur = 0;
}
