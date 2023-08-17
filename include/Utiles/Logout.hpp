/*******************************************************************************
* @file   .hpp
* @brief
*
* ...detail describe
*
* @version   V1.0.1
* @author    LiFeng,
* @date      2018-08-11
*
* @note
*******************************************************************************/

#ifndef __LOGOUT__HH__
#define __LOGOUT__HH__
#include <logger/logger_appender.hpp>
#include <logger/logger.hpp>
#include <logger/loggerlevel.hpp>
#include <QString>
#include <iostream>
#include <QObject>
#include <QDebug>

namespace utiles {

static const int FDR_LOG_TAG_INDEX = 26;

class IAutoRemoveAppender : public utiles::LoggerAppender{
public:
    ~IAutoRemoveAppender(){
        removeFromLogger();
    }
    void addToLogger(utiles_LoggerPtr logger = utiles_LoggerPtr()){
        removeFromLogger();
        if(!logger){
            logger = utiles::GetLogger();
        }
        m_logger = logger;
        logger->AddAppender(this);
    }

    void removeFromLogger(){
        if(m_logger){
            m_logger->RemoveAppender(this);
        }
    }
protected:
    utiles_LoggerPtr m_logger;
};

class Logout : public IAutoRemoveAppender
{
public:
    virtual void Append(const std::string& logtext)
    {
        std::cout << logtext;
    }
};
class TagLogout : public IAutoRemoveAppender
{
public:
    TagLogout(const std::string& tag)
    {
        m_tag = tag;
        m_tag_len = tag.length();
    }

    virtual void Append(const std::string& logtext)
    {
        const int TAG_INDEX = FDR_LOG_TAG_INDEX;

        if(logtext.length() > TAG_INDEX)
        {
            const char* tag_data = m_tag.data();
            const char* log_data = logtext.data();
            if(memcmp(tag_data,&log_data[TAG_INDEX],m_tag_len) == 0)
            {
                std::cout << logtext;
            }
        }
    }
private:
    int m_tag_len;
    std::string m_tag;
};

class LevelLogout : public IAutoRemoveAppender
{
public:
    /**
     * @brief LevelLogout
     * @param min_level 只输出大于等于该级别的日志
     */
    LevelLogout(utiles::LogLevel::ELogLevel min_level)
    {
        m_min_level = min_level;
    }

    utiles::LogLevel::ELogLevel getLevel(const std::string& logtext){
        utiles::LogLevel::ELogLevel ret = utiles::LogLevel::E_Unknow;
        ///日志：DEBUG，INFO，NOTICE,WARN，ERROR，FATAL
        const int TAG_INDEX = FDR_LOG_TAG_INDEX;
        if(logtext.length() > TAG_INDEX){
            if(logtext[TAG_INDEX] == 'D'){
                ret =  utiles::LogLevel::E_Debug;
            } else if(logtext[TAG_INDEX] == 'I') {
                ret =  utiles::LogLevel::E_Info;
            } else if(logtext[TAG_INDEX] == 'N') {
                ret =  utiles::LogLevel::E_Notice;
            } else if(logtext[TAG_INDEX] == 'W') {
                ret =  utiles::LogLevel::E_Warn;
            } else if(logtext[TAG_INDEX] == 'E') {
                ret =  utiles::LogLevel::E_Error;
            } else if(logtext[TAG_INDEX] == 'F') {
                ret =  utiles::LogLevel::E_Fatal;
            }
        }
        return ret;
    }

    virtual void Append(const std::string& logtext)
    {
        /// 日志级别定义值是级别越高，值越小
        if(getLevel(logtext) <= m_min_level){
            std::cout << logtext;
        }
    }
private:
    utiles::LogLevel::ELogLevel m_min_level;
    int m_tag_len;
    std::string m_tag;
};

/**
 * @brief The UIAppender class
 * 同于界面显示日志的appender
 *
 * 需要Object有如下几个槽函数
 *  void Debug(const QString &info);
 *  void Info(const QString &info);
 *  void Warn(const QString &war);
 *  void Error(const QString &err);
 */
class UIAppender : public IAutoRemoveAppender
{
public:
    UIAppender(QObject* obj):m_obj(obj){
    }
    ~UIAppender(){
    }

    virtual void Append(const std::string& logtext)
    {
        QString log = QString::fromLocal8Bit(logtext.c_str());
        const int TAG_INDEX = FDR_LOG_TAG_INDEX;
        QString function = "Debug";
        if(log.length() > TAG_INDEX){
            QString tag_str = log.mid(TAG_INDEX,10);
            if(tag_str.contains("ERROR"))
            {
                function = "Error";
            }
            else if(tag_str.contains("WARN"))
            {
                function = "Warn";
            }
            else if(tag_str.contains("INFO"))
            {
                function = "Info";
            }
        }
        QMetaObject::invokeMethod(m_obj, function.toStdString().c_str(), Qt::QueuedConnection,
                                  Q_ARG(QString, log));
    }

private:
    QObject* m_obj;
};

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
class QLoggerToFdrLoger
{
protected:
    static void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        static utiles_LoggerPtr logger  = utiles::GetNamedLogger("qt");
        utiles::CLogger::In("QDEBUG",logger);
        /// __thiscall utiles::QLoggerToFdrLoger::QLoggerToFdrLoger(void)
        /// int __cdecl main(int,char *[])
        QString function = QString(context.function);
        function = function.mid(0,function.indexOf("(")).trimmed();
        function = function.split(" ").last();
        switch (type) {
        case QtDebugMsg:
            logger->Debug_f("%s:%s",qPrintable(function),qPrintable(msg));
            break;
        case QtInfoMsg:
            logger->Info_f("%s:%s",qPrintable(function),qPrintable(msg));
            break;
        case QtWarningMsg:
            logger->Warn_f("%s:%s",qPrintable(function),qPrintable(msg));
            break;
        case QtCriticalMsg:
            logger->Error_f("%s:%s",qPrintable(function),qPrintable(msg));
            break;
        case QtFatalMsg:
            logger->Fatal_f("%s:%s",qPrintable(function),qPrintable(msg));
        }
    }
public:
    QLoggerToFdrLoger(){
        qInstallMessageHandler(QLoggerToFdrLoger::myMessageOutput);
        ///这只修改了默认MessageHandler的处理方式
        //qSetMessagePattern("%{function}:%{message}");
    }
    ~QLoggerToFdrLoger(){
        qInstallMessageHandler(0);
    }

    void makeCompilerHappy(){}
};
#endif
}

#endif
