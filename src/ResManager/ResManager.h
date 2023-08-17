#pragma once

#include <QObject>
#include <QImage>
#include <ResManager/ResInfo.h>
#include <QDateTime>
#include <QVariant>

/**
 * @brief The ResManager class
 * 资源管理器，管理下载的资源。
 * 只是一个资源的管理与维护。
 *
 * 目前：THM是原图，JPG和MP4只是对THM文件的标识，用于标注当前文件时图片还是MP4。
 * 实际原始文件由于太大并没有下载。需要手动触发下载才会执行下载。
 *
 * 对于安卓需要添加权限：
 *
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />
    <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE" />
 */
class ResManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief getManager
     * @return
     */
    static ResManager* getManager();

public slots:

    /**
     * @brief setResourceFormaters
     * 设置资源的格式后缀，区分大小写, 需要附加“.”，比如“.MP4”
     * 用于监控变化
     * @param formats
     */
    void setResourceFormaters(QStringList formats);

    /**
     * @brief getResourceFormaters
     * @return
     */
    QStringList getResourceFormaters();

    /**
     * @brief exist
     * 检测文件是否存在
     * @param file  相对路径或者绝对路径都可以
     * @return
     */
    bool exist(QString file);

    /**
     * @brief fileSize
     * 文件大小
     * @param file  相对路径或者绝对路径都可以
     * @return
     */
    qint64 fileSize(QString file);

    /**
     * @brief getFileAbsPath
     * 获取文件绝对路径
     * @param 相对路径或者完整路径都可以,也支持file://的url
     * @return
     */
    QString getFileAbsPath(const QString& f);

    /**
     * @brief getResourceDir
     * 获取资源的目录.
     * windows: <workdir>/resource/
     * android: /SDCard/xinda/catenarymonitor/resource
     * @return
     */
    QString getResourceDir() const;

    /**
     * @brief getLoggerDir
     * 获取日志目录
     * @return
     */
    QString getLoggerDir();


    /**
     * @brief getExportDefaultDir
     * 获取导出的默认目录
     * @return
     */
    QString getExportDefaultDir();

    /**
     * @brief getWriteableDir
     * window就是当前程序运行目录，android是SD卡某个目录:/SDCard/xinda/catenarymonitor
     * @return
     */
    QString getWriteableDir();

    /**
     * @brief getNewestFile
     * 获取最新的文件,等同于getNewestThumFile
     * @return
     */
    QString getNewestFile();

    /**
     * @brief getNewestThumFile
     * 获取最新的缩略图文件，等同于getNewestFile
     * @return
     */
    QString getNewestThumFile();

    /**
     * @brief removeFile
     * 移除文件
     * @param file  相对路径或者完成路径都可以
     */
    void removeFile(const QString& file,bool includeSameBaseNameFiles);
    void removeFiles(const QVariantList &file,bool includeSameBaseNameFiles);

    /**
     * @brief getFilesByBaseName
     * 获取baseName是指定名字的文件列表。
     * 用于获取文件以及其相关的原始文件或者缩略图文件
     * @param baseName  只写basename
     * @return 文件名列表，不包括路径
     */
    QStringList getFilesByBaseName(const QString& baseName);

    /**
     * @brief getSamleBaseNameFiles
     * 获取base文件名相同的文件列表
     * @param fileName  相对路径或者完成路径都可以，内部只获取它的baseName
     * @return 文件名列表，不包括路径
     */
    QStringList getSameBaseNameFiles(const QString& fileName);

    /**
     * @brief getSrcThumbFile
     * 获取资源的缩略图
     * @param file  相对路径或者完成路径都可以，内部只获取它的baseName
     * @return 文件名，不包括路径
     */
    QString getSrcThumbFile(const QString& file);
//    // 获取的绝对路径
//    QString getSrcThumbFileAbsPath(const QString& file);

    /**
     * @brief getThumbSrcFile
     * 获取缩略图对应的原始资源
     * @param file  相对路径或者完成路径都可以，内部只获取它的baseName
     * @return 文件名，不包括路径
     */
    QString getThumbSrcFile(const QString& file);
//    QString getThumbSrcFileAbsPath(const QString& file);

    /**
     * @brief isValidImage
     * 检测文件是否是可用的图片
     * @param file 相对路径或者完整路径均可
     * @return
     */
    bool isValidImage(const QString& file);

    /**
     * @brief getFileThums
     * 获取全部的缩略图
     * @return 文件名列表，不包括路径
     */
    QStringList getFileThums();

    /**
     * @brief getFileBaseName
     * @param file,支持相对，绝对路径，也支持url：file
     * @return
     */
    QString getFileBaseName(const QString& file);

    /**
     * @brief getThumbExtensionName
     * 获取缩略图的后缀名 比如 “.THM”
     * @return
     */
    QString getThumbExtensionName();

public slots:

    /**
     * @brief getHistoryDatabaseName
     * 获取记录的数据库名字
     * @return
     */
    QString getHistoryDatabaseName() const;

    /**
     * @brief getHistoryDatabaseDriver
     * 获取记录的驱动方式
     * @return
     */
    QVariant getHistoryDatabaseDriver() const;

    /**
     * @brief getHistoryDatabaseTable
     * 获取记录的数据表
     * @return
     */
    QString getHistoryDatabaseTable() const;

    /**
     * @brief getHistoryDatabaseTableKeys
     * @return
     */
    QStringList getHistoryDatabaseTableKeys() const;


    /**
     * @brief exportFiles
     * 异步导出文件到指定路径。参考exportProcessing
     * 如果当前正在导出，这里将失败触发：exportError
     * @param file
     * @param dir   导出到的根目录
     */
    void exportFiles(const QVariantList &files, QString dir);
    void stopExporting();

signals:
    /// 资源有修改,如果file是空，就没有文件了
    void fileChanged(QString fileBaseName,QDateTime birthTime);
    void fileRemoved(QString fileBaseName);
    void resourceChanged(QString src_file,QString thum_file);
    /**
     * @brief exportProcessing
     * 导出过程中通知,0就是刚开始，100是已经导出完成
     * @param process [0,100]
     */
    void exportProcessing(int process,QString msg);
    /**
     * @brief exportError
     * 导出失败
     * @param error
     */
    void exportError(QString error);

private:
    ResManager();
    ~ResManager();
protected:
    class Impl;
    Impl* m_Impl;
};
