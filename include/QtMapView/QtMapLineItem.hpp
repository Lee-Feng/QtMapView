#pragma once

#include <QtMapView/QtMapItem.hpp>
#include <QMovie>
#include <QPaintEvent>
#include <QPainter>
#include <QtMapView/QtMapViewConfig.hpp>
#include <QElapsedTimer>

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
    /**
     * @brief genLinearGradient
     * 生成在指定百分出的颜色渐变
     * @param percent
     * @return
     */
    virtual QLinearGradient genLinearGradient(qreal percent);

    /**
     * @brief reset
     * 设置动画进度
     * @param percent
     */
    void reset(qreal percent);

    /**
     * @brief percent
     * @return
     */
    qreal percent();

    /**
     * @brief run
     * 开始动画
     * @param percent
     */
    void run(qreal percent);

    /**
     * @brief stop
     * 停止动画
     */
    void stop();
protected:
    virtual void paintEvent(QPaintEvent *event);
protected:
    void animationTick();
    /**
     * @brief genLinearGradient
     * @param backgroundStart   背景开始颜色
     * @param backgroundEnd     背景结束颜色
     * @param targetStart       目标起始颜色
     * @param targetEnd         目标结束颜色
     * @param targetPos         目标位置,[0,1]
     * @param targetLength      目标长度,[0,1]
     * @return
     */
    virtual QLinearGradient genLinearGradient(QColor backgroundStart,QColor backgroundEnd,QColor targetStart,QColor targetEnd, qreal targetPos, qreal targetLength);
protected:
    QPoint m_start;
    QPoint m_end;
    int m_width = 2;
    qreal m_percent = 0;
    QElapsedTimer m_time;
    bool m_running = false;
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
