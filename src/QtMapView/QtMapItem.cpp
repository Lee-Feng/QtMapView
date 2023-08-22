#include <QtMapView/QtMapItem.hpp>
#include <QtMapView/QtMapView.hpp>

QtMapItem::QtMapItem(QWidget *view):QObject(NULL)
{
    m_view = view;
    m_name = new QLabel(view);
    m_name->setAlignment(Qt::AlignCenter);
}

QtMapItem::QtMapItem():QObject(NULL)
{
    m_view = NULL;
    m_name = new QLabel(NULL);
    m_name->setAlignment(Qt::AlignCenter);
}

QtMapItem::~QtMapItem()
{
}

void QtMapItem::attached(QtMapView *map)
{
    setParent(map);
    setView(m_view);
}

void QtMapItem::setView(QWidget *item)
{
    m_view = item;
    m_name->setParent(item);
    QtMapView* mp = map();
    if(mp){
        m_view->setParent(mp);
    }
}

QtMapView *QtMapItem::map()
{
    return qobject_cast<QtMapView*>(parent());
}

QWidget *QtMapItem::item()
{
    return m_view;
}

void QtMapItem::setGeoPos(QPointF pos)
{
    m_geopos = pos;
    if(parent() != nullptr && m_view != nullptr){
        relayout();
    }
}

QPointF QtMapItem::getGeoPos()
{
    return m_geopos;
}

void QtMapItem::relayout()
{
    QtMapView* m = map();
    if(m != nullptr && m_view != nullptr){
        QPoint centerMove = QPoint(m_view->width()/2, m_view->height()/2);
        m_view->move(m->convertToViewPos(m_geopos).toPoint() - centerMove);
        m_name->setGeometry(0,m_view->height(),m_view->width(),20);
    }
}

void QtMapItem::setName(QString name)
{
    if(m_name->text() != name){
        m_name->setText(name);
        if(map()){
            relayout();
        }
    }
}

QString QtMapItem::name()
{
    return m_name->text();
}
