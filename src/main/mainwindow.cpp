﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QUrl>
#include <QCloseEvent>
#include <QTimer>
#include <Utiles/version.hpp>


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

}

MainWindow::~MainWindow()
{
    delete ui;
}
