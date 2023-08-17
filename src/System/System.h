#pragma once

#include <QObject>
#include <QImage>
#include <QDateTime>

/**
 * @brief The ResManager class
 * 资源管理器，管理下载的资源。
 * 只是一个资源的管理与维护。
 *
 * 对于安卓需要添加权限：
 *
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />
    <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE" />
    <uses-permission android:name="android.permission.WAKE_LOCK" />
    <uses-permission android:name="android.permission.VIBRATE" />
 */
class System : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief getManager
     * @return
     */
    static System* getSystem();

    /**
     * @brief setCompanyName
     * 设置公司名字，英语
     * @param company
     */
    static void setCompanyName(QString company);

    /**
     * @brief setAppName
     * 设置程序名字
     * @param app
     */
    static void setAppName(QString app);

public slots:
    /**
     * @brief screenWakelock
     * 开启屏幕常亮
     * @param on
     * @return
     */
    bool screenWakelock(bool on);

    /**
     * @brief vibrate
     * 振动指定的时间
     * @param duration 单位是ms
     * @return
     */
    bool vibrate(int duration);

    /**
     * @brief getSDCardPath
     * Android/IOS返回的是：SD卡路径,如果sd卡不存在(或者无写权限)，就返回内部存储空间路径。
     * 其他平台返回的是程序根路径
     * @return
     */
    QString getSDCardPath();

    /**
     * @brief getAppPrivateStoragePath
     * 返回程序私有的存储空间位置。对于windows、linux就是返回程序当前运行目录。对于ios、android就是app内部空间
     * @return
     */
    QString getAppPrivateStoragePath();

    /**
     * @brief getAppStoragePath
     * Android平台返回的是：<root>/<Company>/<AppName>
     *  root是SD卡、app内部空间、程序当前目录
     * 其他平台返回的是：getSDCardPath
     * @return
     */
    QString getAppStoragePath();

    /**
     * @brief getLoggerPath
     * 获取日志存储位置
     * @return
     */
    QString getLoggerPath();

    /**
     * @brief getMediaPath
     * DCIM
     * @return
     */
    QString getMediaPath();

    /**
     * @brief getExportPath
     * 导出根目录（默认移动端就是sd卡，windows等就是当前./exported目录）
     * @return
     */
    QString getExportPath();

    /**
     * @brief getSerialNumber
     * 获取设备序列号。为了简单默认生成一个UUID
     * @return
     */
    QString getSerialNumber();

private:
    System();
    ~System();
protected:
    class Impl;
    Impl* m_Impl;
};
