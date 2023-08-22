#pragma once

#include <QtMapView/QtMapItem.hpp>
#include <QMovie>
#include <QPaintEvent>
#include <QPainter>
#include <QtMapView/QtMapViewConfig.hpp>

/**
 * @brief The StarLine class
 * 绘制的线
 */
class LF_QMAPVIEW_DECL StarLine : public QWidget{
public:
    StarLine(QWidget* p = NULL);
    void setLine(QPoint start,QPoint end);
    void setWidth(int w);
    int getWidth();
protected:
    virtual void paintEvent(QPaintEvent *event);
protected:
    void animationTick();
protected:
    QPoint m_start;
    QPoint m_end;
    int m_width = 2;
};

/**
 * @brief The QtMapLineItem class
 *地图中的元素，parent就是QtMapView
 */
class LF_QMAPVIEW_DECL QtMapLineItem : public QtMapItem
{
public:
    QtMapLineItem();
    QtMapLineItem(QPointF s,QPointF e);
    virtual ~QtMapLineItem();
    void setLine(QPointF s,QPointF e);
    virtual void relayout() override;
protected:
    QPointF m_geoStart;
    QPointF m_geoEnd;
    StarLine* m_line = NULL;
};
