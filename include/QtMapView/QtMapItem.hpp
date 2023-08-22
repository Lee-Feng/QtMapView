#pragma once

#include <QtMapView/QtMapViewConfig.hpp>
#include <QWidget>
#include <QLabel>

class QtMapView;
/**
 * @brief The QtMapItems class
 *地图中的元素，parent就是QtMapView
 */
class LF_QMAPVIEW_DECL QtMapItem : public QObject
{
    Q_OBJECT
public:
    QtMapItem(QWidget* view);
    QtMapItem();
    virtual ~QtMapItem();

    /**
     * @brief attached
     * 被添加进了地图
     * @param map
     */
    virtual void attached(QtMapView* map);

    /**
     * @brief setView
     * @param item
     */
    virtual void setView(QWidget* item);

    /**
     * @brief item
     * 可显示的item，其parent也是地图
     * @return
     */
    virtual QWidget* item();

    /**
     * @brief map
     * 所在的地图,等同于parent()
     * @return
     */
    virtual QtMapView* map();

    /**
     * @brief setGeoPos
     * 设置Item在地图中的位置
     * @param pos
     */
    virtual void setGeoPos(QPointF pos);

    /**
     * @brief getGeoPos
     * @return
     */
    virtual QPointF getGeoPos();

    /**
     * @brief relayout
     * 更新位置等操作，默认就更新到geo对应的view-pos位置
     */
    virtual void relayout();

    /**
     * @brief setName
     * 修改名字
     * @param name
     */
    virtual void setName(QString name);

    /**
     * @brief name
     * @return
     */
    virtual QString name();

    /**
     * @brief getNameLable
     * @return
     */
    QLabel* getNameLable();
protected:
    QWidget* m_view = NULL;
    QPointF m_geopos; /// x是经度、y是纬度
    QLabel* m_name = NULL;
};
