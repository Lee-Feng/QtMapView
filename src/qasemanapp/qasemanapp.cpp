#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>
#include <QWidget>
#include <QTimer>
#include <MJPEGCapture/MJPEGCapture.h>
#include <ImageItem/ImageItem.h>
#include <PlusItem/PlusItem.h>
#include <Speeker/Speeker.h>
#include <System/System.h>
#include <LocationMonitor/LocationMonitor.h>
#include <Utiles/logger.h>
#include <interface/interface.hpp>
#include <QLabel>
#include <QImage>
#include <Utiles/CrashHandler.hpp>

int main(int argc, char *argv[])
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

    /// 为了避免QWidget在QQuickPaintedItem中render时线程问题，需要如下处理
    //qputenv("QSG_RENDER_LOOP",QByteArray("basic"));

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    System::setCompanyName("xinda");
    System::setAppName("XXXXXXapp");
    logger::Logger log("xxx");
    log.start(System::getSystem()->getLoggerPath());

    QString record_path = System::getSystem()->getAppPrivateStoragePath() + "/records/laser_clearing_record.db";
    qDebug() << QStringLiteral("数据根目录:%1").arg(System::getSystem()->getAppStoragePath());
    qDebug() << QStringLiteral("SDCard目录:%1").arg(System::getSystem()->getSDCardPath());
    qDebug() << QStringLiteral("Media目录:%1").arg(System::getSystem()->getMediaPath());
    qDebug() << QStringLiteral("日志路径:%1").arg(System::getSystem()->getLoggerPath());
    qDebug() << QStringLiteral("记录路径:%1").arg(record_path);

    qDebug() << QStringLiteral("当前系统默认字体:%1").arg(app.font().family());
    /// 微软的字体，不能免费商用?
    //int fontId = QFontDatabase::addApplicationFont(":/fonts/simsunb.ttf");
    /// Mplus免费商用，中英文都等宽，看起来不好看
    //int fontId = QFontDatabase::addApplicationFont("fonts/Mplus1p-Regular.ttf");
    /// UbuntuMono-Regular可以用，只是英文有点奇妙
    int fontId = QFontDatabase::addApplicationFont(":/fonts/UbuntuMono-Regular.ttf");
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    qDebug()<<"fontfamilies:"<<fontFamilies;
    if(!fontFamilies.isEmpty()){
        QFont font;
        font.setFamily(fontFamilies.first());//设置全局字体
        app.setFont(font);
        qDebug() << QStringLiteral("修改程序默认字体为:%1").arg(app.font().family());
    }

    QQmlApplicationEngine engine;
    engine.addImportPath(":/imports/");

    /// C++扩展类型
    qmlRegisterType<MJPEGCapture>("MJPEGCapture",1,0,"MJPEGCapture");
    qmlRegisterType<ImageItem>("ImageItem",1,0,"ImageItem");
    qmlRegisterType<PlusItem>("PlusItem",1,0,"PlusItem");

    MJPEGCapture capture;
    Speeker speeker;
    Interface* api = Interface::getInstance();
    /// 全局变量
    engine.rootContext()->setContextProperty("$system",System::getSystem());
    engine.rootContext()->setContextProperty("$locationSys",LocationMonitor::getManager());
    engine.rootContext()->setContextProperty("$cameraStreamer",&capture);
    engine.rootContext()->setContextProperty("$speeker",&speeker);
    engine.rootContext()->setContextProperty("$api",api);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl){
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    engine.load(url);

    QTimer::singleShot(100,[=](){
        /// 常亮
        System::getSystem()->screenWakelock(true);
    });

    return app.exec();
}
