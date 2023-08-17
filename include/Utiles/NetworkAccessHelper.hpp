/*******************************************************************************
* @file   .hpp
* @brief
*
* ...detail describe
*
* @version   V1.0.1
* @author    LiFeng,
* @date      2022-05-15
*
* @note
*******************************************************************************/

#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

namespace utiles {

class NetworkAccessHelper{
public:
    NetworkAccessHelper(){
    }
    NetworkAccessHelper(bool debug){
        m_log = debug;
    }

    /**
     * @brief waitNetworkReply
     * @param reply
     * @param error
     * @return
     */
    bool waitNetworkReply(QNetworkReply* reply,int timout_ms,QString& error){
        QEventLoop looper;
        QTimer t;
        t.setSingleShot(true);
        t.start(timout_ms);
        QObject::connect(reply,SIGNAL(finished()),&looper,SLOT(quit()));
        QObject::connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),&looper,SLOT(quit()));
        QObject::connect(&t,SIGNAL(timeout()),&looper,SLOT(quit()));
        looper.exec();
        if(reply->error() != QNetworkReply::NoError){
            error = reply->errorString();
            return false;
        }
        if(!t.isActive()){
            error = QStringLiteral("请求超时");
            return false;
        }
        return true;
    }

    /**
     * @brief request
     * 发起网络请求
     * @param url
     * @param rsp
     * @param error
     * @return
     */
    bool request(QString url,QString& rsp,int timeout_ms,QString& error){
        QNetworkAccessManager mgr;
        if(m_log){
            qDebug() << "Req:" << url;
        }
        /// fix qt bug: Network access is disabled
        mgr.clearAccessCache();
        mgr.clearConnectionCache();
        mgr.setNetworkAccessible(QNetworkAccessManager::Accessible);
        QNetworkReply* reply = mgr.get(QNetworkRequest(url));
        if(!waitNetworkReply(reply,timeout_ms,error)){
            reply->deleteLater();
            //qWarning() << error;
            return false;
        }
        rsp = reply->readAll();
        if(m_log){
            qDebug() << "Rsp:" << rsp;
        }
        reply->deleteLater();
        return true;
    }

private:
    bool m_log = true;
};

}
