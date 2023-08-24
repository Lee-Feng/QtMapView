#pragma once

#include <QtMapView/QtMapItem.hpp>
#include <QMovie>

/**
 * @brief The QtMapImageItem class
 *地图中的元素，parent就是QtMapView
 */
class QtMapImageItem : public QtMapItem
{
public:
    QtMapImageItem(QString file,QString name,QSize s = QSize()){
        QLabel* lable = new QLabel();
        QPixmap img;
        img.load(file);
        lable->setPixmap(img);
        if(s.isValid()){
            lable->setGeometry(0,0,s.width(),s.height());
        } else {
            lable->setGeometry(0,0,img.width(),img.height());
        }
        lable->setScaledContents(true);
        setName(name,"color:white");
        setView(lable);
    }
    virtual ~QtMapImageItem(){
    }
};
