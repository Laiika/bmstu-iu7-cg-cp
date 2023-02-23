#include "painter.h"

Point3D *Painter::getLightSource()
{
    return &lightSource;
}

void Painter::clearBuffer()
{
    for(int i = 0; i < 900; i++){
        for(int j = 0; j < 900; j++){
            zBuffer[i][j] = std::numeric_limits<int>::min();
        }
    }
}

Painter::Painter():
    zBuffer(std::vector<std::vector<int>>(900, std::vector<int>(900, 0)))
{}

Painter::Painter(int w, int h, Camera *cam):
    zBuffer(std::vector<std::vector<int>>(900, std::vector<int>(900, 0)))
{
    width = w;
    height = h;
    lightSource = Point3D(0,0,0);
    camera = cam;
    clearBuffer();
}


bool is_visible(const QPoint p, QImage *img)
{
    if (p.x() < 0 || p.x() > img->width() || p.y() < 0 || p.y() > img->height())
        return false;

    return true;
}

void Painter::drawMesh(Mesh &mesh, QImage *img, QImage *texture, Camera* camera, Point3D p)
{
    for(int i = 0; i < mesh.triangles.size(); i+=3)
    {
        int idx0 = mesh.triangles[i];
        int idx1 = mesh.triangles[i+1];
        int idx2 = mesh.triangles[i+2];
        QPoint p1 = mesh.points[idx0];
        QPoint p2 = mesh.points[idx1];
        QPoint p3 = mesh.points[idx2];

        Point3D p3d_0 = mesh.countCamera3DPoint(mesh.Tnodes[idx0], camera);
        Point3D p3d_1 = mesh.countCamera3DPoint(mesh.Tnodes[idx1], camera);
        Point3D p3d_2 = mesh.countCamera3DPoint(mesh.Tnodes[idx2], camera);

        Point3D v1 = p3d_1 - p3d_0;
        Point3D v2 = p3d_2 - p3d_0;

        Point3D Na = v1.cross(v2);
        // косинус угла между вектором камеры и вектором нормали треугольника
        double scalarTriCam = (Na.x * p3d_0.x + Na.y * p3d_0.y + Na.z * p3d_0.z)/(sqrt(Na.x * Na.x + Na.y * Na.y + Na.z * Na.z) * sqrt(p3d_0.x * p3d_0.x + p3d_0.y * p3d_0.y + p3d_0.z * p3d_0.z));


        std::vector<QPoint> tex;
        tex.push_back(mesh.txtPs[mesh.txtI]);
        tex.push_back(mesh.txtPs[mesh.txtI + 1]);
        tex.push_back(mesh.txtPs[mesh.txtI + 2]);
        if(mesh.txtI + 3 >= mesh.txtPs.size())
        {
            mesh.txtI = 0;
        }
        else
        {
            mesh.txtI += 3;
        }

        if (!is_visible(p1, img) || !is_visible(p2, img) || !is_visible(p3, img))
            continue;

        if(scalarTriCam < 0 && (p3d_0.z <= 0 || p3d_1.z <= 0 || p3d_2.z <= 0))
        {
            //if(wn.z > 0){
            std::vector<QPoint> pts;
            pts.push_back(p1);
            pts.push_back(p2);
            pts.push_back(p3);

            double light = 0;
            // Плоское затенение, свет подсчитывается один раз для каждого треугольника
            Point3D Na = mesh.countNormalVector(idx0, idx1, idx2);
            light = countLightIntense(Na,Point3D((mesh.Tnodes[idx0].x + mesh.Tnodes[idx1].x + mesh.Tnodes[idx2].x)/3, (mesh.Tnodes[idx0].y + mesh.Tnodes[idx1].y + mesh.Tnodes[idx2].y)/3,(mesh.Tnodes[idx0].z + mesh.Tnodes[idx1].z + mesh.Tnodes[idx2].z)/3));
            //texture mesh

            if (mesh.type == 1)
                textureTriangle(mesh, idx0, idx1, idx2, pts,tex,texture, img,light);
            else
                textureSpTriangle(mesh, idx0, idx1, idx2, pts,tex,texture, img,light, p);

            pts.clear();
        }
    }
}


double Painter::countLightIntense(Point3D N, Point3D point)
{
    // подсчет интенсивности света
    double I;   // главный свет
    double Ia, Ip;
    double Ka, Ks, Kd;
    double Fatt;    // рассеяние света
    Point3D L;  // вектор к источнику света

    L = Point3D(lightSource.x - point.x,lightSource.y - point.y,lightSource.z/100 - point.z/100);
    double Nol = N.x * L.x + N.y * L.y + N.z * L.z; // скаляр векторов N&L
    Fatt = 0.001;
    Ip = 100;
    Kd = 0.65;

    I = Fatt * Ip * Kd * Nol;
    return -I;
}

double Painter::countStarLightIntense(Point3D N, Point3D point)
{
    // подсчет интенсивности света
    double I;   // главный свет
    double Ia, Ip;
    double Ka, Ks, Kd;
    double Fatt;    // рассеяние света
    Point3D L;  // вектор к источнику света

    L = Point3D((star.x - point.x) * 100, (star.y - point.y) * 100, (star.z - point.z) * 100);
    double Nol = N.x * L.x + N.y * L.y + N.z * L.z; // скаляр векторов N&L
    Fatt = 0.001;
    Ip = 300;
    Kd = 0.65;

    I = Ip * Kd * Nol;
    /*
    L = Point3D((0 - point.x), (0 - point.y), (0 - point.z));
    Nol = N.x * L.x + N.y * L.y + N.z * L.z; // скаляр векторов N&L
    I += Ip * Kd * Nol;*/
    return -I;
}

double minim(double a, double b, double c)
{
    double min = a;
    if(b < min) min = b;
    if(c < min) min = c;
    return min;
}

double maxim(double a, double b, double c)
{
    double maxi  = a;
    if(b > maxi) maxi = b;
    if(c > maxi) maxi = c;
    return maxi;
}

// Текстурирует треугольник со светом
void Painter::textureTriangle(Mesh mesh, int a, int b, int c, std::vector<QPoint> points, std::vector<QPoint> texturePoints, QImage *img, QImage *img2, double light)
{
    double u,v,w;
    QColor kolor;

    double Xa = points[0].x();
    double Xb = points[1].x();
    double Xc = points[2].x();

    double Ya = points[0].y();
    double Yb = points[1].y();
    double Yc = points[2].y();

    double div = (Xb - Xa)  * (Yc - Ya) - (Yb - Ya) * (Xc - Xa);
    if(div == 0.0)
    {
        return;
    }

    // Вектор нормали визуализируемой треугольной грани
    Point3D wall = mesh.countNormalVector(mesh.Tnodes[a],mesh.Tnodes[b], mesh.Tnodes[c]);

    int xmin = (int)minim(Xa, Xb, Xc), ymin = (int)minim(Ya, Yb, Yc);
    int xmax = (int)maxim(Xa, Xb, Xc), ymax = (int)maxim(Ya, Yb, Yc);
    for(int x = xmin; x < xmax; x++)
    {
        for(int y = ymin; y < ymax; y++)
        {
            double Z = (-wall.x * (x - mesh.Tnodes[a].x) + -wall.y * (y - mesh.Tnodes[a].y))/wall.z + mesh.Tnodes[a].z;

            if (zBuffer[x][y] < Z)
            {
                v = ((x - Xa) * (Yc - Ya) - (y - Ya) * (Xc - Xa))/div;
                w = ((Xb - Xa) * (y - Ya) - (Yb - Ya) * (x - Xa))/div;
                u = 1.0 - v - w;
                if(u < 0 || u > 1 || v < 0 || v > 1 || w < 0 || w > 1) continue;

                double xT = u * texturePoints[0].x() + v * texturePoints[1].x() + w * texturePoints[2].x();
                double yT = u * texturePoints[0].y() + v * texturePoints[1].y() + w * texturePoints[2].y();

                if(yT >= img->height()) yT = img->height() -1;
                if(yT < 0) yT = 0;
                if(xT >= img->width()) xT = img->width() -1;
                if(xT < 0) xT = 0;

                QRgb rgb = img->pixel(xT, yT);
                kolor = QColor(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb));

                int r = (int)(kolor.red() - light);
                int g = (int)(kolor.green() - light);
                int bb = (int)(kolor.blue() - light);

                if(r > 255) r = 255;
                if(r < 0) r = 0;
                if(g > 255) g = 255;
                if(g < 0) g = 0;
                if(bb > 255) bb = 255;
                if(bb < 0) bb = 0;

                setPixel(img2, x, y, r, g, bb);
                zBuffer[x][y] = Z;
            }
        }
    }
}

void Painter::textureSpTriangle(Mesh mesh, int a, int b, int c, std::vector<QPoint> points, std::vector<QPoint> texturePoints, QImage *img, QImage *img2, double light, Point3D p)
{
    double u,v,w;
    QColor kolor;

    double Xa = points[0].x();
    double Xb = points[1].x();
    double Xc = points[2].x();

    double Ya = points[0].y();
    double Yb = points[1].y();
    double Yc = points[2].y();

    double div = (Xb - Xa)  * (Yc - Ya) - (Yb - Ya) * (Xc - Xa);
    if(div == 0.0)
    {
        return;
    }

    //подсчитываем вектора нормалей к вершинам a, b, c для затенения Гуро
    Point3D Na = Point3D(mesh.Tnodes[a].x - p.x, mesh.Tnodes[a].y - p.y, mesh.Tnodes[a].z - p.z);
    Point3D Nb = Point3D(mesh.Tnodes[b].x - p.x, mesh.Tnodes[b].y - p.y, mesh.Tnodes[b].z - p.z);
    Point3D Nc = Point3D(mesh.Tnodes[c].x - p.x, mesh.Tnodes[c].y - p.y, mesh.Tnodes[c].z - p.z);

    Point3D pa = mesh.Tnodes[a];
    Point3D pb = mesh.Tnodes[b];
    Point3D pc = mesh.Tnodes[c];

    double Ia = countStarLightIntense(Na, pa);
    double Ib = countStarLightIntense(Nb, pb);
    double Ic = countStarLightIntense(Nc, pc);

    // Вектор нормали визуализируемой треугольной грани
    Point3D wall = mesh.countNormalVector(mesh.Tnodes[a],mesh.Tnodes[b], mesh.Tnodes[c]);

    int xmin = (int)minim(Xa, Xb, Xc), ymin = (int)minim(Ya, Yb, Yc);
    int xmax = (int)maxim(Xa, Xb, Xc), ymax = (int)maxim(Ya, Yb, Yc);
    for(int x = xmin; x < xmax; x++)
    {
        for(int y = ymin; y < ymax; y++)
        {
            double Z = (-wall.x * (x - mesh.Tnodes[a].x) + -wall.y * (y - mesh.Tnodes[a].y))/wall.z + mesh.Tnodes[a].z;

            if (zBuffer[x][y] < Z)
            {
                v = ((x - Xa) * (Yc - Ya) - (y - Ya) * (Xc - Xa))/div;
                w = ((Xb - Xa) * (y - Ya) - (Yb - Ya) * (x - Xa))/div;
                u = 1.0 - v - w;
                if(u < 0 || u > 1 || v < 0 || v > 1 || w < 0 || w > 1) continue;

                double xT = u * texturePoints[0].x() + v * texturePoints[1].x() + w * texturePoints[2].x();
                double yT = u * texturePoints[0].y() + v * texturePoints[1].y() + w * texturePoints[2].y();

                if(yT >= img->height()) yT = img->height() -1;
                if(yT < 0) yT = 0;
                if(xT >= img->width()) xT = img->width() -1;
                if(xT < 0) xT = 0;


                QRgb rgb = img->pixel(xT, yT);
                kolor = QColor(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb));

                //Guro Shading
                //*******!!!***********
                double x1, x2, x3;
                double div = (mesh.points[b].y() - mesh.points[a].y());
                if(div == 0.0){
                    x1 = mesh.points[a].x();
                }else{
                    x1 = ((y - mesh.points[a].y())*(mesh.points[b].x() - mesh.points[a].x()) + (mesh.points[b].y() - mesh.points[a].y()) * x)/div;
                }
                div = (mesh.points[c].y() - mesh.points[b].y());
                if(div == 0.0){
                    x2 = mesh.points[b].x();
                }else{
                    x2 = ((y - mesh.points[b].y())*(mesh.points[c].x() - mesh.points[b].x()) + (mesh.points[c].y() - mesh.points[b].y()) * x)/div;
                }
                div = (mesh.points[a].y() - mesh.points[c].y());
                if(div == 0.0){
                    x3 = mesh.points[c].x();
                }else{
                    x3 = ((y - mesh.points[c].y())*(mesh.points[a].x() - mesh.points[c].x()) + (mesh.points[a].y() - mesh.points[c].y()) * x)/div;
                }
                double xMin, xMax;
                xMin = minim(mesh.points[a].x(), mesh.points[b].x(), mesh.points[c].x());
                xMax = maxim(mesh.points[a].x(), mesh.points[b].x(), mesh.points[c].x());

                double I = -0.001;
                QPoint A = mesh.points[a];
                QPoint B = mesh.points[b];
                QPoint C = mesh.points[c];
                if(x1 >= xMin && x1 <= xMax && x2 >= xMin && x2 <= xMax)
                {
                    if(x1 <= x2)
                    {
                        I = guroShading(B,A,C, Ib, Ia, Ic, x, y, x1,x2);
                    }
                    else
                    {
                        I = guroShading(B,C,A, Ib, Ic, Ia, x, y, x2,x1);
                    }
                }
                else if(x1 >= xMin && x1 <= xMax && x3 >= xMin && x3 <= xMax)
                {
                    if(x1 <= x3)
                    {
                        I = guroShading(A,B,C, Ia, Ib, Ic, x, y, x1,x3);
                    }
                    else
                    {
                        I = guroShading(A,C,B, Ia, Ic, Ib, x, y, x3,x1);
                    }
                }
                else if(x2 >= xMin && x2 <= xMax && x3 >= xMin && x3 <= xMax)
                {
                    if(x2 <= x3)
                    {
                        I = guroShading(C,B,A, Ic, Ib, Ia, x, y, x2,x3);
                    }
                    else
                    {
                        I = guroShading(C,A,B, Ic, Ia, Ib, x, y, x3,x2);
                    }
                }
                Point3D wall = mesh.countNormalVector(mesh.triangles[a],mesh.triangles[b], mesh.triangles[c]);

                light = I;
                setColor(x, y, kolor, light, img2);
                zBuffer[x][y] = Z;
            }
        }
    }
}

void Painter::setColor(int x, int y, QColor kolor, double light, QImage *img)
{
    int r = (int)(kolor.red() - light);
    int g = (int)(kolor.green() - light);
    int bb = (int)(kolor.blue() - light);

    if(r > 255) r = 200;
    if(r < 0) r = 100;
    if(g > 255) g = 200;
    if(g < 0) g = 100;
    if(bb > 255) bb = 200;
    if(bb < 0) bb = 100;

    setPixel(img, x, y, r, g, bb);
}

/*
void Painter::textureSpTriangle(Mesh mesh, int a, int b, int c, std::vector<QPoint> points, std::vector<QPoint> texturePoints, QImage *img, QImage *img2, double light, Point3D p)
{
    double u,v,w;
    QColor kolor;

    double Xa = points[0].x();
    double Xb = points[1].x();
    double Xc = points[2].x();

    double Ya = points[0].y();
    double Yb = points[1].y();
    double Yc = points[2].y();

    double div = (Xb - Xa)  * (Yc - Ya) - (Yb - Ya) * (Xc - Xa);
    if(div == 0.0)
    {
        return;
    }

    //подсчитываем вектора нормалей к вершинам a, b, c для затенения Гуро
    Point3D Na = Point3D(mesh.Tnodes[a].x - p.x, mesh.Tnodes[a].y - p.y, mesh.Tnodes[a].z - p.z);
    Point3D Nb = Point3D(mesh.Tnodes[b].x - p.x, mesh.Tnodes[b].y - p.y, mesh.Tnodes[b].z - p.z);
    Point3D Nc = Point3D(mesh.Tnodes[c].x - p.x, mesh.Tnodes[c].y - p.y, mesh.Tnodes[c].z - p.z);

    Point3D pa = mesh.Tnodes[a];
    Point3D pb = mesh.Tnodes[b];
    Point3D pc = mesh.Tnodes[c];

    double Ia = countStarLightIntense(Na, pa);
    double Ib = countStarLightIntense(Nb, pb);
    double Ic = countStarLightIntense(Nc, pc);

    // Вектор нормали визуализируемой треугольной грани
    Point3D wall = mesh.countNormalVector(mesh.Tnodes[a],mesh.Tnodes[b], mesh.Tnodes[c]);

    QPoint A = mesh.points[a];
    QPoint B = mesh.points[b];
    QPoint C = mesh.points[c];

    if (A.y() > B.y())
    {
        std::swap(A, B);
        std::swap(Ia, Ib);
    }
    if (A.y() > C.y())
    {
        std::swap(A, C);
        std::swap(Ia, Ic);
    }
    if (B.y() > C.y())
    {
        std::swap(B, C);
        std::swap(Ib, Ic);
    }

    int xmin = (int)minim(Xa, Xb, Xc), ymin = (int)minim(Ya, Yb, Yc);
    int xmax = (int)maxim(Xa, Xb, Xc), ymax = (int)maxim(Ya, Yb, Yc);
    for(int y = ymin; y < ymax; y++)
    {
        bool half2 = (y - ymin) > B.y() - A.y() || B.y() == A.y();
        int h = half2 ? C.y() - B.y() : B.y() - A.y();

        float alpha = (float)(y - ymin) / (ymax - ymin);
        float betta = (float)(y - ymin - (half2 ? B.y() - A.y() : 0)) / h;

        QPoint minp = A + (C - A) * alpha;
        QPoint maxp = half2 ? B + (C - B) * betta : A + (B - A) * betta;

        double Id, If;
        Id = Ia + (Ic - Ia) * alpha;
        If = half2 ? Ib + (Ic - Ib) * betta : Ia + (Ib - Ia) * betta;

        if (minp.x() > maxp.x())
        {
            std::swap(minp, maxp);
            std::swap(Id, If);
        }

        minp.setX(std::max(xmin, minp.x()));
        maxp.setX(std::min(xmax, maxp.x()));

        int d = maxp.x() - minp.x();
        int f = minp.x(), l = maxp.x();

        for(int x = f; x < l; x++)
        {
            double Z = (-wall.x * (x - mesh.Tnodes[a].x) + -wall.y * (y - mesh.Tnodes[a].y))/wall.z + mesh.Tnodes[a].z;

            if (zBuffer[x][y] < Z)
            {
                v = ((x - Xa) * (Yc - Ya) - (y - Ya) * (Xc - Xa))/div;
                w = ((Xb - Xa) * (y - Ya) - (Yb - Ya) * (x - Xa))/div;
                u = 1.0 - v - w;
                if(u < 0 || u > 1 || v < 0 || v > 1 || w < 0 || w > 1) continue;

                double xT = u * texturePoints[0].x() + v * texturePoints[1].x() + w * texturePoints[2].x();
                double yT = u * texturePoints[0].y() + v * texturePoints[1].y() + w * texturePoints[2].y();

                if(yT >= img->height()) yT = img->height() -1;
                if(yT < 0) yT = 0;
                if(xT >= img->width()) xT = img->width() -1;
                if(xT < 0) xT = 0;


                QRgb rgb = img->pixel(xT, yT);
                kolor = QColor(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb));

                //Guro Shading
                //*******!!!***********
                float phi = l == f ? 1. : (float)(x - f) / (float)(l - f);
                double I = Id + (If - Id) * phi;

                light = I;
                setColor(x, y, kolor, light, img2);
                zBuffer[x][y] = Z;
            }
        }
    }
}
*/
double Painter::guroShading(QPoint A, QPoint B, QPoint C, double Ia, double Ib, double Ic, double x, double y, double x1, double x2)
{
    double Id, If, Ie;

    int div = (B.y() - A.y());
    if(div == 0)
    {
        Id = Ia;
    }
    else
    {
        Id = Ia * ((B.y() - y)/div) + Ib * ((y - A.y())/div);
    }
    div = C.y() - A.y();
    if(div == 0)
    {
        If = Ic;
    }
    else
    {
        If = Ia * ((C.y() - y)/div) + Ic * ((y - A.y())/div);
    }
    div = x2 - x1;
    if(div == 0)
    {
        Ie = Id;
    }
    else
    {
        Ie = Id * ((x2 - x)/div) + If * ((x-x1)/div);
    }
    return Ie;
}

void Painter::drawMesh(Mesh &mesh, QImage *img)
{
    for(int i = 0; i < mesh.triangles.size(); i+=3)
    {
        int idx0 = mesh.triangles[i];
        int idx1 = mesh.triangles[i+1];
        int idx2 = mesh.triangles[i+2];
        QPoint p1 = mesh.points[idx0];
        QPoint p2 = mesh.points[idx1];
        QPoint p3 = mesh.points[idx2];

        Point3D Na = mesh.countNormalVector(idx0, idx1, idx2);

        if(Na.z > 0)
        {
            // отрисовка сетки треугольников
            drawLine(p1.x(), p1.y(), p2.x(), p2.y(), img);
            drawLine(p1.x(), p1.y(), p3.x(), p3.y(), img);
            drawLine(p2.x(), p2.y(), p3.x(), p3.y(), img);
        }
    }
}


void Painter::textureTriangle(std::vector<QPoint> points, std::vector<QPoint> texturePoints, QImage *img, QImage *img2, double light)
{
    double u,v,w;
    QColor kolor;

    double Xa = points[0].x();
    double Xb = points[1].x();
    double Xc = points[2].x();

    double Ya = points[0].y();
    double Yb = points[1].y();
    double Yc = points[2].y();

    double div = (Xb - Xa)  * (Yc - Ya) - (Yb- Ya) * (Xc - Xa);

    for(int x = 0; x < img2->width(); x++)
    {
        for(int y = 0; y < img2->height(); y++)
        {
            v = ((x - Xa) * (Yc - Ya) - (y - Ya) * (Xc - Xa))/div;
            w = ((Xb - Xa) * (y - Ya) - (Yb - Ya) * (x - Xa))/div;
            u = 1.0 - v - w;
            if(u < 0 || u > 1 || v < 0 || v > 1 || w < 0 || w > 1) continue;

            double xT = u * texturePoints[0].x() + v * texturePoints[1].x() + w * texturePoints[2].x();
            double yT = u * texturePoints[0].y() + v * texturePoints[1].y() + w * texturePoints[2].y();

            if(yT >= height) yT = height -1;
            if(yT < 0) yT = 0;
            if(xT >= width) xT = width -1;
            if(xT < 0) xT = 0;

            // без интерполяции

            if(yT >= height) yT = height -1;
            if(yT < 0) yT = 0;
            if(xT >= width) xT = width -1;
            if(xT < 0) xT = 0;
            QRgb rgb = img->pixel(xT, yT);
            kolor = QColor(qRed(rgb), qGreen(rgb), qBlue(rgb), qAlpha(rgb));
            int r = (int)(kolor.red() + light);
            int g = (int)(kolor.green() + light);
            int b = (int)(kolor.blue() + light);

            if(r > 255) r= 255;
            if(r < 0) r = 0;
            if(g > 255) g= 255;
            if(g < 0) g = 0;
            if(b > 255) b= 255;
            if(b < 0) b = 0;

            setPixel(img2, x, y, r, g, b);
        }
    }
}


void Painter::drawLine(int x0, int y0, int x1, int y1, QImage *img)
{
    float a, b;

    //if(x0 < 0 || x0 > width || x1 < 0 || x1 > width) return;
    //if(y0 < 0 || y0 > height || y1 < 0 || y1 > height) return;

    if(x0 < 0) x0 = 0;
    if(x0 >= width) x0 = width-1;

    if(x1 < 0) x1 = 0;
    if(x1 >= width) x1 = width-1;

    if(y0 < 0) y0 = 0;
    if(y0 >= width) y0 = height-1;

    if(y1 < 0) y1 = 0;
    if(y1 >= width) y1 = height-1;

    if(x0 == x1 )
    {
        a = 0;
        b = y1;
    }
    else if(y0 == y1)
    {
        b = y1;
        a = 0;
    }
    else
    {
        a = (float)(y1 - y0)/(x1 -x0);
        b = (float)(y0 - a * x0);
    }
    if(x0 < x1)
    {
        if(abs(a) < 1)
        {
            for(int x = x0; x <=x1; x++)
            {
                int y = a * x + b;
                y = (int)floor(y + 0.5f);

                setPixel(img, x, y, 0,0,0);
            }
        }
        else if(abs(a) >= 1 && y0 <= y1 )
        {
            for(int y = y0; y <=y1; y++)
            {
                int x = (y - b)/a;
                x = (int)floor(x + 0.5f);

                setPixel(img, x, y, 0,0,0);
            }
        }
        else
        {
            for(int y = y1; y <=y0; y++)
            {
                int x = (y - b)/a;
                x = (int)floor(x + 0.5f);

                setPixel(img, x, y, 0,0,0);
            }
        }
    }
    else if(x0 > x1)
    {
        if(abs(a) < 1)
        {
            for(int x = x1; x <=x0; x++)
            {
                int y = a * x + b;
                y = (int)floor(y + 0.5f);

                setPixel(img, x, y, 0,0,0);
            }
        }
        else if(abs(a) >= 1 && y1 <= y0)
        {
            for(int y = y1; y <=y0; y++)
            {
                int x = (y - b)/a;
                x = (int)floor(x + 0.5f);

                setPixel(img, x, y, 0,0,0);
            }

        }
        else
        {
            for(int y = y0; y <=y1; y++)
            {
                int x = (y - b)/a;
                x = (int)floor(x + 0.5f);

                setPixel(img, x, y, 0,0,0);
            }
        }
    }
    else if(x0 == x1 && y0 < y1)
    {
        for(int y = y0; y <=y1; y++)
        {
            int x;
            x= x0;
            setPixel(img, x, y, 0,0,0);
        }
    }
    else
    {
        for(int y = y1; y <=y0; y++)
        {
            int x;

            x= x0;
            setPixel(img, x, y, 0,0,0);
        }
    }
}

void Painter::setPixel(QImage *img, int x, int y, short red, short green, short blue)
{
    if(x > img->width() || x < 0 || y > img->height() || y < 0) return;
    unsigned char* ptr = img->bits();

    ptr[4 * img->width() * y + 4 * x] = blue;
    ptr[4 * img->width() * y + 4 * x + 1] = green;
    ptr[4 * img->width() * y + 4 * x + 2] = red;
    ptr[4 * img->width() * y + 4 * x + 3] = 255;
}
