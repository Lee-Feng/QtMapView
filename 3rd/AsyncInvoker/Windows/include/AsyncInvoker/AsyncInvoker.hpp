/*******************************************************************************
* @file  AsyncInvoker.hpp
* @brief 异步调用器，主要用于方便实现界面异步调用底层接口
*
* ...detail describe
*
* @version   V1.0.0
* @author    LiFeng
* @date      2018-03-26
*
* @note
*******************************************************************************/
#pragma once

#include <AsyncInvoker/AsyncInvokerConfig.hpp>
#include <QString>
#include <memory>
#include <functional>
#include <QVariant>
#include <QTimer>

namespace ui {

class InvokeResult{
public:
   bool suc;            /// 执行是否成功
   QString errormesg;   /// 执行失败的错误信息
   int errorcode;       /// 错误代码
   QVariant data;       /// 执行成功之后的返回数据
   QMap<QString,QVariant> datas;    /// 执行成功之后的返回数据,批量数据

   InvokeResult(){
       suc = false;
       errorcode = 0;
   }
};

class LF_ASYNCINVK_DECL AsyncInvoker : public QObject
{
public:
    /**
     * @brief getInstance
     * 获取全局的调用器
     * @return
     */
    static AsyncInvoker* getInstance();
public:
    /**
     * @brief AsyncInvoker
     * 构造一个独立的调用器
     */
    AsyncInvoker(int thread_pool_size = 4);

    virtual ~AsyncInvoker();

    /**
     * @brief invoke
     * 调用接口callee，调用完成之后主线程调用update接口
     * @param callee
     * @param update   将在AsyncInvoker所在线程执行
     * @param pthis    是捕获的this指针。如果设置了指针，该对象如果释放了，那么callee与update接口都不会调用
     */
    void invoke(std::function<InvokeResult()> callee,std::function<void(const InvokeResult& result)> update,QObject* pthis = NULL);

    /**
     * @brief cancle
     * 取消自己发出的任务，如果任务正在执行，那么将等待执行完成
     * 用于caller析构时刻
     * @param caller  如果是null，将不进行移除
     */
    void cancle(QObject* caller);

    /**
     * @brief setLicensePassword
     * @param password 要设置的密码
     */
    void setLicensePassword(const QString& password);

private:
    void customEvent(QEvent *event);
private:
    AsyncInvoker(AsyncInvoker&);
    AsyncInvoker& operator =(AsyncInvoker&);
private:
    class Impl;
    Impl* m_Impl;
};


/**
 * @brief registerLicense
 * 该函数需要全局调用一次
 * @param password
 */
static inline void registerLicense(const QString& password){
    AsyncInvoker::getInstance()->setLicensePassword(password);
}

/**
 * @brief invoke
 * 常见用法：
 * 全局注册一次：registerLicense
 *
 * 然后调用函数：
 * ui::invoke([=](){ InvokeResult ret; return ret;},[=](const InvokeResult& result){},this);
 *
 * ui::invoke([=](){
 *      /// 这里是后台可以阻塞的函数调用
 *      InvokeResult ret;
 *      return ret;
 * },
 * [=](const InvokeResult& result){
 *      /// 这里是拿到结果的界面刷新
 *
 * },this);
 *
 *
 * @param callee
 * @param update
 * @param pthis 是捕获的this指针。如果设置了指针，该对象如果释放了，那么callee与update接口都不会调用
 */
static inline void invoke(std::function<InvokeResult()> callee,std::function<void(const InvokeResult& result)> update,QObject* pthis){
    AsyncInvoker::getInstance()->invoke(callee,update,pthis);
}

/**
 * @brief invokePeriod
 * 常见用法：
 * 全局注册一次：registerLicense
 *
 * 周期性的调用
 * @param callee
 * @param update
 * @param pthis
 */
static inline void invokePeriod(int period_ms,std::function<InvokeResult()> callee,std::function<void(const InvokeResult& result)> update,QObject* pthis){
    Q_ASSERT(pthis != NULL);
    QTimer* t = new QTimer(pthis);
    t->setInterval(period_ms);
    t->setSingleShot(false);
    t->start();
    QObject::connect(t,&QTimer::timeout,[=](){
        AsyncInvoker::getInstance()->invoke(callee,update,t);
    });
}

/**
 * @brief invokeCancle
 * 取消异步调用
 * @param pthis
 */
static inline void invokeCancle(QObject* pthis){
    AsyncInvoker::getInstance()->cancle(pthis);
}


}
