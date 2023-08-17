#include <BaseDataXlsParser/BaseDataXlsParser.h>
#include <BasicExcel/BasicExcel.hpp>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QLoggingCategory>

using namespace YExcel;

Q_LOGGING_CATEGORY(mylog, "BaseDataXlsParser")
class BaseDataXlsParser::Impl : public QThread{
public:
    Impl(BaseDataXlsParser* pthis){
        m_parsing = false;
        m_pthis = pthis;
    }
    ~Impl(){
        requestInterruption();
        clearTask();
        wait();
    }
protected:
    void run(){
        qCDebug(mylog) << QStringLiteral("启动基础数据xls文件解析线程...");
        while(!isInterruptionRequested()){
            QString filePath;
            if(!waitAndPopTask(filePath)){
                continue;
            }
            setParsing(true);
            qCDebug(mylog) << QStringLiteral("解析文件(%1)...").arg(filePath);
            if(!parseFile(filePath)){
                qCWarning(mylog) << QStringLiteral("解析文件失败(%1)!").arg(filePath);
            } else {
                qCDebug(mylog) << QStringLiteral("解析文件完成(%1)!").arg(filePath);
            }
            if(!haveTask()){
                setParsing(false);
            }
        }
        setParsing(false);
        qCDebug(mylog) << QStringLiteral("退出基础数据xls文件解析线程");
    }

    /**
     * @brief getGetExcelSheetName
     * 获取工作表的名字
     * @param excel
     * @param index
     * @return
     */
    QString getExcelSheetName(BasicExcel& excel,size_t index){
        if(index >= excel.GetTotalWorkSheets()){
            return "";
        }
        char* ansi = excel.GetAnsiSheetName(index);
        if(ansi != NULL){
            return QString::fromLocal8Bit(ansi);
        }
        wchar_t* unicode = excel.GetUnicodeSheetName(index);
        if(unicode != NULL){
            return QString::fromWCharArray(unicode);
        }
        return "";
    }

    bool parseFile(const QString& file){

        BasicExcel e;
        if(!e.Load(qPrintable(file))){
            qCWarning(mylog) << QStringLiteral("加载文件失败,file=%1").arg(file);
            return false;
        }

        int sheetcnt = e.GetTotalWorkSheets();
        if (sheetcnt < 2){
            qCWarning(mylog) << QStringLiteral("文件的WorkSheet应该至少为2个,file=%1").arg(file);
            return false;
        }

        for(int i=0,cnt=sheetcnt; i<cnt; i++){
            QString name = getExcelSheetName(e,i);
            if(name.isEmpty()){
                qCWarning(mylog) << QStringLiteral("文件的第%1页的名字是空,file=%2").arg(i).arg(file);
                continue;
            }
            qDebug() << QStringLiteral("解析文件第%1页(%2),file=%3").arg(i).arg(name).arg(file);
        }
        return true;
    }

    void testExport(){

    }

public:
    void addTask(QString file){
        QMutexLocker l(&m_mutex);
        files.append(file);
        start();
        m_filesChanged.notify_one();
    }

    /**
     * @brief waitAndPopTask
     * 获取一个文件，没有就等到通知为止
     * @param file
     * @return 如果有返回true，否则返回false
     */
    bool waitAndPopTask(QString& file){
        QMutexLocker l(&m_mutex);
        if(files.isEmpty()){
            m_filesChanged.wait(&m_mutex);
        }
        if(files.isEmpty()){
            return false;
        }
        file = files.takeLast();
        return true;
    }

    void clearTask(){
        QMutexLocker l(&m_mutex);
        files.clear();
        m_filesChanged.notify_one();
    }

    void setParsing(bool p){
        if(p != m_parsing){
            m_parsing = p;
            m_pthis->parsingChanged();
        }
    }

    bool haveTask(){
        QMutexLocker l(&m_mutex);
        return !files.isEmpty();
    }

private:
    QMutex m_mutex;
    QWaitCondition m_filesChanged;
    QStringList files;
    BaseDataXlsParser* m_pthis;
public:
    bool m_parsing;
};

BaseDataXlsParser::BaseDataXlsParser(QObject *parent):QObject(parent)
{
    m_Impl = new Impl(this);
}

BaseDataXlsParser::~BaseDataXlsParser()
{
    delete  m_Impl;
}

void BaseDataXlsParser::addFile(const QString &filepath)
{
    m_Impl->addTask(filepath);
}

bool BaseDataXlsParser::isParsing()
{
    return m_Impl->m_parsing;
}
