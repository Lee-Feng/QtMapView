#include <BaseDataManager/BaseDataManager.h>

BaseDataManager::BaseDataManager(QObject *parent):QObject(parent)
{
}

BaseDataManager::~BaseDataManager()
{
}

BaseDataManager *BaseDataManager::getManager()
{
    static BaseDataManager mgr;
    return &mgr;
}

bool BaseDataManager::createLine(const QString &lineName, const QString &tableNameForUp, const QString &tableNameForDown)
{
    return false;
}

bool BaseDataManager::containsLine(const QString &lineName)
{
    return false;
}

bool BaseDataManager::deleteLineData(const QString &lineName)
{
    return false;
}

bool BaseDataManager::deleteLineUpData(const QString &lineName)
{
    return false;
}

bool BaseDataManager::deleteLineDownData(const QString &lineName)
{
    return false;
}

bool BaseDataManager::appendLineUpData(const QString &lineName, const LineBaseDataItem &data)
{
    return false;
}

bool BaseDataManager::appendLineDownData(const QString &lineName, const LineBaseDataItem &data)
{
    return false;
}

bool BaseDataManager::getLines(QStringList &lines)
{
    return false;
}

bool BaseDataManager::getDirs(QStringList &dirs)
{
    return false;
}

bool BaseDataManager::getDataTableName(const QString &line, QMap<QString, QString> &dirTable)
{
    return false;
}

bool BaseDataManager::getStations(const QString &line, const QString &dir, QStringList &stations)
{
    return false;
}

bool BaseDataManager::getBackbones(const QString &line, const QString &dir, const QString &station, QStringList &backbones)
{
    return false;
}

