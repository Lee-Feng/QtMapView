#pragma once

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
class ThreadSafeCaller : public QObject{
    Q_OBJECT

public:
    typedef std::function<void()> QueueBlockFunc;
public:

    /**
     * @brief ThreadSafeCaller
     * 构造一个对象线程包裹器。线程是内部自己创建的线程
     */
    ThreadSafeCaller(QObject* callee,QueueBlockFunc run){
        if(qApp == NULL){
            qWarning() << QStringLiteral("当前Qt已经退出或者Qt环境还没建立，无法隔离调用");
            return;
        }
        if(callee == NULL){
            return;
        }
        if(QThread::currentThread() == callee->thread()){
            run();
        } else {
            QMetaObject::invokeMethod(this, "onRun", Qt::BlockingQueuedConnection,
                                      Q_ARG(QueueBlockFunc, run));
        }
    }
};

}
