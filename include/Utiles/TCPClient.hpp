#pragma once
#include <QMutex>
#include <QTcpSocket>
#include <QTimer>
#include <QHostAddress>

namespace utiles {

/**
 * @brief The TCPClient class
 * 使用例子：
 *  TCPClient* client = new TCPClient();
 *  client->connectToHost(QHostAddress(ip),port);
 *  client->setLog(log);
 *  QObject::connect(client,&QIODevice::readyRead,std::bind(&Impl::onReadReady,this));
 */
class TCPClient : public QTcpSocket{
    Q_OBJECT
public:
    TCPClient(QObject *parent = nullptr):QTcpSocket(parent){
        connect(this,SIGNAL(connected()),this,SLOT(onConnected()));
        connect(this,SIGNAL(disconnected()),this,SLOT(onDisconnected()));
        connect(this,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(onStateChanged(QAbstractSocket::SocketState)));
        connect(this,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onError(QAbstractSocket::SocketError)));
        m_reconnectTimer = new QTimer(this);
        m_reconnectTimer->setSingleShot(true);
        connect(m_reconnectTimer,SIGNAL(timeout()),this,SLOT(reconnecting()));

        m_connectTimer = new QTimer(this);
        m_connectTimer->setSingleShot(true);
        connect(m_connectTimer,SIGNAL(timeout()),this,SLOT(onConnectTimeout()));
    }
    virtual ~TCPClient(){
        disconnectFromHostNoReconn();
    }

    void setLog(bool log){
        m_log = log;
    }

    bool log(){
        return m_log;
    }

    void setConnectTimeout(int timeout){
        m_connect_timeout_ms = timeout;
    }

    /**
     * @brief connectToHost
     * 有超时的连接
     * @param hostName
     * @param port
     * @param openMode
     * @param protocol
     */
    virtual void connectToHost(const QString &hostName, quint16 port,
                               QIODevice::OpenMode openMode = ReadWrite,
                               QAbstractSocket::NetworkLayerProtocol protocol = AnyIPProtocol){
        if(m_log){
            qDebug() << QString("Connectting[%1]....").arg(targetString());
        }

        m_hostName = hostName;
        m_port = port;
        m_openMode = openMode;
        m_protocol = protocol;
        connecttedByHostname = true;

        QTcpSocket::connectToHost(hostName,port,openMode,protocol);

        m_manual_closed = false;
        startCheckConnectTimeout();
    }
    /**
     * @brief connectToHost
     * 有超时的连接
     * @param address
     * @param port
     * @param openMode
     */
    virtual void connectToHost(const QHostAddress &address, quint16 port, QIODevice::OpenMode openMode = ReadWrite){
        if(m_log){
            qDebug() << QString("Connectting[%1]....").arg(targetString());
        }

        m_address = address;
        m_port = port;
        m_openMode = openMode;
        connecttedByHostname = false;

        QTcpSocket::connectToHost(address,port,openMode);

        m_manual_closed = false;
        startCheckConnectTimeout();
    }

    /**
     * @brief disconnectFromHost
     * 断开连接，不在重连。
     */
    virtual void disconnectFromHostNoReconn(){
        QTcpSocket::disconnectFromHost();
        m_manual_closed = true;
        m_reconnectTimer->stop();
        if(m_log){
            qDebug() << QString("Disconnect no reconn from[%1]!").arg(targetString());
        }
    }
    /**
     * @brief setAutoReconnect
     * 设置断开自动连接
     * @param reconnect
     */
    void setAutoReconnect(bool reconnect){
        m_reconnect = reconnect;
        if(m_reconnect){
            m_reconnectTimer->start(m_reconnect_gap_ms);
        }
    }
    void setReconnectGap(int gap_ms){
        m_reconnect_gap_ms = gap_ms;
    }
    int getReconnectGap(){
        return m_reconnect_gap_ms;
    }
    /**
     * @brief isAutoReconnectedOn
     * 是否开启了自动重连
     * @return
     */
    bool isAutoReconnectedOn(){
        return m_reconnect;
    }

    bool isConnected(){
        return QAbstractSocket::ConnectedState == state();
    }

private slots:

    QString toString(QAbstractSocket::SocketState st){
        QString ret = "";
        switch (st) {
        case UnconnectedState:
            ret = "UnconnectedState";
            break;
        case HostLookupState:
            ret = "HostLookupState";
            break;
        case ConnectingState:
            ret = "ConnectingState";
            break;
        case ConnectedState:
            ret = "ConnectedState";
            break;
        case BoundState:
            ret = "BoundState";
            break;
        case ListeningState:
            ret = "ListeningState";
            break;
        case ClosingState:
            ret = "ClosingState";
            break;
        default:
            ret = QStringLiteral("Unkown(%1)").arg(st);
        }
        return ret;
    }
    void onStateChanged(QAbstractSocket::SocketState socketState){
        if(m_log){
            qDebug() << QStringLiteral("[%1] state = [%2]!").arg(targetString()).arg(toString(socketState));
        }
    }
    void startCheckConnectTimeout(){
        /// 检测连接超时
        if(m_connect_timeout_ms > 0){
            /// 时间到了回调：onConnectTimeout
            m_connectTimer->start(m_connect_timeout_ms);
        }
    }
    void onConnectTimeout(){
        if(m_log){
            qDebug() << QStringLiteral("Connect to [%1] timeout!").arg(targetString());
        }
        QTcpSocket::abort();
        delayAndReconnect();
    }

    void onConnected(){
        m_connectTimer->stop();
        m_reconnectTimer->stop();
        /// 避免自己连自己
        if(localAddress() == peerAddress() && localPort() == peerPort()){
            qDebug() << QStringLiteral("loop connected, try reconnect!");
            QTcpSocket::abort();
        } else {
            if(m_log){
                qDebug() << QStringLiteral("Connect to [%1] successful!").arg(targetString());
            }
        }
    }
    void onDisconnected(){
        if(m_log){
            qDebug() << QStringLiteral("[%1] disconnected").arg(targetString());
        }
        delayAndReconnect();
    }
    void onError(QAbstractSocket::SocketError /*socketError*/){
        if(m_log){
            qDebug() << QStringLiteral("error : [%1] %2").arg(targetString()).arg(errorString());
        }
        QTcpSocket::abort();
        delayAndReconnect();
    }
    void delayAndReconnect(){
        if(!m_manual_closed && m_reconnect_gap_ms > 0){
            /// 时间到了回调：reconnecting
            m_reconnectTimer->start(m_reconnect_gap_ms);
        }
    }

    void reconnecting(){
        if(state() == QAbstractSocket::ConnectedState){
            if(m_log){
                qDebug() << QString("Connected to [%1] successful,no need to reconnect!").arg(targetString());
            }
            return;
        }
        if(m_manual_closed){
            if(m_log){
                qDebug() << QString("Connected cancled[%1],no need to reconnect!").arg(targetString());
            }
            return;
        }
        if(m_log){
            qDebug() << QString("Reconnect to [%1] ...").arg(targetString());
        }
        if(connecttedByHostname){
            connectToHost(m_hostName,m_port,m_openMode,m_protocol);
        } else {
            connectToHost(m_address,m_port,m_openMode);
        }
    }

    QString targetString(){
        return QString("%1:%2").arg(connecttedByHostname ? m_hostName : m_address.toString()).arg(m_port);;
    }

protected:
    /// 连接超时
    int m_connect_timeout_ms = 3000;
    /// 重连间隔
    bool m_reconnect = true;
    int m_reconnect_gap_ms = 500;
    QTimer* m_reconnectTimer;
    QTimer* m_connectTimer;
    bool m_manual_closed = false;
    bool m_log = false;
protected:
    bool connecttedByHostname = false;
    QHostAddress m_address;
    QString m_hostName;
    quint16 m_port;
    QIODevice::OpenMode m_openMode = ReadWrite;
    QAbstractSocket::NetworkLayerProtocol m_protocol = AnyIPProtocol;
};
}
