#pragma once;
#include <QDateTime>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>
#include <QMap>
#include <QWidgetList>
#include <License/LicenseConfig.hpp>

class License;
namespace license {

static inline QString convert(const QDateTime& from)
{
    return from.toString("yyyy-MM-dd HH:mm:ss,zzz");
}
static inline QDateTime convert(const QString& from)
{
    return QDateTime::fromString(from,"yyyy-MM-dd HH:mm:ss,zzz");
}
static inline QDateTime convert(const std::string& from)
{
    return license::convert(QString::fromStdString(from));
}

class LF_LICENSE_DECL ProgramRunTimeInfo{
public:
    /// 程序的第一次启动时间
    QDateTime program_first_activate_time;
    /// 程序本次启动的时间
    QDateTime program_this_run_time;
    /// 程序已经启动了的次数
    int program_activate_count;
};

/**
 * @brief The RequestInfo class
 * 请求信息，也就是机器，应用的相关信息
 */
class LF_LICENSE_DECL RequestInfo{
public:
    /// 是否限制主机
    bool target_machine_limit;
    /// 目标的机器人hash值，也就是要运行程序的主机信息
    QString target_machine_hash_code;
    QDateTime request_date;
    QString version;
    /// 是否限制指定的应用，如果为true
    /// program与program_hash有效。
    bool program_path_limit;
    QString program;
    bool program_content_limit;
    QString program_hash;
    RequestInfo(){
        program_content_limit = false;
        target_machine_limit = true;
        program_path_limit = false;
    }
};

class IAuthItem;
typedef std::shared_ptr<IAuthItem> IAuthItemPtr;
typedef std::shared_ptr<const IAuthItem> IConstAuthItemPtr;
class IAuthItem{
public:
    virtual ~IAuthItem(){}
    /**
     * @brief New
     * 构造一个自己的实例空对象
     * @return
     */
    virtual IAuthItemPtr New() const = 0;

    /**
     * @brief getType
     * 获取授权类型，每一种授权对应的类型不一样.
     * 类型通常是硬编码到代码中的
     * @return
     */
    virtual QString getType() const = 0;

    /**
     * @brief toJson
     * 序列化到json,toJson得到的数据应该能被fromJson成功解析
     * @return
     */
    virtual QJsonObject toJson() const = 0;

    /**
     * @brief fromJson
     * 从json中解析数据
     * @param auth_data
     * @param faild_reason
     * @return
     */
    virtual bool fromJson(QJsonObject auth_data,QString& faild_reason) = 0;

    /**
     * @brief isValid
     * 检测是否授权通过。根据当前的运行信息，以及内部的授权逻辑计算是否能授权通过。
     * @param lic           当前的检测license对象
     * @param faild_reason
     * @return
     */
    virtual bool isValid(const License* lic,QString& faild_reason)  = 0;

    /**
     * @brief getConfigUI
     * 获取当前授权的配置界面，如果不提供就返回NULL
     * @return
     */
    virtual QWidget* getConfigUI() = 0;
};

/**
 * @brief The AuthData class
 */
class LF_LICENSE_DECL AuthData{
public:
    AuthData(const License* lic);
    virtual ~AuthData();
    /**
     * @brief toJsonArray
     * 序列化到json
     * @return
     */
    virtual QJsonArray toJsonArray() const;

    /**
     * @brief fromJsonArray
     * 解析回来
     * @param supportedlist
     *     系统支持的授权列表
     * @param auth_data
     * @param faild_reason
     * @return
     */
    virtual bool fromJsonArray(QJsonArray auth_data,QString& faild_reason);

    /**
     * @brief isValid
     * 当前授权数据是否允许程序运行
     * @param info
     * @param faild_reason
     * @return
     */
    virtual bool isValid(QString& faild_reason) const;

    /**
     * @brief addIAuthItem
     * 添加一个授权信息，如果之前存在同类型的将覆盖
     * @param item 内部不会保存该item。
     *      如果item的内容修改了需要再次调用addIAuthItem才会更新到AuthData里面。
     */
    virtual bool addIAuthItem(const IAuthItem* item,QString& error);

    /**
     * @brief contains
     * @param type
     * @return
     */
    virtual bool contains(const QString& type) const;

    /**
     * @brief getAuthItems
     * 获取当前的所有授权信息
     * @return
     */
    virtual QList<license::IConstAuthItemPtr> getAuthItems() const;

private:
    AuthData(AuthData&);
    AuthData& operator =(AuthData&);
    class Impl;
    Impl* m_impl;
};

/**
 * @brief The LicenseFile class
 * 授权文件内容
 */
class LF_LICENSE_DECL LicenseFile{
public:
    RequestInfo req;
    /// 授权的内容信息
    std::shared_ptr<AuthData> data;
};

}
