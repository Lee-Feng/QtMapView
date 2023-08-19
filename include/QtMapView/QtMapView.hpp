#pragma once

#include <QObject>
#include <QVariantMap>
#include <QImage>
#include <QWidget>
#include <QtMapView/QtMapViewConfig.hpp>

/**
 * @brief The QtMapView class
 */
class LF_QMAPVIEW_DECL QtMapView : public QWidget
{
    Q_OBJECT

//    Q_PROPERTY(QString version READ getVersionString)
//    Q_PROPERTY(int versionValue READ getVersionCode)
//    Q_PROPERTY(QVariantMap devInfo READ getDevInfo NOTIFY devInfoChanged)
//    Q_PROPERTY(QVariantMap location READ getLocation NOTIFY locationChanged)

public:
    enum FillMode{
        PreserveAspectFit = 2,  /// [默认]保持比例，自动缩放内容，刚好全部看完内容，可能会有空白
        PreserveAspectCrop = 3, /// 保持比例，自动缩放内容，刚好全部铺满显示区域，可能会有裁减
    };
    Q_ENUMS(FillMode)

    class GeoRect{
    public:
        /// 纬度
        qreal minLat = 3;
        qreal maxLat = 54;

        /// 经度
        qreal minLon = 73;
        qreal maxLon = 136;

        GeoRect(){

        }
        GeoRect(qreal _minLat,qreal _maxLat,qreal _minLon,qreal _maxLon){
            minLat = _minLat;
            maxLat = _maxLat;
            minLon = _minLon;
            maxLon = _maxLon;
            normalize();
        }
        void normalize(){
            if(minLat > maxLat){
                qSwap(minLat,maxLat);
            }
            if(minLon > maxLon){
                qSwap(minLon,maxLon);
            }
        }

        /// x表示经度、与表示纬度
        QPointF center() const{
            return QPointF((maxLon + minLon)/2,(maxLat + minLat)/2);
        }

        qreal latSize() const{
            return qAbs(maxLat - minLat);
        }
        qreal lonSize() const{
            return qAbs(maxLon - minLon);
        }
    };

public:
    QtMapView(QWidget* parent = NULL);
    virtual ~QtMapView();
public:
    /**
     * @brief setMap
     * @param geoJsonFile
     */
    virtual void setMap(QString geoJsonFile);

    /**
     * @brief setGeoViewRect
     * 设置geo显示范围，x代表进经度，y代表纬度
     * @param r
     */
    virtual void setGeoViewRect(GeoRect r);

    /**
     * @brief geoViewRect
     * @return
     */
    virtual GeoRect geoViewRect();

    /**
     * @brief getFillMode
     * @return
     */
    FillMode getFillMode();

    /**
     * @brief setFillMode
     */
    void setFillMode(FillMode fill);

    /**
     * @brief setAlignment
     * 设置对齐方式
     * @param a
     */
    void setAlignment(Qt::Alignment a);

    /**
     * @brief getAlignment
     * 获取对齐方式
     * @return
     */
    Qt::Alignment getAlignment();

public:
    /**
     * @brief getFitScale
     * 获取当前自适应的缩放值，如果当前自适应功能关闭，那么缩放值便为1.0
     * @return
     */
    virtual qreal getFitScale();

    /**
     * @brief setScale
     * 设置手动缩放
     * @param scale
     */
    void setScale(qreal scale);

    /**
     * @brief getScale
     * 获取手动缩放，默认1.0
     * @return
     */
    qreal getScale();

    /**
     * @brief setMinAbsScale
     * 设置最小绝对缩放，绝对缩放 = 手动缩放 * 自适应缩放
     * @param scale 小于0代表不限制
     */
    void setMinAbsScale(qreal scale);

    /**
     * @brief setMaxAbsScale
     * 设置最大绝对缩放，绝对缩放 = 手动缩放 * 自适应缩放
     * @param scale 小于0代表不限制
     */
    void setMaxAbsScale(qreal scale);

    /**
     * @brief getMinAbsScale
     * @return 小于0代表不限制
     */
    qreal getMinAbsScale();

    /**
     * @brief getMaxAbsScale
     * @return 小于0代表不限制
     */
    qreal getMaxAbsScale();

    /**
     * @brief getAbsScale
     * 获取绝对缩放值
     * @return
     */
    qreal getAbsScale();

    /**
     * @brief getAbsTransMove
     * @return
     */
    QPointF getAbsTransMove();

    /**
     * @brief getAlignShift
     * 获取自动对齐的修正位置
     * @return
     */
    QPointF getAlignShift();

    /**
     * @brief getAbsShift
     * @return
     */
    QPointF getAbsShift();

    /**
     * @brief setFitScaleEnabled
     * 启动或者关闭自适应。
     * 自适应用于将geoViewRect自适应铺满到当前界面可视范围
     * @param enable
     */
    virtual void setFitScaleEnabled(bool enable);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
signals:
private:
    class Impl;
    Impl *m_Impl;

};
