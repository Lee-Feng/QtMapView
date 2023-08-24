#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QUrl>
#include <QCloseEvent>
#include <QTimer>
#include <Utiles/version.hpp>
#include <QtMapView/QtMapView.hpp>
#include <QtMapView/QtMapGifItem.hpp>
#include <QtMapView/QtMapLineItem.hpp>
#include <QtMapView/QtMapImageItem.hpp>
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

    m_map = new QtMapView();
    setCentralWidget(m_map);
    m_map->setMap(QStringLiteral("maps/中华人民共和国.json"));
    p1 = QPointF(96.5,35);
    p2 = QPointF(30,10);

    on_actionradmonAdd_triggered();
    /// 因为窗口还没实验windows的大小
    QTimer::singleShot(10,this,SLOT(on_actionmoveToP1_triggered()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionmoveToP1_triggered()
{
    m_map->moveToGeoPos(p1);
}

void MainWindow::on_actionmoveToP2_triggered()
{
    m_map->moveToGeoPos(p2);
}

void MainWindow::on_actionremoveOneNode_triggered()
{
    QSet<QtMapItem *> nodes = m_map->nodeItems();
    if(!nodes.isEmpty()){
        m_map->removeNode(*nodes.begin());
    }
}

void MainWindow::on_actionaddOneNode_triggered()
{
    m_map->addNode(new QtMapGifItem("images/doctran-loading.gif","Load",QSize(20,20)),getRandomPoint(m_map->geoViewRect()));
}

void MainWindow::on_actionradmonAdd_triggered()
{
    on_actionremoveAll_triggered();
    for(int i=0; i<50; i++){
        m_map->addNode(new QtMapGifItem("images/animation.gif","",QSize(50,20)),getRandomPoint(m_map->geoViewRect()));
    }
    for(int i=0; i<50; i++){
        m_map->addNode(new QtMapGifItem("images/doctran-loading.gif","Load",QSize(20,20)),getRandomPoint(m_map->geoViewRect()));
    }
    for(int i=0; i<3; i++){
        m_map->addNode(new QtMapLineItem(getRandomPoint(m_map->geoViewRect()),getRandomPoint(m_map->geoViewRect())));
    }
    m_map->addNode(new QtMapLineItem(QPointF(86.78,41.253),QPointF(119.80444,29.346)));
    m_map->addNode(new QtMapLineItem(QPointF(119.80444,28.346),QPointF(86.78,39.253)));
}

void MainWindow::on_actionremoveAll_triggered()
{
    m_map->clearNodeItems();
    m_map->addNode(new QtMapImageItem("images/01.png","P1"),p1);
    m_map->addNode(new QtMapImageItem("images/02.png","P2"),p2);
}
