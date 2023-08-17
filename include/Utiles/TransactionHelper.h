#pragma once

#include <QSqlQuery>
#include <QMap>
#include <QMutex>


namespace utiles {

/**
 * @brief The AutoTransactionRollback class
 * 自动回滚,不会多次提交。不会多次回滚。
 * 线程安全，支持嵌套。
 */
class TransactionHelper{
private:
    enum State{
        NO_TRANSACTION,
        IN_TRANSACTION,
    };
public:
    /**
     * @brief TransactionHelper
     * @param db
     * @param begin     比如,MySQL使用：START TRANSACTION;
     *                      SQLite使用：BEGIN TRANSACTION;
     *                  空就是使用默认
     *
     * @param commit    比如：COMMIT;
     *                  空就是使用默认
     *
     * @param rollback  比如：ROLLBACK;
     *                  空就是使用默认
     */
    TransactionHelper(QSqlDatabase* db,
                      QString begin = "",QString commit = "",QString rollback = ""):m_db(db){
        Q_ASSERT(db != NULL);
        if(begin.isEmpty()){
            begin = getBeginSql(db);
        }
        if(commit.isEmpty()){
            commit = getCommitSql(db);
        }
        if(rollback.isEmpty()){
            rollback = getRollbackSql(db);
        }
        m_start_trans = begin;
        m_commit_trans = commit;
        m_rollback_trans = rollback;
        m_mydb_name = db->connectionName();
    }
    ~TransactionHelper(){
        rollback();
    }

    /**
     * @brief run
     * 自动开启事务，然后执行fun函数中的操作，最后提交事务
     * 例如：
     *      utiles::TransactionHelper trans(&m_Impl->m_db);
     *      trans.run([](QString& error){
     *          xxxxx;
     *          xxxxx;
     *          if(xxx){
     *              error = "xxxx";
     *              return false;
     *          }
     *          return true;
     *      });
     * @param fun 在事务中执行的函数，成功执行返回true，只有执行成功才会提交事务。
     *      fun的error参数就是run的error参数值.
     * @return
     */
    bool run(std::function<bool(QString& error)> fun,QString& error){
        if(!begin(error)){
            error.insert(0,"Cannot start transaction.");
            return false;
        }
        if(!fun(error)){
            rollback();
            return false;
        }
        if(!commit(error)){
            error.insert(0,"Cannot commit transaction.");
            rollback();
            return false;
        }
        return true;
    }

    QString getBeginSql(QSqlDatabase* db){
        QString typeName = db->driverName();
        if(typeName == "QSQLITE" || typeName == "QODBC"){
            return "BEGIN TRANSACTION;";
        } else if(typeName == "QMYSQL"){
            return "START TRANSACTION;";
        } else {
            return "BEGIN TRANSACTION;";
        }
    }

    QString getCommitSql(QSqlDatabase* db){
        return "COMMIT;";
    }
    QString getRollbackSql(QSqlDatabase* db){
        return "ROLLBACK;";
    }

    /**
     * @brief begin
     * 如果没开启事务就开启事务
     * @param error
     * @return
     */
    bool begin(QString& error){
        if(m_state == IN_TRANSACTION){
            return true;
        }
        if(do_begin_transaction(*m_db,error)){
            m_state = IN_TRANSACTION;
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief commit
     * 如果没提交或者回滚就提交
     * @param error
     * @return
     */
    bool commit(QString& error){
        if(m_state == NO_TRANSACTION){
            return true;
        }
        if(do_commit(*m_db,error)){
            m_state = NO_TRANSACTION;
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief commit
     * 如果没提交或者回滚就回滚
     * @param error
     * @return
     */
    void rollback(){
        if(m_state == NO_TRANSACTION){
            return;
        }
        if(do_rollback(*m_db)){
            m_state = NO_TRANSACTION;
        } else {
        }
    }

private:
    bool do_begin_transaction(QSqlDatabase& db,QString& error){
        QMutexLocker l(&g_transaction_mutex);
        QString connect_name = db.connectionName();
        if(m_transaction_count.contains(connect_name) && m_transaction_count[connect_name] >= 1){
            m_transaction_count[connect_name] ++;
            return true; /// 外层已经有事务了
        }
        QSqlQuery query(db);
        if(!query.exec(m_start_trans)) {
            error = query.lastError().text();
            return false;
        }
        m_transaction_count[connect_name] ++;
        return true;
    }

    bool do_commit(QSqlDatabase& db,QString&error){
        QMutexLocker l(&g_transaction_mutex);
        QString connect_name = db.connectionName();
        if(m_transaction_count.contains(connect_name) && m_transaction_count[connect_name] > 1){
            m_transaction_count[connect_name] --;
            return true; /// 外层还有事务未提交
        }
        Q_ASSERT(m_transaction_count[connect_name] == 1);
        QSqlQuery query(db);
        if(!query.exec(m_commit_trans)) {
            error = query.lastError().text();
            return false;
        }
        m_transaction_count.remove(connect_name);
        return true;
    }
    bool do_rollback(QSqlDatabase& db){
        QMutexLocker l(&g_transaction_mutex);
        QString connect_name = db.connectionName();
        if(m_transaction_count.contains(connect_name) && m_transaction_count[connect_name] > 1){
            m_transaction_count[connect_name] --;
            return true; /// 外层还有事务未提交或回滚
        }
        Q_ASSERT(m_transaction_count[connect_name] == 1);
        QSqlQuery query(db);
        if(!query.exec(m_rollback_trans)) {
            return false;
        }
        m_transaction_count.remove(connect_name);
        return true;
    }

protected:
    /// 当前事务状态
    State m_state = NO_TRANSACTION;
    /// 待事务管理的连接
    QSqlDatabase* m_db = NULL;
    QString m_mydb_name;
    /// 事务锁
    static QMutex g_transaction_mutex;
    /// 整体事务嵌套层数
    /// 有一个事务请求对应连接的值就是1，如再有请求值便+1
    /// 每提交或者回滚一次值减1，减到最后就真正的提交或者回滚
    static QMap<QString,int> m_transaction_count;
    QString m_start_trans;
    QString m_commit_trans;
    QString m_rollback_trans;
};

QMutex TransactionHelper::g_transaction_mutex;
QMap<QString,int> TransactionHelper::m_transaction_count;
}
