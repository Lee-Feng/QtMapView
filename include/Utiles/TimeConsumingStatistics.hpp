#pragma once

#include <QString>
#include <QObject>
#include <QMap>
#include <QMapIterator>
#include <QDateTime>
#include <QDebug>
#include <memory>
#include <QMutex>


namespace utiles {
namespace private_date {
class Timer;
}
}
/// 计算结点
typedef std::shared_ptr<utiles::private_date::Timer> CalNode;

namespace utiles {

class IStatistics{
public:
    virtual ~IStatistics(){}
    virtual void updateTimeCost(QString key,qint64 cost) = 0;
};

namespace private_date {
    class Timer{
    public:
        Timer(IStatistics* statics,QString key):m_statics(statics),m_key(key){
            m_start = QDateTime::currentDateTime();
            m_end = false;
        }
        ~Timer(){
            end();
        }
        void makeComplierHappy(){

        }

        void end(){
            if(this == NULL){
                return;
            }
            if(!m_end){
                QDateTime now = QDateTime::currentDateTime();
                qint64 costed = m_start.msecsTo(now);
                if(costed >= 0){
                    m_statics->updateTimeCost(m_key,costed);
                }
            }
            m_end = true;
        }
    private:
        QDateTime m_start;
        IStatistics* m_statics;
        QString m_key;
        bool m_end;
    };
}


class TimeConsumingStatistics : public IStatistics{
public:
    TimeConsumingStatistics(const QString& title):m_mutex(QMutex::Recursive){
        m_title = title;
        m_last_log_time = QDateTime::currentDateTime();
    }
    TimeConsumingStatistics(const QString& title,bool enable,int size):m_mutex(QMutex::Recursive){
        m_title = title;
        m_last_log_time = QDateTime::currentDateTime();
        m_size = size;
        setEnable(enable);
    }
    ~TimeConsumingStatistics(){

    }

    /**
     * @brief setEnable
     * 开启或者关闭统计
     * @param en
     */
    void setEnable(bool en){
        m_enable = en;
    }

    /**
     * @brief update
     * 更新统计结果
     * @param key
     */
    CalNode update(const QString& key){
        QMutexLocker l(&m_mutex);
        if(!m_enable){
            return std::shared_ptr<private_date::Timer>();
        }
        return std::shared_ptr<private_date::Timer>(new private_date::Timer(this,key));
    }

    /**
     * @brief tryLog
     * 如果使能的，并且与上次打印时间检测到了gap_suc秒就打印一次
     * @param gap_suc
     */
    void tryLog(int gap_suc){
        QMutexLocker l(&m_mutex);
        if(!m_enable){
            return;
        }

        QDateTime now = QDateTime::currentDateTime();
        if(m_last_log_time.secsTo(now) >= gap_suc){
            m_last_log_time = now;
            log();
        }
    }

    void setTitle(QString t){
        m_title = t;
    }


    /**
     * @brief log
     * 根据记录数据打印统计结果
     */
    void log(){
        QMutexLocker l(&m_mutex);
        QMapIterator<QString, QList<quint64> > it(m_times);
        QStringList logs;
        int nameWidth = 22;
        int width = 11;

        int title_len = m_title.length();
        logs.append("");
        logs.append("==========================================================");
        int space_len = logs.last().length()-title_len;
        QString spaced_title = m_title;
        if(space_len > 0){
            spaced_title = QString("%1%2%1").arg(" ",space_len/2).arg(m_title);
        }
        logs.append(QString("%1").arg(spaced_title,logs.last().length()));
        logs.append("----------------------------------------------------------");
        logs.append("          NAME        |   AVG(ms) |   MIN(ms) |   MAX(ms)");
        logs.append("----------------------------------------------------------");
        while(it.hasNext()){
            it.next();
            quint64 agv = 0;
            quint64 min = 0;
            quint64 max = 0;
            calcResult(it.value(),agv,min,max);
            QString key = it.key();
            key = key.right(nameWidth);
            logs.append(QString("%1|%2|%3|%4").arg(key,nameWidth).arg(agv,width).arg(min,width).arg(max,width));
        }
        logs.append("----------------------------------------------------------");
        qDebug() << qPrintable(logs.join("\n"));
    }

protected:
    /**
     * @brief calcResult
     * 计算统计结果
     * @param src
     * @param agv
     * @param min
     * @param max
     */
    void calcResult(const QList<quint64> & src, quint64& agv, quint64& min, quint64& max){
        if(src.isEmpty()){
            agv = 0;
            min = 0;
            max = 0;
            return;
        }
        agv = 0;
        min = ULLONG_MAX;
        max = 0;
        quint64 sum = 0;
        foreach(auto p,src){
            sum += p;
            if(p < min){
                min = p;
            }
            if(p > max){
                max = p;
            }
        }
        agv = sum / src.size();
    }

private:
    void updateTimeCost(QString key,qint64 cost){
        QMutexLocker l(&m_mutex);

        m_times[key].append(cost);
        while(m_times[key].size() > m_size){
            m_times[key].takeFirst();
        }
    }

protected:
    QMap<QString, QList<quint64> > m_times;
    QDateTime m_last_log_time;
    bool m_enable = true;
    friend class Timer;
    int m_size = 100;
    QString m_title;
    QMutex m_mutex;
};

}
