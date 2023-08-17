#pragma once

#include <QObject>
#include <QString>
#include <BaseDataManager/BaseDataDef.h>

class BaseDataManager : public QObject
{
    Q_OBJECT
public:
    static BaseDataManager* getManager();

public slots:
    /**
     * @brief createLine
     * 创建一个线路数据
     * @param lineName          线路名字，比如徐盐线
     * @param tableNameForUp    上行方向的基础数据表，比如xuyansx
     * @param tableNameForDown  下行方向的基础数据表，比如xuyanxx
     * @return 创建成功返回true
     */
    bool createLine(const QString& lineName,const QString& tableNameForUp,const QString& tableNameForDown);

    /**
     * @brief containsLine
     * 是否存在线路的基础数据
     * @param lineName
     * @return
     */
    bool containsLine(const QString& lineName);

    /**
     * @brief deleteLineData
     * 删除线路的上下行数据
     * @param lineName
     * @return
     */
    bool deleteLineData(const QString& lineName);

    /**
     * @brief deleteLineUpData
     * 删除线路上行数据
     * @param lineName
     * @return
     */
    bool deleteLineUpData(const QString& lineName);

    /**
     * @brief deleteLineDownData
     * 删除线路下行数据
     * @param lineName
     * @return
     */
    bool deleteLineDownData(const QString& lineName);

    /**
     * @brief appendLineUpData
     * 添加基础数据到线路的上行数据中
     * @param table
     * @param data
     * @return
     */
    bool appendLineUpData(const QString& lineName,const LineBaseDataItem& data);

    /**
     * @brief appendLineUpData
     * 添加基础数据到线路的下行数据中
     * @param table
     * @param data
     * @return
     */
    bool appendLineDownData(const QString& lineName,const LineBaseDataItem& data);

public slots:
    /**
     * @brief getLines
     * 获取可用的线路
     * @param lines
     */
    bool getLines(QStringList& lines);

    /**
     * @brief getDirs
     * 获取支持的方向
     * @param dirs
     */
    bool getDirs(QStringList& dirs);

    /**
     * @brief getDataTableName
     * 获取线路的数据表
     * @param line      线路
     * @param dir       方向，表明映射
     */
    bool getDataTableName(const QString& line,QMap<QString,QString>& dirTable);

    /**
     * @brief getStations
     * 获取线路指定方向的战区列表
     * @param line
     * @param dir   比如上行，下行
     * @param stations   得到的站区列表
     */
    bool getStations(const QString& line,const QString& dir,QStringList& stations);

    /**
     * @brief getStations
     * 获取线路指定方向的战区列表
     * @param line
     * @param dir       比如上行，下行
     * @param stations  比如上行，下行
     */
    bool getBackbones(const QString& line,const QString& dir,const QString& station,QStringList& backbones);

private:
    BaseDataManager(QObject* parent = NULL);
    ~BaseDataManager();
    class Impl;
    Impl* m_Impl;
};
