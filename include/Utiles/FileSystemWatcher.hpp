#pragma once

#include <QtGlobal>
#include <QFileSystemWatcher>
#include <QMap>
#include <QDir>
#include <QFileInfo>
#include <QObject>
#include <QDebug>

namespace utiles {
/**
 * @brief The FileSystemWatcher class
 * 使用时需要将头文件添加到项目工程里面
 * HEADERS += $$PTVAR_INC/Utiles/FileSystemWatcher.hpp
 */
class FileSystemWatcher : public QObject{
    Q_OBJECT
public:
    FileSystemWatcher(QObject* parent = NULL):QObject(parent){
    }
    ~FileSystemWatcher(){
    }
    /**
     * @brief watchTree
     * 检测指定目录
     * @param rootDir
     */
    void watchTree(QString rootDir){
        if(m_rootWatchers.contains(rootDir)){
            return;
        }
        QFileSystemWatcher* w = new QFileSystemWatcher();
        initSignalProcess(w);
        if(!watchDirTree(w,rootDir,true)){
            delete w;
            return;
        }
        m_rootWatchers[rootDir] = w;
    }

signals:
    void fileChanged(const QString& path);
    void directoryChanged(const QString& path);

protected:

    void initSignalProcess(QFileSystemWatcher* w){
        connect(w,&QFileSystemWatcher::fileChanged,[&](const QString& path){
            qDebug() << "FileChanged:" << path;
            fileChanged(path);
        });
        connect(w,&QFileSystemWatcher::directoryChanged,[&](const QString& path){
            QFileSystemWatcher* w = qobject_cast<QFileSystemWatcher*>(sender());
            qDebug() << "DirectoryChanged:" << path;
            /// root下面 新建/删除文件，新建/删除文件夹，文件修改，都会触发root目录修改
            directoryChanged(path);
            QDir dir(path);
            /// 文件删除了
            if(!dir.exists()){
                w->deleteLater();
                qDebug() << "directory deleted!";
            }

        });
    }

    /**
     * @brief watchDirTree
     * 用指定的文件检测器检测指定目录。
     *
     * 一个wather监视一个目录，子目录用子watcher监视。
     * 目录删除了，就删除watcher
     * 目录新增了，就新增watcher
     * @param wather
     * @param root
     */
    bool watchDirTree(QFileSystemWatcher* wather,QString root,bool recursive){
        Q_ASSERT(wather != NULL);

        if(wather == NULL){
            return false;
        }

        QFileInfo info(root);
        if(!info.isDir()){
            return false;
        }
        qDebug() << root;
        wather->addPath(root);

        QDir dir(root);
        QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs | QDir::NoSymLinks);
        foreach(auto p, fileInfoList){
            if(!p.isDir()){
                continue;
            }
            QFileSystemWatcher* subWatcher = new QFileSystemWatcher(wather);
            initSignalProcess(subWatcher);
            watchDirTree(subWatcher,p.filePath(),recursive);
        }
        return true;
    }

protected:
    QMap<QString,QFileSystemWatcher*> m_rootWatchers;
};

}
