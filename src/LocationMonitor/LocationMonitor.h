#pragma once

#include <QObject>
#include <QImage>
#include <QGeoPositionInfoSource>

/**
 * @brief The ResManager class
 * 资源管理器，管理下载的资源。
 * 只是一个资源的管理与维护。
 *
 * 对于安卓需要添加权限：
 *
 *  允许一个程序访问精良位置
    <uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />
    允许一个程序访问CellID或WiFi热点来获取粗略的位置
    <uses-permission android:name="android.permission.ACCESS_COARSE_LOCATION" />
 */
class LocationMonitor : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief getManager
     * @return
     */
    static LocationMonitor* getManager();

public slots:
    /**
     * @brief setGpsEnabled
     * GPS是否开启
     * @param enable
     */
    void setGpsEnabled(bool enable);

    /**
     * @brief setBdsEnabled
     * 北斗是否开启
     * @param enable
     */
    void setBdsEnabled(bool enable);

signals:
    void gpsUpdated(bool isvalid,double latitude,double longtitude,double altitude);
    void bdsUpdated(bool isvalid,double latitude,double longtitude,double altitude);
protected slots:
    void onGpsGeoUpdate(QGeoPositionInfo gpi);
    void onGpsUpdateTimeout();
    void onGpsError(QGeoPositionInfoSource::Error err);
    void onGpsSupportedPositioningMethodsChanged();

    void onBdsGeoUpdate(QGeoPositionInfo gpi);
    void onBdsUpdateTimeout();
    void onBdsError(QGeoPositionInfoSource::Error err);
    void onBdsSupportedPositioningMethodsChanged();
private:
    LocationMonitor();
    ~LocationMonitor();
protected:
    class Impl;
    Impl* m_Impl;
};
