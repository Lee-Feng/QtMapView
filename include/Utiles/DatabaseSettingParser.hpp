#pragma once

#ifdef QT_SQL_LIB
#include <QSqlDatabase>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#endif
#include <QFile>
#include <QSettings>
#include <QUuid>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

namespace utiles {

class DatabaseInfo
{
public:
    ///数据库类型
    ///QDB2 		IBM DB2
    ///QIBASE		Borland InterBase Driver
    ///QMYSQL		MySQL Driver
    ///QOCI		Oracle Call Interface Driver
    ///QODBC		ODBC Driver (includes Microsoft SQL Server)
    ///QPSQL		PostgreSQL Driver
    ///QSQLITE          SQLite version 3 or above
    ///QSQLITE2         SQLite version 2
    ///QTDS		Sybase Adaptive Server
    QString type;
    QString hostname;
    ///0代表无效
    int     port;
    ///数据库名字
    QString database;
    QString username;
    QString password;
    ///连接名字
    QString connectname;
    ///连接空闲时间，单位秒，超过该时间没用的连接将自动废弃
    int     idle;
    ///连接池的大小,最小值就是1
    int     poolsize;

    DatabaseInfo()
    {
        port = 0;
        idle = 0;
        poolsize = 0;
    }

    QString toString() const{
        return QString("[%1/%2]-%3:%4").arg(type).arg(database).arg(hostname).arg(port);
    }
};

class DatabaseSettingParser{
public:
    DatabaseSettingParser(){}
    virtual ~DatabaseSettingParser(){}

    int GetDefaultPort(const QString& type)
    {
        int port = 0;
        if(type.compare("QDB2",Qt::CaseInsensitive) == 0){
            port = 50000;
        } else if(type.compare("QIBASE",Qt::CaseInsensitive) == 0){
            port = 3050;
        } else if(type.compare("QMYSQL",Qt::CaseInsensitive) == 0){
            port = 3306;
        } else if(type.compare("QOCI",Qt::CaseInsensitive) == 0){
            port = 1521;
        } else if(type.compare("QODBC",Qt::CaseInsensitive) == 0){
            port = 1433;
        } else if(type.compare("QPSQL",Qt::CaseInsensitive) == 0){
            port = 5432;
        } else if(type.compare("QSQLITE",Qt::CaseInsensitive) == 0){

        } else if(type.compare("QSQLITE2",Qt::CaseInsensitive) == 0){

        } else if(type.compare("QTDS",Qt::CaseInsensitive) == 0){
            port = 5000;
        }

        return port;
    }

    /**
     * @brief GetDatabaseInfo
     * 解析INI文件，提取数据库信息
     * @param ini_file
     * @param info
     * @return
     */
    bool GetDatabaseInfo(QString ini_file,DatabaseInfo& info)
    {
        QSettings set(ini_file,QSettings::IniFormat);
        if(set.status() != QSettings::NoError)
        {
            return false;
        }
        set.beginGroup("database");
        info.type = set.value("type","").toString();
        info.connectname = set.value("connectname","").toString();
        if(info.connectname.isEmpty()){
            info.connectname = QUuid::createUuid().toString();
        }
        info.hostname = set.value("hostname","localhost").toString();
        info.port = set.value("port",0).toInt();
        if(info.port <= 0){
            info.port = GetDefaultPort(info.type);
        }
        info.database = set.value("database","").toString();
        info.username = set.value("username","").toString();
        info.password = set.value("password","").toString();
        info.idle = set.value("idle",0).toInt();
        info.poolsize = set.value("poolsize",1).toInt();
        if(info.poolsize < 1){
            info.poolsize = 1;
        }
        set.endGroup();
        return true;
    }

#ifdef QT_SQL_LIB
    /**
     * @brief CreateDatabaseByIniFile
     * 创建一个数据库连接，内部没有open，调用者根据情况自行open
     * @param filename
     * @return 如果创建失败，返回的连接将是isValid为false
     */
    QSqlDatabase CreateDatabaseByIniFile(QString filename)
    {
        DatabaseInfo info;
        if(!GetDatabaseInfo(filename,info))
        {
            return QSqlDatabase();
        }
        return CreateDatabase(info);
    }

    QSqlDatabase CreateDatabase(const DatabaseInfo& info)
    {
        return CreateDatabase(info,info.connectname);
    }
    QSqlDatabase CreateDatabase(const DatabaseInfo& info,QString connection_name)
    {
        QSqlDatabase dataBase;
        dataBase = QSqlDatabase::addDatabase(info.type,connection_name);
        dataBase.setHostName(info.hostname);
        dataBase.setPort(info.port);
        dataBase.setUserName(info.username);
        dataBase.setPassword(info.password);
        dataBase.setDatabaseName(info.database);
        if(info.type.contains("QSQLITE",Qt::CaseInsensitive) && !createSqliteDbDir(info.database)){
            qWarning() << QStringLiteral("数据库(%1)chuang创建本地目录失败").arg(info.toString());
        }
        return dataBase;
    }
#endif

    bool createSqliteDbDir(const QString& db){
        QFileInfo f(db);
        if(f.exists()){
            return true;
        }
        QDir dir = f.dir();
        if(dir.exists()){
            return true;
        }
        QDir curDir;
        return curDir.mkpath(dir.path());
    }

};

}
