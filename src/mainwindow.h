#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <painter.h>
#include <sphere.h>
#include <scene.h>
#include <QTimer>
#include <QTimerEvent>

#include <QMainWindow>
#include <QKeyEvent>
#include <QPainter>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QImage *MainImage;

    int width, height;
    int startX, startY;

    // Отрисовщик, который рисует пиксели
    Painter painter;
    // Сцена, которая содержит все 3d объекты
    Scene scene;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent *event);

    void timerEvent(QTimerEvent *event);
    int timerID;
    int time;   // переменная увеличивается каждый раз, когда вызывается функция timerEvent
    bool move;
    long long sum;
    bool earthCam;

private slots:
    void paintEvent(QPaintEvent*);

    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
};

#endif // MAINWINDOW_H
