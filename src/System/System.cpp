#include "System.h"
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QDateTime>
#include <QSet>
#include <QSettings>
#include <QUuid>

#ifdef _Android
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

class System::Impl{
public:
    QString m_sdcard_path;
    Impl(){
    }

    QString getSDCardPath(bool redirectToAppPathIfNoPermision){
        QString path;
    #if defined(_Android)
        QAndroidJniObject mediaDir = QAndroidJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
        QAndroidJniObject mediaPath = mediaDir.callObjectMethod( "getAbsolutePath", "()Ljava/lang/String;" );
        QString dataAbsPath = mediaPath.toString();
        QAndroidJniEnvironment env;
        if (env->ExceptionCheck()) {
            // Handle exception here.
            env->ExceptionClear();
        }
        path = dataAbsPath;
        if(path.trimmed() == "" && redirectToAppPathIfNoPermision){
            qWarning() << QStringLiteral("获取到的sd卡路径是空，猜测sd卡路径...");
            path = guessAndroidWriteableSDCardPath();
            if(path.trimmed() == "" && redirectToAppPathIfNoPermision){
                qWarning() << QStringLiteral("猜测到的sd卡路径是空，使用内部路径");
                path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            }
        } else if(!checkWriteable(path) && redirectToAppPathIfNoPermision){
            qWarning() << QStringLiteral("获取到的sd卡路径无权限操作,请打开储存权限");
            path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        }
    #elif defined (ios)
        QStringList paths = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
        qDebug() << "[IOS]download paths count:" << paths.size();
        path = paths.first();
    #elif defined(WIN32)
        return qApp->applicationDirPath();
    #elif defined(Linux)
        return qApp->applicationDirPath();
    #else
        path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    #endif
        return path;
    }

    QString guessAndroidSDCardPath(){
        QFileInfo mnt_sdcard("/mnt/sdcard");
        if(mnt_sdcard.isDir() && mnt_sdcard.exists() && mnt_sdcard.isWritable()){
            return "/mnt/sdcard";
        }

        QFileInfo sdcard("/sdcard");
        if(sdcard.isDir() && sdcard.exists() && sdcard.isWritable()){
            return "/sdcard";
        }
        QFileInfo emulated0("/storage/emulated/0");
        if(emulated0.isDir() && emulated0.exists() && emulated0.isWritable()){
            return "/storage/emulated/0";
        }
        return "";
    }

    /// 测试是否可以写(新增文件，写文件)
    bool checkWriteable(QString dir){
        QString fileNameName = QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
        QString tmpfile = QString("%1/%2.txt").arg(dir).arg(fileNameName);
        QFile file(tmpfile);
        if(!file.open(QIODevice::WriteOnly)){
            return false;
        }
        if(file.write("test!") <= 0){
            return false;
        }
        file.remove();
        return true;
    }

    QString guessAndroidWriteableSDCardPath(){
        QString sdcard = guessAndroidSDCardPath();
        if(sdcard.isEmpty()){
            return "";
        }
        if(!checkWriteable(sdcard)){
            return "";
        }
        return sdcard;
    }
};

System::System()
{
    m_Impl = new Impl();
}

System::~System()
{
    delete m_Impl;
}

System *System::getSystem()
{
    static System mgr;
    return &mgr;
}

static QString g_company = "LeeF";
static QString g_app = "Test";

void System::setCompanyName(QString company)
{
    g_company = company;
}

void System::setAppName(QString app)
{
    g_app = app;
}


/**
 * @brief System::screenWakelock
 * @param on

    // It works fine on Android 4.x，之后版本要崩溃
    // 推荐直接在java中：this.getWindow().addFlags(android.view.WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (activity.isValid()) {
        QAndroidJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
        if (window.isValid()) {
            const int FLAG_KEEP_SCREEN_ON = 128;
            //这种写法要崩溃
            //window.callObjectMethod("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
            window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
        }
    }

 * @return
 */
bool System::screenWakelock(bool on)
{
    qDebug() << QStringLiteral("屏幕常亮[%1]").arg(on?"ON":"OFF");
#if defined(_Android)
    /// getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    /// getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    bool ret = false;
    QtAndroid::runOnAndroidThreadSync([on,&ret](){
        ///FLAG_KEEP_SCREEN_ON:值是128

        QAndroidJniObject activity = QtAndroid::androidActivity();
        if(!activity.isValid()){
            qWarning() << "JNI:androidActivity()Faild";
            ret = false;
            return;
        }
        /// 没有参数，所以参数部分是空()
        /// 返回类型是对象，所以是L
        /// 对象是
        QAndroidJniObject window =  activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
        if(!window.isValid()){
            qWarning() << "JNI:getWindow()Faild";
            ret = false;
            return;
        }
        const int FLAG_KEEP_SCREEN_ON = 128;
        if(on){
            window.callMethod<void>("addFlags","(I)V",FLAG_KEEP_SCREEN_ON); /// 参数是整型，用I表示，没有返回就V
        } else {
            window.callMethod<void>("clearFlags","(I)V",FLAG_KEEP_SCREEN_ON);
        }
        QAndroidJniEnvironment env;
        if (env->ExceptionCheck()) {
            // Handle exception here.
            env->ExceptionDescribe();
            env->ExceptionClear();
            qDebug() << QStringLiteral("屏幕常亮[%1]出错").arg(on?"ON":"OFF");
            ret = false;
            return;
        } else {
            ret = true;
            qDebug() << QStringLiteral("屏幕常亮[%1]成功").arg(on?"ON":"OFF");
        }
    },500);
    return ret;
#else
    Q_UNUSED(on);
    return true;
#endif
}

bool System::vibrate(int duration)
{
    /// https://blog.csdn.net/jfdfhh/article/details/87863196
#if defined(_Android)
    QtAndroid::runOnAndroidThread([duration](){
        QAndroidJniEnvironment env;
        QAndroidJniObject activity = QtAndroid::androidActivity();
        QAndroidJniObject name = QAndroidJniObject::getStaticObjectField("android/content/Context","VIBRATOR_SERVICE", "Ljava/lang/String;");
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            qDebug() << QStringLiteral("振动出错，获取VIBRATOR_SERVICE失败");
            return;
        }

        QAndroidJniObject vibrateService = activity.callObjectMethod( "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;", name.object<jstring>());
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            qDebug() << QStringLiteral("振动出错，getSystemService失败");
            return;
        }

        jlong _duration =duration;
        vibrateService.callMethod<void>("vibrate","(J)V", _duration);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            qDebug() << QStringLiteral("振动出错，vibrate失败");
            return;
        }
    });
    return true;
#else
    Q_UNUSED(duration);
    return true;
#endif
}

QString System::getSDCardPath()
{
    if(m_Impl->m_sdcard_path.isEmpty()){
        m_Impl->m_sdcard_path = m_Impl->getSDCardPath(true);
        qDebug() << QStringLiteral("SD卡初始值:%1").arg(m_Impl->m_sdcard_path);
    }
    return m_Impl->m_sdcard_path;
}

QString System::getAppPrivateStoragePath()
{
#if defined(WIN32)
    return qApp->applicationDirPath();
#elif defined(Linux)
    return qApp->applicationDirPath();
#else
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#endif
}

QString System::getAppStoragePath()
{
#if defined(_Android)
        return getSDCardPath() + "/" + g_company + "/" + g_app;
#else
        return getSDCardPath();
#endif
}

QString System::getLoggerPath()
{
    return getAppStoragePath() + "/log";
}

QString System::getMediaPath()
{
    return getSDCardPath() + "/DCIM";
}

QString System::getExportPath()
{
#if defined(WIN32)
    return qApp->applicationDirPath() + "/exported";
#elif defined(Linux)
    return qApp->applicationDirPath() + "/exported";
#else
    return m_Impl->getSDCardPath(false) + "/exported";
#endif
}

QString System::getSerialNumber()
{
    QString system = QString("%1/system.ini").arg(getAppPrivateStoragePath());
    QSettings st(system,QSettings::IniFormat);
    QString sn = st.value("serial_number").toString();
    if(sn.trimmed().isEmpty()){
        sn = QUuid::createUuid().toString(QUuid::WithoutBraces);
        st.setValue("serial_number",sn);
        st.sync();
        qDebug() << QStringLiteral("生成了序列号:%1").arg(sn);
    }
    return sn;
}
