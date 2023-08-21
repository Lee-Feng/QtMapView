#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QUrl>
#include <QCloseEvent>
#include <QTimer>
#include <Utiles/version.hpp>
#include <QtMapView/QtMapView.hpp>
#include <QLabel>
#include <QMovie>
#include <QRandomGenerator>

QPointF getRandomPoint(QtMapView::GeoRect size){
    qreal r1 = QRandomGenerator::global()->generateDouble();
    qreal r2 = QRandomGenerator::global()->generateDouble();
    return QPointF(size.minLon + size.lonSize() * r1, size.minLat + size.latSize() * r2);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /// 更新版本号显示到界面上
    int code = 0;
    QString version;
    utiles::get_version(code,version);
    qDebug() << QStringLiteral("版本:%1").arg(version);
    setWindowTitle(QString("%1 V%2").arg(windowTitle()).arg(version));

    QtMapView* map = new QtMapView();
    map->setMap(QStringLiteral("maps/中华人民共和国.json"));
    map->show();
    setCentralWidget(map);


    for(int i=0; i<50; i++){
        QLabel* lable = new QLabel(map);
        QMovie* m = new QMovie("images/animation.gif");
        m->start();
        lable->setMovie(m);
        lable->setGeometry(0,0,50,20);
        lable->setScaledContents(true);
        ///
        map->addNode(lable,getRandomPoint(map->geoViewRect()));
    }
    for(int i=0; i<50; i++){
        QLabel* lable = new QLabel(map);
        QMovie* m = new QMovie("images/doctran-loading.gif");
        m->start();
        lable->setMovie(m);
        lable->setGeometry(0,0,20,20);
        lable->setScaledContents(true);
        ///
        map->addNode(lable,getRandomPoint(map->geoViewRect()));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
