#pragma once

#include <QObject>
#include <QImage>

class MJPEGCapture : public QObject
{
    Q_OBJECT

    Q_PROPERTY(double fps READ getFps NOTIFY fpsChanged)
    Q_PROPERTY(bool running READ isRunning)
    Q_PROPERTY(bool autoConnect READ isAutoConnect WRITE setAutoConnect NOTIFY autoConnectedChanged)
    Q_PROPERTY(QString url READ getStreamUrl WRITE setStreamUrl NOTIFY streamUrlChanged)
    Q_PROPERTY(QImage image READ getImage NOTIFY imageChanged)

public:
    MJPEGCapture();
    ~MJPEGCapture();

public slots:
    /**
     * @brief setStreamUrl
     * 设置采集的流url
     * @param url
     */
    void setStreamUrl(QString url);

    /**
     * @brief getStreamUrl
     * 获取设置的url
     * @return
     */
    QString getStreamUrl();

    /**
     * @brief start
     * 启动采集
     */
    void start();

    /**
     * @brief stop
     * 停止采集
     */
    void stop();

    /**
     * @brief getFps
     * 获取当前帧率
     * @return
     */
    double getFps();

    /**
     * @brief isRunning
     * 当前是否正在采集
     * @return
     */
    bool isRunning();

    /**
     * @brief getImage
     * 获取当前图片
     * @return
     */
    QImage getImage();

    /**
     * @brief setAutoConnect
     * 自动重连
     */
    void setAutoConnect(bool autoConnect);
    bool isAutoConnect();

signals:
    void imageChanged(QImage image);
    void streamUrlChanged(QString url);
    void autoConnectedChanged(bool autoConnect);
    void error(QString error);
    void fpsChanged();

protected:
    class Impl;
    Impl* m_Impl;
};
