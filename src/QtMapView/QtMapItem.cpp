#include <QtMapView/QtMapItem.hpp>
#include <QtMapView/QtMapView.hpp>

QtMapItem::QtMapItem(QWidget *view):QObject(NULL)
{
    m_view = view;
}

QtMapItem::QtMapItem():QObject(NULL)
{
    m_view = nullptr;
}

QtMapItem::~QtMapItem()
{
    if(m_view != nullptr){
        delete m_view;
        m_view = nullptr;
    }
    if(m_name != nullptr){
        delete m_name;
        m_name = nullptr;
    }
}

void QtMapItem::attached(QtMapView *map)
{
    setParent(map);
    setView(m_view);
}

void QtMapItem::setView(QWidget *item)
{
    m_view = item;
    QtMapView* mp = map();
    if(mp){
        if(m_name != nullptr){
            m_name->setParent(mp);
        }
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
    if(m == nullptr){
        return;
    }
    if(m_view != nullptr){
        QPoint centerMove = QPoint(m_view->width()/2, m_view->height()/2);
        QPoint viewPos = QPoint(m->convertToViewPos(m_geopos).toPoint() - centerMove);
        m_view->move(viewPos);
        m_view->show();
    }
    if(m_name != nullptr){
        QPoint centerMove = QPoint(m_name->width()/2, m_name->height()/2);
        if(m_view != nullptr){
            centerMove += QPoint(0, m_view->height());
        }
        QPoint viewPos = QPoint(m->convertToViewPos(m_geopos).toPoint() - centerMove);
        m_name->move(viewPos);
        m_name->show();
    }
}

void QtMapItem::setName(QString name, QString qss)
{
    if(m_name == nullptr){
        m_name = new QLabel(map());
        m_name->setAlignment(Qt::AlignCenter);
    }
    if(m_name != nullptr && m_name->text() != name){
        m_name->setText(name);
        m_name->setStyleSheet(qss);
        if(map()){
            relayout();
        }
    }
}

QString QtMapItem::name()
{
    return m_name != nullptr ? m_name->text() : "";
}

QLabel *QtMapItem::getNameLable()
{
    return m_name;
}
