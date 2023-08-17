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
#include <QDebug>

namespace utiles {

class DatabaseUpdater{
public:
    DatabaseUpdater(){

    }
    ~DatabaseUpdater(){

    }

    /**
     * @brief databaseInit
     * 数据库表创建语句。
     * @param creates 数据库建表语句,例如CREATE TABLE IF NOT EXISTS XXXX...
     * @return
     */
    bool databaseInit(QSqlDatabase& db,QStringList creates,QString& error){
        if(!db.isOpen()){
            error = QStringLiteral("数据库打开失败，无法初始化数据库");
            return false;
        }

        QSqlQuery q(db);
        foreach(const auto& p,creates){
            if(!q.exec(p)){
                error = QStringLiteral("数据库表初始化失败。\n%1。\n%2").
                        arg(q.lastError().text()).arg(p);
                return false;
            }
        }
        return true;
    }

    /**
     * @brief getTableFieldNames
     * @return
     */
    bool getTableFieldNames(QSqlDatabase& db,const QString& table,QStringList& fields){
        QSqlQuery q(db);
        /// pragma table_info(record_list)
        QString sql = QString("pragma table_info(%1)").arg(table);
        if(!q.exec(sql)){
            qWarning() << QStringLiteral("sql执行失败:%1").arg(q.lastError().text());
            qWarning() << sql;
            return false;
        }
        fields.clear();
        while(q.next()){
            fields.append(q.value("name").toString());
        }
        return true;
    }


    /**
     * @brief addColumnToTable
     * @param name
     * @param defineString
     * @return
     */
    bool addColumnToTable(QSqlDatabase& db,const QString& table, const QString& col_name,const QString& defineString){
        QSqlQuery q(db);
        /// pragma table_info(record_list)
        QString sql = QString("ALTER TABLE '%1' ADD COLUMN '%2' %3").arg(table).arg(col_name).arg(defineString);
        if(!q.exec(sql)){
            qWarning() << QStringLiteral("sql执行失败:%1").arg(q.lastError().text());
            qWarning() << sql;
            return false;
        }
        return true;
    }

    /**
     * @brief addColumnToTableIfNotExist
     * @param db
     * @param table
     * @param new_feilds
        //        new_feilds.append("data_type");
        //        new_feild_defines.append("TEXT");
     * @param new_feild_defines
     * @return
     */
    bool addColumnToTableIfNotExist(QSqlDatabase& db,const QString& table,
                          const QVector<QString>& new_feilds,
                          const QVector<QString>& new_feild_defines){
        if(new_feilds.isEmpty() || new_feild_defines.isEmpty()){
            return true;
        }
        QStringList fields;
        utiles::DatabaseUpdater updater;
        if(!getTableFieldNames(db,table,fields)){
            qWarning() << QStringLiteral("获取表%1的结构失败,无法判断是否需要升级表结构").arg(table);
            return false;
        }
        if(new_feilds.size() != new_feild_defines.size()){
            qWarning() << QStringLiteral("需要新增的字段信息不全或者有冲突");
            return false;
        }
        bool updated = false;
        for(int i=0,cnt=new_feilds.size(); i<cnt; i++){
            if(fields.contains(new_feilds[i])){
                continue;
            }
            if(!addColumnToTable(db,table,new_feilds[i],new_feild_defines[i])){
                qWarning() << QStringLiteral("需要新增的字段信息不全或者有冲突");
                return false;
            }
            updated = true;
            qDebug() << QStringLiteral("表%1新增了字段%2 %3").arg(table).arg(new_feilds[i]).arg(new_feild_defines[i]);
        }
        if(updated){
            qDebug() << QStringLiteral("表%1升级完成").arg(table);
        } else {
            qDebug() << QStringLiteral("表%1无需升级").arg(table);
        }
        return true;
    }
};

}
