#include <interface/interface.hpp>
#include <QDebug>
#include <QtGlobal>
#include <functional>
#include <QTimer>
#include <Utiles/version.hpp>
#include <QUuid>
#include <QDateTime>
#include <QtConcurrent>
#include <QSettings>
#include <QDir>
#include <Utiles/SmartSplit.hpp>

#define LOCATION_NUM_COUNT 3
#define DEFAULT_SERVER_PORT 8990
const static QString SETTING_FILE   = "cfg/setting_writeable.ini";
const static QString MARK_FILE      = "cfg/setting_mark.ini";
const static QString LOCATION_FILE  = "cfg/setting_location.ini";


class Location{
public:
    QString lineName;
    QString lineDir;
    QString station;
    QString tunnel;
    QString backBone;
    QString dropper;

    bool operator == (const Location& m) const{
        return lineName == m.lineName &&
                lineDir == m.lineDir &&
                station == m.station &&
                tunnel == m.tunnel &&
                backBone == m.backBone &&
                dropper == m.dropper;
    }
};

static QVariantMap toMap(const Location &location)
{
    QVariantMap datas;
    datas["line_name"] = location.lineName;
    datas["line_dir"] = location.lineDir;
    datas["station"] = location.station;
    datas["tunnel"] = location.tunnel;
    datas["back_bone"] = location.backBone;
    datas["dropper"] = location.dropper;
    return datas;
}

static void fromMap(const QVariantMap &data, Location &loc,Location default_value = Location())
{
    loc.lineName = data.value("line_name",default_value.lineName).toString();
    loc.lineDir = data.value("line_dir",default_value.lineDir).toString();
    loc.station = data.value("station",default_value.station).toString();
    loc.tunnel = data.value("tunnel",default_value.tunnel).toString();
    loc.backBone = data.value("back_bone",default_value.backBone).toString();
    loc.dropper = data.value("dropper",default_value.dropper).toString();
    utiles::SmartSplit ss;
    loc.backBone = ss.formatPositiveNumPart(loc.backBone,LOCATION_NUM_COUNT);
    loc.dropper = ss.formatPositiveNumPart(loc.dropper,LOCATION_NUM_COUNT);
}

class Param{
public:
    /// 记录的地点
    Location m_location;
    int backbone_interval = 5;
    bool backbone_next_is_acc = true;
    int default_dropper_count = 6;
    Param(){

    }

    bool operator != (const Param& src) const{
        return !(*this == src);
    }
    bool operator == (const Param& src) const{
        return m_location == src.m_location &&
                backbone_interval == src.backbone_interval &&
                backbone_next_is_acc == src.backbone_next_is_acc &&
                default_dropper_count == src.default_dropper_count &&
                true;
    }

    QVariantMap toQVariantMap() const{
        QVariantMap ret;
        ret["location"] = toMap(m_location);
        ret["backbone_interval"] = backbone_interval;
        ret["backbone_next_is_acc"] = backbone_next_is_acc;
        ret["default_dropper_count"] = default_dropper_count;
        return ret;
    }
};

class DeviceInfo{
public:
    QString m_version;
    int m_version_code = 0;
    /// 剩余电量值[0,100]
    int m_battery_value = 0;
    /// 参数信息
    Param m_param;

    bool operator != (const DeviceInfo& src) const{
        return !(*this == src);
    }
    bool operator == (const DeviceInfo& src) const{
        return m_battery_value ==src.m_battery_value &&
                m_version_code == src.m_version_code &&
                m_version == src.m_version &&
                m_param == src.m_param &&
                true;
    }
    QVariantMap toQVariantMap() const{
        QVariantMap ret;
        ret["version"] = m_version;
        ret["version_code"] = m_version_code;
        ret["battery_value"] = m_battery_value;
        ret["params"] = m_param.toQVariantMap();
        return ret;
    }
};

class Interface::Impl{
public:
    QString m_last_error;
    /// 设备信息
    DeviceInfo m_devinfo;

private:
    Interface* m_pthis;
public:
    Impl(Interface* pthis):m_pthis(pthis){
        utiles::get_version(m_devinfo.m_version_code,m_devinfo.m_version);
        loadSetting();
    }
    ~Impl(){
    }

    void setLocation(Location loc){
        if(m_devinfo.m_param.m_location == loc){
            return;
        }
        m_devinfo.m_param.m_location = loc;
        utiles::SmartSplit ss;
        m_devinfo.m_param.m_location.dropper = ss.formatPositiveNumPart(
                    m_devinfo.m_param.m_location.dropper,LOCATION_NUM_COUNT);
        m_devinfo.m_param.m_location.backBone = ss.formatPositiveNumPart(
                    m_devinfo.m_param.m_location.backBone,LOCATION_NUM_COUNT);
        saveLocationPos();
        m_pthis->locationChanged();
        m_pthis->paramChanged();
        m_pthis->devInfoChanged();
    }

    void setParams(Param p){
        if(p == m_devinfo.m_param){
            return;
        }
        Param old = m_devinfo.m_param;
        m_devinfo.m_param = p;
        saveSetting();
        if(!(old.m_location == p.m_location)){
            m_pthis->locationChanged();
        }
        m_pthis->paramChanged();
        m_pthis->devInfoChanged();
    }

    void setLineCfg(int backbone_interval,bool backbone_next_is_acc,int default_dropper_count){
        if(m_devinfo.m_param.backbone_interval == backbone_interval &&
                m_devinfo.m_param.backbone_next_is_acc == backbone_next_is_acc &&
                m_devinfo.m_param.default_dropper_count == default_dropper_count){
            return;
        }
        m_devinfo.m_param.backbone_interval = backbone_interval;
        m_devinfo.m_param.backbone_next_is_acc = backbone_next_is_acc;
        m_devinfo.m_param.default_dropper_count = default_dropper_count;
        saveLocationPos();
        m_pthis->paramChanged();
        m_pthis->devInfoChanged();
    }

private:
    void loadSetting(){
        loadLocationPos();
        /// TODO：其他配置
    }
    void saveSetting(){
        saveLocationPos();
        /// TODO：其他配置
    }

    void loadLocationPos(){
        QSettings st_mark(LOCATION_FILE,QSettings::IniFormat);
        QVariantMap loc = st_mark.value("location/location").toMap();
        fromMap(loc,m_devinfo.m_param.m_location);
        m_devinfo.m_param.backbone_interval = st_mark.value("location/backbone_interval").toInt();
        m_devinfo.m_param.backbone_next_is_acc = (st_mark.value("location/backbone_next_is_acc").toInt() == 1);
        m_devinfo.m_param.default_dropper_count = st_mark.value("location/default_dropper_count").toInt();
    }
    void saveLocationPos(){
        QSettings st_mark(LOCATION_FILE,QSettings::IniFormat);
        st_mark.setValue("location/location",toMap(m_devinfo.m_param.m_location));
        st_mark.setValue("location/backbone_interval",m_devinfo.m_param.backbone_interval);
        st_mark.setValue("location/backbone_next_is_acc",m_devinfo.m_param.backbone_next_is_acc ? 1 : 0);
        st_mark.setValue("location/default_dropper_count",m_devinfo.m_param.default_dropper_count);
        st_mark.sync();
    }
};

void Interface::registerTypes()
{
}

Interface *Interface::getInstance()
{
    static Interface api;
    return &api;
}

Interface::Interface(QObject *parent):QObject(parent)
{
    m_Impl = new Impl(this);
}

Interface::~Interface()
{
    exitSystem();
    delete m_Impl;
}

int Interface::getVersionCode() const
{
    return m_Impl->m_devinfo.m_version_code;
}

QString Interface::getVersionString() const
{
    return m_Impl->m_devinfo.m_version;
}

void Interface::exitSystem()
{
    qDebug() << QStringLiteral("退出关闭设备...");
    ///TODO:关闭设备
}

int Interface::getButteryValue()
{
    return m_Impl->m_devinfo.m_battery_value;
}

QVariantMap Interface::getDevInfo()
{
    return m_Impl->m_devinfo.toQVariantMap();
}

int Interface::backboneInterval()
{
    return m_Impl->m_devinfo.m_param.backbone_interval;
}

void Interface::setBackboneInterval(int v)
{
    m_Impl->setLineCfg(v,
                       m_Impl->m_devinfo.m_param.backbone_next_is_acc,
                       m_Impl->m_devinfo.m_param.default_dropper_count);
}

bool Interface::nextBackboneIsAcc()
{
    return m_Impl->m_devinfo.m_param.backbone_next_is_acc;
}

void Interface::setNextBackboneIsAcc(bool acc)
{
    m_Impl->setLineCfg(m_Impl->m_devinfo.m_param.backbone_interval,
                       acc,
                       m_Impl->m_devinfo.m_param.default_dropper_count);
}

int Interface::dropperCountInEachBackone()
{
    return m_Impl->m_devinfo.m_param.default_dropper_count;
}

void Interface::setDropperCountInEachBackone(int dropper_count)
{
    m_Impl->setLineCfg(m_Impl->m_devinfo.m_param.backbone_interval,
                       m_Impl->m_devinfo.m_param.backbone_next_is_acc,
                       dropper_count);
}

QVariantMap Interface::getLocation()
{
    return toMap(m_Impl->m_devinfo.m_param.m_location);
}

bool Interface::setLocation(QVariantMap loc)
{
    Location new_loc;
    fromMap(loc,new_loc,m_Impl->m_devinfo.m_param.m_location);
    m_Impl->setLocation(new_loc);
    return true;
}

QVariantMap Interface::getParams()
{
    return m_Impl->m_devinfo.m_param.toQVariantMap();
}

bool Interface::setParams(QVariantMap params)
{
    Param old = m_Impl->m_devinfo.m_param;
    Param p;
    fromMap(params["location"].toMap(),p.m_location,m_Impl->m_devinfo.m_param.m_location);
    p.backbone_interval = params.value("backbone_interval",old.backbone_interval).toInt();
    p.backbone_next_is_acc = params.value("backbone_next_is_acc",old.backbone_next_is_acc).toBool();
    p.default_dropper_count = params.value("default_dropper_count",old.default_dropper_count).toInt();
    m_Impl->setParams(p);
    return true;
}

bool Interface::nextBackbone(bool asc)
{
    if(!nextBackboneIsAcc()){
        asc = !asc;
    }
    Location loc = m_Impl->m_devinfo.m_param.m_location;
    utiles::SmartSplit ss;
    int gap = qAbs(backboneInterval());
    gap *= asc ? 1 : -1;
    QString next = ss.incPositiveNumPart(loc.backBone,gap,LOCATION_NUM_COUNT);
    loc.backBone = next;
    loc.dropper = "000";
    m_Impl->setLocation(loc);
    return true;
}

bool Interface::setBackbone(QString backbone)
{
    Location loc = m_Impl->m_devinfo.m_param.m_location;
    loc.backBone = backbone;
    m_Impl->setLocation(loc);
    return true;
}

bool Interface::nextDropper(bool asc)
{
    if(!nextBackboneIsAcc()){
        asc = !asc;
    }

    Location loc = m_Impl->m_devinfo.m_param.m_location;
    utiles::SmartSplit ss;
    QString next = ss.incPositiveNumPart(loc.dropper,asc?1:-1,LOCATION_NUM_COUNT);
    loc.dropper = next;
    m_Impl->setLocation(loc);
    return true;
}

bool Interface::setDropper(QString dropper)
{
    Location loc = m_Impl->m_devinfo.m_param.m_location;
    loc.dropper = dropper;
    m_Impl->setLocation(loc);
    return true;
}
