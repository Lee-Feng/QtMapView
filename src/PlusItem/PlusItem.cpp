#include "PlusItem.h"
#include <QDebug>
#include <QPainter>

PlusItem::PlusItem(QQuickItem *parent):QQuickPaintedItem(parent)
{
}

int PlusItem::size() const
{
    return m_size;
}

void PlusItem::setSize(const int &size)
{
    if(m_size != size){
        m_size = size;
        sizeChanged();
    }
}

QColor PlusItem::color() const
{
    return m_color;
}

void PlusItem::setColor(QColor c)
{
    if(m_color != c){
        m_color = c;
        colorChanged();
    }
}


void PlusItem::paint(QPainter *painter)
{
    painter->save();
    //int half = width()/2;
    //painter->translate(QPoint(half,half));
    QPen pen;
    pen.setColor(m_color);
    pen.setWidth(m_size);
    painter->setPen(pen);
    painter->drawLine(QPoint(0,height()/2),QPoint(width(),height()/2));
    painter->drawLine(QPoint(width()/2,0),QPoint(width()/2,height()));
    painter->restore();
}
