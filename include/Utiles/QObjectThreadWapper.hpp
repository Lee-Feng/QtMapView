#pragma once

#include <QString>
#include <QObject>
#include <QThread>
#include <functional>
#include <QCoreApplication>
#include <QDebug>

namespace utiles {

/**
 * @brief The ThreadWrapper class
 * 用于把一个对象包裹到Qt线程中。使对象整体都在指定的线程中进行事件循环。

 * 注意：该头文件一定要加到项目中才会被qmake解析。
 */
class QObjectThreadWapper : public QObject{
    Q_OBJECT

public:
    typedef std::function<void()> QueueBlockFunc;
public:

    /**
     * @brief QObjectThreadWapper
     * 构造一个对象线程包裹器。线程是内部自己创建的线程
     */
    QObjectThreadWapper(){
        m_myCreatedThread = new QThread();
        moveToThread(m_myCreatedThread);
        m_myCreatedThread->start();
        m_mythread = m_myCreatedThread;
    }

    /**
     * @brief QObjectThreadWapper
     * 构造一个对象线程包裹器。线程是外部传入的线程。
     */
    QObjectThreadWapper(QThread* outterThread){
        if(outterThread != NULL){
            moveToThread(outterThread);
            outterThread->start();
            m_mythread = outterThread;
        }
    }
    ~QObjectThreadWapper(){
        if(m_myCreatedThread != NULL){
            m_myCreatedThread->quit();
            m_myCreatedThread->wait();
            m_myCreatedThread->deleteLater();
        }
    }

    /**
     * @brief invoke
     * 在ThreadWrapper线程中执行run函数
     *
     * @param run 执行函数，该函数在新的QThread中执行
     */
    void invoke(QueueBlockFunc run){
        if(qApp == NULL){
            qWarning() << QStringLiteral("当前Qt已经退出或者Qt环境还没建立，无法隔离调用");
            return;
        }
        if(QThread::currentThread() == m_mythread){
            run();
        } else {
            QMetaObject::invokeMethod(this, "onRun", Qt::BlockingQueuedConnection,
                                      Q_ARG(QueueBlockFunc, run));
        }
    }

private slots:
    /// 在ThreadWrapper线程中执行了
    void onRun(QueueBlockFunc run){
        Q_ASSERT(QThread::currentThread() == m_mythread);
        run();
    }
protected:
    /// 当前创建的的线程，如果不需要内部创建就是NULL
    QThread* m_myCreatedThread = NULL;
    /// 当前所属线程
    QThread* m_mythread = NULL;
};

}
