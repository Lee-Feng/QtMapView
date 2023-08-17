#pragma once
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QList>
#include <QMutex>
#include <QDebug>
#include <iostream>
#include <QCoreApplication>

namespace logger {
class Logger;
QMutex g_log_mutex;
static QList<Logger*> g_logs;
class Logger {
public:
    Logger(QString name){
        name = name.trimmed();
        if(name.isEmpty()){
            name = "root";
        }
        m_name = name;
    }

    ~Logger(){
        qInstallMessageHandler(NULL);
        g_logs.removeOne(this);
        m_ss.setDevice(NULL);
    }

    void start(const QString& rootdir = "log"){
        if(m_ss.device() != NULL){
            qWarning() << QStringLiteral("已经有一个日志系统了");
            return;
        }

        QDir d;
        QString path = QString("%1/%2").arg(rootdir).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
        if(!d.mkpath(path)){
            qWarning() << QStringLiteral("无法创建目录: %1").arg(path);
            path = QCoreApplication::applicationDirPath();
        }
        m_file.setFileName(QString("%1/log_%2_%3.txt").arg(path).arg(QDateTime::currentDateTime().toString("HHmmsszzz")).arg(m_name));
        if(!m_file.open(QIODevice::WriteOnly)){
            qWarning() << QStringLiteral("打开文件失败,%1").arg(m_file.errorString());
        }
        m_ss.setDevice(&m_file);
        QString logfile = m_file.fileName();
        qApp->setProperty("current_log_file",logfile);
        qInstallMessageHandler(&Logger::myMessageOutput);
        QMutexLocker l(&g_log_mutex);
        g_logs.append(this);
    }

    QTextStream& getoutStream(){
        return m_ss;
    }

protected:
    QFile m_file;
    QTextStream m_ss;
    QString m_name;

private:
    static void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        QString function = QString(context.function);
        function = function.mid(0,function.indexOf("(")).trimmed();
        function = function.split(" ").last();
        QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss,zzz");
        QString trimed_msg = msg;
        if(trimed_msg.startsWith("\"") && trimed_msg.endsWith("\"")){
            trimed_msg = trimed_msg.mid(1,trimed_msg.length()-2);
        }
        QString text;
        QString msg_type;
        switch (type) {
        case QtDebugMsg:
            msg_type = "Debug";
            break;
        case QtInfoMsg:
            msg_type = "Info";
            break;
        case QtWarningMsg:
            msg_type = "Warn";
            break;
        case QtCriticalMsg:
            msg_type = "Cirtical";
            break;
        case QtFatalMsg:
            msg_type = "Fatal";
        }
        text = QString("[%1] [%2] %3:%4\n").arg(date).arg(msg_type).arg(function).arg(trimed_msg);
        QMutexLocker l(&g_log_mutex);
        foreach(Logger* l,g_logs){
            l->getoutStream() << text;
            l->getoutStream().flush();
        }
        FILE* out = stdout;
        fprintf(out,qPrintable(text));
        fflush(out);
//        if(type == QtDebugMsg || type == QtInfoMsg){
//            std::cout << qPrintable(text);
//        } else {
//            std::cerr << qPrintable(text);
//        }
    }
};
}
