#pragma once

#include <QSettings>

#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PlatformThreadFactory.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TNonblockingServerSocket.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/TToString.h>
#include <thrift/stdcxx.h>

#include <boost/thread.hpp>
#include <logger/logger.hpp>


namespace utiles {

using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

template<typename ProcessorFactory,typename IfFactory>
class ThriftServerCreator{
public:

    ThriftServerCreator(FDRCIL_LoggerPtr logger){
        m_log = logger;
    }

    /**
     * @brief createThriftServer
     *
     * @param server_cfg_file
     *  是服务器配置文件。文件内容是：
     * [network]
        ; 参考CommunicationComp.xml。已经有一个服务暂用了8990.而这个咱用8991，他们的功能一样。只是这里走thrift接口。
        server_listen_port=8991
        ; 服务器类型:
        ; TThreadPoolServer,TNonblockingServer,TThreadedServer
        ; 默认是TThreadedServer
        server_type=TNonblockingServer

        ; 处理的线程池中的线程最大个数。
        ; 对于ThreadPoolServer，所以这里的个数就是能同时处理的客户端连接个数于是应该配置16*N的值(因为都是长连接)。
        ; 对于TThreadedServer,该参数无效。因为TThreadedServer为每一个连接都创建一个线程。
        ; 对于TNonblockingServer,该参数描述的是消息处理的线程个数，通常几个或者十几个足以。
        [TNonblockingServer]
        process_thread_cnt=20

        [TThreadPoolServer]
        process_thread_cnt=200
     *
     */
    bool createThriftServer(QString server_cfg_file,stdcxx::shared_ptr<IfFactory> iffactory){
        QSettings st(server_cfg_file,QSettings::IniFormat);
        int port = st.value("network/server_listen_port","8990").toInt();
        QString servertype = st.value("network/server_type","").toString();
        if(port > 0){
            int workerCount = getThreadCount(st,servertype);
            m_threadmanager = ThreadManager::newSimpleThreadManager(workerCount);
            m_threadmanager->threadFactory(stdcxx::make_shared<PlatformThreadFactory>());
            m_threadmanager->start();

            if(servertype.compare("TThreadPoolServer",Qt::CaseInsensitive) == 0){
                m_server = std::shared_ptr<TThreadPoolServer>(
                            new  TThreadPoolServer(
                                    stdcxx::make_shared<ProcessorFactory>(iffactory),
                                    stdcxx::make_shared<TServerSocket>(port),
                                    stdcxx::make_shared<TFramedTransportFactory>(),
                                    stdcxx::make_shared<TBinaryProtocolFactory>(),m_threadmanager)
                            );
            } else if(servertype.compare("TThreadedServer",Qt::CaseInsensitive) == 0){
                m_server = std::shared_ptr<TThreadedServer>(
                            new  TThreadedServer(
                                    stdcxx::make_shared<ProcessorFactory>(iffactory),
                                    stdcxx::make_shared<TServerSocket>(port),
                                    stdcxx::make_shared<TFramedTransportFactory>(),
                                    stdcxx::make_shared<TBinaryProtocolFactory>())
                            );
            } else if(servertype.compare("TNonblockingServer",Qt::CaseInsensitive) == 0){
                m_server = std::shared_ptr<TNonblockingServer>(
                            new  TNonblockingServer(
                                    stdcxx::make_shared<ProcessorFactory>(iffactory),
                                    stdcxx::make_shared<TBinaryProtocolFactory>(),
                                    stdcxx::make_shared<TNonblockingServerSocket>(port),
                                    m_threadmanager)
                            );
            } else {
                servertype = "TThreadedServer";
                if(m_log){
                    m_log->Warn_f("%s:没有正确得配置服务类型,默认使用%s",__FUNCTION__,qPrintable(servertype));
                }
                m_server = std::shared_ptr<TThreadedServer>(
                            new  TThreadedServer(
                                    stdcxx::make_shared<ProcessorFactory>(iffactory),
                                    stdcxx::make_shared<TServerSocket>(port),
                                    stdcxx::make_shared<TFramedTransportFactory>(),
                                    stdcxx::make_shared<TBinaryProtocolFactory>())
                            );
            }

            m_serverthread = boost::thread([=](){
                if(m_log){
                    m_log->Info_f("Thrift服务启动(%s:TFramedTransport),监听端口%d,线程个数%d",qPrintable(servertype),port,workerCount);
                }
                try {
                    m_server->serve();
                } catch (std::exception& e) {
                    if(m_log){
                        m_log->Error_f(":Thrift服务异常:%s",e.what());
                    }
                    exit(-1);
                }
                if(m_log){
                    m_log->Info_f("Thrift服务退出");
                }
            });

            return true;
        } else {
            if(m_log){
                m_log->Info_f("%s:thrift端口为(%d)小于0,不启用thrift服务",__FUNCTION__,port);
            }
            return false;
        }
    }

    void stopServer(){
        if(m_threadmanager){
            m_threadmanager->stop();
        }
        if(m_server){
            m_server->stop();
        }
        if(m_serverthread.joinable()){
            m_serverthread.join();
        }
    }

protected:
    int getThreadCount(QSettings& setting,const QString& servertype){
        int workerCount = setting.value(QString("%1/process_thread_cnt").arg(servertype),"10").toInt();
        if(workerCount < 1){
            workerCount = 4;
        }
        return workerCount;
    }

protected:
    stdcxx::shared_ptr<ThreadManager>   m_threadmanager;
    std::shared_ptr<TServer>            m_server;
    boost::thread                       m_serverthread;
    FDRCIL_LoggerPtr                    m_log;
};


}
