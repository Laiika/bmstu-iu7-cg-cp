#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <chrono>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    width = ui->frame->width();
    height = ui->frame->height();

    startX = ui->frame->x();
    startY = ui->frame->y();

    MainImage = new QImage(width, height, QImage::Format_ARGB32);
    MainImage->fill(QColor(0,0,0,255));


    scene = Scene(&painter, MainImage);
    painter = Painter(width, height, scene.getCamera());

    scene.generateWorld();

    timerID = startTimer(20);
    time = 1;
    move = true;
    sum = 0;
    earthCam = false;
    update();
}

MainWindow::~MainWindow()
{
    killTimer(timerID);
    delete ui;
}

// движение камеры
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_E) // вправо
    {
        scene.getCamera()->tY-=3;
    }
    else if(event->key() == Qt::Key_Q) // влево
    {
        scene.getCamera()->tY+=3;
    }
    else if(event->key() == Qt::Key_A) // вверх
    {
        scene.getCamera()->tX-=3;
    }
    else if(event->key() == Qt::Key_D) // вниз
    {
        scene.getCamera()->tX+=3;
    }
    else if(event->key() == Qt::Key_R) // вниз
    {
        scene.getCamera()->rotateY(3);
    }
    scene.refresh();
    update();
}

// движение сфер
void MainWindow::timerEvent(QTimerEvent *event)
{
    if (move)
    {
        auto start = std::chrono::high_resolution_clock::now();
        scene.planetRound(scene.earth, 0, 6);
        scene.refresh();

        scene.planetRound(scene.jowisz, 1, 6);
        scene.planetRound(scene.saturn,2, 1);
        scene.sputnikRound(scene.uran, scene.saturn->center);

        if(earthCam){
            scene.getCamera()->cX = scene.pos[0].x;
            scene.getCamera()->cY = scene.pos[0].y;
            scene.getCamera()->cZ = scene.pos[0].z - 400;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        if (time < 20 && time > 9)
        {
            qDebug() << duration.count();
            sum += duration.count();
        }

        if (time == 20)
            qDebug() << sum / 10;
    }
    update();
    time++;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    p.drawImage(startX, startY, *MainImage);
}


// запустить
void MainWindow::on_pushButton_3_clicked()
{
    move = true;
}
// остановить
void MainWindow::on_pushButton_4_clicked()
{
    move = false;
}

// вверх
void MainWindow::on_pushButton_5_clicked()
{
    scene.getCamera()->tX-=3;
    scene.refresh();
    update();
}
// вниз
void MainWindow::on_pushButton_6_clicked()
{
    scene.getCamera()->tX+=3;
    scene.refresh();
    update();
}

// вправо
void MainWindow::on_pushButton_7_clicked()
{
    scene.getCamera()->tY-=3;
    scene.refresh();
    update();
}
// влево
void MainWindow::on_pushButton_8_clicked()
{
    scene.getCamera()->tY+=3;
    scene.refresh();
    update();
}

