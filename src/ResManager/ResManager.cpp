#include "ResManager.h"
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
#include <System/System.h>
#include <QUrl>
#include <ResManager/RecordExporter.h>

class ResManager::Impl{
public:
    QFileSystemWatcher m_wather;
    Impl(ResManager* pthis):m_pthis(pthis){
        m_WritebaleRoot = getThisAppWriteablePath();
        m_resourceRoot = getResourceDir();
        m_ExportDefaultRoot = getExportDefaultDir();
        m_LogRoot = getLogDir();
        QDir dir;
        dir.mkpath(m_resourceRoot);

        m_wather.addPath(m_resourceRoot);
        m_delayNotify.setSingleShot(true);
        QObject::connect(&m_delayNotify,&QTimer::timeout,[=](){
            QString file = getNewestThumbFile();
            qDebug() << QStringLiteral("Res当前最新缩略文件:%1").arg(file);
            ResInfo info;
            info.thum_file = file;
            info.src_file = getThumbSrcFile(file);
            m_lastNewFile = file;
            if(!info.src_file.isEmpty() && !info.thum_file.isEmpty()){
                qDebug() << QStringLiteral("Res当前最新资源文件:原始[%1],缩略图[%2]").arg(info.src_file).arg(info.thum_file);
                m_pthis->resourceChanged(info.src_file,info.thum_file);
            }

            /// 检查完整的文件变化
            QSet<QString> currentFiles = getFileList(true);
            QStringList changeList = diff(currentFiles,m_files);
            m_files = currentFiles;
            foreach(auto p,changeList){
                QFileInfo info(p);

                /// 只处理缩略图文件，避免缩略图和原始文件同时触发两次
                if(!isThumbFile(p)){
                    continue;
                }

                if(!info.exists()){
                    qDebug() << QStringLiteral("\nRes:资源[%1]删除了").arg(info.baseName());
                    m_pthis->fileRemoved(info.baseName());
                } else {
                    QDateTime time = info.lastModified();
                    qDebug() << QStringLiteral("\nRes:资源[%1]新增了,时间戳:[%2]").arg(info.baseName()).
                                arg(time.toString("yyyy-MM-dd hh:mm:ss,zzz"));
                    m_pthis->fileChanged(info.baseName(),time);
                }
            }

        });
        QObject::connect(&m_wather,&QFileSystemWatcher::directoryChanged,[=](const QString &path){
            qDebug() << QStringLiteral("Res:目录有文件变化:%1").arg(path);
            m_delayNotify.start(200); /// 避免多次触发
        });

        /// 获取本地最新文件
        m_lastNewFile = getNewestThumbFile();
        m_files = getFileList(true);
    }

    QString getThumbFileSufix(){
        return ".THM";
    }

    /**
     * @brief isThumbFile
     * 是否是缩略图文件
     * @param file
     * @return
     */
    bool isThumbFile(const QString& file){
        return file.endsWith(getThumbFileSufix());
    }

    /**
     * @brief getThumbSrcFile
     * 获取文件的原始文件
     * @param file
     * @return
     */
    QString getThumbSrcFile(const QString& file){
        QString ret;

        if(!isThumbFile(file)){
            return file;
        }

        QStringList files = m_pthis->getSameBaseNameFiles(file);
        foreach(auto p,files){
            if(p == file){
                continue;
            }
            if(isThumbFile(p)){
                continue;
            }
            ret = p;
            break;
        }
        return ret;
    }

    /**
     * @brief getSrcThumbFile
     * 获取文件的THM文件
     * @param file
     * @return
     */
    QString getSrcThumbFile(const QString& file){
        QString ret;

        if(isThumbFile(file)){
            return file;
        }

        QStringList files = m_pthis->getSameBaseNameFiles(file);
        foreach(auto p,files){
            if(p == file){
                continue;
            }
            if(!isThumbFile(p)){
                continue;
            }
            ret = p;
            break;
        }
        return ret;
    }

    QString getNewestThumbFile(){
        QDir dir(m_resourceRoot);
        //要判断路径是否存在
        if(!dir.exists()) {
            return "";
        }
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);//实现对文件的过滤
        dir.setSorting(QDir::Time | QDir::Reversed);//实现对文件输出的排序
        QStringList filters;
        filters << QString("*%1").arg(getThumbFileSufix());
        dir.setNameFilters(filters);  //设置文件名称过滤器，只为filters格
        QFileInfoList list = dir.entryInfoList();
        if(list.empty()){
            return "";
        }
        return list.last().fileName();
    }

    QSet<QString> getFileList(bool includePath = false){
        QSet<QString> ret;

        QDir dir(m_resourceRoot);
        //要判断路径是否存在
        if(!dir.exists()) {
            return ret;
        }
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);//实现对文件的过滤
        dir.setSorting(QDir::Time | QDir::Reversed);//实现对文件输出的排序
        QStringList filters;
        foreach(const auto& p, m_formats){
            filters << QString("*%1").arg(p);
        }
        dir.setNameFilters(filters);  //设置文件名称过滤器，只为filters格
        QFileInfoList list = dir.entryInfoList();
        foreach(auto p,list){
            ret.insert(includePath ? p.filePath():p.fileName());
        }
        return ret;
    }

    QStringList diff(QSet<QString> a,QSet<QString> b){
        QSet<QString> ret;
        foreach(auto p,a){
            if(!b.contains(p)){
                ret.insert(p);
            }
        }
        foreach(auto p,b){
            if(!a.contains(p)){
                ret.insert(p);
            }
        }
        return ret.values();
    }

    /**
     * @brief getOrFindNewestFile
     * @return
     */
    QString getOrFindNewestThumbFile(){
        if(m_lastNewFile.isEmpty()){
            m_lastNewFile = getNewestThumbFile();
        }
        return m_lastNewFile;
    }

    ~Impl(){
    }

protected:
    ResManager* m_pthis;
    QTimer m_delayNotify;
public:
    QString m_lastNewFile;
    QStringList m_formats;
    QString m_resourceRoot;
    QString m_LogRoot;
    QString m_ExportDefaultRoot;
    QString m_WritebaleRoot;
    /// 记录的全部有效文件列表,完整路径
    QSet<QString> m_files;

private:

    QString guessAndroidSDCardPath(){
        QFileInfo mnt_sdcard("/mnt/sdcard");
        if(mnt_sdcard.isDir() && mnt_sdcard.exists()){
            return "/mnt/sdcard";
        }

        QFileInfo sdcard("/sdcard");
        if(sdcard.isDir() && sdcard.exists()){
            return "/sdcard";
        }
        QFileInfo emulated0("/storage/emulated/0");
        if(emulated0.isDir() && emulated0.exists()){
            return "/storage/emulated/0";
        }
        return "";
    }

    QString getLogDir(){
        return getThisAppWriteablePath() + "/log";
    }

    QString getResourceDir()
    {
        return getThisAppWriteablePath() + "/resource";
    }

    QString getExportDefaultDir()
    {
        return System::getSystem()->getSDCardPath() + "/DCIM";
    }

    /**
     * @brief getThisAppWriteablePath
     * 获取当前App的写资源的路径。不同App之间应该有不一样的目录。
     * @return
     */
    QString getThisAppWriteablePath(){
#if defined(_ANDROID_PLATFORM_)
        return System::getSystem()->getSDCardPath() + "/xinda/catenarymonitor/";
#else
        return System::getSystem()->getSDCardPath();
#endif
    }

};

ResManager::ResManager()
{
    m_Impl = new Impl(this);
    bool suc = connect(RecordExporter::getExporter(),SIGNAL(exportProcessing(int,QString)),
                       this,SIGNAL(exportProcessing(int,QString)));
    Q_ASSERT(suc);
    suc = connect(RecordExporter::getExporter(),SIGNAL(exportError(QString)),
            this,SIGNAL(exportError(QString)));
    Q_ASSERT(suc);
}

ResManager::~ResManager()
{
    delete m_Impl;
}

ResManager *ResManager::getManager()
{
    static ResManager mgr;
    return &mgr;
}

void ResManager::setResourceFormaters(QStringList formats)
{
    if(m_Impl->m_formats == formats){
        return;
    }
    m_Impl->m_formats = formats;
    m_Impl->m_lastNewFile.clear();
    m_Impl->getOrFindNewestThumbFile();
}

QStringList ResManager::getResourceFormaters()
{
    return m_Impl->m_formats;
}

bool ResManager::exist(QString file)
{
    return QFileInfo::exists(getFileAbsPath(file));
}

qint64 ResManager::fileSize(QString file)
{
    QFileInfo info(getFileAbsPath(file));
    return info.size();
}

QString ResManager::getFileAbsPath(const QString &f)
{
    QString path = f;
    if(f.startsWith("file://")){
#if defined (Q_OS_WIN32)
        path = path.mid(8); /// windows: file:/// + D:/test/test.txt
#elif defined(Q_OS_LINUX)
        path = path.mid(7); /// windows: file:// + /tmp/test/test.txt
#endif
    }

    QFileInfo info(path);
    if(info.isAbsolute()){
        return path;
    }
    return QString("%1/%2").arg(getResourceDir()).arg(f);
}

QString ResManager::getResourceDir() const
{
    return m_Impl->m_resourceRoot;
}

QString ResManager::getLoggerDir()
{
    return m_Impl->m_LogRoot;
}

QString ResManager::getExportDefaultDir()
{
    return m_Impl->m_ExportDefaultRoot;
}

QString ResManager::getWriteableDir()
{
    return m_Impl->m_WritebaleRoot;
}

QString ResManager::getNewestFile()
{
    return m_Impl->getOrFindNewestThumbFile();
}

QString ResManager::getNewestThumFile()
{
    return m_Impl->getOrFindNewestThumbFile();
}

void ResManager::removeFile(const QString &file, bool includeSameBaseNameFiles)
{
    QStringList files;
    if(includeSameBaseNameFiles){
        files = getSameBaseNameFiles(file);
    } else {
        files.append(file);
    }
    foreach(const auto& p,files){
        QString fileName = getFileAbsPath(p);
        if(!QFile::remove(fileName)){
            qWarning() << QStringLiteral("Res:删除文件失败:%1").arg(fileName);
        } else {
            qDebug() << QStringLiteral("Res:删除文件成功:%1").arg(fileName);
        }
    }
}

void ResManager::removeFiles(const QVariantList &file,bool includeSameBaseNameFiles)
{
    foreach(const auto&p,file){
        QString f = p.toString();
        removeFile(p.toString(),includeSameBaseNameFiles);
    }
}

QStringList ResManager::getFilesByBaseName(const QString &baseName)
{
    QStringList ret;

    QDir dir(getResourceDir());
    //要判断路径是否存在
    if(!dir.exists()) {
        return ret;
    }
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);//实现对文件的过滤
    QStringList filters;
    foreach(const auto& p, m_Impl->m_formats){
        filters << QString("%1*%2").arg(baseName).arg(p);
    }
    dir.setNameFilters(filters);  //设置文件名称过滤器，只为filters格
    QFileInfoList list = dir.entryInfoList();
    if(list.empty()){
        return ret;
    }
    foreach(const auto&p, list){
        if(p.baseName() == baseName){
            ret.append(p.fileName());
        }
    }
    return ret;
}

QStringList ResManager::getSameBaseNameFiles(const QString &fileName)
{
    QFileInfo info(fileName);
    return getFilesByBaseName(info.baseName());
}

QString ResManager::getSrcThumbFile(const QString &file)
{
    return m_Impl->getSrcThumbFile(file);
}

QString ResManager::getThumbSrcFile(const QString &file)
{
    return m_Impl->getThumbSrcFile(file);
}


//static bool IsBitmap(const QString& pstrFileName)
//{
//    long nImageHeader = 2;
//    QByteArray btData;
//    QFile file(pstrFileName);
//    if(file.open(QIODevice::ReadOnly)){
//        btData = file.read(nImageHeader);
//        file.close();
//    }

//    if(btData.size() >= nImageHeader){
//        if((unsigned char)btData[0] == 0x42 && (unsigned char)btData[1] == 0x4d){
//            return true;
//        }
//    }
//    return false;
//}

static bool IsJpeg(const QString& pstrFileName)
{
    long nImageHeader = 2;
    long nImageTail = 2;
    QByteArray headData;
    QByteArray tailData;
    QFile file(pstrFileName);
    if(!file.open(QIODevice::ReadOnly)){
        return false;
    }
    if(file.size() < nImageHeader + nImageTail){
        return false;
    }
    headData = file.read(nImageHeader);
    file.seek(file.size()-nImageTail);
    tailData = file.read(nImageTail);
    file.close();

    if(headData.size() < nImageHeader){
        return false;
    }
    if(tailData.size() < nImageTail){
        return false;
    }
    if(( (unsigned char)headData[0]) == 0xff && ((unsigned char)headData[1]) == 0xd8 &&
            ((unsigned char)tailData[0]) == 0xff && ((unsigned char)tailData[1]) == 0xd9){
        return true;
    }
    return false;
}

//static bool IsPng(const QString& pstrFileName)
//{
//    long nImageHeader = 8;
//    QByteArray btData;
//    QFile file(pstrFileName);
//    if(file.open(QIODevice::ReadOnly)){
//        btData = file.read(nImageHeader);
//        file.close();
//    }

//    if(btData.size() >= nImageHeader){
//        if((unsigned char)btData[0] == 0x89 && (unsigned char)btData[1] ==0x50 && (unsigned char)btData[2] ==0x4E && (unsigned char)btData[3] ==0x47 &&
//            (unsigned char)btData[4] ==0x0D && (unsigned char)btData[5] ==0x0A && (unsigned char)btData[6] ==0x1A && (unsigned char)btData[7] ==0x0A){
//            return true;
//        }
//    }
//    return false;
//}

bool ResManager::isValidImage(const QString &file)
{
    QFileInfo info(file);
    if(info.suffix().compare("JPG") != 0 &&
            info.suffix().compare("JPEG") != 0 &&
            info.suffix().compare("PNG") != 0 &&
            info.suffix().compare("THM") != 0 ){
        return false;
    }
    return IsJpeg(getFileAbsPath(file));
}

QStringList ResManager::getFileThums()
{
    QStringList ret;

    QDir dir(getResourceDir());
    //要判断路径是否存在
    if(!dir.exists()) {
        return ret;
    }
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);//实现对文件的过滤
    dir.setSorting(QDir::Time | QDir::Reversed);//实现对文件输出的排序
    QStringList filters;
    filters << QString("*%1").arg(m_Impl->getThumbFileSufix());
    dir.setNameFilters(filters);  //设置文件名称过滤器，只为filters格
    QFileInfoList list = dir.entryInfoList();
    if(list.empty()){
        return ret;
    }
    foreach(const auto&p, list){
        ret.append(p.fileName());
    }
    return ret;
}

QString ResManager::getFileBaseName(const QString &file)
{
    QFileInfo info(getFileAbsPath(file));
    return info.baseName();
}

QString ResManager::getThumbExtensionName()
{
    return m_Impl->getThumbFileSufix();
}

QString ResManager::getHistoryDatabaseName() const
{
    return getResourceDir() + "/meta/catenary_monitor_history.db";
}

QVariant ResManager::getHistoryDatabaseDriver() const
{
    return QVariant::fromValue(QString("QSQLITE"));
}

QString ResManager::getHistoryDatabaseTable() const
{
    return "HistoryList";
}

QStringList ResManager::getHistoryDatabaseTableKeys() const
{
    return {"id"};
}

void ResManager::exportFiles(const QVariantList &files, QString dir)
{
    RecordExporter::getExporter()->exportDatas(files,dir);
}

void ResManager::stopExporting()
{
    RecordExporter::getExporter()->stopExporting();
}
