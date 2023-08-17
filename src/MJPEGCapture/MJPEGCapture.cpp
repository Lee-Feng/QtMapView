#include "MJPEGCapture.h"
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include <QNetworkProxyFactory>
#include <QThread>
#include <QMutex>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include <QPixmap>

class MJPEGCapture::Impl :public  QThread{
public:
    bool m_simulation = true;

    QString m_stream_url;
    bool isAutoReconnect = true;
    bool m_startCalled = false;
    Impl(MJPEGCapture* pthis):m_pthis(pthis){
        m_lastFrame = QDateTime::currentDateTime();
    }
    ~Impl(){
        m_startCalled = false;
        requestInterruption();
        exitCurrentLoop();
        wait();
    }

    double getFps(){
        return m_fps;
    }

    void setFps(double fps){
        if(qAbs(m_fps - fps) > 1){
            m_fps = fps;
            m_pthis->fpsChanged();
        }
    }

    QImage getImage(){
        QMutexLocker l(&m_mutex);
        return m_curImage;
    }

    void setLoop(QEventLoop* loop){
        QMutexLocker l(&m_mutex);
        m_loop = loop;
    }

    void exitCurrentLoop(){
        QMutexLocker l(&m_mutex);
        if(m_loop != NULL){
            m_loop->quit();
        }
    }

protected:
    int HEADER_MAX_LENGTH = 100;
    int FRAME_MAX_LENGTH = 260000 + HEADER_MAX_LENGTH;
    int FRAME_BUF_LENGTH = (int)(FRAME_MAX_LENGTH * 2);

    /**
     * @brief getEndOfSeqeunce
     * 在data中寻找toFind，并返回toFind的尾巴索引后面的起始位置
     * @param data
     * @param toFind
     * @return -1代表没找i到
     */
    int getEndOfSeqeunce(QByteArray data,QByteArray toFind){
        int idx = data.indexOf(toFind);
        if(idx < 0){
            return idx;
        }
        return idx + toFind.size();
    }
    /**
     * @brief getStartOfSequence
     * 在data中寻找toFind，并返回toFind的头部索引位置
     * @param data
     * @param toFind
     * @return
     */
    int getStartOfSequence(QByteArray data,QByteArray toFind){
        return data.indexOf(toFind);
    }

    /**
     * @brief parseContentLength
     * 解析Content-length的值
     * @param headerData 只是http的header数据
     * @return -1如果没找到
     */
    int parseContentLength(QByteArray headerData)
    {
        QString strContLen = "";
        QByteArray tag("Content-length");
        int tempIndex = headerData.indexOf(tag);
        if (tempIndex == -1) {
            tempIndex = headerData.indexOf("Content-Length");
        }
        if (tempIndex == -1) {
            return -1;
        }
        int value_start_pos = headerData.indexOf(':',tempIndex);
        if(value_start_pos == -1){
            return  -1;
        }
        strContLen = headerData.mid(value_start_pos+1);
        bool ok = false;
        int len = strContLen.toInt(&ok);
        return ok ? len : -1;
    }

    void onGettedImageData(QByteArray data){
        m_imagebuf.append(data);
        /// 去掉多余的
        if(m_imagebuf.size() > FRAME_BUF_LENGTH){
            m_imagebuf = m_imagebuf.mid(m_imagebuf.size() - FRAME_BUF_LENGTH,-1);
        }

        const char JEPG_SOI[3] = {(char)0xFF,(char)0xD8};
        const char JEPG_EOF[3] = {(char)0xFF,(char)0xD9};
        QByteArray SOI_MARKER(JEPG_SOI,2);
        QByteArray EOF_MARKER(JEPG_EOF,2);

        int headerLen = getStartOfSequence(m_imagebuf, SOI_MARKER);
        if(headerLen == -1){
            return;
        }
        QByteArray header = m_imagebuf.left(headerLen);
        int contentLength  = parseContentLength(header);
        if(contentLength < 0){
            /// 头部中没找到，那么就直接用标记末尾
            contentLength = getEndOfSeqeunce(m_imagebuf, EOF_MARKER);
        }
        if(contentLength > 0){
            if(m_imagebuf.size() < headerLen + contentLength){
                //qDebug() << QStringLiteral("数据不够..");
                return;
            }
            /* 构建pixmap */
            QPixmap pixmap;
            QByteArray imagerawData = m_imagebuf.mid(headerLen,contentLength);
            if(pixmap.loadFromData(imagerawData)){
                qint64 cost = m_lastFrame.msecsTo(QDateTime::currentDateTime());
                QMutexLocker l(&m_mutex);
                double fps = 1.0 / cost;
                m_curImage = pixmap.toImage();
                m_pthis->imageChanged(m_curImage);
                setFps(fps);
            }

            /// 移除已经处理了的图片数据
            m_imagebuf = m_imagebuf.mid(headerLen+contentLength);
        }
    }

    void run(){
        qDebug() << QStringLiteral("取流开始...");
        setFps(0);
        while(!isInterruptionRequested()){
            if(m_stream_url.isEmpty()){
                QThread::msleep(100);
                continue;
            }
            qDebug() << QStringLiteral("取流(%1)...").arg(m_stream_url);
            bool isConnectTimeOut = false;

            QEventLoop loop;
            setLoop(&loop);

            /// 超时检测，用于检测连接超时，传输超时
            QTimer timeout;
            timeout.setInterval(1000);
            timeout.setSingleShot(true);
            timeout.start();

//            //httpclient.setRequestProperty("User-Agent", "VLC/2.0.0 LibVLC/2.0.0");
//            httpclient.setRequestProperty("User-Agent", "AOF LiveView");
//            httpclient.setDefaultUseCaches(false);
//            httpclient.setUseCaches(false);
//            httpclient.setRequestMethod("GET");
            QNetworkRequest req(m_stream_url);
            req.setHeader(QNetworkRequest::UserAgentHeader,"AOF LiveView");

            QNetworkAccessManager mgr;
            /// fix bug:Network access is disabled.
            mgr.setNetworkAccessible(QNetworkAccessManager::Accessible);
            QNetworkReply* reply = mgr.get(QNetworkRequest(m_stream_url));
            connect(reply,&QNetworkReply::readyRead,[&](){
                if(!m_startCalled){
                    // 退出等待
                    loop.quit();
                }
                timeout.stop();
                ///读取数据
                onGettedImageData(reply->readAll());
            });
            connect(reply,static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                    [=,&timeout,&loop](QNetworkReply::NetworkError ){
                qWarning() << reply->errorString();
                timeout.stop();
                m_pthis->error(reply->errorString());
                // 退出等待
                loop.quit();
            });

            /// 取完了也就停止超时计时,退出eventloop
            connect(reply,&QNetworkReply::finished,[&timeout,&loop](){
                qDebug() << QStringLiteral("取流结束");
                timeout.stop();
                loop.quit();
            });
            /// 连接超时
            connect(&timeout,&QTimer::timeout,[reply,this,&loop,&isConnectTimeOut](){
                QString e = QStringLiteral("目标系统连接超时:%1").arg(m_stream_url);
                reply->close();
                isConnectTimeOut = true;
                qWarning() << e;
                m_pthis->error(e);
                loop.quit();
            });
            loop.exec();

            setLoop(NULL);
            if(isAutoReconnect && m_startCalled){
                if(!isInterruptionRequested()){
                    QThread::msleep(500);
                }
                continue;
            }
            break;
        }
        setFps(0);
        qDebug() << QStringLiteral("退出取流(%1)").arg(m_stream_url);
    }

private:
    QMutex m_mutex;
    QDateTime m_lastFrame;
    double m_fps = 0;
    QImage m_curImage;
    MJPEGCapture* m_pthis;
    QByteArray m_imagebuf;
    QEventLoop *m_loop = NULL;
};

MJPEGCapture::MJPEGCapture()
{
    m_Impl = new Impl(this);
}

MJPEGCapture::~MJPEGCapture()
{
    delete m_Impl;
}

void MJPEGCapture::setStreamUrl(QString url)
{
    if(m_Impl->m_stream_url != url){
        m_Impl->m_stream_url = url;
        /// 触发重新获取
        m_Impl->exitCurrentLoop();
        streamUrlChanged(url);
    }
}

QString MJPEGCapture::getStreamUrl()
{
    return m_Impl->m_stream_url;
}

void MJPEGCapture::start()
{
    m_Impl->m_startCalled = true;
    m_Impl->start();
}

void MJPEGCapture::stop()
{
    m_Impl->m_startCalled = false;
    m_Impl->exitCurrentLoop();
    m_Impl->requestInterruption();
}

double MJPEGCapture::getFps()
{
    return m_Impl->getFps();
}

bool MJPEGCapture::isRunning()
{
    return m_Impl->isRunning();
}

QImage MJPEGCapture::getImage()
{
    return m_Impl->getImage();
}

void MJPEGCapture::setAutoConnect(bool autoConnect)
{
    if(m_Impl->isAutoReconnect != autoConnect){
        m_Impl->isAutoReconnect = autoConnect;
        autoConnectedChanged(autoConnect);
    }
}

bool MJPEGCapture::isAutoConnect()
{
    return m_Impl->isAutoReconnect;
}

