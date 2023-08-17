#include "LocationMonitor.h"
#include <QGeoPositionInfoSource>
#include <QDebug>

class LocationMonitor::Impl{
public:
    Impl(LocationMonitor* pthis){
    }

public:
    QGeoPositionInfoSource * gps = NULL;
    QGeoPositionInfoSource * bds = NULL;
};

LocationMonitor::LocationMonitor()
{
    m_Impl = new Impl(this);
    qDebug() << QStringLiteral("可用定位信息:") << QGeoPositionInfoSource::availableSources();
    m_Impl->gps = QGeoPositionInfoSource::createDefaultSource(0);
    if(m_Impl->gps){
        qDebug() << "QGeoPositionInfoSource for GSP created!";
        connect(m_Impl->gps,SIGNAL(positionUpdated(QGeoPositionInfo)),this,SLOT(onGpsGeoUpdate(QGeoPositionInfo)));
        connect(m_Impl->gps,SIGNAL(updateTimeout()),this,SLOT(onGpsUpdateTimeout()));
        connect(m_Impl->gps,SIGNAL(error(QGeoPositionInfoSource::Error)),this,SLOT(onGpsError(QGeoPositionInfoSource::Error)));
        connect(m_Impl->gps,SIGNAL(supportedPositioningMethodsChanged()),this,SLOT(onGpsSupportedPositioningMethodsChanged()));
    } else {
        qWarning() << "QGeoPositionInfoSource for GSP error!";
    }

    m_Impl->bds = QGeoPositionInfoSource::createDefaultSource(0);
    if(m_Impl->bds){
        qDebug() << "QGeoPositionInfoSource for BDS created!";
        connect(m_Impl->bds,SIGNAL(positionUpdated(QGeoPositionInfo)),this,SLOT(onBdsGeoUpdate(QGeoPositionInfo)));
        connect(m_Impl->gps,SIGNAL(updateTimeout()),this,SLOT(onBdsUpdateTimeout()));
        connect(m_Impl->gps,SIGNAL(error(QGeoPositionInfoSource::Error)),this,SLOT(onBdsError(QGeoPositionInfoSource::Error)));
        connect(m_Impl->gps,SIGNAL(supportedPositioningMethodsChanged()),this,SLOT(onBdsSupportedPositioningMethodsChanged()));
    } else {
        qWarning() << "QGeoPositionInfoSource for BDS error!";
    }
}

LocationMonitor::~LocationMonitor()
{
    setGpsEnabled(false);
    setBdsEnabled(false);
    delete m_Impl;
}

LocationMonitor *LocationMonitor::getManager()
{
    static LocationMonitor mgr;
    return &mgr;
}

void LocationMonitor::setGpsEnabled(bool enable)
{
    if(m_Impl->gps == NULL){
        qWarning() << QStringLiteral("无GPS可用，是否没有开启GPS或者没有权限");
        return;
    }
    if(enable){
        m_Impl->gps->setUpdateInterval(200);
        m_Impl->gps->startUpdates();
        qDebug() << QStringLiteral("GPS更新开启");
    } else {
        m_Impl->gps->stopUpdates();
        qDebug() << QStringLiteral("GPS更新关闭");
    }
}

void LocationMonitor::setBdsEnabled(bool enable)
{
    if(m_Impl->gps == NULL){
        qWarning() << QStringLiteral("无BDS可用，是否没有开启BDS或者没有权限");
        return;
    }

    if(enable){
        m_Impl->bds->setUpdateInterval(200);
        m_Impl->bds->startUpdates();
        qDebug() << QStringLiteral("BDS更新开启");
    } else {
        m_Impl->bds->stopUpdates();
        qDebug() << QStringLiteral("BDS更新关闭");
    }
}

void LocationMonitor::onGpsGeoUpdate(QGeoPositionInfo gpi)
{
//    qDebug() << "GPS:" << gpi.isValid() <<
//                gpi.coordinate().latitude() <<
//                gpi.coordinate().longitude() <<
//                gpi.coordinate().altitude();

    gpsUpdated(gpi.isValid(),gpi.coordinate().latitude(),gpi.coordinate().longitude(),gpi.coordinate().altitude());
}

void LocationMonitor::onGpsUpdateTimeout()
{
    qWarning() << QStringLiteral("GPS更新超时");
}

void LocationMonitor::onGpsError(QGeoPositionInfoSource::Error err)
{
    qWarning() << QStringLiteral("GPS更新出错:%1").arg(err);
}

void LocationMonitor::onGpsSupportedPositioningMethodsChanged()
{
    QGeoPositionInfoSource* s = qobject_cast<QGeoPositionInfoSource*>(sender());
    if(s){
        qDebug() << QStringLiteral("GPS supported positioning methods changed:%1").arg(s->supportedPositioningMethods());
    }
}

void LocationMonitor::onBdsGeoUpdate(QGeoPositionInfo gpi)
{
//    qDebug() << "BDS:" << gpi.isValid() <<
//                gpi.coordinate().latitude() <<
//                gpi.coordinate().longitude() <<
//                gpi.coordinate().altitude();

    bdsUpdated(gpi.isValid(),gpi.coordinate().latitude(),gpi.coordinate().longitude(),gpi.coordinate().altitude());
}

void LocationMonitor::onBdsUpdateTimeout()
{
    qWarning() << QStringLiteral("BDS更新超时");
}

void LocationMonitor::onBdsError(QGeoPositionInfoSource::Error err)
{
    qWarning() << QStringLiteral("BDS更新出错:%1").arg(err);
}

void LocationMonitor::onBdsSupportedPositioningMethodsChanged()
{
    QGeoPositionInfoSource* s = qobject_cast<QGeoPositionInfoSource*>(sender());
    if(s){
        qDebug() << QStringLiteral("BDS supported positioning methods changed:%1").arg(s->supportedPositioningMethods());
    }
}
