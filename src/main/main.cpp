#include <QApplication>
#include <QDebug>
#include <QString>
#include <stdio.h>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <Utiles/CrashHandler.hpp>
#include <Utiles/logger.h>
#include <AsyncInvoker/AsyncInvoker.hpp>
#include "mainwindow.h"
#include <QMessageBox>
#include <QTextCodec>

void LoadQss()
{
    // 设置界面的主题（如果有界面的话）
    QFile file(QString(":/stylecss/stylecss/blue.css"));
    file.open(QFile::ReadOnly);
    if(file.isOpen())
    {
        QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(qss);
        qApp->setPalette(QPalette(QColor("#F0F0F0")));
    }
}

void passInit(){
    /// test
    QString p;
    p.append('t');
    p.append('e');
    p.append('s');
    p.append('t');
    ui::registerLicense(p);
}

/**
 * @brief main
 */
int main(int argc,char* argv[])
{
    bool crash_hanlder = true;
    for(int i=1; i<argc; i++){
        if(strcmp(argv[i],"nohandler") == 0 ||
                strcmp(argv[i],"-nohandler") == 0 ||
                strcmp(argv[i],"nocrashhandler") == 0 ||
                strcmp(argv[i],"-nocrashhandler") == 0){
            crash_hanlder = false;
        }
    }
    if(crash_hanlder){
        utiles::InstallFaultHandler();
    }

    QApplication app(argc,argv);
    logger::Logger log("test");
    log.start("log");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    passInit();

    std::string my_app_path = argv[0];
    std::string my_app_name = my_app_path.substr(my_app_path.find_last_of("\\")+1);
    qDebug() << QStringLiteral("程序%1,运行目录是%2").
                    arg(QString::fromLocal8Bit(my_app_name.c_str())).
                    arg(QString::fromLocal8Bit(my_app_path.c_str()));

    /// 加载皮肤
    LoadQss();

    QTextCodec* codec = QTextCodec::codecForName("System");
    QTextCodec::setCodecForLocale(codec);


    MainWindow mainwidow;
    mainwidow.show();

    return app.exec();
}
