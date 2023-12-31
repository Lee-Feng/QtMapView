﻿#pragma once

#include <QtMapView/QtMapItem.hpp>
#include <QMovie>

/**
 * @brief The QtMapItems class
 *地图中的元素，parent就是QtMapView
 */
class QtMapGifItem : public QtMapItem
{
public:
    QtMapGifItem(QString file,QString name,QSize s = QSize()){
        QLabel* lable = new QLabel();
        QMovie* m = new QMovie(file);
        m->start();
        lable->setMovie(m);
        if(s.isValid()){
            lable->setGeometry(0,0,s.width(),s.height());
        } else {
            lable->setGeometry(0,0,m->frameRect().width(),m->frameRect().height());
        }
        lable->setScaledContents(true);
        setName(name,"color:white");
        setView(lable);
    }
    virtual ~QtMapGifItem(){
    }
};
