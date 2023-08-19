#include <QtMapView/QtMapView.hpp>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QtLocation/private/qgeojson_p.h>
#include <QGeoCircle>
#include <QGeoPath>
#include <QGeoPolygon>
#include <QMouseEvent>

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

    GeoRect getViewGeoRect() const{
        return m_geoRect;
    }

    QPointF geoPos2Point(QGeoCoordinate c) const{
        /// 上下颠倒一下
        return QPointF(c.longitude(),60-c.latitude());
    }

    void drawGeoPoint(QPainter& p,QGeoCircle c){
        p.drawEllipse(geoPos2Point(c.center()),c.radius(),c.radius());
    }
    void drawGeoPath(QPainter& p,QList<QGeoCoordinate> c){
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

    void drawGeoLine(QPainter& p,QGeoPath c){
        QList<QGeoCoordinate> points = c.path();
        drawGeoPath(p,points);
    }
    void drawGeoPolygon(QPainter& p,QGeoPolygon c){
        QList<QGeoCoordinate> points = c.path();
        drawGeoPath(p,points);
    }
    void drawGeoFeatureData(QPainter& p,int level,QVariantMap data){
        QString shape = data["type"].toString();
        if(shape.compare("Point") == 0){
            drawGeoPoint(p,data["data"].value<QGeoCircle>());
        } else if(shape.compare("LineString") == 0){
            drawGeoLine(p,data["data"].value<QGeoPath>());
        } else if(shape.compare("Polygon") == 0){
            drawGeoPolygon(p,data["data"].value<QGeoPolygon>());
        } else if(shape.startsWith("Multi")){
            QVariantList multShapes = data["data"].toList();
            foreach(auto s,multShapes){
                QVariantMap one = s.toMap();
                drawGeoFeatureData(p,level,one);
            }
        } else {
            qWarning() << QStringLiteral("不支持的geoJson图形类型[%1]").arg(shape);
        }
    }
    void drawGeoJsonFeature(QPainter& p,QVariantMap featureCollection){
        QVariantList datas = featureCollection["data"].toList();
        foreach(auto d,datas){
            drawGeoFeatureData(p,0,d.toMap());
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
};

QtMapView::QtMapView(QWidget *parent):QWidget(parent)
{
    m_Impl = new Impl();
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
    qDebug() << m_Impl->m_nodes;
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
    QPainter p(this);
    /// 背景
    p.save();
    p.setBrush(QBrush(Qt::black));
    p.drawRect(rect());
    p.restore();

    p.translate(getAbsShift());
    qreal absScale = getAbsScale();
    p.scale(absScale,absScale);

    /// 国、省线
    QPen pen = QPen(Qt::white);
    pen.setWidthF(1/absScale);
    p.setPen(pen);
    m_Impl->drawGeoJson(p,m_Impl->m_nodes);

    event->accept();
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
    if(m_Impl->m_mouseDragEnabled && m_Impl->m_draging){
        QPointF now = event->pos();
        m_Impl->m_shift += (now - m_Impl->m_mouseLastPos);
        m_Impl->m_mouseLastPos = now;
        update();
    }
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
