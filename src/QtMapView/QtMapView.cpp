#include <QtMapView/QtMapView.hpp>
#include <QtMapView/QtMapItem.hpp>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QtLocation/private/qgeojson_p.h>
#include <QGeoCircle>
#include <QGeoPath>
#include <QGeoPolygon>
#include <QMouseEvent>
#include <QList>
#include <QLabel>

static const qreal lant_move = 60;
class QtMapView::Impl{
public:
    QString m_file;
    QVariantList m_nodes;
    /// 经纬适应显示范围
    GeoRect m_geoRect = GeoRect(0,57,70,136);

    bool m_mouseDragEnabled = true;
    bool m_mouseScrollScaleEnabled = true;
    QPointF m_mouseLastPos;
    bool m_draging = false;
    QPointF m_mousCurrentPos;

    /// 元素信息
    QSet<QtMapItem*> m_viewnodes;
    QLabel* m_anchor_info;

public:
    Qt::Alignment m_align = Qt::AlignCenter;
    FillMode m_fillmode = PreserveAspectFit;

    /// 绘制的移动和缩放
    QPointF m_shift;
    qreal m_scale = 1.0; /// 操作缩放
    /// 是否开启适应
    bool m_adjust_scale_enable = true;

    /// 小于0代表不限制
    qreal m_max_abs_scale = -1;
    qreal m_min_abs_scale = -1;

    Impl(QtMapView* pthis):m_pthis(pthis){
        m_anchor_info = new QLabel(pthis);
        m_anchor_info->setStyleSheet("background-color:rgba(215,215,215,100); color:white");
        m_anchor_info->setAlignment(Qt::AlignCenter);
    }

    GeoRect getViewGeoRect() const{
        return m_geoRect;
    }

    QPointF geoTrasToShow(QPointF geo) const{
        //return QPointF(geo.x(),lant_move-geo.y());
        return m_pthis->convertToViewPos(geo);
    }

    QGeoCoordinate geoMove(const QGeoCoordinate& p,QPointF move){
        return QGeoCoordinate(p.latitude() + move.y(), p.longitude() + move.x());
    }

    void layoutNodePos(){
        foreach(auto k,m_viewnodes){
            if(k != NULL){
                k->relayout();
            }
        }
        QRect textBg = QRect(0,m_pthis->height()-26,220,20);
        QPointF geoInfo = m_pthis->convertToGeoPos(m_mousCurrentPos);
        QString posTxt = QString("%1,%2").arg(QString::number(geoInfo.x(),'f',6)).arg(QString::number(geoInfo.y(),'f',6));
        m_anchor_info->setText(posTxt);
        m_anchor_info->setGeometry(textBg);
        m_anchor_info->raise();
    }

    QPointF geoPos2Point(QGeoCoordinate c) const{
        return geoTrasToShow(QPointF(c.longitude(),c.latitude()));
    }
    void drawGeoPoint(QPainter& p,QGeoCircle c){
        p.drawEllipse(geoPos2Point(c.center()),c.radius(),c.radius());
    }
    void drawGeoPath(QPainter& p,QList<QGeoCoordinate> c,QVariantMap properties){
        if(c.size() < 2){
            qWarning() << QStringLiteral("路径中的点数小于2个");
            return;
        }
        QPainterPath path(geoPos2Point(c.takeFirst()));
        while (!c.isEmpty()) {
            path.lineTo(geoPos2Point(c.takeFirst()));
        }
        p.drawPath(path);
    }

    void drawGeoLine(QPainter& p,QGeoPath c,QVariantMap properties){
        QList<QGeoCoordinate> points = c.path();
        drawGeoPath(p,points,properties);
    }
    void drawGeoPolygon(QPainter& p,QGeoPolygon c,QVariantMap properties){
        QList<QGeoCoordinate> points = c.path();
        drawGeoPath(p,points,properties);
        QString name = properties["name"].toString();
        if(!name.isEmpty()){
            QGeoRectangle rect = c.boundingGeoRectangle();
            QGeoCoordinate tl = rect.topLeft();
            QGeoCoordinate dr = rect.bottomRight();
            if(name.startsWith(QStringLiteral("内蒙古"))){
                tl = geoMove(tl,QPointF(0,-3));
                dr = geoMove(dr,QPointF(0,-3));
            } else if(name.startsWith(QStringLiteral("甘肃"))){
                tl = geoMove(tl,QPointF(2,0));
                dr = geoMove(dr,QPointF(2,0));
            } else if(name.startsWith(QStringLiteral("陕西"))){
                tl = geoMove(tl,QPointF(2,0));
                dr = geoMove(dr,QPointF(2,0));
            } else if(name.startsWith(QStringLiteral("河北"))){
                tl = geoMove(tl,QPointF(-1,-1));
                dr = geoMove(dr,QPointF(-1,-1));
            } else if(name.startsWith(QStringLiteral("辽宁"))){
                tl = geoMove(tl,QPointF(1,1));
                dr = geoMove(dr,QPointF(1,1));
            } else if(name.startsWith(QStringLiteral("黑龙江"))){
                tl = geoMove(tl,QPointF(0,-1));
                dr = geoMove(dr,QPointF(0,-1));
            }
            QRect drawRect = QRect(geoPos2Point(tl).toPoint(),geoPos2Point(dr).toPoint());
            p.drawText(drawRect,Qt::AlignCenter, properties["name"].toString());
        }
    }
    void drawGeoFeatureData(QPainter& p,int level,QVariantMap data,QVariantMap properties){
        QString shape = data["type"].toString();
        if(shape.compare("Point") == 0){
            drawGeoPoint(p,data["data"].value<QGeoCircle>());
        } else if(shape.compare("LineString") == 0){
            drawGeoLine(p,data["data"].value<QGeoPath>(),properties.isEmpty() ? data["properties"].toMap() : properties);
        } else if(shape.compare("Polygon") == 0){
            drawGeoPolygon(p,data["data"].value<QGeoPolygon>(),properties.isEmpty() ? data["properties"].toMap() : properties);
        } else if(shape.startsWith("Multi")){
            QVariantList multShapes = data["data"].toList();
            QVariantMap pro = data["properties"].toMap();
            foreach(auto s,multShapes){
                QVariantMap one = s.toMap();
                drawGeoFeatureData(p,level,one,pro);
            }
        } else {
            qWarning() << QStringLiteral("不支持的geoJson图形类型[%1]").arg(shape);
        }
    }
    void drawGeoJsonFeature(QPainter& p,QVariantMap featureCollection){
        QVariantList datas = featureCollection["data"].toList();
        foreach(auto d,datas){
            drawGeoFeatureData(p,0,d.toMap(),QVariantMap());
        }
    }
    void drawGeoJson(QPainter& p,QVariantList geoJson){
        foreach(auto node,geoJson){
            QVariantMap attri = node.toMap();
            QString type = attri["type"].toString();
            if(type.compare("FeatureCollection",Qt::CaseInsensitive) == 0){
                drawGeoJsonFeature(p,attri);
            } else {
                qWarning() << QStringLiteral("json的数据类型不是FeatureCollection");
            }
        }
    }

    /**
     * @brief calcAdjustScale
     * 根据界面范围与需要显示的geo范围，计算出铺满的缩放值
     * @param viewSize
     * @param geoSize
     * @return
     */
    qreal calcAdjustScale(QSize viewSize,GeoRect geoSize) const{
        qreal widthscale = ((qreal)viewSize.width()) / ((qreal)geoSize.lonSize());
        qreal heightscale = ((qreal)viewSize.height()) / ((qreal)geoSize.latSize());
        return qMin(widthscale,heightscale);
    }

    /**
     * @brief calcAdjustToCenterFixShift
     * 根据界面范围与需要显示的geo范围，计算出铺满的缩放值
     * @param viewSize
     * @param geoSize
     * @return
     */
    void calcAdjustToCenterTrans(QSize viewSize,GeoRect geoSize,QPointF& move, qreal& scale)const{
        scale = calcAdjustScale(viewSize,geoSize);

        QPointF geoCenter = geoSize.center() / scale;
        QPointF viewCenter = QPointF(viewSize.width()/2,viewSize.height()/2);
        /// 中心点的位移就是需要回动的地方
        move = viewCenter - geoCenter;
    }

private:
    QtMapView* m_pthis;
};

QtMapView::QtMapView(QWidget *parent):QWidget(parent)
{
    m_Impl = new Impl(this);
    setMouseTracking(true);
}

QtMapView::~QtMapView()
{
    delete m_Impl;
}

void QtMapView::setMap(QString geoJsonFile)
{
    m_Impl->m_file = geoJsonFile;
    QFile f(m_Impl->m_file);
    if(!f.open(QIODevice::ReadOnly)){
        qWarning() <<QStringLiteral("打开文件失败,%1,[%2]").arg(f.errorString()).arg(geoJsonFile);
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    m_Impl->m_nodes = QGeoJson::importGeoJson(doc);
    update();
}

QtMapView::FillMode QtMapView::getFillMode()
{
    return m_Impl->m_fillmode;
}

void QtMapView::setFillMode(QtMapView::FillMode fill)
{
    m_Impl->m_fillmode = fill;
    update();
}

void QtMapView::setAlignment(Qt::Alignment a)
{
    m_Impl->m_align = a;
    update();
}

Qt::Alignment QtMapView::getAlignment()
{
    return m_Impl->m_align;
}

void QtMapView::setGeoViewRect(GeoRect r)
{
    r.normalize();
    m_Impl->m_geoRect = r;
    update();
}

QtMapView::GeoRect QtMapView::geoViewRect()
{
    return m_Impl->m_geoRect;
}

void QtMapView::setFitScaleEnabled(bool enable)
{
    m_Impl->m_adjust_scale_enable = enable;
    update();
}

QPointF QtMapView::convertToViewPos(QPointF geo)
{
    /// trans + (geo.x) * scale  = viewX
    /// trans + (lant_move - geo.y) * scale  = viewY
    ///
    ///
    qreal x = getAbsShift().x() + geo.x() * getAbsScale();
    qreal y = getAbsShift().y() + ( lant_move - geo.y() ) * getAbsScale();
    return QPointF(x,y);
}

QPointF QtMapView::convertToGeoPos(QPointF p)
{
    /// trans + (geo.x) * scale  = viewX
    /// trans + (lant_move - geo.y) * scale  = viewY
    ///
    qreal geox = (p.x() - getAbsShift().x()) / getAbsScale();
    qreal geoy = lant_move - (p.y() - getAbsShift().y()) / getAbsScale();
    return QPointF(geox,geoy);
}

void QtMapView::addNode(QtMapItem *node)
{
    if(contains(node)){
        qWarning() << QStringLiteral("设备的UI以存在，不再添加");
        return;
    }
    if(node == NULL){
        qWarning() << QStringLiteral("设备的UI是空，不再添加");
        return;
    }
    m_Impl->m_viewnodes.insert(node);
    node->attached(this);
    update();
}

void QtMapView::addNode(QtMapItem *node,QPointF geoPos)
{
    addNode(node);
    if(node){
        node->setGeoPos(geoPos);
    }
}

void QtMapView::removeNode(QtMapItem *nodeView)
{
    nodeView->deleteLater();
    m_Impl->m_viewnodes.remove(nodeView);
    update();
}

bool QtMapView::contains(QtMapItem *nodeView)
{
    return m_Impl->m_viewnodes.contains(nodeView);
}

QLabel *QtMapView::getAnchorLable()
{
    return m_Impl->m_anchor_info;
}

qreal QtMapView::getFitScale()
{
    qreal fit_scale = 1.0;
    if(!m_Impl->m_adjust_scale_enable){
        return fit_scale;
    }
    if(m_Impl->m_fillmode == PreserveAspectFit){
        qreal widthscale = ((qreal)width()) / ((qreal)m_Impl->m_geoRect.lonSize());
        qreal heightscale = ((qreal)height()) / ((qreal)m_Impl->m_geoRect.latSize());
        fit_scale = qMin(widthscale,heightscale);
    } else if(m_Impl->m_fillmode == PreserveAspectCrop){
        qreal widthscale = ((qreal)width()) / ((qreal)m_Impl->m_geoRect.lonSize());
        qreal heightscale = ((qreal)height()) / ((qreal)m_Impl->m_geoRect.latSize());
        fit_scale = qMax(widthscale,heightscale);
    }
    return fit_scale;
}

void QtMapView::setScale(qreal scale)
{
    m_Impl->m_scale = scale;
    update();
}

qreal QtMapView::getScale()
{
    return m_Impl->m_scale;
}

void QtMapView::setMinAbsScale(qreal scale)
{
    m_Impl->m_min_abs_scale = scale;
}

void QtMapView::setMaxAbsScale(qreal scale)
{
    m_Impl->m_max_abs_scale = scale;
}

qreal QtMapView::getMinAbsScale()
{
    return m_Impl->m_min_abs_scale;
}

qreal QtMapView::getMaxAbsScale()
{
    return m_Impl->m_max_abs_scale;
}

qreal QtMapView::getAbsScale()
{
    qreal scale = getFitScale() * m_Impl->m_scale;
    if(m_Impl->m_min_abs_scale > 0){
        scale = qMax(scale,m_Impl->m_min_abs_scale);
    }
    if(m_Impl->m_max_abs_scale > 0){
        scale = qMin(scale,m_Impl->m_max_abs_scale);
    }
    return scale;
}

QPointF QtMapView::getAbsTransMove()
{
    return m_Impl->m_shift + getAlignShift();
}

QPointF QtMapView::getAlignShift()
{
    QPointF ret;
    if(!m_Impl->m_adjust_scale_enable){
        return ret;
    }

    qreal viewWidth = width();
    qreal viewHeight = height();
    qreal scale = getAbsScale();
    qreal geoWidth = m_Impl->m_geoRect.latSize() * scale;
    qreal geoHeight = m_Impl->m_geoRect.lonSize() * scale;
    QPointF geoCenter = m_Impl->m_geoRect.center() * scale;
    QPointF viewCenter = QPointF(viewWidth/2,viewHeight/2);

    if((m_Impl->m_align & Qt::AlignHCenter) != 0){
        ret.setX(viewCenter.x() - geoCenter.x());
    }
    if((m_Impl->m_align & Qt::AlignRight) != 0){
        ret.setX(viewWidth - geoWidth);
    }
    if((m_Impl->m_align & Qt::AlignLeft) != 0){
        ret.setX(geoWidth - viewWidth);
    }

    if((m_Impl->m_align & Qt::AlignVCenter) != 0){
        ret.setY(viewCenter.y() - geoCenter.y());
    }
    if((m_Impl->m_align & Qt::AlignTop) != 0){
        ret.setY(geoHeight - viewHeight);
    }
    if((m_Impl->m_align & Qt::AlignBottom) != 0){
        ret.setY(viewHeight - geoHeight);
    }
    return ret;
}

QPointF QtMapView::getAbsShift()
{
    return m_Impl->m_shift + getAlignShift();
}

void QtMapView::paintEvent(QPaintEvent *event)
{
    event->accept();

    QPainter p(this);
    /// 背景
    p.save();
    p.setBrush(QBrush(Qt::black));
    p.drawRect(rect());
    p.restore();

    /// 地图
    p.save();
    /// 国、省线
    QPen pen = QPen(Qt::white);
    pen.setWidthF(1);
    p.setPen(pen);
    m_Impl->drawGeoJson(p,m_Impl->m_nodes);
    p.restore();

    m_Impl->layoutNodePos();

//    /// 光标位置
//    p.save();
//    QRect textBg = QRect(0,height()-26,220,20);
//    p.setBrush(QBrush(QColor(0xD7,0xD7,0xD7,100)));
//    p.drawRect(textBg);
//    p.setPen(QPen(Qt::white));
//    QPointF geoInfo = convertToGeoPos(m_Impl->m_mousCurrentPos);
//    QString posTxt = QString("%1,%2").arg(QString::number(geoInfo.x(),'f',6)).arg(QString::number(geoInfo.y(),'f',6));
//    p.drawText(textBg,  Qt::AlignCenter, posTxt);
//    p.restore();

}

void QtMapView::mousePressEvent(QMouseEvent *event)
{
    if(m_Impl->m_mouseDragEnabled){
        m_Impl->m_mouseLastPos = event->pos();
        m_Impl->m_draging = true;
    }
}

void QtMapView::mouseReleaseEvent(QMouseEvent *event)
{
    m_Impl->m_draging = false;
}

void QtMapView::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    m_Impl->m_mousCurrentPos = event->pos();
    if(m_Impl->m_mouseDragEnabled && m_Impl->m_draging){
        QPointF now = event->pos();
        m_Impl->m_shift += (now - m_Impl->m_mouseLastPos);
        m_Impl->m_mouseLastPos = now;
    }
    update();
}

void QtMapView::wheelEvent(QWheelEvent *event)
{
    if(m_Impl->m_mouseScrollScaleEnabled){
        QPoint numDegrees = event->angleDelta() / 8;
        if(numDegrees.y() > 0){
            m_Impl->m_scale *= 1.1;
        } else {
            m_Impl->m_scale *= 0.9;
        }
        update();
    }
}

void QtMapView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}
