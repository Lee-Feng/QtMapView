#pragma once
#include <QObject>
#include <QString>

class BaseDataXlsParser : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool parsing READ isParsing NOTIFY parsingChanged)

public:
    BaseDataXlsParser(QObject* parent = NULL);
    ~BaseDataXlsParser();
public slots:

    /**
     * @brief addFile
     * 添加要解析的文件
     * @param filepath
     */
    void addFile(const QString& filepath);

    /**
     * @brief isParsing
     * 当前是否正在解析
     * @return
     */
    bool isParsing();

signals:
    /**
     * @brief parsingFile
     * 解析文件通知
     * @param filepath
     */
    void parsingFile(QString filepath);
    /**
     * @brief parsingFileFaild
     * 解析文件成功
     * @param filepath
     */
    void parsingFileFaild(QString filepath);
    void parsingFileSuccess(QString filepath);

    /**
     * @brief parsingChanged
     * 解析状态发生变化了
     */
    void parsingChanged();

protected:
    class Impl;
    Impl* m_Impl;
};
