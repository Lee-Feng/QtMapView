#pragma once

#include <QThread>
#include <functional>
#include <QDebug>
#include <QWaitCondition>
#include <QMutex>

namespace utiles {

typedef std::function<void()> StateFun;

/**
 * @brief The SimpleStateMachine class
 * 一个简单的状态机
 */
class SimpleStateMachine{
public:
    SimpleStateMachine(QString name = ""){
        m_thread = new MachineThread(this);
        m_exit_flg = false;
        m_name = name;
        m_userdata = NULL;
    }
    ~SimpleStateMachine(){
        stop(true);
        delete m_thread;
    }

    /**
     * @brief userData
     * 获取之前设置的用户数据
     * @return
     */
    void* userData(){
        return m_userdata;
    }

    /**
     * @brief running
     * @return
     */
    bool running(){
        return m_thread->isRunning();
    }

    /**
     * @brief setUserData
     * 设置用户数据
     * @param data
     */
    void setUserData(void* data){
        m_userdata = data;
    }

    /**
     * @brief setName
     * 设置名字
     * @param name
     */
    void setName(QString name){
        m_name = name;
    }

    /**
     * @brief start
     * 启动状态机
     */
    void start(){
        m_exit_flg = false;
        m_thread->start();
    }

    /**
     * @brief stop
     * 停止状态机
     */
    void stop(bool wait){
        QMutexLocker l(&m_mutex);
        m_quit.notify_all();
        if(m_thread->isRunning()){
            m_exit_flg = true;
            m_thread->quit();
            m_thread->requestInterruption();
            if(wait){
                l.unlock();
                m_thread->wait();
            }
        }
    }

    /**
     * @brief setNextState
     * 设置下一个状态
     * 例如：
     * m_sm.setNextState(std::bind(&XXX::Impl::State_xxxx,this));
     * @param fun
     */
    void setNextState(StateFun fun){
        QMutexLocker l(&m_mutex);
        m_statefun = fun;
        m_quit.notify_all();
    }


    /**
     * @brief sleep
     * 睡眠，但是可以被函数：setNextState，stop中断
     * @param ms
     */
    void sleep(int ms){
        QMutexLocker l(&m_mutex);
        m_quit.wait(&m_mutex,ms);
    }
private:
    void run(){
        qDebug() << QString("%1 running...").arg(m_name);
        while(!m_exit_flg){
            execute_state();
        }
        qDebug() << QString("%1 exit").arg(m_name);
    }

    void execute_state(){
        if(m_statefun){
            m_statefun();
        } else {
            QThread::msleep(10);
        }
    }
private:
    class MachineThread : public QThread{
    public:
        MachineThread(SimpleStateMachine* mat){
            m_mat = mat;
        }
    private:
        void run(){
            m_mat->run();
        }
    private:
        SimpleStateMachine* m_mat;
    };

    MachineThread*  m_thread;
    bool            m_exit_flg;
    QString         m_name;
    void*           m_userdata;
    StateFun        m_statefun;
    QMutex          m_mutex;
    QWaitCondition  m_quit;
};


}
