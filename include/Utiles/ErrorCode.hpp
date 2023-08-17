#pragma once

#include <QFileSystemWatcher>
#include <QString>
#include <QMap>
#include <QMutex>
#include <QSettings>
#include <QDebug>

namespace utiles {
class ErrorCode{
public:
    /**
     * @brief ErrorCode
     * @param errorcode_file 参考ErrorCode_samples.ini
     */
    ErrorCode(const QString& errorcode_file,const QString& group){
        m_errorcode_file = errorcode_file;
        m_errorcode_group = group;
        m_wather.addPath(errorcode_file);
        m_con = QObject::connect(&m_wather,&QFileSystemWatcher::fileChanged,[=](){
            load();
        });
        load();
    }
    ~ErrorCode(){
        QObject::disconnect(m_con);
    }

    /**
     * @brief load
     * @return
     */
    bool load(){
        qDebug() << (QStringLiteral("解析文件:[%1]...").arg(m_errorcode_file));
        QSettings st(m_errorcode_file,QSettings::IniFormat);
        QMap<int,QString> codes;
        /// 解析
        st.beginGroup(m_errorcode_group);
        QStringList keys = st.childKeys();
        foreach (const QString& k, keys) {
            bool ok = false;
            int key_value = k.toInt(&ok);
            if(!ok){
                qWarning() << (QStringLiteral("错误号[%1]不是有效整数").arg(k));
                continue;
            }
            if(codes.contains(key_value)){
                qWarning() << (QStringLiteral("错误号[%1]已经存在了,替换原来错误号").arg(key_value));
            }
            codes[key_value] = st.value(k,"").toString();
        }
        st.endGroup();
        ///
        QMutexLocker l(&m_mutex);
        m_codes.swap(codes);
        qDebug() << (QStringLiteral("解析文件:[%1]完成,数据已经生效").arg(m_errorcode_file));
        return true;
    }

    /**
     * @brief toString
     * 获取错误描述
     * @param code
     * @return
     */
    QString toString(int code){
        QMutexLocker l(&m_mutex);
        if(m_codes.contains(code) && !m_codes[code].isEmpty()){
            return QString("%1(code=%2)").arg(m_codes[code]).arg(code);
        }
        return QStringLiteral("未知错误(code=%1)").arg(code);
    }

protected:
    QMutex m_mutex;
    QMap<int,QString> m_codes;
    QString m_errorcode_file;
    QString m_errorcode_group;
    QFileSystemWatcher m_wather;
    QMetaObject::Connection m_con;
};

}
