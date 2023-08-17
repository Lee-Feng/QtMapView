#pragma once

#include <QObject>
#include <interface/interfaceConfig.hpp>
#include <QVariantMap>
#include <QImage>
#include <QVideoFrame>

/**
 * @brief The Interface class
 * XXXX设备控制接口。
 *
 * 为了方便QML能直接使用，下面做如下约定：
 * 1）这里的接口都是异步接口(非阻塞接口)。
 * 2）因为QML的js端无法接收引用返回值，所以不能像C++定义一样使用多返回值。
 * 3）参数和返回值都是Qt自带类型。推荐多用QVarient,QVariantMap,QVariantList,或者自定义的QObject指针
 */
class LF_LASERDEVAPI_DECL Interface : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString version READ getVersionString)
    Q_PROPERTY(int versionValue READ getVersionCode)
    Q_PROPERTY(QVariantMap devInfo READ getDevInfo NOTIFY devInfoChanged)
    Q_PROPERTY(QVariantMap location READ getLocation NOTIFY locationChanged)
public:
    static void registerTypes();

    /**
     * @brief getInstance
     * @return
     */
    static Interface* getInstance();

public:
    Interface(QObject* parent = NULL);
    virtual ~Interface();

public slots:

    /**
     * @brief getVersionCode
     * 获取当前接口的版本信息。版本信息有3部分组成：major.minor.patch
     *  code = (major << 16) + (minor << 8) + patch
     * @return
     */
    int getVersionCode() const;

    /**
     * @brief getVersionString
     * 比如1.0.0
     * @return
     */
    QString getVersionString() const;

    /**
     * @brief exitSystem
     */
    void exitSystem();

public slots:
    /**
     * @brief getDeviceState
     * 获取设备的完整信息
     * @return
     */
    QVariantMap getDevInfo();

    /**
     * @brief backboneInterval
     * 杆号跳跃间隔（正数）
     * @return
     */
    int backboneInterval();
    void setBackboneInterval(int v);

    /**
     * @brief nextBackboneIsAcc
     * 下一杆是杆号增加（backboneInterval）还是减少（backboneInterval）
     * @return
     */
    bool nextBackboneIsAcc();
    void setNextBackboneIsAcc(bool acc);

    /**
     * @brief dropperCountInEachBackone
     * 杆之间的吊弦个数
     * @return
     */
    int dropperCountInEachBackone();
    void setDropperCountInEachBackone(int dropper_count);

    /**
     * @brief getLocation
     * 获取位置信息
     * @return
     */
    QVariantMap getLocation();

    /**
     * @brief setLocation
     * @param loc
     * @return
     */
    bool setLocation(QVariantMap loc);

    /**
     * @brief getParams
     * @return
     */
    QVariantMap getParams();
    bool setParams(QVariantMap params);

    /**
     * @brief nextBackbone
     * 下一杆号
     * @return
     */
    bool nextBackbone(bool asc);
    bool setBackbone(QString backbone);

    /**
     * @brief nextDropper
     * 下一吊弦
     * @return
     */
    bool nextDropper(bool asc);
    bool setDropper(QString dropper);

    /**
     * @brief getButteryValue
     * 获电量值
     * @return [0,100]
     */
    int getButteryValue();

signals:
    /// 有错误了
    /// 接口调用有错误也会通过该接口通知
    void error(QString error);
    /// 设备信息更新了
    void devInfoChanged();
    /// 位置更新了
    void locationChanged();
    /// 参数更新了
    void paramChanged();
private:
    class Impl;
    Impl *m_Impl;

};
