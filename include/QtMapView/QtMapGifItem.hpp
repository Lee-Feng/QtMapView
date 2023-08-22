#pragma once

#include <QtMapView/QtMapItem.hpp>
#include <QMovie>

/**
 * @brief The QtMapItems class
 *地图中的元素，parent就是QtMapView
 */
class QtMapGifItem : public QtMapItem
{
public:
    QtMapGifItem(QString file,QString name,QSize s){
        QLabel* lable = new QLabel();
        QMovie* m = new QMovie(file);
        m->start();
        lable->setMovie(m);
        lable->setGeometry(0,0,s.width(),s.height());
        lable->setScaledContents(true);
        setName(name);
        setView(lable);
    }
    virtual ~QtMapGifItem(){
    }
};
